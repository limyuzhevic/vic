#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/core/Validator/Validator.hpp"

namespace py = pybind11;
namespace nlm {

// Helper functions for Python bindings
static std::string configToJson(const Config& config) {
    return config.summary();
}

static std::vector<std::string> getConfigKeys(const Config& config) {
    return config.getKeys();
}

static bool hasConfigKey(const Config& config, const std::string& key) {
    return config.has(key);
}

static double getConfigDouble(const Config& config, const std::string& key, double defaultValue = 0.0) {
    return config.getOr<double>(key, defaultValue);
}

static int64_t getConfigInt(const Config& config, const std::string& key, int64_t defaultValue = 0) {
    return config.getOr<int64_t>(key, defaultValue);
}

static std::string getConfigString(const Config& config, const std::string& key, const std::string& defaultValue = "") {
    return config.getOr<std::string>(key, defaultValue);
}

static bool getConfigBool(const Config& config, const std::string& key, bool defaultValue = false) {
    return config.getOr<bool>(key, defaultValue);
}

// Advanced Brain methods for Python bindings
static void brainSetConfigValue(Brain& brain, const std::string& key, const std::string& value) {
    brain.getConfig()->set(key, value);
}

static std::string brainGetConfigValue(Brain& brain, const std::string& key) {
    std::optional<std::string> valueOpt = brain.getConfig()->get<std::string>(key);
    return valueOpt ? *valueOpt : "";
}

static bool brainValidateConfig(Brain& brain) {
    auto& validator = getConfigValidator();
    auto errors = validator.validate(*brain.getConfig());
    return errors.empty();
}

static std::string brainGetValidationErrors(Brain& brain) {
    auto& validator = getConfigValidator();
    auto errors = validator.validate(*brain.getConfig());
    
    std::ostringstream oss;
    oss << "Configuration Validation Errors (" << errors.size() << "):\n";
    for (const auto& error : errors) {
        oss << "  " << error.key << ": " << error.message;
        if (!error.value.empty()) oss << " (value: " << error.value << ")";
        oss << "\n";
    }
    return oss.str();
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        This module provides access to the complete NLM neural simulation framework including:
        - Real spiking neural computation with LIF neurons
        - Event-driven spike propagation with synaptic delays
        - STDP and Hebbian plasticity
        - Structural plasticity (synaptogenesis/pruning)
        - Integrated memory, prediction, cognition, and neuromodulation systems
        - Configuration validation and parameter checking
        - Advanced logging and monitoring capabilities
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // NeuronId
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });

    // SynapseId
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    // RegionId
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    // PopulationId
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // NeuronType
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // SynapseType
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // DevelopmentalStage
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // ActionType
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward)
        .value("MoveBackward", ActionType::MoveBackward)
        .value("MoveLeft", ActionType::MoveLeft)
        .value("MoveRight", ActionType::MoveRight)
        .value("TurnLeft", ActionType::TurnLeft)
        .value("TurnRight", ActionType::TurnRight)
        .value("Look", ActionType::Look)
        .value("LookUp", ActionType::LookUp)
        .value("LookDown", ActionType::LookDown)
        .value("Interact", ActionType::Interact)
        .value("Eat", ActionType::Eat)
        .value("Drink", ActionType::Drink)
        .value("Rest", ActionType::Rest)
        .value("Wait", ActionType::Wait)
        .value("Custom", ActionType::Custom)
        .export_values();

    // MotorCommand
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    // WorldObjectType
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Config class with enhanced Python bindings
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration class for NLM system with JSON support)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file (preferred) or key=value format (backward compatible)")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file (formatted for readability)")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a formatted summary string of the configuration")
        
        // Enhanced Python-specific methods
        .def("to_dict", [](const Config& cfg) {
            std::map<std::string, std::string> result;
            auto keys = cfg.getKeys();
            for (const auto& key : keys) {
                // Try to get value as string, fallback to other types
                auto strVal = cfg.get<std::string>(key);
                if (strVal) {
                    result[key] = *strVal;
                } else {
                    auto intVal = cfg.get<int64_t>(key);
                    if (intVal) {
                        result[key] = std::to_string(*intVal);
                    } else {
                        auto doubleVal = cfg.get<double>(key);
                        if (doubleVal) {
                            result[key] = std::to_string(*doubleVal);
                        } else {
                            auto boolVal = cfg.get<bool>(key);
                            if (boolVal) {
                                result[key] = *boolVal ? "true" : "false";
                            }
                        }
                    }
                }
            }
            return result;
        }, "Convert configuration to Python dictionary")
        
        .def("from_dict", [](Config& cfg, const py::dict& d) {
            for (auto item : d) {
                std::string key = py::cast<std::string>(item.first);
                std::string value = py::cast<std::string>(item.second);
                cfg.set(key, value);
            }
            return cfg;
        }, py::arg("dictionary"), "Load configuration from Python dictionary")
        
        .def("get_or", [](const Config& cfg, const std::string& key, py::object defaultValue) {
            // Try different types in order
            if (defaultValue.is_none()) return cfg.getOr<std::string>(key, "");
            
            // Try to infer type from default value
            if (py::isinstance<py::int_>(defaultValue)) {
                return cfg.getOr<int64_t>(key, py::cast<int64_t>(defaultValue));
            } else if (py::isinstance<py::float_>(defaultValue)) {
                return cfg.getOr<double>(key, py::cast<double>(defaultValue));
            } else if (py::isinstance<py::bool_>(defaultValue)) {
                return cfg.getOr<bool>(key, py::cast<bool>(defaultValue));
            } else {
                return cfg.getOr<std::string>(key, py::cast<std::string>(defaultValue));
            }
        }, py::arg("key"), py::arg("default_value"),
            "Get configuration value with type inference")
        
        .def("set_int", [](Config& cfg, const std::string& key, int64_t value, const std::string& description = "") {
            cfg.set(key, value);
            return cfg;
        }, py::arg("key"), py::arg("value"), py::arg("description") = "",
            "Set integer configuration value")
        
        .def("set_float", [](Config& cfg, const std::string& key, double value, const std::string& description = "") {
            cfg.set(key, value);
            return cfg;
        }, py::arg("key"), py::arg("value"), py::arg("description") = "",
            "Set float configuration value")
        
        .def("set_string", [](Config& cfg, const std::string& key, const std::string& value, const std::string& description = "") {
            cfg.set(key, value);
            return cfg;
        }, py::arg("key"), py::arg("value"), py::arg("description") = "",
            "Set string configuration value")
        
        .def("set_bool", [](Config& cfg, const std::string& key, bool value, const std::string& description = "") {
            cfg.set(key, value);
            return cfg;
        }, py::arg("key"), py::arg("value"), py::arg("description") = "",
            "Set boolean configuration value")
        
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        
        .def("__str__", &Config::summary);

    // Add factory function for default config
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with recommended settings");

    // Config validation helper functions
    m.def("validateConfig", [](const std::shared_ptr<Config>& config) {
        auto& validator = getConfigValidator();
        auto errors = validator.validate(*config);
        return errors;
    }, py::arg("config"), "Validate configuration and return list of errors");

    m.def("getConfigValidatorSummary", []() {
        auto& validator = getConfigValidator();
        return validator.getSummary();
    }, "Get configuration validator summary");

    // Enhanced Brain methods
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class with advanced features)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create a new brain with configuration")
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
             "Produce motor action based on neural activity (enhanced with vector control)")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
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
             "Log brain status (enhanced with performance metrics)")
        
        // Enhanced Python-specific methods
        .def("setConfigValue", &brainSetConfigValue,
             py::arg("brain"), py::arg("key"), py::arg("value"),
             "Set configuration value (Python-friendly)")
        
        .def("getConfigValue", &brainGetConfigValue,
             py::arg("brain"), py::arg("key"),
             "Get configuration value (Python-friendly)")
        
        .def("validateConfig", &brainValidateConfig,
             py::arg("brain"),
             "Validate brain configuration and return success status")
        
        .def("getValidationErrors", &brainGetValidationErrors,
             py::arg("brain"),
             "Get validation errors as formatted string");

    // AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and sensory input")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object from world")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid in world")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy capacity")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // Factory functions
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    // Convenience factory functions for common configurations
    m.def("createDefaultBrain", []() {
        auto config = std::make_shared<Config>();
        // Set up default configuration
        config->set("neuron_count", static_cast<int64_t>(1000));
        config->set("region_count", static_cast<int64_t>(1));
        config->set("connection_probability", 0.1f);
        config->set("simulation_timestep", 0.001);
        config->set("random_seed", static_cast<int64_t>(42));
        return std::make_shared<Brain>(config);
    }, "Create a brain with default configuration");

    m.def("createDefaultWorld", []() {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->setAgentStart(10.0, 10.0);
        return world;
    }, "Create a world with default configuration");

    // Constants and helper values
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Enhanced logging utilities
    m.def("setLogLevel", [](EnhancedLogLevel level) {
        EnhancedLogger::getGlobal().setGlobalLevel(level);
    }, py::arg("level"), "Set global log level");

    m.def("getLogLevel", []() {
        return EnhancedLogger::getGlobal().getLevel();
    }, "Get global log level");

    // Example configuration presets
    m.def("getPresetConfig", [](const std::string& preset) {
        auto config = std::make_shared<Config>();
        
        if (preset == "basic") {
            config->set("neuron_count", static_cast<int64_t>(500));
            config->set("region_count", static_cast<int64_t>(1));
            config->set("connection_probability", 0.1f);
        } else if (preset == "advanced") {
            config->set("neuron_count", static_cast<int64_t>(5000));
            config->set("region_count", static_cast<int64_t>(3));
            config->set("connection_probability", 0.15f);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("neuromodulation.curiosity.enable", true);
            config->set("development.rate", 0.001);
        } else if (preset == "minimal") {
            config->set("neuron_count", static_cast<int64_t>(100));
            config->set("region_count", static_cast<int64_t>(1));
            config->set("connection_probability", 0.05f);
        }
        
        return config;
    }, py::arg("preset"), "Get a preset configuration");

    // Version and metadata
    m.attr("__version__") = "1.0.0";
    m.attr("__author__") = "NLM Project";
    m.attr("__description__") = "Neural Learning Machine - Python Bindings";

    // Additional utility functions
    m.def("getAvailablePresets", []() {
        return std::vector<std::string>{"basic", "advanced", "minimal"};
    }, "Get list of available configuration presets");

    m.def("isValidConfigKey", [](const std::string& key) {
        auto& validator = getConfigValidator();
        return validator.hasKey(key);
    }, py::arg("key"), "Check if configuration key is valid");

    m.def("getConfigInfo", [](const std::string& key) {
        auto& validator = getConfigValidator();
        const auto* param = validator.getParameter(key);
        if (!param) return std::string();
        
        std::ostringstream oss;
        oss << "Key: " << key << "\n";
        oss << "Type: " << param->type << "\n";
        oss << "Required: " << (param->required ? "yes" : "no") << "\n";
        oss << "Description: " << param->description << "\n";
        if (!param->defaultValue.empty()) {
            oss << "Default: " << param->defaultValue << "\n";
        }
        return oss.str();
    }, py::arg("key"), "Get information about a configuration key");
}

} // namespace nlm
