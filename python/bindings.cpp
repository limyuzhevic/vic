#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <memory>
#include <string>

// Include all modular binding files
#include "ids.hpp"
#include "enums.hpp"
#include "basic_classes.hpp"
#include "sensory_classes.hpp"
#include "world_classes.hpp"
#include "brain_classes.hpp"

namespace py = pybind11;

namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Register all enums first
    register_enums(m);

    // Configure all classes in logical groups
    configureBasicClasses(m);
    configureSensoryClasses(m);
    configureWorldClasses(m);
    configureBrainClasses(m);

    // Register module attributes (invalid IDs)
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Register factory functions
    m.def("createDefaultConfig", &createDefaultConfig,
          "Create a default configuration");

    m.def("createBrain", &createBrain, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", &createSimpleWorld,
          "Create a new simple world");

    m.def("createAgentBrain", &createAgentBrain, py::arg("brain"), "Create a new agent brain interface");

    // Additional utility functions
    m.def("version", []() { return "0.1.0"; }, "Get library version");

    // Quick reference helper
    m.def("quick_start", []() {
        return R"(
        Quick Start Guide:
        -----------------
        1. Create a brain: brain = pynlm.createBrain(pynlm.createDefaultConfig())
        2. Initialize it: brain.initialize()
        3. Run steps: for i in range(100): brain.step(i)
        
        This is equivalent to: brain = pynlm.createBrain(pynlm.createDefaultConfig()); brain.initialize(); 
        for i in range(100): brain.step(i)
        )";
    }, "Get quick start guide");
}

} // namespace nlm
