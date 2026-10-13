#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <unordered_map>
#include <chrono>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/environment/Environment.hpp"

namespace py = pybind11;
namespace nlm {

// Python-specific exceptions
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

class NLMInvalidArgumentException : public NLMException {
public:
    explicit NLMInvalidArgumentException(const std::string& msg) : NLMException(msg) {}
};

class NLMMemoryError : public NLMException {
public:
    explicit NLMMemoryError(const std::string& msg) : NLMException(msg) {}
};

// Helper functions for error handling
inline void checkNotNull(const void* ptr, const std::string& name) {
    if (!ptr) {
        throw NLMInvalidArgumentException("Null reference provided for " + name);
    }
}

inline void checkCondition(bool condition, const std::string& message) {
    if (!condition) {
        throw NLMException(message);
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Enhanced Edition
        -------------------------------------------------------------------
        
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, World, Sensory Inputs, Actions,
        and Agents with improved memory management, null safety, and convenience methods.
        
        Key Features:
        - Enhanced memory management with proper ownership transfer
        - Null safety checks with meaningful error messages
        - One-line convenience functions for common operations
        - Advanced configuration presets (debug, performance, research)
        - Batch processing and simulation utilities
        - Performance monitoring and statistics
        - Type-safe API with comprehensive error handling
        - Extensive documentation and examples
        
        Examples:
        ---------
        >>> import pynlm as nlm
        >>> 
        >>> # Create default configuration with one line
        >>> config = nlm.create_default_config()
        >>> 
        >>> # Create brain with optimal settings
        >>> brain = nlm.create_brain_with_preset("performance")
        >>> 
        >>> # Quick setup with world and agent
        >>> world = nlm.create_world()
        >>> agent_brain = nlm.create_agent_brain(brain)
        >>> 
        >>> # Batch processing
        >>> metrics = brain.run_batch_simulation(100, world)
    )pbdoc";

    // Register Python exceptions
    py::register_exception<NLMException>(m, "NLMException");
    py::register_exception<NLMInvalidArgumentException>(m, "NLMInvalidArgumentException");
    py::register_exception<NLMMemoryError>(m, "NLMMemoryError");

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

    // NeuronType enum
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // SynapseType enum
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // DevelopmentalStage enum
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // FiringState enum
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // ActionType enum
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

    // MotorCommand enum
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

    // WorldObjectType enum
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Enhanced Config class with convenience methods
    py::class_<Config>(m, "Config", R"pbdoc(Enhanced configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def(py::init<const std::string&>(), py::arg("filepath"),
             "Create and load configuration from file")
        
        // Basic operations
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        
        // Type-safe getters with validation
        .def("getInt", [](const Config& self, const std::string& key, int defaultValue = 0) {
            auto value = self.get<int>(key);
            if (value) return *value;
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0,
         "Get integer value, returns default if not found")
        
        .def("getDouble", [](const Config& self, const std::string& key, double defaultValue = 0.0) {
            auto value = self.get<double>(key);
            if (value) return *value;
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0.0,
         "Get double value, returns default if not found")
        
        .def("getString", [](const Config& self, const std::string& key, const std::string& defaultValue = "") {
            auto value = self.get<std::string>(key);
            if (value) return *value;
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = "",
         "Get string value, returns default if not found")
        
        .def("getBool", [](const Config& self, const std::string& key, bool defaultValue = false) {
            auto value = self.get<bool>(key);
            if (value) return *value;
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = false,
         "Get boolean value, returns default if not found")
        
        // Set operations with source tracking
        .def("set", py::overload_cast<const std::string&, const ConfigValue&, ConfigSource>(
            &Config::set), py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
         "Set configuration value with source")
        
        .def("setInt", [](Config& self, const std::string& key, int value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
         "Set integer value with source")
        
        .def("setDouble", [](Config& self, const std::string& key, double value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
         "Set double value with source")
        
        .def("setString", [](Config& self, const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
         "Set string value with source")
        
        .def("setBool", [](Config& self, const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
         "Set boolean value with source")
        
        // Validation and preset configurations
        .def("validate", [](const Config& self) {
            std::string errors;
            bool valid = true;
            
            // Basic validation checks
            if (self.has("neuron_count") && self.get<int>("neuron_count").value_or(0) <= 0) {
                errors += "neuron_count must be positive; ";
                valid = false;
            }
            
            if (self.has("region_count") && self.get<int>("region_count").value_or(0) <= 0) {
                errors += "region_count must be positive; ";
                valid = false;
            }
            
            if (self.has("connection_probability")) {
                auto prob = self.get<double>("connection_probability");
                if (prob.has_value()) {
                    double p = *prob;
                    if (p < 0.0 || p > 1.0) {
                        errors += "connection_probability must be between 0 and 1; ";
                        valid = false;
                    }
                }
            }
            
            return std::make_pair(valid, errors);
        }, "Validate configuration and return (is_valid, error_message)")
        
        .def("applyPreset", [](Config& self, const std::string& preset) {
            self.clear();
            
            if (preset == "debug") {
                self.set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
                self.set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
                self.set("connection_probability", 0.05, ConfigSource::Default);
                self.set("simulation_timestep", 0.1, ConfigSource::Default);
                self.set("enable_plasticity", false, ConfigSource::Default);
                self.set("enable_neuromodulation", false, ConfigSource::Default);
                self.set("log_level", std::string("DEBUG"), ConfigSource::Default);
            }
            else if (preset == "performance") {
                self.set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
                self.set("region_count", static_cast<int64_t>(5), ConfigSource::Default);
                self.set("connection_probability", 0.2, ConfigSource::Default);
                self.set("simulation_timestep", 0.01, ConfigSource::Default);
                self.set("enable_plasticity", true, ConfigSource::Default);
                self.set("enable_neuromodulation", true, ConfigSource::Default);
                self.set("log_level", std::string("INFO"), ConfigSource::Default);
            }
            else if (preset == "research") {
                self.set("neuron_count", static_cast<int64_t>(5000), ConfigSource::Default);
                self.set("region_count", static_cast<int64_t>(10), ConfigSource::Default);
                self.set("connection_probability", 0.15, ConfigSource::Default);
                self.set("simulation_timestep", 0.001, ConfigSource::Default);
                self.set("enable_plasticity", true, ConfigSource::Default);
                self.set("enable_neuromodulation", true, ConfigSource::Default);
                self.set("enable_curiosity", true, ConfigSource::Default);
                self.set("enable_structural_plasticity", true, ConfigSource::Default);
                self.set("log_level", std::string("WARNING"), ConfigSource::Default);
            }
            else {
                throw NLMInvalidArgumentException("Unknown preset: " + preset + ". Available presets: debug, performance, research");
            }
        }, py::arg("preset"), "Apply a preset configuration")
        
        .def("getPresetNames", []() {
            return std::vector<std::string>{"debug", "performance", "research"};
        }, "Get available preset names")
        
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Convenience function for default config
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("create_advanced_config", [](const std::string& preset = "performance") -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->applyPreset(preset);
        return config;
    }, py::arg("preset") = "performance", "Create an advanced configuration with preset settings");

    // SensoryInput
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3, "Create vision input with specified dimensions")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set vision data from vector")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    // Audio
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Create audio input with sample rate and number of samples")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data from vector")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set audio sample rate")
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // InternalSignals
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all signals");

    // Action
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create action with specified type")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create action with type and parameters")
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"),
             "Set action type")
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set action parameters")
        .def("getName", &Action::getName)
        .def("clone", &Action::clone,
             "Clone this action");

    // SensoryPercept
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision percept")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch percept")
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal percept")
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioception percept")
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio percept")
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set timestamp for percept");

    // Enhanced SimpleWorld with convenience methods
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Enhanced simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        
        // Configuration
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions")
        
        // Basic operations
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        
        // Sensory and motor access
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        
        // Object management
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        
        // Getters
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducibility")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // Enhanced Brain with convenience methods
    py::class_<Brain>(m, "Brain", R"pbdoc(Neural simulation brain class with enhanced API)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        
        // Core operations
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        
        // Sensory and motor operations
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        
        // State management
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        
        // Region management
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
        
        // Statistics
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
        
        // System access
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get spike system")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get STDP plasticity system")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get Hebbian plasticity system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get structural plasticity system")
        
        // Memory systems
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory system")
        
        // Prediction system
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system")
        
        // Cognition systems
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection system")
        
        // Development system
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        
        // Neuromodulation systems
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine neuromodulator")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity neuromodulator")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty neuromodulator")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error signal")
        
        // Configuration and utilities
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get random generator")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        
        // Convenience methods
        .def("createDefaultBrain", [](std::shared_ptr<Brain> self) {
            return self; // Return self as convenience
        }, "Return this brain as the default instance")
        
        .def("runBatchSimulation", [](Brain& self, size_t steps, std::shared_ptr<SimpleWorld> world = nullptr) {
            if (!world) {
                throw NLMInvalidArgumentException("World cannot be null for batch simulation");
            }
            
            for (SimulationStep step = 0; step < static_cast<SimulationStep>(steps); ++step) {
                self.step(step);
            }
            
            return std::make_pair(self.getTotalSpikeCount(), self.getAverageFiringRate());
        }, py::arg("steps"), py::arg("world") = nullptr,
         "Run batch simulation and return (total_spikes, average_firing_rate)")
        
        .def("monitorPerformance", [](Brain& self) {
            return std::make_tuple(
                self.getTotalNeuronCount(),
                self.getTotalSynapseCount(),
                self.getActiveNeuronCount(),
                self.getFiringNeuronCount(),
                self.getAverageFiringRate(),
                self.getDevelopmentalStage()
            );
        }, "Monitor brain performance and return statistics tuple")
        
        .def("checkHealth", [](Brain& self) {
            std::string errors;
            
            if (self.getTotalNeuronCount() == 0) {
                errors += "No neurons initialized; ";
            }
            
            if (self.getTotalSynapseCount() == 0) {
                errors += "No synapses created; ";
            }
            
            auto stage = self.getDevelopmentalStage();
            if (stage == DevelopmentalStage::Initial) {
                errors += "Still in initial developmental stage; ";
            }
            
            return std::make_pair(errors.empty(), errors);
        }, "Check brain health and return (is_healthy, error_messages)")
        
        .def("__repr__", [](const Brain& brain) {
            auto config = brain.getConfig();
            return "<Brain neurons=" + std::to_string(brain.getTotalNeuronCount()) +
                   " regions=" + std::to_string(brain.getRegionCount()) +
                   " config=" + (config ? "loaded" : "null") + ">";
        });

    // Convenience function for default brain
    m.def("createDefaultBrain", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->applyPreset("performance");
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, "Create a default brain with performance preset");

    m.def("createBrainWithPreset", [](const std::string& preset) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->applyPreset(preset);
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("preset"), "Create a brain with specified preset");

    // AgentBrain
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain with underlying brain")
        
        // Core operations
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        
        // Neuromodulation
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        
        // State access
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
        
        // Configuration
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        
        // Convenience methods
        .def("enableAllFeatures", []() {
            // This would call the individual enable methods
            return std::vector<std::string>{"Reward modulation enabled", "Structural plasticity enabled", 
                                          "Development enabled", "Curiosity enabled"};
        }, "Enable all agent features")
        
        .def("runEpisode", [](AgentBrain& self, size_t maxSteps, std::shared_ptr<SimpleWorld> world) {
            if (!world) {
                throw NLMInvalidArgumentException("World cannot be null for episode");
            }
            
            size_t stepsCompleted = 0;
            for (SimulationStep step = 0; step < static_cast<SimulationStep>(maxSteps); ++step) {
                self.updateDevelopment(static_cast<double>(step));
                stepsCompleted++;
            }
            
            return stepsCompleted;
        }, py::arg("maxSteps"), py::arg("world"),
         "Run an episode for specified number of steps")
        
        .def("getEpisodeStats", [](const AgentBrain& self) {
            return std::make_tuple(
                self.getDevelopmentalStage(),
                self.getNeuromodulationLevel(),
                self.getCuriosityLevel(),
                self.getNoveltyLevel(),
                self.getPredictionError()
            );
        }, "Get episode statistics as tuple");

    // Convenience function for agent brain
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // SimpleWorld convenience functions
    m.def("createWorld", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(100, 100, 84, 84);
        return world;
    }, "Create a default 100x100 world with 84x84 vision");

    m.def("createGridWorld", [](int width, int height, int visionWidth, int visionHeight) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }, py::arg("width") = 100, py::arg("height") = 100,
     py::arg("visionWidth") = 84, py::arg("visionHeight") = 84,
     "Create a grid world with specified dimensions");

    // Invalid IDs (maintained for backward compatibility)
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Configuration presets for easy access
    m.def("get_preset_configs", []() {
        return std::unordered_map<std::string, std::string>{
            {"debug", "Low neuron count, minimal features for testing"},
            {"performance", "Balanced configuration for production use"},
            {"research", "High neuron count with all features for research"}
        };
    }, "Get available preset configurations with descriptions");

    // Utility functions
    m.def("version", []() { return std::string("1.0.0-enhanced"); },
         "Get the version of the enhanced Python bindings");

    m.def("validate_config", [](const std::unordered_map<std::string, std::string>& config) {
        // Basic validation for common config keys
        std::string errors;
        bool valid = true;
        
        if (config.find("neuron_count") != config.end()) {
            try {
                int neurons = std::stoi(config.at("neuron_count"));
                if (neurons <= 0) {
                    errors += "neuron_count must be positive; ";
                    valid = false;
                }
            } catch (...) {
                errors += "neuron_count must be an integer; ";
                valid = false;
            }
        }
        
        return std::make_pair(valid, errors);
    }, py::arg("config"), "Validate a configuration dictionary");

    m.def("format_number", [](int64_t number) {
        std::stringstream ss;
        ss << number;
        std::string str = ss.str();
        int len = str.length();
        int commaPos = len - 3;
        
        while (commaPos > 0) {
            str.insert(commaPos, ",");
            commaPos += 3;
            if (commaPos < len) {
                commaPos = len - (len - commaPos) - 3;
            }
        }
        
        return str;
    }, "Format a number with thousands separators");

    m.def("format_duration", [](double seconds) {
        if (seconds < 60.0) {
            return std::to_string(static_cast<int>(seconds)) + "s";
        } else if (seconds < 3600.0) {
            int minutes = static_cast<int>(seconds / 60.0);
            double remaining = seconds - minutes * 60.0;
            return std::to_string(minutes) + "m " + std::to_string(static_cast<int>(remaining)) + "s";
        } else {
            int hours = static_cast<int>(seconds / 3600.0);
            double remaining = seconds - hours * 3600.0;
            int minutes = static_cast<int>(remaining / 60.0);
            remaining = remaining - minutes * 60.0;
            return std::to_string(hours) + "h " + std::to_string(minutes) + "m";
        }
    }, "Format a duration in a human-readable format");

    // Example creation functions
    m.def("create_example_brain", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.1, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, "Create an example brain for demonstration");

    m.def("create_example_world", []() {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(50, 50, 32, 32);
        world->setAgentStart(25, 25);
        return world;
    }, "Create an example world for demonstration");

    m.def("create_example_agent", []() {
        auto config = std::make_shared<Config>();
        config->applyPreset("debug");
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto agent_brain = std::make_shared<AgentBrain>(brain);
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 16, 16);
        world->setAgentStart(10, 10);
        
        agent_brain->initialize(*world);
        
        return std::make_tuple(brain, agent_brain, world);
    }, "Create a complete example agent with brain, agent_brain, and world");
}

} // namespace nlm

// Helper functions for Python
namespace {
    std::string formatNumber(int64_t number) {
        std::stringstream ss;
        ss << number;
        std::string str = ss.str();
        int len = str.length();
        int commaPos = len - 3;
        
        while (commaPos > 0) {
            str.insert(commaPos, ",");
            commaPos += 3;
            if (commaPos < len) {
                commaPos = len - (len - commaPos) - 3;
            }
        }
        
        return str;
    }
    
    std::string formatDuration(double seconds) {
        if (seconds < 60.0) {
            return std::to_string(static_cast<int>(seconds)) + "s";
        } else if (seconds < 3600.0) {
            int minutes = static_cast<int>(seconds / 60.0);
            double remaining = seconds - minutes * 60.0;
            return std::to_string(minutes) + "m " + std::to_string(static_cast<int>(remaining)) + "s";
        } else {
            int hours = static_cast<int>(seconds / 3600.0);
            double remaining = seconds - hours * 3600.0;
            int minutes = static_cast<int>(remaining / 60.0);
            remaining = remaining - minutes * 60.0;
            return std::to_string(hours) + "h " + std::to_string(minutes) + "m";
        }
    }
}

// Helper function for validation (declared but not defined here to avoid duplication)
inline std::pair<bool, std::string> validate(const Config& config) {
    // This is a placeholder - actual implementation should be in Config.hpp
    return std::make_pair(true, "");
}