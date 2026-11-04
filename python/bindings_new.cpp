#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

// Core types bindings
void bind_types(py::module_& m);
// Configuration bindings
void bind_config(py::module_& m);
// Brain bindings
void bind_brain(py::module_& m);
// Agent bindings
void bind_agent(py::module_& m);
// World bindings
void bind_world(py::module_& m);
// Sensory bindings
void bind_sensory(py::module_& m);
// Motor bindings
void bind_motor(py::module_& m);

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Key Features:
        - Neural network simulation with spiking neurons
        - Memory systems (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Developmental stages and plasticity
        - World interaction and sensory-motor loops
        - Learning and adaptation
        
        Quick Start:
        ```python
        import pynlm
        
        # Create a brain
        brain = pynlm.createBrain()
        brain.initialize()
        
        # Create a world
        world = pynlm.createSimpleWorld()
        world.configure(100, 100, 32, 32)
        
        # Run simulation
        for step in range(1000):
            # Get sensory input
            percept = world.getSensoryPercept()
            brain.receiveSensoryInput(percept)
            
            # Produce action
            action = brain.produceAction()
            world.applyMotorCommand(action)
        ```
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "IndexError");
    
    // Bind all subsystems
    bind_types(m);
    bind_config(m);
    bind_brain(m);
    bind_agent(m);
    bind_world(m);
    bind_sensory(m);
    bind_motor(m);
    
    // Module-level convenience functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");
    
    // Module constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}