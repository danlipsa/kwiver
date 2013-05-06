/*ckwg +5
 * Copyright 2011-2013 by Kitware, Inc. All Rights Reserved. Please refer to
 * KITWARE_LICENSE.TXT for licensing information, or contact General Counsel,
 * Kitware, Inc., 28 Corporate Drive, Clifton Park, NY 12065.
 */

#include "helpers/pipeline_builder.h"
#include "helpers/tool_io.h"
#include "helpers/tool_main.h"
#include "helpers/tool_usage.h"

#include <sprokit/pipeline_util/export_dot.h>
#include <sprokit/pipeline_util/path.h>

#include <sprokit/pipeline/config.h>
#include <sprokit/pipeline/modules.h>
#include <sprokit/pipeline/pipeline.h>
#include <sprokit/pipeline/process.h>
#include <sprokit/pipeline/process_cluster.h>
#include <sprokit/pipeline/process_registry.h>
#include <sprokit/pipeline/types.h>

#include <boost/filesystem/fstream.hpp>
#include <boost/program_options/value_semantic.hpp>
#include <boost/program_options/variables_map.hpp>

#include <string>
#include <vector>

#include <cstddef>
#include <cstdlib>

static boost::program_options::options_description pipe_to_dot_cluster_options();
static boost::program_options::options_description pipe_to_dot_pipeline_options();

int
tool_main(int argc, char* argv[])
{
  sprokit::load_known_modules();

  boost::program_options::options_description desc;
  desc
    .add(tool_common_options())
    .add(pipeline_common_options())
    .add(pipeline_input_options())
    .add(pipe_to_dot_cluster_options())
    .add(pipeline_output_options())
    .add(pipe_to_dot_pipeline_options());

  boost::program_options::variables_map const vm = tool_parse(argc, argv, desc);

  sprokit::process_cluster_t cluster;
  sprokit::pipeline_t pipe;

  bool const have_cluster = vm.count("cluster");
  bool const have_cluster_type = vm.count("cluster-type");
  bool const have_pipeline = vm.count("pipeline");
  bool const have_setup = vm.count("setup");

  bool const export_cluster = (have_cluster || have_cluster_type);

  if (export_cluster && have_pipeline)
  {
    std::cerr << "Error: The \'cluster\' and \'cluster-type\' options are "
                 "incompatible with the \'pipeline\' option" << std::endl;

    return EXIT_FAILURE;
  }

  if (export_cluster && have_setup)
  {
    std::cerr << "Error: The \'cluster\' and \'cluster-type\' options are "
                 "incompatible with the \'setup\' option" << std::endl;

    return EXIT_FAILURE;
  }

  std::string const graph_name = vm["name"].as<std::string>();

  if (export_cluster)
  {
    if (have_cluster && have_cluster_type)
    {
      std::cerr << "Error: The \'cluster\' option is incompatible "
                   "with the \'cluster-type\' option" << std::endl;

      return EXIT_FAILURE;
    }

    pipeline_builder builder;

    builder.load_from_options(vm);
    sprokit::config_t const conf = builder.config();

    if (have_cluster)
    {
      sprokit::path_t const ipath = vm["cluster"].as<sprokit::path_t>();

      istream_t const istr = open_istream(ipath);

      sprokit::cluster_info_t const info = sprokit::bake_cluster(*istr);

      conf->set_value(sprokit::process::config_name, graph_name);

      sprokit::process_t const proc = info->ctor(conf);
      cluster = boost::dynamic_pointer_cast<sprokit::process_cluster>(proc);
    }
    else if (have_cluster_type)
    {
      sprokit::process_registry_t const reg = sprokit::process_registry::self();

      sprokit::process::type_t const type = vm["cluster-type"].as<sprokit::process::type_t>();

      sprokit::process_t const proc = reg->create_process(type, graph_name, conf);
      cluster = boost::dynamic_pointer_cast<sprokit::process_cluster>(proc);

      if (!cluster)
      {
        std::cerr << "Error: The given type (\'" << type << "\') "
                     "is not a cluster" << std::endl;

        return EXIT_FAILURE;
      }
    }
    else
    {
      std::cerr << "Internal error: option tracking failure" << std::endl;

      return EXIT_FAILURE;
    }
  }
  else if (have_pipeline)
  {
    pipeline_builder const builder(vm, desc);

    pipe = builder.pipeline();

    if (!pipe)
    {
      std::cerr << "Error: Unable to bake pipeline" << std::endl;

      return EXIT_FAILURE;
    }
  }
  else
  {
    std::cerr << "Error: One of \'cluster\', \'cluster-type\', or "
                 "\'pipeline\' must be specified" << std::endl;

    tool_usage(EXIT_FAILURE, desc);
  }

  // Make sure we have one, but not both.
  if (!cluster == !pipe)
  {
    std::cerr << "Internal error: option tracking failure" << std::endl;

    return EXIT_FAILURE;
  }

  sprokit::path_t const opath = vm["output"].as<sprokit::path_t>();

  ostream_t const ostr = open_ostream(opath);

  if (cluster)
  {
    sprokit::export_dot(*ostr, cluster, graph_name);
  }
  else if (pipe)
  {
    if (have_setup)
    {
      pipe->setup_pipeline();
    }

    sprokit::export_dot(*ostr, pipe, graph_name);
  }

  return EXIT_SUCCESS;
}

boost::program_options::options_description
pipe_to_dot_cluster_options()
{
  boost::program_options::options_description desc("Cluster options");

  desc.add_options()
    ("cluster,C", boost::program_options::value<std::string>()->value_name("FILE"), "the cluster file to export")
    ("cluster-type,T", boost::program_options::value<std::string>()->value_name("TYPE"), "the cluster type to export")
  ;

  return desc;
}

boost::program_options::options_description
pipe_to_dot_pipeline_options()
{
  boost::program_options::options_description desc("Pipeline options");

  desc.add_options()
    ("name,n", boost::program_options::value<std::string>()->value_name("NAME")->default_value("unnamed"), "the name of the graph")
    ("setup", "whether to setup the pipeline before exporting or not")
  ;

  return desc;
}
