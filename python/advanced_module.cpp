// Python module exports for advanced utilities
#ifndef NLM_ADVANCED_MODULE_H
#define NLM_ADVANCED_MODULE_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "advanced_utils.h"

namespace py = pybind11;
namespace nlm {

// PerformanceProfiler Python bindings
void bind_performance_profiler(py::module& m) {
    py::class_<PerformanceProfiler::PerformanceMetrics>(m, "PerformanceMetrics", R"pbdoc(
        Performance measurement metrics for NLM simulations.
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("total_time_ms", &PerformanceProfiler::PerformanceMetrics::total_time_ms,
                      "Total execution time in milliseconds")
        .def_readwrite("average_step_time_ms", &PerformanceProfiler::PerformanceMetrics::average_step_time_ms,
                      "Average time per simulation step in milliseconds")
        .def_readwrite("min_step_time_ms", &PerformanceProfiler::PerformanceMetrics::min_step_time_ms,
                      "Minimum step time in milliseconds")
        .def_readwrite("max_step_time_ms", &PerformanceProfiler::PerformanceMetrics::max_step_time_ms,
                      "Maximum step time in milliseconds")
        .def_readwrite("total_neurons", &PerformanceProfiler::PerformanceMetrics::total_neurons,
                      "Total number of neurons")
        .def_readwrite("total_synapses", &PerformanceProfiler::PerformanceMetrics::total_synapses,
                      "Total number of synapses")
        .def_readwrite("total_spikes", &PerformanceProfiler::PerformanceMetrics::total_spikes,
                      "Total number of spikes")
        .def_readwrite("average_firing_rate", &PerformanceProfiler::PerformanceMetrics::average_firing_rate,
                      "Average firing rate in Hz")
        .def_readwrite("step_count", &PerformanceProfiler::PerformanceMetrics::step_count,
                      "Number of simulation steps")
        .def("__repr__", [](const PerformanceProfiler::PerformanceMetrics& metrics) {
            return "PerformanceMetrics(steps=" + std::to_string(metrics.step_count) +
                   ", time_ms=" + std::to_string(metrics.total_time_ms) +
                   ", neurons=" + std::to_string(metrics.total_neurons) + ")";
        }, "String representation of PerformanceMetrics");
    
    m.def("profile_brain", &PerformanceProfiler::profile_brain,
         py::arg("brain"), py::arg("steps"), py::arg("warmup") = true,
         "Profile brain performance over multiple steps");
    
    m.def("print_performance_metrics", [](const PerformanceProfiler::PerformanceMetrics& metrics) {
        std::cout << PerformanceProfiler::metrics_to_string(metrics) << std::endl;
    }, py::arg("metrics"), "Print performance metrics to console");
}

// BatchProcessor Python bindings
void bind_batch_processor(py::module& m) {
    py::class_<BatchProcessor::BatchResult>(m, "BatchResult", R"pbdoc(
        Result of batch processing operations.
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("brains", &BatchProcessor::BatchResult::brains,
                      "List of processed brains")
        .def_readwrite("agents", &BatchProcessor::BatchResult::agents,
                      "List of processed agent brains")
        .def_readwrite("worlds", &BatchProcessor::BatchResult::worlds,
                      "List of processed worlds")
        .def_readwrite("validation_results", &BatchProcessor::BatchResult::validation_results,
                      "List of validation results")
        .def_readwrite("errors", &BatchProcessor::BatchResult::errors,
                      "List of error messages")
        .def_readwrite("success", &BatchProcessor::BatchResult::success,
                      "Whether the batch processing succeeded");
    
    m.def("batch_simulate", &BatchProcessor::batch_simulate,
         py::arg("configs"), py::arg("step_counts"), py::arg("num_threads") = 1,
         "Simulate multiple brains in parallel");
    
    m.def("batch_synchronize", &BatchProcessor::batch_synchronize,
         py::arg("brains"), py::arg("agents"), py::arg("worlds"),
         "Synchronize multiple simulation components");
}

// AdvancedConfigManager Python bindings
void bind_advanced_config_manager(py::module& m) {
    py::class_<AdvancedConfigManager::ConfigurationProfile>(m, "ConfigurationProfile", R"pbdoc(
        Configuration profile for saving and loading NLM settings.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<const std::string&, const std::map<std::string, std::string>&,
                   const std::string&, const std::string&, const std::string&>(),
             py::arg("name"), py::arg("settings"), py::arg("description") = "",
             py::arg("author") = "", py::arg("version") = "1.0.0")
        .def_readwrite("name", &AdvancedConfigManager::ConfigurationProfile::name,
                      "Profile name")
        .def_readwrite("settings", &AdvancedConfigManager::ConfigurationProfile::settings,
                      "Configuration settings")
        .def_readwrite("description", &AdvancedConfigManager::ConfigurationProfile::description,
                      "Profile description")
        .def_readwrite("author", &AdvancedConfigManager::ConfigurationProfile::author,
                      "Profile author")
        .def_readwrite("version", &AdvancedConfigManager::ConfigurationProfile::version,
                      "Profile version")
        .def("__repr__", [](const AdvancedConfigManager::ConfigurationProfile& profile) {
            return "ConfigurationProfile(name=" + profile.name + ", version=" + profile.version + ")";
        }, "String representation of ConfigurationProfile");
    
    m.def("load_config_library", &AdvancedConfigManager::load_config_library,
         py::arg("directory"), "Load configuration profiles from directory");
    
    m.def("save_config_profile", &AdvancedConfigManager::save_config_profile,
         py::arg("profile"), py::arg("directory"), "Save configuration profile to directory");
    
    m.def("merge_configs", &AdvancedConfigManager::merge_configs,
         py::arg("configs"), py::arg("overrides"), "Merge multiple configurations with overrides");
    
    m.def("extract_config_summary", &AdvancedConfigManager::extract_config_summary,
         py::arg("config"), "Extract configuration summary as dictionary");
}

// StateExporter Python bindings
void bind_state_exporter(py::module& m) {
    m.def("export_brain_state", &StateExporter::export_brain_state,
         py::arg("brain"), "Export brain state as string");
    
    m.def("export_agent_state", &StateExporter::export_agent_state,
         py::arg("agent"), "Export agent state as string");
    
    m.def("export_world_state", &StateExporter::export_world_state,
         py::arg("world"), "Export world state as string");
    
    m.def("export_complete_system", &StateExporter::export_complete_system,
         py::arg("brain"), py::arg("agent"), py::arg("world"), "Export complete system state");
    
    m.def("export_performance_metrics", &StateExporter::export_performance_metrics,
         py::arg("metrics"), "Export performance metrics as string");
}

// EventSystem Python bindings
void bind_event_system(py::module& m) {
    py::class_<EventSystem::Event>(m, "Event", R"pbdoc(
        Event in the NLM event system.
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("type", &EventSystem::Event::type, "Event type")
        .def_readwrite("timestamp", &EventSystem::Event::timestamp, "Event timestamp")
        .def_readwrite("data", &EventSystem::Event::data, "Event data")
        .def_readwrite("source", &EventSystem::Event::source, "Event source");
    
    py::class_<EventSystem::EventListener>(m, "EventListener", R"pbdoc(
        Listener for NLM events.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<const std::string&, std::function<bool(const EventSystem::Event&)>>(),
             py::arg("name"), py::arg("callback"))
        .def_readwrite("name", &EventSystem::EventListener::name, "Listener name")
        .def_readwrite("callback", &EventSystem::EventListener::callback, "Callback function")
        .def_readwrite("enabled", &EventSystem::EventListener::enabled, "Whether listener is enabled");
    
    py::class_<EventSystem::EventManager>(m, "EventManager", R"pbdoc(
        Manager for NLM events.
    )pbdoc")
        .def(py::init<>())
        .def("add_listener", &EventSystem::EventManager::add_listener,
             py::arg("listener"), "Add event listener")
        .def("remove_listener", &EventSystem::EventManager::remove_listener,
             py::arg("name"), "Remove event listener by name")
        .def("emit_event", &EventSystem::EventManager::emit_event,
             py::arg("type"), py::arg("data"), py::arg("source") = "system",
             "Emit an event")
        .def("advance_time", &EventSystem::EventManager::advance_time,
             py::arg("dt"), "Advance simulation time")
        .def("get_event_history", &EventSystem::EventManager::get_event_history,
             "Get event history")
        .def("clear_event_history", &EventSystem::EventManager::clear_event_history,
             "Clear event history")
        .def("enable_listener", &EventSystem::EventManager::enable_listener,
             py::arg("name"), py::arg("enabled"), "Enable/disable event listener")
        .def("get_listener_status", &EventSystem::EventManager::get_listener_status,
             "Get listener status");
    
    m.def("setup_default_events", &EventSystem::setup_default_events,
         py::arg("event_manager"), "Setup default event listeners");
}

// DebugUtils Python bindings
void bind_debug_utils(py::module& m) {
    m.def("generate_debug_report", &DebugUtils::generate_debug_report,
         py::arg("brain"), py::arg("title") = "Debug Report",
         "Generate detailed debug report for brain");
    
    m.def("format_memory_usage", &DebugUtils::format_memory_usage,
         py::arg("bytes"), "Format memory usage in human-readable format");
    
    m.def("get_system_info", &DebugUtils::get_system_info,
         "Get system information");
}

// AdvancedPlasticity Python bindings
void bind_advanced_plasticity(py::module& m) {
    py::class_<AdvancedPlasticity::PlasticityRule>(m, "PlasticityRule", R"pbdoc(
        Custom plasticity rule for NLM.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<const std::string&, const std::string&>(),
             py::arg("name"), py::arg("description"))
        .def_readwrite("name", &AdvancedPlasticity::PlasticityRule::name,
                      "Rule name")
        .def_readwrite("description", &AdvancedPlasticity::PlasticityRule::description,
                      "Rule description")
        .def_readwrite("parameters", &AdvancedPlasticity::PlasticityRule::parameters,
                      "Rule parameters");
    
    py::class_<AdvancedPlasticity::CustomPlasticityConfig>(m, "CustomPlasticityConfig", R"pbdoc(
        Custom plasticity configuration for advanced NLM simulations.
    )pbdoc")
        .def(py::init<>())
        .def("add_rule", &AdvancedPlasticity::CustomPlasticityConfig::add_rule,
             py::arg("rule"), "Add plasticity rule")
        .def("set_global_parameter", &AdvancedPlasticity::CustomPlasticityConfig::set_global_parameter,
             py::arg("key"), py::arg("value"), "Set global parameter")
        .def("get_global_parameter", &AdvancedPlasticity::CustomPlasticityConfig::get_global_parameter,
             py::arg("key"), py::arg("default_value") = 0.0, "Get global parameter");
    
    m.def("create_advanced_config", &AdvancedPlasticity::create_advanced_config,
         "Create advanced plasticity configuration");
    
    m.def("analyze_plasticity_impact", &AdvancedPlasticity::analyze_plasticity_impact,
         py::arg("brain"), py::arg("config"), "Analyze plasticity impact");
}

// Advanced utilities module binding
PYBIND11_MODULE(advanced, m) {
    m.doc() = R"pbdoc(
        Advanced utilities for NLM (Neural Learning Machine) Python bindings.
        
        This module provides advanced functionality for power users and researchers,
        including performance profiling, batch processing, event systems,
        debugging tools, and custom plasticity rules.
        
        Key Features:
        - Performance profiling and benchmarking
        - Batch processing with parallel execution
        - Advanced configuration management
        - Custom event systems
        - Debugging and diagnostic tools
        - Custom plasticity rules and advanced learning
        - State export/import utilities
        - Memory usage analysis
    )pbdoc";
    
    // Bind performance profiler
    bind_performance_profiler(m);
    
    // Bind batch processor
    bind_batch_processor(m);
    
    // Bind advanced config manager
    bind_advanced_config_manager(m);
    
    // Bind state exporter
    bind_state_exporter(m);
    
    // Bind event system
    bind_event_system(m);
    
    // Bind debug utils
    bind_debug_utils(m);
    
    // Bind advanced plasticity
    bind_advanced_plasticity(m);
    
    // Version information
    m.attr("__version__") = "2.0.0";
    m.attr("__author__") = "NLM Project";
    m.attr("__description__") = "Advanced utilities for NLM Python bindings";
}

} // namespace nlm

#endif // NLM_ADVANCED_MODULE_H