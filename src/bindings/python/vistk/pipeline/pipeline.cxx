/*ckwg +5
 * Copyright 2011-2013 by Kitware, Inc. All Rights Reserved. Please refer to
 * KITWARE_LICENSE.TXT for licensing information, or contact General Counsel,
 * Kitware, Inc., 28 Corporate Drive, Clifton Park, NY 12065.
 */

#include <sprokit/pipeline/pipeline.h>
#include <sprokit/pipeline/process_cluster.h>

#include <boost/python/class.hpp>
#include <boost/python/module.hpp>

/**
 * \file pipeline.cxx
 *
 * \brief Python bindings for \link sprokit::pipeline\endlink.
 */

using namespace boost::python;

BOOST_PYTHON_MODULE(pipeline)
{
  class_<sprokit::pipeline, sprokit::pipeline_t, boost::noncopyable>("Pipeline"
    , "A data structure for a collection of connected processes."
    , no_init)
    .def(init<>())
    .def(init<sprokit::config_t>())
    .def("add_process", &sprokit::pipeline::add_process
      , (arg("process"))
      , "Add a process to the pipeline.")
    .def("remove_process", &sprokit::pipeline::remove_process
      , (arg("name"))
      , "Remove a process from the pipeline.")
    .def("connect", &sprokit::pipeline::connect
      , (arg("upstream"), arg("upstream_port"), arg("downstream"), arg("downstream_port"))
      , "Connect two ports within the pipeline together.")
    .def("disconnect", &sprokit::pipeline::disconnect
      , (arg("upstream"), arg("upstream_port"), arg("downstream"), arg("downstream_port"))
      , "Disconnect two ports from each other in the pipeline.")
    .def("setup_pipeline", &sprokit::pipeline::setup_pipeline
      , "Prepares the pipeline for execution.")
    .def("is_setup", &sprokit::pipeline::is_setup
      , "Returns True if the pipeline has been setup, False otherwise.")
    .def("setup_successful", &sprokit::pipeline::setup_successful
      , "Returns True if the pipeline has been successfully setup, False otherwise.")
    .def("reset", &sprokit::pipeline::reset
      , "Resets connections and mappings within the pipeline.")
    .def("reconfigure", &sprokit::pipeline::reconfigure
      , (arg("conf"))
      , "Reconfigures processes within the pipeline.")
    .def("process_names", &sprokit::pipeline::process_names
      , "Returns a list of all process names in the pipeline.")
    .def("process_by_name", &sprokit::pipeline::process_by_name
      , (arg("name"))
      , "Get a process by name.")
    .def("parent_cluster", &sprokit::pipeline::parent_cluster
      , (arg("name"))
      , "Get a process' parent cluster.")
    .def("cluster_names", &sprokit::pipeline::cluster_names
      , "Returns a list of all cluster names in the pipeline.")
    .def("cluster_by_name", &sprokit::pipeline::cluster_by_name
      , (arg("name"))
      , "Get a cluster by name.")
    .def("connections_from_addr", &sprokit::pipeline::connections_from_addr
      , (arg("name"), arg("port"))
      , "Return the addresses of ports that are connected downstream of a port.")
    .def("connection_to_addr", &sprokit::pipeline::connection_to_addr
      , (arg("name"), arg("port"))
      , "Return the address for the port that is connected upstream of a port.")
    .def("upstream_for_process", &sprokit::pipeline::upstream_for_process
      , (arg("name"))
      , "Return all processes upstream of the given process.")
    .def("upstream_for_port", &sprokit::pipeline::upstream_for_port
      , (arg("name"), arg("port"))
      , "Return the process upstream of the given port.")
    .def("downstream_for_process", &sprokit::pipeline::downstream_for_process
      , (arg("name"))
      , "Return all processes downstream of the given process.")
    .def("downstream_for_port", &sprokit::pipeline::downstream_for_port
      , (arg("name"), arg("port"))
      , "Return the processes downstream of the given port.")
    .def("sender_for_port", &sprokit::pipeline::sender_for_port
      , (arg("name"), arg("port"))
      , "Return the port that is sending to the given port.")
    .def("receivers_for_port", &sprokit::pipeline::receivers_for_port
      , (arg("name"), arg("port"))
      , "Return the ports that are receiving from the given port.")
    .def("edge_for_connection", &sprokit::pipeline::edge_for_connection
      , (arg("upstream_name"), arg("upstream_port"), arg("downstream_name"), arg("downstream_port"))
      , "Returns the edge for the connection.")
    .def("input_edges_for_process", &sprokit::pipeline::input_edges_for_process
      , (arg("name"))
      , "Return the edges that are sending to the given process.")
    .def("input_edge_for_port", &sprokit::pipeline::input_edge_for_port
      , (arg("name"), arg("port"))
      , "Return the edge that is sending to the given port.")
    .def("output_edges_for_process", &sprokit::pipeline::output_edges_for_process
      , (arg("name"))
      , "Return the edges that are receiving data from the given process.")
    .def("output_edges_for_port", &sprokit::pipeline::output_edges_for_port
      , (arg("name"), arg("port"))
      , "Return the edges that are receiving data from the given port.")
  ;
}
