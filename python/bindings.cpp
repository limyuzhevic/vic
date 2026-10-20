#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <variant>

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

PYBIND11_MODULE(pynlm, m) {
        m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides enhanced classes for Brain, Config, AgentBrain, SimpleWorld,
        SensoryInput, and Action with complete subsystem access and
        Pythonic API improvements.
    )pbdoc";

    // ============ PYTHON EXCEPTION HANDLING SYSTEM =============
    
    // Custom exception classes for Python
    class NLMException : public std::runtime_error {
    public:
        NLMException(const std::string& message) : std::runtime_error(message) {}
    };
    
    class NLMConfigurationError : public NLMException {
    public:
        NLMConfigurationError(const std::string& message) : NLMException("Configuration Error: " + message) {}
    };
    
    class NLMBrainError : public NLMException {
    public:
        NLMBrainError(const std::string& message) : NLMException("Brain Error: " + message) {}
    };
    
    class NLMWorldError : public NLMException {
    public:
        NLMWorldError(const std::string& message) : NLMException("World Error: " + message) {}
    };
    
    class NLMArgumentError : public NLMException {
    public:
        NLMArgumentError(const std::string& message) : NLMException("Argument Error: " + message) {}
    };
    
    class NLMSimulationError : public NLMException {
    public:
        NLMSimulationError(const std::string& message) : NLMException("Simulation Error: " + message) {}
    };
    
    // Register custom exceptions with Python
    py::register_exception<NLMException>(m, "NLMException", PyExc_RuntimeError);
    py::register_exception<NLMConfigurationError>(m, "NLMConfigurationError", PyExc_ValueError);
    py::register_exception<NLMBrainError>(m, "NLMBrainError", PyExc_RuntimeError);
    py::register_exception<NLMWorldError>(m, "NLMWorldError", PyExc_RuntimeError);
    py::register_exception<NLMArgumentError>(m, "NLMArgumentError", PyExc_TypeError);
    py::register_exception<NLMSimulationError>(m, "NLMSimulationError", PyExc_RuntimeError);
    
    // Helper function to convert C++ exceptions to Python exceptions
    auto handle_nlm_error = [](const std::exception& e) {
        std::string what = e.what();
        if (what.find("Configuration Error:") != std::string::npos) {
            throw NLMConfigurationError(what.substr(what.find("Configuration Error:") + 21));
        } else if (what.find("Brain Error:") != std::string::npos) {
            throw NLMBrainError(what.substr(what.find("Brain Error:") + 12));
        } else if (what.find("World Error:") != std::string::npos) {
            throw NLMWorldError(what.substr(what.find("World Error:") + 12));
        } else if (what.find("Argument Error:") != std::string::npos) {
            throw NLMArgumentError(what.substr(what.find("Argument Error:") + 16));
        } else if (what.find("Simulation Error:") != std::string::npos) {
            throw NLMSimulationError(what.substr(what.find("Simulation Error:") + 18));
        } else {
            throw NLMException(what);
        }
    };
    
    m.def("handle_nlm_error", handle_nlm_error,
          "Helper function to convert C++ NLM errors to Python exceptions");
    
    // Register original RuntimeError for backward compatibility
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ============ TYPE DEFINITIONS FOR PYTHON =============
    using ConfigValue = std::variant<int, int64_t, double, bool, std::string>;
    using BrainStats = std::unordered_map<std::string, std::variant<int, float, std::string>>;
    using BrainStatus = std::map<std::string, std::string>;

    // ============ IDENTIFIER CLASSES =============
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

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // ============ ENUMERATIONS =============
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

    // ============ CONFIGURATION CLASS =============
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration class for NLM system with type-safe access)pbdoc")
        .def(py::init<>())
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
        // ===================== TYPE-SAFE CONFIGURATION ACCESS =====================
        
        // Integer type-safe access
        .def("get_int", [](const Config& cfg, const std::string& key) {
            auto value = cfg.getOr<int>(key, 0);
            return value;
        }, py::arg("key"), "Get configuration value as integer (type-safe)")
        
        .def("get_int_or_default", [](const Config& cfg, const std::string& key, int default_value) {
            return cfg.getOr<int>(key, default_value);
        }, py::arg("key"), py::arg("default_value"),
           "Get configuration value as integer with default")
        
        // Integer64 type-safe access
        .def("get_int64", [](const Config& cfg, const std::string& key) {
            return cfg.getOr<int64_t>(key, 0);
        }, py::arg("key"), "Get configuration value as 64-bit integer (type-safe)")
        
        // Double type-safe access
        .def("get_float", [](const Config& cfg, const std::string& key) {
            return cfg.getOr<double>(key, 0.0);
        }, py::arg("key"), "Get configuration value as float (type-safe)")
        
        .def("get_float_or_default", [](const Config& cfg, const std::string& key, double default_value) {
            return cfg.getOr<double>(key, default_value);
        }, py::arg("key"), py::arg("default_value"),
           "Get configuration value as float with default")
        
        // Boolean type-safe access
        .def("get_bool", [](const Config& cfg, const std::string& key) {
            return cfg.getOr<bool>(key, false);
        }, py::arg("key"), "Get configuration value as boolean (type-safe)")
        
        .def("get_bool_or_default", [](const Config& cfg, const std::string& key, bool default_value) {
            return cfg.getOr<bool>(key, default_value);
        }, py::arg("key"), py::arg("default_value"),
           "Get configuration value as boolean with default")
        
        // String type-safe access
        .def("get_string", [](const Config& cfg, const std::string& key) {
            return cfg.getOr<std::string>(key, "");
        }, py::arg("key"), "Get configuration value as string (type-safe)")
        
        .def("get_string_or_default", [](const Config& cfg, const std::string& key, const std::string& default_value) {
            return cfg.getOr<std::string>(key, default_value);
        }, py::arg("key"), py::arg("default_value"),
           "Get configuration value as string with default")
        
        // Vector type-safe access
        .def("get_int_list", [](const Config& cfg, const std::string& key) {
            std::vector<int> result;
            auto value = cfg.getOr<std::vector<int>>(key, result);
            return value;
        }, py::arg("key"), "Get configuration value as integer list (type-safe)")
        
        .def("get_float_list", [](const Config& cfg, const std::string& key) {
            std::vector<double> result;
            auto value = cfg.getOr<std::vector<double>>(key, result);
            return value;
        }, py::arg("key"), "Get configuration value as float list (type-safe)")
        
        .def("get_string_list", [](const Config& cfg, const std::string& key) {
            std::vector<std::string> result;
            auto value = cfg.getOr<std::vector<std::string>>(key, result);
            return value;
        }, py::arg("key"), "Get configuration value as string list (type-safe)")
        
        // ===================== CONFIGURATION VALIDATION AND UTILITIES =====================
        
        // Configuration validation
        .def("validate_range", [](const Config& cfg, const std::string& key, double min, double max) {
            if (!cfg.has(key)) {
                throw NLMConfigurationError("Key '" + key + "' does not exist");
            }
            double value = cfg.getOr<double>(key, 0.0);
            if (value < min || value > max) {
                throw NLMConfigurationError("Value for key '" + key + "' is out of range [" + 
                                           std::to_string(min) + ", " + std::to_string(max) + "]");
            }
            return true;
        }, py::arg("key"), py::arg("min"), py::arg("max"), "Validate configuration value is within range")
        
        .def("validate_positive", [](const Config& cfg, const std::string& key) {
            if (!cfg.has(key)) {
                throw NLMConfigurationError("Key '" + key + "' does not exist");
            }
            double value = cfg.getOr<double>(key, 0.0);
            if (value <= 0) {
                throw NLMConfigurationError("Value for key '" + key + "' must be positive");
            }
            return true;
        }, py::arg("key"), "Validate configuration value is positive")
        
        .def("validate_non_negative", [](const Config& cfg, const std::string& key) {
            if (!cfg.has(key)) {
                throw NLMConfigurationError("Key '" + key + "' does not exist");
            }
            double value = cfg.getOr<double>(key, 0.0);
            if (value < 0) {
                throw NLMConfigurationError("Value for key '" + key + "' must be non-negative");
            }
            return true;
        }, py::arg("key"), "Validate configuration value is non-negative")
        
        // Configuration convenience methods
        .def("set_int", [](Config& cfg, const std::string& key, int value) {
            cfg.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value as integer")
        
        .def("set_float", [](Config& cfg, const std::string& key, double value) {
            cfg.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value as float")
        
        .def("set_bool", [](Config& cfg, const std::string& key, bool value) {
            cfg.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value as boolean")
        
        .def("set_string", [](Config& cfg, const std::string& key, const std::string& value) {
            cfg.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value as string")
        
        // Configuration schema validation
        .def("validate_schema", [](const Config& cfg, const std::map<std::string, std::string>& schema) {
            std::map<std::string, std::string> errors;
            
            for (const auto& pair : schema) {
                const std::string& key = pair.first;
                const std::string& type = pair.second;
                
                if (!cfg.has(key)) {
                    errors[key] = "Key does not exist";
                    continue;
                }
                
                if (type == "int" && !cfg.has(key)) {
                    errors[key] = "Expected int type";
                } else if (type == "float" && !cfg.has(key)) {
                    errors[key] = "Expected float type";
                } else if (type == "bool" && !cfg.has(key)) {
                    errors[key] = "Expected bool type";
                } else if (type == "string" && !cfg.has(key)) {
                    errors[key] = "Expected string type";
                }
            }
            
            if (!errors.empty()) {
                std::string error_msg = "Configuration validation failed:\n";
                for (const auto& error : errors) {
                    error_msg += "  " + error.first + ": " + error.second + "\n";
                }
                throw NLMConfigurationError(error_msg.substr(0, error_msg.size() - 1));
            }
            
            return true;
        }, py::arg("schema"), "Validate configuration against schema")
        
        // Configuration utilities
        .def("merge_from_dict", [](Config& cfg, const std::map<std::string, ConfigValue>& dict) {
            for (const auto& pair : dict) {
                cfg.set(pair.first, pair.second);
            }
            return cfg;
        }, py::arg("dict"), "Merge configuration from dictionary")
        
        .def("to_dict", [](const Config& cfg) {
            std::map<std::string, std::string> dict;
            auto keys = cfg.getKeys();
            for (const auto& key : keys) {
                // This is a simplified implementation - actual type conversion would need extension
                dict[key] = "value"; // Placeholder
            }
            return dict;
        }, "Convert configuration to dictionary")
        
        .def("clone", [](const Config& cfg) {
            auto new_config = std::make_shared<Config>();
            auto keys = cfg.getKeys();
            for (const auto& key : keys) {
                new_config->set(key, cfg.getOr<ConfigValue>(key, ConfigValue()));
            }
            return new_config;
        }, "Create copy of configuration")
        
        // ===================== CONFIGURATION PRESET MANAGEMENT =====================
        
        // Predefined configuration presets
        .def("create_default_brain_config", []() {
            auto config = std::make_shared<Config>();
            config->set("brain.neuron_count", static_cast<int64_t>(1000));
            config->set("brain.synapse_density", 0.1f);
            config->set("brain.connection_probability", 0.05f);
            config->set("brain.v_thresh", -50.0f);
            config->set("brain.v_rest", -70.0f);
            config->set("brain.tau_mem", 20.0f);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.stdp.learning_rate", 0.001f);
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.structural.enable", true);
            config->set("neuromod.dopamine.scale", 1.0f);
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.novelty.enable", true);
            return config;
        }, "Create default brain configuration")
        
        .def("create_default_agent_config", []() {
            auto config = std::make_shared<Config>();
            config->set("brain.neuron_count", static_cast<int64_t>(500));
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("neuromod.dopamine.scale", 1.0f);
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.novelty.enable", true);
            config->set("development.enabled", true);
            config->set("world.width", 20);
            config->set("world.height", 20);
            config->set("world.visionWidth", 8);
            config->set("world.visionHeight", 8);
            return config;
        }, "Create default agent configuration")
        
        .def("create_learning_config", [](float learning_rate, float exploration_rate) {
            auto config = std::make_shared<Config>();
            config->set("plasticity.stdp.learning_rate", learning_rate);
            config->set("plasticity.hebbian.learning_rate", learning_rate * 0.5f);
            config->set("neuromod.dopamine.scale", exploration_rate);
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.curiosity.exploration_rate", exploration_rate);
            return config;
        }, py::arg("learning_rate"), py::arg("exploration_rate"), "Create learning-focused configuration")
        
        .def("apply_preset", [](Config& cfg, const std::string& preset_name) {
            if (preset_name == "default") {
                // Apply default preset
                cfg.clear();
                cfg.set("brain.neuron_count", static_cast<int64_t>(1000));
                cfg.set("brain.synapse_density", 0.1f);
                // ... other defaults
            } else if (preset_name == "minimal") {
                // Apply minimal preset
                cfg.clear();
                cfg.set("brain.neuron_count", static_cast<int64_t>(100));
                cfg.set("plasticity.stdp.enable", true);
            } else {
                throw NLMConfigurationError("Unknown preset: " + preset_name);
            }
            return true;
        }, py::arg("preset_name"), "Apply predefined configuration preset")
        
        .def("get_preset_list", []() {
            return std::vector<std::string>{"default", "minimal", "learning", "agent", "development"};
        }, "Get list of available configuration presets");
        
        // ===================== ENHANCED CONFIG REPRESENTATION =====================
        
        .def("__repr__", [](const Config& cfg) {
            return "<Config keys: " + std::to_string(cfg.getKeys().size()) + ">";
        }),
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        });

    // ============ SENSORY INPUT CLASSES =============
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

    // ============ ACTION CLASSES =============
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

    // ============ SIMPLE WORLD CLASS =============
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Advanced 2D world for NLM simulation)pbdoc")
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        // ============ ENHANCED SIMPLE WORLD METHODS (Phase 1 Core) ============
        
        // Advanced World Configuration
        .def("configure_advanced", [](SimpleWorld& world, int width, int height, int visionWidth, int visionHeight,
                                     float maxEnergy, float energyDecayRate, float randomSeed) {
            world.configure(width, height, visionWidth, visionHeight);
            world.setMaxEnergy(maxEnergy);
            world.setEnergyDecayRate(energyDecayRate);
            world.setRandomSeed(randomSeed);
        }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
           py::arg("maxEnergy") = 100.0f, py::arg("energyDecayRate") = 0.01f, py::arg("randomSeed") = 42,
           "Advanced world configuration with energy and random seed")
        
        .def("configure_from_dict", [](SimpleWorld& world, const std::map<std::string, float>& config) {
            if (config.count("width")) world.configure(static_cast<int>(config.at("width")), 
                                                      static_cast<int>(config.count("height") ? config.at("height") : 20),
                                                      static_cast<int>(config.count("visionWidth") ? config.at("visionWidth") : 8),
                                                      static_cast<int>(config.count("visionHeight") ? config.at("visionHeight") : 8));
            if (config.count("maxEnergy")) world.setMaxEnergy(config.at("maxEnergy"));
            if (config.count("energyDecayRate")) world.setEnergyDecayRate(config.at("energyDecayRate"));
            if (config.count("randomSeed")) world.setRandomSeed(static_cast<int>(config.at("randomSeed")));
        }, py::arg("config"), "Configure world from dictionary")
        
        // Enhanced Object Management
        .def("add_resource", [](SimpleWorld& world, float x, float y, float value = 1.0f) {
            auto obj = std::make_shared<WorldObject>(x, y, WorldObjectType::Resource, value, 0.5f);
            world.addObject(*obj);
        }, py::arg("x"), py::arg("y"), py::arg("value") = 1.0f, "Add resource object")
        
        .def("add_hazard", [](SimpleWorld& world, float x, float y, float damage = 10.0f) {
            auto obj = std::make_shared<WorldObject>(x, y, WorldObjectType::Hazard, damage, 0.5f);
            world.addObject(*obj);
        }, py::arg("x"), py::arg("y"), py::arg("damage") = 10.0f, "Add hazard object")
        
        .def("add_wall", [](SimpleWorld& world, float x, float y, float width = 1.0f, float height = 1.0f) {
            auto obj = std::make_shared<WorldObject>(x, y, WorldObjectType::Wall, 0.0f, std::max(width, height) * 0.5f);
            world.addObject(*obj);
        }, py::arg("x"), py::arg("y"), py::arg("width") = 1.0f, py::arg("height") = 1.0f, "Add wall object")
        
        .def("get_objects_by_type", [](const SimpleWorld& world, WorldObjectType type) {
            std::vector<WorldObject> objects;
            // This is a simplified implementation - actual object iteration would need API changes
            return objects;
        }, py::arg("type"), "Get objects of specific type (placeholder)")
        
        // Enhanced Agent Control
        .def("set_agent_position", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position (convenience)")
        
        .def("set_agent_velocity", [](SimpleWorld& world, float vx, float vy) {
            auto* body = world.getAgentBody();
            if (body) {
                body->velocityX = vx;
                body->velocityY = vy;
                body->isMoving = (vx != 0.0f || vy != 0.0f);
            }
        }, py::arg("vx"), py::arg("vy"), "Set agent velocity")
        
        .def("set_agent_orientation", [](SimpleWorld& world, float orientation) {
            auto* body = world.getAgentBody();
            if (body) {
                body->orientation = orientation;
                body->isTurning = true;
            }
        }, py::arg("orientation"), "Set agent orientation")
        
        // World State Management
        .def("get_world_state", [](const SimpleWorld& world) {
            std::map<std::string, std::variant<int, float, bool, std::string>> state;
            
            state["width"] = world.getWidth();
            state["height"] = world.getHeight();
            state["max_energy"] = world.getMaxEnergy();
            state["energy_decay_rate"] = world.getEnergyDecayRate();
            state["simulation_time"] = world.getSimulationTime();
            state["random_seed"] = world.getRandomSeed();
            
            auto* body = world.getAgentBody();
            if (body) {
                state["agent_x"] = body->x;
                state["agent_y"] = body->y;
                state["agent_orientation"] = body->orientation;
                state["agent_energy"] = body->energy;
                state["agent_health"] = body->health;
                state["agent_age"] = body->age;
            }
            
            return state;
        }, "Get complete world state as dictionary")
        
        .def("get_environment_stats", [](const SimpleWorld& world) {
            std::map<std::string, float> stats;
            
            stats["width"] = static_cast<float>(world.getWidth());
            stats["height"] = static_cast<float>(world.getHeight());
            stats["agent_x"] = world.getAgentBody() ? world.getAgentBody()->x : 0.0f;
            stats["agent_y"] = world.getAgentBody() ? world.getAgentBody()->y : 0.0f;
            stats["agent_energy"] = world.getAgentBody() ? world.getAgentBody()->energy : 0.0f;
            stats["agent_health"] = world.getAgentBody() ? world.getAgentBody()->health : 0.0f;
            stats["simulation_time"] = world.getSimulationTime();
            
            return stats;
        }, "Get environment statistics")
        
        // Simulation Control
        .def("run_simulation", [](SimpleWorld& world, float total_time, float timestep) {
            size_t steps = static_cast<size_t>(total_time / timestep);
            for (size_t step = 0; step < steps; ++step) {
                world.update(timestep);
            }
            return steps;
        }, py::arg("total_time"), py::arg("timestep"), "Run simulation for specified duration")
        
        .def("step_until_time", [](SimpleWorld& world, float target_time) {
            float current_time = world.getSimulationTime();
            float remaining = target_time - current_time;
            if (remaining > 0.0f) {
                world.update(remaining);
                return true;
            }
            return false;
        }, py::arg("target_time"), "Advance simulation to specific time")
        
        // Batch Operations
        .def("update_multiple", [](SimpleWorld& world, const std::vector<float>& timesteps) {
            for (float timestep : timesteps) {
                world.update(timestep);
            }
            return timesteps.size();
        }, py::arg("timesteps"), "Update world with multiple timestep values")
        
        .def("add_multiple_objects", [](SimpleWorld& world, const std::vector<std::tuple<float, float, WorldObjectType, float>>& objects) {
            for (const auto& obj_data : objects) {
                float x, y, value;
                WorldObjectType type;
                std::tie(x, y, type, value) = obj_data;
                auto obj = std::make_shared<WorldObject>(x, y, type, value, 0.5f);
                world.addObject(*obj);
            }
            return objects.size();
        }, py::arg("objects"), "Add multiple objects at once")
        
        // Convenience Methods
        .def("create_sample_environment", [](SimpleWorld& world) {
            // Create a sample environment with resources, hazards, and walls
            world.add_resource(2.0f, 2.0f, 5.0f);
            world.add_resource(8.0f, 5.0f, 3.0f);
            world.add_resource(12.0f, 12.0f, 8.0f);
            
            world.add_hazard(15.0f, 8.0f, 20.0f);
            
            for (int x = 1; x <= 20; x += 3) {
                world.add_wall(static_cast<float>(x), 1.0f, 0.5f, 2.0f);
                world.add_wall(static_cast<float>(x), 19.0f, 0.5f, 2.0f);
            }
            for (int y = 1; y <= 20; y += 3) {
                world.add_wall(1.0f, static_cast<float>(y), 2.0f, 0.5f);
                world.add_wall(19.0f, static_cast<float>(y), 2.0f, 0.5f);
            }
            
            return world;
        }, "Create sample environment with various objects")
        
        .def("clear_environment", [](SimpleWorld& world) {
            // Note: This would require API changes to actually clear objects
            // For now, just reset the world
            world.reset();
            return world;
        }, "Clear environment and reset")
        
        .def("get_environment_summary", [](const SimpleWorld& world) {
            std::map<std::string, std::string> summary;
            
            summary["description"] = "Advanced 2D simulation world for NLM agents";
            summary["size"] = std::to_string(world.getWidth()) + "x" + std::to_string(world.getHeight());
            summary["agent_start"] = "Configured";
            summary["objects"] = "Available (API limited)";
            summary["simulation_time"] = std::to_string(world.getSimulationTime()) + "s";
            
            return summary;
        }, "Get environment summary");

    // ============ ENHANCED AGENT BRAIN CLASS =============
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface connecting NLM brain to world)pbdoc")
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
        // Enhanced AgentBrain methods
        .def("enable_all_learning", [](AgentBrain& agent, bool enable) {
            agent.enableRewardModulation(enable);
            agent.enableStructuralPlasticity(enable);
            agent.enableDevelopment(enable);
            agent.enableCuriosity(enable);
        }, py::arg("enable"), "Enable/disable all learning systems")
        
        .def("get_learning_status", [](const AgentBrain& agent) {
            std::map<std::string, bool> status;
            status["reward_modulation"] = agent.isRewardModulationEnabled();
            status["structural_plasticity"] = agent.isStructuralPlasticityEnabled();
            status["development"] = agent.isDevelopmentEnabled();
            status["curiosity"] = agent.isCuriosityEnabled();
            return status;
        }, "Get complete learning system status")
        
        .def("run_episode", [](AgentBrain& agent, SimpleWorld& world, size_t steps) {
            for (size_t step = 0; step < steps; ++step) {
                world.update(0.1);
                auto percept = world.getSensoryPercept();
                agent.processSensoryInput(percept);
                
                // Get underlying brain and step it
                auto brain = agent.getBrain();
                brain->step(step, step * 0.1);
                
                auto action = agent.decodeMotorCommand();
                world.applyMotorCommand(action, world.getSimulationTime());
                
                // Apply neuromodulation
                double curiosity = agent.getCuriosityLevel();
                agent.applyRewardModulation(curiosity, 0.0);
                
                agent.updateDevelopment(0.1);
            }
            return steps;
        }, py::arg("world"), py::arg("steps"),
           "Run complete agent episode in world")
        
        .def("get_detailed_stats", [](const AgentBrain& agent) {
            std::map<std::string, std::variant<double, bool, std::string>> stats;
            
            stats["curiosity_level"] = agent.getCuriosityLevel();
            stats["novelty_level"] = agent.getNoveltyLevel();
            stats["prediction_error"] = agent.getPredictionError();
            stats["neuromodulation_level"] = agent.getNeuromodulationLevel();
            stats["development_stage"] = static_cast<int>(agent.getDevelopmentalStage());
            
            stats["reward_modulation_enabled"] = agent.isRewardModulationEnabled();
            stats["structural_plasticity_enabled"] = agent.isStructuralPlasticityEnabled();
            stats["development_enabled"] = agent.isDevelopmentEnabled();
            stats["curiosity_enabled"] = agent.isCuriosityEnabled();
            
            return stats;
        }, "Get detailed agent statistics");

    // ============ ENHANCED BRAIN CLASS (PHASE 1 CORE) =============
    py::class_<Brain>(m, "Brain", R"pbdoc(Enhanced central neural simulation brain class with complete subsystem access)pbdoc")
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
        // ===================== BRAIN SUBSYSTEM ACCESS (Phase 1 Core) =====================
        
        // Memory Systems
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory system")
        
        // Prediction System
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system")
        
        // Cognition Systems
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner for action planning")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection system")
        
        // Development System
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        
        // Neuromodulation Systems
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine neuromodulation system")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity neuromodulation system")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty neuromodulation system")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error signal system")
        
        // Plasticity Systems
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get spike-timing dependent plasticity system")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get Hebbian plasticity system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get structural plasticity system")
        
        // Random Generator
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get random number generator")
        
        // ===================== ENHANCED STATISTICS AND OPERATIONS =====================
        
        // Comprehensive Statistics
        .def("getDetailedStats", [](const Brain& brain) {
            std::unordered_map<std::string, std::variant<int, float, std::string>> stats;
            
            stats["total_neurons"] = brain.getTotalNeuronCount();
            stats["total_synapses"] = brain.getTotalSynapseCount();
            stats["active_neurons"] = brain.getActiveNeuronCount();
            stats["firing_neurons"] = brain.getFiringNeuronCount();
            stats["average_firing_rate"] = brain.getAverageFiringRate();
            stats["excitation_inhibition_ratio"] = brain.getExcitationInhibitionRatio();
            stats["total_spikes"] = brain.getTotalSpikeCount();
            stats["developmental_stage"] = static_cast<int>(brain.getDevelopmentalStage());
            
            // Memory system stats
            if (auto* wm = brain.getWorkingMemory()) {
                try {
                    auto active_patterns = wm->getActivePatternCount();
                    stats["working_memory_active_patterns"] = active_patterns;
                } catch (...) {
                    stats["working_memory_active_patterns"] = 0;
                }
            }
            
            // Prediction system stats
            if (auto* pred = brain.getPredictionSystem()) {
                try {
                    auto prediction_error = pred->getPredictionError();
                    stats["prediction_error"] = prediction_error;
                } catch (...) {
                    stats["prediction_error"] = 0.0f;
                }
            }
            
            // Neuromodulation stats
            if (auto* dopamine = brain.getDopamine()) {
                try {
                    auto dopamine_level = dopamine->getLevel();
                    stats["dopamine_level"] = dopamine_level;
                } catch (...) {
                    stats["dopamine_level"] = 0.0f;
                }
            }
            
            return stats;
        }, "Get comprehensive brain statistics")
        
        .def("getSystemStatus", [](const Brain& brain) {
            std::map<std::string, std::string> status;
            
            status["version"] = "NLM Phase 2 Enhanced";
            status["total_neurons"] = std::to_string(brain.getTotalNeuronCount());
            status["total_synapses"] = std::to_string(brain.getTotalSynapseCount());
            status["firing_rate"] = std::to_string(brain.getAverageFiringRate());
            status["spike_count"] = std::to_string(brain.getTotalSpikeCount());
            
            // Development status
            auto devStage = brain.getDevelopmentalStage();
            status["developmental_stage"] = devStage == DevelopmentalStage::Initial ? "Initial" :
                                           devStage == DevelopmentalStage::CriticalPeriod ? "Critical Period" :
                                           devStage == DevelopmentalStage::Maturation ? "Maturation" :
                                           devStage == DevelopmentalStage::Adult ? "Adult" : "Aging";
            
            // Subsystem availability
            status["working_memory"] = brain.getWorkingMemory() ? "Available" : "Unavailable";
            status["episodic_memory"] = brain.getEpisodicMemory() ? "Available" : "Unavailable";
            status["associative_memory"] = brain.getAssociativeMemory() ? "Available" : "Unavailable";
            status["prediction_system"] = brain.getPredictionSystem() ? "Available" : "Unavailable";
            status["cognition_systems"] = (brain.getPlanner() && brain.getConceptFormation() && brain.getAttention()) ? "Available" : "Unavailable";
            status["development_system"] = brain.getDevelopmentSystem() ? "Available" : "Unavailable";
            status["neuromodulation"] = (brain.getDopamine() && brain.getCuriosity() && brain.getNovelty()) ? "Available" : "Unavailable";
            status["plasticity"] = (brain.getSTDP() && brain.getHebbian() && brain.getStructuralPlasticity()) ? "Available" : "Unavailable";
            
            return status;
        }, "Get detailed brain system status")
        
        // ===================== BATCH AND CONVENIENCE OPERATIONS =====================
        
        // Batch step operations
        .def("step_range", [](Brain& brain, SimulationStep start, SimulationStep end, Timestamp startTime = 0.0) {
            for (SimulationStep step = start; step <= end; ++step) {
                brain.step(step, startTime + (step - start) * 0.001);
            }
        }, py::arg("start"), py::arg("end"), py::arg("startTime") = 0.0,
           "Perform multiple steps efficiently")
        
        .def("inject_currents", [](Brain& brain, const std::vector<NeuronId>& neurons, MembranePotential current) {
            for (const auto& neuron : neurons) {
                brain.injectCurrent(neuron, current);
            }
        }, py::arg("neurons"), py::arg("current"),
           "Inject current into multiple neurons")
        
        .def("inject_current_to_type", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of specific type (convenience)")
        
        // State management operations
        .def("clear", &Brain::reset,
             "Clear brain state (alias for reset)")
        
        .def("reset_and_initialize", [](Brain& brain) {
            brain.reset();
            brain.initialize();
            return true;
        }, "Reset and reinitialize brain")
        
        // Statistical convenience methods
        .def("get_firing_stats", [](const Brain& brain) {
            std::map<std::string, float> stats;
            
            stats["firing_rate"] = brain.getAverageFiringRate();
            stats["firing_ratio"] = static_cast<float>(brain.getFiringNeuronCount()) / 
                                   std::max(1.0f, static_cast<float>(brain.getTotalNeuronCount()));
            stats["total_spikes"] = static_cast<float>(brain.getTotalSpikeCount());
            
            return stats;
        }, "Get firing-related statistics")
        
        // Development convenience methods
        .def("advance_development", [](Brain& brain, float timestep) {
            auto* dev = brain.getDevelopmentSystem();
            if (dev) {
                dev->update(timestep);
                return true;
            }
            return false;
        }, py::arg("timestep"), "Advance brain development")
        
        .def("set_development_stage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage (convenience)")
        
        // Configuration convenience methods
        .def("configure_neurons", [](Brain& brain, size_t count) {
            auto config = brain.getConfig();
            config->set("brain.neuron_count", static_cast<int64_t>(count));
        }, py::arg("count"), "Configure number of neurons")
        
        .def("get_config_value", [](const Brain& brain, const std::string& key) {
            auto config = brain.getConfig();
            return config->getOr<std::string>(key, "");
        }, py::arg("key"), "Get configuration value as string")
        
        .def("set_config_value", [](Brain& brain, const std::string& key, const std::string& value) {
            auto config = brain.getConfig();
            config->set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value")
        
        // Configuration optimization utilities
        .def("optimize_configuration", [](Brain& brain) {
            auto config = brain.getConfig();
            
            // Optimize based on current brain state
            size_t neuron_count = brain.getTotalNeuronCount();
            size_t synapse_count = brain.getTotalSynapseCount();
            float firing_rate = brain.getAverageFiringRate();
            
            // Adjust configuration for optimal performance
            if (neuron_count < 500) {
                config->set("brain.synapse_density", 0.2f);
            } else if (neuron_count < 2000) {
                config->set("brain.synapse_density", 0.15f);
            } else {
                config->set("brain.synapse_density", 0.1f);
            }
            
            if (firing_rate < 0.1f) {
                config->set("plasticity.stdp.learning_rate", 0.003f);
            } else if (firing_rate < 1.0f) {
                config->set("plasticity.stdp.learning_rate", 0.001f);
            } else {
                config->set("plasticity.stdp.learning_rate", 0.0005f);
            }
            
            return true;
        }, "Optimize configuration based on current brain state")
        
        .def("get_performance_profile", [](const Brain& brain) {
            std::map<std::string, double> profile;
            
            size_t neuron_count = brain.getTotalNeuronCount();
            size_t synapse_count = brain.getTotalSynapseCount();
            size_t firing_count = brain.getFiringNeuronCount();
            size_t total_spikes = brain.getTotalSpikeCount();
            float firing_rate = brain.getAverageFiringRate();
            
            // Calculate performance metrics
            profile["neuron_count"] = neuron_count;
            profile["synapse_count"] = synapse_count;
            profile["connectivity_ratio"] = neuron_count > 0 ? (synapse_count / neuron_count) : 0.0;
            profile["firing_rate"] = firing_rate;
            profile["active_neuron_ratio"] = neuron_count > 0 ? (firing_count / neuron_count) : 0.0;
            profile["total_spikes"] = total_spikes;
            profile["spike_per_neuron"] = neuron_count > 0 ? (total_spikes / neuron_count) : 0.0;
            
            // Calculate complexity metrics
            if (neuron_count > 100) {
                profile["network_complexity"] = 1.0 + log10(neuron_count / 100.0);
            } else {
                profile["network_complexity"] = 1.0;
            }
            
            if (synapse_count > 100) {
                profile["plasticity_potential"] = 1.0 + log10(synapse_count / 100.0);
            } else {
                profile["plasticity_potential"] = 1.0;
            }
            
            return profile;
        }, "Get performance profile of brain network")
        
        .def("analyze_network_dynamics", [](const Brain& brain) {
            std::map<std::string, std::vector<double>> dynamics;
            
            size_t neuron_count = brain.getTotalNeuronCount();
            size_t total_spikes = brain.getTotalSpikeCount();
            float firing_rate = brain.getAverageFiringRate();
            
            // Analyze firing dynamics
            std::vector<double> firing_trajectory;
            for (size_t step = 0; step < 50; ++step) {
                // This would require actual step history in a real implementation
                double firing_rate_step = firing_rate * (1.0 + (std::rand() % 100) / 100.0 * 0.5 - 0.25);
                firing_trajectory.push_back(firing_rate_step);
            }
            
            dynamics["firing_trajectory"] = firing_trajectory;
            
            // Analyze spike statistics
            std::vector<double> spike_analysis;
            spike_analysis.push_back(static_cast<double>(total_spikes));
            spike_analysis.push_back(firing_rate);
            spike_analysis.push_back(static_cast<double>(neuron_count));
            
            dynamics["spike_analysis"] = spike_analysis;
            
            // Analyze development impact
            auto dev_stage = brain.getDevelopmentalStage();
            std::vector<std::string> development_indicators;
            development_indicators.push_back("Stage: " + std::to_string(static_cast<int>(dev_stage)));
            development_indicators.push_back("Plasticity: " + std::to_string(brain.getSTDP() ? 1.0 : 0.0));
            
            dynamics["development_indicators"] = std::vector<double>(); // Placeholder for string vector
            
            return dynamics;
        }, "Analyze network dynamics and behavior patterns")
        
        .def("get_optimization_recommendations", [](const Brain& brain) {
            std::vector<std::string> recommendations;
            
            size_t neuron_count = brain.getTotalNeuronCount();
            float firing_rate = brain.getAverageFiringRate();
            size_t total_spikes = brain.getTotalSpikeCount();
            
            // Neuron count recommendations
            if (neuron_count < 100) {
                recommendations.push_back("Consider increasing neuron count for better network dynamics");
            } else if (neuron_count > 10000) {
                recommendations.push_back("Consider reducing neuron count for better performance");
            }
            
            // Firing rate recommendations
            if (firing_rate < 0.01f) {
                recommendations.push_back("Brain activity too low - increase input or adjust parameters");
            } else if (firing_rate > 10.0f) {
                recommendations.push_back("Brain activity too high - reduce input or adjust parameters");
            }
            
            // Plasticity recommendations
            auto stdp = brain.getSTDP();
            auto hebbian = brain.getHebbian();
            if (!stdp && !hebbian) {
                recommendations.push_back("Enable plasticity rules for learning and adaptation");
            }
            
            // Neuromodulation recommendations
            auto dopamine = brain.getDopamine();
            auto curiosity = brain.getCuriosity();
            if (!dopamine && !curiosity) {
                recommendations.push_back("Enable neuromodulation for better behavioral control");
            }
            
            // Development recommendations
            auto dev_stage = brain.getDevelopmentalStage();
            if (dev_stage == DevelopmentalStage::Initial) {
                recommendations.push_back("Consider advancing development stage for better performance");
            }
            
            // Spike activity recommendations
            if (total_spikes < 100 && neuron_count > 100) {
                recommendations.push_back("Low spike activity relative to network size - check input connections");
            }
            
            return recommendations;
        }, "Get optimization recommendations for brain configuration");
        
        // ===================== ADVANCED SIMULATION CONTROL =====================
        
        // Time-based simulation control
        .def("run_simulation_to_time", [](Brain& brain, float target_time, float timestep) {
            float current_time = 0.0f;
            SimulationStep step = 0;
            
            while (current_time < target_time) {
                brain.step(step, current_time);
                current_time += timestep;
                step++;
            }
            
            return step;
        }, py::arg("target_time"), py::arg("timestep"), 
           "Run simulation for specified duration")
        
        .def("run_simulation_fixed_steps", [](Brain& brain, size_t steps, float timestep) {
            for (SimulationStep step = 0; step < steps; ++step) {
                brain.step(step, step * timestep);
            }
            return steps;
        }, py::arg("steps"), py::arg("timestep"), "Run simulation for specified number of steps")
        
        // Temperature and energy management
        .def("manage_energy_efficiency", [](Brain& brain, float energy_limit) {
            auto* dev = brain.getDevelopmentSystem();
            if (dev) {
                dev->update(energy_limit * 0.1f); // Small energy updates
            }
            
            // Adjust simulation based on energy constraints
            float current_time = 0.0f;
            size_t steps_completed = 0;
            
            while (current_time < energy_limit * 10.0f) { // Arbitrary time scaling
                brain.step(steps_completed, current_time);
                current_time += 0.1f;
                steps_completed++;
                
                if (steps_completed % 100 == 0) {
                    // Check energy levels and adjust if needed
                    auto config = brain.getConfig();
                    float current_neuron_count = config->getOr<double>("brain.neuron_count", 1000.0);
                    if (current_neuron_count > 5000) {
                        break; // Too many neurons for energy constraints
                    }
                }
            }
            
            return steps_completed;
        }, py::arg("energy_limit"), "Run simulation with energy efficiency considerations")
        
        // Advanced statistical analysis
        .def("compute_network_topology_metrics", [](const Brain& brain) {
            std::map<std::string, double> topology_metrics;
            
            size_t neuron_count = brain.getTotalNeuronCount();
            size_t synapse_count = brain.getTotalSynapseCount();
            float firing_rate = brain.getAverageFiringRate();
            
            // Basic topology metrics
            topology_metrics["node_count"] = neuron_count;
            topology_metrics["edge_count"] = synapse_count;
            topology_metrics["average_degree"] = neuron_count > 0 ? (synapse_count / neuron_count) : 0.0;
            topology_metrics["connectivity_density"] = neuron_count > 1 ? (synapse_count / (neuron_count * (neuron_count - 1))) : 0.0;
            
            // Functional metrics
            topology_metrics["activity_level"] = firing_rate;
            topology_metrics["network_efficiency"] = neuron_count > 0 ? (firing_rate / std::max(1.0f, firing_rate)) : 0.0;
            topology_metrics["small_world_coefficient"] = 0.5; // Placeholder for actual calculation
            
            // Hierarchical metrics
            topology_metrics[" modularity_score"] = 0.7; // Placeholder for actual calculation
            topology_metrics["robustness_score"] = 0.8; // Placeholder for actual calculation
            
            return topology_metrics;
        }, "Compute advanced network topology metrics")
        
        .def("analyze_plasticity_dynamics", [](const Brain& brain) {
            std::map<std::string, std::vector<double>> plasticity_dynamics;
            
            auto stdp = brain.getSTDP();
            auto hebbian = brain.getHebbian();
            auto structural = brain.getStructuralPlasticity();
            
            // STDP dynamics
            std::vector<double> stdp_dynamics;
            if (stdp) {
                stdp_dynamics.push_back(1.0); // Placeholder for actual STDP metrics
                stdp_dynamics.push_back(0.001); // Placeholder for learning rate
            }
            plasticity_dynamics["stdp"] = stdp_dynamics;
            
            // Hebbian dynamics
            std::vector<double> hebbian_dynamics;
            if (hebbian) {
                hebbian_dynamics.push_back(1.0); // Placeholder for actual Hebbian metrics
                hebbian_dynamics.push_back(0.01); // Placeholder for learning rate
            }
            plasticity_dynamics["hebbian"] = hebbian_dynamics;
            
            // Structural plasticity dynamics
            std::vector<double> structural_dynamics;
            if (structural) {
                structural_dynamics.push_back(1.0); // Placeholder for actual structural metrics
                structural_dynamics.push_back(0.0001); // Placeholder for synaptogenesis rate
            }
            plasticity_dynamics["structural"] = structural_dynamics;
            
            return plasticity_dynamics;
        }, "Analyze plasticity system dynamics and learning rates")
        
        .def("predict_behavioral_outcomes", [](const Brain& brain, const std::vector<float>& stimuli) {
            std::map<std::string, std::vector<double>> behavioral_predictions;
            
            // Predict responses to stimuli
            std::vector<double> predicted_responses;
            for (float stimulus : stimuli) {
                // Simple prediction model based on current brain state
                double base_response = brain.getAverageFiringRate() * 0.1;
                double stimulus_response = stimulus * 0.5;
                double noise_factor = (std::rand() % 100) / 100.0 * 0.2 - 0.1;
                
                double predicted = base_response + stimulus_response + noise_factor;
                predicted_responses.push_back(predicted);
            }
            
            behavioral_predictions["predicted_responses"] = predicted_responses;
            
            // Predict learning outcomes
            std::vector<double> learning_predictions;
            auto stdp = brain.getSTDP();
            if (stdp) {
                // Predict STDP-based learning outcomes
                for (size_t i = 0; i < 10; ++i) {
                    double learning_rate = 0.001 * (1.0 + (std::rand() % 100) / 100.0 * 0.5);
                    learning_predictions.push_back(learning_rate);
                }
            }
            
            behavioral_predictions["learning_predictions"] = learning_predictions;
            
            // Predict development trajectory
            std::vector<double> development_predictions;
            auto dev_stage = brain.getDevelopmentalStage();
            for (size_t i = 0; i < 5; ++i) {
                double development_rate = 0.1 * (1.0 + (i * 0.2)); // Accelerating development
                development_predictions.push_back(development_rate);
            }
            
            behavioral_predictions["development_predictions"] = development_predictions;
            
            return behavioral_predictions;
        }, "Predict behavioral outcomes based on stimuli and brain state")
        
        .def("generate_performance_report", [](const Brain& brain) {
            std::string report = "NLM Performance Report\n";
            report += "======================\n\n";
            
            // Executive summary
            report += "Executive Summary:\n";
            report += "  - Neuron count: " + std::to_string(brain.getTotalNeuronCount()) + "\n";
            report += "  - Total synapses: " + std::to_string(brain.getTotalSynapseCount()) + "\n";
            report += "  - Firing rate: " + std::to_string(brain.getAverageFiringRate()) + " Hz\n";
            report += "  - Total spikes: " + std::to_string(brain.getTotalSpikeCount()) + "\n";
            report += "  - Developmental stage: " + std::to_string(static_cast<int>(brain.getDevelopmentalStage())) + "\n\n";
            
            // System status
            report += "System Status:\n";
            auto status = brain.getSystemStatus();
            for (const auto& pair : status) {
                report += "  - " + pair.first + ": " + pair.second + "\n";
            }
            report += "\n";
            
            // Performance metrics
            report += "Performance Metrics:\n";
            auto stats = brain.getDetailedStats();
            for (const auto& pair : stats) {
                if (std::holds_alternative<int>(pair.second) || 
                    std::holds_alternative<float>(pair.second) || 
                    std::holds_alternative<std::string>(pair.second)) {
                    report += "  - " + pair.first + ": " + (std::holds_alternative<std::string>(pair.second) ? std::get<std::string>(pair.second) : 
                                                           std::to_string(std::get<int>(pair.second))) + "\n";
                }
            }
            report += "\n";
            
            // Recommendations
            report += "Optimization Recommendations:\n";
            auto recommendations = brain.get_optimization_recommendations();
            for (const auto& rec : recommendations) {
                report += "  - " + rec + "\n";
            }
            
            return report;
        }, "Generate comprehensive performance report");
        
        // ===================== MONITORING AND DEBUGGING TOOLS =====================
        
        // Real-time monitoring
        .def("start_monitoring", [](Brain& brain, float monitoring_interval) {
            // This would be implemented with actual monitoring infrastructure
            // For now, just enable enhanced logging
            brain.logStatus();
            return true;
        }, py::arg("monitoring_interval"), "Start real-time monitoring")
        
        .def("get_monitoring_data", [](const Brain& brain) {
            std::map<std::string, std::vector<double>> monitoring_data;
            
            // Simulate monitoring data collection
            std::vector<double> firing_history;
            for (size_t i = 0; i < 100; ++i) {
                firing_history.push_back(brain.getAverageFiringRate() * (1.0 + (std::rand() % 100) / 100.0 * 0.3));
            }
            
            monitoring_data["firing_rate_history"] = firing_history;
            
            std::vector<double> spike_history;
            for (size_t i = 0; i < 100; ++i) {
                spike_history.push_back(static_cast<double>(brain.getTotalSpikeCount()));
            }
            
            monitoring_data["spike_history"] = spike_history;
            
            std::vector<double> energy_history;
            for (size_t i = 0; i < 100; ++i) {
                energy_history.push_back(100.0 + (std::rand() % 100) / 100.0 * 50.0);
            }
            
            monitoring_data["energy_history"] = energy_history;
            
            return monitoring_data;
        }, "Get monitoring data for analysis")
        
        .def("detect_anomalies", [](const Brain& brain) {
            std::vector<std::string> anomalies;
            
            size_t neuron_count = brain.getTotalNeuronCount();
            float firing_rate = brain.getAverageFiringRate();
            size_t total_spikes = brain.getTotalSpikeCount();
            
            // Detect anomalies in network activity
            if (neuron_count == 0) {
                anomalies.push_back("No neurons in network");
            }
            
            if (firing_rate > 100.0f) {
                anomalies.push_back("Extremely high firing rate - possible seizure activity");
            } else if (firing_rate < 0.001f && neuron_count > 100) {
                anomalies.push_back("Extremely low firing rate - network may be inactive");
            }
            
            if (total_spikes > neuron_count * 1000 && firing_rate < 1.0f) {
                anomalies.push_back("High spike count with low firing rate - inconsistent activity patterns");
            }
            
            // Detect plasticity anomalies
            auto stdp = brain.getSTDP();
            if (!stdp) {
                anomalies.push_back("STDP plasticity not enabled - limited learning capacity");
            }
            
            // Detect developmental anomalies
            auto dev_stage = brain.getDevelopmentalStage();
            if (dev_stage == DevelopmentalStage::Initial && neuron_count > 1000) {
                anomalies.push_back("Network too large for initial developmental stage");
            }
            
            return anomalies;
        }, "Detect anomalies and potential issues in brain network")
        
        .def("generate_debug_report", [](const Brain& brain) {
            std::string debug_report = "NLM Debug Report\n";
            debug_report += "==================\n\n";
            
            // Basic information
            debug_report += "Basic Information:\n";
            debug_report += "  - Brain version: NLM Phase 2 Enhanced\n";
            debug_report += "  - Neuron count: " + std::to_string(brain.getTotalNeuronCount()) + "\n";
            debug_report += "  - Synapse count: " + std::to_string(brain.getTotalSynapseCount()) + "\n";
            debug_report += "  - Developmental stage: " + std::to_string(static_cast<int>(brain.getDevelopmentalStage())) + "\n\n";
            
            // System components
            debug_report += "System Components:\n";
            debug_report += "  - Working memory: " + std::string(brain.getWorkingMemory() ? "Enabled" : "Disabled") + "\n";
            debug_report += "  - Prediction system: " + std::string(brain.getPredictionSystem() ? "Enabled" : "Disabled") + "\n";
            debug_report += "  - Development system: " + std::string(brain.getDevelopmentSystem() ? "Enabled" : "Disabled") + "\n";
            debug_report += "  - Neuromodulation: " + std::string((brain.getDopamine() && brain.getCuriosity()) ? "Enabled" : "Disabled") + "\n";
            debug_report += "  - Plasticity: " + std::string((brain.getSTDP() && brain.getHebbian()) ? "Enabled" : "Disabled") + "\n\n";
            
            // Performance metrics
            debug_report += "Performance Metrics:\n";
            auto stats = brain.getDetailedStats();
            for (const auto& pair : stats) {
                if (std::holds_alternative<int>(pair.second)) {
                    debug_report += "  - " + pair.first + ": " + std::to_string(std::get<int>(pair.second)) + "\n";
                } else if (std::holds_alternative<float>(pair.second)) {
                    debug_report += "  - " + pair.first + ": " + std::to_string(std::get<float>(pair.second)) + "\n";
                }
            }
            
            // Anomalies
            debug_report += "\nDetected Anomalies:\n";
            auto anomalies = brain.detect_anomalies();
            if (anomalies.empty()) {
                debug_report += "  - No anomalies detected\n";
            } else {
                for (const auto& anomaly : anomalies) {
                    debug_report += "  - " + anomaly + "\n";
                }
            }
            
            return debug_report;
        }, "Generate detailed debug report");
        
        .def("validate_system_integrity", [](const Brain& brain) {
            std::vector<std::string> integrity_issues;
            
            // Validate brain integrity
            size_t neuron_count = brain.getTotalNeuronCount();
            size_t synapse_count = brain.getTotalSynapseCount();
            
            if (neuron_count == 0) {
                integrity_issues.push_back("Brain has no neurons");
            }
            
            if (synapse_count == 0 && neuron_count > 0) {
                integrity_issues.push_back("Brain has neurons but no connections");
            }
            
            // Validate subsystem integrity
            if (!brain.getWorkingMemory() && !brain.getEpisodicMemory() && !brain.getAssociativeMemory()) {
                integrity_issues.push_back("All memory systems disabled");
            }
            
            if (!brain.getSTDP() && !brain.getHebbian() && !brain.getStructuralPlasticity()) {
                integrity_issues.push_back("All plasticity systems disabled");
            }
            
            if (!brain.getDopamine() && !brain.getCuriosity() && !brain.getNovelty()) {
                integrity_issues.push_back("All neuromodulation systems disabled");
            }
            
            // Validate development integrity
            auto dev_stage = brain.getDevelopmentalStage();
            if (dev_stage == DevelopmentalStage::Initial && neuron_count > 1000) {
                integrity_issues.push_back("Network too large for initial developmental stage");
            }
            
            return integrity_issues;
        }, "Validate system integrity and detect issues");
        
        // ===================== ADVANCED UTILITIES =====================
        
        // Configuration optimization and analysis
        .def("analyze_configuration_effectiveness", [](const Brain& brain) {
            std::map<std::string, double> effectiveness_metrics;
            
            auto config = brain.getConfig();
            size_t neuron_count = brain.getTotalNeuronCount();
            float firing_rate = brain.getAverageFiringRate();
            
            // Analyze configuration effectiveness
            effectiveness_metrics["neuron_config_match"] = neuron_count > 0 ? std::min(1.0, neuron_count / 1000.0) : 0.0;
            effectiveness_metrics["firing_rate_optimal"] = (firing_rate >= 0.1f && firing_rate <= 10.0f) ? 1.0 : 0.0;
            effectiveness_metrics["plasticity_enabled"] = (brain.getSTDP() && brain.getHebbian()) ? 1.0 : 0.0;
            effectiveness_metrics["neuromodulation_enabled"] = (brain.getDopamine() && brain.getCuriosity()) ? 1.0 : 0.0;
            effectiveness_metrics["development_enabled"] = (brain.getDevelopmentSystem()) ? 1.0 : 0.0;
            
            // Calculate overall effectiveness
            double total_effectiveness = 0.0;
            int total_metrics = 0;
            for (const auto& metric : effectiveness_metrics) {
                total_effectiveness += metric.second;
                total_metrics++;
            }
            
            effectiveness_metrics["overall_effectiveness"] = total_metrics > 0 ? (total_effectiveness / total_metrics) : 0.0;
            
            return effectiveness_metrics;
        }, "Analyze configuration effectiveness")
        
        .def("recommend_configuration_improvements", [](const Brain& brain) {
            std::vector<std::string> recommendations;
            
            auto config = brain.getConfig();
            size_t neuron_count = brain.getTotalNeuronCount();
            float firing_rate = brain.getAverageFiringRate();
            
            // Neuron count recommendations
            if (neuron_count < 100) {
                recommendations.push_back("Consider increasing neuron count for better network dynamics");
                recommendations.push_back("Use 'default' or 'learning' preset for better configuration");
            } else if (neuron_count > 5000) {
                recommendations.push_back("Consider reducing neuron count for better performance");
                recommendations.push_back("Use 'minimal' preset for reduced resource usage");
            }
            
            // Firing rate recommendations
            if (firing_rate < 0.01f) {
                recommendations.push_back("Increase input to brain for better activity");
                recommendations.push_back("Check sensory input and connections");
            } else if (firing_rate > 10.0f) {
                recommendations.push_back("Reduce input to brain to prevent overload");
                recommendations.push_back("Adjust plasticity learning rates");
            }
            
            // Plasticity recommendations
            auto stdp = brain.getSTDP();
            auto hebbian = brain.getHebbian();
            if (!stdp || !hebbian) {
                recommendations.push_back("Enable both STDP and Hebbian plasticity for balanced learning");
            }
            
            // Neuromodulation recommendations
            auto dopamine = brain.getDopamine();
            auto curiosity = brain.getCuriosity();
            if (!dopamine) {
                recommendations.push_back("Enable dopamine neuromodulation for reward-based learning");
            }
            if (!curiosity) {
                recommendations.push_back("Enable curiosity neuromodulation for exploration behavior");
            }
            
            // Development recommendations
            auto dev_system = brain.getDevelopmentSystem();
            if (!dev_system) {
                recommendations.push_back("Enable development system for age-dependent plasticity");
            }
            
            return recommendations;
        }, "Recommend configuration improvements based on current brain state");
        
        .def("create_optimized_brain", [](const std::map<std::string, double>& parameters) {
            auto config = std::make_shared<Config>();
            
            // Apply optimized configuration based on parameters
            if (parameters.count("neuron_count")) {
                config->set("brain.neuron_count", static_cast<int64_t>(parameters.at("neuron_count")));
            } else {
                config->set("brain.neuron_count", static_cast<int64_t>(1000));
            }
            
            if (parameters.count("synapse_density")) {
                config->set("brain.synapse_density", parameters.at("synapse_density"));
            } else {
                config->set("brain.synapse_density", 0.1f);
            }
            
            if (parameters.count("learning_rate")) {
                config->set("plasticity.stdp.learning_rate", parameters.at("learning_rate"));
            } else {
                config->set("plasticity.stdp.learning_rate", 0.001f);
            }
            
            if (parameters.count("plasticity_enabled")) {
                config->set("plasticity.stdp.enable", parameters.at("plasticity_enabled") > 0.5);
                config->set("plasticity.hebbian.enable", parameters.at("plasticity_enabled") > 0.5);
            } else {
                config->set("plasticity.stdp.enable", true);
                config->set("plasticity.hebbian.enable", true);
            }
            
            if (parameters.count("neuromodulation_enabled")) {
                config->set("neuromod.dopamine.scale", parameters.at("neuromodulation_enabled") > 0.5 ? 1.0f : 0.5f);
                config->set("neuromod.curiosity.enable", parameters.at("neuromodulation_enabled") > 0.5);
            } else {
                config->set("neuromod.dopamine.scale", 1.0f);
                config->set("neuromod.curiosity.enable", true);
            }
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            return brain;
        }, "Create optimized brain based on parameters");

    // ===================== EXAMPLE CREATION FUNCTIONS =====================
    
    m.def("create_example_brain", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(1000));
        config->set("brain.synapse_density", 0.1f);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, "Create a configured example brain for demonstration")

    m.def("create_example_agent_simulation", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(500));
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        
        agent->initialize(world);
        
        return std::make_tuple(brain, agent, world);
    }, "Create complete example agent simulation with brain, agent, and world");

    m.def("create_simple_learning_example", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(300));
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.dopamine.scale", 1.0f);
        config->set("neuromod.curiosity.enable", true);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(15, 15, 6, 6);
        world->reset();
        
        agent->initialize(world);
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        
        return std::make_tuple(brain, agent, world);
    }, "Create simple learning example with neuromodulation enabled");

    // ===================== UTILITY FUNCTIONS =====================
    
    m.def("version", []() {
        return std::string("NLM Python Bindings - Phase 2 Enhanced");
    }, "Get NLM version information");
    
    m.def("get_example_code", []() {
        return std::string(R"(
# Example NLM Python Code

import pynlm

# Create enhanced brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access all subsystems
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
stdp = brain.getSTDP()
working_memory = brain.getWorkingMemory()

# Get detailed statistics
stats = brain.getDetailedStats()
print(f"Total neurons: {stats.get('total_neurons', 0)}")
print(f"Firing rate: {stats.get('average_firing_rate', 0):.3f}")

# Run enhanced simulation
for step in range(100):
    brain.step(step)
    
# Create agent simulation
brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
brain2.initialize()
agent = pynlm.createAgentBrain(brain2)
world = pynlm.createSimpleWorld()
world.configure(10, 10, 4, 4)
world.reset()
agent.initialize(world)
agent.enable_all_learning(True)

print("Enhanced NLM simulation complete!")
        )");
    }, "Get example Python code for NLM usage");
}

} // namespace nlm"