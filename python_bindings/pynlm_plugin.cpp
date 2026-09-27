#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/plugin/PluginBase.hpp"
#include "../src/plugin/PluginManager.hpp"
#include "../src/plugin/PluginRegistry.hpp"

namespace py = pybind11;

void init_pynlm_plugin(py::module& m) {
    // PluginError classes
    py::register_exception<PluginError>(m, "PluginError");
    py::register_exception<PluginNotFoundError>(m, "PluginNotFoundError");
    py::register_exception<PluginLoadError>(m, "PluginLoadError");
    py::register_exception<PluginActivationError>(m, "PluginActivationError");

    // Plugin class
    py::class_<Plugin>(m, "Plugin")
        .def(py::init<std::string>())
        .def("get_name", &Plugin::get_name)
        .def("get_version", &Plugin::get_version)
        .def("get_description", &Plugin::get_description)
        .def("get_author", &Plugin::get_author)
        .def("get_dependencies", &Plugin::get_dependencies)
        .def("get_optional_dependencies", &Plugin::get_optional_dependencies)
        .def("get_supported_platforms", &Plugin::get_supported_platforms)
        .def("get_supported_brain_types", &Plugin::get_supported_brain_types)
        .def("configure", &Plugin::configure)
        .def("get_configuration", &Plugin::get_configuration)
        .def("get_configuration_value", &Plugin::get_configuration_value,
             py::arg("key"), py::arg("default") = nullptr)
        .def("enable", &Plugin::enable)
        .def("disable", &Plugin::disable)
        .def("activate", &Plugin::activate)
        .def("deactivate", &Plugin::deactivate)
        .def("is_enabled", &Plugin::is_enabled)
        .def("is_active", &Plugin::is_active)
        .def("add_event_handler", &Plugin::add_event_handler,
             py::arg("event_type"), py::arg("handler"))
        .def("remove_event_handler", &Plugin::remove_event_handler,
             py::arg("event_type"), py::arg("handler"))
        .def("emit_event", &Plugin::emit_event,
             py::arg("event_type"), py::arg("data") = nullptr)
        .def("add_hook", &Plugin::add_hook,
             py::arg("hook_type"), py::arg("hook_func"))
        .def("remove_hook", &Plugin::remove_hook,
             py::arg("hook_type"), py::arg("hook_func"))
        .def("execute_hook", &Plugin::execute_hook,
             py::arg("hook_type"), py::arg("args") = py::list(),
             py::arg("kwargs") = py::dict());

    // PluginManager class
    py::class_<PluginManager>(m, "PluginManager")
        .def(py::init<>())
        .def("add_plugin_directory", &PluginManager::add_plugin_directory,
             py::arg("directory"))
        .def("discover_plugins", &PluginManager::discover_plugins)
        .def("load_plugin", &PluginManager::load_plugin,
             py::arg("plugin_name"), py::arg("force") = false)
        .def("unload_plugin", &PluginManager::unload_plugin,
             py::arg("plugin_name"))
        .def("enable_plugin", &PluginManager::enable_plugin,
             py::arg("plugin_name"))
        .def("disable_plugin", &PluginManager::disable_plugin,
             py::arg("plugin_name"))
        .def("activate_plugin", &PluginManager::activate_plugin,
             py::arg("plugin_name"))
        .def("deactivate_plugin", &PluginManager::deactivate_plugin,
             py::arg("plugin_name"))
        .def("configure_plugin", &PluginManager::configure_plugin,
             py::arg("plugin_name"), py::arg("config"))
        .def("get_plugin", &PluginManager::get_plugin,
             py::arg("plugin_name"))
        .def("list_plugins", &PluginManager::list_plugins)
        .def("execute_plugin_method", &PluginManager::execute_plugin_method,
             py::arg("plugin_name"), py::arg("method"),
             py::arg("args") = py::list(), py::arg("kwargs") = py::dict())
        .def("get_plugin_info", &PluginManager::get_plugin_info,
             py::arg("plugin_name"));

    // High-level API functions
    m.def("load_plugin", &load_plugin,
          py::arg("plugin_name"), py::arg("plugin_class") = nullptr);
    m.def("unload_plugin", &unload_plugin,
          py::arg("plugin_name"));
    m.def("enable_plugin", &enable_plugin,
          py::arg("plugin_name"));
    m.def("disable_plugin", &disable_plugin,
          py::arg("plugin_name"));
    m.def("activate_plugin", &activate_plugin,
          py::arg("plugin_name"));
    m.def("deactivate_plugin", &deactivate_plugin,
          py::arg("plugin_name"));
    m.def("configure_plugin", &configure_plugin,
          py::arg("plugin_name"), py::arg("config"));
    m.def("list_plugins", &list_plugins);
    m.def("execute_plugin", &execute_plugin,
          py::arg("plugin_name"), py::arg("method"),
          py::arg("args") = py::list(), py::arg("kwargs") = py::dict());
}

PYBIND11_MODULE(pynlm_plugin, m) {
    init_pynlm_plugin(m);
}