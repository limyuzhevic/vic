#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <filesystem>

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

// Python utility functions
namespace py_utils {
    // Validate file path exists and is readable
    std::string validateFilepath(const std::string& filepath) {
        std::filesystem::path path(filepath);
        if (!std::filesystem::exists(path)) {
            throw std::runtime_error("File does not exist: " + filepath);
        }
        if (!std::filesystem::is_regular_file(path)) {
            throw std::runtime_error("Path is not a regular file: " + filepath);
        }
        if (!std::filesystem::is_readable(path)) {
            throw std::runtime_error("File is not readable: " + filepath);
        }
        return filepath;
    }
    
    // Validate neuron ID
    NeuronId validateNeuronId(const NeuronId& id) {
        if (id.value == UINT64_MAX) {
            throw std::runtime_error("Invalid neuron ID (sentinel value)");
        }
        return id;
    }
    
    // Validate membrane potential range
    float validateMembranePotential(float current, float min = -100.0f, float max = 100.0f) {
        if (current < min || current > max) {
            throw std::runtime_error("Membrane potential " + std::to_string(current) + 
                                   " is out of range [" + std::to_string(min) + ", " + 
                                   std::to_string(max) + "]");
        }
        return current;
    }
    
    // Convert ConfigValue to Python object with type safety
    py::object convertConfigValue(const ConfigValue& value) {
        return py::detail::cast_to_python(value);
    }
    
    // Safe get with type checking and conversion
    template<typename T>
    T getTypedConfigValue(const std::string& key, const std::optional<T>& valueOpt) {
        if (!valueOpt.has_value()) {
            throw std::runtime_error("Configuration key '" + key + "' not found");
        }
        return valueOpt.value();
    }
}

// Core Types Binding
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        Provides classes for:
        - Core: Config, NeuronId, SynapseId, RegionId, PopulationId
        - Brain: Brain, NeuralRegion, NeuralPopulation
        - Agent: AgentBrain, AgentBody
        - World: SimpleWorld, WorldObject
        - Sensory: Vision, Audio, InternalSignals
        - Motor: Action, MotorCommand
        
        Features:
        - Pythonic snake_case API
        - Type conversion utilities
        - Comprehensive parameter validation
        - Detailed docstrings with examples
        - Efficient memory management
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ========================================================================
    // CORE TYPES
    // ========================================================================
    
    // NeuronId - Unique identifier for a neuron
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron
        
        Provides safe integer-based indexing and comparison operations.
        Examples:
            >>> id1 = NeuronId(0)
            >>> id2 = NeuronId()
            >>> id1 == id2
            False
            >>> hash(id1)
            0
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create a NeuronId from a 64-bit integer value")
        .def_readwrite("value", &NeuronId::value,
                      "Internal integer value of the neuron ID")
        .def("index", &NeuronId::index,
             "Get the neuron index as a 64-bit integer")
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__lt__", &NeuronId::operator<)
        .def("__le__", &NeuronId::operator<=)
        .def("__gt__", &NeuronId::operator>)
        .def("__ge__", &NeuronId::operator>=)
        .def("__hash__", [](const NeuronId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        });

    // SynapseId - Unique identifier for a synapse
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse
        
        Provides safe integer-based indexing and comparison operations.
        Examples:
            >>> id = SynapseId(42)
            >>> id.value
            42
            >>> id.index()
            42
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create a SynapseId from a 64-bit integer value")
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    // RegionId - Unique identifier for a brain region
    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region
        
        Provides safe integer-based indexing and comparison operations.
        Examples:
            >>> region_id = RegionId(1)
            >>> int(region_id.value)
            1
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create a RegionId from a 64-bit integer value")
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    // PopulationId - Unique identifier for a neuron population
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population
        Examples:
            >>> pop_id = PopulationId(5)
            >>> print(f"Population ID: {pop_id}")
            Population ID: PopulationId(5)
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create a PopulationId from a 64-bit integer value")
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Type Enums
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Neuron type enumeration for different neuron classes
        
        Values:
        - Excitatory: Stimulates other neurons
        - Inhibitory: Suppresses other neurons  
        - Modulatory: Affects synaptic plasticity
        - Sensory: Processes external stimuli
        - Motor: Produces motor output
        - Internal: Internal processing neurons
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Stimulating neuron")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibiting neuron")
        .value("Modulatory", NeuronType::Modulatory, "Modulates plasticity")
        .value("Sensory", NeuronType::Sensory, "Sensory processing")
        .value("Motor", NeuronType::Motor, "Motor output")
        .value("Internal", NeuronType::Internal, "Internal processing");

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Synapse type enumeration for connection types
        
        Values:
        - Excitatory: Increases postsynaptic neuron activity
        - Inhibitory: Decreases postsynaptic neuron activity
        - Modulatory: Modulates synaptic strength
        - Electrical: Direct electrical coupling
        - GapJunction: Electrical synapse
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction);

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Developmental stage enumeration for brain maturation
        
        Values:
        - Initial: Initial state
        - CriticalPeriod: Critical learning period
        - Maturation: Maturation phase
        - Adult: Adult functionality
        - Aging: Age-related changes
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging);

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Neuron firing state enumeration
        
        Values:
        - Resting: Not firing
        - Active: Currently firing
        - Refractory: Cannot fire temporarily
        - Inhibited: Suppressed from firing
    )pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited);

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Action type enumeration for motor behaviors
        
        Values:
        - MoveForward: Move forward
        - MoveBackward: Move backward
        - MoveLeft: Move left
        - MoveRight: Move right
        - TurnLeft: Turn left
        - TurnRight: Turn right
        - Look: Look around
        - LookUp: Look up
        - LookDown: Look down
        - Interact: Interact with objects
        - Eat: Consume food
        - Drink: Consume water
        - Rest: Rest/stay
        - Wait: Wait
        - Custom: Custom action
    )pbdoc")
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
        .value("Custom", ActionType::Custom);

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Low-level motor command enumeration
        
        Values:
        - MoveForward: Move forward
        - MoveBackward: Move backward
        - TurnLeft: Turn left
        - TurnRight: Turn right
        - LookLeft: Look left
        - LookRight: Look right
        - Interact: Interact
        - Wait: Wait
    )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait);

    // ========================================================================
    // CONFIGURATION CLASS
    // ========================================================================
    
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system
        
        Provides key-value storage with JSON file I/O, command-line parsing,
        and type-safe value access. Supports multiple configuration sources
        (file, command line, runtime) with precedence handling.
        
        Examples:
            >>> config = Config()
            >>> config.set("learning_rate", 0.01)
            >>> learning_rate = config.get("learning_rate")
            >>> if learning_rate:
            ...     print(f"Learning rate: {learning_rate}")
            
            >>> config.load_from_file("config.json")
            >>> config.set("brain_width", 100)
            >>> config.save_to_file("updated_config.json")
    )pbdoc")
        .def(py::init<>())
        
        // Load configuration from file (JSON format)
        .def("load_from_file", 
             [](Config& self, const std::string& filepath) {
                 py_utils::validateFilepath(filepath);
                 bool success = self.loadFromFile(filepath);
                 if (!success) {
                     throw std::runtime_error("Failed to load configuration from file: " + filepath);
                 }
                 return success;
             },
             py::arg("filepath"),
             R"pbdoc(
             Load configuration from a JSON file.
             
             Args:
                 filepath: Path to the JSON configuration file
                 
             Returns:
                 bool: True if successful, False otherwise
                 
             Raises:
                 RuntimeError: If file doesn't exist, isn't readable, or parsing fails
                 
             Examples:
                 >>> config = Config()
                 >>> config.load_from_file("config.json")
             )pbdoc")
        
        // Load from command line arguments
        .def("load_from_args", 
             &Config::loadFromArgs,
             py::arg("argc"), py::arg("argv"),
             R"pbdoc(
             Load configuration from command line arguments.
             
             Parses standard NLM command line options (e.g., --learning_rate 0.01)
             and updates configuration accordingly.
             
             Args:
                 argc: Number of command line arguments
                 argv: Command line argument vector
                 
             Returns:
                 bool: True if successful, False otherwise
                 
             Examples:
                 >>> config = Config()
                 >>> config.load_from_args(len(sys.argv), sys.argv)
             )pbdoc")
        
        // Save to file
        .def("save_to_file",
             [](Config& self, const std::string& filepath) {
                 py_utils::validateFilepath(filepath);
                 bool success = self.saveToFile(filepath);
                 if (!success) {
                     throw std::runtime_error("Failed to save configuration to file: " + filepath);
                 }
                 return success;
             },
             py::arg("filepath"),
             R"pbdoc(
             Save configuration to a JSON file.
             
             Args:
                 filepath: Path to save the JSON configuration file
                 
             Returns:
                 bool: True if successful, False otherwise
                 
             Examples:
                 >>> config = Config()
                 >>> config.set("neural_width", 100)
                 >>> config.save_to_file("config.json")
             )pbdoc")
        
        // Get value with type safety
        .def("get", 
             [](Config& self, const std::string& key) {
                 std::optional<int> intVal = self.get<int>(key);
                 if (intVal.has_value()) return py::int_(intVal.value());
                 std::optional<double> doubleVal = self.get<double>(key);
                 if (doubleVal.has_value()) return py::float_(doubleVal.value());
                 std::optional<bool> boolVal = self.get<bool>(key);
                 if (boolVal.has_value()) return py::bool_(boolVal.value());
                 std::optional<std::string> stringVal = self.get<std::string>(key);
                 if (stringVal.has_value()) return py::str(stringVal.value());
                 return py::none();
             },
             py::arg("key"),
             R"pbdoc(
             Get configuration value with automatic type detection.
             
             Args:
                 key: Configuration key to retrieve
                 
             Returns:
                 Value or None if key doesn't exist. Returns int, float, bool,
                 or str based on the underlying value type.
                 
             Raises:
                 RuntimeError: If key doesn't exist
                 
             Examples:
                 >>> config = Config()
                 >>> config.set("max_neurons", 100)
                 >>> max_neurons = config.get("max_neurons")
                 >>> print(f"Max neurons: {max_neurons}")
                 >>> print(f"Type: {type(max_neurons)}")
             )pbdoc")
        
        // Get with default value
        .def("get_or",
             [](Config& self, const std::string& key, const py::object& defaultValue) {
                 if (defaultValue.is_none()) {
                     throw std::runtime_error("Default value cannot be None");
                 }
                 
                 if (defaultValue.is_int()) {
                     return self.getOr<int>(key, defaultValue.cast<int>());
                 } else if (defaultValue.is_float()) {
                     return self.getOr<double>(key, defaultValue.cast<double>());
                 } else if (defaultValue.is_bool()) {
                     return self.getOr<bool>(key, defaultValue.cast<bool>());
                 } else {
                     return self.getOr<std::string>(key, defaultValue.cast<std::string>());
                 }
             },
             py::arg("key"), py::arg("default_value"),
             R"pbdoc(
             Get configuration value with fallback to default.
             
             Args:
                 key: Configuration key to retrieve
                 default_value: Value to return if key doesn't exist
                 
             Returns:
                 Configuration value or default value
                 
             Raises:
                 RuntimeError: If default_value is None
                 
             Examples:
                 >>> config = Config()
                 >>> max_neurons = config.get_or("max_neurons", 100)
                 >>> print(f"Max neurons: {max_neurons}")
             )pbdoc")
        
        // Set value with multiple type support
        .def("set", 
             [](Config& self, const std::string& key, const py::object& value) {
                 if (value.is_none()) {
                     throw std::runtime_error("Cannot set value to None");
                 }
                 
                 if (value.is_int()) {
                     self.set(key, value.cast<int>());
                 } else if (value.is_float()) {
                     self.set(key, value.cast<double>());
                 } else if (value.is_bool()) {
                     self.set(key, value.cast<bool>());
                 } else {
                     self.set(key, value.cast<std::string>());
                 }
             },
             py::arg("key"), py::arg("value"),
             R"pbdoc(
             Set configuration value with Python type support.
             
             Args:
                 key: Configuration key to set
                 value: Value to set (int, float, bool, or str)
                 
             Examples:
                 >>> config = Config()
                 >>> config.set("learning_rate", 0.01)
                 >>> config.set("enabled", True)
                 >>> config.set("name", "neural_net")
             )pbdoc")
        
        .def("set_with_source",
             [](Config& self, const std::string& key, const py::object& value, 
                const std::string& source) {
                 if (value.is_none()) {
                     throw std::runtime_error("Cannot set value to None");
                 }
                 
                 ConfigSource configSource;
                 if (source == "Default") configSource = ConfigSource::Default;
                 else if (source == "File") configSource = ConfigSource::File;
                 else if (source == "CommandLine") configSource = ConfigSource::CommandLine;
                 else configSource = ConfigSource::Runtime;
                 
                 if (value.is_int()) {
                     self.set(key, value.cast<int>(), configSource);
                 } else if (value.is_float()) {
                     self.set(key, value.cast<double>(), configSource);
                 } else if (value.is_bool()) {
                     self.set(key, value.cast<bool>(), configSource);
                 } else {
                     self.set(key, value.cast<std::string>(), configSource);
                 }
             },
             py::arg("key"), py::arg("value"), py::arg("source"),
             R"pbdoc(
             Set configuration value with explicit source.
             
             Args:
                 key: Configuration key to set
                 value: Value to set (int, float, bool, or str)
                 source: Configuration source ("Default", "File", "CommandLine", "Runtime")
                 
             Examples:
                 >>> config = Config()
                 >>> config.set_with_source("learning_rate", 0.01, "File")
             )pbdoc")
        
        // Pythonic snake_case aliases
        .def("load_from_file", 
             [](Config& self, const std::string& filepath) {
                 py_utils::validateFilepath(filepath);
                 bool success = self.loadFromFile(filepath);
                 if (!success) {
                     throw std::runtime_error("Failed to load configuration from file: " + filepath);
                 }
                 return success;
             },
             py::arg("filepath"),
             "Pythonic alias for loadFromFile")
        
        .def("save_to_file",
             [](Config& self, const std::string& filepath) {
                 py_utils::validateFilepath(filepath);
                 bool success = self.saveToFile(filepath);
                 if (!success) {
                     throw std::runtime_error("Failed to save configuration to file: " + filepath);
                 }
                 return success;
             },
             py::arg("filepath"),
             "Pythonic alias for saveToFile")
        
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists (Pythonic alias: key_exists)")
        
        .def("key_exists",
             &Config::has, py::arg("key"),
             "Pythonic alias for has")
        
        .def("get_keys", &Config::getKeys,
             R"pbdoc(
             Get all configuration keys.
             
             Returns:
                 List of all configuration keys
                 
             Examples:
                 >>> config = Config()
                 >>> config.set("learning_rate", 0.01)
                 >>> config.set("neurons", 100)
                 >>> keys = config.get_keys()
                 >>> print(f"Keys: {keys}")
             )pbdoc")
        
        .def("keys",
             &Config::getKeys,
             "Pythonic alias for get_keys")
        
        .def("clear", &Config::clear,
             R"pbdoc(
             Clear all configuration entries.
             
             Examples:
                 >>> config = Config()
                 >>> config.set("temp", 1)
                 >>> config.clear()
                 >>> print(config.keys())
                 []
             )pbdoc")
        
        .def("items", 
             [](const Config& self) {
                 py::list items;
                 auto keys = self.getKeys();
                 for (const auto& key : keys) {
                     py::object value = py::none();
                     std::optional<int> intVal = self.get<int>(key);
                     if (intVal.has_value()) {
                         value = py::int_(intVal.value());
                     } else {
                         std::optional<double> doubleVal = self.get<double>(key);
                         if (doubleVal.has_value()) {
                             value = py::float_(doubleVal.value());
                         } else {
                             std::optional<bool> boolVal = self.get<bool>(key);
                             if (boolVal.has_value()) {
                                 value = py::bool_(boolVal.value());
                             } else {
                                 std::optional<std::string> stringVal = self.get<std::string>(key);
                                 if (stringVal.has_value()) {
                                     value = py::str(stringVal.value());
                                 }
                             }
                         }
                     }
                     items.append(py::make_tuple(key, value));
                 }
                 return items;
             },
             R"pbdoc(
             Get all configuration as a list of (key, value) tuples.
             
             Returns:
                 List of (key, value) tuples for all configuration entries
                 
             Examples:
                 >>> config = Config()
                 >>> config.set("learning_rate", 0.01)
                 >>> config.set("neurons", 100)
                 >>> items = config.items()
                 >>> for key, value in items:
                 ...     print(f"{key}: {value}")
             )pbdoc")
        
        .def("update",
             [](Config& self, const py::dict& updates) {
                 py::list errors;
                 for (auto item : updates) {
                     std::string key = py::str(item.first);
                     py::object value = item.second;
                     try {
                         if (value.is_none()) {
                             throw std::runtime_error("Value cannot be None");
                         }
                         if (value.is_int()) {
                             self.set(key, value.cast<int>());
                         } else if (value.is_float()) {
                             self.set(key, value.cast<double>());
                         } else if (value.is_bool()) {
                             self.set(key, value.cast<bool>());
                         } else {
                             self.set(key, value.cast<std::string>());
                         }
                     } catch (const std::exception& e) {
                         errors.append(py::str(e.what()));
                     }
                 }
                 if (!errors.empty()) {
                     throw std::runtime_error("Errors updating configuration: " + 
                                           py::str(errors[0]));
                 }
                 return true;
             },
             py::arg("updates"),
             R"pbdoc(
             Update configuration from a dictionary.
             
             Args:
                 updates: Dictionary of key-value pairs to update
                 
             Returns:
                 bool: True if successful
                 
             Raises:
                 RuntimeError: If any update fails
                 
             Examples:
                 >>> config = Config()
                 >>> config.update({"learning_rate": 0.01, "neurons": 100})
             )pbdoc")
        
        .def("merge",
             [](Config& self, const Config& other) {
                 auto keys = other.getKeys();
                 for (const auto& key : keys) {
                     std::optional<int> intVal = other.get<int>(key);
                     if (intVal.has_value()) {
                         self.set(key, intVal.value());
                     } else {
                         std::optional<double> doubleVal = other.get<double>(key);
                         if (doubleVal.has_value()) {
                             self.set(key, doubleVal.value());
                         } else {
                             std::optional<bool> boolVal = other.get<bool>(key);
                             if (boolVal.has_value()) {
                                 self.set(key, boolVal.value());
                             } else {
                                 std::optional<std::string> stringVal = other.get<std::string>(key);
                                 if (stringVal.has_value()) {
                                     self.set(key, stringVal.value());
                                 }
                             }
                         }
                     }
                 }
                 return true;
             },
             py::arg("other_config"),
             R"pbdoc(
             Merge another configuration into this one.
             
             Args:
                 other_config: Another Config object to merge
                 
             Returns:
                 bool: True if successful
                 
             Examples:
                 >>> config1 = Config()
                 >>> config1.set("learning_rate", 0.01)
                 >>> config2 = Config()
                 >>> config2.set("batch_size", 32)
                 >>> config1.merge(config2)
                 >>> print(config1.get("batch_size"))
                 32
             )pbdoc")
        
        .def("validate",
             [](const Config& self, const py::dict& schema) {
                 py::list errors;
                 for (auto item : schema) {
                     std::string key = py::str(item.first);
                     std::string typeStr = py::str(item.second);
                     
                     if (!self.has(key)) {
                         errors.append(py::str("Missing required key: " + key));
                         continue;
                     }
                     
                     py::object value = self.get(key);
                     if (typeStr == "int" && !value.is_none() && !value.is_int()) {
                         errors.append(py::str("Key '" + key + "' must be int"));
                     } else if (typeStr == "float" && !value.is_none() && !value.is_float()) {
                         errors.append(py::str("Key '" + key + "' must be float"));
                     } else if (typeStr == "bool" && !value.is_none() && !value.is_bool()) {
                         errors.append(py::str("Key '" + key + "' must be bool"));
                     } else if (typeStr == "str" && !value.is_none() && !value.is_str()) {
                         errors.append(py::str("Key '" + key + "' must be str"));
                     }
                 }
                 
                 if (!errors.empty()) {
                     std::string errorMsg = "Validation errors:";
                     for (size_t i = 0; i < errors.size(); ++i) {
                         errorMsg += " " + py::str(errors[i]);
                         if (i < errors.size() - 1) errorMsg += ",";
                     }
                     throw std::runtime_error(errorMsg);
                 }
                 
                 return true;
             },
             py::arg("schema"),
             R"pbdoc(
             Validate configuration against a schema.
             
             Args:
                 schema: Dictionary mapping keys to expected types as strings
                        ("int", "float", "bool", or "str")
                 
             Returns:
                 bool: True if valid
                 
             Raises:
                 RuntimeError: If validation fails
                 
             Examples:
                 >>> config = Config()
                 >>> config.set("learning_rate", 0.01)
                 >>> config.set("batch_size", 32)
                 >>> schema = {"learning_rate": "float", "batch_size": "int"}
                 >>> config.validate(schema)
                 True
             )pbdoc")
        
        .def("summary",
             &Config::summary,
             R"pbdoc(
             Get a summary string of the configuration.
             
             Returns:
                 Summary string with configuration statistics
                 
             Examples:
                 >>> config = Config()
                 >>> config.set("learning_rate", 0.01)
                 >>> config.set("batch_size", 32)
                 >>> print(config.summary())
                 "Config: 2 keys, max value type: double"
             )pbdoc")
        
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // ========================================================================
    // SENSORY INPUT CLASSES
    // ========================================================================
    
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for sensory input
        
        Represents sensory data from environment or internal brain state.
    )pbdoc")
        .def("get_type", &SensoryInput::getType,
             "Get the type of sensory input")
        .def("get_data", &SensoryInput::getData,
             "Get the raw data as a vector of floats")
        .def("get_dimensions", &SensoryInput::getDimensions,
             "Get the dimensionality of the sensory input")
        .def("get_timestamp", &SensoryInput::getTimestamp,
             "Get the timestamp of the sensory input")
        .def("set_timestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp of the sensory input");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input representing visual data
        
        Provides image-like input with width, height, and channel information.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3,
             "Create Vision input with specified dimensions")
        .def("set_data", 
             [](Vision& self, const std::vector<float>& data) {
                 self.setData(data);
             },
             py::arg("data"),
             "Set vision data as a vector of floats")
        .def("get_width", &Vision::getWidth,
             "Get image width in pixels")
        .def("get_height", &Vision::getHeight,
             "Get image height in pixels")
        .def("get_channels", &Vision::getChannels,
             "Get number of color channels (typically 1, 3, or 4)");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input representing sound data
        
        Provides audio samples with sample rate and duration information.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sample_rate"), py::arg("num_samples"),
             "Create Audio input with specified sample rate and duration")
        .def("set_data",
             [](Audio& self, const std::vector<float>& data) {
                 self.setData(data);
             },
             py::arg("data"),
             "Set audio data as a vector of float samples")
        .def("set_sample_rate", &Audio::setSampleRate, py::arg("sample_rate"),
             "Set the audio sample rate (samples per second)")
        .def("get_sample_rate", &Audio::getSampleRate,
             "Get the audio sample rate")
        .def("get_num_samples", &Audio::getNumSamples,
             "Get the number of audio samples");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal brain signals as sensory input
        
        Represents internal neural states and neuromodulatory signals.
    )pbdoc")
        .def(py::init<>())
        .def("add_signal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value to the internal signals")
        .def("clear_signals", &InternalSignals::clearSignals,
             "Clear all internal signals");

    // ========================================================================
    // MOTOR OUTPUT CLASSES
    // ========================================================================
    
    py::class_<Action>(m, "Action", R"pbdoc(
        Action representation for motor output
        
        Represents motor commands with type and parameters for behavior execution.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create Action with specified type")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create Action with type and parameters")
        .def("get_type", &Action::getType,
             "Get the action type")
        .def("set_type", &Action::setType, py::arg("type"),
             "Set the action type")
        .def("get_parameters", &Action::getParameters,
             "Get action parameters as a list of floats")
        .def("set_parameters", &Action::setParameters, py::arg("params"),
             "Set action parameters")
        .def("get_name", &Action::getName,
             "Get human-readable action name")
        .def("clone", &Action::clone,
             "Create a copy of this action");

    // ========================================================================
    // WORLD CLASS
    // ========================================================================
    
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        World object representation
        
        Represents an object in the 2D world with position, type, and properties.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Create a world object with position, type, and properties")
        .def_readwrite("x", &WorldObject::x, "X position in world coordinates")
        .def_readwrite("y", &WorldObject::y, "Y position in world coordinates")
        .def_readwrite("radius", &WorldObject::radius, "Object radius")
        .def_readwrite("type", &WorldObject::type, "Object type (WorldObjectType enum)")
        .def_readwrite("value", &WorldObject::value, "Object value (reward/damage)")
        .def_readwrite("active", &WorldObject::active, "Whether object is active")
        .def("__repr__", [](const WorldObject& obj) {
            return "<WorldObject: " + std::to_string(obj.x) + ", " + 
                   std::to_string(obj.y) + ", " + 
                   py::cast(static_cast<int>(obj.type)) + 
                   ">";
        });

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state representation
        
        Contains position, orientation, velocity, and physiological state.
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position")
        .def_readwrite("y", &AgentBody::y, "Y position")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation in radians")
        .def_readwrite("velocity_x", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocity_y", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angular_velocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Energy level (0.0-1.0)")
        .def_readwrite("health", &AgentBody::health, "Health level (0.0-1.0)")
        .def_readwrite("age", &AgentBody::age, "Age in simulation steps")
        .def_readwrite("is_moving", &AgentBody::isMoving, "Whether agent is moving")
        .def_readwrite("is_turning", &AgentBody::isTurning, "Whether agent is turning")
        .def_readwrite("last_action_time", &AgentBody::lastActionTime, "Last action time")
        .def("reset", &AgentBody::reset, "Reset body to initial state");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Result from applying a motor command to the world
        
        Contains reward, success status, and explanatory message.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create ActionResult with reward, success, and message")
        .def_readwrite("reward", &ActionResult::reward, "Reward value (can be negative)")
        .def_readwrite("success", &ActionResult::success, "Whether action succeeded")
        .def_readwrite("message", &ActionResult::message, "Explanatory message");

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation
        
        Provides a basic environment with physics, objects, and sensory feedback.
    )pbdoc")
        .def(py::init<>())
        .def("configure",
             &SimpleWorld::configure,
             py::arg("width"), py::arg("height"), py::arg("vision_width"), py::arg("vision_height"),
             "Configure world dimensions and agent vision")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("set_agent_start", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent's starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world state")
        .def("apply_motor_command",
             &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("current_time"),
             "Apply motor command and get result")
        .def("get_sensory_percept",
             &SimpleWorld::getSensoryPercept,
             py::return_value_policy::copy,
             "Get current sensory percept for the agent")
        .def("get_agent_body",
             &SimpleWorld::getAgentBody,
             py::return_value_policy::copy,
             "Get agent body state")
        .def("add_object", &SimpleWorld::addObject, py::arg("obj"), "Add object to world")
        .def("remove_object", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("is_valid_position", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not a wall)")
        .def("get_object_at", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             "Get object at position (if any)")
        .def("get_width", &SimpleWorld::getWidth, "Get world width")
        .def("get_height", &SimpleWorld::getHeight, "Get world height")
        .def("get_max_energy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum energy")
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("get_simulation_time", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("set_random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("get_random_seed", &SimpleWorld::getRandomSeed, "Get random seed");

    // ========================================================================
    // BRAIN CLASS
    // ========================================================================
    
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class
        
        Implements real spiking neural computation with event-driven dynamics
        and integrated memory, prediction, cognition, and neuromodulation systems.
        
        The Brain is the core component that processes sensory input, maintains
        internal state, and produces motor actions through neural computation.
        
        Examples:
            >>> config = Config()
            >>> config.set("brain_width", 100)
            >>> brain = Brain(config)
            >>> brain.initialize()
            >>> 
            >>> # Process sensory input
            >>> vision = Vision(84, 84, 3)
            >>> # ... set vision data ...
            >>> percept = brain.get_sensory_percept(vision)
            >>> 
            >>> # Step simulation
            >>> brain.step(current_step)
            >>> 
            >>> # Get action
            >>> action = brain.produce_action()
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def("initialize",
             &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step",
             static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("current_step"),
             "Perform a simulation step with current step number")
        .def("step",
             static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("current_step"), py::arg("current_time"),
             "Perform a simulation step with timestamp")
        .def("receive_sensory_input",
             &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("inject_current",
             &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current directly into a specific neuron")
        .def("inject_current_to_neurons",
             &Brain::injectCurrentToNeurons,
             py::arg("neuron_type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produce_action",
             &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset",
             &Brain::reset,
             "Reset brain state")
        .def("save",
             &Brain::save,
             py::arg("filepath"),
             "Save brain state to file (checkpointing)")
        .def("load",
             &Brain::load,
             py::arg("filepath"),
             "Load brain state from file")
        .def("add_region",
             &Brain::addRegion,
             py::arg("name") = "",
             "Add a new neural region")
        .def("get_region",
             &Brain::getRegion,
             py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("get_region_count",
             &Brain::getRegionCount,
             "Get the number of regions")
        .def("get_region_ids",
             &Brain::getRegionIds,
             "Get all region IDs")
        .def("get_regions",
             &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("get_total_neuron_count",
             &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("get_total_synapse_count",
             &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("get_active_neuron_count",
             &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("get_firing_neuron_count",
             &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("get_average_firing_rate",
             &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("get_excitation_inhibition_ratio",
             &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("get_total_spike_count",
             &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("get_developmental_stage",
             &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("set_developmental_stage",
             &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("get_config",
             &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("log_status",
             &Brain::logStatus,
             "Log brain status to console");

    // ========================================================================
    // AGENT BRAIN CLASS
    // ========================================================================
    
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world
        
        Handles sensory transduction and motor decoding for agent-environment interaction.
        
        AgentBrain bridges the gap between the low-level neural simulation in Brain
        and the high-level agent behavior in the world. It processes sensory input,
        maintains internal state, and produces motor commands for action execution.
        
        Examples:
            >>> config = Config()
            >>> brain = Brain(config)
            >>> agent_brain = AgentBrain(brain)
            >>> agent_brain.initialize(world)
            >>> 
            >>> # Process environment
            >>> percept = agent_brain.get_sensory_percept()
            >>> action = agent_brain.decode_motor_command()
            >>> 
            >>> # Apply action
            >>> result = world.apply_motor_command(action, current_time)
            >>> 
            >>> # Update brain with reward
            >>> agent_brain.apply_reward_modulation(result.reward, predicted_reward)
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain with underlying neural brain")
        .def("initialize",
             &AgentBrain::initialize,
             py::arg("world"),
             "Initialize with world")
        .def("get_sensory_input_size",
             &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("get_motor_output_size",
             &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("process_sensory_input",
             &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decode_motor_command",
             &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("apply_reward_modulation",
             &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predicted_reward"),
             "Apply reward-based neuromodulation")
        .def("update_development",
             &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("get_developmental_stage",
             &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("get_neuromodulation_level",
             &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("get_curiosity_level",
             &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("get_novelty_level",
             &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("get_prediction_error",
             &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset",
             &AgentBrain::reset,
             "Reset agent for new episode")
        .def("get_brain",
             &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enable_reward_modulation",
             &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             "Enable or disable reward modulation")
        .def("enable_structural_plasticity",
             &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable or disable structural plasticity")
        .def("enable_development",
             &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable or disable development")
        .def("enable_curiosity",
             &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable or disable curiosity-driven exploration")
        .def("is_reward_modulation_enabled",
             &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("is_structural_plasticity_enabled",
             &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("is_development_enabled",
             &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("is_curiosity_enabled",
             &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled");

    // ========================================================================
    // FACTORY FUNCTIONS
    // ========================================================================
    
    m.def("create_default_config",
          []() -> std::shared_ptr<Config> {
              return std::make_shared<Config>();
          },
          "Create a default configuration")
          .def("doc", "Create a default configuration");

    m.def("create_brain",
          [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
              return std::make_shared<Brain>(config);
          },
          py::arg("config"),
          "Create a new brain with configuration")
          .def("doc", "Create a new brain with configuration");

    m.def("create_simple_world",
          []() -> std::shared_ptr<SimpleWorld> {
              return std::make_shared<SimpleWorld>();
          },
          "Create a new simple world")
          .def("doc", "Create a new simple world");

    m.def("create_agent_brain",
          [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
              return std::make_shared<AgentBrain>(brain);
          },
          py::arg("brain"),
          "Create a new agent brain interface")
          .def("doc", "Create a new agent brain interface");

    // Global constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Pythonic snake_case aliases for convenience
    m.def("create_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Pythonic alias for create_default_config")
    .def("doc", "Create a config");

    m.def("create_neural_brain", 
          [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
              return std::make_shared<Brain>(config);
          },
          py::arg("config"),
          "Pythonic alias for create_brain")
    .def("doc", "Create a brain");

    m.def("create_simulation_world",
          []() -> std::shared_ptr<SimpleWorld> {
              return std::make_shared<SimpleWorld>();
          },
          "Pythonic alias for create_simple_world")
    .def("doc", "Create a world");
}

} // namespace nlm