#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

// Include all modular binding headers
#include "core_types.cpp"
#include "enums.cpp"
#include "config_module.cpp"
#include "sensory_module.cpp"
#include "brain_module.cpp"
#include "agent_module.cpp"
#include "world_module.cpp"
#include "advanced_apis.cpp"
#include "error_handling.cpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action,
        plus advanced APIs for experiment management, visualization, and error handling.
    )pbdoc";

    // Register custom exceptions for better error handling
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Bind all modules
    bind_core_types(m);
    bind_enumerations(m);
    bind_config(m);
    bind_sensory(m);
    bind_brain(m);
    bind_agent(m);
    bind_world(m);
    bind_advanced_apis(m);
    bind_error_handling(m);

    // Export convenience factory functions
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

    // Export constants for backward compatibility
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Advanced utility functions
    m.def("run_quick_test", [](std::shared_ptr<Brain> brain) {
        // Quick test to verify brain is working
        auto region = brain->getRegion(RegionId(1));
        if (region) {
            auto neurons = region->getAllNeurons();
            if (!neurons.empty()) {
                neurons[0]->injectCurrent(50.0f);
                brain->step(0, 0.001);
                return true;
            }
        }
        return false;
    }, py::arg("brain"), "Run a quick test to verify brain functionality");

    m.def("get_system_info", []() {
        py::dict info;
        info["version"] = "0.1.0";
        info["framework"] = "NLM (Neural Learning Machine)";
        info["description"] = "Experimental artificial developmental brain";
        info["phase"] = "6 - Final Integration";
        return info;
    }, "Get system information and version details");
}

} // namespace nlm
