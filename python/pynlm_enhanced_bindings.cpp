#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>
#include <chrono>

namespace py = pybind11;

// Helper for the enhanced bindings
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM Python Bindings - Enhanced Version
        
        This module provides enhanced Python bindings for the NLM neural simulation framework.
        Key improvements include:
        - Context managers for automatic resource management
        - Enhanced error handling
        - Performance monitoring APIs
        - Advanced configuration presets
        - Network analysis utilities
        - Experimental features for advanced users
    )pbdoc";
    
    // Register custom exception types
    py::register_exception<ConfigurationError>(m, "ConfigurationError", PyExc_ValueError);
    py::register_exception<SimulationError>(m, "SimulationError", PyExc_RuntimeError);
    
    // Basic functionality (enhanced version)
    m.def("createBrain", [](std::shared_ptr<Config> config) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, "Create and initialize a brain with configuration", py::arg("config"));
    
    m.def("createSimpleWorld", []() {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<AgentBrain>(brain);
    }, "Create a new agent brain interface", py::arg("brain"));
    
    // Performance monitoring
    py::class_<PerformanceMonitor>(m, "PerformanceMonitor")
        .def(py::init<>())
        .def("recordStep", &PerformanceMonitor::recordStep)
        .def("getStats", &PerformanceMonitor::getStats)
        .def("reset", &PerformanceMonitor::reset);
    
    // Simulation context manager
    py::class_<BrainSimulationManager>(m, "SimulationContext")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<AgentBrain>, 
                     std::shared_ptr<SimpleWorld>, bool, size_t>())
        .def("simulate", &BrainSimulationManager::simulate)
        .def("getStats", &BrainSimulationManager::getStats)
        .def("getBrain", &BrainSimulationManager::getBrain)
        .def("getAgent", &BrainSimulationManager::getAgent)
        .def("getWorld", &BrainSimulationManager::getWorld);
    
    // Configuration presets
    m.def("createMiniBrainConfig", []() {
        return AdvancedConfig::createMiniBrain();
    });
    
    m.def("createStandardBrainConfig", []() {
        return AdvancedConfig::createStandardBrain();
    });
    
    m.def("createAdvancedBrainConfig", []() {
        return AdvancedConfig::createAdvancedBrain();
    });
    
    // Experimental features
    auto experimental = m.def_submodule("experimental", "Experimental features for advanced users");
    
    py::class_<experimental::SynapticManipulation>(experimental, "SynapticManipulation")
        .def_static("strengthenConnections", &experimental::SynapticManipulation::strengthenConnections)
        .def_static("createPattern", &experimental::SynapticManipulation::createPattern);
    
    py::class_<experimental::CustomPlasticity>(experimental, "CustomPlasticity")
        .def_static("applyCustomSTDP", &experimental::CustomPlasticity::applyCustomSTDP);
    
    py::class_<experimental::AdvancedMemory>(experimental, "AdvancedMemory")
        .def_static("replayExperience", &experimental::AdvancedMemory::replayExperience);
    
    // Module attributes
    m.attr("version") = "2.0.0-enhanced";
}