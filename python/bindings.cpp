#include <pybind11/pybind11.h>
#include "../python/nlm/core/binding_helpers.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        This module provides access to neural brain simulation, agent control,
        and world simulation capabilities through a clean, Pythonic API.
        
        Key Features:
        - Brain simulation with configurable neural networks
        - Agent-based control with sensory processing
        - World simulation with physics and interactions
        - Type safety with strong validation
        - Advanced APIs for experienced users
        - NumPy integration for data processing
        - Comprehensive error handling with custom exceptions
        
        Quick Start Example:
            import pynlm as nlm
            
            # Create a brain with default configuration
            brain = nlm.Brain(nlm.createDefaultConfig())
            brain.initialize()
            
            # Create a world
            world = nlm.SimpleWorld(100, 100, 10, 10)
            world.setAgentStart(50, 50)
            
            # Create agent brain interface
            agent_brain = nlm.AgentBrain(brain)
            agent_brain.initialize(world)
            
            # Run simulation
            for step in range(100):
                percept = world.getSensoryPercept()
                agent_brain.processSensoryInput(percept)
                action = agent_brain.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                world.update(0.01)  # 10ms timestep
                
        Advanced Features:
            - Batch operations for performance
            - Neural network visualization utilities
            - NumPy integration for data processing
            - Custom exception classes for error handling
            - Type validation and safety checks
    )pbdoc";

    // Register custom exception classes
    py::register_exception<nlm::NLMError>(m, "NLMError");
    py::register_exception<nlm::ConfigError>(m, "ConfigError");
    py::register_exception<nlm::BrainError>(m, "BrainError");
    py::register_exception<nlm::WorldError>(m, "WorldError");

    // Include all binding modules
    bindBrainTypes(m);
    bindBrainClasses(m);
    bindBrainFactories(m);
    bindBrainAdvanced(m);
    bindBrainErrors(m);

    // Invalid ID sentinels for convenience
    m.attr("INVALID_NEURON_ID") = py::cast(nlm::INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(nlm::INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(nlm::INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(nlm::INVALID_POPULATION_ID);
}

} // namespace nlm