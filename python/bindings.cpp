#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

// Main bindings file for NLM Python bindings - modular structure
// Includes all modules from separate files for better organization and maintainability

// Forward declarations for global constants (to be declared in headers)
extern const nlm::NeuronId INVALID_NEURON_ID;
extern const nlm::SynapseId INVALID_SYNAPSE_ID;
extern const nlm::RegionId INVALID_REGION_ID;
extern const nlm::PopulationId INVALID_POPULATION_ID;

// Include headers for global constants
#include "../src/core/Types/Types.hpp"

// Module initialization function declarations
void init_core_types(py::module_ &m);
void init_config(py::module_ &m);
void init_sensory(py::module_ &m);
void init_motor(py::module_ &m);
void init_world(py::module_ &m);
void init_agent(py::module_ &m);
void init_brain(py::module_ &m);

namespace py = pybind11;

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Initialize all modules
    init_core_types(m);
    init_config(m);
    init_sensory(m);
    init_motor(m);
    init_world(m);
    init_agent(m);
    init_brain(m);

    // Factory functions
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

    // Global constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}
