# Improved Python bindings for NLM with better error handling

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

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

// Enhanced Python exception mapping
class ConfigError : public std::runtime_error {
public:
    ConfigError(const std::string& msg) : std::runtime_error("Config error: " + msg) {}
};

class BrainError : public std::runtime_error {
public:
    BrainError(const std::string& msg) : std::runtime_error("Brain error: " + msg) {}
};

class WorldError : public std::runtime_error {
public:
    WorldError(const std::string& msg) : std::runtime_error("World error: " + msg) {}
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        ---------------------------------------------
        Enhanced Python binding for the NLM C++ neural simulation framework.
        
        Key improvements:
        - Better error handling with Python exceptions
        - Enhanced configuration validation
        - Improved API documentation
    )pbdoc";

    // Register custom exceptions
    py::register_exception<ConfigError>(m, "ConfigError");
    py::register_exception<BrainError>(m, "BrainError");
    py::register_exception<WorldError>(m, "WorldError");

    // Enhanced Config class with better error handling
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a file (simple key=value format)")
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size() + 1);
            argv.push_back(nullptr);
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size() - 1), argv.data());
        }, py::arg("args"),
             "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get", [](const Config& self, const std::string& key) {
            auto value = self.get<double>(key);
            if (value.has_value()) {
                return py::float_(value.value());
            } else {
                auto str_value = self.get<std::string>(key);
                if (str_value.has_value()) {
                    return py::str(str_value.value());
                } else {
                    auto int_value = self.get<int64_t>(key);
                    if (int_value.has_value()) {
                        return py::int_(int_value.value());
                    } else {
                        auto bool_value = self.get<bool>(key);
                        if (bool_value.has_value()) {
                            return py::bool_(bool_value.value());
                        } else {
                            throw ConfigError("Key not found: " + key);
                        }
                    }
                }
            }
        }, py::arg("key"),
             "Get a configuration value (returns appropriate Python type)")
        .def("getOr", [](const Config& self, const std::string& key, py::object default_value) {
            if (default_value.is(py::int_())) {
                auto value = self.get<int64_t>(key);
                return py::int_(value.value_or(default_value.cast<int64_t>()));
            } else if (default_value.is(py::float_())) {
                auto value = self.get<double>(key);
                return py::float_(value.value_or(default_value.cast<double>()));
            } else if (default_value.is(py::str())) {
                auto value = self.get<std::string>(key);
                return py::str(value.value_or(default_value.cast<std::string>()));
            } else if (default_value.is(py::bool_())) {
                auto value = self.get<bool>(key);
                return py::bool_(value.value_or(default_value.cast<bool>()));
            } else {
                throw ConfigError("Invalid default value type for key: " + key);
            }
        }, py::arg("key"), py::arg("default_value"),
             "Get a configuration value or return default if not found")
        .def("set", [](Config& self, const std::string& key, py::object value) {
            if (value.is(py::int_())) {
                self.set(key, value.cast<int64_t>(), ConfigSource::Runtime);
            } else if (value.is(py::float_())) {
                self.set(key, value.cast<double>(), ConfigSource::Runtime);
            } else if (value.is(py::str())) {
                self.set(key, value.cast<std::string>(), ConfigSource::Runtime);
            } else if (value.is(py::bool_())) {
                self.set(key, value.cast<bool>(), ConfigSource::Runtime);
            } else {
                throw ConfigError("Invalid value type for key: " + key);
            }
        }, py::arg("key"), py::arg("value"),
             "Set a configuration value (accepts Python int, float, str, or bool)")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("validate", &Config::validate,
             "Validate the configuration (returns True if valid)")
        .def("getValidationErrors", &Config::getValidationErrors,
             "Get validation errors as a list of strings")
        .def("summary", &Config::summary,
             "Get a string summary of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config keys=" + std::to_string(cfg.getKeys().size()) + ">";
        });

    // Enhanced Brain class with better error handling
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        .def_property("neuron_count", &Brain::getTotalNeuronCount,
                     "Get total neuron count")
        .def_property("synapse_count", &Brain::getTotalSynapseCount,
                     "Get total synapse count")
        .def_property("firing_count", &Brain::getFiringNeuronCount,
                     "Get firing neuron count")
        .def_property("average_firing_rate", &Brain::getAverageFiringRate,
                     "Get average firing rate")
        .def_property("total_spikes", &Brain::getTotalSpikeCount,
                     "Get total spike count");

    // Enhanced AgentBrain class with better error handling
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        .def_property("neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
                     "Get neuromodulation level")
        .def_property("curiosity_level", &AgentBrain::getCuriosityLevel,
                     "Get curiosity level")
        .def_property("novelty_level", &AgentBrain::getNoveltyLevel,
                     "Get novelty level")
        .def_property("prediction_error", &AgentBrain::getPredictionError,
                     "Get prediction error");

    // Convenience factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration")
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration")
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world")
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Invalid ID constants for convenience
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Example usage and documentation
    m.def("example_basic_usage", []() {
        py::print("=== NLM Python Basic Usage Example ===");
        py::print("""# Create a brain and world
brain = nlm.createBrain()
world = nlm.createSimpleWorld()
agent = nlm.createAgentBrain(brain)

# Initialize
brain.initialize()
agent.initialize(world)

# Run a simulation step
for step in range(100):
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    for _ in range(10):  # 10 neural steps per action
        brain.step(step)
    cmd = agent.decodeMotorCommand()
    result = world.applyMotorCommand(cmd, world.getSimulationTime())
    agent.applyRewardModulation(result.reward, 0.0)
    world.update(0.1)

print(f"Simulation completed: {brain.getTotalSpikeCount()} spikes")""";
    }, "Example of basic NLM usage");
    
    m.def("example_config_usage", []() {
        py::print("=== NLM Python Configuration Example ===");
        py::print("""# Create and configure a configuration
config = nlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("region_count", 2)
config.set("random_seed", 42)
config.set("simulation_timestep", 0.001)

# Save configuration
config.saveToFile("my_config.cfg")

# Load configuration
config2 = nlm.createDefaultConfig()
config2.loadFromFile("my_config.cfg")

# Get configuration values
print(f"Neurons: {config2.get('neuron_count')}")
print(f"Regions: {config2.getOr('region_count', 1)}")""");
    }, "Example of configuration usage");

    // Add module metadata
    m.attr("__version__") = "0.1.0";
    m.attr("__description__") = "Enhanced Python bindings for NLM neural simulation framework";
    m.attr("__author__") = "NLM Project";
    m.attr("__license__") = "MIT";
}

} // namespace nlm