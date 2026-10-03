#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

namespace py = pybind11;
namespace nlm {

// Python exception class for NLM errors
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
    explicit NLMError(const char* msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError(msg) {}
    explicit ConfigurationError(const char* msg) : NLMError(msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError(msg) {}
    explicit SimulationError(const char* msg) : NLMError(msg) {}
};

class InitializationError : public NLMError {
public:
    explicit InitializationError(const std::string& msg) : NLMError(msg) {}
    explicit InitializationError(const char* msg) : NLMError(msg) {}
};

class NLMValidationError : public NLMError {
public:
    explicit NLMValidationError(const std::string& msg) : NLMError(msg) {}
    explicit NLMValidationError(const char* msg) : NLMError(msg) {}
};

class NLMConversionError : public NLMError {
public:
    explicit NLMConversionError(const std::string& msg) : NLMError(msg) {}
    explicit NLMConversionError(const char* msg) : NLMError(msg) {}
};

class NLMPerformanceError : public NLMError {
public:
    explicit NLMPerformanceError(const std::string& msg) : NLMError(msg) {}
    explicit NLMPerformanceError(const char* msg) : NLMError(msg) {}
};

// Helper function to create a Config from a Python dictionary
inline std::shared_ptr<Config> createConfigFromDict(const py::dict& d) {
    auto config = std::make_shared<Config>();
    for (auto item : d) {
        std::string key = py::cast<std::string>(item.first);
        py::object value_obj = item.second;
        
        if (value_obj.is_none()) {
            config->set(key, ConfigValue(std::string("")), ConfigSource::Runtime);
        } else if (value_obj.is(py::bool_())) {
            config->set(key, py::cast<bool>(value_obj), ConfigSource::Runtime);
        } else if (value_obj.is(py::int_())) {
            config->set(key, py::cast<int>(value_obj), ConfigSource::Runtime);
        } else if (value_obj.is(py::float_())) {
            config->set(key, py::cast<double>(value_obj), ConfigSource::Runtime);
        } else if (value_obj.is(py::str())) {
            config->set(key, py::cast<std::string>(value_obj), ConfigSource::Runtime);
        } else if (value_obj.is(py::list())) {
            py::list lst = value_obj.cast<py::list>();
            bool all_int = true;
            bool all_float = true;
            bool all_str = true;
            
            for (auto elem : lst) {
                if (!elem.is(py::int_())) all_int = false;
                if (!elem.is(py::float_())) all_float = false;
                if (!elem.is(py::str())) all_str = false;
            }
            
            if (all_int) {
                std::vector<int> int_list;
                for (auto elem : lst) {
                    int_list.push_back(py::cast<int>(elem));
                }
                config->set(key, ConfigValue(int_list), ConfigSource::Runtime);
            } else if (all_float) {
                std::vector<double> float_list;
                for (auto elem : lst) {
                    float_list.push_back(py::cast<double>(elem));
                }
                config->set(key, ConfigValue(float_list), ConfigSource::Runtime);
            } else if (all_str) {
                std::vector<std::string> str_list;
                for (auto elem : lst) {
                    str_list.push_back(py::cast<std::string>(elem));
                }
                config->set(key, ConfigValue(str_list), ConfigSource::Runtime);
            } else {
                throw ConfigurationError("Unsupported list element type for key: " + key);
            }
        } else {
            throw ConfigurationError("Unsupported value type for key: " + key);
        }
    }
    return config;
}

// Advanced type conversion utilities
namespace TypeConversion {
    // Safe type conversion from Python object to C++
    template<typename T>
    T pyToCpp(py::object obj) {
        try {
            if (std::is_same<T, std::string>::value) {
                return obj.cast<std::string>();
            } else if (std::is_same<T, int>::value) {
                return obj.cast<int>();
            } else if (std::is_same<T, double>::value) {
                return obj.cast<double>();
            } else if (std::is_same<T, bool>::value) {
                return obj.cast<bool>();
            } else if (std::is_same<T, std::vector<int>>::value) {
                auto lst = obj.cast<py::list>();
                std::vector<int> result;
                result.reserve(lst.size());
                for (auto item : lst) {
                    result.push_back(pyToCpp<int>(item));
                }
                return result;
            } else if (std::is_same<T, std::vector<double>>::value) {
                auto lst = obj.cast<py::list>();
                std::vector<double> result;
                result.reserve(lst.size());
                for (auto item : lst) {
                    result.push_back(pyToCpp<double>(item));
                }
                return result;
            } else if (std::is_same<T, std::vector<std::string>>::value) {
                auto lst = obj.cast<py::list>();
                std::vector<std::string> result;
                result.reserve(lst.size());
                for (auto item : lst) {
                    result.push_back(pyToCpp<std::string>(item));
                }
                return result;
            } else if (std::is_same<T, std::vector<bool>>::value) {
                auto lst = obj.cast<py::list>();
                std::vector<bool> result;
                result.reserve(lst.size());
                for (auto item : lst) {
                    result.push_back(pyToCpp<bool>(item));
                }
                return result;
            } else {
                throw NLMConversionError("Unsupported type conversion from Python to C++");
            }
        } catch (const py::cast_error& e) {
            throw NLMConversionError("Type conversion error: " + std::string(e.what()));
        }
    }
    
    // Convert C++ types to Python objects
    template<typename T>
    py::object cppToPy(const T& value) {
        if (std::is_same<T, std::string>::value) {
            return py::cast(std::any_cast<std::string>(value));
        } else if (std::is_same<T, int>::value) {
            return py::cast(std::any_cast<int>(value));
        } else if (std::is_same<T, double>::value) {
            return py::cast(std::any_cast<double>(value));
        } else if (std::is_same<T, bool>::value) {
            return py::cast(std::any_cast<bool>(value));
        } else if (std::is_same<T, std::vector<int>>::value) {
            return py::cast(std::any_cast<std::vector<int>>(value));
        } else if (std::is_same<T, std::vector<double>>::value) {
            return py::cast(std::any_cast<std::vector<double>>(value));
        } else if (std::is_same<T, std::vector<std::string>>::value) {
            return py::cast(std::any_cast<std::vector<std::string>>(value));
        } else if (std::is_same<T, std::vector<bool>>::value) {
            return py::cast(std::any_cast<std::vector<bool>>(value));
        } else {
            throw NLMConversionError("Unsupported type conversion from C++ to Python");
        }
    }
    
    // Validate type compatibility
    bool validateTypes(const py::object& obj, const std::type_info& expectedType) {
        try {
            if (expectedType == typeid(std::string)) {
                return obj.is(py::str());
            } else if (expectedType == typeid(int)) {
                return obj.is(py::int_());
            } else if (expectedType == typeid(double)) {
                return obj.is(py::float_());
            } else if (expectedType == typeid(bool)) {
                return obj.is(py::bool_());
            } else if (expectedType == typeid(std::vector<int>)) {
                if (obj.is(py::list())) {
                    py::list lst = obj.cast<py::list>();
                    for (auto item : lst) {
                        if (!item.is(py::int_())) return false;
                    }
                    return true;
                }
                return false;
            } else if (expectedType == typeid(std::vector<double>)) {
                if (obj.is(py::list())) {
                    py::list lst = obj.cast<py::list>();
                    for (auto item : lst) {
                        if (!item.is(py::float_())) return false;
                    }
                    return true;
                }
                return false;
            } else if (expectedType == typeid(std::vector<std::string>)) {
                if (obj.is(py::list())) {
                    py::list lst = obj.cast<py::list>();
                    for (auto item : lst) {
                        if (!item.is(py::str())) return false;
                    }
                    return true;
                }
                return false;
            } else {
                return false;
            }
        } catch (...) {
            return false;
        }
    }
}

// Cached lambda functions for frequently used operations
inline std::string getCachedRepr(const std::string& prefix, const std::string& content) {
    return prefix + content;
}

inline double clampValue(double value, double minVal, double maxVal) {
    return std::max(minVal, std::min(maxVal, value));
}

inline std::string generateCacheKey(const std::string& str) {
    return "repr_" + str;
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = "NLM (Neural Learning Machine) Python Bindings - "
              "A Python binding for the NLM C++ neural simulation framework. "
              "Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action. "
              "Key features: exception-safe error handling, type-safe ID classes, convenience methods, "
              "Pythonic interfaces with clear documentation, optimized string operations.";

    // Register custom exception classes
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError);
    py::register_exception<ConfigurationError>(m, "ConfigurationError", PyExc_RuntimeError);
    py::register_exception<SimulationError>(m, "SimulationError", PyExc_RuntimeError);
    py::register_exception<InitializationError>(m, "InitializationError", PyExc_RuntimeError);
    py::register_exception<NLMValidationError>(m, "NLMValidationError", PyExc_ValueError);
    py::register_exception<NLMConversionError>(m, "NLMConversionError", PyExc_TypeError);
    py::register_exception<NLMPerformanceError>(m, "NLMPerformanceError", PyExc_RuntimeError);

    // Register all ID types using a generic templated function
    // This reduces code duplication and ensures consistency
    registerIdType<NeuronId>(m, "NeuronId", "Unique identifier for a neuron");
    registerIdType<SynapseId>(m, "SynapseId", "Unique identifier for a synapse");
    registerIdType<RegionId>(m, "RegionId", "Unique identifier for a brain region");
    registerIdType<PopulationId>(m, "PopulationId", "Unique identifier for a neuron population");

    // Register enumeration types
    py::enum_<NeuronType>(m, "NeuronType", "Enumeration of neuron types")
        .value("Excitatory", NeuronType::Excitatory, "Excitatory neuron type")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neuron type")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neuron type")
        .value("Sensory", NeuronType::Sensory, "Sensory neuron type")
        .value("Motor", NeuronType::Motor, "Motor neuron type")
        .value("Internal", NeuronType::Internal, "Internal neuron type")
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", "Enumeration of synapse types")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory synapse")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synapse")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synapse")
        .value("Electrical", SynapseType::Electrical, "Electrical synapse")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction synapse")
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", "Enumeration of developmental stages")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period stage")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation stage")
        .value("Adult", DevelopmentalStage::Adult, "Adult stage")
        .value("Aging", DevelopmentalStage::Aging, "Aging stage")
        .export_values();

    py::enum_<FiringState>(m, "FiringState", "Enumeration of neuron firing states")
        .value("Resting", FiringState::Resting, "Resting state")
        .value("Active", FiringState::Active, "Active firing state")
        .value("Refractory", FiringState::Refractory, "Refractory period")
        .value("Inhibited", FiringState::Inhibited, "Inhibited state")
        .export_values();

    py::enum_<ActionType>(m, "ActionType", "Enumeration of action types")
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
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", "Enumeration of motor commands")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward command")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward command")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left command")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right command")
        .value("LookLeft", MotorCommand::LookLeft, "Look left command")
        .value("LookRight", MotorCommand::LookRight, "Look right command")
        .value("Interact", MotorCommand::Interact, "Interact command")
        .value("Wait", MotorCommand::Wait, "Wait command")
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", "Enumeration of world object types")
        .value("Empty", WorldObjectType::Empty, "Empty world object")
        .value("Resource", WorldObjectType::Resource, "Resource object")
        .value("Hazard", WorldObjectType::Hazard, "Hazard object")
        .value("Wall", WorldObjectType::Wall, "Wall object")
        .value("Marker", WorldObjectType::Marker, "Marker object")
        .export_values();

    // Config class with enhanced Python API
    py::class_<Config>(m, "Config", 
        "Configuration class for NLM system. Manages neural simulation parameters and settings. "
        "Provides type-safe get/set operations with fallback values. "
        "Supports loading from files, command line arguments, and Python dictionaries."
    )
        .def(py::init<>())
        .def("loadFromFile", 
            [](Config& self, const std::string& filepath) {
                try {
                    return self.loadFromFile(filepath);
                } catch (const std::exception& e) {
                    throw ConfigurationError("Failed to load config from file '" + filepath + "': " + e.what());
                }
            },
            py::arg("filepath"), 
            "Load configuration from a file (JSON format).\n\n"
            "Args:\n"
            "    filepath: Path to the configuration file\n\n"
            "Returns:\n"
            "    True if successful, False otherwise\n\n"
            "Raises:\n"
            "    ConfigurationError: If the file cannot be read or parsed"
        )
        .def("loadFromArgs",
            [](Config& self, int argc, char** argv) {
                try {
                    return self.loadFromArgs(argc, argv);
                } catch (const std::exception& e) {
                    throw ConfigurationError("Failed to load config from arguments: " + e.what());
                }
            },
            py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments.\n\n"
            "Args:\n"
            "    argc: Argument count\n"
            "    argv: Argument vector (typically from main())\n\n"
            "Returns:\n"
            "    True if successful, False otherwise\n\n"
            "Raises:\n"
            "    ConfigurationError: If argument parsing fails"
        )
        .def("saveToFile",
            [](const Config& self, const std::string& filepath) {
                try {
                    return self.saveToFile(filepath);
                } catch (const std::exception& e) {
                    throw ConfigurationError("Failed to save config to file '" + filepath + "': " + e.what());
                }
            },
            py::arg("filepath"),
            "Save configuration to a file.\n\n"
            "Args:\n"
            "    filepath: Path to save the configuration file\n\n"
            "Returns:\n"
            "    True if successful, False otherwise\n\n"
            "Raises:\n"
            "    ConfigurationError: If the file cannot be written"
        )
        .def("has",
            [](const Config& self, const std::string& key) {
                try {
                    return self.has(key);
                } catch (const std::exception& e) {
                    throw ConfigurationError("Error checking config key '" + key + "': " + e.what());
                }
            },
            py::arg("key"),
            "Check if a configuration key exists.\n\n"
            "Args:\n"
            "    key: Configuration key to check\n\n"
            "Returns:\n"
            "    True if the key exists, False otherwise\n\n"
            "Raises:\n"
            "    ConfigurationError: If there is an internal error"
        )
        .def("getKeys",
            [](const Config& self) {
                try {
                    return self.getKeys();
                } catch (const std::exception& e) {
                    throw ConfigurationError("Error getting config keys: " + e.what());
                }
            },
            "Get all configuration keys.\n\n"
            "Returns:\n"
            "    List of all configuration keys\n\n"
            "Raises:\n"
            "    ConfigurationError: If there is an internal error"
        )
        .def("clear",
            [](Config& self) {
                try {
                    self.clear();
                } catch (const std::exception& e) {
                    throw ConfigurationError("Error clearing config: " + e.what());
                }
            },
            "Clear all configuration entries.\n\n"
            "Raises:\n"
            "    ConfigurationError: If there is an internal error"
        )
        .def("summary",
            [](const Config& self) {
                try {
                    return self.summary();
                } catch (const std::exception& e) {
                    throw ConfigurationError("Error generating config summary: " + e.what());
                }
            },
            "Get a human-readable summary of the configuration.\n\n"
            "Returns:\n"
            "    String containing configuration summary\n\n"
            "Raises:\n"
            "    ConfigurationError: If there is an internal error"
        )
        // Convenience methods for common types
        .def("getOrInt", 
            [](const Config& self, const std::string& key, int defaultValue) {
                return getOrInt(self, key, defaultValue);
            },
            py::arg("key"), py::arg("defaultValue") = 0,
            "Get an integer value or return default if not present.\n\n"
            "Args:\n"
            "    key: Configuration key\n"
            "    defaultValue: Default value if key not found (default: 0)\n\n"
            "Returns:\n"
            "    Integer value or default\n\n"
            "Raises:\n"
            "    ConfigurationError: If key exists but has wrong type"
        )
        .def("getOrFloat",
            [](const Config& self, const std::string& key, double defaultValue) {
                return getOrFloat(self, key, defaultValue);
            },
            py::arg("key"), py::arg("defaultValue") = 0.0,
            "Get a float value or return default if not present.\n\n"
            "Args:\n"
            "    key: Configuration key\n"
            "    defaultValue: Default value if key not found (default: 0.0)\n\n"
            "Returns:\n"
            "    Float value or default\n\n"
            "Raises:\n"
            "    ConfigurationError: If key exists but has wrong type"
        )
        .def("getOrString",
            [](const Config& self, const std::string& key, const std::string& defaultValue) {
                return getOrString(self, key, defaultValue);
            },
            py::arg("key"), py::arg("defaultValue") = "",
            "Get a string value or return default if not present.\n\n"
            "Args:\n"
            "    key: Configuration key\n"
            "    defaultValue: Default value if key not found (default: \"\")\n\n"
            "Returns:\n"
            "    String value or default\n\n"
            "Raises:\n"
            "    ConfigurationError: If key exists but has wrong type"
        )
        .def("getOrBool",
            [](const Config& self, const std::string& key, bool defaultValue) {
                return getOrBool(self, key, defaultValue);
            },
            py::arg("key"), py::arg("defaultValue") = false,
            "Get a boolean value or return default if not present.\n\n"
            "Args:\n"
            "    key: Configuration key\n"
            "    defaultValue: Default value if key not found (default: False)\n\n"
            "Returns:\n"
            "    Boolean value or default\n\n"
            "Raises:\n"
            "    ConfigurationError: If key exists but has wrong type"
        )
        // Type-safe get methods
        .def("getInt",
            [](const Config& self, const std::string& key) {
                auto val = self.get<int>(key);
                if (!val) throw ConfigurationError("Key '" + key + "' not found or wrong type");
                return *val;
            },
            py::arg("key"),
            "Get an integer value.\n\n"
            "Args:\n"
            "    key: Configuration key\n\n"
            "Returns:\n"
            "    Integer value\n\n"
            "Raises:\n"
            "    ConfigurationError: If key not found or wrong type"
        )
        .def("getFloat",
            [](const Config& self, const std::string& key) {
                auto val = self.get<double>(key);
                if (!val) throw ConfigurationError("Key '" + key + "' not found or wrong type");
                return *val;
            },
            py::arg("key"),
            "Get a float value.\n\n"
            "Args:\n"
            "    key: Configuration key\n\n"
            "Returns:\n"
            "    Float value\n\n"
            "Raises:\n"
            "    ConfigurationError: If key not found or wrong type"
        )
        .def("getString",
            [](const Config& self, const std::string& key) {
                auto val = self.get<std::string>(key);
                if (!val) throw ConfigurationError("Key '" + key + "' not found or wrong type");
                return *val;
            },
            py::arg("key"),
            "Get a string value.\n\n"
            "Args:\n"
            "    key: Configuration key\n\n"
            "Returns:\n"
            "    String value\n\n"
            "Raises:\n"
            "    ConfigurationError: If key not found or wrong type"
        )
        .def("getBool",
            [](const Config& self, const std::string& key) {
                auto val = self.get<bool>(key);
                if (!val) throw ConfigurationError("Key '" + key + "' not found or wrong type");
                return *val;
            },
            py::arg("key"),
            "Get a boolean value.\n\n"
            "Args:\n"
            "    key: Configuration key\n\n"
            "Returns:\n"
            "    Boolean value\n\n"
            "Raises:\n"
            "    ConfigurationError: If key not found or wrong type"
        )
        // Matrix/set methods
        .def("getInts",
            [](const Config& self, const std::string& key) {
                auto val = self.get<std::vector<int>>(key);
                if (!val) throw ConfigurationError("Key '" + key + "' not found or wrong type");
                return *val;
            },
            py::arg("key"),
            "Get an integer array.\n\n"
            "Args:\n"
            "    key: Configuration key\n\n"
            "Returns:\n"
            "    Integer array\n\n"
            "Raises:\n"
            "    ConfigurationError: If key not found or wrong type"
        )
        .def("getFloats",
            [](const Config& self, const std::string& key) {
                auto val = self.get<std::vector<double>>(key);
                if (!val) throw ConfigurationError("Key '" + key + "' not found or wrong type");
                return *val;
            },
            py::arg("key"),
            "Get a float array.\n\n"
            "Args:\n"
            "    key: Configuration key\n\n"
            "Returns:\n"
            "    Float array\n\n"
            "Raises:\n"
            "    ConfigurationError: If key not found or wrong type"
        )
        .def("getStrings",
            [](const Config& self, const std::string& key) {
                auto val = self.get<std::vector<std::string>>(key);
                if (!val) throw ConfigurationError("Key '" + key + "' not found or wrong type");
                return *val;
            },
            py::arg("key"),
            "Get a string array.\n\n"
            "Args:\n"
            "    key: Configuration key\n\n"
            "Returns:\n"
            "    String array\n\n"
            "Raises:\n"
            "    ConfigurationError: If key not found or wrong type"
        )
        // Performance optimizations
        .def("__repr__", 
            [](const Config& cfg) {
                return getCachedRepr("<Config: ", cfg.summary() + ">");
            }
        )
        // Advanced Config methods
        .def("hasKey", 
            [](const Config& self, const std::string& key) {
                return self.has(key);
            },
            py::arg("key"),
            "Check if a configuration key exists.\n\n"
            "Args:\n"
            "    key: Configuration key to check\n\n"
            "Returns:\n"
            "    True if the key exists, False otherwise\n\n"
            "Raises:\n"
            "    ConfigurationError: If there is an internal error"
        )
        .def("getFloatWithFallback",
            [](const Config& self, const std::string& key, double defaultValue, double minVal, double maxVal) {
                auto val = self.getOr<double>(key, defaultValue);
                return clampValue(val, minVal, maxVal);
            },
            py::arg("key"), py::arg("defaultValue") = 0.0, py::arg("minVal") = std::numeric_limits<double>::lowest(), py::arg("maxVal") = std::numeric_limits<double>::max(),
            "Get a float value with bounds checking.\n\n"
            "Args:\n"
            "    key: Configuration key\n"
            "    defaultValue: Default value if key not found (default: 0.0)\n"
            "    minVal: Minimum allowed value (default: negative infinity)\n"
            "    maxVal: Maximum allowed value (default: positive infinity)\n\n"
            "Returns:\n"
            "    Clamped float value\n\n"
            "Raises:\n"
            "    ConfigurationError: If key exists but has wrong type"
        )
        .def("toDict",
            [](const Config& self) {
                py::dict d;
                auto keys = self.getKeys();
                for (const auto& key : keys) {
                    if (self.has(key)) {
                        auto val = self.get<std::any>(key);
                        if (val.has_value()) {
                            if (val.type() == typeid(int)) {
                                d[key.c_str()] = std::any_cast<int>(val);
                            } else if (val.type() == typeid(double)) {
                                d[key.c_str()] = std::any_cast<double>(val);
                            } else if (val.type() == typeid(bool)) {
                                d[key.c_str()] = std::any_cast<bool>(val);
                            } else if (val.type() == typeid(std::string)) {
                                d[key.c_str()] = std::any_cast<std::string>(val);
                            } else if (val.type() == typeid(std::vector<int>)) {
                                d[key.c_str()] = py::cast(std::any_cast<std::vector<int>>(val));
                            } else if (val.type() == typeid(std::vector<double>)) {
                                d[key.c_str()] = py::cast(std::any_cast<std::vector<double>>(val));
                            } else if (val.type() == typeid(std::vector<std::string>)) {
                                d[key.c_str()] = py::cast(std::any_cast<std::vector<std::string>>(val));
                            }
                        }
                    }
                }
                return d;
            },
            "Convert configuration to Python dictionary.\n\n"
            "Returns:\n"
            "    Python dictionary with all configuration values\n\n"
            "Raises:\n"
            "    ConfigurationError: If there is an internal error"
        )
        .def_static("fromDict",
            [](py::dict d) {
                return createConfigFromDict(d);
            },
            py::arg("d"),
            "Create a configuration from a Python dictionary.\n\n"
            "Args:\n"
            "    d: Python dictionary with configuration values\n\n"
            "Returns:\n"
            "    Config object populated with dictionary values\n\n"
            "Raises:\n"
            "    ConfigurationError: If dictionary contains unsupported types"
        )
        .def("validateConfig",
            [](const Config& self) {
                py::dict validation;
                validation["is_valid"] = true;
                validation["warnings"] = py::list();
                validation["recommendations"] = py::list();
                
                if (!self.has("brain.neuron_count")) {
                    validation["is_valid"] = false;
                    validation["warnings"].attr("append")("Missing brain.neuron_count");
                    validation["recommendations"].attr("append")("Set neuron count (recommendation: 100-1000)");
                }
                
                if (!self.has("simulation.timestep")) {
                    validation["is_valid"] = false;
                    validation["warnings"].attr("append")("Missing simulation.timestep");
                    validation["recommendations"].attr("append")("Set timestep (recommendation: 0.01-0.1)");
                }
                
                try {
                    auto neuron_count = self.getOr<int>("brain.neuron_count", 0);
                    if (neuron_count < 10 || neuron_count > 10000) {
                        validation["warnings"].attr("append")("Neuron count out of recommended range (10-10000)");
                    }
                } catch (...) {}
                
                try {
                    auto timestep = self.getOr<double>("simulation.timestep", 0.0);
                    if (timestep <= 0.0 || timestep > 1.0) {
                        validation["warnings"].attr("append")("Timestep should be positive and <= 1.0");
                    }
                } catch (...) {}
                
                return validation;
            },
            "Validate configuration.\n\n"
            "Returns:\n"
            "    Dictionary with validation results including is_valid, warnings, and recommendations\n\n"
            "Raises:\n"
            "    ConfigurationError: If there is an internal error"
        )
        // Pythonic interface enhancements
        // Context manager support
        .def("__enter__", [](Config& self) { return &self; })
        .def("__exit__", [](Config& self, py::object, py::object, py::object) {
            self.clear();
        })
        // Iteration support for Config keys
        .def("__iter__", [](const Config& self) {
            auto keys = self.getKeys();
            return py::make_iterator(keys.begin(), keys.end());
        }, py::keep_alive<0, 1>())
        // Map/dict comprehensions compatibility
        .def("items", 
            [](const Config& self) {
                py::dict d;
                auto keys = self.getKeys();
                for (const auto& key : keys) {
                    if (self.has(key)) {
                        auto val = self.get<std::any>(key);
                        if (val.has_value()) {
                            d[key.c_str()] = py::cast(val);
                        }
                    }
                }
                return d;
            },
            "Get configuration as a dictionary of key-value pairs.\n\n"
            "Returns:\n"
            "    Dictionary with all configuration keys and values"
        )

    // SensoryInput and subclasses
    py::class_<SensoryInput>(m, "SensoryInput", "Base class for sensory input")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"), "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", "Vision sensory input")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", "Audio sensory input")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", "Internal signals sensory input")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // Action class
    py::class_<Action>(m, "Action", "Action representation for motor output")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    // WorldObject class
    py::class_<WorldObject>(m, "WorldObject", "World object representation")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x, "X coordinate")
        .def_readwrite("y", &WorldObject::y, "Y coordinate")
        .def_readwrite("radius", &WorldObject::radius, "Radius")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Object value")
        .def_readwrite("active", &WorldObject::active, "Whether object is active");

    // AgentBody class
    py::class_<AgentBody>(m, "AgentBody", "Agent body state")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position")
        .def_readwrite("y", &AgentBody::y, "Y position")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation angle")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Current energy")
        .def_readwrite("health", &AgentBody::health, "Current health")
        .def_readwrite("age", &AgentBody::age, "Current age")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Last action time")
        .def("reset", &AgentBody::reset, "Reset agent body to initial state");

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", "Action result from world")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward, "Reward value")
        .def_readwrite("success", &ActionResult::success, "Whether action was successful")
        .def_readwrite("message", &ActionResult::message, "Result message");

    // SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", "Sensory percept data")
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

    // SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", "Simple 2D world for NLM simulation")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and vision parameters")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent's starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world simulation")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply a motor command to the world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get current agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add an object to the world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"), "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"), "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed");

    // Brain class
    py::class_<Brain>(m, "Brain", "Central neural simulation brain class")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
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
             "Log brain status");

    // AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", "Agent brain interface connecting NLM brain to world")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface with underlying brain")
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

    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration")

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        try {
            return std::make_shared<Brain>(config);
        } catch (const std::exception& e) {
            throw InitializationError("Failed to create brain: " + std::string(e.what()));
        }
    }, py::arg("config"), "Create a new brain with configuration")

          [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
             std::shared_ptr<AgentBrain> agent, int num_steps) {
              brain->initialize();
              world->reset();
              agent->initialize(*world);
              
              py::dict results;
              results["total_spikes"] = brain->getTotalSpikeCount();
              results["firing_neuron_counts"] = std::vector<int64_t>();
              
              for (int i = 0; i < num_steps; ++i) {
                  agent->step(i);
                  world->update(0.1);
                  results["firing_neuron_counts"].attr("append")(brain->getFiringNeuronCount());
              }
              
              return results;
          },
          py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
          "Run an agent simulation.\n\n"
          "Args:\n"
          "    brain: Brain object\n"
          "    world: World object\n"
          "    agent: Agent brain interface\n"
          "    num_steps: Number of simulation steps\n\n"
          "Returns:\n"
          "    Dictionary with simulation results"
    )

    // Type conversion utilities
    m.def("pyToCpp", [](py::object obj) {
        try {
            // Safe type conversion from Python to C++
            if (obj.is(py::str())) {
                return py::cast<std::string>(obj);
            } else if (obj.is(py::int_())) {
                return py::cast<int>(obj);
            } else if (obj.is(py::float_())) {
                return py::cast<double>(obj);
            } else if (obj.is(py::bool_())) {
                config->getOr<int>("vision_height", 16)
            );
            world->setMaxEnergy(config->getOr<double>("max_energy", 100.0));
            world->setEnergyDecayRate(config->getOr<double>("energy_decay", 0.01));
            world->setRandomSeed(config->getOr<int>("random_seed", 42));
        }
        return world;
    }, py::arg("config") = nullptr, "Create and configure a SimpleWorld from Config");

    // Factory functions for brain and agent creation
    m.def("createAgentBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<AgentBrain> {
        try {
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            auto agent = std::make_shared<AgentBrain>(brain);
            return agent;
        } catch (const std::exception& e) {
            throw InitializationError("Failed to create agent brain: " + std::string(e.what()));
        }
    }, py::arg("config"), "Create and initialize an AgentBrain from configuration");

    // Simulation convenience functions
    m.def("runSimulation", 
        [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, int steps) {
            brain->initialize();
            world->reset();
            
            py::dict results;
            results["total_spikes"] = brain->getTotalSpikeCount();
            results["firing_neuron_counts"] = std::vector<int64_t>();
            
            for (int i = 0; i < steps; ++i) {
                brain->step(i);
                world->update(0.1);
                results["firing_neuron_counts"].attr("append")(brain->getFiringNeuronCount());
            }
            
            return results;
        },
        py::arg("brain"), py::arg("world"), py::arg("steps"),
        "Run a simple brain-world simulation.\n\n"
        "Args:\n"
        "    brain: Brain object\n"
        "    world: World object\n"
        "    steps: Number of simulation steps\n\n"
        "Returns:\n"
        "    Dictionary with simulation results"
    );

    m.def("configureAgentBrain", 
        [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world) {
            agent->initialize(*world);
        },
        py::arg("agent"), py::arg("world"),
        "Configure an agent brain with a world.\n\n"
        "Args:\n"
        "    agent: AgentBrain object\n"
        "    world: World object\n\n"
        "Returns:\n"
        "    None"
    );

        // Export constant IDs
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Module metadata
    m.attr("__version__") = "1.0.0";
    m.attr("__description__") = "NLM (Neural Learning Machine) Python Bindings - Production Ready";
    m.attr("__author__") = "NLM Framework Team";
}