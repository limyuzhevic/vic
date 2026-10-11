// Python bindings for NLM - Modular Structure

This file contains the main entry point for the modular Python bindings.
It includes all other binding files and sets up the main module.

The bindings are organized into separate files for better maintainability:
- core_bindings.cpp: Core utilities and types
- config_bindings.cpp: Configuration system
- brain_bindings.cpp: Main brain functionality
- agent_bindings.cpp: Agent-brain interface
- world_bindings.cpp: World simulation
- memory_bindings.cpp: Memory systems
- neuromodulation_bindings.cpp: Neuromodulation systems
- cognition_bindings.cpp: Cognitive functions
- plasticity_bindings.cpp: Plasticity rules
- development_bindings.cpp: Developmental system
- sensory_bindings.cpp: Sensory processing
- motor_bindings.cpp: Motor system
- prediction_bindings.cpp: Prediction system
- visualization_bindings.cpp: Visualization interfaces
- experiment_bindings.cpp: Experiment framework

Each binding file follows Pythonic conventions and includes proper documentation.
"""

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>

// Include all core headers
#include "../src/core/Types/Types.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// Global function declarations for Python
std::shared_ptr<Config> createDefaultConfig();
std::shared_ptr<Brain> createBrain(std::shared_ptr<Config> config);
std::shared_ptr<SimpleWorld> createSimpleWorld();
std::shared_ptr<AgentBrain> createAgentBrain(std::shared_ptr<Brain> brain);

// Enhanced exception types with additional context and chaining
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
    NLMError(const std::string& msg, const std::string& context) 
        : std::runtime_error(msg + " (context: " + context + ")") {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration error: " + msg, "configuration") {}
    ConfigurationError(const std::string& msg, const std::string& key) 
        : NLMError("Configuration error for key '" + key + "': " + msg, "configuration") {}
};

class SimulationError : public NLMError {
public:
    SimulationError(const std::string& msg, SimulationStep step) 
        : NLMError("Simulation error at step " + std::to_string(step) + ": " + msg, "simulation"),
          step_(step) {}
    
    SimulationStep getStep() const { return step_; }
    
private:
    SimulationStep step_;
};

class ValidationError : public NLMError {
public:
    explicit ValidationError(const std::string& msg) : NLMError("Validation error: " + msg, "validation") {}
};

class ResourceError : public NLMError {
public:
    explicit ResourceError(const std::string& msg) : NLMError("Resource error: " + msg, "resources") {}
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        
        A Python binding for the NLM C++ neural simulation framework.
        
        This module provides access to the full NLM artificial brain simulation
        system, including neurons, synapses, memory, learning, and cognition.
        
        Key Features:
        - Real spiking neural computation with LIF neurons
        - Event-driven dynamics with synaptic delays
        - Multiple plasticity mechanisms (STDP, Hebbian, structural)
        - Integrated memory systems (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Cognitive functions (planning, attention, concept formation)
        - Developmental system with critical periods
        - World interaction with sensory perception and motor control
        - Checkpoint persistence and replay mechanisms
        
        Installation:
            pip install pynlm
            
        Basic Usage:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> brain.step(0)
            
        The brain will process one simulation step, updating all neural dynamics,
        applying plasticity rules, and producing motor output.
        
        See the documentation for detailed examples and API reference.
        
        Copyright (c) 2024 NLM Project
        License: MIT
    )pbdoc";

    // Register exception types
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError);
    py::register_exception<ConfigurationError>(m, "ConfigurationError", PyExc_ValueError);
    py::register_exception<SimulationError>(m, "SimulationError", PyExc_RuntimeError);

    // Include all binding modules
    include_core_bindings(m);
    include_config_bindings(m);
    include_brain_bindings(m);
    include_agent_bindings(m);
    include_world_bindings(m);
    include_memory_bindings(m);
    include_neuromodulation_bindings(m);
    include_cognition_bindings(m);
    include_plasticity_bindings(m);
    include_development_bindings(m);
    include_sensory_bindings(m);
    include_motor_bindings(m);
    include_prediction_bindings(m);
    include_visualization_bindings(m);
    include_experiment_bindings(m);

    // Global factory functions
    m.def("createDefaultConfig", &createDefaultConfig,
          "Create a default configuration for the NLM system");

    m.def("createBrain", &createBrain, py::arg("config"),
          "Create a new brain with the given configuration");

    m.def("createSimpleWorld", &createSimpleWorld,
          "Create a new simple world for NLM simulation");

    m.def("createAgentBrain", &createAgentBrain, py::arg("brain"),
          "Create a new agent brain interface");

    // Version and utility functions
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Project";
    m.attr("__license__") = "MIT";

    // Python module metadata
    m.attr("__doc__") = R"pbdoc(
        NLM Python Bindings - Neural Learning Machine
        
        This module provides Python access to the NLM artificial brain simulation framework.
        
        The NLM (熙然, meaning "serene flow") is an experimental computational brain
        project that aims to create a neural system beginning in a primitive developmental
        state and acquiring increasingly complex abilities through interaction with an environment.
        
        For detailed documentation, see the project README and HOW_TO_USE.md files.
    )pbdoc";
}

// Include directive functions - these will be implemented in separate binding files
inline void include_core_bindings(py::module& m) {}
inline void include_config_bindings(py::module& m) {}
inline void include_brain_bindings(py::module& m) {}
inline void include_agent_bindings(py::module& m) {}
inline void include_world_bindings(py::module& m) {}
inline void include_memory_bindings(py::module& m) {}
inline void include_neuromodulation_bindings(py::module& m) {}
inline void include_cognition_bindings(py::module& m) {}
inline void include_plasticity_bindings(py::module& m) {}
inline void include_development_bindings(py::module& m) {}
inline void include_sensory_bindings(py::module& m) {}
inline void include_motor_bindings(py::module& m) {}
inline void include_prediction_bindings(py::module& m) {}
inline void include_visualization_bindings(py::module& m) {}
inline void include_experiment_bindings(py::module& m) {}

// Global function definitions
std::shared_ptr<Config> createDefaultConfig() {
    return std::make_shared<Config>();
}

std::shared_ptr<Brain> createBrain(std::shared_ptr<Config> config) {
    return std::make_shared<Brain>(config);
}

std::shared_ptr<SimpleWorld> createSimpleWorld() {
    return std::make_shared<SimpleWorld>();
}

std::shared_ptr<AgentBrain> createAgentBrain(std::shared_ptr<Brain> brain) {
    return std::make_shared<AgentBrain>(brain);
}
