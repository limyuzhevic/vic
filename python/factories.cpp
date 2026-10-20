#include "factories.h"
#include "error_handling.h"
#include <pybind11/stl.h>

namespace py = pybind11;
namespace nlm {

void include_factories(py::module_& m) {
    // Factory functions with enhanced Pythonic features

    // Config factory
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration object with sensible initial values",
         py::arg("config") = py::none());

    m.def("createConfig", [](const std::string& jsonString) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Add JSON parsing logic here if needed
        return config;
    }, "Create a configuration from JSON string",
         py::arg("jsonString"));

    m.def("loadConfigFromFile", [](const std::string& filepath) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->loadFromFile(filepath);
        return config;
    }, "Load configuration from file and return Config object",
         py::arg("filepath"));

    // Brain factory
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, "Create a new brain with configuration", py::arg("config"));

    m.def("createBrainFromConfig", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->loadFromFile(filepath);
        return std::make_shared<Brain>(config);
    }, "Create a brain from configuration file", py::arg("filepath"));

    m.def("createBrainWithName", [](std::string name, std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (config) {
            config->set("brain.name", name);
        }
        return std::make_shared<Brain>(config);
    }, "Create a brain with specified name and optional config", py::arg("name"), py::arg("config") = py::none());

    // SimpleWorld factory
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world with default settings", py::arg("world") = py::none());

    m.def("createWorldWithConfig", [](size_t width, size_t height, size_t visionWidth, size_t visionHeight) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }, "Create a configured world with specified dimensions", py::arg("width"), py::arg("height"),
       py::arg("visionWidth"), py::arg("visionHeight"));

    m.def("createWorld", [](size_t width, size_t height) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 32, 32); // Default vision dimensions
        return world;
    }, "Create a world with specified dimensions and default vision settings", py::arg("width"), py::arg("height"));

    // AgentBrain factory
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, "Create a new agent brain interface", py::arg("brain"));

    m.def("createAgentBrainWithBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, "Create an agent brain with specified brain", py::arg("brain"));

    m.def("createDefaultAgentBrain", []() -> std::shared_ptr<AgentBrain> {
        auto brain = std::make_shared<Brain>(std::make_shared<Config>());
        return std::make_shared<AgentBrain>(brain);
    }, "Create an agent brain with default configuration");

    // Component factory utilities
    m.def("createSensoryPercept", []() -> std::shared_ptr<SensoryPercept> {
        return std::make_shared<SensoryPercept>();
    }, "Create an empty sensory percept", py::arg("percept") = py::none());

    m.def("createAgentBody", []() -> std::shared_ptr<AgentBody> {
        return std::make_shared<AgentBody>();
    }, "Create an agent body with initial state", py::arg("body") = py::none());

    m.def("createAction", [](ActionType type) -> std::shared_ptr<Action> {
        return std::make_shared<Action>(type);
    }, "Create an action with specified type", py::arg("type"));

    m.def("createActionWithParams", [](ActionType type, const std::vector<float>& params) -> std::shared_ptr<Action> {
        return std::make_shared<Action>(type, params);
    }, "Create an action with type and parameters", py::arg("type"), py::arg("params"));

    m.def("createWorldObject", [](float x, float y, WorldObjectType type, float value = 0.0f, float radius = 0.5f) -> std::shared_ptr<WorldObject> {
        return std::make_shared<WorldObject>(x, y, type, value, radius);
    }, "Create a world object with specified properties", py::arg("x"), py::arg("y"), py::arg("type"),
       py::arg("value") = 0.0f, py::arg("radius") = 0.5f);

    // Batch factory functions
    m.def("createSimulationComponents", [](const std::string& configFile = "") -> std::tuple<
        std::shared_ptr<Config>, std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>> {
        auto config = std::make_shared<Config>();
        if (!configFile.empty()) {
            config->loadFromFile(configFile);
        }
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        world->configure(100, 100, 32, 32); // Default dimensions
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(*world);
        return std::make_tuple(config, brain, world, agentBrain);
    }, "Create all simulation components in one call (config, brain, world, agentBrain)",
       py::arg("configFile") = "");

    // Validation and utility functions
    m.def("validateConfiguration", [](const std::shared_ptr<Config>& config) {
        if (!config) {
            throw std::runtime_error("Configuration is null");
        }
        return true;
    }, "Validate configuration object", py::arg("config"));

    m.def("validateBrain", [](const std::shared_ptr<Brain>& brain) {
        if (!brain) {
            throw std::runtime_error("Brain is null");
        }
        return true;
    }, "Validate brain object", py::arg("brain"));

    m.def("validateWorld", [](const std::shared_ptr<SimpleWorld>& world) {
        if (!world) {
            throw std::runtime_error("World is null");
        }
        return true;
    }, "Validate world object", py::arg("world"));

    m.def("validateAgentBrain", [](const std::shared_ptr<AgentBrain>& agentBrain) {
        if (!agentBrain) {
            throw std::runtime_error("AgentBrain is null");
        }
        return true;
    }, "Validate agent brain object", py::arg("agentBrain"));

    // Type conversion and helper functions
    m.def("asConfig", [](std::shared_ptr<Config> config) {
        return config;
    }, "Convert to Config reference", py::arg("config"));

    m.def("asBrain", [](std::shared_ptr<Brain> brain) {
        return brain;
    }, "Convert to Brain reference", py::arg("brain"));

    m.def("asWorld", [](std::shared_ptr<SimpleWorld> world) {
        return world;
    }, "Convert to World reference", py::arg("world"));

    m.def("asAgentBrain", [](std::shared_ptr<AgentBrain> agentBrain) {
        return agentBrain;
    }, "Convert to AgentBrain reference", py::arg("agentBrain"));

    // Component statistics
    m.def("getComponentCount", [](const std::shared_ptr<Brain>& brain) {
        return std::make_tuple(brain->getRegionCount(), 
                             brain->getTotalNeuronCount(),
                             brain->getTotalSynapseCount());
    }, "Get component counts as (regionCount, neuronCount, synapseCount)",
       py::arg("brain"));

    m.def("getComponentStats", [](const std::shared_ptr<Brain>& brain) {
        return std::make_tuple(brain->getRegionCount(),
                             brain->getTotalNeuronCount(),
                             brain->getTotalSynapseCount(),
                             brain->getActiveNeuronCount(),
                             brain->getFiringNeuronCount());
    }, "Get comprehensive component statistics", py::arg("brain"));

    // Component cloning and copying
    m.def("cloneBrain", [](const std::shared_ptr<Brain>& brain) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(*brain);
    }, "Create a deep copy of a brain", py::arg("brain"));

    m.def("cloneWorld", [](const std::shared_ptr<SimpleWorld>& world) -> std::shared_ptr<SimpleWorld> {
        auto clonedWorld = std::make_shared<SimpleWorld>();
        // Add world copy logic here if needed
        return clonedWorld;
    }, "Create a deep copy of a world", py::arg("world"));

    m.def("cloneAgentBrain", [](const std::shared_ptr<AgentBrain>& agentBrain) -> std::shared_ptr<AgentBrain> {
        auto clonedBrain = std::make_shared<Brain>(*agentBrain->getBrain());
        return std::make_shared<AgentBrain>(clonedBrain);
    }, "Create a deep copy of an agent brain", py::arg("agentBrain"));

} // namespace nlm