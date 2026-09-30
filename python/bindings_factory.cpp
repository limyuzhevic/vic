#include <pybind11/pybind11.h>
#include <memory>
#include <string>

#include "../src/core/Config/Config.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/core/Types/Types.hpp"

namespace py = pybind11;
namespace nlm {

void bindFactory(py::module_ &m) {
    // Factory functions for creating NLM objects
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration")
    
    .def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration")
    
    .def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world")
    
    .def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface")
    
    // Configuration factory methods
    .def("createConfigFromFile", [](const std::string& filepath) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            throw std::runtime_error("Failed to load configuration from: " + filepath);
        }
        return config;
    }, py::arg("filepath"), "Create config from file")
    
    .def("createConfigFromDict", [](const py::dict& d) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        for (auto it : d) {
            std::string key = it.first.cast<std::string>();
            py::object value = it.second;
            
            if (value.is(py::int_())) {
                config->set(key, value.cast<int64_t>());
            } else if (value.is(py::float_())) {
                config->set(key, value.cast<double>());
            } else if (value.is(py::bool_())) {
                config->set(key, value.cast<bool>());
            } else {
                config->set(key, value.cast<std::string>());
            }
        }
        return config;
    }, py::arg("d"), "Create config from Python dictionary")
    
    // Quick creation functions for common use cases
    .def("create_demo_brain", [](int neuron_count = 500) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(neuron_count));
        config->set("brain.region_count", static_cast<int64_t>(1));
        config->set("random_seed", static_cast<int64_t>(42));
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("neuron_count") = 500, "Create a demo brain with default settings")
    
    .def("create_demo_agent", [](int world_size = 20, int steps = 100) {
        // This returns a tuple that makes it easy to use in examples
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(500));
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(world_size, world_size, 8, 8);
        world->reset();
        world->setAgentStart(world_size/2.0f, world_size/2.0f);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        return py::make_tuple(brain, world, agent, steps);
    }, py::arg("world_size") = 20, py::arg("steps") = 100,
        "Create a complete demo setup (brain, world, agent, steps)")
    
    // Error handling helper
    .def("check_dependencies", []() {
        return py::dict(
            "pybind11_version", PYBIND11_VERSION,
            "python_version", PYBIND11_VERSION_HEX,
            "status", "ready"
        );
    }, "Check Python binding dependencies and status");
}

void bindVersion(py::module_ &m) {
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Authors";
    m.attr("__description__") = "Python bindings for NLM neural simulation framework";
    m.attr("__license__") = "MIT";
    m.attr("__website__") = "https://github.com/nlm-project/nlm";
    
    // Version info dictionary
    m.def("get_version_info", []() {
        return py::dict(
            "version", m.attr("__version__"),
            "author", m.attr("__author__"),
            "description", m.attr("__description__"),
            "license", m.attr("__license__"),
            "website", m.attr("__website__")
        );
    }, "Get comprehensive version information");
    
    // Compatibility check
    m.def("check_compatibility", []() {
        return py::dict(
            "minimum_python_version", "3.8",
            "requires", std::vector<std::string>{"numpy>=1.20", "pybind11"},
            "status", "compatible"
        );
    }, "Check Python compatibility requirements");
}
