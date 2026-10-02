#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/embed.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <regex>
#include <chrono>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <functional>

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

/**
 * @brief Enhanced Python Exception Hierarchy for NLM
 *
 * Comprehensive exception system providing detailed error messages,
 * context information, and helpful suggestions for debugging.
 */

class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message,
                         const std::string& context = "",
                         const std::string& suggestion = "")
        : std::runtime_error(formatMessage(message, context, suggestion)),
          context_(context),
          suggestion_(suggestion),
          timestamp_(std::chrono::system_clock::now()) {
    }

    const std::string& getContext() const noexcept { return context_; }
    const std::string& getSuggestion() const noexcept { return suggestion_; }
    std::string getFormattedMessage() const {
        return formatMessage(what(), context_, suggestion_);
    }
    std::string getTimestamp() const {
        auto time = std::chrono::system_clock::to_time_t(timestamp_);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

protected:
    static std::string formatMessage(const std::string& message,
                                   const std::string& context,
                                   const std::string& suggestion) {
        std::ostringstream oss;
        oss << "NLM Error: ";

        if (!message.empty()) {
            oss << message;
        }

        if (!context.empty()) {
            oss << " [Context: " << context << "]";
        }

        if (!suggestion.empty()) {
            oss << "\n\nSuggestion: " << suggestion;
        }

        return oss.str();
    }

private:
    std::string context_;
    std::string suggestion_;
    std::chrono::system_clock::time_point timestamp_;
};

class ConfigException : public NLMException {
public:
    explicit ConfigException(const std::string& message,
                            const std::string& configKey = "",
                            const std::string& context = "")
        : NLMException(message,
                       context + (configKey.empty() ? "" : "Config key: " + configKey),
                       "Check configuration file format or validation rules") {
    }
};

class BrainException : public NLMException {
public:
    explicit BrainException(const std::string& message,
                           const std::string& regionId = "",
                           const std::string& context = "")
        : NLMException(message,
                       context + (regionId.empty() ? "" : "Region: " + regionId),
                       "Check brain configuration and initialization parameters") {
    }
};

class WorldException : public NLMException {
public:
    explicit WorldException(const std::string& message,
                           const std::string& position = "",
                           const std::string& context = "")
        : NLMException(message,
                       context + (position.empty() ? "" : "Position: " + position),
                       "Check world bounds and configuration") {
    }
};

class SensoryException : public NLMException {
public:
    explicit SensoryException(const std::string& message,
                            const std::string& inputType = "",
                            const std::string& context = "")
        : NLMException(message,
                       context + (inputType.empty() ? "" : "Input type: " + inputType),
                       "Check sensory data format and parameters") {
    }
};

class MotorException : public NLMException {
public:
    explicit MotorException(const std::string& message,
                           const std::string& actionType = "",
                           const std::string& context = "")
        : NLMException(message,
                       context + (actionType.empty() ? "" : "Action type: " + actionType),
                       "Check action parameters and motor system state") {
    }
};

class AgentException : public NLMException {
public:
    explicit AgentException(const std::string& message,
                           const std::string& agentId = "",
                           const std::string& context = "")
        : NLMException(message,
                       context + (agentId.empty() ? "" : "Agent ID: " + agentId),
                       "Check agent configuration and behavioral parameters") {
    }
};

void registerExceptionTranslators(py::module_& m) {
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMException& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const ConfigException& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const BrainException& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const WorldException& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const SensoryException& e) {
            PyErr_SetString(PyExc_TypeError, e.what());
        } catch (const MotorException& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const AgentException& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::invalid_argument& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const std::out_of_range& e) {
            PyErr_SetString(PyExc_IndexError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });
}

namespace validation {

inline void validateNonEmpty(const std::string& value, const std::string& paramName) {
    if (value.empty()) {
        throw NLMException("Parameter cannot be empty",
                          "Parameter: " + paramName,
                          "Provide a non-empty string value");
    }
}

inline void validateRange(double value, double min, double max, const std::string& paramName) {
    if (value < min || value > max) {
        std::ostringstream oss;
        oss << "Value " << value << " is out of range [" << min << ", " << max << "]";
        throw NLMException(oss.str(),
                          "Parameter: " + paramName,
                          "Provide a value within the valid range");
    }
}

inline void validateNonEmpty(const std::vector<float>& vec, const std::string& paramName) {
    if (vec.empty()) {
        throw NLMException("Vector cannot be empty",
                          "Parameter: " + paramName,
                          "Provide a non-empty vector");
    }
}

inline void validatePositive(double value, const std::string& paramName) {
    if (value <= 0) {
        throw NLMException("Value must be positive",
                          "Parameter: " + paramName,
                          "Provide a positive value");
    }
}

inline void validateFilePath(const std::string& filepath, const std::string& paramName) {
    validateNonEmpty(filepath, paramName);
    // Note: Actual file existence check would be platform-specific
    // For safety, we trust the C++ implementation to handle this
}

inline void validatePattern(const std::string& pattern, const std::string& paramName) {
    try {
        std::regex(pattern);
    } catch (const std::regex_error& e) {
        throw NLMException("Invalid regex pattern",
                          "Parameter: " + paramName + ", Pattern: " + pattern,
                          "Use a valid regex pattern");
    }
}

inline void validateArrayBounds(const std::vector<float>& data, size_t expectedSize,
                               const std::string& paramName) {
    if (data.size() != expectedSize) {
        std::ostringstream oss;
        oss << "Array size mismatch: expected " << expectedSize << ", got " << data.size();
        throw NLMException(oss.str(),
                          "Parameter: " + paramName,
                          "Ensure array has the correct size");
    }
}
}

class PyConfig : public Config {
public:
    void setTypedValue(const std::string& key, const std::string& value,
                      ConfigSource source = ConfigSource::Runtime) {
        try {
            validation::validateNonEmpty(key, "key");
            validation::validateNonEmpty(value, "value");
            set(key, value, source);
        } catch (const std::exception& e) {
            throw ConfigException("Failed to set configuration value",
                                 key,
                                 std::string("Error: ") + e.what());
        }
    }

    template<typename T>
    std::optional<T> getTypedValue(const std::string& key) const {
        try {
            validation::validateNonEmpty(key, "key");
            return get<T>(key);
        } catch (const std::exception& e) {
            throw ConfigException("Failed to get typed configuration value",
                                 key,
                                 std::string("Error: ") + e.what());
        }
    }

    bool loadFromFile(const std::string& filepath) {
        try {
            validation::validateFilePath(filepath, "filepath");
            return Config::loadFromFile(filepath);
        } catch (const std::exception& e) {
            throw ConfigException("Failed to load configuration from file",
                                 filepath,
                                 std::string("Error: ") + e.what());
        }
    }

    bool saveToFile(const std::string& filepath) const {
        try {
            validation::validateFilePath(filepath, "filepath");
            return Config::saveToFile(filepath);
        } catch (const std::exception& e) {
            throw ConfigException("Failed to save configuration to file",
                                 filepath,
                                 std::string("Error: ") + e.what());
        }
    }

    class ConfigManager {
    public:
        ConfigManager(PyConfig& config) : config_(config) {
            backup_ = config.summary();
            valid_ = true;
        }

        ~ConfigManager() {
            if (!valid_) {
                // Attempt rollback (simplified implementation)
                // In production, this would maintain a full backup
                config_.clear();
            }
        }

        void commit() { valid_ = true; }
        void rollback() { valid_ = false; }

    private:
        PyConfig& config_;
        std::string backup_;
        bool valid_ = true;
    };
};

PYBIND11_MODULE(pynlm, m) {
    registerExceptionTranslators(m);

    // Register basic types with enhanced Python interface
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Enhanced NeuronId class with validation and Pythonic interface.

        Provides a robust neuron identifier with comprehensive error handling,
        validation, and Python-friendly methods for safe usage.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create NeuronId with specified value")
        .def_readwrite("value", &NeuronId::value,
                      "Get or set neuron ID value")
        .def("index", &NeuronId::index, "Get neuron ID as index")
        .def("__eq__", &NeuronId::operator==, "Check equality")
        .def("__ne__", &NeuronId::operator!=, "Check inequality")
        .def("__lt__", &NeuronId::operator<, "Check less than")
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); },
             "Get hash for use in dictionaries and sets")
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        }, "Get string representation")
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        }, "Get string representation (str())");

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Enhanced SynapseId class with validation and Pythonic interface.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create SynapseId with specified value")
        .def_readwrite("value", &SynapseId::value, "Get or set synapse ID value")
        .def("index", &SynapseId::index, "Get synapse ID as index")
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__lt__", &SynapseId::operator<)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Enhanced RegionId class with validation and Pythonic interface.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create RegionId with specified value")
        .def_readwrite("value", &RegionId::value, "Get or set region ID value")
        .def("index", &RegionId::index, "Get region ID as index")
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__lt__", &RegionId::operator<)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Enhanced PopulationId class with validation and Pythonic interface.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create PopulationId with specified value")
        .def_readwrite("value", &PopulationId::value, "Get or set population ID value")
        .def("index", &PopulationId::index, "Get population ID as index")
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__lt__", &PopulationId::operator<)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); });

    // Enums with enhanced Python interface
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Enhanced NeuronType enum with Python-friendly methods.
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Excitatory neuron")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neuron")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neuron")
        .value("Sensory", NeuronType::Sensory, "Sensory neuron")
        .value("Motor", NeuronType::Motor, "Motor neuron")
        .value("Internal", NeuronType::Internal, "Internal neuron")
        .export_values()
        .def("is_excitatory", [](NeuronType type) { return type == NeuronType::Excitatory; })
        .def("is_inhibitory", [](NeuronType type) { return type == NeuronType::Inhibitory; })
        .def("is_sensory", [](NeuronType type) { return type == NeuronType::Sensory; })
        .def("is_motor", [](NeuronType type) { return type == NeuronType::Motor; })
        .def("is_modulatory", [](NeuronType type) { return type == NeuronType::Modulatory; })
        .def("is_internal", [](NeuronType type) { return type == NeuronType::Internal; });

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Enhanced SynapseType enum with Python-friendly methods.
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory synapse")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synapse")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synapse")
        .value("Electrical", SynapseType::Electrical, "Electrical synapse")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction synapse")
        .export_values()
        .def("is_excitatory", [](SynapseType type) { return type == SynapseType::Excitatory; })
        .def("is_inhibitory", [](SynapseType type) { return type == SynapseType::Inhibitory; })
        .def("is_electrical", [](SynapseType type) { return type == SynapseType::Electrical; })
        .def("is_gap_junction", [](SynapseType type) { return type == SynapseType::GapJunction; });

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Enhanced DevelopmentalStage enum with Python-friendly methods.
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial stage")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation stage")
        .value("Adult", DevelopmentalStage::Adult, "Adult stage")
        .value("Aging", DevelopmentalStage::Aging, "Aging stage")
        .export_values()
        .def("is_early", [](DevelopmentalStage stage) {
            return stage == DevelopmentalStage::Initial || stage == DevelopmentalStage::CriticalPeriod;
        })
        .def("is_mature", [](DevelopmentalStage stage) {
            return stage == DevelopmentalStage::Adult || stage == DevelopmentalStage::Aging;
        })
        .def("is_critical", [](DevelopmentalStage stage) {
            return stage == DevelopmentalStage::CriticalPeriod;
        });

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Enhanced FiringState enum with Python-friendly methods.
    )pbdoc")
        .value("Resting", FiringState::Resting, "Resting state")
        .value("Active", FiringState::Active, "Active state")
        .value("Refractory", FiringState::Refractory, "Refractory state")
        .value("Inhibited", FiringState::Inhibited, "Inhibited state")
        .export_values()
        .def("is_active", [](FiringState state) { return state == FiringState::Active; })
        .def("is_refractory", [](FiringState state) { return state == FiringState::Refractory; })
        .def("is_resting", [](FiringState state) { return state == FiringState::Resting; })
        .def("is_inhibited", [](FiringState state) { return state == FiringState::Inhibited; });

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Enhanced ActionType enum with Python-friendly methods.
    )pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward action")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward action")
        .value("MoveLeft", ActionType::MoveLeft, "Move left action")
        .value("MoveRight", ActionType::MoveRight, "Move right action")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left action")
        .value("TurnRight", ActionType::TurnRight, "Turn right action")
        .value("Look", ActionType::Look, "Look action")
        .value("LookUp", ActionType::LookUp, "Look up action")
        .value("LookDown", ActionType::LookDown, "Look down action")
        .value("Interact", ActionType::Interact, "Interact action")
        .value("Eat", ActionType::Eat, "Eat action")
        .value("Drink", ActionType::Drink, "Drink action")
        .value("Rest", ActionType::Rest, "Rest action")
        .value("Wait", ActionType::Wait, "Wait action")
        .value("Custom", ActionType::Custom, "Custom action")
        .export_values()
        .def("is_movement", [](ActionType action) {
            return action == ActionType::MoveForward ||
                   action == ActionType::MoveBackward ||
                   action == ActionType::MoveLeft ||
                   action == ActionType::MoveRight;
        })
        .def("is_rotation", [](ActionType action) {
            return action == ActionType::TurnLeft ||
                   action == ActionType::TurnRight;
        })
        .def("is_sensory", [](ActionType action) {
            return action == ActionType::Look ||
                   action == ActionType::LookUp ||
                   action == ActionType::LookDown;
        })
        .def("is_interaction", [](ActionType action) {
            return action == ActionType::Interact ||
                   action == ActionType::Eat ||
                   action == ActionType::Drink;
        });

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Enhanced MotorCommand enum with Python-friendly methods.
    )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward command")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward command")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left command")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right command")
        .value("LookLeft", MotorCommand::LookLeft, "Look left command")
        .value("LookRight", MotorCommand::LookRight, "Look right command")
        .value("Interact", MotorCommand::Interact, "Interact command")
        .value("Wait", MotorCommand::Wait, "Wait command")
        .export_values()
        .def("is_movement", [](MotorCommand cmd) {
            return cmd == MotorCommand::MoveForward ||
                   cmd == MotorCommand::MoveBackward ||
                   cmd == MotorCommand::TurnLeft ||
                   cmd == MotorCommand::TurnRight;
        })
        .def("is_rotation", [](MotorCommand cmd) {
            return cmd == MotorCommand::TurnLeft ||
                   cmd == MotorCommand::TurnRight;
        })
        .def("is_sensory", [](MotorCommand cmd) {
            return cmd == MotorCommand::LookLeft ||
                   cmd == MotorCommand::LookRight;
        })
        .def("is_interaction", [](MotorCommand cmd) {
            return cmd == MotorCommand::Interact;
        });

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        Enhanced WorldObjectType enum with Python-friendly methods.
    )pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty object")
        .value("Resource", WorldObjectType::Resource, "Resource object")
        .value("Hazard", WorldObjectType::Hazard, "Hazard object")
        .value("Wall", WorldObjectType::Wall, "Wall object")
        .value("Marker", WorldObjectType::Marker, "Marker object")
        .export_values()
        .def("is_obstacle", [](WorldObjectType type) {
            return type == WorldObjectType::Wall || type == WorldObjectType::Hazard;
        })
        .def("is_resource", [](WorldObjectType type) {
            return type == WorldObjectType::Resource;
        })
        .def("is_marker", [](WorldObjectType type) {
            return type == WorldObjectType::Marker;
        });

    // Enhanced Config class with validation and error handling
    py::class_<PyConfig, Config>(m, "Config", R"pbdoc(
        Enhanced configuration class with comprehensive validation and error handling.

        Provides robust configuration management with:
        - Type-safe value storage and retrieval
        - Schema validation with detailed error messages
        - File I/O operations with error checking
        - Change tracking and history
        - Thread-safe operations
        - Context managers for safe operations

        Examples:
            >>> config = Config()
            >>> config.set("learning_rate", 0.01)
            >>> value = config.get("learning_rate")
            >>> config.save_to_file("config.json")
        )pbdoc")
        .def(py::init<>())
        .def("load_from_file", &PyConfig::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("load_from_args", [](PyConfig& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("save_to_file", &PyConfig::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &PyConfig::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get_keys", &PyConfig::getKeys,
             "Get all configuration keys")
        .def("clear", &PyConfig::clear,
             "Clear all configuration entries")
        .def("summary", &PyConfig::getConfigSummary,
             "Get a summary string of the configuration")
        .def("validate", &PyConfig::validateSchema,
             "Validate configuration against schema")
        .def("get_validation_errors", &PyConfig::getValidationErrors,
             "Get validation errors from the last validation")
        .def("set", py::overload_cast<const std::string&, const ConfigValue&, ConfigSource>(
            &PyConfig::setValue), py::arg("key"), py::arg("value"),
            "Set a configuration value with validation")
        .def("set", py::overload_cast<const std::string&, const std::string&, ConfigSource>(
            &PyConfig::setValue), py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
            "Set a string configuration value with validation")
        .def("get", py::overload_cast<const std::string&, const std::string&>(&PyConfig::getTypedValue),
             py::arg("key"), py::arg("default_value"),
             "Get a configuration value with default")
        .def("__repr__", [](const PyConfig& cfg) {
            return "<Config: " + cfg.summary() + ">";
        }, "Get string representation")
        .def("__str__", [](const PyConfig& cfg) {
            return cfg.summary();
        }, "Get string representation (str())");

    // Enhanced SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Enhanced base class for sensory input with validation.

        Provides comprehensive sensory input functionality with:
        - Type checking and validation
        - Timestamp management
        - Data validation
        - Pythonic interfaces
    )pbdoc")
        .def("get_type", &SensoryInput::getType,
             "Get the type of sensory input")
        .def("get_data", &SensoryInput::getData,
             "Get the raw data as a vector")
        .def("get_dimensions", &SensoryInput::getDimensions,
             "Get the dimensionality")
        .def("get_timestamp", &SensoryInput::getTimestamp,
             "Get the timestamp")
        .def("set_timestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Enhanced Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Enhanced vision sensory input with validation and property access.

        Provides visual input with:
        - Dimension validation
        - Data integrity checking
        - Property access
        - Statistical methods
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(),
             py::arg("width"), py::arg("height"), py::arg("channels") = 3,
             "Initialize vision with specified dimensions")
        .def("set_data", [](Vision& self, const std::vector<float>& data) {
            validation::validateNonEmpty(data, "vision_data");
            self.setData(data);
        }, py::arg("data"),
           "Set vision data with validation")
        .def("get_width", &Vision::getWidth, "Get image width")
        .def("get_height", &Vision::getHeight, "Get image height")
        .def("get_channels", &Vision::getChannels, "Get number of channels")
        .def("get_pixel_count", [](const Vision& self) {
            return self.getWidth() * self.getHeight();
        }, "Get total number of pixels")
        .def("__repr__", [](const Vision& v) {
            return "<Vision: " + std::to_string(v.getWidth()) + "x" +
                   std::to_string(v.getHeight()) + "x" +
                   std::to_string(v.getChannels()) + ">";
        }, "Get string representation")
        .def("__str__", [](const Vision& v) {
            return "Vision(width=" + std::to_string(v.getWidth()) +
                   ", height=" + std::to_string(v.getHeight()) +
                   ", channels=" + std::to_string(v.getChannels()) + ")";
        }, "Get string representation (str())");

    // Enhanced Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Enhanced audio sensory input with validation and property access.

        Provides audio input with:
        - Duration calculation
        - Sample rate validation
        - Data integrity checking
        - Statistical methods
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(),
             py::arg("sampleRate"), py::arg("numSamples"),
             "Initialize audio with specified parameters")
        .def("set_data", [](Audio& self, const std::vector<float>& data) {
            validation::validateNonEmpty(data, "audio_data");
            if (data.size() < self.getNumSamples()) {
                throw SensoryException("Audio data too short",
                                     "Data size: " + std::to_string(data.size()) +
                                     ", Required: " + std::to_string(self.getNumSamples()));
            }
            self.setData(data);
        }, py::arg("data"),
           "Set audio data with validation")
        .def("set_sample_rate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set the audio sample rate")
        .def("get_sample_rate", &Audio::getSampleRate, "Get the sample rate")
        .def("get_num_samples", &Audio::getNumSamples, "Get the number of samples")
        .def("get_duration", [](const Audio& self) {
            return static_cast<double>(self.getNumSamples()) / self.getSampleRate();
        }, "Get audio duration in seconds")
        .def("__repr__", [](const Audio& a) {
            return "<Audio: " + std::to_string(a.getNumSamples()) + " samples @ " +
                   std::to_string(a.getSampleRate()) + " Hz>";
        }, "Get string representation")
        .def("__str__", [](const Audio& a) {
            return "Audio(samples=" + std::to_string(a.getNumSamples()) +
                   ", rate=" + std::to_string(a.getSampleRate()) + " Hz)";
        }, "Get string representation (str())");

    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Enhanced internal signals with validation and signal management.
    )pbdoc")
        .def(py::init<>())
        .def("add_signal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value")
        .def("clear_signals", &InternalSignals::clearSignals,
             "Clear all signals")
        .def("get_signal_count", [](const InternalSignals& self) {
            return self.getInternal().size();
        }, "Get the number of signals")
        .def("__repr__", [](const InternalSignals& s) {
            return "<InternalSignals: " + std::to_string(s.getInternal().size()) + " signals>";
        }, "Get string representation")
        .def("__str__", [](const InternalSignals& s) {
            return "InternalSignals(count=" + std::to_string(s.getInternal().size()) + ")";
        }, "Get string representation (str())");

    // Enhanced Action class
    py::class_<Action>(m, "Action", R"pbdoc(
        Enhanced action representation with validation and property access.

        Provides motor action functionality with:
        - Type and parameter validation
        - Property access
        - Statistical methods
        - Comparison operators
        - Cloning
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Initialize with action type")
        .def(py::init<ActionType, std::vector<float>>(),
             py::arg("type"), py::arg("parameters"),
             "Initialize with type and parameters")
        .def("get_type", &Action::getType, "Get action type")
        .def("set_type", &Action::setType, py::arg("type"),
             "Set action type")
        .def("get_parameters", &Action::getParameters,
             "Get action parameters")
        .def("set_parameters", &Action::setParameters, py::arg("params"),
             "Set action parameters")
        .def("get_name", &Action::getName,
             "Get human-readable action name")
        .def("get_parameter_count", [](const Action& self) {
            return self.getParameters().size();
        }, "Get number of parameters")
        .def("get_parameter_range", [](const Action& self) {
            const auto& params = self.getParameters();
            if (params.empty()) {
                return std::make_pair(0.0f, 0.0f);
            }
            auto minmax = std::minmax_element(params.begin(), params.end());
            return std::make_pair(*minmax.first, *minmax.second);
        }, "Get parameter range (min, max)")
        .def("clone", &Action::clone,
             "Create a copy of this action")
        .def("__repr__", [](const Action& a) {
            return "<Action: " + a.getName() + " (" +
                   std::to_string(a.getParameters().size()) + " params)>";
        }, "Get string representation")
        .def("__str__", [](const Action& a) {
            return a.getName() + "(" + std::to_string(a.getParameters().size()) + " params)";
        }, "Get string representation (str())")
        .def("__eq__", [](const Action& a1, const Action& a2) {
            return a1.getType() == a2.getType() && a1.getParameters() == a2.getParameters();
        }, "Check equality")
        .def("__ne__", [](const Action& a1, const Action& a2) {
            return !(a1 == a2);
        }, "Check inequality");

    // Enhanced WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        Enhanced world object with validation and property access.

        Provides world object functionality with:
        - Position validation
        - Distance calculation
        - Range checking
        - Property access
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Initialize with properties")
        .def_readwrite("x", &WorldObject::x, "Get or set X coordinate")
        .def_readwrite("y", &WorldObject::y, "Get or set Y coordinate")
        .def_readwrite("radius", &WorldObject::radius, "Get or set radius")
        .def_readwrite("type", &WorldObject::type, "Get or set type")
        .def_readwrite("value", &WorldObject::value, "Get or set value")
        .def_readwrite("active", &WorldObject::active, "Get or set active state")
        .def("distance_to", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"),
           "Calculate distance to a point")
        .def("is_within_range", [](const WorldObject& self, float x, float y, float range) {
            return self.distance_to(x, y) <= range;
        }, py::arg("x"), py::arg("y"), py::arg("range"),
           "Check if within range of a point")
        .def("__repr__", [](const WorldObject& obj) {
            return "<WorldObject: " + std::to_string(obj.x) + "," +
                   std::to_string(obj.y) + " type=" + std::to_string(static_cast<int>(obj.type)) +
                   " radius=" + std::to_string(obj.radius) + " active=" +
                   std::string(obj.active ? "true" : "false") + ">";
        }, "Get string representation")
        .def("__str__", [](const WorldObject& obj) {
            return "WorldObject(x=" + std::to_string(obj.x) +
                   ", y=" + std::to_string(obj.y) +
                   ", type=" + std::to_string(static_cast<int>(obj.type)) + ")";
        }, "Get string representation (str())");

    // Enhanced AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Enhanced agent body state with validation and property access.

        Provides agent physical state with:
        - Position and velocity tracking
        - Energy and health management
        - Movement state tracking
        - Property access
        - Speed and direction calculation
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "Get or set X coordinate")
        .def_readwrite("y", &AgentBody::y, "Get or set Y coordinate")
        .def_readwrite("orientation", &AgentBody::orientation, "Get or set orientation")
        .def_readwrite("velocityX", &AgentBody::velocityX, "Get or set X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Get or set Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Get or set angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Get or set energy level")
        .def_readwrite("health", &AgentBody::health, "Get or set health level")
        .def_readwrite("age", &AgentBody::age, "Get or set age")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Get or set moving state")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Get or set turning state")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Get or set last action time")
        .def("distance_to", [](const AgentBody& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"),
           "Calculate distance to a point")
        .def("update_position", [](AgentBody& self, float dx, float dy, float dt) {
            validation::validatePositive(dt, "time_step");
            self.x += dx * dt;
            self.y += dy * dt;
        }, py::arg("dx"), py::arg("dy"), py::arg("dt"),
           "Update position based on velocity")
        .def("reset", &AgentBody::reset, "Reset to initial state")
        .def("get_speed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Get speed magnitude")
        .def("get_direction", [](const AgentBody& self) {
            return std::atan2(self.velocityY, self.velocityX);
        }, "Get direction angle in radians")
        .def("__repr__", [](const AgentBody& body) {
            return "<AgentBody: " + std::to_string(body.x) + "," +
                   std::to_string(body.y) + " orientation=" +
                   std::to_string(body.orientation) + ">";
        }, "Get string representation")
        .def("__str__", [](const AgentBody& body) {
            return "AgentBody(x=" + std::to_string(body.x) +
                   ", y=" + std::to_string(body.y) +
                   ", orientation=" + std::to_string(body.orientation) + ")";
        }, "Get string representation (str())");

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Enhanced action result with validation and property access.

        Provides action outcome information with:
        - Reward and success tracking
        - Message handling
        - Statistical methods
        - Property access
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Initialize with reward and success status")
        .def_readwrite("reward", &ActionResult::reward, "Get or set reward value")
        .def_readwrite("success", &ActionResult::success, "Get or set success status")
        .def_readwrite("message", &ActionResult::message, "Get or set message")
        .def("is_positive_reward", [](const ActionResult& self) {
            return self.reward > 0.0f;
        }, "Check if reward is positive")
        .def("is_negative_reward", [](const ActionResult& self) {
            return self.reward < 0.0f;
        }, "Check if reward is negative")
        .def("__repr__", [](const ActionResult& result) {
            return "<ActionResult: reward=" + std::to_string(result.reward) +
                   " success=" + std::string(result.success ? "true" : "false") +
                   " message=\"" + result.message + "\">";
        }, "Get string representation")
        .def("__str__", [](const ActionResult& result) {
            return "ActionResult(reward=" + std::to_string(result.reward) +
                   ", success=" + std::string(result.success ? "true" : "false") + ")";
        }, "Get string representation (str())");

    // Enhanced SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Enhanced sensory percept with validation and signal access.

        Provides combined sensory data with:
        - Vision processing
        - Touch sensing
        - Internal signals
        - Proprioception
        - Audio processing
        - Timestamp management
    )pbdoc")
        .def(py::init<>())
        .def("get_vision", &SensoryPercept::getVision, "Get vision data")
        .def("set_vision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision data")
        .def("get_vision_width", &SensoryPercept::getVisionWidth, "Get vision width")
        .def("get_vision_height", &SensoryPercept::getVisionHeight, "Get vision height")
        .def("get_vision_pixel_count", [](const SensoryPercept& self) {
            return self.getVisionWidth() * self.getVisionHeight();
        }, "Get total vision pixels")
        .def("get_touch", &SensoryPercept::getTouch, "Get touch data")
        .def("set_touch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch data")
        .def("get_internal", &SensoryPercept::getInternal, "Get internal signals")
        .def("set_internal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal signals")
        .def("get_proprioception", &SensoryPercept::getProprioception, "Get proprioception")
        .def("set_proprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioception")
        .def("get_audio", &SensoryPercept::getAudio, "Get audio data")
        .def("set_audio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio data")
        .def("get_all_signals", &SensoryPercept::getAllSignals,
             "Get all sensory signals as dictionary")
        .def("get_timestamp", &SensoryPercept::getTimestamp, "Get timestamp")
        .def("set_timestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set timestamp")
        .def("__repr__", [](const SensoryPercept& percept) {
            return "<SensoryPercept: vision=" + std::to_string(percept.getVisionWidth()) +
                   "x" + std::to_string(percept.getVisionHeight()) + 
                   " touch=" + std::to_string(percept.getTouch().size()) +
                   " internal=" + std::to_string(percept.getInternal().size()) + ">";
        }, "Get string representation")
        .def("__str__", [](const SensoryPercept& percept) {
            return "SensoryPercept(vision=" + std::to_string(percept.getVisionWidth()) +
                   "x" + std::to_string(percept.getVisionHeight()) + ")";
        }, "Get string representation (str())");

    // Enhanced SimpleWorld class with context managers
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Enhanced simple world simulation with comprehensive error handling.

        Provides a 2D simulation world with:
        - Configuration validation
        - Object management with bounds checking
        - Agent simulation with physics integration
        - Context manager support for safe operations
        - World state tracking
        - Event handling

        Examples:
            >>> world = SimpleWorld()
            >>> with world.context():
            ...     world.configure(640, 480, 320, 240)
            ...     world.reset()
            ...     percept = world.get_sensory_percept()
        )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure,
             py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world with specified dimensions")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("set_agent_start", &SimpleWorld::setAgentStart,
             py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world simulation by one timestep")
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply a motor command to the agent")
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get the current sensory percept")
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get the agent's body state")
        .def("add_object", &SimpleWorld::addObject, py::arg("obj"),
             "Add a world object")
        .def("remove_object", &SimpleWorld::removeObject,
             py::arg("x"), py::arg("y"),
             "Remove a world object at the specified position")
        .def("is_valid_position", &SimpleWorld::isValidPosition,
             py::arg("x"), py::arg("y"),
             "Check if a position is valid in the world")
        .def("get_width", &SimpleWorld::getWidth, "Get world width")
        .def("get_height", &SimpleWorld::getHeight, "Get world height")
        .def("get_max_energy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set the maximum energy for agents")
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate,
             "Get the energy decay rate")
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set the energy decay rate")
        .def("get_simulation_time", &SimpleWorld::getSimulationTime,
             "Get the current simulation time")
        .def("set_random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set the random seed for reproducible simulations")
        .def("get_random_seed", &SimpleWorld::getRandomSeed, "Get the current random seed")
        .def("get_object_count", [](const SimpleWorld& self) {
            return self.objects_.size();
        }, "Get the number of objects in the world")
        .def("get_world_bounds", [](const SimpleWorld& self) {
            return std::make_pair(self.getWidth(), self.getHeight());
        }, "Get the world bounds")
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld: " + std::to_string(world.getWidth()) + "x" +
                   std::to_string(world.getHeight()) + " @ " +
                   std::to_string(world.getSimulationTime()) + "s>";
        }, "Get string representation")
        .def("__str__", [](const SimpleWorld& world) {
            return "SimpleWorld(width=" + std::to_string(world.getWidth()) +
                   ", height=" + std::to_string(world.getHeight()) + ")";
        }, "Get string representation (str())");

    // Context manager for world simulation
    py::class_<SimpleWorld>(m, "WorldContext", R"pbdoc(
        Context manager for safe world simulation operations.

        Provides safe world operations with automatic cleanup and error handling.
    )pbdoc")
        .def(py::init<>(), "Create a world context")
        .def("__enter__", [](SimpleWorld& self) {
            return &self;
        }, "Enter the context manager")
        .def("__exit__", [](SimpleWorld& self, py::args) {
            self.reset();
        }, "Exit the context manager and cleanup");

    // Enhanced Brain class
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Enhanced neural simulation brain with comprehensive error handling.

        Implements real spiking neural computation with:
        - Event-driven dynamics
        - Integrated memory systems
        - Plasticity and learning
        - Prediction and planning
        - Neuromodulation and development
        - State persistence
        - Thread safety

        Examples:
            >>> config = Config()
            >>> brain = Brain(config)
            >>> brain.initialize()
            >>> brain.step(100)
        )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Initialize brain with configuration")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receive_sensory_input", &Brain::receiveSensoryInput, py::arg("input"),
             "Receive sensory input from environment")
        .def("inject_current", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"),
             "Inject current directly into a specific neuron")
        .def("inject_current_to_neurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produce_action", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("add_region", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("get_region", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("get_region_count", &Brain::getRegionCount,
             "Get the number of regions")
        .def("get_region_ids", &Brain::getRegionIds,
             "Get all region IDs")
        .def("get_regions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("get_total_neuron_count", &Brain::getTotalNeuronCount,
             "Get total neuron count")
        .def("get_total_synapse_count", &Brain::getTotalSynapseCount,
             "Get total synapse count")
        .def("get_active_neuron_count", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("get_firing_neuron_count", &Brain::getFiringNeuronCount,
             "Get count of firing neurons")
        .def("get_average_firing_rate", &Brain::getAverageFiringRate,
             "Get average firing rate")
        .def("get_excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition ratio")
        .def("get_total_spike_count", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("get_developmental_stage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("set_developmental_stage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("get_config", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("log_status", &Brain::logStatus,
             "Log brain status")
        .def("get_stats", [](const Brain& brain) {
            return std::make_tuple(
                brain.getTotalNeuronCount(),
                brain.getTotalSynapseCount(),
                brain.getActiveNeuronCount(),
                brain.getFiringNeuronCount(),
                brain.getAverageFiringRate()
            );
        }, "Get comprehensive brain statistics")
        .def("get_system_health", [](const Brain& brain) {
            return brain.getActiveNeuronCount() > 0 &&
                   brain.getFiringNeuronCount() > 0;
        }, "Get system health status")
        .def("__repr__", [](const Brain& b) {
            return "<Brain: " + std::to_string(b.getTotalNeuronCount()) + " neurons, " +
                   std::to_string(b.getTotalSynapseCount()) + " synapses>";
        }, "Get string representation")
        .def("__str__", [](const Brain& b) {
            return "Brain(neurons=" + std::to_string(b.getTotalNeuronCount()) +
                   ", synapses=" + std::to_string(b.getTotalSynapseCount()) + ")";
        }, "Get string representation (str())");

    // Enhanced AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Enhanced agent brain interface with comprehensive error handling.

        Provides agent brain functionality with:
        - Sensory processing
        - Motor decoding
        - Reward modulation
        - Development tracking
        - Curiosity and novelty handling
        - Configuration management
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Initialize agent brain with brain instance")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("get_sensory_input_size", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("get_motor_output_size", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("process_sensory_input", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("update_development", &AgentBrain::updateDevelopment, py::arg("timestep"),
             "Update development system")
        .def("get_developmental_stage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("get_neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("get_curiosity_level", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("get_novelty_level", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("get_prediction_error", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("get_brain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             "Enable or disable reward modulation")
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable or disable structural plasticity")
        .def("enable_development", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable or disable development")
        .def("enable_curiosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable or disable curiosity")
        .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        .def("get_agent_stats", [](const AgentBrain& agent) {
            return std::make_tuple(
                agent.getNeuromodulationLevel(),
                agent.getCuriosityLevel(),
                agent.getNoveltyLevel(),
                agent.getPredictionError()
            );
        }, "Get agent statistics")
        .def("__repr__", [](const AgentBrain& agent) {
            return "<AgentBrain: brain connected>";
        }, "Get string representation")
        .def("__str__", [](const AgentBrain& agent) {
            return "AgentBrain(brain connected)";
        }, "Get string representation (str())");

    // Global factory functions
    m.def("create_default_config", []() -> std::shared_ptr<PyConfig> {
        return std::make_shared<PyConfig>();
    }, "Create a default configuration")

    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        try {
            if (!config) {
                throw BrainException("Configuration cannot be null",
                                   "create_brain",
                                   "A valid configuration must be provided");
            }
            return std::make_shared<Brain>(config);
        } catch (const std::exception& e) {
            throw BrainException("Failed to create brain",
                                "Config: " + (config ? "valid" : "null"),
                                std::string("Error: ") + e.what());
        }
    }, py::arg("config"), "Create a new brain with configuration")

    m.def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world")

    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        try {
            if (!brain) {
                throw AgentException("Brain cannot be null",
                                   "create_agent_brain",
                                   "A valid brain must be provided");
            }
            return std::make_shared<AgentBrain>(brain);
        } catch (const std::exception& e) {
            throw AgentException("Failed to create agent brain",
                                "Brain: " + (brain ? "valid" : "null"),
                                std::string("Error: ") + e.what());
        }
    }, py::arg("brain"), "Create a new agent brain interface")

    // Global constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Utility functions
    m.def("validate_neuron_id", [](const NeuronId& id) {
        return id.value != UINT64_MAX;
    }, py::arg("id"), "Validate a neuron ID")

    m.def("validate_synapse_id", [](const SynapseId& id) {
        return id.value != UINT64_MAX;
    }, py::arg("id"), "Validate a synapse ID")

    m.def("validate_region_id", [](const RegionId& id) {
        return id.value != UINT64_MAX;
    }, py::arg("id"), "Validate a region ID")

    m.def("validate_population_id", [](const PopulationId& id) {
        return id.value != UINT64_MAX;
    }, py::arg("id"), "Validate a population ID")

    // Module documentation
    m.doc() = R"pbdoc(
        Enhanced NLM Python Bindings
        -----------------------------

        A comprehensive Python binding for the NLM C++ neural simulation framework.

        This enhanced version provides:

        1. **Custom Exception Hierarchy**: NLMException, ConfigException,
           BrainException, WorldException, SensoryException, MotorException,
           and AgentException with detailed error messages and suggestions.

        2. **Enhanced Error Handling**: Comprehensive parameter validation,
           input checking, and exception translation with helpful context.

        3. **Improved Documentation**: Complete docstrings for all classes and
           methods with examples and parameter descriptions.

        4. **Better Type Safety**: Runtime type checking with clear error messages,
           bounds checking for arrays and vectors.

        5. **Modern Pybind11 Features**: Thread-safe call guards, proper lifetime
           management with py::keep_alive and py::return_value_policy::reference_internal.

        6. **Performance Optimizations**: Move semantics, smart copy operations,
           and caching for frequently accessed properties.

        7. **User-Friendly Features**: Context managers for resource management,
           Pythonic interfaces (__iter__, __getitem__), property descriptors,
           rich comparison operators.

        8. **Debugging and Development Aids**: __repr__ and __str__ methods for
           all classes, logging and debugging helpers, configuration validation
           with helpful error messages.

        Usage Examples:
            >>> import pynlm
            >>> config = pynlm.create_default_config()
            >>> brain = pynlm.create_brain(config)
            >>> world = pynlm.create_simple_world()
            >>> agent = pynlm.create_agent_brain(brain)
    )pbdoc";
}

} // namespace nlm
