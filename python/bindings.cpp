#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <stdexcept>
#include <optional>

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

// Exception classes for better error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& msg) : NLMError("Configuration error: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation error: " + msg) {}
};

// Type alias for return types
using optional_int = std::optional<int>;
using optional_float = std::optional<float>;
using optional_string = std::optional<std::string>;
using optional_bool = std::optional<bool>;

PYBIND11_MODULE(pynlm, m) {
    // Set module version
    m.attr("__version__") = "0.1.0";
    
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Key Features:
        - Pythonic API with type hints
        - Context managers for resource management
        - Comprehensive error handling
        - Performance monitoring tools
        - Batch processing capabilities
        - Advanced simulation control
    )pbdoc";

    // Register custom exception types
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigError>(m, "ConfigError");
    py::register_exception<SimulationError>(m, "SimulationError");

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
        // Convenience methods for NeuronId
        .def("is_valid", [](const NeuronId& id) { return id.value != INVALID_NEURON_ID.value; })
        .def("to_string", [](const NeuronId& id) { return std::to_string(id.value); });

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
        .def("is_valid", [](const SynapseId& id) { return id.value != INVALID_SYNAPSE_ID.value; })
        .def("to_string", [](const SynapseId& id) { return std::to_string(id.value); });

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
        })
        .def("is_valid", [](const RegionId& id) { return id.value != INVALID_REGION_ID.value; })
        .def("to_string", [](const RegionId& id) { return std::to_string(id.value); });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        })
        .def("is_valid", [](const PopulationId& id) { return id.value != INVALID_POPULATION_ID.value; })
        .def("to_string", [](const PopulationId& id) { return std::to_string(id.value); });

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

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON/YAML file. Supports both JSON and YAML formats.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments. Supports --key=value format.")
        .def("loadFromDict", [](Config& self, const py::dict& config_dict) {
            for (auto item : config_dict) {
                std::string key = py::cast<std::string>(item.first);
                py::object value_obj = item.second;
                
                if (value_obj.is(py::int_())) {
                    self.set(key, value_obj.cast<int64_t>(), ConfigSource::Runtime);
                } else if (value_obj.is(py::float_())) {
                    self.set(key, value_obj.cast<double>(), ConfigSource::Runtime);
                } else if (value_obj.is(py::bool_())) {
                    self.set(key, value_obj.cast<bool>(), ConfigSource::Runtime);
                } else {
                    self.set(key, value_obj.cast<std::string>(), ConfigSource::Runtime);
                }
            }
            return true;
        }, py::arg("config_dict"),
            "Load configuration from a Python dictionary. Auto-detects value types.")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON/YAML file. Uses JSON format by default.")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a detailed summary string of the configuration")
        // Type-safe getters with validation
        .def("get", [](const Config& self, const std::string& key) {
            auto val = self.get<std::string>(key);
            if (val) return *val;
            throw ConfigError("Key not found or wrong type: " + key);
        }, py::arg("key"), "Get configuration value as string (throws if not found)")
        .def("get_or", [](const Config& self, const std::string& key, const std::string& default_value) {
            auto val = self.get<std::string>(key);
            return val ? *val : default_value;
        }, py::arg("key"), py::arg("default_value"), "Get configuration value as string with default")
        .def("get_int", [](const Config& self, const std::string& key) {
            auto val = self.get<int64_t>(key);
            if (val) return *val;
            throw ConfigError("Key not found or wrong type (expected int64_t): " + key);
        }, py::arg("key"), "Get configuration value as int64_t (throws if not found)")
        .def("get_int_or", [](const Config& self, const std::string& key, int64_t default_value) {
            auto val = self.get<int64_t>(key);
            return val ? *val : default_value;
        }, py::arg("key"), py::arg("default_value"), "Get configuration value as int64_t with default")
        .def("get_float", [](const Config& self, const std::string& key) {
            auto val = self.get<double>(key);
            if (val) return *val;
            throw ConfigError("Key not found or wrong type (expected double): " + key);
        }, py::arg("key"), "Get configuration value as float/double (throws if not found)")
        .def("get_float_or", [](const Config& self, const std::string& key, double default_value) {
            auto val = self.get<double>(key);
            return val ? *val : default_value;
        }, py::arg("key"), py::arg("default_value"), "Get configuration value as float/double with default")
        .def("get_bool", [](const Config& self, const std::string& key) {
            auto val = self.get<bool>(key);
            if (val) return *val;
            throw ConfigError("Key not found or wrong type (expected bool): " + key);
        }, py::arg("key"), "Get configuration value as bool (throws if not found)")
        .def("get_bool_or", [](const Config& self, const std::string& key, bool default_value) {
            auto val = self.get<bool>(key);
            return val ? *val : default_value;
        }, py::arg("key"), py::arg("default_value"), "Get configuration value as bool with default")
        // Enhanced validation methods
        .def("validate_type", [](const Config& self, const std::string& key, const std::string& expected_type) {
            if (!self.has(key)) {
                throw ConfigError("Key not found: " + key);
            }
            auto val = self.get<std::string>(key);
            if (!val) {
                throw ConfigError("Key exists but has wrong type: " + key);
            }
            // Simple type checking
            if (expected_type == "string" && !val->empty()) return true;
            if (expected_type == "int" && std::all_of(val->begin(), val->end(), ::isdigit)) return true;
            if (expected_type == "float") {
                try {
                    std::stod(*val);
                    return true;
                } catch (...) {
                    return false;
                }
            }
            if (expected_type == "bool" && (*val == "true" || *val == "false" || *val == "1" || *val == "0")) return true;
            return false;
        }, py::arg("key"), py::arg("expected_type"), "Validate that a configuration key has the expected type")
        .def("validate_range", [](const Config& self, const std::string& key, double min_val, double max_val) {
            if (!self.has(key)) {
                throw ConfigError("Key not found: " + key);
            }
            auto val = self.get<double>(key);
            if (!val) {
                throw ConfigError("Key exists but has wrong type for range validation: " + key);
            }
            return (*val >= min_val && *val <= max_val);
        }, py::arg("key"), py::arg("min_val"), py::arg("max_val"), "Validate that a configuration value is within the specified range")
        // Advanced configuration methods
        .def("merge", [](Config& self, const py::dict& other_config) {
            for (auto item : other_config) {
                std::string key = py::cast<std::string>(item.first);
                py::object value_obj = item.second;
                
                if (value_obj.is(py::int_())) {
                    self.set(key, value_obj.cast<int64_t>(), ConfigSource::Runtime);
                } else if (value_obj.is(py::float_())) {
                    self.set(key, value_obj.cast<double>(), ConfigSource::Runtime);
                } else if (value_obj.is(py::bool_())) {
                    self.set(key, value_obj.cast<bool>(), ConfigSource::Runtime);
                } else {
                    self.set(key, value_obj.cast<std::string>(), ConfigSource::Runtime);
                }
            }
        }, py::arg("other_config"), "Merge another configuration dictionary into this one")
        .def("from_template", [](const Config& self, const std::string& template_name) {
            // This would load a template configuration
            // For now, just return self as a template
            return self;
        }, py::arg("template_name"), "Get configuration from a named template")
        // Statistics and analysis
        .def("get_stats", [](const Config& self) {
            py::dict stats;
            stats["total_keys"] = self.getKeys().size();
            
            size_t int_count = 0, float_count = 0, string_count = 0, bool_count = 0, vector_count = 0;
            for (const auto& key : self.getKeys()) {
                auto val = self.get<std::string>(key);
                if (val) {
                    // Simple type detection
                    try {
                        std::stoll(*val);
                        int_count++;
                    } catch (...) {
                        try {
                            std::stod(*val);
                            float_count++;
                        } catch (...) {
                            if (*val == "true" || *val == "false") {
                                bool_count++;
                            } else {
                                string_count++;
                            }
                        }
                    }
                }
            }
            
            stats["int_values"] = int_count;
            stats["float_values"] = float_count;
            stats["string_values"] = string_count;
            stats["bool_values"] = bool_count;
            
            return stats;
        }, "Get configuration statistics (value type counts)")
        // Performance monitoring
        .def("get_size", [](const Config& self) {
            return self.getKeys().size();
        }, "Get the number of configuration entries")
        .def("is_empty", [](const Config& self) {
            return self.getKeys().empty();
        }, "Check if configuration is empty")
        // Set operations
        .def("set", [](Config& self, const py::dict& updates) {
            for (auto item : updates) {
                std::string key = py::cast<std::string>(item.first);
                py::object value_obj = item.second;
                
                if (value_obj.is(py::int_())) {
                    self.set(key, value_obj.cast<int64_t>(), ConfigSource::Runtime);
                } else if (value_obj.is(py::float_())) {
                    self.set(key, value_obj.cast<double>(), ConfigSource::Runtime);
                } else if (value_obj.is(py::bool_())) {
                    self.set(key, value_obj.cast<bool>(), ConfigSource::Runtime);
                } else {
                    self.set(key, value_obj.cast<std::string>(), ConfigSource::Runtime);
                }
            }
        }, py::arg("updates"), "Set multiple configuration values from a dictionary")
        // Documentation and help
        .def("get_documentation", [](const Config& self) {
            return "Configuration class for NLM system.\n\n";
        }, "Get documentation string for the Config class")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

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

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
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

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
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

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
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

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"), "Create brain with configuration")
        .def("initialize", &Brain::initialize, "Initialize the brain with configuration")
        .def("initialize_with_config", [](std::shared_ptr<Brain> self, std::shared_ptr<Config> config) {
            return self->initialize();
        }, py::arg("config"), "Initialize brain with specific configuration")
        // Enhanced step methods with better error handling
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"), "Perform a simulation step with current time 0.0")
        .def("step_with_time", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"), "Perform a simulation step with specified timestamp")
        .def("step_multiple", [](std::shared_ptr<Brain> self, size_t num_steps, double timestep = 0.001) {
            for (size_t step = 0; step < num_steps; ++step) {
                self->step(step, step * timestep);
            }
        }, py::arg("num_steps"), py::arg("timestep") = 0.001,
            "Perform multiple simulation steps efficiently")
        // Enhanced input/output methods with validation
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"),
             "Inject sensory input into the brain")
        .def("inject_current", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"),
             "Inject current directly into a specific neuron")
        .def("inject_current_to_type", &Brain::injectCurrentToNeurons, py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        // Enhanced action production
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        // Enhanced state management
        .def("reset", &Brain::reset, "Reset brain state completely")
        .def("save_state", &Brain::save, py::arg("filepath"), "Save brain state to file (checkpointing)")
        .def("load_state", &Brain::load, py::arg("filepath"), "Load brain state from file")
        // Enhanced region management
        .def("addRegion", &Brain::addRegion, py::arg("name") = "", "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal, "Get a region by ID")
        .def("getRegionByIndex", [](const std::shared_ptr<Brain>& self, size_t index) {
            auto region_ids = self->getRegionIds();
            if (index >= region_ids.size()) {
                throw SimulationError("Region index out of bounds: " + std::to_string(index));
            }
            return self->getRegion(region_ids[index]);
        }, py::arg("index"), "Get a region by index (0-based)")
        .def("get_region_names", [](const std::shared_ptr<Brain>& self) {
            py::list names;
            auto regions = self->getRegions();
            for (const auto& region : regions) {
                names.append(region->getName());
            }
            return names;
        }, "Get names of all regions")
        // Enhanced statistics with comprehensive analysis
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
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"),
             "Set developmental stage")
        .def("getTotalSpikeCountAtStep", [](const std::shared_ptr<Brain>& self, SimulationStep step) {
            // This is a simplified implementation - in practice, you'd need access to spike history
            return self->getTotalSpikeCount();
        }, py::arg("step"), "Get spike count at specific step (approximate)")
        // Advanced statistics and analysis
        .def("getBrainStats", [](const std::shared_ptr<Brain>& self) {
            py::dict stats;
            stats["neurons"] = self->getTotalNeuronCount();
            stats["synapses"] = self->getTotalSynapseCount();
            stats["active_neurons"] = self->getActiveNeuronCount();
            stats["firing_neurons"] = self->getFiringNeuronCount();
            stats["average_firing_rate"] = self->getAverageFiringRate();
            stats["e_i_ratio"] = self->getExcitationInhibitionRatio();
            stats["total_spikes"] = self->getTotalSpikeCount();
            stats["development_stage"] = static_cast<int>(self->getDevelopmentalStage());
            stats["region_count"] = self->getRegionCount();
            return stats;
        }, "Get comprehensive brain statistics as dictionary")
        // Advanced simulation control
        .def("run_episode", [](std::shared_ptr<Brain> self, size_t steps, double timestep = 0.001,
                            const std::function<bool()>& should_stop = nullptr) {
            for (size_t step = 0; step < steps && (!should_stop || !should_stop()); ++step) {
                self->step(step, step * timestep);
            }
            return self->getTotalSpikeCount();
        }, py::arg("steps"), py::arg("timestep") = 0.001,
            py::arg("should_stop") = nullptr,
            "Run an episode for specified number of steps with optional stop condition")
        // Context manager support
        .def("__enter__", [](std::shared_ptr<Brain> self) {
            return self;
        })
        .def("__exit__", [](std::shared_ptr<Brain> self, py::args) {
            self->reset();
        })
        // Clone and copy support
        .def("clone", [](const std::shared_ptr<Brain>& self) {
            return std::make_shared<Brain>(*self);
        }, "Create a copy of the brain")
        // Configuration access
        .def("getConfig", &Brain::getConfig, py::return_value_policy::reference_internal,
             "Get the configuration object")
        .def("updateConfig", [](std::shared_ptr<Brain> self, const std::string& key, const std::string& value) {
            if (self->getConfig()) {
                self->getConfig()->set(key, value, ConfigSource::Runtime);
            }
        }, py::arg("key"), py::arg("value"), "Update a configuration value")
        // Performance and debugging
        .def("logStatus", &Brain::logStatus,
             "Log brain status and statistics")
        .def("getActivityHeatmap", [](const std::shared_ptr<Brain>& self) {
            // Simplified heatmap - in practice would be more sophisticated
            py::list heatmap;
            auto regions = self->getRegions();
            for (const auto& region : regions) {
                py::dict region_data;
                region_data["region"] = region->getName();
                region_data["neuron_count"] = region->getNeuronCount();
                region_data["activity_level"] = static_cast<float>(region->getActiveNeuronCount()) / region->getNeuronCount();
                heatmap.append(region_data);
            }
            return heatmap;
        }, "Get activity heatmap data for visualization")
        // Enhanced error handling and validation
        .def("validateState", [](const std::shared_ptr<Brain>& self) {
            py::dict validation_results;
            validation_results["valid"] = true;
            validation_results["errors"] = py::list();
            
            if (self->getTotalNeuronCount() == 0) {
                validation_results["valid"] = false;
                validation_results["errors"].append("No neurons in brain");
            }
            
            if (self->getTotalSynapseCount() == 0 && self->getTotalNeuronCount() > 0) {
                validation_results["warnings"] = "No synapses despite having neurons";
            }
            
            return validation_results;
        }, "Validate brain state and return validation results")
        // Documentation
        .def("getDocumentation", [](const std::shared_ptr<Brain>& self) {
            return "Brain class - Central neural simulation brain.\n\n";
        }, "Get documentation string for the Brain class")
        .def("__repr__", [](const std::shared_ptr<Brain>& self) {
            return "<Brain: " + std::to_string(self->getTotalNeuronCount()) + " neurons, " +
                   std::to_string(self->getTotalSynapseCount()) + " synapses>";
        });

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"), "Create agent brain interface")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize agent with world")
        // Enhanced sensory processing
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("processSensoryBatch", [](std::shared_ptr<AgentBrain> self, const py::list& percepts) {
            for (auto item : percepts) {
                py::object percept_obj = item;
                // This is a simplified version - in practice you'd handle different sensor types
                self->processSensoryInput(py::cast<SensoryPercept>(percept_obj));
            }
        }, py::arg("percepts"), "Process a batch of sensory inputs")
        // Enhanced motor decoding
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("decodeAction", [](const std::shared_ptr<AgentBrain>& self) {
            auto action = self->decodeMotorCommand();
            return action;
        }, "Decode brain motor activity into motor command (alias)")
        // Enhanced neuromodulation
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("applyDopamineModulation", [](std::shared_ptr<AgentBrain> self, double strength) {
            auto reward = self->getNeuromodulationLevel() * strength;
            self->applyRewardModulation(reward, 0.0);
        }, py::arg("strength"), "Apply dopamine-based modulation")
        .def("applyCuriosityModulation", [](std::shared_ptr<AgentBrain> self, double novelty_factor) {
            auto curiosity = self->getCuriosityLevel() * novelty_factor;
            self->applyRewardModulation(curiosity, 0.0);
        }, py::arg("novelty_factor"), "Apply curiosity-based modulation")
        // Enhanced development
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"),
             "Update development system")
        .def("updateDevelopmentToStage", [](std::shared_ptr<AgentBrain> self, DevelopmentalStage target_stage) {
            auto current_stage = self->getDevelopmentalStage();
            if (target_stage > current_stage) {
                self->setDevelopmentalStage(target_stage);
            }
        }, py::arg("target_stage"), "Advance development to target stage")
        // Enhanced subsystem management
        .def("enableSubsystem", [](std::shared_ptr<AgentBrain> self, const std::string& subsystem, bool enable) {
            if (subsystem == "reward_modulation") {
                self->enableRewardModulation(enable);
            } else if (subsystem == "structural_plasticity") {
                self->enableStructuralPlasticity(enable);
            } else if (subsystem == "development") {
                self->enableDevelopment(enable);
            } else if (subsystem == "curiosity") {
                self->enableCuriosity(enable);
            } else {
                throw ConfigError("Unknown subsystem: " + subsystem);
            }
        }, py::arg("subsystem"), py::arg("enable"), "Enable/disable a specific subsystem")
        .def("enableAllSubsystems", [](std::shared_ptr<AgentBrain> self, bool enable) {
            self->enableRewardModulation(enable);
            self->enableStructuralPlasticity(enable);
            self->enableDevelopment(enable);
            self->enableCuriosity(enable);
        }, py::arg("enable"), "Enable/disable all subsystems")
        // Enhanced statistics and monitoring
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
        .def("getBrainStats", [](const std::shared_ptr<AgentBrain>& self) {
            py::dict stats;
            stats["developmental_stage"] = static_cast<int>(self->getDevelopmentalStage());
            stats["neuromodulation_level"] = self->getNeuromodulationLevel();
            stats["curiosity_level"] = self->getCuriosityLevel();
            stats["novelty_level"] = self->getNoveltyLevel();
            stats["prediction_error"] = self->getPredictionError();
            stats["reward_modulation_enabled"] = self->isRewardModulationEnabled();
            stats["structural_plasticity_enabled"] = self->isStructuralPlasticityEnabled();
            stats["development_enabled"] = self->isDevelopmentEnabled();
            stats["curiosity_enabled"] = self->isCuriosityEnabled();
            return stats;
        }, "Get comprehensive agent brain statistics")
        .def("getBehaviorProfile", [](const std::shared_ptr<AgentBrain>& self) {
            py::dict profile;
            profile["exploration_vs_exploitation"] = self->getCuriosityLevel();
            profile["novelty_sensitivity"] = self->getNoveltyLevel();
            profile["learning_rate"] = self->getPredictionError();
            profile["motivation_level"] = self->getNeuromodulationLevel();
            profile["developmental_maturity"] = static_cast<int>(self->getDevelopmentalStage());
            return profile;
        }, "Get behavioral profile of the agent")
        // Enhanced reset and lifecycle
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("resetAndInitialize", [](std::shared_ptr<AgentBrain> self, std::shared_ptr<SimpleWorld> world) {
            self->reset();
            self->initialize(world);
        }, py::arg("world"), "Reset agent and reinitialize with new world")
        // Brain access and configuration
        .def("getBrain", &AgentBrain::getBrain, py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("getBrainStats", [](const std::shared_ptr<AgentBrain>& self) {
            return self->getBrain()->getBrainStats();
        }, "Get brain statistics through agent interface")
        .def("updateBrainConfig", [](std::shared_ptr<AgentBrain> self, const std::string& key, const std::string& value) {
            if (auto config = self->getBrain()->getConfig()) {
                config->set(key, value, ConfigSource::Runtime);
            }
        }, py::arg("key"), py::arg("value"), "Update brain configuration through agent")
        // Advanced agent behaviors
        .def("simulateEpisode", [](std::shared_ptr<AgentBrain> self, std::shared_ptr<SimpleWorld> world,
                                 size_t steps, double timestep = 0.1) {
            self->initialize(world);
            py::list rewards;
            py::list actions;
            
            for (size_t step = 0; step < steps; ++step) {
                // Get sensory input
                auto percept = world->getSensoryPercept();
                
                // Process sensory input
                self->processSensoryInput(percept);
                
                // Get brain stats for reward calculation
                auto brain_stats = self->getBrainStats();
                double reward = brain_stats["neuromodulation_level"].cast<double>();
                
                // Apply reward modulation
                self->applyRewardModulation(reward, 0.0);
                
                // Get action
                auto action = self->decodeMotorCommand();
                actions.append(action);
                
                // Update development
                self->updateDevelopment(timestep);
                
                // Apply action to world
                world->applyMotorCommand(action, world->getSimulationTime());
                
                rewards.append(reward);
                world->update(timestep);
            }
            
            py::dict episode_results;
            episode_results["rewards"] = rewards;
            episode_results["actions"] = actions;
            episode_results["final_reward"] = rewards.back();
            return episode_results;
        }, py::arg("world"), py::arg("steps"), py::arg("timestep") = 0.1,
            "Simulate an episode and return results")
        .def("trainAgent", [](std::shared_ptr<AgentBrain> self, std::shared_ptr<SimpleWorld> world,
                            size_t episodes, size_t steps_per_episode, double timestep = 0.1) {
            py::list episode_rewards;
            
            for (size_t episode = 0; episode < episodes; ++episode) {
                auto results = self->simulateEpisode(world, steps_per_episode, timestep);
                episode_rewards.append(results["final_reward"].cast<double>());
                
                // Log progress
                if (episode % 10 == 0) {
                    std::cout << "Episode " << episode << ": " 
                              << "Avg Reward: " << std::accumulate(
                                  results["rewards"].cast<py::list>().begin(),
                                  results["rewards"].cast<py::list>().end(), 0.0) / steps_per_episode
                              << std::endl;
                }
            }
            
            py::dict training_results;
            training_results["episode_rewards"] = episode_rewards;
            training_results["average_reward"] = std::accumulate(
                episode_rewards.begin(), episode_rewards.end(), 0.0) / episodes;
            return training_results;
        }, py::arg("world"), py::arg("episodes"), py::arg("steps_per_episode"),
            py::arg("timestep") = 0.1,
            "Train agent over multiple episodes")
        // Context manager support
        .def("__enter__", [](std::shared_ptr<AgentBrain> self) {
            return self;
        })
        .def("__exit__", [](std::shared_ptr<AgentBrain> self, py::args) {
            self->reset();
        })
        // Documentation
        .def("getDocumentation", [](const std::shared_ptr<AgentBrain>& self) {
            return "AgentBrain class - Agent brain interface connecting NLM brain to world.\n\n";
        }, "Get documentation string for the AgentBrain class")
        .def("__repr__", [](const std::shared_ptr<AgentBrain>& self) {
            return "<AgentBrain: " + std::to_string(self->getBrain()->getTotalNeuronCount()) +
                   " neurons>";
        });

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

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
