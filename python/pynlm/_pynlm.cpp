// Main bindings entry point for NLM Python bindings
// This file includes all binding functions from modular binding files

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <memory>

// Forward declarations for all NLM classes
namespace nlm {
    class Config;
    class Brain;
    class AgentBrain;
    class SimpleWorld;
    class SensoryInput;
    class Vision;
    class Audio;
    class InternalSignals;
    class SensoryPercept;
    class Action;
    class WorldObject;
    class AgentBody;
    class Observation;
    
    // Enums
    enum class NeuronType;
    enum class SynapseType;
    enum class DevelopmentalStage;
    enum class FiringState;
    enum class ActionType;
    enum class MotorCommand;
    enum class WorldObjectType;
    
    // Type aliases
    using NeuronId = uint64_t;
    using SynapseId = uint64_t;
    using RegionId = uint64_t;
    using PopulationId = uint64_t;
    
    // Constants
    extern const NeuronId INVALID_NEURON_ID;
    extern const SynapseId INVALID_SYNAPSE_ID;
    extern const RegionId INVALID_REGION_ID;
    extern const PopulationId INVALID_POPULATION_ID;
}

// Forward declare binding functions
void bindCoreTypes(py::module& m);
void bindConfigClass(py::module& m);
void bindBrainClass(py::module& m);
void bindAgentBrainClass(py::module& m);
void bindSimpleWorldClass(py::module& m);
void bindSensoryInputClasses(py::module& m);
void bindActionClasses(py::module& m);
void bindGetAgentStats(py::module& m);

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    // Register exception types
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Bind all modules using modular binding functions
    bindCoreTypes(m);
    bindConfigClass(m);
    bindBrainClass(m);
    bindAgentBrainClass(m);
    bindSimpleWorldClass(m);
    bindSensoryInputClasses(m);
    bindActionClasses(m);

    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<nlm::Config> {
        return std::make_shared<nlm::Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<nlm::Config> config) -> std::shared_ptr<nlm::Brain> {
        return std::make_shared<nlm::Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<nlm::SimpleWorld> {
        return std::make_shared<nlm::SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<nlm::Brain> brain) -> std::shared_ptr<nlm::AgentBrain> {
        return std::make_shared<nlm::AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Advanced monitoring function
    bindGetAgentStats(m);

    // Module constants
    m.attr("INVALID_NEURON_ID") = py::cast(nlm::INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(nlm::INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(nlm::INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(nlm::INVALID_POPULATION_ID);
}
