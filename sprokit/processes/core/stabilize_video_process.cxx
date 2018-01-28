/*ckwg +29
 * Copyright 2017 by Kitware, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  * Neither name of Kitware, Inc. nor the names of any contributors may be used
 *    to endorse or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS [yas] elisp error!AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "stabilize_video_process.h"

#include <vital/vital_types.h>
#include <vital/types/timestamp.h>
#include <vital/types//homography_f2f.h>
#include <vital/types/timestamp_config.h>
#include <vital/types/image_container.h>
#include <vital/algo/stabilize_video.h>
#include <vital/algo/warp_image.h>
#include <vital/util/wall_timer.h>
#include <arrows/ocv/image_container.h>

#include <kwiver_type_traits.h>

#include <sprokit/pipeline/process_exception.h>

namespace algo = kwiver::vital::algo;

namespace kwiver {

// ==================================================
// TBD A better approach would be to remove the warp algorithm reference
// in this process and do the warping in the pipeline.

create_config_trait( stabilize, std::string, "", "Stabilization algorithm "
                     "configuration subblock" );
create_config_trait( warp, std::string, "", "Warping algorithm configuration "
                     "subblock" );
create_config_trait( edge_buffer, int, "0", "Number of rows to crop from the "
                     "top and bottom as well as the number of columns to crop "
                     "from the left and right of the stabilized image to allow "
                     "for motion without black, unpopulated pixels. This "
                     "operation is defined prior to any rescaling." );
create_config_trait( rescale_factor, double, "1", "Sets the scaling of the "
                     "stabilized output image relative to the source image. A "
                     "value greater than 1 results in a higher-resolution "
                     "output while a value less than one reduces the output "
                     "image resolution.");

//----------------------------------------------------------------
// Private implementation class
class stabilize_video_process::priv
{
public:
  priv();
  ~priv();

  // Configuration values
  algo::stabilize_video_sptr m_stabilize;
  algo::warp_image_sptr m_warp;
  int m_edge_buffer = 0;
  double m_rescale_factor = 1;
  kwiver::vital::wall_timer m_timer;

}; // end priv class

// ================================================================

stabilize_video_process
::stabilize_video_process( kwiver::vital::config_block_sptr const& config )
  : process( config ),
    d( new stabilize_video_process::priv )
{
  attach_logger( kwiver::vital::get_logger( name() ) ); // could use a better approach

  make_ports();
  make_config();
}


stabilize_video_process
::~stabilize_video_process()
{
}


// ----------------------------------------------------------------
void stabilize_video_process
::_configure()
{
  kwiver::vital::config_block_sptr algo_config = get_config();
  
  d->m_edge_buffer          = config_value_using_trait( edge_buffer );
  d->m_rescale_factor       = config_value_using_trait( rescale_factor );

  // Check config so it will give run-time diagnostic of config problems
  if ( ! algo::stabilize_video::check_nested_algo_configuration( "stabilize", algo_config ) )
  {
    throw sprokit::invalid_configuration_exception( name(), "Configuration check failed." );
  }

  algo::stabilize_video::set_nested_algo_configuration( "stabilize", algo_config, d->m_stabilize );
  if ( ! d->m_stabilize )
  {
    throw sprokit::invalid_configuration_exception( name(), "Unable to create stabilization algorithm" );
  }

  // Check config so it will give run-time diagnostic of config problems
  if ( ! algo::warp_image::check_nested_algo_configuration( "warp", algo_config ) )
  {
    throw sprokit::invalid_configuration_exception( name(), "Configuration check failed." );
  }

  algo::warp_image::set_nested_algo_configuration( "warp", algo_config, d->m_warp );
  if ( ! d->m_warp )
  {
    throw sprokit::invalid_configuration_exception( name(), "Unable to create image warping algorithm" );
  }
}


// ----------------------------------------------------------------
void
stabilize_video_process
::_step()
{
  d->m_timer.start();
  kwiver::vital::homography_f2f_sptr src_to_ref_homography;

  kwiver::vital::timestamp frame_time;
  
  // Test to see if optional port is connected.
  if (has_input_port_edge_using_trait( timestamp ) )
  {
    frame_time = grab_input_using_trait( timestamp );
  }
  
  // image
  kwiver::vital::image_container_sptr in_image = grab_from_port_using_trait( image );

  // LOG_DEBUG - this is a good thing to have in all processes that handle frames.
  LOG_DEBUG( logger(), "Processing frame " << frame_time );

  // -- outputs --
  kwiver::vital::homography_f2f_sptr s2r_homog;
  bool new_ref;
  kwiver::vital::image_container_sptr stab_image;
  
  // create empty image of desired size
  int out_width = (in_image->width() - 2*(d->m_edge_buffer))*d->m_rescale_factor;
  int out_height = (in_image->height() - 2*(d->m_edge_buffer))*d->m_rescale_factor;
  vital::image im( out_width, out_height );
  
  // get pointer to new image container.
  stab_image = std::make_shared<kwiver::vital::simple_image_container>( im );

  d->m_stabilize->process_image( frame_time, in_image,
                                 s2r_homog, new_ref );
  
  if( d->m_edge_buffer != 0 || d->m_rescale_factor != 1 )
  {
    Eigen::Matrix< double, 3, 3 > H = (*(s2r_homog->homography())).matrix();
  
    if( d->m_edge_buffer != 0 )
    {
      // Modify s2r_homog so that (edge_buffer,edge_buffer) maps to (0,0) in the 
      // stabilized image.
      H(0,2) -= d->m_edge_buffer;
      H(1,2) -= d->m_edge_buffer;
    }
    
    if( d->m_rescale_factor != 0 )
    {
      // Modify s2r_homog so that one source image pixel corresponds to 
      // 'rescale_factor' rendered image pixels.
      H(0,0) *= d->m_rescale_factor;
      H(0,1) *= d->m_rescale_factor;
      H(0,2) *= d->m_rescale_factor;
      H(1,0) *= d->m_rescale_factor;
      H(1,1) *= d->m_rescale_factor;
      H(1,2) *= d->m_rescale_factor;
    }
    
    *s2r_homog = kwiver::vital::homography_f2f( H, s2r_homog->from_id(), 
                                                s2r_homog->to_id() );
   }

  d->m_warp->warp( in_image, stab_image, s2r_homog->homography() );
  
  push_to_port_using_trait( homography_src_to_ref, s2r_homog );
  push_to_port_using_trait( image, stab_image );
  push_to_port_using_trait( coordinate_system_updated, new_ref );
  
  d->m_timer.stop();
  double elapsed_time = d->m_timer.elapsed();
  LOG_DEBUG( logger(), "Total processing time: " << elapsed_time << " seconds");
}


// ----------------------------------------------------------------
void stabilize_video_process
::make_ports()
{
  // Set up for required ports
  sprokit::process::port_flags_t optional;
  sprokit::process::port_flags_t required;
  required.insert( flag_required );

  // -- input --
  declare_input_port_using_trait( timestamp, optional );
  declare_input_port_using_trait( image, required );

  // -- output --
  declare_output_port_using_trait( homography_src_to_ref, optional );
  declare_output_port_using_trait( image, optional );
  declare_output_port_using_trait( coordinate_system_updated, optional );
}


// ----------------------------------------------------------------
void stabilize_video_process
::make_config()
{
  declare_config_using_trait( stabilize );
  declare_config_using_trait( warp );
  declare_config_using_trait( edge_buffer );
  declare_config_using_trait( rescale_factor );
}


// ================================================================
stabilize_video_process::priv
::priv()
{
}


stabilize_video_process::priv
::~priv()
{
}

} // end namespace