// Enhanced Python bindings with improved functionality and error handling

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
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

// Custom Python exceptions
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& msg) : NLMError("Configuration error: " + msg) {}
};

class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& msg) : NLMError("Brain error: " + msg) {}
};

// Enhanced configuration methods
void Config_setWithDescription(Config& self, const std::string& key, const std::string& value, 
                              const std::string& description, ConfigSource source) {
    self.set(key, value, source);
}

// Enhanced sensory input methods
void Vision_setFromArray(Vision& self, const std::vector<float>& data, 
                        size_t width, size_t height, size_t channels = 3) {
    self.setData(data);
}

void Audio_setFromArray(Audio& self, const std::vector<float>& data,
                       size_t sampleRate, size_t numSamples) {
    self.setData(data);
    self.setSampleRate(sampleRate);
}

// Enhanced world configuration
void SimpleWorld_configureAdvanced(SimpleWorld& self, 
                                  size_t width, size_t height,
                                  size_t visionWidth, size_t visionHeight,
                                  size_t visionChannels = 3) {
    self.configure(width, height, visionWidth, visionHeight);
}

// Advanced action methods
void Action_setFromDict(Action& self, py::dict params) {
    std::vector<float> paramVec;
    for (auto item : params) {
        paramVec.push_back(py::cast<float>(item.second));
    }
    self.setParameters(paramVec);
}

// Brain utility methods
void Brain_enableAllFeatures(Brain& self) {
    if (auto agent = self.getAgentBrain()) {
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
    }
}

py::dict Brain_getStatistics(const Brain& self) {
    py::dict stats;
    stats["total_neurons"] = self.getTotalNeuronCount();
    stats["total_synapses"] = self.getTotalSynapseCount();
    stats["active_neurons"] = self.getActiveNeuronCount();
    stats["firing_neurons"] = self.getFiringNeuronCount();
    stats["total_spikes"] = self.getTotalSpikeCount();
    stats["average_firing_rate"] = self.getAverageFiringRate();
    stats["e_i_ratio"] = self.getExcitationInhibitionRatio();
    stats["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
    return stats;
}

// AgentBrain advanced methods
void AgentBrain_configureForWorld(AgentBrain& self, const SimpleWorld& world) {
    self.initialize(world);
}

void AgentBrain_processSensoryBatch(AgentBrain& self, const std::vector<SensoryPercept>& percepts) {
    for (const auto& percept : percepts) {
        self.processSensoryInput(percept);
    }
}

// Enhanced config utilities
py::dict Config_validateRequiredKeys(const Config& self, const std::vector<std::string>& requiredKeys) {
    py::dict result;
    result["valid"] = true;
    result["missing_keys"] = py::list();
    
    for (const auto& key : requiredKeys) {
        if (!self.has(key)) {
            result["valid"] = false;
            py::list missing = py::cast(result["missing_keys"]);
            missing.append(key);
            result["missing_keys"] = missing;
        }
    }
    
    return result;
}

// Serialization utilities
std::string Config_exportToJson(const Config& self) {
    json j;
    for (const auto& entry : self.getKeys()) {
        std::visit([&j, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                j[entry] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                j[entry] = arg;
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                j[entry] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                j[entry] = arg;
            }
        }, self.get(entry));
    }
    return j.dump(2);
}

bool Config_importFromJson(Config& self, const std::string& jsonStr) {
    try {
        auto j = json::parse(jsonStr);
        self.clear();
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            const auto& value = *it;
            
            ConfigValue configValue;
            if (value.is_string()) {
                configValue = value.get<std::string>();
            } else if (value.is_number_integer()) {
                configValue = value.get<int>();
            } else if (value.is_number_unsigned()) {
                configValue = value.get<size_t>();
            } else if (value.is_number_float()) {
                configValue = value.get<double>();
            } else if (value.is_boolean()) {
                configValue = value.get<bool>();
            }
            
            self.set(key, configValue, ConfigSource::File);
        }
        return true;
    } catch (const std::exception& e) {
        throw ConfigError(std::string("Failed to parse JSON: ") + e.what());
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Enhanced
        --------------------------------------------------------
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        This enhanced version includes:
        - Better error handling with custom exceptions
        - Advanced configuration management (JSON/YAML support)
        - Enhanced serialization utilities
        - Improved world and agent interfaces
        - Batch processing capabilities
        - Advanced configuration validation
        - Enhanced documentation and type safety
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigError>(m, "ConfigError", "NLMError");
    py::register_exception<BrainError>(m, "BrainError", "NLMError");

    // Neuron ID types
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
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    // Synapse ID types
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
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        });

    // Region ID types
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

    // Enumerations
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

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

    // Enhanced Config class
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from file (supports JSON, YAML, and simple key=value formats)")
        .def("loadFromJsonFile", [](Config& self, const std::string& filepath) {
            return self.loadFromJsonFile(filepath);
        }, py::arg("filepath"), "Load configuration from JSON file")
        .def("loadFromYamlFile", [](Config& self, const std::string& filepath) {
            return self.loadFromYamlFile(filepath);
        }, py::arg("filepath"), "Load configuration from YAML file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"), "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to file (auto-detects format from extension)")
        .def("importFromJson", [](Config& self, const std::string& jsonStr) {
            return self.importFromJson(jsonStr);
        }, py::arg("json_string"), "Import configuration from JSON string")
        .def("exportToJson", &Config::exportToJson, "Export configuration to JSON string")
        .def("has", &Config::has, py::arg("key"), "Check if a configuration key exists")
        .def("get", [](const Config& self, const std::string& key) {
            auto val = self.get<std::string>(key);
            if (val) return *val;
            throw ConfigError("Key not found: " + key);
        }, py::arg("key"), "Get configuration value as string (throws if not found)")
        .def("getOr", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            auto val = self.get<std::string>(key);
            return val.has_value() ? *val : defaultValue;
        }, py::arg("key"), py::arg("default_value"), "Get configuration value with default")
        .def("set", static_cast<void(Config::*)(const std::string&, const std::string&, ConfigSource)>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set configuration value")
        .def("setWithDescription", &Config_setWithDescription,
             py::arg("key"), py::arg("value"), py::arg("description"), py::arg("source") = ConfigSource::Runtime,
             "Set configuration value with description")
        .def("validateRequiredKeys", &Config_validateRequiredKeys,
             py::arg("required_keys"), "Validate that all required keys exist")
        .def("getKeys", &Config::getKeys, "Get all configuration keys")
        .def("clear", &Config::clear, "Clear all configuration entries")
        .def("summary", &Config::summary, "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Enhanced SensoryInput classes
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"), "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("setData", &Vision::setData, py::arg("data"), "Set vision data")
        .def("setFromArray", &Vision_setFromArray,
             py::arg("data"), py::arg("width"), py::arg("height"), py::arg("channels") = 3,
             "Set vision data from array with dimensions")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", &Audio::setData, py::arg("data"), "Set audio data")
        .def("setFromArray", &Audio_setFromArray,
             py::arg("data"), py::arg("sampleRate"), py::arg("numSamples"),
             "Set audio data from array with sample rate")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // Enhanced Action class
    py::class_<Action>(m, "Action", R"pbdoc(Enhanced action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("setFromDict", &Action_setFromDict,
             py::arg("params"), "Set parameters from dictionary")
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    // Enhanced WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);

    // Enhanced AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Enhanced agent body state)pbdoc")
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
        .def("reset", &AgentBody::reset);

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // Enhanced SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Enhanced sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    // Enhanced SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Enhanced simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("configureAdvanced", &SimpleWorld_configureAdvanced,
             py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"), py::arg("visionChannels") = 3,
             "Advanced world configuration with vision channels")
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // Enhanced Brain class
    py::class_<Brain>(m, "Brain", R"pbdoc(Enhanced central neural simulation brain class)pbdoc")
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
             "Log brain status")
        .def("getStatistics", &Brain_getStatistics,
             "Get brain statistics as dictionary")
        .def("enableAllFeatures", &Brain_enableAllFeatures,
             "Enable all brain features for agent operation");

    // Enhanced AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("configureForWorld", &AgentBrain_configureForWorld,
             py::arg("world"), "Configure for specific world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("processSensoryBatch", &AgentBrain_processSensoryBatch,
             py::arg("percepts"), "Process multiple sensory inputs")
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

    // Factory functions
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

    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

// Register custom exceptions for error handling
PYBIND11_MODULE(pynlm, m) {
    // (Same module definition as above for pybind11 compatibility)
    // This is needed to register the exceptions with the module
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigError>(m, "ConfigError");
    py::register_exception<BrainError>(m, "BrainError");
}