// NLM Python Bindings - Main Module Entry Point
// This file contains the main module definition and delegates to specialized binding functions

#include "exceptions.h"
#include "core_identifiers.h"
#include "enums.h"
#include "config_class.h"
#include "sensory_input.h"
#include "action_world_object.h"
#include "agent_body_result.h"
#include "percept.h"
#include "world_class.h"
#include "brain_class.h"
#include "agent_brain_class.h"
#include "factory_functions.h"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A comprehensive Python interface to the NLM C++ neural simulation framework.
        
        This module provides access to all NLM functionality including:
        - Neural brain simulation with real LIF dynamics
        - Multiple plasticity mechanisms (STDP, Hebbian, structural)
        - Integrated memory systems
        - Prediction and cognitive systems
        - Neuromodulation (dopamine, curiosity, novelty)
        - Developmental stages
        - Sensorimotor interfaces
        - World simulation
        
        The bindings are organized into logical groups:
        - Core identifier types (NeuronId, SynapseId, RegionId, PopulationId)
        - Enum types (NeuronType, ActionType, etc.)
        - Configuration system
        - Sensory input classes (Vision, Audio, InternalSignals)
        - Action and world object classes
        - Agent body and result classes
        - Percept class
        - World simulation class
        - Brain simulation class with subsystem accessors
        - Agent brain interface class
        - Factory functions for object creation
        
        Usage examples are provided in the documentation of each class.
    )pbdoc";

    // Register custom NLM exceptions
    py::register_exception<nlm::NLMError>(m, "NLMError");
    py::register_exception<nlm::ConfigError>(m, "ConfigError");
    py::register_exception<nlm::FileError>(m, "FileError");
    py::register_exception<nlm::SimulationError>(m, "SimulationError");
    py::register_exception<nlm::ValidationError>(m, "ValidationError");

    // Register exception for std::runtime_error
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Bind all functionality to the module
    bindCoreIdentifiers(m);
    bindEnumTypes(m);
    bindConfigClass(m);
    bindSensoryInputClasses(m);
    bindActionWorldObjectClasses(m);
    bindAgentBodyResultClasses(m);
    bindPerceptClass(m);
    bindWorldClass(m);
    bindBrainClass(m);
    bindAgentBrainClass(m);
    bindFactoryFunctions(m);
}

} // namespace nlm