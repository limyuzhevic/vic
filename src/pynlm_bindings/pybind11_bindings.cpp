// pybind11_bindings.cpp - Main enhanced Python bindings module
// Modular and comprehensive bindings for NLM

#include "pybind11_bindings.h"
#include "pybind11_basic_types.h"
#include "../src/core/Config/Config.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        --------------------------------------------------------
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        
        This module provides enhanced Python APIs for all NLM components with:
        - Full type safety and validation
        - Comprehensive documentation
        - Pythonic interfaces with intuitive method names
        - Error handling and validation
        - Support for complex object interactions
        
        Key features:
        - Advanced configuration system
        - Complete sensory processing pipeline
        - World simulation and interaction
        - Brain simulation with neuromodulation
        - Agent-based behavior modeling
        - Development and learning capabilities
        - Statistical analysis and metrics
        - File I/O and persistence
        
        For usage examples and detailed API documentation, see:
        - docs/ARCHITECTURE.md
        - docs/SCIENCE.md
        - HOW_TO_USE.md
        - easy_usage.md
    )pbdoc";

    // Register exceptions
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");

    // Bind basic types first (dependencies for other bindings)
    bind_basic_types(m);

    // Import and bind all other modules
    bind_config_module(m);
    bind_sensory_module(m);
    bind_motor_module(m);
    bind_world_module(m);
    bind_brain_module(m);
    bind_agent_module(m);

    // Add factory functions for enhanced Python API
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with validation")
    .def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            throw py::value_error("Config cannot be null");
        }
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration (config is required)")
    .def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world")
    .def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw py::value_error("Brain cannot be null");
        }
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface (brain is required)")
    .def("validate_config", [](const std::shared_ptr<Config>& config) -> bool {
        if (!config) {
            throw py::value_error("Config cannot be null");
        }
        return config->has("brain.neuron_count") && 
               config->has("brain.synapse_density");
    }, py::arg("config"), "Validate configuration (checks for required keys)")
    .def("get_version", []() {
        return std::string("NLM 0.1.0 - Phase 2 Enhanced Bindings");
    }, "Get NLM version and binding information")
    .def("get_supported_neuron_types", []() {
        std::vector<std::string> types;
        for (int i = 0; i < 7; ++i) {
            types.push_back(std::to_string(i));
        }
        return types;
    }, "Get supported neuron type indices")
    .def("get_supported_action_types", []() {
        std::vector<std::string> actions;
        actions.push_back("MoveForward");
        actions.push_back("MoveBackward");
        actions.push_back("TurnLeft");
        actions.push_back("TurnRight");
        actions.push_back("Look");
        actions.push_back("Interact");
        actions.push_back("Wait");
        return actions;
    }, "Get supported action type names");

    // Add module metadata
    m.attr("__version__") = "2.0.0-enhanced";
    m.attr("__description__") = "NLM Neural Learning Machine - Enhanced Python Bindings";
    m.attr("__author__") = "NLM Research Team";
    m.attr("__license__") = "MIT";
    m.attr("__copyright__") = "Copyright (c) 2024 NLM Project";

    // Add utility functions
    m.def("simulate_step", [](std::shared_ptr<Brain> brain, size_t step) {
        if (!brain) {
            throw py::value_error("Brain cannot be null");
        }
        brain->step(step);
        return brain->getFiringNeuronCount();
    }, py::arg("brain"), py::arg("step"), 
         "Run a single simulation step and return firing neuron count")
    .def("reset_simulation", [](std::shared_ptr<Brain> brain) {
        if (!brain) {
            throw py::value_error("Brain cannot be null");
        }
        brain->reset();
        return true;
    }, py::arg("brain"), "Reset brain state and return success status")
    .def("get_simulation_stats", [](const std::shared_ptr<Brain>& brain) {
        if (!brain) {
            throw py::value_error("Brain cannot be null");
        }
        py::dict stats;
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        stats["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        return stats;
    }, py::arg("brain"), "Get comprehensive simulation statistics as a dictionary");
}

} // namespace nlm
