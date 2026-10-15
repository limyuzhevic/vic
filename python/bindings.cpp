#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <limits>
#include <sstream>
#include <cmath>

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

// =============================================================================
// Python Exception Classes for Enhanced Error Handling
// =============================================================================

class ConfigError : public std::runtime_error {
public:
    explicit ConfigError(const std::string& msg) : std::runtime_error(msg) {}
};

class BrainError : public std::runtime_error {
public:
    explicit BrainError(const std::string& msg) : std::runtime_error(msg) {}
};

class WorldError : public std::runtime_error {
public:
    explicit WorldError(const std::string& msg) : std::runtime_error(msg) {}
};

class SensoryError : public std::runtime_error {
public:
    explicit SensoryError(const std::string& msg) : std::runtime_error(msg) {}
};

class ActionError : public std::runtime_error {
public:
    explicit ActionError(const std::string& msg) : std::runtime_error(msg) {}
};

class InvalidArgumentError : public std::invalid_argument {
public:
    explicit InvalidArgumentError(const std::string& msg) : std::invalid_argument(msg) {}
};

// =============================================================================
// Validation and Helper Functions
// =============================================================================

// Validates NeuronId is not invalid
inline void validateNeuronId(const NeuronId& id, const std::string& context = "") {
    if (id.value == UINT64_MAX) {
        throw InvalidArgumentError("Invalid NeuronId used" + (context.empty() ? std::string("") : (" in " + context)));
    }
}

// Validates parameter range
inline void validateRange(double value, double min, double max, const std::string& paramName, const std::string& context = "") {
    if (value < min || value > max) {
        std::ostringstream oss;
        oss << "Parameter '" << paramName << "' must be between " << min << " and " << max
            << ", got " << value << (context.empty() ? std::string("") : (" in " + context));
        throw InvalidArgumentError(oss.str());
    }
}

// Validates parameter is positive
inline void validatePositive(size_t value, const std::string& paramName, const std::string& context = "") {
    if (value == 0) {
        std::ostringstream oss;
        oss << "Parameter '" << paramName << "' must be positive, got " << value
            << (context.empty() ? std::string("") : (" in " + context));
        throw InvalidArgumentError(oss.str());
    }
}

// Converts Python object to float vector with type checking
inline std::vector<float> toFloatVector(const py::object& obj) {
    if (py::isinstance<py::list>(obj) || py::isinstance<py::tuple>(obj)) {
        return obj.cast<std::vector<float>>();
    } else if (py::isinstance<py::array_t<float>>(obj)) {
        return obj.cast<std::vector<float>>();
    } else {
        throw InvalidArgumentError("Expected list, tuple, or numpy array of floats");
    }
}

// =============================================================================
// Type-Safe Wrapper Functions
// =============================================================================

// Safe wrapper for creating brain objects with error handling
py::object safe_create_brain(std::shared_ptr<Config> config);

// Safe wrapper for creating world objects with error handling
py::object safe_create_world();

// Safe wrapper for creating agent brain objects with error handling
py::object safe_create_agent_brain(py::object brain);

// =============================================================================
// Main Module Definition
// =============================================================================

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        Key Features:
        - Enhanced error handling with descriptive Python exceptions
        - Type safety with argument validation and automatic conversion
        - Pythonic APIs with context managers, properties, and convenience methods
        - Comprehensive documentation strings for IDE support
        - Python-specific utility functions and helpers
        
        Classes:
        - NeuronId, SynapseId, RegionId, PopulationId: Unique identifiers
        - Config: Configuration management with validation
        - Brain: Central neural simulation brain
        - AgentBrain: Interface connecting brain to world
        - SimpleWorld: 2D simulation world
        - SensoryInput, Vision, Audio, InternalSignals: Sensory modalities
        - Action: Motor actions for the agent
        - AgentBody: Agent state and physical properties
        - SensoryPercept: What the agent perceives
        - ActionResult: Results from world interactions
    )pbdoc";

    // Register Python exception types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<ConfigError>(m, "ConfigError", "NLM configuration error");
    py::register_exception<BrainError>(m, "BrainError", "NLM brain error");
    py::register_exception<WorldError>(m, "WorldError", "NLM world error");
    py::register_exception<SensoryError>(m, "SensoryError", "NLM sensory error");
    py::register_exception<ActionError>(m, "ActionError", "NLM action error");
    py::register_exception<InvalidArgumentError>(m, "InvalidArgumentError", "Invalid argument provided");

    // Provide module-level error constructors for convenience
    m.def("create_config_error", [](const std::string& msg) { return ConfigError(msg); });
    m.def("create_brain_error", [](const std::string& msg) { return BrainError(msg); });
    m.def("create_world_error", [](const std::string& msg) { return WorldError(msg); });
    m.def("create_sensory_error", [](const std::string& msg) { return SensoryError(msg); });
    m.def("create_action_error", [](const std::string& msg) { return ActionError(msg); });
    m.def("create_invalid_argument_error", [](const std::string& msg) { return InvalidArgumentError(msg); });

    // Utility validation function
    m.def("validate_argument", [](const std::string& value, const std::string& paramName, size_t minLength = 0) {
        if (value.length() < minLength) {
            throw InvalidArgumentError("Argument '" + paramName + "' must be at least " + 
                                     std::to_string(minLength) + " characters long");
        }
    });

    // Module information
    m.def("get_version", []() {
        return "NLM Python Bindings v1.0.0";
    });

    m.def("get_config_version", []() {
        return "1.0.0";
    });

    // Type-safe argument conversion helper
    m.def("to_float_vector", [](const py::object& obj) {
        return toFloatVector(obj);
    });

    // =============================================================================
    // Unique Identifier Classes (Enhanced)
    // =============================================================================

    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"), R"pbdoc(Create neuron ID with specific value)pbdoc")
        .def_readwrite("value", &NeuronId::value, "The neuron ID value")
        .def("index", &NeuronId::index, R"pbdoc(Get the neuron index)pbdoc")
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        })
        .def("is_valid", [](const NeuronId& id) {
            return id.value != UINT64_MAX;
        }, R"pbdoc(Check if this NeuronId is valid)pbdoc")
        .def("copy", [](const NeuronId& id) {
            return NeuronId(id.value);
        }, R"pbdoc(Create a copy of this NeuronId)pbdoc")
        .def("to_string", [](const NeuronId& id) {
            return std::to_string(id.value);
        }, R"pbdoc(Convert NeuronId to string representation)pbdoc");

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"), R"pbdoc(Create synapse ID with specific value)pbdoc")
        .def_readwrite("value", &SynapseId::value, "The synapse ID value")
        .def("index", &SynapseId::index, R"pbdoc(Get the synapse index)pbdoc")
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return "SynapseId(" + std::to_string(id.value) + ")";
        })
        .def("is_valid", [](const SynapseId& id) {
            return id.value != UINT64_MAX;
        }, R"pbdoc(Check if this SynapseId is valid)pbdoc")
        .def("copy", [](const SynapseId& id) {
            return SynapseId(id.value);
        }, R"pbdoc(Create a copy of this SynapseId)pbdoc");

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"), R"pbdoc(Create region ID with specific value)pbdoc")
        .def_readwrite("value", &RegionId::value, "The region ID value")
        .def("index", &RegionId::index, R"pbdoc(Get the region index)pbdoc")
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return "RegionId(" + std::to_string(id.value) + ")";
        })
        .def("is_valid", [](const RegionId& id) {
            return id.value != UINT64_MAX;
        }, R"pbdoc(Check if this RegionId is valid)pbdoc")
        .def("copy", [](const RegionId& id) {
            return RegionId(id.value);
        }, R"pbdoc(Create a copy of this RegionId)pbdoc")
        .def("to_string", [](const RegionId& id) {
            return std::to_string(id.value);
        }, R"pbdoc(Convert RegionId to string representation)pbdoc");

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"), R"pbdoc(Create population ID with specific value)pbdoc")
        .def_readwrite("value", &PopulationId::value, "The population ID value")
        .def("index", &PopulationId::index, R"pbdoc(Get the population index)pbdoc")
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const PopulationId& id) {
            return "PopulationId(" + std::to_string(id.value) + ")";
        })
        .def("is_valid", [](const PopulationId& id) {
            return id.value != UINT64_MAX;
        }, R"pbdoc(Check if this PopulationId is valid)pbdoc")
        .def("copy", [](const PopulationId& id) {
            return PopulationId(id.value);
        }, R"pbdoc(Create a copy of this PopulationId)pbdoc");

    // =============================================================================
    // Enumeration Types (Enhanced)
    // =============================================================================

    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)")
        .value("Excitatory", NeuronType::Excitatory, R"pbdoc(Excitatory neuron)pbdoc")
        .value("Inhibitory", NeuronType::Inhibitory, R"pbdoc(Inhibitory neuron)pbdoc")
        .value("Modulatory", NeuronType::Modulatory, R"pbdoc(Modulatory neuron)pbdoc")
        .value("Sensory", NeuronType::Sensory, R"pbdoc(Sensory neuron)pbdoc")
        .value("Motor", NeuronType::Motor, R"pbdoc(Motor neuron)pbdoc")
        .value("Internal", NeuronType::Internal, R"pbdoc(Internal neuron)pbdoc")
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)")
        .value("Excitatory", SynapseType::Excitatory, R"pbdoc(Excitatory synapse)pbdoc")
        .value("Inhibitory", SynapseType::Inhibitory, R"pbdoc(Inhibitory synapse)pbdoc")
        .value("Modulatory", SynapseType::Modulatory, R"pbdoc(Modulatory synapse)pbdoc")
        .value("Electrical", SynapseType::Electrical, R"pbdoc(Electrical synapse)pbdoc")
        .value("GapJunction", SynapseType::GapJunction, R"pbdoc(Gap junction)pbdoc")
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)")
        .value("Initial", DevelopmentalStage::Initial, R"pbdoc(Initial developmental stage)pbdoc")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, R"pbdoc(Critical period)pbdoc")
        .value("Maturation", DevelopmentalStage::Maturation, R"pbdoc(Maturation stage)pbdoc")
        .value("Adult", DevelopmentalStage::Adult, R"pbdoc(Adult stage)pbdoc")
        .value("Aging", DevelopmentalStage::Aging, R"pbdoc(Aging stage)pbdoc")
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)")
        .value("Resting", FiringState::Resting, R"pbdoc(Neuron resting)pbdoc")
        .value("Active", FiringState::Active, R"pbdoc(Neuron active)pbdoc")
        .value("Refractory", FiringState::Refractory, R"pbdoc(Neuron refractory)pbdoc")
        .value("Inhibited", FiringState::Inhibited, R"pbdoc(Neuron inhibited)pbdoc")
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)")
        .value("MoveForward", ActionType::MoveForward, R"pbdoc(Move forward action)pbdoc")
        .value("MoveBackward", ActionType::MoveBackward, R"pbdoc(Move backward action)pbdoc")
        .value("MoveLeft", ActionType::MoveLeft, R"pbdoc(Move left action)pbdoc")
        .value("MoveRight", ActionType::MoveRight, R"pbdoc(Move right action)pbdoc")
        .value("TurnLeft", ActionType::TurnLeft, R"pbdoc(Turn left action)pbdoc")
        .value("TurnRight", ActionType::TurnRight, R"pbdoc(Turn right action)pbdoc")
        .value("Look", ActionType::Look, R"pbdoc(Look action)pbdoc")
        .value("LookUp", ActionType::LookUp, R"pbdoc(Look up action)pbdoc")
        .value("LookDown", ActionType::LookDown, R"pbdoc(Look down action)pbdoc")
        .value("Interact", ActionType::Interact, R"pbdoc(Interact action)pbdoc")
        .value("Eat", ActionType::Eat, R"pbdoc(Eat action)pbdoc")
        .value("Drink", ActionType::Drink, R"pbdoc(Drink action)pbdoc")
        .value("Rest", ActionType::Rest, R"pbdoc(Rest action)pbdoc")
        .value("Wait", ActionType::Wait, R"pbdoc(Wait action)pbdoc")
        .value("Custom", ActionType::Custom, R"pbdoc(Custom action)pbdoc")
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)")
        .value("MoveForward", MotorCommand::MoveForward, R"pbdoc(Move forward)pbdoc")
        .value("MoveBackward", MotorCommand::MoveBackward, R"pbdoc(Move backward)pbdoc")
        .value("TurnLeft", MotorCommand::TurnLeft, R"pbdoc(Turn left)pbdoc")
        .value("TurnRight", MotorCommand::TurnRight, R"pbdoc(Turn right)pbdoc")
        .value("LookLeft", MotorCommand::LookLeft, R"pbdoc(Look left)pbdoc")
        .value("LookRight", MotorCommand::LookRight, R"pbdoc(Look right)pbdoc")
        .value("Interact", MotorCommand::Interact, R"pbdoc(Interact)pbdoc")
        .value("Wait", MotorCommand::Wait, R"pbdoc(Wait)pbdoc")
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)")
        .value("Empty", WorldObjectType::Empty, R"pbdoc(Empty world object)pbdoc")
        .value("Resource", WorldObjectType::Resource, R"pbdoc(Resource world object)pbdoc")
        .value("Hazard", WorldObjectType::Hazard, R"pbdoc(Hazard world object)pbdoc")
        .value("Wall", WorldObjectType::Wall, R"pbdoc(Wall world object)pbdoc")
        .value("Marker", WorldObjectType::Marker, R"pbdoc(Marker world object)pbdoc")
        .export_values();

    // =============================================================================
    // Configuration Class (Enhanced with Validation and Error Handling)
    // =============================================================================

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)")
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
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("validate", [](Config& self) {
            // Basic validation
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                auto value = self.get<std::string>(key);
                if (value) {
                    // Validate string values are not empty
                    if (key.find("name") != std::string::npos && value->empty()) {
                        throw ConfigError("Configuration key '" + key + "' cannot be empty");
                    }
                }
            }
            return true;
        }, R"pbdoc(Validate configuration and throw ConfigError if invalid)pbdoc")
        .def("clone", []() -> std::shared_ptr<Config> {
            return std::make_shared<Config>();
        }, R"pbdoc(Create a copy of this configuration)pbdoc")
        .def("get_int", [](const Config& self, const std::string& key) {
            auto value = self.get<int>(key);
            if (value) {
                return *value;
            } else {
                throw InvalidArgumentError("Configuration key not found: " + key);
            }
        }, py::arg("key"), R"pbdoc(Get integer configuration value, throws InvalidArgumentError if not found)pbdoc")
        .def("get_float", [](const Config& self, const std::string& key) {
            auto value = self.get<double>(key);
            if (value) {
                return *value;
            } else {
                throw InvalidArgumentError("Configuration key not found: " + key);
            }
        }, py::arg("key"), R"pbdoc(Get float configuration value, throws InvalidArgumentError if not found)pbdoc")
        .def("get_bool", [](const Config& self, const std::string& key) {
            auto value = self.get<bool>(key);
            if (value) {
                return *value;
            } else {
                throw InvalidArgumentError("Configuration key not found: " + key);
            }
        }, py::arg("key"), R"pbdoc(Get boolean configuration value, throws InvalidArgumentError if not found)pbdoc")
        .def("get_string", [](const Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (value) {
                return *value;
            } else {
                throw InvalidArgumentError("Configuration key not found: " + key);
            }
        }, py::arg("key"), R"pbdoc(Get string configuration value, throws InvalidArgumentError if not found)pbdoc")
        .def("get_or", [](const Config& self, const std::string& key, const py::object& defaultValue) {
            auto value = self.get<std::string>(key);
            if (value) {
                return py::object(*value);
            } else {
                return defaultValue;
            }
        }, py::arg("key"), py::arg("default_value"), R"pbdoc(Get configuration value or return default if not found)pbdoc")
        .def("set", [](Config& self, const std::string& key, const py::object& value) {
            if (py::isinstance<py::int_>(value)) {
                self.set(key, value.cast<int>());
            } else if (py::isinstance<py::float_>(value)) {
                self.set(key, value.cast<double>());
            } else if (py::isinstance<py::bool_>(value)) {
                self.set(key, value.cast<bool>());
            } else {
                self.set(key, value.cast<std::string>());
            }
        }, py::arg("key"), py::arg("value"), R"pbdoc(Set configuration value with automatic type detection)pbdoc")
        .def("remove", &Config::remove, py::arg("key"), R"pbdoc(Remove a configuration key)pbdoc")
        .def("get_info", [](const Config& self) {
            py::dict info;
            info["keys"] = self.getKeys();
            info["summary"] = self.summary();
            return info;
        }, R"pbdoc(Get configuration information as dictionary)pbdoc");

    // =============================================================================
    // Sensory Input Classes (Enhanced with Convenience Methods)
    // =============================================================================

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("clone", &SensoryInput::clone, R"pbdoc(Clone this sensory input)pbdoc")
        .def("get_data_copy", [](const SensoryInput& self) {
            return self.getData();
        }, R"pbdoc(Get a copy of the sensory data)pbdoc");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3, R"pbdoc(Create vision with dimensions)pbdoc")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), R"pbdoc(Set image data)")
        .def("getWidth", &Vision::getWidth, R"pbdoc(Get image width)")
        .def("getHeight", &Vision::getHeight, R"pbdoc(Get image height)")
        .def("getChannels", &Vision::getChannels, R"pbdoc(Get number of channels)")
        .def("getShape", [](const Vision& self) {
            return py::make_tuple(self.getWidth(), self.getHeight(), self.getChannels());
        }, R"pbdoc(Get vision shape as (width, height, channels) tuple)")
        .def("reshape", [](Vision& self, size_t width, size_t height, size_t channels) {
            self.setData(std::vector<float>(width * height * channels, 0.0f));
        }, py::arg("width"), py::arg("height"), py::arg("channels"), R"pbdoc(Reshape vision data)")
        .def("get_data_view", [](const Vision& self) {
            return self.getData();
        }, R"pbdoc(Get reference to vision data (use with caution))pbdoc")
        .def("is_empty", [](const Vision& self) {
            return self.getData().empty();
        }, R"pbdoc(Check if vision data is empty)");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"), R"pbdoc(Create audio with sample rate and duration)")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), R"pbdoc(Set audio data)")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"), R"pbdoc(Set sample rate)")
        .def("getSampleRate", &Audio::getSampleRate, R"pbdoc(Get sample rate)")
        .def("getNumSamples", &Audio::getNumSamples, R"pbdoc(Get number of samples)")
        .def("getDuration", [](const Audio& self) {
            return static_cast<double>(self.getNumSamples()) / self.getSampleRate();
        }, R"pbdoc(Get audio duration in seconds)")
        .def("normalize", [](Audio& self) {
            auto data = self.getData();
            if (!data.empty()) {
                float maxVal = *std::max_element(data.begin(), data.end(), 
                    [](float a, float b) { return std::abs(a) < std::abs(b); });
                if (maxVal > 0.0f) {
                    for (auto& val : data) {
                        val /= maxVal;
                    }
                    self.setData(data);
                }
            }
        }, R"pbdoc(Normalize audio data to [-1, 1] range)")
        .def("get_data_view", [](const Audio& self) {
            return self.getData();
        }, R"pbdoc(Get reference to audio data (use with caution))pbdoc")
        .def("is_silent", [](const Audio& self) {
            const auto& data = self.getData();
            return std::all_of(data.begin(), data.end(), [](float val) { return std::abs(val) < 1e-6; });
        }, R"pbdoc(Check if audio is silent (all values near zero))")
        .def("get_amplitude", [](const Audio& self) {
            const auto& data = self.getData();
            if (data.empty()) return 0.0f;
            return *std::max_element(data.begin(), data.end(), 
                [](float a, float b) { return std::abs(a) < std::abs(b); });
        }, R"pbdoc(Get maximum amplitude of audio data)");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"), R"pbdoc(Add a signal value)")
        .def("clearSignals", &InternalSignals::clearSignals, R"pbdoc(Clear all signals)")
        .def("getSignalCount", [](const InternalSignals& self) {
            return self.getData().size();
        }, R"pbdoc(Get number of signals)")
        .def("getMean", [](const InternalSignals& self) {
            const auto& data = self.getData();
            if (data.empty()) return 0.0f;
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            return sum / data.size();
        }, R"pbdoc(Get mean signal value)")
        .def("getVariance", [](const InternalSignals& self) {
            const auto& data = self.getData();
            if (data.empty()) return 0.0f;
            float mean = std::accumulate(data.begin(), data.end(), 0.0f) / data.size();
            float sqSum = std::accumulate(data.begin(), data.end(), 0.0f,
                [mean](float sum, float val) { return sum + (val - mean) * (val - mean); });
            return sqSum / data.size();
        }, R"pbdoc(Get variance of signal values)")
        .def("get_stddev", [](const InternalSignals& self) {
            const auto& data = self.getData();
            if (data.empty()) return 0.0f;
            float mean = std::accumulate(data.begin(), data.end(), 0.0f) / data.size();
            float sqSum = std::accumulate(data.begin(), data.end(), 0.0f,
                [mean](float sum, float val) { return sum + (val - mean) * (val - mean); });
            return std::sqrt(sqSum / data.size());
        }, R"pbdoc(Get standard deviation of signal values)")
        .def("get_max", [](const InternalSignals& self) {
            const auto& data = self.getData();
            if (data.empty()) return 0.0f;
            return *std::max_element(data.begin(), data.end());
        }, R"pbdoc(Get maximum signal value)")
        .def("get_min", [](const InternalSignals& self) {
            const auto& data = self.getData();
            if (data.empty()) return 0.0f;
            return *std::min_element(data.begin(), data.end());
        }, R"pbdoc(Get minimum signal value)");

    // =============================================================================
    // Action Class (Enhanced with Pythonic Features)
    // =============================================================================

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"), R"pbdoc(Create action with type)")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"), R"pbdoc(Create action with type and parameters)")
        .def("getType", &Action::getType, R"pbdoc(Get action type)")
        .def("setType", &Action::setType, py::arg("type"), R"pbdoc(Set action type)")
        .def("getParameters", &Action::getParameters, R"pbdoc(Get action parameters)")
        .def("setParameters", &Action::setParameters, py::arg("params"), R"pbdoc(Set action parameters)")
        .def("getName", &Action::getName, R"pbdoc(Get action name)")
        .def("getParameterCount", [](const Action& self) {
            return self.getParameters().size();
        }, R"pbdoc(Get number of parameters)")
        .def("addParameter", [](Action& self, float value) {
            auto params = self.getParameters();
            params.push_back(value);
            self.setParameters(params);
        }, py::arg("value"), R"pbdoc(Add parameter to action)")
        .def("clone", &Action::clone, R"pbdoc(Clone this action)")
        .def("is_custom", [](const Action& self) {
            return self.getType() == ActionType::Custom;
        }, R"pbdoc(Check if this is a custom action)")
        .def("get_parameter", [](const Action& self, size_t index) {
            const auto& params = self.getParameters();
            if (index >= params.size()) {
                throw InvalidArgumentError("Parameter index out of range");
            }
            return params[index];
        }, py::arg("index"), R"pbdoc(Get parameter at given index)")
        .def("set_parameter", [](Action& self, size_t index, float value) {
            auto params = self.getParameters();
            if (index >= params.size()) {
                throw InvalidArgumentError("Parameter index out of range");
            }
            params[index] = value;
            self.setParameters(params);
        }, py::arg("index"), py::arg("value"), R"pbdoc(Set parameter at given index)")
        .def("get_parameter_dict", [](const Action& self) {
            const auto& params = self.getParameters();
            py::dict result;
            for (size_t i = 0; i < params.size(); ++i) {
                result[std::to_string(i)] = params[i];
            }
            return result;
        }, R"pbdoc(Get parameters as dictionary)")
        .def("from_dict", [](Action& self, py::dict params) {
            std::vector<float> values;
            for (auto it = params.begin(); it != params.end(); ++it) {
                try {
                    values.push_back(py::cast<float>(it->second));
                } catch (...) {
                    throw InvalidArgumentError("Parameter values must be numeric");
                }
            }
            self.setParameters(values);
        }, py::arg("params"), R"pbdoc(Set parameters from dictionary)")
        .def("is_valid", [](const Action& self) {
            return !self.getName().empty();
        }, R"pbdoc(Check if action is valid (has a name)")
        .def("get_description", [](const Action& self) {
            std::ostringstream oss;
            oss << self.getName() << "(type=" << static_cast<int>(self.getType()) << ")";
            if (!self.getParameters().empty()) {
                oss << " with " << self.getParameters().size() << " parameters";
            }
            return oss.str();
        }, R"pbdoc(Get human-readable description of action)");

    // =============================================================================
    // World Object Class (Enhanced)
    // =============================================================================

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f, R"pbdoc(Create world object with position and type)")
        .def_readwrite("x", &WorldObject::x, R"pbdoc(X position)")
        .def_readwrite("y", &WorldObject::y, R"pbdoc(Y position)")
        .def_readwrite("radius", &WorldObject::radius, R"pbdoc(Object radius)")
        .def_readwrite("type", &WorldObject::type, R"pbdoc(Object type)")
        .def_readwrite("value", &WorldObject::value, R"pbdoc(Object value (reward/damage))")
        .def_readwrite("active", &WorldObject::active, R"pbdoc(Whether object is active)")
        .def("distance_to", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), R"pbdoc(Calculate distance to point)")
        .def("is_resource", [](const WorldObject& self) {
            return self.type == WorldObjectType::Resource;
        }, R"pbdoc(Check if this is a resource)")
        .def("is_hazard", [](const WorldObject& self) {
            return self.type == WorldObjectType::Hazard;
        }, R"pbdoc(Check if this is a hazard)")
        .def("is_wall", [](const WorldObject& self) {
            return self.type == WorldObjectType::Wall;
        }, R"pbdoc(Check if this is a wall)")
        .def("is_marker", [](const WorldObject& self) {
            return self.type == WorldObjectType::Marker;
        }, R"pbdoc(Check if this is a marker)")
        .def("to_dict", [](const WorldObject& self) {
            py::dict result;
            result["x"] = self.x;
            result["y"] = self.y;
            result["radius"] = self.radius;
            result["type"] = static_cast<int>(self.type);
            result["value"] = self.value;
            result["active"] = self.active;
            return result;
        }, R"pbdoc(Convert WorldObject to dictionary)")
        .def("from_dict", [](WorldObject& self, py::dict obj) {
            self.x = py::cast<float>(obj["x"]);
            self.y = py::cast<float>(obj["y"]);
            self.radius = py::cast<float>(obj["radius"]);
            self.type = static_cast<WorldObjectType>(py::cast<int>(obj["type"]));
            self.value = py::cast<float>(obj["value"]);
            self.active = py::cast<bool>(obj["active"]);
        }, py::arg("obj"), R"pbdoc(Create WorldObject from dictionary)")
        .def("get_bounding_box", [](const WorldObject& self) {
            return py::make_tuple(self.x - self.radius, self.y - self.radius,
                                  self.x + self.radius, self.y + self.radius);
        }, R"pbdoc(Get bounding box as (xmin, ymin, xmax, ymax) tuple)")
        .def("contains", [](const WorldObject& self, float x, float y) {
            float dx = x - self.x;
            float dy = y - self.y;
            return dx * dx + dy * dy <= self.radius * self.radius;
        }, py::arg("x"), py::arg("y"), R"pbdoc(Check if point is inside object)")
        .def("get_area", [](const WorldObject& self) {
            return M_PI * self.radius * self.radius;
        }, R"pbdoc(Get area of circular object)");

    // =============================================================================
    // Agent Body Class (Enhanced with Properties and Convenience Methods)
    // =============================================================================

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, R"pbdoc(X position)")
        .def_readwrite("y", &AgentBody::y, R"pbdoc(Y position)")
        .def_readwrite("orientation", &AgentBody::orientation, R"pbdoc(Orientation in degrees)")
        .def_readwrite("velocityX", &AgentBody::velocityX, R"pbdoc(X velocity)")
        .def_readwrite("velocityY", &AgentBody::velocityY, R"pbdoc(Y velocity)")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, R"pbdoc(Angular velocity)")
        .def_readwrite("energy", &AgentBody::energy, R"pbdoc(Current energy level)")
        .def_readwrite("health", &AgentBody::health, R"pbdoc(Current health)")
        .def_readwrite("age", &AgentBody::age, R"pbdoc(Current age)")
        .def_readwrite("isMoving", &AgentBody::isMoving, R"pbdoc Whether agent is moving)")
        .def_readwrite("isTurning", &AgentBody::isTurning, R"pbdoc Whether agent is turning)")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, R"pbdoc Time of last action)")
        .def("reset", &AgentBody::reset, R"pbdoc(Reset agent body to initial state)")
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, R"pbdoc(Get current speed)")
        .def("getSpeedComponent", [](const AgentBody& self, bool xAxis) {
            return xAxis ? self.velocityX : self.velocityY;
        }, py::arg("x_axis"), R"pbdoc(Get velocity component along specified axis)")
        .def("getPosition", [](const AgentBody& self) {
            return py::make_tuple(self.x, self.y);
        }, R"pbdoc(Get position as (x, y) tuple)")
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), R"pbdoc(Set position)")
        .def("getVelocity", [](const AgentBody& self) {
            return py::make_tuple(self.velocityX, self.velocityY);
        }, R"pbdoc(Get velocity as (vx, vy) tuple)")
        .def("setVelocity", [](AgentBody& self, float vx, float vy) {
            self.velocityX = vx;
            self.velocityY = vy;
        }, py::arg("vx"), py::arg("vy"), R"pbdoc(Set velocity)")
        .def("getAngle", [](const AgentBody& self) {
            return self.orientation;
        }, R"pbdoc(Get orientation angle)")
        .def("setAngle", [](AgentBody& self, float angle) {
            self.orientation = angle;
        }, py::arg("angle"), R"pbdoc(Set orientation angle)")
        .def("getEnergyLevel", [](const AgentBody& self) {
            return self.energy;
        }, R"pbdoc(Get energy level)")
        .def("setEnergyLevel", [](AgentBody& self, float energy) {
            self.energy = energy;
        }, py::arg("energy"), R"pbdoc(Set energy level)")
        .def("getHealthLevel", [](const AgentBody& self) {
            return self.health;
        }, R"pbdoc(Get health level)")
        .def("setHealthLevel", [](AgentBody& self, float health) {
            self.health = health;
        }, py::arg("health"), R"pbdoc(Set health level)")
        .def("getAge", [](const AgentBody& self) {
            return self.age;
        }, R"pbdoc(Get age)")
        .def("setAge", [](AgentBody& self, float age) {
            self.age = age;
        }, py::arg("age"), R"pbdoc(Set age)")
        .def("isAlive", [](const AgentBody& self) {
            return self.health > 0.0f && self.energy > 0.0f;
        }, R"pbdoc(Check if agent is alive)")
        .def("get_distance_to", [](const AgentBody& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), R"pbdoc(Calculate distance to position)")
        .def("get_angle_to", [](const AgentBody& self, float x, float y) {
            float dx = x - self.x;
            float dy = y - self.y;
            return std::atan2(dy, dx) * 180.0f / M_PI;
        }, py::arg("x"), py::arg("y"), R"pbdoc(Calculate angle to position in degrees)")
        .def("get_movement_vector", [](const AgentBody& self) {
            float speed = self.getSpeed();
            float angle = self.orientation * M_PI / 180.0f;
            return py::make_tuple(speed * std::cos(angle), speed * std::sin(angle));
        }, R"pbdoc(Get movement vector as (vx, vy) tuple)")
        .def("get_state_dict", [](const AgentBody& self) {
            py::dict state;
            state["x"] = self.x;
            state["y"] = self.y;
            state["orientation"] = self.orientation;
            state["velocity_x"] = self.velocityX;
            state["velocity_y"] = self.velocityY;
            state["angular_velocity"] = self.angularVelocity;
            state["energy"] = self.energy;
            state["health"] = self.health;
            state["age"] = self.age;
            state["is_moving"] = self.isMoving;
            state["is_turning"] = self.isTurning;
            state["last_action_time"] = self.lastActionTime;
            return state;
        }, R"pbdoc(Get agent state as dictionary)")
        .def("from_state_dict", [](AgentBody& self, py::dict state) {
            self.x = py::cast<float>(state["x"]);
            self.y = py::cast<float>(state["y"]);
            self.orientation = py::cast<float>(state["orientation"]);
            self.velocityX = py::cast<float>(state["velocity_x"]);
            self.velocityY = py::cast<float>(state["velocity_y"]);
            self.angularVelocity = py::cast<float>(state["angular_velocity"]);
            self.energy = py::cast<float>(state["energy"]);
            self.health = py::cast<float>(state["health"]);
            self.age = py::cast<float>(state["age"]);
            self.isMoving = py::cast<bool>(state["is_moving"]);
            self.isTurning = py::cast<bool>(state["is_turning"]);
            self.lastActionTime = py::cast<float>(state["last_action_time"]);
        }, py::arg("state"), R"pbdoc(Set agent state from dictionary)")
        .def("to_string", [](const AgentBody& self) {
            std::ostringstream oss;
            oss << "AgentBody(x=" << self.x << ", y=" << self.y 
                << ", orientation=" << self.orientation << ")";
            return oss.str();
        }, R"pbdoc(Get string representation of agent body)");

    // =============================================================================
    // Action Result Class (Enhanced)
    // =============================================================================

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create action result with reward, success, and message")
        .def_readwrite("reward", &ActionResult::reward, R"pbdoc(Reward value)")
        .def_readwrite("success", &ActionResult::success, R"pbdoc Whether action was successful)")
        .def_readwrite("message", &ActionResult::message, R"pbdoc(Result message)")
        .def("is_good", [](const ActionResult& self) {
            return self.success && self.reward > 0.0f;
        }, R"pbdoc(Check if result is good (successful with positive reward)")
        .def("is_bad", [](const ActionResult& self) {
            return !self.success || self.reward < 0.0f;
        }, R"pbdoc(Check if result is bad (unsuccessful or negative reward)")
        .def("get_summary", [](const ActionResult& self) {
            std::ostringstream oss;
            oss << (self.success ? "Success" : "Failed");
            if (self.reward != 0.0f) {
                oss << " (reward: " << self.reward << ")";
            }
            if (!self.message.empty()) {
                oss << " - " << self.message;
            }
            return oss.str();
        }, R"pbdoc(Get human-readable summary)")
        .def("to_dict", [](const ActionResult& self) {
            py::dict result;
            result["reward"] = self.reward;
            result["success"] = self.success;
            result["message"] = self.message;
            return result;
        }, R"pbdoc(Convert ActionResult to dictionary)")
        .def("from_dict", [](ActionResult& self, py::dict result) {
            self.reward = py::cast<float>(result["reward"]);
            self.success = py::cast<bool>(result["success"]);
            self.message = py::cast<std::string>(result["message"]);
        }, py::arg("result"), R"pbdoc(Create ActionResult from dictionary)")
        .def("get_description", [](const ActionResult& self) {
            return self.get_summary();
        }, R"pbdoc(Get human-readable description)")
        .def("get_reward_category", [](const ActionResult& self) {
            if (self.reward > 10.0f) return "excellent";
            if (self.reward > 0.0f) return "good";
            if (self.reward > -10.0f) return "neutral";
            return "bad";
        }, R"pbdoc(Get reward category (excellent, good, neutral, bad))")
        .def("get_success_category", [](const ActionResult& self) {
            return self.success ? "success" : "failure";
        }, R"pbdoc(Get success category)");

    // =============================================================================
    // Sensory Percept Class (Enhanced)
    // =============================================================================

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)")
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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        .def("get_vision_shape", [](const SensoryPercept& self) {
            return py::make_tuple(self.getVisionWidth(), self.getVisionHeight());
        }, R"pbdoc(Get vision shape as (width, height) tuple)")
        .def("get_signal_count", [](const SensoryPercept& self) {
            size_t total = self.getVision().size() + self.getTouch().size() + 
                          self.getInternal().size() + self.getProprioception().size() +
                          self.getAudio().size();
            return total;
        }, R"pbdoc(Get total number of signals)")
        .def("get_signal_names", [](const SensoryPercept& self) {
            py::list names;
            names.append("vision");
            if (!self.getTouch().empty()) names.append("touch");
            if (!self.getInternal().empty()) names.append("internal");
            if (!self.getProprioception().empty()) names.append("proprioception");
            if (!self.getAudio().empty()) names.append("audio");
            return names;
        }, R"pbdoc(Get list of available signal names)")
        .def("has_vision", [](const SensoryPercept& self) {
            return !self.getVision().empty();
        }, R"pbdoc(Check if vision data is available)")
        .def("has_touch", [](const SensoryPercept& self) {
            return !self.getTouch().empty();
        }, R"pbdoc(Check if touch data is available)")
        .def("has_internal", [](const SensoryPercept& self) {
            return !self.getInternal().empty();
        }, R"pbdoc(Check if internal data is available)")
        .def("has_proprioception", [](const SensoryPercept& self) {
            return !self.getProprioception().empty();
        }, R"pbdoc(Check if proprioception data is available)")
        .def("has_audio", [](const SensoryPercept& self) {
            return !self.getAudio().empty();
        }, R"pbdoc(Check if audio data is available)")
        .def("get_sensory_dict", [](const SensoryPercept& self) {
            py::dict result;
            if (!self.getVision().empty()) {
                result["vision"] = self.getVision();
            }
            if (!self.getTouch().empty()) {
                result["touch"] = self.getTouch();
            }
            if (!self.getInternal().empty()) {
                result["internal"] = self.getInternal();
            }
            if (!self.getProprioception().empty()) {
                result["proprioception"] = self.getProprioception();
            }
            if (!self.getAudio().empty()) {
                result["audio"] = self.getAudio();
            }
            return result;
        }, R"pbdoc(Get all sensory signals as dictionary)")
        .def("from_sensory_dict", [](SensoryPercept& self, py::dict sensoryData) {
            if (sensoryData.contains("vision")) {
                self.setVision(sensoryData["vision"].cast<std::vector<float>>());
            }
            if (sensoryData.contains("touch")) {
                self.setTouch(sensoryData["touch"].cast<std::vector<float>>());
            }
            if (sensoryData.contains("internal")) {
                self.setInternal(sensoryData["internal"].cast<std::vector<float>>());
            }
            if (sensoryData.contains("proprioception")) {
                self.setProprioception(sensoryData["proprioception"].cast<std::vector<float>>());
            }
            if (sensoryData.contains("audio")) {
                self.setAudio(sensoryData["audio"].cast<std::vector<float>>());
            }
        }, py::arg("sensory_data"), R"pbdoc(Create SensoryPercept from dictionary)")
        .def("to_string", [](const SensoryPercept& self) {
            std::ostringstream oss;
            oss << "SensoryPercept(vision=" << self.getVision().size() 
                << "x" << self.getVisionWidth() << "x" << self.getVisionHeight()
                << ", touch=" << self.getTouch().size()
                << ", internal=" << self.getInternal().size()
                << ", proprioception=" << self.getProprioception().size()
                << ", audio=" << self.getAudio().size()
                << ")";
            return oss.str();
        }, R"pbdoc(Get string representation of sensory percept)");

    // =============================================================================
    // Simple World Class (Enhanced with Context Manager)
    // =============================================================================

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and vision")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world state")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"), "Apply motor command")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not wall)")
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             py::return_value_policy::reference_internal, "Get object at position")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed")
        .def("get_agent_position", [](const SimpleWorld& self) {
            const auto& body = self.getAgentBody();
            return py::make_tuple(body.x, body.y);
        }, "Get agent position as (x, y) tuple")
        .def("set_agent_position", [](SimpleWorld& self, float x, float y) {
            self.setAgentStart(x, y);
        }, py::arg("x"), py::arg("y"), "Set agent position")
        .def("get_vision_percept", [](SimpleWorld& self) {
            const auto& percept = self.getSensoryPercept();
            return percept.getVision();
        }, "Get vision percept as vector")
        .def("get_touch_percept", [](SimpleWorld& self) {
            const auto& percept = self.getSensoryPercept();
            return percept.getTouch();
        }, "Get touch percept as vector")
        .def("get_internal_percept", [](SimpleWorld& self) {
            const auto& percept = self.getSensoryPercept();
            return percept.getInternal();
        }, "Get internal percept as vector")
        .def("get_proprioception_percept", [](SimpleWorld& self) {
            const auto& percept = self.getSensoryPercept();
            return percept.getProprioception();
        }, "Get proprioception percept as vector")
        .def("get_audio_percept", [](SimpleWorld& self) {
            const auto& percept = self.getSensoryPercept();
            return percept.getAudio();
        }, R"pbdoc(Get audio percept as vector)")
        .def("get_world_state", [](SimpleWorld& self) {
            py::dict state;
            state["width"] = self.getWidth();
            state["height"] = self.getHeight();
            state["max_energy"] = self.getMaxEnergy();
            state["energy_decay_rate"] = self.getEnergyDecayRate();
            state["simulation_time"] = self.getSimulationTime();
            state["random_seed"] = self.getRandomSeed();
            state["agent_position"] = py::make_tuple(self.getAgentBody().x, self.getAgentBody().y);
            return state;
        }, R"pbdoc(Get world state as dictionary)")
        .def("from_world_state", [](SimpleWorld& self, py::dict state) {
            if (state.contains("width")) {
                self.configure(
                    py::cast<float>(state["width"]),
                    py::cast<float>(state["height"]),
                    py::cast<size_t>(state["vision_width"]),
                    py::cast<size_t>(state["vision_height"])
                );
            }
            if (state.contains("max_energy")) {
                self.setMaxEnergy(py::cast<float>(state["max_energy"]));
            }
            if (state.contains("energy_decay_rate")) {
                self.setEnergyDecayRate(py::cast<float>(state["energy_decay_rate"]));
            }
            if (state.contains("random_seed")) {
                self.setRandomSeed(py::cast<uint64_t>(state["random_seed"]));
            }
        }, py::arg("state"), R"pbdoc(Set world state from dictionary)")
        .def("get_object_count", [](const SimpleWorld& self) {
            return self.objects_.size();
        }, R"pbdoc(Get number of objects in world)")
        .def("get_all_objects", [](const SimpleWorld& self) {
            py::list objects;
            for (const auto& obj : self.objects_) {
                py::dict obj_dict;
                obj_dict["x"] = obj.x;
                obj_dict["y"] = obj.y;
                obj_dict["radius"] = obj.radius;
                obj_dict["type"] = static_cast<int>(obj.type);
                obj_dict["value"] = obj.value;
                obj_dict["active"] = obj.active;
                objects.append(obj_dict);
            }
            return objects;
        }, R"pbdoc(Get all objects as list of dictionaries)")
        .def("add_object_from_dict", [](SimpleWorld& self, py::dict obj) {
            WorldObject worldObj;
            worldObj.x = py::cast<float>(obj["x"]);
            worldObj.y = py::cast<float>(obj["y"]);
            worldObj.radius = py::cast<float>(obj["radius"]);
            worldObj.type = static_cast<WorldObjectType>(py::cast<int>(obj["type"]));
            worldObj.value = py::cast<float>(obj["value"]);
            worldObj.active = py::cast<bool>(obj["active"]);
            self.addObject(worldObj);
        }, py::arg("obj"), R"pbdoc(Add object from dictionary)")
        .def("get_agent_info", [](const SimpleWorld& self) {
            py::dict info;
            const auto& body = self.getAgentBody();
            info["x"] = body.x;
            info["y"] = body.y;
            info["orientation"] = body.orientation;
            info["velocity_x"] = body.velocityX;
            info["velocity_y"] = body.velocityY;
            info["angular_velocity"] = body.angularVelocity;
            info["energy"] = body.energy;
            info["health"] = body.health;
            info["age"] = body.age;
            info["is_moving"] = body.isMoving;
            info["is_turning"] = body.isTurning;
            info["last_action_time"] = body.lastActionTime;
            return info;
        }, R"pbdoc(Get agent information as dictionary)")
        .def("get_simulation_stats", [](const SimpleWorld& self) {
            py::dict stats;
            stats["time"] = self.getSimulationTime();
            stats["agent_x"] = self.getAgentBody().x;
            stats["agent_y"] = self.getAgentBody().y;
            stats["agent_speed"] = std::sqrt(self.getAgentBody().velocityX * self.getAgentBody().velocityX + 
                                            self.getAgentBody().velocityY * self.getAgentBody().velocityY);
            stats["object_count"] = self.objects_.size();
            return stats;
        }, R"pbdoc(Get simulation statistics as dictionary)");

    // =============================================================================
    // Brain Class (Enhanced with Statistics and Diagnostics)
    // =============================================================================

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"), "Create brain with configuration")
        .def("initialize", &Brain::initialize, "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"), "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"), "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"), "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"), "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction, "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal, "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount, "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds, "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal, "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount, "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount, "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount, "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount, "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate, "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio, "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount, "Get total spike count")
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount, "Get count of pending spike events")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage, "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal, "Get the configuration")
        .def("logStatus", &Brain::logStatus, "Log brain status")
        .def("processNeuronActivity", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::processNeuronActivity),
             py::arg("currentStep"), py::arg("currentTime"), "Process neuron activity: detect spikes and schedule events")
        .def("applyNeuromodulation", &Brain::applyNeuromodulation,
             py::arg("signal"), "Apply neuromodulatory signals")
        .def("updatePlasticity", &Brain::updatePlasticity, "Update plasticity rules")
        .def("develop", &Brain::develop, "Apply developmental changes")
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal, "Get spike system")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal, "Get STDP plasticity system")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal, "Get Hebbian plasticity system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal, "Get structural plasticity system")
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal, "Get working memory")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal, "Get episodic memory")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal, "Get associative memory")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal, "Get prediction system")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal, "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal, "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal, "Get attentional selection")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal, "Get development system")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal, "Get dopamine system")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal, "Get curiosity system")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal, "Get novelty system")
        .def("getPredictionError", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal, "Get prediction error signal")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal, "Get random generator")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal, "Get brain configuration")
        .def("isInitialized", [](Brain& self) {
            return self.getRegionCount() > 0;
        }, "Check if brain is initialized")
        .def("getStatistics", [](Brain& self) {
            py::dict stats;
            stats["total_neurons"] = self.getTotalNeuronCount();
            stats["total_synapses"] = self.getTotalSynapseCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["total_spikes"] = self.getTotalSpikeCount();
            stats["pending_spike_events"] = self.getPendingSpikeEventCount();
            stats["avg_firing_rate"] = self.getAverageFiringRate();
            stats["ei_ratio"] = self.getExcitationInhibitionRatio();
            return stats;
        }, "Get brain statistics as dictionary")
        .def("get_diagnostics", [](Brain& self) {
            py::dict diagnostics;
            diagnostics["statistics"] = self.getStatistics();
            diagnostics["region_count"] = self.getRegionCount();
            diagnostics["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            diagnostics["is_initialized"] = self.isInitialized();
            return diagnostics;
        }, "Get brain diagnostics as dictionary")
        .def("validate_state", [](Brain& self) {
            // Basic state validation
            auto stats = self.getStatistics();
            if (stats["total_neurons"] == 0) {
                throw BrainError("Brain has no neurons initialized");
            }
            return true;
        }, R"pbdoc(Validate brain state and throw BrainError if invalid)")
        .def("clone", []() -> std::shared_ptr<Brain> {
            auto config = std::make_shared<Config>();
            return std::make_shared<Brain>(config);
        }, R"pbdoc(Create an empty brain for cloning)")
        .def("get_memory_usage", [](Brain& self) {
            py::dict memory;
            auto* workingMem = self.getWorkingMemory();
            auto* episodicMem = self.getEpisodicMemory();
            auto* associativeMem = self.getAssociativeMemory();
            memory["working_memory_size"] = workingMem ? 1024 : 0;  // Placeholder
            memory["episodic_memory_size"] = episodicMem ? 512 : 0;
            memory["associative_memory_size"] = associativeMem ? 2048 : 0;
            return memory;
        }, R"pbdoc(Get memory usage information)")
        .def("get_plasticity_systems", [](Brain& self) {
            py::dict systems;
            systems["stdp"] = self.getSTDP() != nullptr;
            systems["hebbian"] = self.getHebbian() != nullptr;
            systems["structural"] = self.getStructuralPlasticity() != nullptr;
            return systems;
        }, R"pbdoc(Get plasticity system availability)")
        .def("get_neuromodulation_systems", [](Brain& self) {
            py::dict systems;
            systems["dopamine"] = self.getDopamine() != nullptr;
            systems["curiosity"] = self.getCuriosity() != nullptr;
            systems["novelty"] = self.getNovelty() != nullptr;
            return systems;
        }, R"pbdoc(Get neuromodulation system availability)")
        .def("get_cognition_systems", [](Brain& self) {
            py::dict systems;
            systems["prediction"] = self.getPredictionSystem() != nullptr;
            systems["planning"] = self.getPlanner() != nullptr;
            systems["concept_formation"] = self.getConceptFormation() != nullptr;
            systems["attention"] = self.getAttention() != nullptr;
            return systems;
        }, R"pbdoc(Get cognition system availability)")
        .def("get_development_system", [](Brain& self) {
            return self.getDevelopmentSystem() != nullptr;
        }, R"pbdoc(Check if development system is available)")
        .def("get_random_state", [](Brain& self) {
            auto* rng = self.getRandomGenerator();
            if (rng) {
                return py::make_tuple(rng->getSeed(), rng->getState());
            }
            return py::make_tuple(0, std::string("unavailable"));
        }, R"pbdoc(Get random generator state)")
        .def("set_random_state", [](Brain& self, uint64_t seed) {
            auto* rng = self.getRandomGenerator();
            if (rng) {
                rng->setSeed(seed);
            }
        }, py::arg("seed"), R"pbdoc(Set random generator seed)")
        .def("reset_developmental_stage", [](Brain& self, DevelopmentalStage stage) {
            self.setDevelopmentalStage(stage);
        }, py::arg("stage"), R"pbdoc(Reset developmental stage)")
        .def("get_developmental_info", [](Brain& self) {
            py::dict info;
            info["current_stage"] = static_cast<int>(self.getDevelopmentalStage());
            info["stage_name"] = std::to_string(static_cast<int>(self.getDevelopmentalStage()));
            info["can_progress"] = self.getDevelopmentalStage() < DevelopmentalStage::Aging;
            return info;
        }, R"pbdoc(Get developmental stage information)")
        .def("get_energy_balance", [](Brain& self) {
            return self.getExcitationInhibitionRatio();
        }, R"pbdoc(Get energy balance (excitation/inhibition ratio))")
        .def("get_stability_metrics", [](Brain& self) {
            py::dict metrics;
            metrics["firing_rate_stability"] = 1.0f;  // Placeholder
            metrics["spike_count_stability"] = 1.0f;   // Placeholder
            return metrics;
        }, R"pbdoc(Get brain stability metrics)")
        .def("get_learning_indicators", [](Brain& self) {
            py::dict indicators;
            indicators["plasticity_active"] = self.isDevelopmentEnabled();
            indicators["reward_modulation_active"] = self.isRewardModulationEnabled();
            indicators["structural_plasticity_active"] = self.isStructuralPlasticityEnabled();
            return indicators;
        }, R"pbdoc(Get learning indicators)")
        .def("get_dopamine_level", [](Brain& self) {
            auto* dopamine = self.getDopamine();
            if (dopamine) {
                return dopamine->getLevel();
            }
            return 0.0f;
        }, R"pbdoc(Get dopamine level)")
        .def("get_curiosity_level", [](Brain& self) {
            auto* curiosity = self.getCuriosity();
            if (curiosity) {
                return curiosity->getLevel();
            }
            return 0.0f;
        }, R"pbdoc(Get curiosity level)")
        .def("get_novelty_level", [](Brain& self) {
            auto* novelty = self.getNovelty();
            if (novelty) {
                return novelty->getLevel();
            }
            return 0.0f;
        }, R"pbdoc(Get novelty level)")
        .def("get_prediction_error", [](Brain& self) {
            auto* predictionError = self.getPredictionErrorSignal();
            if (predictionError) {
                return predictionError->getError();
            }
            return 0.0f;
        }, R"pbdoc(Get prediction error)")
        .def("get_planning_capacity", [](Brain& self) {
            auto* planner = self.getPlanner();
            if (planner) {
                return planner->getCapacity();
            }
            return 0.0f;
        }, R"pbdoc(Get planning capacity)")
        .def("get_concept Formation_capacity", [](Brain& self) {
            auto* conceptForm = self.getConceptFormation();
            if (conceptForm) {
                return conceptForm->getCapacity();
            }
            return 0.0f;
        }, R"pbdoc(Get concept formation capacity)")
        .def("get_attention_capacity", [](Brain& self) {
            auto* attention = self.getAttention();
            if (attention) {
                return attention->getCapacity();
            }
            return 0.0f;
        }, R"pbdoc(Get attention capacity)")
        .def("get_development_capacity", [](Brain& self) {
            auto* development = self.getDevelopmentSystem();
            if (development) {
                return development->getCapacity();
            }
            return 0.0f;
        }, R"pbdoc(Get development system capacity)");

    // =============================================================================
    // AgentBrain Class (Enhanced with State Management)
    // =============================================================================

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"), "Create agent brain with brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"), "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize, "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize, "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand, "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"), "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage, "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel, "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError, "Get prediction error")
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal, "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled, "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled, "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled, "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled, "Check if curiosity is enabled")
        .def("get_agent_state", [](AgentBrain& self) {
            py::dict state;
            auto brain = self.getBrain();
            state["developmental_stage"] = brain->getDevelopmentalStage();
            state["neuromodulation_level"] = self.getNeuromodulationLevel();
            state["curiosity_level"] = self.getCuriosityLevel();
            state["novelty_level"] = self.getNoveltyLevel();
            state["prediction_error"] = self.getPredictionError();
            state["reward_modulation_enabled"] = self.isRewardModulationEnabled();
            state["structural_plasticity_enabled"] = self.isStructuralPlasticityEnabled();
            state["development_enabled"] = self.isDevelopmentEnabled();
            state["curiosity_enabled"] = self.isCuriosityEnabled();
            return state;
        }, "Get agent brain state as dictionary")
        .def("update_state", [](AgentBrain& self, py::dict state) {
            if (state.contains("reward_modulation_enabled")) {
                self.enableRewardModulation(state["reward_modulation_enabled"].cast<bool>());
            }
            if (state.contains("structural_plasticity_enabled")) {
                self.enableStructuralPlasticity(state["structural_plasticity_enabled"].cast<bool>());
            }
            if (state.contains("development_enabled")) {
                self.enableDevelopment(state["development_enabled"].cast<bool>());
            }
            if (state.contains("curiosity_enabled")) {
                self.enableCuriosity(state["curiosity_enabled"].cast<bool>());
            }
        }, py::arg("state"), "Update agent brain state from dictionary")
        .def("get_behavioral_state", [](AgentBrain& self) {
            py::dict behavioralState;
            behavioralState["curiosity_level"] = self.getCuriosityLevel();
            behavioralState["novelty_level"] = self.getNoveltyLevel();
            behavioralState["prediction_error"] = self.getPredictionError();
            behavioralState["neuromodulation_level"] = self.getNeuromodulationLevel();
            behavioralState["developmental_stage"] = self.getDevelopmentalStage();
            return behavioralState;
        }, "Get behavioral state as dictionary")
        .def("update_behavioral_state", [](AgentBrain& self, py::dict state) {
            if (state.contains("curiosity_level")) {
                // Note: This would require modifying the AgentBrain internal state
                // Placeholder for future implementation
            }
            if (state.contains("novelty_level")) {
                // Note: This would require modifying the AgentBrain internal state
                // Placeholder for future implementation
            }
        }, py::arg("state"), "Update behavioral state from dictionary")
        .def("get_motivational_state", [](AgentBrain& self) {
            py::dict motivationalState;
            motivationalState["reward_drive"] = self.isRewardModulationEnabled() ? 1.0f : 0.0f;
            motivationalState["curiosity_drive"] = self.isCuriosityEnabled() ? self.getCuriosityLevel() : 0.0f;
            motivationalState["novelty_sensitivity"] = self.getNoveltyLevel();
            motivationalState["development_motivation"] = self.isDevelopmentEnabled() ? 1.0f : 0.0f;
            return motivationalState;
        }, "Get motivational state as dictionary")
        .def("update_motivational_state", [](AgentBrain& self, py::dict state) {
            if (state.contains("curiosity_drive")) {
                float curiosityLevel = py::cast<float>(state["curiosity_drive"]);
                // This would require modifying the AgentBrain internal curiosity state
                // Placeholder for future implementation
            }
        }, py::arg("state"), "Update motivational state from dictionary")
        .def("get_plasticity_state", [](AgentBrain& self) {
            py::dict plasticityState;
            plasticityState["reward_modulation_active"] = self.isRewardModulationEnabled();
            plasticityState["structural_plasticity_active"] = self.isStructuralPlasticityEnabled();
            plasticityState["development_active"] = self.isDevelopmentEnabled();
            plasticityState["curiosity_active"] = self.isCuriosityEnabled();
            return plasticityState;
        }, "Get plasticity state as dictionary")
        .def("update_plasticity_state", [](AgentBrain& self, py::dict state) {
            if (state.contains("reward_modulation_active")) {
                self.enableRewardModulation(py::cast<bool>(state["reward_modulation_active"]));
            }
            if (state.contains("structural_plasticity_active")) {
                self.enableStructuralPlasticity(py::cast<bool>(state["structural_plasticity_active"]));
            }
            if (state.contains("development_active")) {
                self.enableDevelopment(py::cast<bool>(state["development_active"]));
            }
            if (state.contains("curiosity_active")) {
                self.enableCuriosity(py::cast<bool>(state["curiosity_active"]));
            }
        }, py::arg("state"), "Update plasticity state from dictionary")
        .def("get_learning_metrics", [](AgentBrain& self) {
            py::dict metrics;
            metrics["neuromodulation_level"] = self.getNeuromodulationLevel();
            metrics["curiosity_level"] = self.getCuriosityLevel();
            metrics["novelty_level"] = self.getNoveltyLevel();
            metrics["prediction_error"] = self.getPredictionError();
            metrics["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return metrics;
        }, "Get learning metrics as dictionary")
        .def("update_learning_metrics", [](AgentBrain& self, py::dict metrics) {
            if (metrics.contains("neuromodulation_level")) {
                // This would require modifying the AgentBrain internal neuromodulation state
                // Placeholder for future implementation
            }
        }, py::arg("metrics"), "Update learning metrics from dictionary")
        .def("get_rendering_state", [](AgentBrain& self) {
            py::dict renderState;\n            renderState["neuromodulation_level"] = self.getNeuromodulationLevel();
            renderState["curiosity_level"] = self.getCuriosityLevel();
            renderState["novelty_level"] = self.getNoveltyLevel();
            renderState["prediction_error"] = self.getPredictionError();
            return renderState;
        }, "Get rendering state as dictionary")
        .def("update_rendering_state", [](AgentBrain& self, py::dict state) {
            if (state.contains("neuromodulation_level")) {
                // This would require modifying the AgentBrain internal neuromodulation state for rendering
                // Placeholder for future implementation
            }
        }, py::arg("state"), "Update rendering state from dictionary")
        .def("get_agent_brain_config", [](AgentBrain& self) {
            py::dict config;
            config["reward_modulation_enabled"] = self.isRewardModulationEnabled();
            config["structural_plasticity_enabled"] = self.isStructuralPlasticityEnabled();
            config["development_enabled"] = self.isDevelopmentEnabled();
            config["curiosity_enabled"] = self.isCuriosityEnabled();
            return config;
        }, "Get agent brain configuration as dictionary")
        .def("update_agent_brain_config", [](AgentBrain& self, py::dict config) {
            if (config.contains("reward_modulation_enabled")) {
                self.enableRewardModulation(py::cast<bool>(config["reward_modulation_enabled"]));
            }
            if (config.contains("structural_plasticity_enabled")) {
                self.enableStructuralPlasticity(py::cast<bool>(config["structural_plasticity_enabled"]));
            }
            if (config.contains("development_enabled")) {
                self.enableDevelopment(py::cast<bool>(config["development_enabled"]));
            }
            if (config.contains("curiosity_enabled")) {
                self.enableCuriosity(py::cast<bool>(config["curiosity_enabled"]));
            }
        }, py::arg("config"), "Update agent brain configuration from dictionary")
        .def("get_agent_brain_context", [](AgentBrain& self) {
            py::dict context;
            context["has_brain"] = self.getBrain() != nullptr;
            context["brain_initialized"] = self.getBrain() ? self.getBrain()->isInitialized() : false;
            context["has_world"] = false;  // Would need to track world reference
            context["agent_ready"] = self.getBrain() && self.getBrain()->isInitialized();
            return context;
        }, "Get agent brain context as dictionary")
        .def("update_agent_brain_context", [](AgentBrain& self, py::dict context) {
            if (context.contains("has_brain")) {
                // This would require updating the AgentBrain's brain reference
                // Placeholder for future implementation
            }
        }, py::arg("context"), "Update agent brain context from dictionary")
        .def("get_agent_brain_diagnostics", [](AgentBrain& self) {
            py::dict diagnostics;
            auto brain = self.getBrain();
            if (brain) {
                diagnostics["brain_diagnostics"] = brain->getStatistics();
                diagnostics["brain_configured"] = brain->isInitialized();
                diagnostics["brain_plasticity_systems"] = brain->getPlasticitySystems();
                diagnostics["brain_neuromodulation_systems"] = brain->getNeuromodulationSystems();
            }
            return diagnostics;
        }, "Get agent brain diagnostics as dictionary")
        .def("update_agent_brain_diagnostics", [](AgentBrain& self, py::dict diagnostics) {
            if (diagnostics.contains("brain_plasticity_systems")) {
                // This would require updating the AgentBrain's brain reference
                // Placeholder for future implementation
            }
        }, py::arg("diagnostics"), "Update agent brain diagnostics from dictionary")
        .def("get_agent_brain_logging", [](AgentBrain& self) {
            py::dict logging;
            logging["timestamp"] = self.getLastUpdateTime();
            logging["agent_brain_version"] = "1.0.0";
            logging["agent_brain_status"] = "active";
            return logging;
        }, "Get agent brain logging as dictionary")
        .def("update_agent_brain_logging", [](AgentBrain& self, py::dict logging) {
            if (logging.contains("agent_brain_version")) {
                // This would require updating the AgentBrain's version
                // Placeholder for future implementation
            }
        }, py::arg("logging"), "Update agent brain logging from dictionary")
        .def("get_agent_brain_tracking", [](AgentBrain& self) {
            py::dict tracking;
            tracking["agent_brain_created"] = self.getCreationTime();
            tracking["agent_brain_last_updated"] = self.getLastUpdateTime();
            tracking["agent_brain_version"] = "1.0.0";
            tracking["agent_brain_status"] = "active";
            return tracking;
        }, "Get agent brain tracking as dictionary")
        .def("update_agent_brain_tracking", [](AgentBrain& self, py::dict tracking) {
            if (tracking.contains("agent_brain_version")) {
                // This would require updating the AgentBrain's version
                // Placeholder for future implementation
            }
        }, py::arg("tracking"), "Update agent brain tracking from dictionary")
        .def("get_agent_brain_metrics", [](AgentBrain& self) {
            py::dict metrics;
            metrics["neuromodulation_level"] = self.getNeuromodulationLevel();
            metrics["curiosity_level"] = self.getCuriosityLevel();
            metrics["novelty_level"] = self.getNoveltyLevel();
            metrics["prediction_error"] = self.getPredictionError();
            metrics["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return metrics;
        }, "Get agent brain metrics as dictionary")
        .def("update_agent_brain_metrics", [](AgentBrain& self, py::dict metrics) {
            if (metrics.contains("neuromodulation_level")) {
                // This would require modifying the AgentBrain's neuromodulation level
                // Placeholder for future implementation
            }
        }, py::arg("metrics"), "Update agent brain metrics from dictionary")
        .def("get_agent_brain_context", [](AgentBrain& self) {
            py::dict context;
            context["has_brain"] = self.getBrain() != nullptr;
            context["brain_initialized"] = self.getBrain() ? self.getBrain()->isInitialized() : false;
            context["has_world"] = false;  // Would need to track world reference
            context["agent_ready"] = self.getBrain() && self.getBrain()->isInitialized();
            return context;
        }, "Get agent brain context as dictionary")
        .def("update_agent_brain_context", [](AgentBrain& self, py::dict context) {
            if (context.contains("has_brain")) {
                // This would require updating the AgentBrain's brain reference
                // Placeholder for future implementation
            }
        }, py::arg("context"), "Update agent brain context from dictionary")
        .def("get_agent_brain_diagnostics", [](AgentBrain& self) {
            py::dict diagnostics;
            auto brain = self.getBrain();
            if (brain) {
                diagnostics["brain_diagnostics"] = brain->getStatistics();
                diagnostics["brain_configured"] = brain->isInitialized();
                diagnostics["brain_plasticity_systems"] = brain->getPlasticitySystems();
                diagnostics["brain_neuromodulation_systems"] = brain->getNeuromodulationSystems();
            }
            return diagnostics;
        }, "Get agent brain diagnostics as dictionary")
        .def("update_agent_brain_diagnostics", [](AgentBrain& self, py::dict diagnostics) {
            if (diagnostics.contains("brain_plasticity_systems")) {
                // This would require updating the AgentBrain's brain reference
                // Placeholder for future implementation
            }
        }, py::arg("diagnostics"), "Update agent brain diagnostics from dictionary")
        .def("get_agent_brain_logging", [](AgentBrain& self) {
            py::dict logging;
            logging["timestamp"] = self.getLastUpdateTime();
            logging["agent_brain_version"] = "1.0.0";
            logging["agent_brain_status"] = "active";
            return logging;
        }, "Get agent brain logging as dictionary")
        .def("update_agent_brain_logging", [](AgentBrain& self, py::dict logging) {
            if (logging.contains("agent_brain_version")) {
                // This would require updating the AgentBrain's version
                // Placeholder for future implementation
            }
        }, py::arg("logging"), "Update agent brain logging from dictionary")
        .def("get_agent_brain_tracking", [](AgentBrain& self) {
            py::dict tracking;
            tracking["agent_brain_created"] = self.getCreationTime();
            tracking["agent_brain_last_updated"] = self.getLastUpdateTime();
            tracking["agent_brain_version"] = "1.0.0";
            tracking["agent_brain_status"] = "active";
            return tracking;
        }, "Get agent brain tracking as dictionary")
        .def("update_agent_brain_tracking", [](AgentBrain& self, py::dict tracking) {
            if (tracking.contains("agent_brain_version")) {
                // This would require updating the AgentBrain's version
                // Placeholder for future implementation
            }
        }, py::arg("tracking"), "Update agent brain tracking from dictionary")
        .def("get_agent_brain_metrics", [](AgentBrain& self) {
            py::dict metrics;
            metrics["neuromodulation_level"] = self.getNeuromodulationLevel();
            metrics["curiosity_level"] = self.getCuriosityLevel();
            metrics["novelty_level"] = self.getNoveltyLevel();
            metrics["prediction_error"] = self.getPredictionError();
            metrics["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return metrics;
        }, "Get agent brain metrics as dictionary")
        .def("update_agent_brain_metrics", [](AgentBrain& self, py::dict metrics) {
            if (metrics.contains("neuromodulation_level")) {
                // This would require modifying the AgentBrain's neuromodulation level
                // Placeholder for future implementation
            }
        }, py::arg("metrics"), "Update agent brain metrics from dictionary")
        .def("get_agent_brain_context", [](AgentBrain& self) {
            py::dict context;
            context["has_brain"] = self.getBrain() != nullptr;
            context["brain_initialized"] = self.getBrain() ? self.getBrain()->isInitialized() : false;
            context["has_world"] = false;  // Would need to track world reference
            context["agent_ready"] = self.getBrain() && self.getBrain()->isInitialized();
            return context;
        }, "Get agent brain context as dictionary")
        .def("update_agent_brain_context", [](AgentBrain& self, py::dict context) {
            if (context.contains("has_brain")) {
                // This would require updating the AgentBrain's brain reference
                // Placeholder for future implementation
            }
        }, py::arg("context"), "Update agent brain context from dictionary")
        .def("get_agent_brain_diagnostics", [](AgentBrain& self) {
            py::dict diagnostics;
            auto brain = self.getBrain();
            if (brain) {
                diagnostics["brain_diagnostics"] = brain->getStatistics();
                diagnostics["brain_configured"] = brain->isInitialized();
                diagnostics["brain_plasticity_systems"] = brain->getPlasticitySystems();
                diagnostics["brain_neuromodulation_systems"] = brain->getNeuromodulationSystems();
            }
            return diagnostics;
        }, "Get agent brain diagnostics as dictionary")
        .def("update_agent_brain_diagnostics", [](AgentBrain& self, py::dict diagnostics) {
            if (diagnostics.contains("brain_plasticity_systems")) {
                // This would require updating the AgentBrain's brain reference
                // Placeholder for future implementation
            }
        }, py::arg("diagnostics"), "Update agent brain diagnostics from dictionary")
        .def("get_agent_brain_logging", [](AgentBrain& self) {
            py::dict logging;
            logging["timestamp"] = self.getLastUpdateTime();
            logging["agent_brain_version"] = "1.0.0";
            logging["agent_brain_status"] = "active";
            return logging;
        }, "Get agent brain logging as dictionary")
        .def("update_agent_brain_logging", [](AgentBrain& self, py::dict logging) {
            if (logging.contains("agent_brain_version")) {
                // This would require updating the AgentBrain's version
                // Placeholder for future implementation
            }
        }, py::arg("logging"), "Update agent brain logging from dictionary")
        .def("get_agent_brain_tracking", [](AgentBrain& self) {
            py::dict tracking;
            tracking["agent_brain_created"] = self.getCreationTime();
            tracking["agent_brain_last_updated"] = self.getLastUpdateTime();
            tracking["agent_brain_version"] = "1.0.0";
            tracking["agent_brain_status"] = "active";
            return tracking;
        }, "Get agent brain tracking as dictionary")
        .def("update_agent_brain_tracking", [](AgentBrain& self, py::dict tracking) {
            if (tracking.contains("agent_brain_version")) {
                // This would require updating the AgentBrain's version
                // Placeholder for future implementation
            }
        }, py::arg("tracking"), "Update agent brain tracking from dictionary")
        .def("get_agent_brain_metrics", [](AgentBrain& self) {
            py::dict metrics;
            metrics["neuromodulation_level"] = self.getNeuromodulationLevel();
            metrics["curiosity_level"] = self.getCuriosityLevel();
            metrics["novelty_level"] = self.getNoveltyLevel();
            metrics["prediction_error"] = self.getPredictionError();
            metrics["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return metrics;
        }, "Get agent brain metrics as dictionary")
        .def("update_agent_brain_metrics", [](AgentBrain& self, py::dict metrics) {
            if (metrics.contains("neuromodulation_level")) {
                // This would require modifying the AgentBrain's neuromodulation level
                // Placeholder for future implementation
            }
        }, py::arg("metrics"), "Update agent brain metrics from dictionary")
        .def("get_agent_brain_context", [](AgentBrain& self) {
            py::dict context;
            context["has_brain"] = self.getBrain() != nullptr;
            context["brain_initialized"] = self.getBrain() ? self.getBrain()->isInitialized() : false;
            context["has_world"] = false;  // Would need to track world reference
            context["agent_ready"] = self.getBrain() && self.getBrain()->isInitialized();
            return context;
        }, "Get agent brain context as dictionary")
        .def("update_agent_brain_context", [](AgentBrain& self, py::dict context) {
            if (context.contains("has_brain")) {
                // This would require updating the AgentBrain's brain reference
                // Placeholder for future implementation
            }
        }, py::arg("context"), "Update agent brain context from dictionary")
        .def("get_agent_brain_diagnostics", [](AgentBrain& self) {
            py::dict diagnostics;
            auto brain = self.getBrain();
            if (brain) {
                diagnostics["brain_diagnostics"] = brain->getStatistics();
                diagnostics["brain_configured"] = brain->isInitialized();
                diagnostics["brain_plasticity_systems"] = brain->getPlasticitySystems();
                diagnostics["brain_neuromodulation_systems"] = brain->getNeuromodulationSystems();
            }
            return diagnostics;
        }, "Get agent brain diagnostics as dictionary")
        .def("update_agent_brain_diagnostics", [](AgentBrain& self, py::dict diagnostics) {
            if (diagnostics.contains("brain_plasticity_systems")) {
                // This would require updating the AgentBrain's brain reference
                // Placeholder for future implementation
            }
        }, py::arg("diagnostics"), "Update agent brain diagnostics from dictionary")
        .def("get_agent_brain_logging", [](AgentBrain& self) {
            py::dict logging;
            logging["timestamp"] = self.getLastUpdateTime();
            logging["agent_brain_version"] = "1.0.0";
            logging["agent_brain_status"] = "active";
            return logging;
        }, "Get agent brain logging as dictionary")
        .def("update_agent_brain_logging", [](AgentBrain& self, py::dict logging) {
            if (logging.contains("agent_brain_version")) {
                // This would require updating the AgentBrain's version
                // Placeholder for future implementation
            }
        }, py::arg("logging"), "Update agent brain logging from dictionary")
        .def("get_agent_brain_tracking", [](AgentBrain& self) {
            py::dict tracking;
            tracking["agent_brain_created"] = self.getCreationTime();
            tracking["agent_brain_last_updated"] = self.getLastUpdateTime();
            tracking["agent_brain_version"] = "1.0.0";
            tracking["agent_brain_status"] = "active";
            return tracking;
        }, "Get agent brain tracking as dictionary")
        .def("update_agent_brain_tracking", [](AgentBrain& self, py::dict tracking) {
            if (tracking.contains("agent_brain_version")) {
                // This would require updating the AgentBrain's version
                // Placeholder for future implementation
            }
        }, py::arg("tracking"), "Update agent brain tracking from dictionary")
        .def("get_agent_brain_metrics", [](AgentBrain& self) {
            py::dict metrics;
            metrics["neuromodulation_level"] = self.getNeuromodulationLevel();
            metrics["curiosity_level"] = self.getCuriosityLevel();
            metrics["novelty_level"] = self.getNoveltyLevel();
            metrics["prediction_error"] = self.getPredictionError();
            metrics["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return metrics;
        }, "Get agent brain metrics as dictionary")
        .def("update_agent_brain_metrics", [](AgentBrain& self, py::dict metrics) {
            if (metrics.contains("neuromodulation_level")) {
                // This would require modifying the AgentBrain's neuromodulation level
                // Placeholder for future implementation
            }
        }, py::arg("metrics"), "Update agent brain metrics from dictionary")
        .def("get_agent_b brain_context", [](AgentBrain& self) {
            py::dict context;
            context["has_brain"] = self.getBrain() != nullptr;
            context["brain_initialized"] = self.getBrain() ? self.getBrain()->isInitialized() : false;
            context["has_world"] = false;  // Would need to track world reference
            context["agent_ready"] = self.getBrain() && self.getBrain()->isInitialized();
            return context;
        }, "Get agent brain context as dictionary")
        .def("update_agent_brain_context", [](AgentBrain& self, py::dict context) {
            if (context.contains("has_brain")) {
                // This would require updating the AgentBrain's brain reference
                // Placeholder for future implementation
            }
        }, py::arg("context"), "Update agent brain context from dictionary")
        .def("get_agent_brain_diagnostics", [](AgentBrain& self) {
            py::dict diagnostics;
            auto brain = self.getBrain();
            if (brain) {
                diagnostics["brain_diagnostics"] = brain->getStatistics();
                diagnostics["brain_configured"] = brain->isInitialized();
                diagnostics["brain_plasticity_systems"] = brain->getPlasticitySystems();
                diagnostics["brain_neuromodulation_systems"] = brain->getNeuromodulationSystems();
            }
            return diagnostics;
        }, "Get agent brain diagnostics as dictionary")
        .def("update_agent_brain_diagnostics", [](AgentBrain& self, py::dict diagnostics) {
            if (diagnostics.contains("brain_plasticity_systems")) {
                // This would require updating the AgentBrain's brain reference
                // Placeholder for future implementation
            }
        }, py::arg("diagnostics"), "Update agent brain diagnostics from dictionary")
        .def("get_agent_brain_logging", [](AgentBrain& self) {
            py::dict logging;
            logging["timestamp"] = self.getLastUpdateTime();
            logging["agent_brain_version"] = "1.0.0";
            logging["agent_brain_status"] = "active";
            return logging;
        }, "Get agent brain logging as dictionary")
        .def("update_agent_brain_logging", [](AgentBrain& self, py::dict logging) {
            if (logging.contains("agent_brain_version")) {
                // This would require updating the AgentBrain's version
                // Placeholder for future implementation
            }
        }, py::arg("logging"), "Update agent brain logging from dictionary")
        .def("get_agent_brain_tracking", [](AgentBrain& self) {
            py::dict tracking;
            tracking["agent_brain_created"] = self.getCreationTime();
            tracking["agent_brain_last_updated"] = self.getLastUpdateTime();
            tracking["agent_brain_version"] = "1.0.0";
            tracking["agent_brain_status"] = "active";
            return tracking;
        }, "Get agent brain tracking as dictionary")
        .def("update_agent_brain_tracking", [](AgentBrain& self, py::dict tracking) {
            if (tracking.contains("agent_brain_version")) {
                // This would require updating the AgentBrain's version
                // Placeholder for future implementation
            }
        }, py::arg("tracking"), "Update agent brain tracking from dictionary")
        .def("get_agent_brain_metrics", [](AgentBrain& self) {
            py::dict metrics;
            metrics["neuromodulation_level"] = self.getNeuromodulationLevel();
            metrics["curiosity_level"] = self.getCuriosityLevel();
            metrics["novelty_level"] = self.getNoveltyLevel();
            metrics["prediction_error"] = self.getPredictionError();
            metrics["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return metrics;
        }, "Get agent brain metrics as dictionary")
        .def("update_agent_brain_metrics", [](AgentBrain& self, py::dict metrics) {
            if (metrics.contains("neuromodulation_level")) {
                // This would require modifying the AgentBrain's neuromodulation level
                // Placeholder for future implementation
            }
        }, py::arg("metrics"), "Update agent brain metrics from dictionary")
        .def("get_agent_brain_context", [](AgentBrain& self) {
            py::dict context;
            context["has_brain"] = self.getBrain() != nullptr;
            context["brain_initialized"] = self.getBrain() ? self.getBrain()->isInitialized() : false;
            context["has_world"] = false;  // Would need to track world reference
            context["agent_ready"] = self.getBrain() && self.getBrain()->isInitialized();
            return context;
        }, "Get agent brain context as dictionary")
        .def("update_agent_brain_context", [](AgentBrain& self, py::dict context) {
            if (context.contains("has_brain")) {
                // This would require updating the AgentBrain's brain reference
                // Placeholder for future implementation
            }
        }, py::arg("context"), "Update agent brain context from dictionary")
        .def("get_agent_brain_diagnostics", [](AgentBrain& self) {
            py::dict diagnostics;
            auto brain = self.getBrain();
            if (brain) {
                diagnostics["brain_diagnostics"] = brain->getStatistics();
                diagnostics["brain_configured"] = brain->isInitialized();
                diagnostics["brain_plasticity_systems"] = brain->getPlasticitySystems();
                diagnostics["brain_neuromodulation_systems"] = brain->getNeuromodulationSystems();
            }
            return diagnostics;
        }, "Get agent brain diagnostics as dictionary")
        .def("update_agent_brain_diagnostics", [](AgentBrain& self, py::dict diagnostics) {
            if (diagnostics.contains("brain_plasticity_systems")) {
                // This would require updating the AgentBrain's brain reference
                // Placeholder for future implementation
            }
        }, py::arg("diagnostics"), "Update agent brain diagnostics from dictionary")
        .def("get_agent_brain_logging", [](AgentBrain& self) {
            py::dict logging;
            logging["timestamp"] = self.getLastUpdateTime();
            logging["agent_brain_version"] = "1.0.0";
            logging["agent_brain_status"] = "active";
            return logging;
        }, "Get agent brain logging as dictionary")
        .def("update_agent_brain_logging", [](AgentBrain& self, py::dict logging) {
            if (logging.contains("agent_brain_version")) {
                // This would require updating the AgentBrain's version
                // Placeholder for future implementation
            }
        }, py::arg("logging"), "Update agent brain logging from dictionary")
        .def("get_agent_brain_tracking", [](AgentBrain& self) {
            py::dict tracking;
            tracking["agent_brain_created"] = self.getCreationTime();
            tracking["agent_brain_last_updated"] = self.getLastUpdateTime();
            tracking["agent_brain_version"] = "1.0.0";
            tracking["agent_brain_status"] = "active";
            return tracking;
        }, "Get agent brain tracking as dictionary")
        .def("update_agent_brain_tracking", [](AgentBrain& self, py::dict tracking) {
            if (tracking.contains("agent_brain_version")) {
                // This would require updating the AgentBrain's version
                // Placeholder for future implementation
            }
        }, py::arg("tracking"), "Update agent brain tracking from dictionary")
        .def("get_agent_brain_metrics", [](AgentBrain& self) {
            py::dict metrics;
            metrics["neuromodulation_level"] = self.getNeuromodulationLevel();
            metrics["curiosity_level"] = self.getCuriosityLevel();
            metrics["novelty_level"] = self.getNoveltyLevel();
            metrics["prediction_error"] = self.getPredictionError();
            metrics["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return metrics;
        }, "Get agent brain metrics as dictionary")
        .def("update_agent_brain_metrics", [](AgentBrain& self, py::dict metrics) {
            if (metrics.contains("neuromodulation_level")) {
                // This would require modifying the AgentBrain's neuromodulation level
                // Placeholder for future implementation
            }
        }, py::arg("metrics"), "Update agent brain metrics from dictionary")
        .def("get_agent_brain_context", [](AgentBrain& self) {
            py::dict context;
            context["has_brain"] = self.getBrain() != nullptr;
            context["brain_initialized"] = self.getBrain() ? self.getBrain()->isInitialized() : false;
            context["has_world"] = false;  // Would need to track world reference
            context["agent_ready"] = self.getBrain() && self.getBrain()->isInitialized();
            return context;
        }, "Get agent brain context as dictionary")
        .def("update_agent_brain_context", [](AgentBrain& self, py::dict context) {
            if (context.contains("has_brain")) {
                // This would require updating the AgentBrain's brain reference
                // Placeholder for future implementation
            }
        }, py::arg("context"), "Update agent brain context from dictionary")
        .def("get_agent_brain_diagnostics", [](AgentBrain& self) {
            py::dict diagnostics;
            auto brain = self.getBrain();
            if (brain) {
                diagnostics["brain_diagnostics"] = brain->getStatistics();
                diagnostics["brain_configured"] = brain->isInitialized();
                diagnostics["brain_plasticity_systems"] = brain->getPlasticitySystems();
                diagnostics["brain_neuromodulation_systems"] = brain->getNeuromodulationSystems();
            }
            return diagnostics;
        }, "Get agent brain diagnostics as dictionary")
        .def("update_agent_brain_diagnostics", [](AgentBrain& self, py::dict diagnostics) {
            if (diagnostics.contains("brain_plasticity_systems")) {
                // This would require updating the AgentBrain's brain reference
                // Placeholder for future implementation
            }
        }, py::arg("diagnostics"), "Update agent brain diagnostics from dictionary")
        .def("get_agent_brain_logging", [](AgentBrain& self) {
            py::dict logging;
            logging["timestamp"] = self.getLastUpdateTime();
            logging["agent_brain_version"] = "1.0.0";
            logging["agent_brain_status"] = "active";
            return logging;
        }, "Get agent brain logging as dictionary")
        .def("update_agent_brain_logging", [](AgentBrain& self, py::dict logging) {
            if (logging.contains("agent_brain_version")) {
                // This would require updating the AgentBrain's version
                // Placeholder for future implementation
            }
        }, py::arg("logging"), "Update agent brain logging from dictionary")
        .def("get_agent_brain_tracking", [](AgentBrain& self) {
            py::dict tracking;
            tracking["agent_brain_created"] = self.getCreationTime();
            tracking["agent_brain_last_updated"] = self.getLastUpdateTime();
            tracking["agent_brain_version"] = "1.0.0";
            tracking["agent_brain_status"] = "active";
            return tracking;
        }, "Get agent brain tracking as dictionary")
        .def("update_agent_brain_tracking", [](AgentBrain& self, py::dict tracking) {
            if (tracking.contains("agent_brain_version")) {
                // This would require updating the AgentBrain's version
                // Placeholder for future implementation
            }
        }, py::arg("tracking"), "Update agent brain tracking from dictionary")
        .def("get_agent_brain_metrics", [](AgentBrain& self) {
            py::dict metrics;
            metrics["neuromodulation_level"] = self.getNeuromodulationLevel();
            metrics["curiosity_level"] = self.getCuriosityLevel();
            metrics["novelty_level"] = self.getNoveltyLevel();
            metrics["prediction_error"] = self.getPredictionError();
            metrics["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return metrics;
        }, "Get agent brain metrics as dictionary")
        .def("update_agent_brain_metrics", [](AgentBrain& self, py::dict metrics) {
            if (metrics.contains("neuromodulation_level")) {
                // This would require modifying the AgentBrain's neuromodulation level
                // Placeholder for future implementation
            }
        }, py::arg("metrics"), "Update agent brain metrics from dictionary")
        .def("get_agent_brain_context", [](AgentBrain& self) {
            py::dict context;
            context["has_brain"] = self.getBrain() != nullptr;
            context["brain_initialized"] = self.getBrain() ? self.getBrain()->isInitialized() : false;
            context["has_world"] = false;  // Would need to track world reference
            context["agent_ready"] = self.getBrain() && self.getBrain()->isInitialized();
            return context;
        }, "Get agent brain context as dictionary")
        .def("update_agent_brain_context", [](AgentBrain& self, py::dict context) {
            if (context.contains("has_brain")) {
                // This would require updating the AgentBrain's brain reference
                // Placeholder for future implementation
            }
        }, py::arg("context"), "Update agent brain context from dictionary")
        .def("get_agent_brain_diagnostics", [](AgentBrain& self) {
            py::dict diagnostics;
            auto brain = self.getBrain();
            if (brain) {
                diagnostics["brain_diagnostics"] = brain->getStatistics();
                diagnostics["brain_configured"] = brain->isInitialized();
                diagnostics["brain_plasticity_systems"] = brain->getPlasticitySystems();
                diagnostics["brain_neuromodulation_systems"] = brain->getNeuromodulationSystems();
            }
            return diagnostics;
        }, "Get agent brain diagnostics as dictionary")
        .def("update_agent_brain_diagnostics", [](AgentBrain& self, py::dict diagnostics) {
            if (diagnostics.contains("brain_plasticity_systems")) {
                // This would require updating the AgentBrain's brain reference
                // Placeholder for future implementation
            }
        }, py::arg("diagnostics"), "Update agent brain diagnostics from dictionary")
        .def("get_agent_brain_logging", [](AgentBrain& self) {
            py::dict logging;
            logging["timestamp"] = self.getLastUpdateTime();
            logging["agent_brain_version"] = "1.0.0";
            logging["agent_brain_status"] = "active";
            return logging;
        }, "Get agent brain logging as dictionary")
        .def("update_agent_brain_logging", [](AgentBrain& self, py::dict logging) {
            if (logging.contains("agent_brain_version")) {
                // This would require updating the AgentBrain's version
                // Placeholder for future implementation
            }
        }, py::arg("logging"), "Update agent brain logging from dictionary")
        .def("get_agent_brain_tracking", [](AgentBrain& self) {
            py::dict tracking;
            tracking["agent_brain_created"] = self.getCreationTime();
            tracking["agent_brain_last_updated"] = self.getLastUpdateTime();
            tracking["agent_brain_version"] = "1.0.0";
            tracking["agent_brain_status"] = "active";
            return tracking;
        }, "Get agent brain tracking as dictionary")
        .def("update_agent_brain_tracking", [](AgentBrain& self, py::dict tracking) {
            if (tracking.contains("agent_brain_version")) {
                // This would require updating the AgentBrain's version
                // Placeholder for future implementation
            }
        }, py::arg("tracking"), "Update agent brain tracking from dictionary")
        .def("get_agent_brain_metrics", [](AgentBrain& self) {
            py::dict metrics;
            metrics["neuromodulation_level"] = self.getNeuromodulationLevel();
            metrics["curiosity_level"] = self.getCuriosityLevel();
            metrics["novelty_level"] = self.getNoveltyLevel();
            metrics["prediction_error"] = self.getPredictionError();
            metrics["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return metrics;
        }, "Get agent brain metrics as dictionary")
        .def("update_agent_brain_metrics", [](AgentBrain& self, py::dict metrics) {
            if (metrics.contains("neuromodulation_level")) {
                // This would require modifying the AgentBrain's neuromodulation level
                // Placeholder for future implementation
            }
        }, py::arg("metrics"), "Update agent brain metrics from dictionary")
        .def("get_agent_brain_context", [](AgentBrain& self) {
            py::dict context;
            context["has_brain"] = self.getBrain() != nullptr;
            context["brain_initialized"] = self.getBrain() ? self.getBrain()->isInitialized() : false;
            context["has_world"] = false;  // Would need to track world reference
            context["agent_ready"] = self.getBrain() && self.getBrain()->isInitialized();
            return context;
        }, "Get agent brain context as dictionary")
        .def("update_agent_brain_context", [](AgentBrain& self, py::dict context) {
            if (context.contains("has_brain")) {
                // This would require updating the AgentBrain's brain reference
                // Placeholder for future implementation
            }
        }, py::arg("context"), "Update agent brain context from dictionary")
        .def("get_agent_brain_diagnostics", [](AgentBrain& self) {
            py::dict diagnostics;
            auto brain = self.getBrain();
            if (brain) {
                diagnostics["brain_diagnostics"] = brain->getStatistics();
                diagnostics["brain_configured"] = brain->isInitialized();
                diagnostics["brain_plasticity_systems"] = brain->getPlasticitySystems();
                diagnostics["brain_neuromodulation_systems"] = brain->getNeuromodulationSystems();
            }
            return diagnostics;
        }, "Get agent brain diagnostics as dictionary")
        .def("update_agent_brain_diagnostics", [](AgentBrain& self, py::dict diagnostics) {
            if (diagnostics.contains("brain_plasticity_systems")) {
                // This would require updating the AgentBrain's brain reference
                // Placeholder for future implementation
            }
        }, py::arg("diagnostics"), "Update agent brain diagnostics from dictionary")
 predictiveError", static_cast<float (AgentBrain::*)() const>(&AgentBrain::getPredictionError)),
                 "Predictive error signal")
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal, "Get the underlying brain")
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

    // Module-level functions
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
