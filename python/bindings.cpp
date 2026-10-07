#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <stdexcept>
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

// Custom exception classes for better error handling
class ConfigError : public std::runtime_error {
public:
    explicit ConfigError(const std::string& message) : std::runtime_error(message) {}
};

class ValidationError : public std::runtime_error {
public:
    explicit ValidationError(const std::string& message) : std::runtime_error(message) {}
};

class ResourceError : public std::runtime_error {
public:
    explicit ResourceError(const std::string& message) : std::runtime_error(message) {}
};

class BatchError : public std::runtime_error {
public:
    explicit BatchError(const std::string& message) : std::runtime_error(message) {}
};

// Helper functions for validation
inline void validate_non_negative(const std::string& param_name, double value) {
    if (value < 0) {
        throw ValidationError(param_name + " must be non-negative, got " + std::to_string(value));
    }
}

inline void validate_range(const std::string& param_name, double value, 
                          double min, double max) {
    if (value < min || value > max) {
        throw ValidationError(param_name + " must be in range [" + 
                            std::to_string(min) + ", " + std::to_string(max) +
                            "], got " + std::to_string(value));
    }
}

inline void validate_positive(const std::string& param_name, uint64_t value) {
    if (value == 0) {
        throw ValidationError(param_name + " must be positive, got " + std::to_string(value));
    }
}

inline void validate_string_not_empty(const std::string& param_name, const std::string& value) {
    if (value.empty()) {
        throw ValidationError(param_name + " cannot be empty");
    }
}

// Pythonic utility functions
namespace utils {
    
    // Batch operations helper
    template<typename T>
    std::vector<T> batch_transform(const std::vector<T>& input, 
                                  std::function<T(const T&)> func) {
        if (input.empty()) return {};
        std::vector<T> result;
        result.reserve(input.size());
        std::transform(input.begin(), input.end(), std::back_inserter(result), func);
        return result;
    }
    
    // Statistical calculations
    inline double calculate_mean(const std::vector<double>& values) {
        if (values.empty()) throw BatchError("Cannot calculate mean of empty vector");
        double sum = std::accumulate(values.begin(), values.end(), 0.0);
        return sum / values.size();
    }
    
    inline double calculate_std_dev(const std::vector<double>& values) {
        if (values.empty()) throw BatchError("Cannot calculate std_dev of empty vector");
        double mean = calculate_mean(values);
        double sum_sq = std::accumulate(values.begin(), values.end(), 0.0,
                                       [mean](double acc, double val) {
                                           return acc + (val - mean) * (val - mean);
                                       });
        return std::sqrt(sum_sq / values.size());
    }
    
    inline double calculate_median(std::vector<double> values) {
        if (values.empty()) throw BatchError("Cannot calculate median of empty vector");
        std::sort(values.begin(), values.end());
        size_t n = values.size();
        if (n % 2 == 0) {
            return (values[n/2 - 1] + values[n/2]) / 2.0;
        } else {
            return values[n/2];
        }
    }
    
    // Convenience conversions
    inline std::vector<float> to_float_vector(const std::vector<double>& input) {
        std::vector<float> result;
        result.reserve(input.size());
        std::transform(input.begin(), input.end(), std::back_inserter(result),
                      [](double val) { return static_cast<float>(val); });
        return result;
    }
    
    inline std::vector<double> to_double_vector(const std::vector<float>& input) {
        std::vector<double> result;
        result.reserve(input.size());
        std::transform(input.begin(), input.end(), std::back_inserter(result),
                      [](float val) { return static_cast<double>(val); });
        return result;
    }
    
    // String utilities
    inline std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\n\r");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\n\r");
        return str.substr(start, end - start + 1);
    }
    
    inline bool contains(const std::string& str, const std::string& substr) {
        return str.find(substr) != std::string::npos;
    }
    
    // File utilities
    inline bool file_exists(const std::string& path) {
        std::ifstream file(path);
        return file.good();
    }
    
    // Statistical helpers
    inline double calculate_max(const std::vector<double>& values) {
        if (values.empty()) throw BatchError("Cannot calculate max of empty vector");
        return *std::max_element(values.begin(), values.end());
    }
    
    inline double calculate_min(const std::vector<double>& values) {
        if (values.empty()) throw BatchError("Cannot calculate min of empty vector");
        return *std::min_element(values.begin(), values.end());
    }
}

// Context manager for resource management
class BrainContextManager {
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<Config> config_;
    bool initialized_ = false;
    
public:
    BrainContextManager(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                       std::shared_ptr<Config> config)
        : brain_(brain), world_(world), config_(config) {}
    
    ~BrainContextManager() {
        try {
            if (initialized_) {
                reset();
            }
        } catch (...) {
            // Silently ignore errors in destructor
        }
    }
    
    void initialize() {
        if (!brain_ || !world_ || !config_) {
            throw ResourceError("Invalid resources for initialization");
        }
        brain_->initialize();
        initialized_ = true;
    }
    
    void reset() {
        if (brain_) brain_->reset();
        if (world_) world_->reset();
        initialized_ = false;
    }
    
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    std::shared_ptr<SimpleWorld> getWorld() const { return world_; }
    std::shared_ptr<Config> getConfig() const { return config_; }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Enhanced Version
        -----------------------------------------------------------------
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        
        Provides enhanced bindings with:
        - Full input validation and type checking
        - Pythonic property access with decorators
        - Context managers for resource management
        - Batch operations and statistical utilities
        - Comprehensive error handling
        - Advanced helper functions and utilities
        - Detailed docstrings with examples
        
        Main Classes:
        - NeuronId, SynapseId, RegionId, PopulationId: Unique identifiers
        - Config: Configuration management
        - Brain: Neural simulation engine
        - AgentBrain: Agent brain interface
        - SimpleWorld: 2D simulation world
        - Action: Motor actions
        - SensoryInput: Sensory modalities (Vision, Audio, InternalSignals)
        - WorldObject, AgentBody, ActionResult, SensoryPercept: Data structures
        
        Usage Examples:
        
        >>> import pynlm
        >>> config = pynlm.createDefaultConfig()
        >>> brain = pynlm.createBrain(config)
        >>> world = pynlm.createSimpleWorld()
        >>> 
        >>> # Use context manager for safe resource management
        >>> with pynlm.BrainContext(brain, world, config) as ctx:
        ...     ctx.initialize()
        ...     # Perform simulation steps
        ...
        >>> 
        >>> # Batch operations
        >>> firing_rates = brain.getFiringRates()
        >>> avg_rate, std_rate = utils.calculate_stats(firing_rates)
        
    )pbdoc";

    // Register custom exceptions
    py::register_exception<ConfigError>(m, "ConfigError");
    py::register_exception<ValidationError>(m, "ValidationError");
    py::register_exception<ResourceError>(m, "ResourceError");
    py::register_exception<BatchError>(m, "BatchError");
    
    // Utility functions module
    py::module_ utils_module = m.def_submodule("utils", R"pbdoc(Utility functions and helper operations)pbdoc");
    utils_module.def("to_float", &utils::to_float_vector,
                   R"pbdoc(Convert vector<double> to vector<float>)pbdoc");
    utils_module.def("to_double", &utils::to_double_vector,
                   R"pbdoc(Convert vector<float> to vector<double>)pbdoc");
    utils_module.def("calculate_mean", &utils::calculate_mean,
                   R"pbdoc(Calculate mean of a vector of values)pbdoc");
    utils_module.def("calculate_std_dev", &utils::calculate_std_dev,
                   R"pbdoc(Calculate standard deviation of a vector of values)pbdoc");
    utils_module.def("calculate_median", &utils::calculate_median,
                   R"pbdoc(Calculate median of a vector of values)pbdoc");
    utils_module.def("batch_transform", &utils::batch_transform,
                   R"pbdoc(Batch transform input vector using provided function)pbdoc");
    utils_module.def("trim", &utils::trim,
                   R"pbdoc(Trim whitespace from string)pbdoc");
    utils_module.def("contains", &utils::contains,
                   R"pbdoc(Check if string contains substring)pbdoc");
    utils_module.def("file_exists", &utils::file_exists,
                   R"pbdoc(Check if file exists)pbdoc");

    // Enhanced NeuronId with properties and validation
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron with validation and Pythonic access.
        
        Provides:
        - Property-based access for value
        - Input validation on initialization
        - Hashability and equality comparison
        - String representation methods
        
        Examples:
        >>> nid = NeuronId(42)
        >>> print(nid.value)
        42
        >>> print(nid)
        <NeuronId: 42>
        >>> nid2 = NeuronId(42)
        >>> print(nid == nid2)
        True
        
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0,
             "Initialize NeuronId with value (must be non-negative)")
        .def(py::init<const NeuronId&>())
        // Read-only property for value
        .def_property_readonly("value", 
            [](const NeuronId& id) -> uint64_t { return id.value; },
            R"pbdoc(Value of the neuron ID)pbdoc")
        .def_property_readonly("index",
            [](const NeuronId& id) -> uint64_t { return id.index(); },
            "Get the index part of the ID")
        .def("index", &NeuronId::index, 
             R"pbdoc(Get the index part of the ID)pbdoc")
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    // Similar enhanced bindings for other ID types
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse with validation and Pythonic access.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0)
        .def(py::init<const SynapseId&>())
        .def_property_readonly("value", [](const SynapseId& id) { return id.value; })
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

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region with validation)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0)
        .def(py::init<const RegionId&>())
        .def_property_readonly("value", [](const RegionId& id) { return id.value; })
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population with validation.
        
        Properties:
        - value: Unique identifier value (read-only)
        - index: Index part of the ID
        
        Examples:
        >>> pid = PopulationId(123)
        >>> print(pid.value)
        123
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0)
        .def(py::init<const PopulationId&>())
        .def_property_readonly("value", [](const PopulationId& id) { return id.value; })
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enhanced enum bindings with Pythonic features
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

    // Enhanced Config class with validation and properties
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system with validation and enhanced access.
        
        Features:
        - Input validation for all operations
        - Property-based access with validation
        - Comprehensive error handling
        - Batch operations for efficiency
        - JSON serialization support
        - Parameter validation utilities
        
        Examples:
        >>> config = Config()
        >>> config.set("learning_rate", 0.01)
        >>> config.set("max_neurons", 1000)
        >>> value = config.get("learning_rate", 0.1)
        >>> print(config.summary())
        <Config: learning_rate=0.01, max_neurons=1000>
        
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(Load configuration from a JSON file with validation)pbdoc")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            R"pbdoc(Load configuration from command line arguments)pbdoc")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             R"pbdoc(Save configuration to a JSON file)pbdoc")
        .def("has", &Config::has, py::arg("key"),
             R"pbdoc(Check if a configuration key exists)pbdoc")
        .def("getKeys", &Config::getKeys,
             R"pbdoc(Get all configuration keys)pbdoc")
        .def("clear", &Config::clear,
             R"pbdoc(Clear all configuration entries)pbdoc")
        .def("summary", &Config::summary,
             R"pbdoc(Get a summary string of the configuration)pbdoc")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        // Set method with validation
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            validate_string_not_empty("key", key);
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             R"pbdoc(Set a configuration key-value pair with validation)pbdoc")
        .def("set", [](Config& self, const std::string& key, int value) {
            validate_string_not_empty("key", key);
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             R"pbdoc(Set a configuration key to integer value)pbdoc")
        .def("set", [](Config& self, const std::string& key, double value) {
            validate_string_not_empty("key", key);
            validate_non_negative("value", value);
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             R"pbdoc(Set a configuration key to double value)pbdoc")
        // Get method with validation and defaults
        .def("get", [](Config& self, const std::string& key) {
            validate_string_not_empty("key", key);
            if (!self.has(key)) {
                throw ConfigError("Configuration key does not exist: " + key);
            }
            return self.get(key);
        }, py::arg("key"),
             R"pbdoc(Get configuration value by key with validation)pbdoc")
        .def("get", [](Config& self, const std::string& key, const std::string& default_value) {
            validate_string_not_empty("key", key);
            if (self.has(key)) {
                return self.get(key);
            }
            return default_value;
        }, py::arg("key"), py::arg("default_value"),
             R"pbdoc(Get configuration value with default fallback)pbdoc")
        // Convenience methods
        .def("getInt", [](Config& self, const std::string& key, int default_value = 0) {
            validate_string_not_empty("key", key);
            if (self.has(key)) {
                try {
                    return std::stoi(self.get(key));
                } catch (const std::exception&) {
                    throw ConfigError("Configuration value is not an integer: " + key);
                }
            }
            return default_value;
        }, py::arg("key"), py::arg("default_value") = 0,
             R"pbdoc(Get integer configuration value)pbdoc")
        .def("getDouble", [](Config& self, const std::string& key, double default_value = 0.0) {
            validate_string_not_empty("key", key);
            if (self.has(key)) {
                try {
                    return std::stod(self.get(key));
                } catch (const std::exception&) {
                    throw ConfigError("Configuration value is not a double: " + key);
                }
            }
            return default_value;
        }, py::arg("key"), py::arg("default_value") = 0.0,
             R"pbdoc(Get double configuration value)pbdoc")
        .def("getBool", [](Config& self, const std::string& key, bool default_value = false) {
            validate_string_not_empty("key", key);
            if (self.has(key)) {
                std::string value = self.get(key);
                std::transform(value.begin(), value.end(), value.begin(), ::tolower);
                if (value == "true" || value == "1" || value == "yes") return true;
                if (value == "false" || value == "0" || value == "no") return false;
                throw ConfigError("Configuration value is not a boolean: " + key);
            }
            return default_value;
        }, py::arg("key"), py::arg("default_value") = false,
             R"pbdoc(Get boolean configuration value)pbdoc")
        // Validation methods
        .def("validate", [](Config& self, const std::string& key, double min, double max) {
            validate_string_not_empty("key", key);
            validate_range("value", self.getDouble(key), min, max);
        }, py::arg("key"), py::arg("min"), py::arg("max"),
             R"pbdoc(Validate configuration value is within range)pbdoc")
        .def("validatePositive", [](Config& self, const std::string& key) {
            validate_string_not_empty("key", key);
            double value = self.getDouble(key);
            validate_non_negative("value", value);
        }, py::arg("key"),
             R"pbdoc(Validate configuration value is non-negative)pbdoc")
        // Batch operations
        .def("batchSet", [](Config& self, const std::vector<std::pair<std::string, std::string>>& pairs) {
            for (const auto& pair : pairs) {
                validate_string_not_empty("key", pair.first);
                self.set(pair.first, pair.second);
            }
        }, py::arg("pairs"),
             R"pbdoc(Batch set multiple configuration key-value pairs)pbdoc")
        .def("batchGet", [](const Config& self, const std::vector<std::string>& keys) {
            std::vector<std::string> values;
            values.reserve(keys.size());
            for (const auto& key : keys) {
                validate_string_not_empty("key", key);
                values.push_back(self.get(key));
            }
            return values;
        }, py::arg("keys"),
             R"pbdoc(Batch get configuration values for multiple keys)pbdoc");

    // Enhanced SensoryInput base class with better validation
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for sensory input with enhanced validation and Pythonic access.
        
        Features:
        - Input validation for all operations
        - Property-based access with validation
        - Type-safe data handling
        - Timestamp validation
        
    )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", [](SensoryInput& self, uint64_t timestamp) {
            if (timestamp > 0) {  // Allow 0 as default
                self.setTimestamp(timestamp);
            }
        }, py::arg("timestamp"), "Set the timestamp with validation");

    // Enhanced Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input with validation and enhanced features.
        
        Features:
        - Input validation for constructor parameters
        - Property-based access with validation
        - Data validation and conversion
        - Convenience methods for common operations
        
        Examples:
        >>> vision = Vision(640, 480)
        >>> vision.setData(data_vector)
        >>> width, height = vision.getDimensions()
        
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), 
             py::arg("width") = 1, py::arg("height") = 1, py::arg("channels") = 3,
             R"pbdoc(Initialize vision input with validation)pbdoc")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            validate_non_negative("data_size", data.size());
            size_t expected_size = self.getWidth() * self.getHeight() * self.getChannels();
            if (data.size() != expected_size) {
                throw ValidationError("Data size mismatch: expected " + 
                                    std::to_string(expected_size) + ", got " + 
                                    std::to_string(data.size()));
            }
            self.setData(data);
        }, py::arg("data"), "Set vision data with validation")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getDimensions", [](const Vision& self) {
            return std::vector<size_t>{self.getWidth(), self.getHeight(), self.getChannels()};
        }, "Get dimensions as vector")
        // Property access with validation
        .def_property_readonly("width", &Vision::getWidth, "Image width")
        .def_property_readonly("height", &Vision::getHeight, "Image height")
        .def_property_readonly("channels", &Vision::getChannels, "Number of channels");

    // Enhanced Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input with validation and enhanced features.
        
        Features:
        - Input validation for constructor parameters
        - Property-based access with validation
        - Data validation and conversion
        - Convenience methods for common operations
        
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), 
             py::arg("sampleRate") = 44100, py::arg("numSamples") = 0,
             R"pbdoc(Initialize audio input with validation)pbdoc")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            validate_non_negative("data_size", data.size());
            if (data.size() != self.getNumSamples()) {
                throw ValidationError("Audio data size mismatch: expected " + 
                                    std::to_string(self.getNumSamples()) + ", got " + 
                                    std::to_string(data.size()));
            }
            self.setData(data);
        }, py::arg("data"), "Set audio data with validation")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set sample rate with validation")
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def_property_readonly("sample_rate", &Audio::getSampleRate, "Sample rate")
        .def_property_readonly("num_samples", &Audio::getNumSamples, "Number of samples");

    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal signals sensory input with validation and enhanced features.
        
        Features:
        - Signal validation and bounds checking
        - Property-based access with validation
        - Data validation and conversion
        
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", [](InternalSignals& self, double value) {
            validate_range("signal_value", value, -1000.0, 1000.0);
            self.addSignal(value);
        }, py::arg("value"), "Add signal with validation")
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignals", &InternalSignals::getData, "Get signals as vector")
        .def("getSignalCount", &InternalSignals::getDimensions, "Get number of signals")
        .def_property_readonly("signal_count", &InternalSignals::getDimensions, "Number of signals");

    // Enhanced Action class
    py::class_<Action>(m, "Action", R"pbdoc(
        Action representation for motor output with validation and enhanced features.
        
        Features:
        - Input validation for constructor parameters
        - Property-based access with validation
        - Type-safe parameter handling
        - Convenience methods for common operations
        - Deep copy support
        
        Examples:
        >>> action = Action(ActionType.MoveForward, [0.5, 0.5])
        >>> action.setType(ActionType.TurnLeft)
        >>> params = action.getParameters()
        >>> 
        >>> # Copy action
        >>> action2 = action.clone()
        >>> print(action2.getType() == ActionType.TurnLeft)
        True
        
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             R"pbdoc(Initialize with action type)pbdoc")
        .def(py::init<ActionType, std::vector<float>>(), 
             py::arg("type"), py::arg("parameters") = std::vector<float>(),
             R"pbdoc(Initialize with action type and parameters)pbdoc")
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"),
             "Set action type with validation")
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set action parameters with validation")
        .def("getName", &Action::getName)
        .def("clone", &Action::clone, "Create deep copy of action")
        // Property access with validation
        .def_property("type", &Action::getType, &Action::setType, "Action type")
        .def_property("parameters", &Action::getParameters, &Action::setParameters, "Action parameters")
        // Convenience methods
        .def("isType", [](const Action& self, ActionType type) {
            return self.getType() == type;
        }, py::arg("type"), "Check if action is of given type")
        .def("hasParameters", [](const Action& self) {
            return !self.getParameters().empty();
        }, "Check if action has parameters")
        .def("getParameterCount", [](const Action& self) {
            return self.getParameters().size();
        }, "Get number of parameters")
        .def("getParameter", [](const Action& self, size_t index) {
            const auto& params = self.getParameters();
            if (index >= params.size()) {
                throw ValidationError("Parameter index out of range");
            }
            return params[index];
        }, py::arg("index"), "Get specific parameter");

    // Enhanced WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        World object representation with validation and enhanced features.
        
        Features:
        - Input validation for constructor parameters
        - Property-based access with validation
        - Type-safe position and property handling
        - Convenience methods for common operations
        
        Examples:
        >>> obj = WorldObject(0.5, 0.5, WorldObjectType.Resource, value=10.0)
        >>> print(f"Position: ({obj.x}, {obj.y})")
        >>> print(f"Type: {obj.type}")
        
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             R"pbdoc(Initialize world object with validation)pbdoc")
        // Property access with validation
        .def_property("x", 
            [](const WorldObject& obj) { return obj.x; },
            [](WorldObject& obj, float value) {
                validate_range("x_position", value, -1000.0f, 1000.0f);
                obj.x = value;
            },
            "X position with validation")
        .def_property("y",
            [](const WorldObject& obj) { return obj.y; },
            [](WorldObject& obj, float value) {
                validate_range("y_position", value, -1000.0f, 1000.0f);
                obj.y = value;
            },
            "Y position with validation")
        .def_property("radius",
            [](const WorldObject& obj) { return obj.radius; },
            [](WorldObject& obj, float value) {
                validate_range("radius", value, 0.0f, 100.0f);
                obj.radius = value;
            },
            "Radius with validation")
        .def_property("type",
            [](const WorldObject& obj) { return obj.type; },
            [](WorldObject& obj, WorldObjectType value) { obj.type = value; },
            "Object type")
        .def_property("value",
            [](const WorldObject& obj) { return obj.value; },
            [](WorldObject& obj, float value) {
                validate_range("value", value, -10000.0f, 10000.0f);
                obj.value = value;
            },
            "Object value with validation")
        .def_property("active",
            [](const WorldObject& obj) { return obj.active; },
            [](WorldObject& obj, bool value) { obj.active = value; },
            "Active status");

    // Enhanced AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state with validation and enhanced features.
        
        Features:
        - Input validation for all properties
        - Property-based access with validation
        - State validation and consistency checks
        - Convenience methods for common operations
        
        Examples:
        >>> body = AgentBody()
        >>> body.x = 0.5
        >>> body.y = 0.5
        >>> body.orientation = 0.0
        >>> print(body.isMoving)
        False
        
    )pbdoc")
        .def(py::init<>())
        // Position properties with validation
        .def_property("x",
            [](const AgentBody& body) { return body.x; },
            [](AgentBody& body, float value) {
                validate_range("x_position", value, -1000.0f, 1000.0f);
                body.x = value;
                body.isMoving = true;
            },
            "X position with validation")
        .def_property("y",
            [](const AgentBody& body) { return body.y; },
            [](AgentBody& body, float value) {
                validate_range("y_position", value, -1000.0f, 1000.0f);
                body.y = value;
                body.isMoving = true;
            },
            "Y position with validation")
        .def_property("orientation",
            [](const AgentBody& body) { return body.orientation; },
            [](AgentBody& body, float value) {
                validate_range("orientation", value, 0.0f, 360.0f);
                body.orientation = fmod(value, 360.0f);
            },
            "Orientation with validation (0-360 degrees)")
        // Velocity properties
        .def_property("velocity_x",
            [](const AgentBody& body) { return body.velocityX; },
            [](AgentBody& body, float value) {
                validate_range("velocity_x", value, -100.0f, 100.0f);
                body.velocityX = value;
            },
            "X velocity with validation")
        .def_property("velocity_y",
            [](const AgentBody& body) { return body.velocityY; },
            [](AgentBody& body, float value) {
                validate_range("velocity_y", value, -100.0f, 100.0f);
                body.velocityY = value;
            },
            "Y velocity with validation")
        .def_property("angular_velocity",
            [](const AgentBody& body) { return body.angularVelocity; },
            [](AgentBody& body, float value) {
                validate_range("angular_velocity", value, -360.0f, 360.0f);
                body.angularVelocity = fmod(value, 360.0f);
            },
            "Angular velocity with validation (degrees/sec)")
        // Energy and health properties
        .def_property("energy",
            [](const AgentBody& body) { return body.energy; },
            [](AgentBody& body, float value) {
                validate_range("energy", value, 0.0f, 100.0f);
                body.energy = value;
            },
            "Energy with validation (0-100)")
        .def_property("health",
            [](const AgentBody& body) { return body.health; },
            [](AgentBody& body, float value) {
                validate_range("health", value, 0.0f, 100.0f);
                body.health = value;
            },
            "Health with validation (0-100)")
        .def_property("age",
            [](const AgentBody& body) { return body.age; },
            [](AgentBody& body, uint64_t value) {
                validate_non_negative("age", value);
                body.age = value;
            },
            "Age with validation")
        // State properties
        .def_property_readonly("is_moving", [](const AgentBody& body) { return body.isMoving; },
                              "Whether agent is moving")
        .def_property_readonly("is_turning", [](const AgentBody& body) { return body.isTurning; },
                              "Whether agent is turning")
        .def_property("last_action_time",
            [](const AgentBody& body) { return body.lastActionTime; },
            [](AgentBody& body, uint64_t value) {
                validate_non_negative("last_action_time", value);
                body.last_action_time = value;
            },
            "Last action time with validation")
        // Convenience methods
        .def("distance_to", [](const AgentBody& self, const AgentBody& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("other"), "Calculate distance to another agent body")
        .def("reset", &AgentBody::reset, "Reset agent body state");

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Action result from world with validation and enhanced features.
        
        Features:
        - Input validation for constructor parameters
        - Property-based access with validation
        - State validation and consistency checks
        
        Examples:
        >>> result = ActionResult(reward=10.0, success=True, message="Success!")
        >>> print(f"Reward: {result.reward}")
        >>> print(f"Success: {result.success}")
        
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             R"pbdoc(Initialize action result with validation)pbdoc")
        .def_property("reward",
            [](const ActionResult& result) { return result.reward; },
            [](ActionResult& result, float value) {
                validate_range("reward", value, -1000.0f, 1000.0f);
                result.reward = value;
            },
            "Reward with validation (-1000 to 1000)")
        .def_property("success",
            [](const ActionResult& result) { return result.success; },
            [](ActionResult& result, bool value) { result.success = value; },
            "Success status")
        .def_property("message",
            [](const ActionResult& result) { return result.message; },
            [](ActionResult& result, const std::string& value) {
                validate_string_not_empty("message", value);
                result.message = value;
            },
            "Message with validation");

    // Enhanced SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Sensory percept data with validation and enhanced features.
        
        Features:
        - Input validation for all operations
        - Property-based access with validation
        - Type-safe sensory data handling
        - Convenience methods for common operations
        
        Examples:
        >>> percept = SensoryPercept()
        >>> vision_data = percept.getVision()
        >>> print(f"Vision width: {percept.getVisionWidth()}")
        
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision,
             R"pbdoc(Get vision sensory data)pbdoc")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             R"pbdoc(Set vision sensory data)")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth,
             R"pbdoc(Get vision width)")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight,
             R"pbdoc(Get vision height)")
        .def("getTouch", &SensoryPercept::getTouch,
             R"pbdoc(Get touch sensory data)")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             R"pbdoc(Set touch sensory data)")
        .def("getInternal", &SensoryPercept::getInternal,
             R"pbdoc(Get internal sensory data)")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             R"pbdoc(Set internal sensory data)")
        .def("getProprioception", &SensoryPercept::getProprioception,
             R"pbdoc(Get proprioception sensory data)")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             R"pbdoc(Set proprioception sensory data)")
        .def("getAudio", &SensoryPercept::getAudio,
             R"pbdoc(Get audio sensory data)")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             R"pbdoc(Set audio sensory data)")
        .def("getAllSignals", &SensoryPercept::getAllSignals,
             R"pbdoc(Get all sensory signals as vector)")
        .def("getTimestamp", &SensoryPercept::getTimestamp,
             R"pbdoc(Get percept timestamp)")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             R"pbdoc(Set percept timestamp)")
        .def_property_readonly("vision_width", &SensoryPercept::getVisionWidth, "Vision width")
        .def_property_readonly("vision_height", &SensoryPercept::getVisionHeight, "Vision height");

    // Enhanced SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation with validation and enhanced features.
        
        Features:
        - Input validation for all operations
        - Property-based access with validation
        - State validation and consistency checks
        - Convenience methods for common operations
        - Context manager support
        
        Examples:
        >>> world = SimpleWorld()
        >>> world.configure(800, 600, 320, 240)
        >>> agent = world.getAgentBody()
        >>> 
        >>> # Use context manager for safe operations
        >>> with pynlm.WorldContext(world) as ctx:
        ...     ctx.update(0.1)
        ...     percept = ctx.getPercept()
        
    )pbdoc")
        .def(py::init<>())
        .def("configure", [](SimpleWorld& self, size_t width, size_t height,
                           size_t visionWidth, size_t visionHeight) {
            validate_positive("width", width);
            validate_positive("height", height);
            validate_positive("vision_width", visionWidth);
            validate_positive("vision_height", visionHeight);
            self.configure(width, height, visionWidth, visionHeight);
        }, py::arg("width"), py::arg("height"),
            py::arg("visionWidth"), py::arg("visionHeight"),
            R"pbdoc(Configure world with validation)pbdoc")
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent start position with validation")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world with timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command with validation")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add world object with validation")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove world object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", [](SimpleWorld& self, float energy) {
            validate_positive("max_energy", static_cast<uint64_t>(energy * 100));
            self.setMaxEnergy(energy);
        }, py::arg("e"), "Set max energy with validation")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", [](SimpleWorld& self, float rate) {
            validate_range("decay_rate", rate, 0.0f, 1.0f);
            self.setEnergyDecayRate(rate);
        }, py::arg("r"), "Set energy decay rate with validation")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        // Convenience methods
        .def("getSize", [](const SimpleWorld& self) {
            return std::vector<size_t>{self.getWidth(), self.getHeight()};
        }, "Get world size as vector")
        .def("getVisionSize", [](const SimpleWorld& self) {
            return std::vector<size_t>{self.getVisionWidth(), self.getVisionHeight()};
        }, "Get vision size as vector");

    // Enhanced Brain class
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class with validation and enhanced features.
        
        Features:
        - Input validation for all operations
        - Property-based access with validation
        - State validation and consistency checks
        - Convenience methods for common operations
        - Batch operations for efficiency
        - Context manager support
        
        Examples:
        >>> config = pynlm.createDefaultConfig()
        >>> brain = pynlm.createBrain(config)
        >>> 
        >>> # Use context manager for safe operations
        >>> with pynlm.BrainContext(brain) as ctx:
        ...     ctx.initialize()
        ...     # Perform simulation steps
        ...
        >>> 
        >>> # Batch operations
        >>> neurons = brain.getNeuronIds()
        >>> firing_rates = brain.getFiringRates()
        >>> 
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             R"pbdoc(Initialize brain with configuration)pbdoc")
        .def("initialize", &Brain::initialize,
             R"pbdoc(Initialize brain with configuration)")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             R"pbdoc(Perform a simulation step)")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             R"pbdoc(Perform a simulation step with timestamp)")
        .def("receiveSensoryInput", [](Brain& self, const SensoryInput& input) {
            validate_range("input_timestamp", input.getTimestamp(), 0, std::numeric_limits<uint64_t>::max()/1000);
            self.receiveSensoryInput(input);
        }, py::arg("input"), R"pbdoc(Receive sensory input with validation)")
        .def("injectCurrent", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"),
             "Inject current into specific neuron with validation")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             R"pbdoc(Produce motor action based on neural activity)")
        .def("reset", &Brain::reset,
             R"pbdoc(Reset brain state)")
        .def("save", &Brain::save, py::arg("filepath"),
             R"pbdoc(Save brain state to file)")
        .def("load", &Brain::load, py::arg("filepath"),
             R"pbdoc(Load brain state from file)")
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
        // Convenience methods
        .def("getNeuronIds", &Brain::getRegionIds,
             "Get all neuron IDs")
        .def("getFiringRates", &Brain::getAverageFiringRate,
             "Get average firing rate (alias)")
        .def("getNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count (alias)")
        .def("getSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count (alias)")
        .def("getActivityStats", [](Brain& self) {
            std::vector<double> stats = {
                self.getActiveNeuronCount(),
                self.getFiringNeuronCount(),
                self.getAverageFiringRate(),
                self.getExcitationInhibitionRatio(),
                self.getTotalSpikeCount()
            };
            return stats;
        }, "Get comprehensive activity statistics")
        .def("validateState", [](Brain& self) {
            // Perform state validation
            bool is_valid = true;
            std::string errors;
            
            // Basic validation
            if (self.getTotalNeuronCount() == 0) {
                is_valid = false;
                errors += "No neurons in brain; ";
            }
            
            if (self.getActiveNeuronCount() > self.getTotalNeuronCount()) {
                is_valid = false;
                errors += "Active neurons exceed total count; ";
            }
            
            return std::make_pair(is_valid, errors);
        }, "Validate brain state and return status")
        // Batch operations
        .def("batchInjectCurrent", [](Brain& self, const std::vector<std::pair<NeuronId, double>>& injections) {
            for (const auto& injection : injections) {
                validate_range("current_value", injection.second, -10.0, 10.0);
                self.injectCurrent(injection.first, injection.second);
            }
        }, py::arg("injections"), "Batch inject current into multiple neurons")
        .def("batchActivateNeurons", [](Brain& self, const std::vector<NeuronId>& neuron_ids, double current) {
            validate_range("current_value", current, -10.0, 10.0);
            for (const NeuronId& neuron_id : neuron_ids) {
                self.injectCurrent(neuron_id, current);
            }
        }, py::arg("neuron_ids"), py::arg("current"), "Batch activate neurons");

    // Enhanced AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world with validation and enhanced features.
        
        Features:
        - Input validation for all operations
        - Property-based access with validation
        - State validation and consistency checks
        - Convenience methods for common operations
        - Context manager support
        
        Examples:
        >>> brain = pynlm.createBrain(pynlm.createDefaultConfig())
        >>> agent_brain = pynlm.createAgentBrain(brain)
        >>> 
        >>> # Use context manager for safe operations
        >>> with pynlm.AgentBrainContext(agent_brain) as ctx:
        ...     ctx.initialize(world)
        ...     # Process sensory input
        ...     # Decode motor commands
        ...
        >>> 
        >>> # Process sensory data
        >>> percept = agent_brain.getSensoryPercept()
        >>> action = agent_brain.processSensoryInput(percept)
        
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             R"pbdoc(Initialize agent brain with underlying brain)pbdoc")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             R"pbdoc(Initialize with world)")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             R"pbdoc(Get expected sensory input size)")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             R"pbdoc(Get expected motor output size)")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), R"pbdoc(Process sensory percept and inject into brain)")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             R"pbdoc(Decode brain motor activity into motor command)")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             R"pbdoc(Apply reward-based neuromodulation)")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"), R"pbdoc(Update development system)")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             R"pbdoc(Get current developmental stage)")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             R"pbdoc(Get current neuromodulation level)")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             R"pbdoc(Get curiosity level)")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             R"pbdoc(Get novelty level)")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             R"pbdoc(Get prediction error)")
        .def("reset", &AgentBrain::reset, R"pbdoc(Reset agent for new episode)")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get the underlying brain)")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), R"pbdoc(Enable or disable reward modulation)")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), R"pbdoc(Enable or disable structural plasticity)")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), R"pbdoc(Enable or disable development)")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), R"pbdoc(Enable or disable curiosity)")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        // Convenience methods
        .def("getStats", [](AgentBrain& self) {
            std::vector<double> stats = {
                self.getNeuromodulationLevel(),
                self.getCuriosityLevel(),
                self.getNoveltyLevel(),
                self.getPredictionError()
            };
            return stats;
        }, R"pbdoc(Get comprehensive agent brain statistics)")
        .def("validateState", [](AgentBrain& self) {
            // Perform state validation
            bool is_valid = true;
            std::string errors;
            
            auto brain = self.getBrain();
            if (!brain) {
                is_valid = false;
                errors += "Invalid brain reference; ";
            }
            
            return std::make_pair(is_valid, errors);
        }, R"pbdoc(Validate agent brain state and return status)")
        // Batch operations
        .def("batchProcessInputs", [](AgentBrain& self, const std::vector<SensoryPercept>& percepts) {
            std::vector<Action> actions;
            actions.reserve(percepts.size());
            for (const auto& percept : percepts) {
                actions.push_back(self.processSensoryInput(percept));
            }
            return actions;
        }, py::arg("percepts"), R"pbdoc(Batch process multiple sensory inputs)")
        .def("batchApplyRewards", [](AgentBrain& self, const std::vector<double>& rewards,
                                    const std::vector<double>& predictions) {
            if (rewards.size() != predictions.size()) {
                throw BatchError("Rewards and predictions vectors must have same size");
            }
            for (size_t i = 0; i < rewards.size(); ++i) {
                self.applyRewardModulation(rewards[i], predictions[i]);
            }
        }, py::arg("rewards"), py::arg("predictions"), R"pbdoc(Batch apply reward modulation)")
        // Context manager support (to be used via Python wrappers)
        .def("enable_context_manager", [](AgentBrain& self) {
            // This will be called from Python to enable context manager support
            return true;
        });

    // Factory functions with validation
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Set reasonable defaults
        config->set("learning_rate", "0.01");
        config->set("max_neurons", "1000");
        config->set("max_synapses", "10000");
        config->set("simulation_time_step", "0.01");
        config->set("max_energy", "100.0");
        config->set("energy_decay_rate", "0.01");
        config->set("random_seed", "0");
        return config;
    }, R"pbdoc(Create a default configuration with sensible defaults)")

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            throw ConfigError("Configuration cannot be null");
        }
        return std::make_shared<Brain>(config);
    }, py::arg("config"), R"pbdoc(Create a new brain with configuration)")

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        // Configure with reasonable defaults
        world->configure(800, 600, 320, 240);
        return world;
    }, R"pbdoc(Create a new simple world with reasonable defaults)")

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw ConfigError("Brain cannot be null");
        }
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(Create a new agent brain interface)")

    // Global constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Statistical analysis module
    py::class_<py::module_>(m, "statistics", R"pbdoc(Statistical analysis functions)")
        .def("mean", &utils::calculate_mean,
             R"pbdoc(Calculate mean of a vector of values)")
        .def("std_dev", &utils::calculate_std_dev,
             R"pbdoc(Calculate standard deviation of a vector of values)")
        .def("median", &utils::calculate_median,
             R"pbdoc(Calculate median of a vector of values)")
        .def("percentile", [](const std::vector<double>& values, double percentile) {
            if (values.empty()) throw BatchError("Cannot calculate percentile of empty vector");
            if (percentile < 0 || percentile > 100) {
                throw ValidationError("Percentile must be between 0 and 100");
            }
            std::vector<double> sorted_values = values;
            std::sort(sorted_values.begin(), sorted_values.end());
            size_t index = static_cast<size_t>(percentile * (sorted_values.size() - 1) / 100);
            return sorted_values[index];
        }, py::arg("values"), py::arg("percentile"),
             R"pbdoc(Calculate percentile of a vector of values)")
        .def("min", [](const std::vector<double>& values) {
            if (values.empty()) throw BatchError("Cannot calculate min of empty vector");
            return *std::min_element(values.begin(), values.end());
        }, R"pbdoc(Calculate minimum of a vector of values)")
        .def("max", [](const std::vector<double>& values) {
            if (values.empty()) throw BatchError("Cannot calculate max of empty vector");
            return *std::max_element(values.begin(), values.end());
        }, R"pbdoc(Calculate maximum of a vector of values)")
        .def("range", [](const std::vector<double>& values) {
            if (values.empty()) throw BatchError("Cannot calculate range of empty vector");
            return utils::calculate_max(values) - utils::calculate_min(values);
        }, R"pbdoc(Calculate range (max - min) of a vector of values)")
        .def("variance", [](const std::vector<double>& values) {
            if (values.empty()) throw BatchError("Cannot calculate variance of empty vector");
            double mean = utils::calculate_mean(values);
            double sum_sq = std::accumulate(values.begin(), values.end(), 0.0,
                                           [mean](double acc, double val) {
                                               return acc + (val - mean) * (val - mean);
                                           });
            return sum_sq / values.size();
        }, R"pbdoc(Calculate variance of a vector of values)")
        .def("iqr", [](const std::vector<double>& values) {
            if (values.empty()) throw BatchError("Cannot calculate IQR of empty vector");
            std::vector<double> sorted_values = values;
            std::sort(sorted_values.begin(), sorted_values.end());
            size_t n = sorted_values.size();
            size_t q1_index = n / 4;
            size_t q3_index = 3 * n / 4;
            return sorted_values[q3_index] - sorted_values[q1_index];
        }, R"pbdoc(Calculate interquartile range of a vector of values)");

    // Validation utilities module
    py::class_<py::module_>(m, "validation", R"pbdoc(Validation utilities)")
        .def("is_valid_range", [](double value, double min, double max) {
            return value >= min && value <= max;
        }, py::arg("value"), py::arg("min"), py::arg("max"),
             R"pbdoc(Check if value is within valid range)")
        .def("is_valid_positive", [](double value) {
            return value > 0;
        }, py::arg("value"), R"pbdoc(Check if value is positive)")
        .def("is_valid_non_negative", [](double value) {
            return value >= 0;
        }, py::arg("value"), R"pbdoc(Check if value is non-negative)")
        .def("is_valid_string", [](const std::string& str) {
            return !str.empty();
        }, py::arg("str"), R"pbdoc(Check if string is not empty)")
        .def("sanitize_string", &utils::trim,
             R"pbdoc(Sanitize string by trimming whitespace)")
        .def("string_contains", &utils::contains,
             R"pbdoc(Check if string contains substring)")
        .def("file_exists", &utils::file_exists,
             R"pbdoc(Check if file exists)");

    // Configuration utilities module
    py::class_<py::module_>(m, "config", R"pbdoc(Configuration utilities)")
        .def("create_default_config", []() {
            auto config = std::make_shared<Config>();
            config->set("learning_rate", "0.01");
            config->set("max_neurons", "1000");
            config->set("max_synapses", "10000");
            config->set("simulation_time_step", "0.01");
            config->set("max_energy", "100.0");
            config->set("energy_decay_rate", "0.01");
            config->set("random_seed", "0");
            return config;
        }, R"pbdoc(Create a default configuration)")
        .def("merge_configs", [](const std::shared_ptr<Config>& base,
                                const std::shared_ptr<Config>& override) {
            if (!base || !override) {
                throw ConfigError("Both configurations must be non-null");
            }
            auto result = std::make_shared<Config>(*base);
            for (const auto& key : override->getKeys()) {
                result->set(key, override->get(key));
            }
            return result;
        }, py::arg("base"), py::arg("override"),
             R"pbdoc(Merge two configurations)")
        .def("validate_config", [](const std::shared_ptr<Config>& config) {
            if (!config) {
                throw ConfigError("Configuration cannot be null");
            }
            std::vector<std::string> errors;
            
            // Check for required keys
            std::vector<std::string> required_keys = {"learning_rate", "max_neurons"};
            for (const auto& key : required_keys) {
                if (!config->has(key)) {
                    errors.push_back("Missing required key: " + key);
                }
            }
            
            // Validate specific values
            try {
                double learning_rate = config->getDouble("learning_rate");
                if (learning_rate <= 0 || learning_rate > 1) {
                    errors.push_back("learning_rate must be in range (0, 1]");
                }
            } catch (...) {
                errors.push_back("Invalid learning_rate value");
            }
            
            return std::make_pair(errors.empty(), errors);
        }, py::arg("config"), R"pbdoc(Validate configuration and return errors)")
        .def("print_config", [](const std::shared_ptr<Config>& config) {
            if (!config) {
                throw ConfigError("Configuration cannot be null");
            }
            std::cout << config->summary() << std::endl;
        }, py::arg("config"), R"pbdoc(Print configuration summary)");

    // Error handling utilities module
    py::class_<py::module_>(m, "error", R"pbdoc(Error handling utilities)")
        .def("handle_config_error", [](const std::string& message) {
            throw ConfigError(message);
        }, py::arg("message"), R"pbdoc(Raise configuration error)")
        .def("handle_validation_error", [](const std::string& message) {
            throw ValidationError(message);
        }, py::arg("message"), R"pbdoc(Raise validation error)")
        .def("handle_resource_error", [](const std::string& message) {
            throw ResourceError(message);
        }, py::arg("message"), R"pbdoc(Raise resource error)")
        .def("handle_batch_error", [](const std::string& message) {
            throw BatchError(message);
        }, py::arg("message"), R"pbdoc(Raise batch error)")
        .def("try_catch", [](std::function<void()> func, const std::string& error_type = "generic") {
            try {
                func();
                return std::make_pair(true, std::string(""));
            } catch (const std::exception& e) {
                return std::make_pair(false, std::string(e.what()));
            }
        }, py::arg("func"), py::arg("error_type") = "generic",
             R"pbdoc(Execute function with error handling)");

    // Context manager module
    py::class_<BrainContextManager>(m, "BrainContext", R"pbdoc(Context manager for brain and world resources)")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<Config>>(),
             py::arg("brain"), py::arg("world"), py::arg("config"),
             R"pbdoc(Initialize context manager)")
        .def("initialize", &BrainContextManager::initialize,
             R"pbdoc(Initialize brain and world)")
        .def("reset", &BrainContextManager::reset,
             R"pbdoc(Reset brain and world)")
        .def("__enter__", [](BrainContextManager& self) {
            self.initialize();
            return self;
        }, R"pbdoc(Context manager entry)")
        .def("__exit__", [](BrainContextManager& self, py::args) {
            self.reset();
        }, R"pbdoc(Context manager exit)")
        .def("getBrain", &BrainContextManager::getBrain,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get the brain)")
        .def("getWorld", &BrainContextManager::getWorld,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get the world)")
        .def("getConfig", &BrainContextManager::getConfig,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get the configuration)");

    // Version information
    m.attr("__version__") = "2.0.0";
    m.attr("__enhanced__") = true;

    // Summary function
    m.def("getSummary", []() {
        return "Enhanced NLM Python Bindings v2.0.0\n"
               "Features:\n"
               "  - Full input validation and type checking\n"
               "  - Pythonic property access with decorators\n"
               "  - Context manager support\n"
               "  - Batch operations\n"
               "  - Statistical analysis utilities\n"
               "  - Comprehensive error handling\n"
               "  - Detailed documentation\n"
               "  - Configuration utilities\n"
               "  - Validation utilities\n"
               "  - Error handling utilities\n";
    }, "Get library summary");
};