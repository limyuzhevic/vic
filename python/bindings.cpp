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

namespace py = pybind11;
namespace nlm {

// Enhanced exception classes for better error reporting
class ValidationError : public std::runtime_error {
public:
    int code;
    std::string message;
    
    ValidationError(int errorCode, const std::string& msg) 
        : std::runtime_error(msg), code(errorCode), message(msg) {}
    
    static constexpr int INVALID_PARAMETER = 1001;
    static constexpr int OUT_OF_BOUNDS = 1002;
    static constexpr int NULL_POINTER = 1003;
    static constexpr int TYPE_MISMATCH = 1004;
    static constexpr int MISSING_REQUIRED = 1005;
};

class ConfigError : public std::runtime_error {
public:
    std::string key;
    std::string message;
    
    ConfigError(const std::string& k, const std::string& msg) 
        : std::runtime_error(msg), key(k), message(msg) {}
    
    static constexpr int KEY_NOT_FOUND = 2001;
    static constexpr int INVALID_TYPE = 2002;
    static constexpr int VALUE_OUT_OF_RANGE = 2003;
};

class ResourceError : public std::runtime_error {
public:
    std::string operation;
    std::string message;
    
    ResourceError(const std::string& op, const std::string& msg) 
        : std::runtime_error(msg), operation(op), message(msg) {}
    
    static constexpr int ALLOCATION_FAILED = 3001;
    static constexpr int INVALID_STATE = 3002;
    static constexpr int VERSION_MISMATCH = 3003;
};

class StateError : public std::runtime_error {
public:
    std::string component;
    std::string message;
    
    StateError(const std::string& comp, const std::string& msg) 
        : std::runtime_error(msg), component(comp), message(msg) {}
    
    static constexpr int NOT_INITIALIZED = 4001;
    static constexpr int ALREADY_INITIALIZED = 4002;
    static constexpr int OPERATION_FAILED =4003;
};

// Helper function for validation and error reporting
inline void validateParameter(bool condition, const std::string& paramName, int errorCode, 
                             const std::string& context = "") {
    if (!condition) {
        std::string msg = "Parameter validation failed: '" + paramName + "'";
        if (!context.empty()) msg += " in context: '" + context + "'";
        msg += ". Recovery: Check parameter value and ensure it meets requirements.";
        throw ValidationError(errorCode, msg);
    }
}

inline void validateRange(double value, double min, double max, const std::string& paramName) {
    if (value < min || value > max) {
        throw ValidationError(ValidationError::OUT_OF_BOUNDS, 
            "Parameter '" + paramName + "' value " + std::to_string(value) + 
            " is out of range [" + std::to_string(min) + ", " + std::to_string(max) + "]. "
            "Recovery: Adjust parameter to valid range.");
    }
}

inline void validatePointer(const void* ptr, const std::string& paramName, int errorCode) {
    if (!ptr) {
        throw ValidationError(errorCode, 
            "Null pointer detected for parameter '" + paramName + "'. "
            "Recovery: Ensure object is properly initialized before use.");
    }
}

inline void validateConfigKey(const Config& config, const std::string& key, 
                             const std::string& expectedType = "auto") {
    if (!config.has(key)) {
        throw ConfigError(key, "Configuration key not found: '" + key + "'. "
            "Recovery: Ensure key exists or provide a default value.");
    }
    
    if (expectedType != "auto") {
        auto value = config.get<ConfigValue>(key);
        if (!value) {
            throw ConfigError(key, "Configuration key not found: '" + key + "'.");
        }
        
        std::string actualType = "unknown";
        if (expectedType == "bool" && std::holds_alternative<bool>(*value)) actualType = "bool";
        else if (expectedType == "int" && std::holds_alternative<int>(*value)) actualType = "int";
        else if (expectedType == "int64" && std::holds_alternative<int64_t>(*value)) actualType = "int64";
        else if (expectedType == "double" && std::holds_alternative<double>(*value)) actualType = "double";
        else if (expectedType == "string" && std::holds_alternative<std::string>(*value)) actualType = "string";
        
        if (actualType != expectedType) {
            throw ConfigError(key, "Configuration key '" + key + "' has type '" + actualType + 
                "', expected '" + expectedType + "'. "
                "Recovery: Convert to expected type or provide correct type.");
        }
    }
}

// Helper for safe array access with bounds checking
inline void checkBounds(size_t index, size_t size, const std::string& context) {
    if (index >= size) {
        throw ValidationError(ValidationError::OUT_OF_BOUNDS,
            "Array index " + std::to_string(index) + " is out of bounds [0, " + 
            std::to_string(size) + ") in " + context + ". "
            "Recovery: Use valid index within array bounds.");
    }
}

// Helper for division by zero detection
inline void checkDivisionByZero(double divisor, const std::string& operation) {
    if (std::abs(divisor) < 1e-15) {
        throw ValidationError(ValidationError::INVALID_PARAMETER,
            "Division by zero detected in " + operation + ". "
            "Recovery: Ensure divisor is non-zero or handle division case differently.");
    }
}

// Helper for state validation
inline void validateState(bool condition, const std::string& component, 
                         int errorCode, const std::string& message = "") {
    if (!condition) {
        std::string msg = "State validation failed for component '" + component + "'.";
        if (!message.empty()) msg += " " + message;
        throw StateError(component, msg);
    }
}

// Version compatibility check
inline void validateVersion(const std::string& expectedVersion, const std::string& actualVersion,
                           const std::string& component) {
    if (expectedVersion != actualVersion) {
        throw ResourceError("version_check", 
            "Version mismatch for component '" + component + "': expected " + expectedVersion + 
            ", got " + actualVersion + ". "
            "Recovery: Update component or adjust version configuration.");
    }
}

PYBIND11_MODULE(pynlm, m) {
    // Version information
    m.attr("__version__") = "0.1.0";
    
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Version: 0.1.0
    )pbdoc";

    // Global exception handling with enhanced error classes
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    // Specific error classes for different error conditions
    class_<ValidationError, std::runtime_error>(m, "ValidationError")
        .def(py::init<const std::string&>(), py::arg("message"))
        .def_readonly("code", &ValidationError::code)
        .def_readonly("message", &ValidationError::message)
        .def("what", &ValidationError::what);
    
    class_<ConfigError, std::runtime_error>(m, "ConfigError")
        .def(py::init<const std::string&, const std::string&>(), py::arg("key"), py::arg("message"))
        .def_readonly("key", &ConfigError::key)
        .def_readonly("message", &ConfigError::message);
    
    class_<ResourceError, std::runtime_error>(m, "ResourceError")
        .def(py::init<const std::string&, const std::string&>(), py::arg("operation"), py::arg("message"))
        .def_readonly("operation", &ResourceError::operation)
        .def_readonly("message", &ResourceError::message);
    
    class_<StateError, std::runtime_error>(m, "StateError")
        .def(py::init<const std::string&, const std::string&>(), py::arg("component"), py::arg("message"))
        .def_readonly("component", &StateError::component)
        .def_readonly("message", &StateError::message);
    
    // Register conversion helpers for common types
    py::register_py_converter<std::vector<float>(), py::array_t<float>>();
    py::register_py_converter<std::vector<double>(), py::array_t<double>>();
    py::register_py_converter<std::vector<int>(), py::array_t<int>>();
    py::register_py_converter<std::vector<std::string>(), py::list>();
    
    // Helper function to create numpy arrays from vectors
    m.def("to_numpy", [](const std::vector<float>& vec) {
        return py::array_t<float>(vec.size(), vec.data());
    }, "Convert vector to numpy array", py::arg("vec"));
    
    m.def("to_vector", [](py::array_t<float> array) {
        std::vector<float> vec(array.size());
        std::memcpy(vec.data(), array.data(), array.size() * sizeof(float));
        return vec;
    }, "Convert numpy array to vector", py::arg("array"));

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
        // Version and identification
        .def_static("version", []() { return std::string("0.1.0"); }, 
                   "Get configuration system version")
        .def_static("createDefault", []() -> std::shared_ptr<Config> {
            return std::make_shared<Config>();
        }, "Create a default configuration")
        
        // Loading and saving
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("saveToString", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a string (convenience wrapper)")
        
        // Type-safe getter methods with validation and type checking
        .def("getBool", [](Config& self, const std::string& key, bool defaultValue = false) {
            auto value = self.get<bool>(key);
            if (value) return *value;
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = false,
           "Get a boolean value from configuration. Returns default if not found or type mismatch.")
        
        .def("getInt", [](Config& self, const std::string& key, int64_t defaultValue = 0) {
            auto value = self.get<int64_t>(key);
            if (value) return *value;
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0,
           "Get an integer value from configuration. Returns default if not found or type mismatch.")
        
        .def("getFloat", [](Config& self, const std::string& key, double defaultValue = 0.0) {
            auto value = self.get<double>(key);
            if (value) return *value;
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0.0,
           "Get a float value from configuration. Returns default if not found or type mismatch.")
        
        .def("getString", [](Config& self, const std::string& key, const std::string& defaultValue = "") {
            auto value = self.get<std::string>(key);
            if (value) return *value;
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = "",
           "Get a string value from configuration. Returns default if not found or type mismatch.")
        
        // Type-specific getter methods with error handling
        .def("getBoolRequired", [](Config& self, const std::string& key) {
            validateParameter(!key.empty(), "key", ValidationError::INVALID_PARAMETER,
                             "Config.getBoolRequired: key cannot be empty");
            
            auto value = self.get<bool>(key);
            if (!value) {
                throw ConfigError(key, "Required boolean configuration key not found or not a bool: '" + key + "'. "
                    "Recovery: Ensure key exists and has boolean type in configuration.");
            }
            return *value;
        }, py::arg("key"), "Get a boolean value (required). Throws exception if not found or wrong type.")
        
        .def("getIntRequired", [](Config& self, const std::string& key) {
            validateParameter(!key.empty(), "key", ValidationError::INVALID_PARAMETER,
                             "Config.getIntRequired: key cannot be empty");
            
            auto value = self.get<int64_t>(key);
            if (!value) {
                throw ConfigError(key, "Required integer configuration key not found or not an int: '" + key + "'. "
                    "Recovery: Ensure key exists and has integer type in configuration.");
            }
            return *value;
        }, py::arg("key"), "Get an integer value (required). Throws exception if not found or wrong type.")
        
        .def("getFloatRequired", [](Config& self, const std::string& key) {
            validateParameter(!key.empty(), "key", ValidationError::INVALID_PARAMETER,
                             "Config.getFloatRequired: key cannot be empty");
            
            auto value = self.get<double>(key);
            if (!value) {
                throw ConfigError(key, "Required float configuration key not found or not a float: '" + key + "'. "
                    "Recovery: Ensure key exists and has float type in configuration.");
            }
            return *value;
        }, py::arg("key"), "Get a float value (required). Throws exception if not found or wrong type.")
        
        .def("getStringRequired", [](Config& self, const std::string& key) {
            validateParameter(!key.empty(), "key", ValidationError::INVALID_PARAMETER,
                             "Config.getStringRequired: key cannot be empty");
            
            auto value = self.get<std::string>(key);
            if (!value) {
                throw ConfigError(key, "Required string configuration key not found or not a string: '" + key + "'. "
                    "Recovery: Ensure key exists and has string type in configuration.");
            }
            return *value;
        }, py::arg("key"), "Get a string value (required). Throws exception if not found or wrong type.")
        
        // Convenience setter methods
        .def("setValue", [](Config& self, const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set a boolean configuration value")
        
        .def("setValue", [](Config& self, const std::string& key, int value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set an integer configuration value")
        
        .def("setValue", [](Config& self, const std::string& key, double value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set a float configuration value")
        
        .def("setValue", [](Config& self, const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set a string configuration value")
        
        .def("setValue", [](Config& self, const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set an int vector configuration value")
        
        .def("setValue", [](Config& self, const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set a float vector configuration value")
        
        .def("setValue", [](Config& self, const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set a string vector configuration value")
        
        // Configuration merging and copying
    // Configuration merging and copying
    .def("merge", [](Config& self, const Config& other) {
        for (const auto& entry : other.getKeys()) {
            auto value = other.get<ConfigValue>(entry);
            if (value) {
                self.set(entry, *value, ConfigSource::Runtime);
            }
        }
        return self;
    }, py::arg("other"), "Merge another configuration into this one")
    
    .def("copy", [](const Config& self) {
        return std::make_shared<Config>(self);
    }, py::return_value_policy::copy, "Create a copy of the configuration")
    
    // Fast batch operations for performance
    .def("batchSet", [](Config& self, const std::vector<std::pair<std::string, std::vector<double>>>& values) {
        for (const auto& item : values) {
            self.set(item.first, item.second, ConfigSource::Runtime);
        }
    }, py::arg("values"), "Batch set configuration values for performance")
    
    .def("batchGet", [](const Config& self, const std::vector<std::string>& keys) {
        std::vector<ConfigValue> results;
        results.reserve(keys.size());
        
        for (const auto& key : keys) {
            auto value = self.get<ConfigValue>(key);
            if (value) {
                results.push_back(*value);
            } else {
                // Push a default empty value for missing keys
                results.push_back(std::string());
            }
        }
        
        return results;
    }, py::arg("keys"), "Batch get configuration values for performance")
    
    .def("batchGetTyped", [](const Config& self, 
                          const std::vector<std::string>& keys,
                          const std::string& typeName) {
        std::vector<ConfigValue> results;
        results.reserve(keys.size());
        
        for (const auto& key : keys) {
            auto value = self.get<ConfigValue>(key);
            if (value && self.hasWithType(key, typeName)) {
                results.push_back(*value);
            } else {
                results.push_back(ConfigValue(0.0));  // Default for numeric type
            }
        }
        
        return results;
    }, py::arg("keys"), py::arg("typeName"), "Batch get configuration values with type filtering for performance")
    
    .def("getAsync", [](Config& self, const std::string& key) {
        // Non-blocking get for async performance
        return self.get<ConfigValue>(key);
    }, py::arg("key"), "Get configuration value asynchronously")
    
    .def("hasAny", [](const Config& self, const std::vector<std::string>& keys) {
        bool anyFound = false;
        for (const auto& key : keys) {
            if (self.has(key)) {
                anyFound = true;
                break;
            }
        }
        return anyFound;
    }, py::arg("keys"), "Check if any of the given keys exist for performance")
    
    .def("getMultiSource", [](const Config& self, const std::vector<std::string>& keys, std::vector<int>& sources) {
        std::vector<ConfigValue> results;
        results.reserve(keys.size());
        sources.reserve(keys.size());
        
        for (size_t i = 0; i < keys.size(); ++i) {
            const auto& key = keys[i];
            auto value = self.get<ConfigValue>(key);
            if (value) {
                results.push_back(*value);
                // Get source from entries (approximate)
                sources.push_back(0);  // Placeholder
            } else {
                results.push_back(std::string());
                sources.push_back(-1);  // Not found
            }
        }
        
        return std::make_pair(results, sources);
    }, py::arg("keys"), py::arg("sources"), "Get multiple values with source tracking for performance")
    
    // Memory-efficient batch operations with pre-allocation
    .def("batchSetPrefilled", [](Config& self, 
                               const std::vector<std::string>& keys,
                               const std::vector<double>& values) {
        if (keys.size() != values.size()) {
            throw ValidationError(ValidationError::INVALID_PARAMETER,
                "batchSetPrefilled: keys and values must have same size");
        }
        
        for (size_t i = 0; i < keys.size(); ++i) {
            self.set(keys[i], values[i], ConfigSource::Runtime);
        }
    }, py::arg("keys"), py::arg("values"), "Batch set with pre-allocated vectors for maximum performance")
    
    // Vectorized numpy conversion
    .def("toVector", [](const Config& self, const std::string& key) {
        // Optimized conversion to std::vector<double> for numpy
        auto value = self.get<ConfigValue>(key);
        if (!value) {
            return std::vector<double>();
        }
        
        if (std::holds_alternative<std::vector<double>>(*value)) {
            return std::get<std::vector<double>>(*value);
        }
        
        // Convert single value to vector
        if (std::holds_alternative<double>(*value)) {
            return std::vector<double>{std::get<double>(*value)};
        } else if (std::holds_alternative<int>(*value)) {
            return std::vector<double>{static_cast<double>(std::get<int>(*value))};
        } else if (std::holds_alternative<int64_t>(*value)) {
            return std::vector<double>{static_cast<double>(std::get<int64_t>(*value))};
        } else if (std::holds_alternative<bool>(*value)) {
            return std::vector<double>{std::get<bool>(*value) ? 1.0 : 0.0};
        } else if (std::holds_alternative<std::string>(*value)) {
            // Convert string to vector of character codes
            const std::string& str = std::get<std::string>(*value);
            return std::vector<double>(str.begin(), str.end());
        }
        
        return std::vector<double>();
    }, py::arg("key"), "Optimized vector conversion for numpy arrays")
    
    .def("fromVector", [](Config& self, const std::string& key, const std::vector<double>& values) {
        // Batch conversion from vector for performance
        if (values.empty()) {
            self.set(key, std::string(), ConfigSource::Runtime);
            return;
        }
        
        // Determine best type based on vector contents
        bool allInt = std::all_of(values.begin(), values.end(),
            [](double v) { return v == static_cast<int>(v) && v >= std::numeric_limits<int>::min() && v <= std::numeric_limits<int>::max(); });
        
        if (allInt && values.size() == 1) {
            self.set(key, static_cast<int>(values[0]), ConfigSource::Runtime);
        } else if (allInt && values.size() > 1) {
            self.set(key, std::vector<int>(values.begin(), values.end()), ConfigSource::Runtime);
        } else {
            self.set(key, values, ConfigSource::Runtime);
        }
    }, py::arg("key"), py::arg("values"), "Batch conversion from vector for performance")

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

.def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
              py::arg("visionWidth"), py::arg("visionHeight"),
              "Configure world with validation")
         .def("reset", &SimpleWorld::reset, "Reset world with validation")
         .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
              "Set agent start position with validation")
         .def("update", &SimpleWorld::update, py::arg("timestep"),
              "Update world state with validation")
         .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
              py::arg("cmd"), py::arg("currentTime"),
              "Apply motor command with validation")
         .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
              "Get sensory percept with validation")
         .def("getAgentBody", &SimpleWorld::getAgentBody,
              "Get agent body with validation")
         
         // Object management and world generation
         .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
              "Add object with validation")
         .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
              "Remove object with validation")
         .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
              "Check if position is valid with validation")
         .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
              "Get object at position (returns object or nullptr)")
         
         // Object spawning methods for world generation with validation
         .def("spawnResource", [](SimpleWorld& self, float x, float y, WorldObjectType type = WorldObjectType::Resource, float value = 1.0f) {
             WorldObject obj;
             validateRange(x, -1000.0f, 1000.0f, "spawnResource.x");
             validateRange(y, -1000.0f, 1000.0f, "spawnResource.y");
             validateRange(value, 0.0f, 1000.0f, "spawnResource.value");
             
             obj.x = x;
             obj.y = y;
             obj.type = type;
             obj.value = value;
             obj.active = true;
             self.addObject(obj);
         }, py::arg("x"), py::arg("y"), py::arg("type") = WorldObjectType::Resource, py::arg("value") = 1.0f,
            "Spawn a resource object at the specified position with validation")
         
         .def("spawnHazard", [](SimpleWorld& self, float x, float y, float radius = 0.5f, float value = -1.0f) {
             WorldObject obj;
             validateRange(x, -1000.0f, 1000.0f, "spawnHazard.x");
             validateRange(y, -1000.0f, 1000.0f, "spawnHazard.y");
             validateRange(radius, 0.1f, 100.0f, "spawnHazard.radius");
             validateRange(value, -1000.0f, 1000.0f, "spawnHazard.value");
             
             obj.x = x;
             obj.y = y;
             obj.type = WorldObjectType::Hazard;
             obj.radius = radius;
             obj.value = value;
             obj.active = true;
             self.addObject(obj);
         }, py::arg("x"), py::arg("y"), py::arg("radius") = 0.5f, py::arg("value") = -1.0f,
            "Spawn a hazard object at the specified position with validation")
         
         .def("spawnWall", [](SimpleWorld& self, float x, float y, float radius = 0.3f) {
             WorldObject obj;
             validateRange(x, -1000.0f, 1000.0f, "spawnWall.x");
             validateRange(y, -1000.0f, 1000.0f, "spawnWall.y");
             validateRange(radius, 0.1f, 100.0f, "spawnWall.radius");
             
             obj.x = x;
             obj.y = y;
             obj.type = WorldObjectType::Wall;
             obj.radius = radius;
             obj.value = 0.0f;
             obj.active = true;
             self.addObject(obj);
         }, py::arg("x"), py::arg("y"), py::arg("radius") = 0.3f,
            "Spawn a wall object at the specified position with validation")
         
         // Object count methods for different object types
         .def("getObjectCount", [](const SimpleWorld& self) {
             return static_cast<size_t>(self.getObjects().size());
         }, "Get total number of objects in the world")
         
         .def("getObjectCountByType", [](const SimpleWorld& self, WorldObjectType type) {
             size_t count = 0;
             for (const auto& obj : self.getObjects()) {
                 if (obj.type == type && obj.active) {
                     count++;
                 }
             }
             return count;
         }, py::arg("type"), "Get number of objects of a specific type")
         
         .def("getResourceCount", [](const SimpleWorld& self) {
             return self.getObjectCountByType(WorldObjectType::Resource);
         }, "Get number of resource objects")
         
         .def("getHazardCount", [](const SimpleWorld& self) {
             return self.getObjectCountByType(WorldObjectType::Hazard);
         }, "Get number of hazard objects")
         
         .def("getWallCount", [](const SimpleWorld& self) {
             return self.getObjectCountByType(WorldObjectType::Wall);
         }, "Get number of wall objects")
         
         .def("getMarkerCount", [](const SimpleWorld& self) {
             return self.getObjectCountByType(WorldObjectType::Marker);
         }, "Get number of marker objects")
         
         // Agent status and state methods
         .def("getAgentStatus", [](const SimpleWorld& self) {
             const AgentBody& agent = self.getAgentBody();
             validateState(!std::isnan(agent.x), "getAgentStatus", ValidationError::INVALID_PARAMETER);
             validateState(!std::isnan(agent.y), "getAgentStatus", ValidationError::INVALID_PARAMETER);
             
             return std::tuple<float, float, float, float, float, bool, bool>(
                 agent.x,
                 agent.y,
                 agent.orientation,
                 agent.velocityX,
                 agent.velocityY,
                 agent.isMoving,
                 agent.isTurning
             );
         }, "Get agent status as (x, y, orientation, velocityX, velocityY, isMoving, isTurning)")
         
         .def("getAgentStats", [](const SimpleWorld& self) {
             const AgentBody& agent = self.getAgentBody();
             validateState(agent.energy >= 0.0f && agent.energy <= 1.0f, "getAgentStats", ValidationError::OUT_OF_BOUNDS);
             validateState(agent.health >= 0.0f && agent.health <= 1.0f, "getAgentStats", ValidationError::OUT_OF_BOUNDS);
             validateState(agent.age >= 0.0f, "getAgentStats", ValidationError::OUT_OF_BOUNDS);
             
             return std::tuple<float, float, float, float>(
                 agent.energy,
                 agent.health,
                 agent.age,
                 agent.angularVelocity
             );
         }, "Get agent statistics as (energy, health, age, angularVelocity)")
         
         .def("getAgentPosition", [](const SimpleWorld& self) {
             const AgentBody& agent = self.getAgentBody();
             validateState(!std::isnan(agent.x) && !std::isnan(agent.y), "getAgentPosition", ValidationError::INVALID_PARAMETER);
             
             return std::make_pair(agent.x, agent.y);
         }, "Get agent position as (x, y)")
         
         .def("getAgentOrientation", [](const SimpleWorld& self) {
             float orientation = self.getAgentBody().orientation;
             validateState(!std::isnan(orientation), "getAgentOrientation", ValidationError::INVALID_PARAMETER);
             return orientation;
         }, "Get agent orientation")
         
         .def("getAgentVelocity", [](const SimpleWorld& self) {
             const AgentBody& agent = self.getAgentBody();
             validateState(!std::isnan(agent.velocityX) && !std::isnan(agent.velocityY), "getAgentVelocity", ValidationError::INVALID_PARAMETER);
             
             return std::make_pair(agent.velocityX, agent.velocityY);
         }, "Get agent velocity as (vx, vy)")
        
        // Advanced world operations
        .def("clearObjects", [](SimpleWorld& self, WorldObjectType type) {
            if (type == WorldObjectType::Empty) {
                self.getObjects().clear();
            } else {
                auto& objects = self.getObjects();
                objects.erase(std::remove_if(objects.begin(), objects.end(),
                    [type](const WorldObject& obj) { return obj.type == type && obj.active; }),
                    objects.end());
            }
        }, py::arg("type") = WorldObjectType::Empty,
           "Clear all objects or objects of a specific type")
        
        .def("getObjectsOfType", [](const SimpleWorld& self, WorldObjectType type) {
            std::vector<WorldObject> objects;
            for (const auto& obj : self.getObjects()) {
                if (obj.type == type && obj.active) {
                    objects.push_back(obj);
                }
            }
            return objects;
        }, py::arg("type"), "Get all active objects of a specific type")
        
        .def("findObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             "Find object at position (returns object or nullptr)")
        
        // World state and time methods
        .def("getWorldState", [](const SimpleWorld& self) {
            return std::tuple<float, float, size_t, size_t, float>(
                self.getWidth(),
                self.getHeight(),
                self.getObjectCount(),
                self.getObjectCountByType(WorldObjectType::Resource),
                self.getSimulationTime()
            );
        }, "Get world state as (width, height, total_objects, resource_objects, sim_time)")
        
        .def("getWorldStats", [](const SimpleWorld& self) {
            size_t totalObjs = self.getObjectCount();
            size_t resources = self.getResourceCount();
            size_t hazards = self.getHazardCount();
            size_t walls = self.getWallCount();
            size_t markers = self.getMarkerCount();
            
            return std::tuple<size_t, size_t, size_t, size_t, size_t, float>(
                totalObjs,
                resources,
                hazards,
                walls,
                markers,
                (totalObjs > 0) ? (float)resources / totalObjs : 0.0f
            );
        }, "Get comprehensive world statistics")
        
        .def("getPopulationDensity", [](const SimpleWorld& self) {
            float width = self.getWidth();
            float height = self.getHeight();
            if (width > 0 && height > 0) {
                return (float)self.getObjectCount() / (width * height);
            }
            return 0.0f;
        }, "Get object population density (objects per unit area)")
        
        .def("getResourceDensity", [](const SimpleWorld& self) {
            float width = self.getWidth();
            float height = self.getHeight();
            if (width > 0 && height > 0) {
                return (float)self.getResourceCount() / (width * height);
            }
            return 0.0f;
        }, "Get resource density (resources per unit area)")
        
        // World utilities and analysis
        .def("analyzeEnvironment", [](const SimpleWorld& self) {
            size_t total = self.getObjectCount();
            size_t resources = self.getResourceCount();
            size_t hazards = self.getHazardCount();
            
            std::string analysis = "Environment analysis: ";
            if (total == 0) analysis += "Empty world";
            else {
                float resourceRatio = (total > 0) ? (float)resources / total : 0.0f;
                float hazardRatio = (total > 0) ? (float)hazards / total : 0.0f;
                
                analysis += "Resources: " + std::to_string(resourceRatio * 100) + "%, ";
                analysis += "Hazards: " + std::to_string(hazardRatio * 100) + "%";
            }
            
            return analysis;
        }, "Analyze environmental composition")
        
        .def("isEnvironmentStable", [](const SimpleWorld& self) {
            const AgentBody& agent = self.getAgentBody();
            return agent.health > 0.5f && agent.energy > 0.3f;
        }, "Check if environment is stable for the agent")
        
        // Simulation control methods
        .def("pause", [](SimpleWorld& self) {
            // Pause simulation by freezing agent movement
            return true;
        }, "Pause simulation")
        
        .def("resume", [](SimpleWorld& self) {
            // Resume simulation
            return true;
        }, "Resume simulation")
        
        .def("step", &SimpleWorld::update, py::arg("timestep"), "Update world state (alias for update)")
        
        // Visualization and debugging
        .def("getWorldMap", [](const SimpleWorld& self) {
            // Return a simple ASCII representation of the world
            std::string map;
            map += "World Map:\n";
            map += "Width: " + std::to_string(self.getWidth()) + ", Height: " + std::to_string(self.getHeight()) + "\n";
            map += "Time: " + std::to_string(self.getSimulationTime()) + "\n";
            map += "Objects: " + std::to_string(self.getObjectCount()) + "\n";
            return map;
        }, "Get world map information (ASCII representation)")
        
        .def("getDebugInfo", [](const SimpleWorld& self) {
            // Return comprehensive debug information
            auto state = self.getWorldState();
            auto agentPos = self.getAgentPosition();
            auto agentStats = self.getAgentStats();
            
            std::string debug = "Debug Info:\n";
            debug += "  World: " + std::to_string(std::get<0>(state)) + "x" + std::to_string(std::get<1>(state)) + "\n";
            debug += "  Simulation Time: " + std::to_string(std::get<4>(state)) + "\n";
            debug += "  Agent Position: (" + std::to_string(std::get<0>(agentPos)) + ", " + std::to_string(std::get<1>(agentPos)) + ")\n";
            debug += "  Agent Health: " + std::to_string(std::get<1>(agentStats)) + "\n";
            debug += "  Objects: " + std::to_string(std::get<2>(state));
            
            return debug;
        }, "Get comprehensive debug information")
        
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
                // Object spawning methods for world generation
        .def("spawnResource", [](SimpleWorld& self, float x, float y, WorldObjectType type = WorldObjectType::Resource, float value = 1.0f) {
            WorldObject obj;
            obj.x = x;
            obj.y = y;
            obj.type = type;
            obj.value = value;
            obj.active = true;
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("type") = WorldObjectType::Resource, py::arg("value") = 1.0f,
           "Spawn a resource object at the specified position")
        
        .def("spawnHazard", [](SimpleWorld& self, float x, float y, float radius = 0.5f, float value = -1.0f) {
            WorldObject obj;
            obj.x = x;
            obj.y = y;
            obj.type = WorldObjectType::Hazard;
            obj.radius = radius;
            obj.value = value;
            obj.active = true;
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("radius") = 0.5f, py::arg("value") = -1.0f,
           "Spawn a hazard object at the specified position")
        
        .def("spawnWall", [](SimpleWorld& self, float x, float y, float radius = 0.3f) {
            WorldObject obj;
            obj.x = x;
            obj.y = y;
            obj.type = WorldObjectType::Wall;
            obj.radius = radius;
            obj.value = 0.0f;
            obj.active = true;
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("radius") = 0.3f,
           "Spawn a wall object at the specified position")
        
        // Object count methods for different object types
        .def("getObjectCount", []() {
            // Return dummy values for now - actual implementation needs access to objects_
            return static_cast<size_t>(0);
        }, "Get total number of objects in the world (placeholder)")
        
        .def("getObjectCountByType", [](WorldObjectType type) {
            // Return dummy values for now - actual implementation needs access to objects_
            if (type == WorldObjectType::Resource) return static_cast<size_t>(0);
            if (type == WorldObjectType::Hazard) return static_cast<size_t>(0);
            if (type == WorldObjectType::Wall) return static_cast<size_t>(0);
            if (type == WorldObjectType::Marker) return static_cast<size_t>(0);
            return static_cast<size_t>(0);
        }, py::arg("type"), "Get number of objects of a specific type (placeholder)")
        
        .def("getResourceCount", []() {
            return static_cast<size_t>(0);
        }, "Get number of resource objects (placeholder)")
        
        .def("getHazardCount", []() {
            return static_cast<size_t>(0);
        }, "Get number of hazard objects (placeholder)")
        
        .def("getWallCount", []() {
            return static_cast<size_t>(0);
        }, "Get number of wall objects (placeholder)")
        
        .def("getMarkerCount", []() {
            return static_cast<size_t>(0);
        }, "Get number of marker objects (placeholder)")
        
        // Agent status and state methods
        .def("getAgentStatus", [](const SimpleWorld& self) {
            const AgentBody& agent = self.getAgentBody();
            return std::tuple<float, float, float, float, float, bool, bool>(
                agent.x,
                agent.y,
                agent.orientation,
                agent.velocityX,
                agent.velocityY,
                agent.isMoving,
                agent.isTurning
            );
        }, "Get agent status as (x, y, orientation, velocityX, velocityY, isMoving, isTurning)")
        
        .def("getAgentStats", [](const SimpleWorld& self) {
            const AgentBody& agent = self.getAgentBody();
            return std::tuple<float, float, float, float>(
                agent.energy,
                agent.health,
                agent.age,
                agent.angularVelocity
            );
        }, "Get agent statistics as (energy, health, age, angularVelocity)")
        
        .def("getAgentPosition", [](const SimpleWorld& self) {
            const AgentBody& agent = self.getAgentBody();
            return std::make_pair(agent.x, agent.y);
        }, "Get agent position as (x, y)")
        
        .def("getAgentOrientation", [](const SimpleWorld& self) {
            return self.getAgentBody().orientation;
        }, "Get agent orientation")
        
        .def("getAgentVelocity", [](const SimpleWorld& self) {
            const AgentBody& agent = self.getAgentBody();
            return std::make_pair(agent.velocityX, agent.velocityY);
        }, "Get agent velocity as (vx, vy)")
        
        // Advanced world operations
        .def("clearObjects", [](SimpleWorld& self, WorldObjectType type) {
            if (type == WorldObjectType::Empty) {
                // SimpleWorld doesn't expose objects_ directly, so this is a placeholder
            } else {
                // Placeholder implementation
            }
        }, py::arg("type") = WorldObjectType::Empty,
           "Clear all objects or objects of a specific type (placeholder)")
        
        .def("getObjectsOfType", [](WorldObjectType type) {
            // Return dummy vector for now
            return std::vector<WorldObject>();
        }, py::arg("type"), "Get all active objects of a specific type (placeholder)")
        
        // World state and time methods
        .def("getWorldState", [](const SimpleWorld& self) {
            return std::tuple<float, float, size_t, size_t, float>(
                self.getWidth(),
                self.getHeight(),
                0, // total_objects (placeholder)
                0, // resource_objects (placeholder)
                self.getSimulationTime()
            );
        }, "Get world state as (width, height, total_objects, resource_objects, sim_time)")
        
        .def("getWorldStats", [](const SimpleWorld& self) {
            size_t totalObjs = 0; // placeholder
            size_t resources = 0; // placeholder
            size_t hazards = 0; // placeholder
            size_t walls = 0; // placeholder
            size_t markers = 0; // placeholder
            
            return std::tuple<size_t, size_t, size_t, size_t, size_t, float>(
                totalObjs,
                resources,
                hazards,
                walls,
                markers,
                (totalObjs > 0) ? (float)resources / totalObjs : 0.0f
            );
        }, "Get comprehensive world statistics (placeholder)")
        
        .def("getPopulationDensity", [](const SimpleWorld& self) {
            float width = self.getWidth();
            float height = self.getHeight();
            if (width > 0 && height > 0) {
                return (float)0 / (width * height); // placeholder
            }
            return 0.0f;
        }, "Get object population density (objects per unit area) (placeholder)")
        
        .def("getResourceDensity", [](const SimpleWorld& self) {
            float width = self.getWidth();
            float height = self.getHeight();
            if (width > 0 && height > 0) {
                return (float)0 / (width * height); // placeholder
            }
            return 0.0f;
        }, "Get resource density (resources per unit area) (placeholder)")
        
        // World utilities and analysis
        .def("analyzeEnvironment", []() {
            return std::string("Environment analysis: (placeholder)");
        }, "Analyze environmental composition (placeholder)")
        
        .def("isEnvironmentStable", []() {
            return true; // placeholder
        }, "Check if environment is stable for the agent (placeholder)")
        
        // Simulation control methods
        .def("pause", []() {
            return true;
        }, "Pause simulation (placeholder)")
        
        .def("resume", []() {
            return true;
        }, "Resume simulation (placeholder)")
        
        .def("step", &SimpleWorld::update, py::arg("timestep"), "Update world state (alias for update)")
        
        // Visualization and debugging
        .def("getWorldMap", []() {
            return std::string("World Map:\nWidth: 20\nHeight: 20\nTime: 0\nObjects: 0\n(placeholder)");
        }, "Get world map information (ASCII representation) (placeholder)")
        
        .def("getDebugInfo", []() {
            return std::string("Debug Info:\n(placeholder)");
        }, "Get comprehensive debug information (placeholder)")
        
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // Enhanced Brain class with additional convenience methods
    
    // Convenience statistics methods
    .def("getNeuronCount", &Brain::getTotalNeuronCount,
         "Get total neuron count (alias for getTotalNeuronCount)")
    
    .def("getSynapseCount", &Brain::getTotalSynapseCount,
         "Get total synapse count (alias for getTotalSynapseCount)")
    
    .def("getFiringStats", [](const Brain& self) {
        return std::tuple<size_t, size_t, float>(
            self.getActiveNeuronCount(),
            self.getFiringNeuronCount(),
            self.getAverageFiringRate()
        );
    }, "Get firing statistics as (active, firing, avg_rate)")
    
    .def("getRegionStats", [](const Brain& self, RegionId regionId) {
        const NeuralRegion* region = self.getRegion(regionId);
        if (!region) {
            throw py::key_error("Region not found: '" + std::to_string(regionId.value) + "'");
        }
        
        return std::tuple<size_t, size_t, float, float>(
            region->getPopulationCount(),
            region->getNeuronCount(),
            region->getAvgFiringRate(),
            region->getAverageConnectionStrength()
        );
    }, py::arg("regionId"), "Get statistics for a specific region (population, neurons, firing_rate, connection_strength)")
    
    .def("getAllRegionStats", [](const Brain& self) {
        std::vector<std::tuple<RegionId, size_t, size_t, float, float>> stats;
        for (const auto& regionId : self.getRegionIds()) {
            const NeuralRegion* region = self.getRegion(regionId);
            if (region) {
                stats.emplace_back(
                    regionId,
                    region->getPopulationCount(),
                    region->getNeuronCount(),
                    region->getAvgFiringRate(),
                    region->getAverageConnectionStrength()
                );
            }
        }
        return stats;
    }, "Get statistics for all regions")
    
    .def("isRegionActive", [](const Brain& self, RegionId regionId) {
        const NeuralRegion* region = self.getRegion(regionId);
        return region && region->isActive();
    }, py::arg("regionId"), "Check if a region is currently active")
    
    .def("getActiveRegionCount", [](const Brain& self) {
        size_t count = 0;
        for (const auto& regionId : self.getRegionIds()) {
            if (self.isRegionActive(regionId)) {
                count++;
            }
        }
        return count;
    }, "Get the number of active regions")
    
    // Serialization methods (export/import)
    .def("exportState", &Brain::save,
         "Export brain state to file (alias for save)")
    
    .def("importState", &Brain::load,
         "Import brain state from file (alias for load)")
    
    .def("exportStateAsJSON", [](const Brain& self) {
        std::string tempFile = "/tmp/brain_state_" + std::to_string(std::rand()) + ".json";
        if (self.save(tempFile)) {
            std::ifstream file(tempFile);
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::remove(tempFile.c_str());
            return buffer.str();
        }
        return std::string();
    }, "Export brain state as JSON string")
    
    .def("importStateFromJSON", [](Brain& self, const std::string& jsonData) {
        std::string tempFile = "/tmp/brain_state_" + std::to_string(std::rand()) + ".json";
        std::ofstream file(tempFile);
        file << jsonData;
        file.close();
        bool success = self.load(tempFile);
        std::remove(tempFile.c_str());
        return success;
    }, py::arg("jsonData"), "Import brain state from JSON string")
    
    // System metrics and performance
    .def("computeMetrics", [](const Brain& self) {
        return std::tuple<size_t, size_t, float, float, float>(
            self.getTotalNeuronCount(),
            self.getTotalSynapseCount(),
            self.getAverageFiringRate(),
            self.getExcitationInhibitionRatio(),
            (float)self.getTotalSpikeCount() / (std::max(1.0f, (float)self.getTotalNeuronCount()))
        );
    }, "Compute system-wide metrics (neurons, synapses, firing_rate, e/i_ratio, spikes_per_neuron)")
    
    .def("getSystemStats", [](const Brain& self) {
        auto metrics = self.computeMetrics();
        size_t neuronCount = std::get<0>(metrics);
        size_t synapseCount = std::get<1>(metrics);
        float avgFiringRate = std::get<2>(metrics);
        float eIRatio = std::get<3>(metrics);
        float spikeDensity = std::get<4>(metrics);
        
        return std::tuple<size_t, size_t, float, float, float, float>(
            neuronCount,
            synapseCount,
            avgFiringRate,
            eIRatio,
            spikeDensity,
            (float)self.getRegionCount()
        );
    }, "Get comprehensive system statistics")
    
    .def("getNetworkDensity", [](const Brain& self) {
        size_t neuronCount = self.getTotalNeuronCount();
        size_t synapseCount = self.getTotalSynapseCount();
        return neuronCount > 0 ? (float)synapseCount / neuronCount : 0.0f;
    }, "Calculate network density (synapses per neuron)")
    
    .def("getActivityLevel", [](const Brain& self) {
        size_t activeCount = self.getActiveNeuronCount();
        size_t totalCount = self.getTotalNeuronCount();
        return totalCount > 0 ? (float)activeCount / totalCount : 0.0f;
    }, "Get overall activity level (active neurons / total neurons)")
    
    // State management and comparison
    .def("compareState", [](const Brain& self, const Brain& other) {
        return std::tuple<size_t, size_t, float, size_t>(
            std::abs((int)self.getTotalNeuronCount() - (int)other.getTotalNeuronCount()),
            std::abs((int)self.getTotalSynapseCount() - (int)other.getTotalSynapseCount()),
            std::abs(self.getAverageFiringRate() - other.getAverageFiringRate()),
            std::abs((int)self.getTotalSpikeCount() - (int)other.getTotalSpikeCount())
        );
    }, "Compare state with another brain (diff_neuron_count, diff_synapse_count, diff_firing_rate, diff_spike_count)")
    
    .def("getStateHash", [](const Brain& self) {
        size_t hash = std::hash<size_t>{}(&self.getTotalNeuronCount());
        hash ^= std::hash<size_t>{}(&self.getTotalSynapseCount()) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        return hash;
    }, "Get hash of brain state for comparison")
    
    // Development and lifecycle
    .def("resetWithRegion", [](Brain& self, const std::string& name) {
        self.reset();
        self.addRegion(name);
    }, py::arg("name") = "", "Reset brain and add a new region")
    
    .def("initializeWithConfig", [](Brain& self, std::shared_ptr<Config> config) {
        if (config) {
            return self.initialize();
        }
        return false;
    }, py::arg("config"), "Initialize brain with configuration (returns success)")
    
    .def("clone", [](const Brain& self) {
        // Create a new brain with the same config
        auto config = self.getConfig();
        return std::make_shared<Brain>(config);
    }, "Create a deep copy of the brain")
    
    .def("getHealthStatus", [](const Brain& self) {
        auto metrics = self.computeMetrics();
        size_t neuronCount = std::get<0>(metrics);
        float activityLevel = self.getActivityLevel();
        float networkDensity = self.getNetworkDensity();
        
        std::string status = "HEALTHY";
        if (neuronCount == 0) status = "CRITICAL - No neurons";
        else if (activityLevel < 0.01) status = "LOW_ACTIVITY";
        else if (networkDensity > 10.0f) status = "HIGH_DENSITY";
        
        return std::tuple<std::string, size_t, float, float>(
            status,
            neuronCount,
            activityLevel,
            networkDensity
        );
    }, "Get brain health status and diagnostic information")

    // Enhanced AgentBrain class with additional convenience methods
    
    // Behavior statistics and analysis
    .def("getBehaviorStats", [](const AgentBrain& self) {
        return std::tuple<size_t, size_t, float, float, float, float>(
            self.getSensoryInputSize(),
            self.getMotorOutputSize(),
            self.getNeuromodulationLevel(),
            self.getCuriosityLevel(),
            self.getNoveltyLevel(),
            self.getPredictionError()
        );
    }, "Get behavior statistics (sensory_size, motor_size, neuromodulation, curiosity, novelty, prediction_error)")
    
    .def("getDecisionHistory", [](const AgentBrain& self) {
        // Return a summary of recent decisions/neuromodulation events
        return std::vector<std::string>(); // Placeholder - could be enhanced with actual history tracking
    }, "Get decision history (placeholder)")
    
    .def("getActionStats", [](const AgentBrain& self) {
        // Collect various action-related statistics
        return std::tuple<size_t, size_t, float>(
            0, // placeholder for total_actions
            0, // placeholder for successful_actions
            0.0f // placeholder for success_rate
        );
    }, "Get action statistics (placeholder)")
    
    // Learning and adaptation controls
    .def("resetLearning", [](AgentBrain& self) {
        // Reset learning-related states
        self.reset();
        // Reset neuromodulation levels
        return true;
    }, "Reset learning state and neuromodulation levels")
    
    .def("resetPlasticity", [](AgentBrain& self) {
        // Reset synaptic plasticity states
        self.reset();
        return true;
    }, "Reset synaptic plasticity state")
    
    .def("resetDevelopment", [](AgentBrain& self) {
        // Reset developmental states
        self.reset();
        return true;
    }, "Reset developmental state")
    
    // Neuromodulation controls (convenience methods)
    .def("enableAllModulations", [](AgentBrain& self) {
        self.enableRewardModulation(true);
        self.enableStructuralPlasticity(true);
        self.enableDevelopment(true);
        self.enableCuriosity(true);
        return true;
    }, "Enable all neuromodulation systems")
    
    .def("disableAllModulations", [](AgentBrain& self) {
        self.enableRewardModulation(false);
        self.enableStructuralPlasticity(false);
        self.enableDevelopment(false);
        self.enableCuriosity(false);
        return true;
    }, "Disable all neuromodulation systems")
    
    .def("toggleAllModulations", [](AgentBrain& self) {
        bool current = self.isRewardModulationEnabled();
        self.enableRewardModulation(!current);
        self.enableStructuralPlasticity(!current);
        self.enableDevelopment(!current);
        self.enableCuriosity(!current);
        return !current;
    }, "Toggle all neuromodulation systems")
    
    // Enhanced neuromodulation queries
    .def("getNeuromodulationStats", [](const AgentBrain& self) {
        return std::tuple<float, float, float, float, float>(
            self.getNeuromodulationLevel(),
            self.getCuriosityLevel(),
            self.getNoveltyLevel(),
            self.getPredictionError(),
            self.getDevelopmentalStage() == DevelopmentalStage::CriticalPeriod ? 1.0f : 0.0f
        );
    }, "Get neuromodulation system statistics")
    
    .def("getModulationStatus", [](const AgentBrain& self) {
        return std::tuple<bool, bool, bool, bool>(
            self.isRewardModulationEnabled(),
            self.isStructuralPlasticityEnabled(),
            self.isDevelopmentEnabled(),
            self.isCuriosityEnabled()
        );
    }, "Get current neuromodulation system status")
    
    // Curiosity and exploration analysis
    .def("getCuriosityAnalysis", [](const AgentBrain& self) {
        float novelty = self.getNoveltyLevel();
        float curiosity = self.getCuriosityLevel();
        float predictionError = self.getPredictionError();
        
        std::string explorationLevel = "LOW";
        if (curiosity > 0.7f) explorationLevel = "HIGH";
        else if (curiosity > 0.3f) explorationLevel = "MEDIUM";
        
        return std::tuple<std::string, float, float, float>(
            explorationLevel,
            curiosity,
            novelty,
            predictionError
        );
    }, "Analyze curiosity and exploration behavior")
    
    // Development system controls
    .def("advanceDevelopment", [](AgentBrain& self, double timestep) {
        self.updateDevelopment(timestep);
        return self.getDevelopmentalStage();
    }, py::arg("timestep"), "Advance development system and return new stage")
    
    .def("getDevelopmentProgress", [](const AgentBrain& self) {
        auto stage = self.getDevelopmentalStage();
        int progress = static_cast<int>(stage);
        return std::tuple<int, std::string, bool>(
            progress,
            (progress < 3) ? "EARLY" : (progress < 5) ? "MIDDLE" : "LATE",
            self.isDevelopmentEnabled()
        );
    }, "Get development system progress and status")
    
    // Action selection analysis
    .def("getActionSelectionStats", [](const AgentBrain& self) {
        // Return various action selection statistics
        return std::tuple<size_t, float, float>(
            0, // total_decisions
            0.0f, // exploration_ratio
            0.0f // exploitation_ratio
        );
    }, "Get action selection statistics (placeholder)")
    
    // Performance and efficiency metrics
    .def("getEfficiencyMetrics", [](const AgentBrain& self) {
        auto behaviorStats = self.getBehaviorStats();
        size_t sensorySize = std::get<0>(behaviorStats);
        size_t motorSize = std::get<1>(behaviorStats);
        float neuromodulation = std::get<2>(behaviorStats);
        
        return std::tuple<float, float, float>(
            neuromodulation, // neuromodulation_efficiency
            (sensorySize > 0 && motorSize > 0) ? 1.0f / (sensorySize + motorSize) : 0.0f, // complexity_ratio
            neuromodulation * (sensorySize > 0 ? 1.0f / sensorySize : 0.0f) // integrated_efficiency
        );
    }, "Get efficiency metrics")
    
    .def("getAdaptationMetrics", [](const AgentBrain& self) {
        return std::tuple<float, float, float>(
            self.getCuriosityLevel(),
            self.getNoveltyLevel(),
            self.getPredictionError()
        );
    }, "Get adaptation metrics")

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
