// Convenience functions implementation
#include "NLMPythonConvenience.h"
#include "NLMPythonTypes.h"

void registerConvenienceFunctions(py::module_& m) {
    // Default configuration creation
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    // Brain creation
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    // Simple world creation
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    // Agent brain creation
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Version information
    m.attr("__version__") = "0.1.0";
    
    // Platform information
    m.def("getPlatformInfo", []() {
        std::map<std::string, std::string> info;
        info["author"] = "NLM Authors";
        info["description"] = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework";
        info["license"] = "MIT";
        return info;
    });
}
