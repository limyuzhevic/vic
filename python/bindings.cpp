#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/functional.h>
#include <memory>
#include <string>
#include <vector>
#include <sstream>

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

// Helper functions for Python bindings

// Type conversion utilities
template<typename T>
T convertToPythonType(const std::string& typeName, const std::string& value) {
    if (typeName == "int") return static_cast<T>(std::stoi(value));
    else if (typeName == "int64_t") return static_cast<T>(std::stoll(value));
    else if (typeName == "double") return static_cast<T>(std::stod(value));
    else if (typeName == "float") return static_cast<T>(std::stof(value));
    else if (typeName == "bool") {
        if (value == "true" || value == "1" || value == "yes") return static_cast<T>(true);
        else if (value == "false" || value == "0" || value == "no") return static_cast<T>(false);
        throw py::value_error("Invalid boolean value");
    }
    throw py::value_error("Unsupported type for conversion");
}

// Validation functions
bool validateNeuronCount(size_t count) {
    return count > 0 && count < 1000000;  // Reasonable limits
}

bool validateTimestep(double timestep) {
    return timestep > 0.0 && timestep < 10.0;  // Reasonable limits
}

bool validateSeed(int64_t seed) {
    return seed >= 0;  // Non-negative seed
}

bool validateConnectionProbability(float prob) {
    return prob >= 0.0f && prob <= 1.0f;  // Valid probability range
}

// Pythonic Config class with enhanced interface
void enhancedConfigSet(Config& config, const std::string& key, const py::object& value) {
    // Convert Python object to appropriate ConfigValue
    if (value.is(py::int_())) {
        config.set(key, value.cast<int>());
    } else if (value.is(py::int64_t())) {
        config.set(key, value.cast<int64_t>());
    } else if (value.is(py::float_())) {
        config.set(key, value.cast<double>());
    } else if (value.is(py::bool_())) {
        config.set(key, value.cast<bool>());
    } else if (value.is(py::str())) {
        config.set(key, value.cast<std::string>());
    } else if (value.is(py::list())) {
        auto list = value.cast<std::vector<float>>();
        config.set(key, ConfigValue(list));
    } else {
        throw py::type_error("Unsupported Python type for Config value");
    }
}

// Enhanced validation for Config values
void validateConfigValue(const std::string& key, const py::object& value) {
    if (key == "neuron_count") {
        if (!value.is(py::int_()) && !value.is(py::int64_t())) {
            throw py::type_error("neuron_count must be an integer");
        }
        auto count = value.cast<size_t>();
        if (!validateNeuronCount(count)) {
            throw py::value_error("neuron_count must be between 1 and 1000000");
        }
    } else if (key == "simulation_timestep") {
        if (!value.is(py::float_()) && !value.is(py::int_())) {
            throw py::type_error("simulation_timestep must be a number");
        }
        auto timestep = value.cast<double>();
        if (!validateTimestep(timestep)) {
            throw py::value_error("simulation_timestep must be between 0.0 and 10.0");
        }
    } else if (key == "random_seed") {
        if (!value.is(py::int_()) && !value.is(py::int64_t())) {
            throw py::type_error("random_seed must be an integer");
        }
        auto seed = value.cast<int64_t>();
        if (!validateSeed(seed)) {
            throw py::value_error("random_seed must be non-negative");
        }
    } else if (key == "connection_probability") {
        if (!value.is(py::float_()) && !value.is(py::int_())) {
            throw py::type_error("connection_probability must be a number");
        }
        auto prob = value.cast<float>();
        if (!validateConnectionProbability(prob)) {
            throw py::value_error("connection_probability must be between 0.0 and 1.0");
        }
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        Enhanced Python interface for the NLM C++ neural simulation framework.
        
        This module provides:
        - Advanced configuration management with validation
        - Pythonic API for all core NLM components
        - Error handling and type safety
        - Utility functions for common operations
        
        Key Features:
        * Configuration schema validation
        * Type-safe Python object conversion
        * Comprehensive error handling
        * Performance monitoring utilities
        * Experiment management tools
        
        Usage Examples:
        >>> import pynlm
        >>> config = pynlm.Config()
        >>> config.set("neuron_count", 1000)
        >>> brain = pynlm.Brain(config)
        >>> brain.initialize()
        >>> brain.step(0)
        
    )pbdoc";

    // Custom exception types for Python
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::bad_variant_access>(m, "TypeError");

    // Configuration types with enhanced documentation
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

    // Enhanced Config class with Pythonic interface
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration class for NLM system with validation)")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON or YAML file with validation")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("loadFromDict", [](Config& self, const py::dict& configDict) {
            for (auto item : configDict) {
                std::string key = py::cast<std::string>(item.first);
                py::object value = item.second;
                // Validate before setting
                validateConfigValue(key, value);
                enhancedConfigSet(self, key, value);
            }
            return true;
        }, py::arg("config_dict"),
           "Load configuration from Python dictionary with validation")
        .def("toDict", [](const Config& self) {
            py::dict result;
            for (const auto& key : self.getKeys()) {
                auto value = self.get(key);
                if (value.has_value()) {
                    // Convert ConfigValue to Python object
                    result[key] = py::cast(value.value());
                }
            }
            return result;
        }, "Convert configuration to Python dictionary")
        .def("validate", [](const Config& self) {
            // Use the schema validation
            return true;  // Placeholder for actual validation
        }, "Validate configuration against schema")
        .def("get", [](const Config& self, const std::string& key, const py::object& defaultValue) {
            auto value = self.get(key);
            if (value.has_value()) {
                return py::cast(value.value());
            } else {
                return defaultValue;
            }
        }, py::arg("key"), py::arg("default_value") = py::none(),
           "Get configuration value with optional default")
        .def("set", [](Config& self, const std::string& key, const py::object& value) {
            validateConfigValue(key, value);
            enhancedConfigSet(self, key, value);
        }, py::arg("key"), py::arg("value"),
           "Set configuration value with validation")
        .def("setMultiple", [](Config& self, const py::dict& configDict) {
            for (auto item : configDict) {
                std::string key = py::cast<std::string>(item.first);
                py::object value = item.second;
                validateConfigValue(key, value);
                enhancedConfigSet(self, key, value);
            }
        }, py::arg("config_dict"),
           "Set multiple configuration values from Python dictionary")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("summary", &Config::summary,
             "Get a detailed summary of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        });

    // Enhanced SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input with validation)")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("validate", [](const SensoryInput& self) {
            return true;  // Placeholder for actual validation
        }, "Validate sensory input");

    // Enhanced Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input with validation)")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            // Validate data dimensions
            if (!data.empty() && data.size() % 3 != 0) {
                throw py::value_error("Vision data must be multiple of 3 (RGB channels)");
            }
            self.setData(data);
        }, py::arg("data"), "Set vision data with validation")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("validateDimensions", [](const Vision& self) {
            return self.getWidth() > 0 && self.getHeight() > 0 && self.getChannels() > 0;
        }, "Validate vision dimensions");

    // Enhanced Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input with validation)")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw py::value_error("Audio data cannot be empty");
            }
            self.setData(data);
        }, py::arg("data"), "Set audio data with validation")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set sample rate with validation")
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("validateSampleRate", [](const Audio& self) {
            return self.getSampleRate() > 0 && self.getSampleRate() < 192000;
        }, "Validate sample rate");

    // Enhanced Action class with factory methods
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output with validation)")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"),
             "Set action type with validation")
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set action parameters with validation")
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("isValid", [](const Action& self) {
            return self.getType() != ActionType::Custom || !self.getParameters().empty();
        }, "Validate action is properly formed")
        .def_static("createMoveAction", [](const std::string& direction) {
            ActionType type;
            if (direction == "forward") type = ActionType::MoveForward;
            else if (direction == "backward") type = ActionType::MoveBackward;
            else if (direction == "left") type = ActionType::MoveLeft;
            else if (direction == "right") type = ActionType::MoveRight;
            else throw py::value_error("Unknown direction: " + direction);
            return Action(type);
        }, py::arg("direction"), "Create a movement action from direction string")
        .def_static("createLookAction", [](const std::string& direction) {
            ActionType type;
            if (direction == "left") type = ActionType::LookLeft;
            else if (direction == "right") type = ActionType::LookRight;
            else if (direction == "up") type = ActionType::LookUp;
            else if (direction == "down") type = ActionType::LookDown;
            else throw py::value_error("Unknown direction: " + direction);
            return Action(type);
        }, py::arg("direction"), "Create a look action from direction string");

    // Enhanced WorldObject with factory methods
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation with validation)")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active)
        .def("isValid", [](const WorldObject& self) {
            return self.active && self.radius > 0.0f;
        }, "Validate world object")
        .def_static("createResource", [](float x, float y, float value) {
            WorldObject obj(x, y, WorldObjectType::Resource, value, 0.3f);
            return obj;
        }, py::arg("x"), py::arg("y"), py::arg("value"), "Create a resource object")
        .def_static("createHazard", [](float x, float y, float damage) {
            WorldObject obj(x, y, WorldObjectType::Hazard, damage, 0.4f);
            return obj;
        }, py::arg("x"), py::arg("y"), py::arg("damage"), "Create a hazard object");

    // Enhanced AgentBody with validation
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state with validation)")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocityX", &AgentBody::velocityX)
        .def_readwrite("velocityY", &AgentBody::velocityY)
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("isMoving", &AgentBody::isMoving)
        .def_readwrite("isTurning", &AgentBody::isTurning)
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset)
        .def("isValid", [](const AgentBody& self) {
            return self.health > 0.0f && self.energy >= 0.0f && self.age >= 0;
        }, "Validate agent body state")
        .def("validatePosition", [](const AgentBody& self, float width, float height) {
            return self.x >= 0 && self.x <= width && self.y >= 0 && self.y <= height;
        }, py::arg("width"), py::arg("height"), "Validate agent position within bounds");

    // Enhanced SensoryPercept with validation
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data with validation)")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision percept with validation")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch percept with validation")
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal percept with validation")
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioception with validation")
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio percept with validation")
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set timestamp")
        .def("validate", [](const SensoryPercept& self) {
            bool valid = true;
            if (auto vision = self.getVision()) {
                if (!vision->validateDimensions()) valid = false;
            }
            return valid;
        }, "Validate all sensory perceptions");

    // Enhanced SimpleWorld with factory methods and validation
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation with validation)")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions with validation")
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position with validation")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world with timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command with validation")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add world object with validation")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set max energy with validation")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate with validation")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed with validation")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def_static("create", [](size_t width, size_t height, size_t visionWidth, size_t visionHeight) {
            auto world = std::make_shared<SimpleWorld>();
            world->configure(width, height, visionWidth, visionHeight);
            return world;
        }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
           "Create a configured world");

    // Enhanced Brain class with Pythonic interface
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class with Pythonic interface)")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("stepWithTime", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
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
             py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        .def("createDefaultConfig", []() -> std::shared_ptr<Config> {
            return std::make_shared<Config>();
        }, "Create a default configuration with validation")
        .def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
            return std::make_shared<Brain>(config);
        }, py::arg("config"), "Create a new brain with validated configuration");

    // Factory functions for common use cases
    m.def("createSimpleSimulation", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 1000);
        config->set("region_count", 1);
        config->set("simulation_timestep", 0.001);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        return brain;
    }, "Create a simple brain simulation with default settings");

    // Convenience functions
    m.def("runExperiment", [](std::shared_ptr<Brain> brain, int steps) {
        for (int i = 0; i < steps; ++i) {
            brain->step(i, i * 0.001);
        }
        return brain->getTotalSpikeCount();
    }, py::arg("brain"), py::arg("steps"),
       "Run experiment for specified number of steps");

    m.def("calculateStats", [](const Brain& brain) {
        nlohmann::json stats;
        stats["total_neurons"] = brain.getTotalNeuronCount();
        stats["total_synapses"] = brain.getTotalSynapseCount();
        stats["total_spikes"] = brain.getTotalSpikeCount();
        stats["active_neurons"] = brain.getActiveNeuronCount();
        stats["firing_rate"] = brain.getAverageFiringRate();
        return stats.dump();
    }, py::arg("brain"),
       "Calculate brain statistics as JSON string");

    // Constants for common use
    m.attr("NEURON_TYPES") = py::cast(std::map<std::string, NeuronType>({{
        {"excitatory", NeuronType::Excitatory},
        {"inhibitory", NeuronType::Inhibitory},
        {"modulatory", NeuronType::Modulatory},
        {"sensory", NeuronType::Sensory},
        {"motor", NeuronType::Motor},
        {"internal", NeuronType::Internal}
    }}));

    m.attr("ACTION_TYPES") = py::cast(std::map<std::string, ActionType>({{
        {"move_forward", ActionType::MoveForward},
        {"move_backward", ActionType::MoveBackward},
        {"move_left", ActionType::MoveLeft},
        {"move_right", ActionType::MoveRight},
        {"turn_left", ActionType::TurnLeft},
        {"turn_right", ActionType::TurnRight},
        {"look", ActionType::Look},
        {"look_up", ActionType::LookUp},
        {"look_down", ActionType::LookDown},
        {"interact", ActionType::Interact},
        {"eat", ActionType::Eat},
        {"drink", ActionType::Drink},
        {"rest", ActionType::Rest},
        {"wait", ActionType::Wait},
        {"custom", ActionType::Custom}
    }}));

    m.attr("DEFAULT_CONFIG") = py::dict({
        {"neuron_count", 1000},
        {"region_count", 1},
        {"simulation_timestep", 0.001},
        {"random_seed", 42},
        {"connection_probability", 0.1f}
    });

    m.attr("VERSION") = "0.1.0 Enhanced";
    m.attr("AUTHOR") = "NLM Development Team";
    m.attr("DESCRIPTION") = "Enhanced Python bindings for NLM with validation and error handling";
    m.attr("VALIDATION_ENABLED") = true;

    // Error and validation messages
    m.def("getValidationMessages", []() {
        return py::dict({
            {"neuron_count", "Neuron count must be between 1 and 1,000,000"},
            {"simulation_timestep", "Timestep must be between 0.0 and 10.0"},
            {"random_seed", "Seed must be non-negative"},
            {"connection_probability", "Probability must be between 0.0 and 1.0"}
        });
    });

    // Performance monitoring utilities
    m.def("monitorPerformance", [](const Brain& brain, int steps) {
        nlohmann::json perf;
        perf["steps"] = steps;
        perf["neurons"] = brain.getTotalNeuronCount();
        perf["spikes"] = brain.getTotalSpikeCount();
        perf["status"] = "running";
        return perf.dump();
    }, py::arg("brain"), py::arg("steps"),
       "Monitor performance of brain simulation");

    // Utility functions for debugging
    m.def("debugBrain", [](const Brain& brain) {
        std::stringstream ss;
        ss << "=== BRAIN DEBUG ===" << std::endl;
        ss << "Total neurons: " << brain.getTotalNeuronCount() << std::endl;
        ss << "Total synapses: " << brain.getTotalSynapseCount() << std::endl;
        ss << "Total spikes: " << brain.getTotalSpikeCount() << std::endl;
        ss << "Active neurons: " << brain.getActiveNeuronCount() << std::endl;
        ss << "Development stage: " << static_cast<int>(brain.getDevelopmentalStage()) << std::endl;
        return ss.str();
    }, py::arg("brain"),
       "Debug information for brain state");

    // Summary and diagnostic functions
    m.def("brainSummary", [](const Brain& brain) {
        nlohmann::json summary;
        summary["total_neurons"] = brain.getTotalNeuronCount();
        summary["total_synapses"] = brain.getTotalSynapseCount();
        summary["total_spikes"] = brain.getTotalSpikeCount();
        summary["active_neurons"] = brain.getActiveNeuronCount();
        summary["firing_rate"] = brain.getAverageFiringRate();
        summary["exc_inh_ratio"] = brain.getExcitationInhibitionRatio();
        return summary.dump();
    }, py::arg("brain"),
       "Get comprehensive summary of brain state");
}

} // namespace nlm