#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <memory>

#include "../src/core/Config/Config.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/agent/AgentBrain.hpp"

// Forward declarations for registration functions
namespace nlm {
    void register_id_types(pybind11::module_& m);
    void register_enums(pybind11::module_& m);
    void register_config(pybind11::module_& m);
    void register_sensory(pybind11::module_& m);
    void register_state(pybind11::module_& m);
    void register_world(pybind11::module_& m);
    void register_brain(pybind11::module_& m);
    void register_agent(pybind11::module_& m);
}

namespace py = pybind11;
namespace nlm {

// main_bindings.cpp - Main module setup and convenience functions
// Purpose: Contains the main PYBIND11_MODULE definition and convenience factory functions

PYBIND11_MODULE(pynlm, m) {

    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides comprehensive classes and functions for neural simulation,
        including brain modeling, sensory processing, motor control, and learning.
        
        Key Components:
        - Brain: Central neural simulation engine with neurons and synapses
        - Config: Configuration management for brain parameters
        - AgentBrain: Interface connecting brain to environment
        - SimpleWorld: 2D simulation environment
        - SensoryInput: Base class for sensory modalities (Vision, Audio, InternalSignals)
        - Action: Motor actions and behaviors
        - Neural identifiers: NeuronId, SynapseId, RegionId, PopulationId
        - Enumerations: NeuronType, SynapseType, DevelopmentalStage, etc.
        
        The NLM brain operates as a closed-loop system with neural dynamics,
        learning, memory, prediction, and developmental capabilities.
        
        Examples:
            >>> import nlm
            >>> config = nlm.createDefaultConfig()
            >>> brain = nlm.createBrain(config)
            >>> world = nlm.createSimpleWorld()
            >>> agent = nlm.createAgentBrain(brain)
            >>> brain.initialize()
            >>> # Run simulation...
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Register all binding modules
    register_id_types(m);
    register_enums(m);
    register_config(m);
    register_sensory(m);
    register_state(m);
    register_world(m);
    register_brain(m);
    register_agent(m);

    // Convenience factory functions with enhanced documentation
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, R"pbdoc(
        Create a default configuration for the NLM brain.
        
        Returns:
            Config: A new configuration object with default settings.
                   Typical defaults include: neuron_count=100, region_count=1,
                   connection_probability=0.1, etc.
        
        Examples:
            >>> config = nlm.createDefaultConfig()
            >>> config.get("neuron_count")
            100
        )pbdoc");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), R"pbdoc(
        Create a new brain with the given configuration.
        
        Args:
            config: Configuration object controlling brain parameters like
                   neuron count, connectivity, and developmental settings.
        
        Returns:
            Brain: A new brain instance ready for simulation.
        
        Raises:
            ValueError: If config is None or invalid
        
        Examples:
            >>> config = nlm.createDefaultConfig()
            >>> config.set("neuron_count", 200)
            >>> brain = nlm.createBrain(config)
        )pbdoc");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(
        Create a new SimpleWorld simulation environment.
        
        Returns:
            SimpleWorld: A new world instance for agent-environment interaction.
                        Defaults to a 20x20 grid world with 14x14 vision range.
        
        Examples:
            >>> world = nlm.createSimpleWorld()
            >>> world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
            >>> # Configure world for exploration
        )pbdoc");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(
        Create a new AgentBrain interface connecting a brain to a world.
        
        Args:
            brain: The Brain instance to be controlled by this agent.
        
        Returns:
            AgentBrain: An interface that processes sensory input and generates
                       motor actions for the brain.
        
        Examples:
            >>> brain = nlm.createBrain(nlm.createDefaultConfig())
            >>> world = nlm.createSimpleWorld()
            >>> agent = nlm.createAgentBrain(brain)
            >>> agent.initialize(world)
        )pbdoc");

    // Global constant attributes for invalid IDs
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Additional module-level convenience functions
    m.def("get_version", []() -> std::string {
        return "0.1.0";
    }, R"pbdoc(
        Get the version of the NLM Python bindings.
        
        Returns:
            str: Version string in semantic versioning format.
        
        Examples:
            >>> version = nlm.get_version()
            '0.1.0'
        )pbdoc");

    m.def("get_build_info", []() -> py::dict {
        py::dict info;
        info["description"] = "Python bindings for NLM (Neural Learning Machine)";
        info["author"] = "NLM Authors";
        info["license"] = "MIT";
        info["python_binding_version"] = "1.0";
        info["cpp_standard"] = "C++20";
        return info;
    }, R"pbdoc(
        Get build and module information.
        
        Returns:
            dict: Dictionary containing module metadata including version,
                  author, license, and technical details.
        
        Examples:
            >>> info = nlm.get_build_info()
            >>> info["description"]
            'Python bindings for NLM (Neural Learning Machine)'
        )pbdoc");
}

} // namespace nlm