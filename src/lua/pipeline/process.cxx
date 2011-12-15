/*ckwg +5
 * Copyright 2011 by Kitware, Inc. All Rights Reserved. Please refer to
 * KITWARE_LICENSE.TXT for licensing information, or contact General Counsel,
 * Kitware, Inc., 28 Corporate Drive, Clifton Park, NY 12065.
 */

#include <vistk/pipeline/edge.h>
#include <vistk/pipeline/process.h>
#include <vistk/pipeline/stamp.h>

#include <lua/helpers/lua_include.h>
#include <lua/helpers/lua_convert_set.h>
#include <lua/helpers/lua_convert_vector.h>
#include <lua/helpers/lua_static_member.h>

#include <luabind/class.hpp>
#include <luabind/function.hpp>

/**
 * \file process.cxx
 *
 * \brief Lua bindings for \link vistk::process\endlink.
 */

extern "C"
{

int luaopen_vistk_pipeline_process(lua_State* L);

}

using namespace luabind;

class wrap_process
  : public vistk::process
  , public wrap_base
{
  public:
    wrap_process(vistk::config_t const& config);
    ~wrap_process();

    void _base_init();

    void _base_step();

    constraints_t _base_constraints() const;

    void _base_connect_input_port(port_t const& port, vistk::edge_t edge);
    void _base_connect_output_port(port_t const& port, vistk::edge_t edge);

    ports_t _base_input_ports() const;
    ports_t _base_output_ports() const;

    port_info_t _base_input_port_info(port_t const& port);
    port_info_t _base_output_port_info(port_t const& port);

    vistk::config::keys_t _base_available_config() const;

    conf_info_t _base_config_info(vistk::config::key_t const& key);

    void _init();

    void _step();

    constraints_t _constraints() const;

    void _connect_input_port(port_t const& port, vistk::edge_t edge);
    void _connect_output_port(port_t const& port, vistk::edge_t edge);

    ports_t _input_ports() const;
    ports_t _output_ports() const;

    port_info_t _input_port_info(port_t const& port);
    port_info_t _output_port_info(port_t const& port);

    vistk::config::keys_t _available_config() const;

    conf_info_t _config_info(vistk::config::key_t const& key);

    void _declare_input_port(port_t const& port, port_info_t const& info);
    void _declare_output_port(port_t const& port, port_info_t const& info);

    void _declare_configuration_key(vistk::config::key_t const& key, conf_info_t const& info);

    void _mark_process_as_complete();
    vistk::stamp_t _heartbeat_stamp() const;

    vistk::edge_t _input_port_edge(port_t const& port) const;
    vistk::edges_t _output_port_edges(port_t const& port) const;

    vistk::edge_datum_t _grab_from_port(port_t const& port) const;
    void _push_to_port(port_t const& port, vistk::edge_datum_t const& dat) const;

    vistk::config_t _get_config() const;
    vistk::config::value_t _config_value(vistk::config::key_t const& key) const;

    vistk::process::data_info_t _edge_data_info(vistk::edge_data_t const& data);
    void _push_to_edges(vistk::edges_t const& edges, vistk::edge_datum_t const& dat);
    vistk::edge_datum_t _grab_from_edge(vistk::edge_t const& edge);
};

int
luaopen_vistk_pipeline_process(lua_State* L)
{
  open(L);

  module(L, "vistk")
  [
    namespace_("pipeline")
    [
      class_<vistk::process::name_t>("process_name")
        .def(constructor<>())
    , class_<vistk::process::names_t>("process_names")
        .def(constructor<>())
    , class_<vistk::process::constraint_t>("constraint")
        .def(constructor<>())
    , class_<vistk::process::constraints_t>("constraints")
        .def(constructor<>())
    , class_<vistk::process::port_description_t>("port_description")
        .def(constructor<>())
    , class_<vistk::process::port_t>("port")
        .def(constructor<>())
    , class_<vistk::process::ports_t>("ports")
        .def(constructor<>())
    , class_<vistk::process::port_type_t>("port_type")
        .def(constructor<>())
    , class_<vistk::process::port_flag_t>("port_flag")
        .def(constructor<>())
    , class_<vistk::process::port_flags_t>("port_flags")
        .def(constructor<>())
    , class_<vistk::process::port_addr_t>("port_addr")
        .def(constructor<>())
        .def_readwrite("process", &vistk::process::port_addr_t::first)
        .def_readwrite("port", &vistk::process::port_addr_t::second)
    , class_<vistk::process::port_addrs_t>("port_addrs")
        .def(constructor<>())
    , class_<vistk::process::port_info, vistk::process::port_info_t>("port_info")
        .def(constructor<vistk::process::port_type_t, vistk::process::port_flags_t, vistk::process::port_description_t>())
        .def_readonly("type", &vistk::process::port_info::type)
        .def_readonly("flags", &vistk::process::port_info::flags)
        .def_readonly("description", &vistk::process::port_info::description)
    , class_<vistk::process::conf_info, vistk::process::conf_info_t>("conf_info")
        .def(constructor<vistk::config::value_t, vistk::config::description_t>())
        .def_readonly("default", &vistk::process::conf_info::def)
        .def_readonly("description", &vistk::process::conf_info::description)
    , class_<vistk::process::data_info, vistk::process::data_info_t>("data_info")
        .def(constructor<bool, bool, vistk::datum::type_t>())
        .def_readonly("same_color", &vistk::process::data_info::same_color)
        .def_readonly("in_sync", &vistk::process::data_info::in_sync)
        .def_readonly("max_status", &vistk::process::data_info::max_status)
    , class_<vistk::process, wrap_process, vistk::process_t>("lua_process")
        .def(constructor<vistk::config_t>())
        .def("init", &vistk::process::init)
        .def("step", &vistk::process::step)
        .def("constraints", &vistk::process::constraints)
        .def("connect_input_port", &vistk::process::connect_input_port)
        .def("connect_output_port", &vistk::process::connect_output_port)
        .def("input_ports", &vistk::process::input_ports)
        .def("output_ports", &vistk::process::output_ports)
        .def("input_port_info", &vistk::process::input_port_info)
        .def("output_port_info", &vistk::process::output_port_info)
        .def("available_config", &vistk::process::available_config)
        .def("config_info", &vistk::process::config_info)
        .def("name", &vistk::process::name)
        .def("type", &vistk::process::type)
        .def("_base_init", &wrap_process::_base_init)
        .def("_base_step", &wrap_process::_base_step)
        .def("_base_constraints", &wrap_process::_base_constraints)
        .def("_base_connect_input_port", &wrap_process::_base_connect_input_port)
        .def("_base_connect_output_port", &wrap_process::_base_connect_output_port)
        .def("_base_input_ports", &wrap_process::_base_input_ports)
        .def("_base_output_ports", &wrap_process::_base_output_ports)
        .def("_base_input_port_info", &wrap_process::_base_input_port_info)
        .def("_base_output_port_info", &wrap_process::_base_output_port_info)
        .def("_base_available_config", &wrap_process::_base_available_config)
        .def("_base_config_info", &wrap_process::_base_config_info)
        .def("_init", &wrap_process::_init, &wrap_process::_base_init)
        .def("_step", &wrap_process::_step, &wrap_process::_base_step)
        .def("_constraints", &wrap_process::_constraints, &wrap_process::_base_constraints)
        .def("_connect_input_port", &wrap_process::_connect_input_port, &wrap_process::_base_connect_input_port)
        .def("_connect_output_port", &wrap_process::_connect_output_port, &wrap_process::_base_connect_output_port)
        .def("_input_ports", &wrap_process::_input_ports, &wrap_process::_base_input_ports)
        .def("_output_ports", &wrap_process::_output_ports, &wrap_process::_base_output_ports)
        .def("_input_port_info", &wrap_process::_input_port_info, &wrap_process::_base_input_port_info)
        .def("_output_port_info", &wrap_process::_output_port_info, &wrap_process::_base_output_port_info)
        .def("_available_config", &wrap_process::_available_config, &wrap_process::_base_available_config)
        .def("_config_info", &wrap_process::_config_info, &wrap_process::_base_config_info)
        .def("declare_input_port", &wrap_process::_declare_input_port)
        .def("declare_output_port", &wrap_process::_declare_output_port)
        .def("declare_configuration_key", &wrap_process::_declare_configuration_key)
        .def("mark_process_as_complete", &wrap_process::_mark_process_as_complete)
        .def("heartbeat_stamp", &wrap_process::_heartbeat_stamp)
        .def("input_port_edge", &wrap_process::_input_port_edge)
        .def("output_port_edges", &wrap_process::_output_port_edges)
        .def("grab_from_port", &wrap_process::_grab_from_port)
        .def("push_to_port", &wrap_process::_push_to_port)
        .def("get_config", &wrap_process::_get_config)
        .def("config_value", &wrap_process::_config_value)
        .def("edge_data_info", &wrap_process::_edge_data_info)
        .def("push_to_edges", &wrap_process::_push_to_edges)
        .def("grab_from_edge", &wrap_process::_grab_from_edge)
    ]
  ];

  lua_getfield(L, LUA_GLOBALSINDEX, "vistk");
  lua_getfield(L, -1, "pipeline");
  lua_getfield(L, -1, "lua_process");
  LUA_STATIC_MEMBER(L, string, vistk::process::constraint_no_threads, "constraint_no_threads");
  LUA_STATIC_MEMBER(L, string, vistk::process::constraint_no_reentrancy, "constraint_no_reentrancy");
  LUA_STATIC_MEMBER(L, string, vistk::process::constraint_unsync_input, "constraint_unsync_input");
  LUA_STATIC_MEMBER(L, string, vistk::process::constraint_unsync_output, "constraint_unsync_output");
  LUA_STATIC_MEMBER(L, string, vistk::process::port_heartbeat, "port_heartbeat");
  LUA_STATIC_MEMBER(L, string, vistk::process::config_name, "config_name");
  LUA_STATIC_MEMBER(L, string, vistk::process::config_type, "config_type");
  LUA_STATIC_MEMBER(L, string, vistk::process::type_any, "type_any");
  LUA_STATIC_MEMBER(L, string, vistk::process::type_none, "type_none");
  LUA_STATIC_MEMBER(L, string, vistk::process::flag_output_const, "flag_output_const");
  LUA_STATIC_MEMBER(L, string, vistk::process::flag_input_mutable, "flag_input_mutable");
  LUA_STATIC_MEMBER(L, string, vistk::process::flag_required, "flag_required");
  lua_pop(L, 3);

  return 0;
}

wrap_process
::wrap_process(vistk::config_t const& config)
  : vistk::process(config)
{
}

wrap_process
::~wrap_process()
{
}

void
wrap_process
::_base_init()
{
  process::_init();
}

void
wrap_process
::_base_step()
{
  process::_step();
}

vistk::process::constraints_t
wrap_process
::_base_constraints() const
{
  static constraint_t const constraint_lua = constraint_t("_lua");

  constraints_t consts = process::_constraints();

  consts.insert(constraint_lua);

  return consts;
}

void
wrap_process
::_base_connect_input_port(port_t const& port, vistk::edge_t edge)
{
  process::_connect_input_port(port, edge);
}

void
wrap_process
::_base_connect_output_port(port_t const& port, vistk::edge_t edge)
{
  process::_connect_output_port(port, edge);
}

vistk::process::ports_t
wrap_process
::_base_input_ports() const
{
  return process::_input_ports();
}

vistk::process::ports_t
wrap_process
::_base_output_ports() const
{
  return process::_output_ports();
}

vistk::process::port_info_t
wrap_process
::_base_input_port_info(port_t const& port)
{
  return process::_input_port_info(port);
}

vistk::process::port_info_t
wrap_process
::_base_output_port_info(port_t const& port)
{
  return process::_output_port_info(port);
}

vistk::config::keys_t
wrap_process
::_base_available_config() const
{
  return process::_available_config();
}

vistk::process::conf_info_t
wrap_process
::_base_config_info(vistk::config::key_t const& key)
{
  return process::_config_info(key);
}

void
wrap_process
::_init()
{
  call<void>("_init");
}

void
wrap_process
::_step()
{
  call<void>("_step");
}

vistk::process::constraints_t
wrap_process
::_constraints() const
{
  return call<vistk::process::constraints_t>("_constraints");
}

void
wrap_process
::_connect_input_port(port_t const& port, vistk::edge_t edge)
{
  call<void>("_connect_input_port", port, edge);
}

void
wrap_process
::_connect_output_port(port_t const& port, vistk::edge_t edge)
{
  call<void>("_connect_output_port", port, edge);
}

vistk::process::ports_t
wrap_process
::_input_ports() const
{
  return call<vistk::process::ports_t>("_input_ports");
}

vistk::process::ports_t
wrap_process
::_output_ports() const
{
  return call<vistk::process::ports_t>("_output_ports");
}

vistk::process::port_info_t
wrap_process
::_input_port_info(port_t const& port)
{
  return call<vistk::process::port_info_t>("_input_port_info", port);
}

vistk::process::port_info_t
wrap_process
::_output_port_info(port_t const& port)
{
  return call<vistk::process::port_info_t>("_output_port_info", port);
}

vistk::config::keys_t
wrap_process
::_available_config() const
{
  return call<vistk::config::keys_t>("_available_config");
}

vistk::process::conf_info_t
wrap_process
::_config_info(vistk::config::key_t const& key)
{
  return call<vistk::process::conf_info_t>("_config_info", key);
}

void
wrap_process
::_declare_input_port(port_t const& port, port_info_t const& info)
{
  declare_input_port(port, info);
}

void
wrap_process
::_declare_output_port(port_t const& port, port_info_t const& info)
{
  declare_output_port(port, info);
}

void
wrap_process
::_declare_configuration_key(vistk::config::key_t const& key, conf_info_t const& info)
{
  declare_configuration_key(key, info);
}

void
wrap_process
::_mark_process_as_complete()
{
  mark_process_as_complete();
}

vistk::stamp_t
wrap_process
::_heartbeat_stamp() const
{
  return heartbeat_stamp();
}

vistk::edge_t
wrap_process
::_input_port_edge(port_t const& port) const
{
  return input_port_edge(port);
}

vistk::edges_t
wrap_process
::_output_port_edges(port_t const& port) const
{
  return output_port_edges(port);
}

vistk::edge_datum_t
wrap_process
::_grab_from_port(port_t const& port) const
{
  return grab_from_port(port);
}

void
wrap_process
::_push_to_port(port_t const& port, vistk::edge_datum_t const& dat) const
{
  return push_to_port(port, dat);
}

vistk::config_t
wrap_process
::_get_config() const
{
  return get_config();
}

vistk::config::value_t
wrap_process
::_config_value(vistk::config::key_t const& key) const
{
  return config_value<vistk::config::value_t>(key);
}

vistk::process::data_info_t
wrap_process
::_edge_data_info(vistk::edge_data_t const& data)
{
  return edge_data_info(data);
}

void
wrap_process
::_push_to_edges(vistk::edges_t const& edges, vistk::edge_datum_t const& dat)
{
  push_to_edges(edges, dat);
}

vistk::edge_datum_t
wrap_process
::_grab_from_edge(vistk::edge_t const& edge)
{
  return grab_from_edge(edge);
}
