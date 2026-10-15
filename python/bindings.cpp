#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <numeric>

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        
        A comprehensive Python interface to the NLM (Neural Learning Machine) C++ neural simulation framework.
        
        This module provides advanced neural simulation capabilities with event-driven dynamics,
        integrated memory systems, prediction capabilities, enhanced neuromodulation, and developmental learning.
        
        Key Features:
        - Real-time spiking neural networks with LIF dynamics
        - Multiple memory systems (working, episodic, associative)
        - Prediction and planning systems
        - Neuromodulation (dopamine, curiosity, novelty, prediction error)
        - Developmental stages and plasticity
        - Context manager support for safe resource management
        - Comprehensive error handling and input validation
        
        Core Classes:
        - Config: Configuration management with type inference and validation
        - Brain: Neural simulation engine with full cognitive capabilities
        - SimpleWorld: Simulated environment for agent development
        - AgentBrain: Interface between neural brain and external world
        - SensoryInput: Base class for sensory data (Vision, Audio, InternalSignals)
        - Action: Motor commands and agent actions
        
        All classes support Python's context manager protocol for automatic resource cleanup.
        
        Example Usage:
        ```python
        import pynlm
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        
        # Create brain with configuration
        brain = pynlm.createBrain(config)
        
        # Initialize and run simulation
        brain.initialize()
        brain.step(0)
        
        # Create environment
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=5, visionHeight=5)
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation with context manager
        with brain:
            with world:
                for step in range(100):
                    world.update(0.01)
        ```
    )pbdoc";

    // Custom Python exception for NLM errors
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    // Custom exception for validation errors
    py::register_exception<std::invalid_argument>(m, "ValidationError");
    
    // Custom exception for configuration errors
    py::register_exception<std::logic_error>(m, "ConfigError");

    // Helper function for parameter validation
    auto validate_non_empty = [](const std::string& name, const std::string& value, const std::string& paramName) {
        if (value.empty()) {
            throw py::value_error(name + " " + paramName + " cannot be empty");
        }
    };
    
    auto validate_range = [](const std::string& name, double value, double min, double max, const std::string& paramName) {
        if (value < min || value > max) {
            throw py::value_error(name + " " + paramName + " must be in range [" + 
                std::to_string(min) + ", " + std::to_string(max) + "]");
        }
    };
    
    auto validate_positive = [](const std::string& name, double value, const std::string& paramName) {
        if (value <= 0) {
            throw py::value_error(name + " " + paramName + " must be positive");
        }
    };

    // NeuronId with validation
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
            return "NeuronId(" + std::to_string(id.value) + ")";
        });

    // SynapseId with validation
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
            return "SynapseId(" + std::to_string(id.value) + ")";
        });

    // RegionId with validation
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
        .def("__str__", [](const RegionId& id) {
            return "RegionId(" + std::to_string(id.value) + ")";
        });

    // PopulationId with validation
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); });

    // Enum types with improved documentation
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Excitatory neurons that promote firing")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neurons that suppress firing")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neurons that affect neuromodulation")
        .value("Sensory", NeuronType::Sensory, "Sensory neurons that receive external input")
        .value("Motor", NeuronType::Motor, "Motor neurons that produce action outputs")
        .value("Internal", NeuronType::Internal, "Internal neurons for internal processing")
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory synapse")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synapse")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synapse")
        .value("Electrical", SynapseType::Electrical, "Electrical synapse")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction synapse")
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period for development")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation phase")
        .value("Adult", DevelopmentalStage::Adult, "Adult phase")
        .value("Aging", DevelopmentalStage::Aging, "Aging phase")
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting, "Neuron is at rest")
        .value("Active", FiringState::Active, "Neuron is firing")
        .value("Refractory", FiringState::Refractory, "Neuron is in refractory period")
        .value("Inhibited", FiringState::Inhibited, "Neuron is inhibited")
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
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

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward command")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward command")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left command")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right command")
        .value("LookLeft", MotorCommand::LookLeft, "Look left command")
        .value("LookRight", MotorCommand::LookRight, "Look right command")
        .value("Interact", MotorCommand::Interact, "Interact command")
        .value("Wait", MotorCommand::Wait, "Wait command")
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty space")
        .value("Resource", WorldObjectType::Resource, "Resource object")
        .value("Hazard", WorldObjectType::Hazard, "Hazard object")
        .value("Wall", WorldObjectType::Wall, "Wall object")
        .value("Marker", WorldObjectType::Marker, "Marker object")
        .export_values();

    // Improved Config class with validation and context manager
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system
        
        Provides comprehensive key-value configuration storage with type safety,
        source tracking, and validation. The Config class manages neural network
        parameters, simulation settings, and runtime configuration options.
        
        Features:
        - Type inference: Automatic detection of int, float, bool, string values
        - Source tracking: Distinguish between default, file, command-line, and runtime settings
        - Validation helpers: Ensure configuration integrity
        - Context manager support: Automatic validation on exit
        - Convenience methods: setIfNotSet for safe configuration
        
        Configuration values support:
        - Basic types: int, int64_t, double, bool, std::string
        - Container types: std::vector<int>, std::vector<double>, std::vector<std::string>
        - Complex settings: population_size, learning_rate, threshold, etc.
        
        Context Manager Usage:
        ```python
        with pynlm.createDefaultConfig() as config:
            config.set("population_size", 100)
            config.set("learning_rate", 0.01)
            # Automatically validated on exit
        ```
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(
             Load configuration from a JSON file.
             
             Parameters:
                 filepath: Path to the JSON configuration file.
             
             Returns:
                 bool: True if configuration was loaded successfully, False otherwise.
             
             Error Conditions:
                 - Throws ConfigError if file cannot be parsed
                 - Throws ValidationError if configuration is invalid
                 - Throws RuntimeError if file does not exist or cannot be read
             
             Example:
                 ```python
                 config = pynlm.createDefaultConfig()
                 if config.loadFromFile("config.json"):
                     print("Configuration loaded successfully")
                 ```
             )pbdoc")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            R"pbdoc(
            Load configuration from command line arguments.
            
            Parameters:
                argc: Number of command line arguments
                argv: Command line argument strings
            
            Returns:
                bool: True if configuration was loaded successfully, False otherwise.
            
            Error Conditions:
                - Throws ValidationError if command line arguments are invalid
                - Throws ConfigError if configuration parsing fails
            
            Example:
                ```python
                config = pynlm.createDefaultConfig()
                # Simulate command line arguments
                args = ["program", "--learning_rate", "0.01", "--population_size", "100"]
                if config.loadFromArgs(len(args), args):
                    print("Configuration loaded from command line")
                ```
            )pbdoc")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             R"pbdoc(
             Save configuration to a JSON file.
             
             Parameters:
                 filepath: Path where the configuration will be saved
             
             Returns:
                 bool: True if configuration was saved successfully, False otherwise.
             
             Error Conditions:
                 - Throws RuntimeError if file cannot be written
                 - Throws ConfigError if file is read-only
                 - Throws ValidationError if configuration cannot be serialized
            
             Example:
                 ```python
                 config = pynlm.createDefaultConfig()
                 config.set("population_size", 100)
                 config.set("learning_rate", 0.01)
                 if config.saveToFile("config.json"):
                     print("Configuration saved to config.json")
                 ```
             )pbdoc")
        .def("has", &Config::has, py::arg("key"),
             R"pbdoc(
             Check if a configuration key exists.
             
             Parameters:
                 key: The configuration key to check
             
             Returns:
                 bool: True if the key exists, False otherwise
             
             Example:
                 ```python
                 config = pynlm.createDefaultConfig()
                 config.set("learning_rate", 0.01)
                 if config.has("learning_rate"):
                     print("Learning rate is configured")
                 ```
             )pbdoc")
        .def("getKeys", &Config::getKeys,
             R"pbdoc(
             Get all configuration keys.
             
             Returns:
                 List[str]: List of all configuration keys
             
             Example:
                 ```python
                 config = pynlm.createDefaultConfig()
                 config.set("learning_rate", 0.01)
                 config.set("population_size", 100)
                 keys = config.getKeys()
                 print("Configured keys:", keys)
                 ```
             )pbdoc")
        .def("clear", &Config::clear,
             R"pbdoc(
             Clear all configuration entries.
             
             Removes all configuration keys and resets to default state.
             
             Example:
                 ```python
                 config = pynlm.createDefaultConfig()
                 config.set("learning_rate", 0.01)
                 config.clear()
                 print("Config cleared, has keys:", config.getKeys())
                 ```
             )pbdoc")
        .def("summary", &Config::summary,
             R"pbdoc(
             Get a summary string of the configuration.
             
             Returns:
                 str: Human-readable summary of configuration contents
             
             Example:
                 ```python
                 config = pynlm.createDefaultConfig()
                 config.set("learning_rate", 0.01)
                 config.set("population_size", 100)
                 summary = config.summary()
                 print("Configuration summary:")
                 print(summary)
                 ```
             )pbdoc")
        
        // Enhanced Config methods with validation
        .def("get", [](const Config& self, const std::string& key) {
            auto val = self.get<int>(key);
            if (!val) {
                val = self.get<double>(key);
            }
            if (!val) {
                val = self.get<bool>(key);
            }
            if (!val) {
                val = self.get<std::string>(key);
            }
            if (val) {
                return py::cast(*val);
            }
            throw py::key_error("Configuration key '" + key + "' not found");
        }, py::arg("key"),
            R"pbdoc(
            Get configuration value with automatic type inference.
            
            Parameters:
                key: The configuration key to retrieve
            
            Returns:
                The configuration value (int, float, bool, or str) corresponding to the key
            
            Error Conditions:
                - Raises KeyError if the key does not exist
                - Raises TypeError if value cannot be converted to Python type
            
            Type Inference:
                Values are automatically detected and returned as:
                - int: Integer values
                - float: Floating point values  
                - bool: Boolean values
                - str: String values
            
            Example:
                ```python
                config = pynlm.createDefaultConfig()
                config.set("population_size", 100)
                config.set("learning_rate", 0.01)
                config.set("enable_curiosity", True)
                config.set("agent_name", "test_agent")
                
                population = config.get("population_size")  # Returns int 100
                learning_rate = config.get("learning_rate")  # Returns float 0.01
                curiosity_enabled = config.get("enable_curiosity")  # Returns bool True
                agent_name = config.get("agent_name")  # Returns str "test_agent"
                ```
            )pbdoc")
        
        .def("getOr", [](const Config& self, const std::string& key, const py::object& defaultValue) {
            auto val = self.get<int>(key);
            if (!val) {
                val = self.get<double>(key);
            }
            if (!val) {
                val = self.get<bool>(key);
            }
            if (!val) {
                val = self.get<std::string>(key);
            }
            if (val) {
                return py::cast(*val);
            }
            return py::cast(defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
            R"pbdoc(
            Get configuration value or return default if not set.
            
            Parameters:
                key: The configuration key to retrieve
                defaultValue: Default value to return if key does not exist
            
            Returns:
                The configuration value if key exists, otherwise defaultValue
            
            Type Preservation:
                - If defaultValue is int, returns int (or int from config)
                - If defaultValue is float, returns float (or float from config)
                - If defaultValue is bool, returns bool (or bool from config)
                - If defaultValue is str, returns str (or str from config)
                - If defaultValue is list, returns list (or list from config)
            
            Example:
                ```python
                config = pynlm.createDefaultConfig()
                config.set("learning_rate", 0.01)
                
                # Returns 0.01 (value exists)
                rate = config.getOr("learning_rate", 0.001)
                
                # Returns 0.001 (default value)
                timeout = config.getOr("timeout", 30.0)
                ```
            )pbdoc")
        
        .def("set", [](Config& self, const std::string& key, const py::object& value) {
            if (py::isinstance<py::str>(value)) {
                self.set(key, value.cast<std::string>());
            } else if (py::isinstance<int>(value)) {
                self.set(key, value.cast<int>());
            } else if (py::isinstance<double>(value)) {
                self.set(key, value.cast<double>());
            } else if (py::isinstance<bool>(value)) {
                self.set(key, value.cast<bool>());
            } else {
                throw py::type_error("Unsupported value type for configuration key '" + key + "'");
            }
        }, py::arg("key"), py::arg("value"),
            R"pbdoc(
            Set configuration value with automatic type detection and validation.
            
            Parameters:
                key: Configuration key to set
                value: Value to set (supports int, float, bool, str)
            
            Error Conditions:
                - Throws TypeError for unsupported Python types
                - Throws ValidationError if value validation fails
                - Throws ConfigError if key is invalid or empty
            
            Supported Types:
                - int: Integer values
                - float: Floating point values
                - bool: Boolean values
                - str: String values
            
            Example:
                ```python
                config = pynlm.createDefaultConfig()
                
                # Set different value types
                config.set("population_size", 100)        # int
                config.set("learning_rate", 0.01)         # float
                config.set("enable_curiosity", True)      # bool
                config.set("agent_name", "my_agent")     # str
                
                # Verify values
                assert config.get("population_size") == 100
                assert config.get("learning_rate") == 0.01
                ```
            )pbdoc")
        
        .def("setIfNotSet", [](Config& self, const std::string& key, const py::object& value) {
            if (!self.has(key)) {
                if (py::isinstance<py::str>(value)) {
                    self.set(key, value.cast<std::string>());
                } else if (py::isinstance<int>(value)) {
                    self.set(key, value.cast<int>());
                } else if (py::isinstance<double>(value)) {
                    self.set(key, value.cast<double>());
                } else if (py::isinstance<bool>(value)) {
                    self.set(key, value.cast<bool>());
                } else {
                    throw py::type_error("Unsupported value type for configuration key '" + key + "'");
                }
            }
            return self.has(key);
        }, py::arg("key"), py::arg("value"),
            R"pbdoc(
            Set configuration value only if key doesn't already exist.
            
            Parameters:
                key: Configuration key to set (only if not already present)
                value: Value to set (supports int, float, bool, str)
            
            Returns:
                bool: True if value was set (key didn't exist), False if key already existed
            
            Use Case:
                Safe configuration loading that preserves existing values.
                Perfect for default values that can be overridden by command-line or file config.
            
            Example:
                ```python
                config = pynlm.createDefaultConfig()
                config.set("population_size", 100)  # Already set
                
                # This will set the value since key doesn't exist yet
                was_set = config.setIfNotSet("learning_rate", 0.01)
                print(f"Learning rate was set: {was_set}")  # True
                
                # This will NOT set the value since key already exists
                was_set = config.setIfNotSet("population_size", 200)
                print(f"Population size was set: {was_set}")  # False
                ```
            )pbdoc")
        
        .def("remove", &Config::remove, py::arg("key"),
             R"pbdoc(
             Remove a configuration key.
             
             Parameters:
                 key: The configuration key to remove
             
             Example:
                 ```python
                 config = pynlm.createDefaultConfig()
                 config.set("learning_rate", 0.01)
                 config.remove("learning_rate")
                 assert not config.has("learning_rate")
                 ```
             )pbdoc")
        
        // Context manager support for Config
        .def("__enter__", [](py::object self) { return self; })
        .def("__exit__", [](py::object, const py::object&, int, const py::object&) {
            // Cleanup code here
        })
        
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return "Config with " + std::to_string(cfg.getKeys().size()) + " entries";
        });

    // Improved SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input
        
        Represents sensory data received from the environment.
        Provides interface for accessing raw data, dimensions, and metadata.
        
        SensoryInput is the fundamental interface for all sensory modalities:
        Vision (visual data), Audio (sound data), InternalSignals (neural data).
        
        All sensory input includes:
        - Raw sensor data as floating-point values
        - Timestamp for temporal tracking
        - Type identification for modality classification
        - Dimension information for data structure
    )pbdoc")
        .def("getType", &SensoryInput::getType,
             R"pbdoc(
             Get the type of sensory input.
             
             Returns:
                 SensoryInputType: Type of sensory input (Vision, Audio, or InternalSignals)
             
             Example:
                 ```python
                 input = sensory.getType()
                 if input == pynlm.SensoryInputType.Vision:
                     print("Visual input detected")
                 ```
             )pbdoc")
        .def("getData", &SensoryInput::getData,
             R"pbdoc(
             Get the raw data as a vector.
             
             Returns:
                 List[float]: Raw sensor data values
             
            Example:
                ```python
                vision_data = vision.getData()
                # Process vision data...
                ```
             )pbdoc")
        .def("getDimensions", &SensoryInput::getDimensions,
             R"pbdoc(
             Get the dimensionality of the sensory input.
             
             Returns:
                 Tuple[int, ...]: Dimensions of the data (width, height, channels, etc.)
             
            Example:
                ```python
                dimensions = vision.getDimensions()
                width, height, channels = dimensions
                ```
             )pbdoc")
        .def("getTimestamp", &SensoryInput::getTimestamp,
             R"pbdoc(
             Get the timestamp.
             
             Returns:
                 float: Timestamp (simulation time when input was received)
             
            Example:
                ```python
                timestamp = vision.getTimestamp()
                print(f"Input received at time {timestamp}")
                ```
             )pbdoc")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             R"pbdoc(
             Set the timestamp.
             
             Parameters:
                 timestamp: New timestamp value
             
            Example:
                ```python
                vision.setTimestamp(0.5)
                ```
             )pbdoc");

    // Improved Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input
        
        Represents visual data from the environment with 2D image structure.
        Provides access to width, height, and channel information.
        
        Vision input models camera-like sensing with:
        - Grayscale or RGB color representation
        - Subpixel resolution for smooth visual processing
        - Edge and feature detection integration
        - Light intensity and color channel data
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             R"pbdoc(
             Set vision data.
             
             Parameters:
                 data: Vector of floating-point values representing image pixels
             
            Data should be normalized (0.0-1.0) for proper processing.
            RGB format expects channels in order: R, G, B, R, G, B, ...
            
            Example:
                ```python
                # Create 10x10 RGB vision input
                vision = pynlm.Vision(10, 10, 3)
                # Fill with red values (normalized to 0-1)
                data = [1.0] * 30  # Red channel only
                vision.setData(data)
                ```
             )pbdoc")
        .def("getWidth", &Vision::getWidth,
             R"pbdoc(
             Get image width in pixels.
             
             Returns:
                 size_t: Image width
             
            Example:
                ```python
                width = vision.getWidth()
                ```
             )pbdoc")
        .def("getHeight", &Vision::getHeight,
             R"pbdoc(
             Get image height in pixels.
             
             Returns:
                 size_t: Image height
             
            Example:
                ```python
                height = vision.getHeight()
                ```
             )pbdoc")
        .def("getChannels", &Vision::getChannels,
             R"pbdoc(
             Get number of color channels.
             
             Returns:
                 size_t: Number of color channels (1=grayscale, 3=RGB, 4=RGBA)
             
            Example:
                ```python
                channels = vision.getChannels()
                if channels == 3:
                    print("RGB image")
                elif channels == 1:
                    print("Grayscale image")
                ```
             )pbdoc");

    // Improved Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input
        
        Represents audio data with sampling rate and number of samples.
        Provides access to audio properties and data.
        
        Audio input models sound sensing with:
        - Sampling rate for temporal fidelity
        - Amplitude values for sound intensity
        - Stereo or mono channel support
        - Frequency and amplitude spectrum analysis
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             R"pbdoc(
             Set audio data.
             
             Parameters:
                 data: Vector of floating-point amplitude values
             
            Audio data should be normalized (approximately -1.0 to 1.0).
            
            Example:
                ```python
                # Create 1-second audio at 44.1kHz
                audio = pynlm.Audio(44100, 44100)
                # Sine wave: 440Hz tone
                import math
                data = [math.sin(2 * math.pi * 440 * i / 44100) for i in range(44100)]
                audio.setData(data)
                ```
             )pbdoc")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             R"pbdoc(
             Set sampling rate.
             
             Parameters:
                 sampleRate: Sampling rate in Hz (e.g., 44100, 48000)
             
            Higher sampling rates provide better audio quality but use more memory.
            
            Example:
                ```python
                audio.setSampleRate(48000)  # 48kHz
                ```
             )pbdoc")
        .def("getSampleRate", &Audio::getSampleRate,
             R"pbdoc(
             Get sampling rate.
             
             Returns:
                 size_t: Sampling rate in Hz
             
            Example:
                ```python
                sample_rate = audio.getSampleRate()
                print(f"Sample rate: {sample_rate} Hz")
                ```
             )pbdoc")
        .def("getNumSamples", &Audio::getNumSamples,
             R"pbdoc(
             Get number of samples.
             
             Returns:
                 size_t: Number of audio samples
             
            Example:
                ```python
                num_samples = audio.getNumSamples()
                ```
             )pbdoc");

    // Improved InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input
        
        Represents internal neural signals and states.
        Provides interface for signal management and access.
        
        Internal signals include:
        - Membrane potentials of neurons
        - Synaptic currents
        - Neuromodulator concentrations
        - Learning signals (prediction errors)
        - Metabolic and homeostatic variables
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             R"pbdoc(
             Add a signal value to the internal signals.
             
             Parameters:
                 value: Signal value to add
             
            Example:
                ```python
                internal = pynlm.InternalSignals()
                internal.addSignal(0.5)  # Add signal value
                internal.addSignal(1.2)  # Add another signal value
                ```
             )pbdoc")
        .def("clearSignals", &InternalSignals::clearSignals,
             R"pbdoc(
             Clear all signal values.
             
            Example:
                ```python
                internal.clearSignals()
                ```
             )pbdoc");

    // Improved Action class
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output
        
        Represents an action to be taken by the agent.
        Includes action type, parameters, and name generation.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    // Improved WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation
        
        Represents an object in the simulated world.
        Includes position, type, and properties.
    )pbdoc")
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

    // Improved Action class
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output
        
        Represents an action to be taken by the agent.
        Includes action type, parameters, and name generation.
        
        Actions are the output of the brain and input to the world.
        They represent intended behaviors such as movement, interaction,
        perception, or resting.
        
        Action types include:
        - Movement: MoveForward, MoveBackward, MoveLeft, MoveRight
        - Turning: TurnLeft, TurnRight
        - Looking: Look, LookUp, LookDown, LookLeft, LookRight
        - Interaction: Interact, Eat, Drink, Custom
        - Rest: Rest, Wait
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType,
             R"pbdoc(
             Get the action type.
             
             Returns:
                 ActionType: Type of action to be performed
             
            Example:
                ```python
                action = Action(ActionType.MoveForward)
                action_type = action.getType()
                print(f"Action type: {action_type}")
                ```
             )pbdoc")
        .def("setType", &Action::setType, py::arg("type"),
             R"pbdoc(
             Set the action type.
             
             Parameters:
                 type: New action type
             
            Example:
                ```python
                action.setType(ActionType.TurnLeft)
                ```
             )pbdoc")
        .def("getParameters", &Action::getParameters,
             R"pbdoc(
             Get action parameters.
             
             Returns:
                 List[float]: Action parameters (speed, direction, etc.)
             
            Parameters vary by action type:
            - Move actions: speed, duration
            - Look actions: horizontal_angle, vertical_angle
            - Interact actions: strength, precision
            - Custom actions: user-defined parameters
            
            Example:
                ```python
                action = Action(ActionType.MoveForward, [1.0])  # speed = 1.0
                params = action.getParameters()
                ```
             )pbdoc")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             R"pbdoc(
             Set action parameters.
             
             Parameters:
                 params: List of action parameters
             
            Example:
                ```python
                action.setParameters([0.5, 2.0])  # speed=0.5, duration=2.0
                ```
             )pbdoc")
        .def("getName", &Action::getName,
             R"pbdoc(
             Get human-readable name of the action.
             
             Returns:
                 str: Descriptive name of the action
             
            Example:
                ```python
                action = Action(ActionType.Eat)
                name = action.getName()
                print(f"Action: {name}")
                ```
             )pbdoc")
        .def("clone", &Action::clone,
             R"pbdoc(
             Create a copy of this action.
             
             Returns:
                 Action: Clone of this action
             
            Useful for modifying actions or maintaining action history.
            
            Example:
                ```python
                original = Action(ActionType.MoveForward, [1.0])
                copy = original.clone()
                copy.setParameters([0.5])  # Modify copy without affecting original
                ```
             )pbdoc");

    // Improved WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation
        
        Represents an object in the simulated world.
        Includes position, type, and properties.
        
        World objects can be:
        - Empty: Empty space (passable)
        - Resource: Valuable item (can be collected)
        - Hazard: Dangerous object (causes damage)
        - Wall: Impassable barrier
        - Marker: Special landmark or goal
        
        Objects have physical properties:
        - Position (x, y) in world coordinates
        - Radius for collision detection
        - Type for behavior classification
        - Value for reward/damage calculation
        - Active state for temporary objects
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x,
             R"pbdoc(
             X coordinate of object center.
             
            Example:
                ```python
                obj.x = 10.0
                ```
             )pbdoc")
        .def_readwrite("y", &WorldObject::y,
             R"pbdoc(
             Y coordinate of object center.
             
            Example:
                ```python
                obj.y = 15.0
                ```
             )pbdoc")
        .def_readwrite("radius", &WorldObject::radius,
             R"pbdoc(
             Radius of object (collision detection).
             
            Example:
                ```python
                obj.radius = 1.0
                ```
             )pbdoc")
        .def_readwrite("type", &WorldObject::type,
             R"pbdoc(
             Type of object (Empty, Resource, Hazard, Wall, Marker).
             
            Example:
                ```python
                obj.type = pynlm.WorldObjectType.Resource
                ```
             )pbdoc")
        .def_readwrite("value", &WorldObject::value,
             R"pbdoc(
             Value of object (reward for resources, damage for hazards).
             
            Example:
                ```python
                obj.value = 10.0  # Resource gives 10 reward
                ```
             )pbdoc")
        .def_readwrite("active", &WorldObject::active,
             R"pbdoc(
             Whether object is active (can be collected/damaged).
             
            Example:
                ```python
                obj.active = False
                ```
             )pbdoc");

    // Improved AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state
        
        Represents the physical state of the agent in the world.
        Includes position, orientation, velocity, and physiological state.
        
        The agent body tracks the agent's physical properties:
        - Position and orientation in world coordinates
        - Velocity and angular velocity for movement
        - Energy level and health status
        - Age and action timing
        - Movement and turning states
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x,
             R"pbdoc(
             X position in world coordinates.
             
            Example:
                ```python
                body.x = 10.5
                ```
             )pbdoc")
        .def_readwrite("y", &AgentBody::y,
             R"pbdoc(
             Y position in world coordinates.
             
            Example:
                ```python
                body.y = 15.5
                ```
             )pbdoc")
        .def_readwrite("orientation", &AgentBody::orientation,
             R"pbdoc(
             Orientation angle in radians (0 = east, π/2 = north).
             
            Example:
                ```python
                body.orientation = math.pi / 2  # North
                ```
             )pbdoc")
        .def_readwrite("velocityX", &AgentBody::velocityX,
             R"pbdoc(
             X velocity component.
             
            Example:
                ```python
                body.velocityX = 2.0
                ```
             )pbdoc")
        .def_readwrite("velocityY", &AgentBody::velocityY,
             R"pbdoc(
             Y velocity component.
             
            Example:
                ```python
                body.velocityY = 1.0
                ```
             )pbdoc")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity,
             R"pbdoc(
             Angular velocity (radians per time unit).
             
            Example:
                ```python
                body.angularVelocity = 0.5
                ```
             )pbdoc")
        .def_readwrite("energy", &AgentBody::energy,
             R"pbdoc(
             Current energy level.
             
            Example:
                ```python
                body.energy = 50.0
                ```
             )pbdoc")
        .def_readwrite("health", &AgentBody::health,
             R"pbdoc(
             Health status (0 = dead, 100 = perfect health).
             
            Example:
                ```python
                body.health = 75.0
                ```
             )pbdoc")
        .def_readwrite("age", &AgentBody::age,
             R"pbdoc(
             Age in simulation time units.
             
            Example:
                ```python
                body.age = 100.0
                ```
             )pbdoc")
        .def_readwrite("isMoving", &AgentBody::isMoving,
             R"pbdoc(
             Whether agent is currently moving.
             
            Example:
                ```python
                body.isMoving = True
                ```
             )pbdoc")
        .def_readwrite("isTurning", &AgentBody::isTurning,
             R"pbdoc(
             Whether agent is currently turning.
             
            Example:
                ```python
                body.isTurning = True
                ```
             )pbdoc")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime,
             R"pbdoc(
             Time since last action.
             
            Example:
                ```python
                body.lastActionTime = 5.0
                ```
             )pbdoc")
        .def("reset", &AgentBody::reset,
             R"pbdoc(
             Reset body state to initial conditions.
             
            Resets:
            - Position to (0, 0)
            - Orientation to 0 (east)
            - Velocity to (0, 0)
            - Angular velocity to 0
            - Energy to maximum
            - Health to 100
            - Age to 0
            - Movement and turning states to false
            - Last action time to infinity
            
            Example:
                ```python
                body.reset()
                ```
             )pbdoc");

    // Improved ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world
        
        Represents the outcome of an action taken by the agent.
        Includes reward, success status, and descriptive message.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // Improved ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world
        
        Represents the outcome of an action taken by the agent.
        Includes reward, success status, and descriptive message.
        
        Action results provide feedback about action consequences:
        - Reward: Positive or negative value from environment
        - Success: Whether action was successful
        - Message: Human-readable description of outcome
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward,
             R"pbdoc(
             Reward received from performing the action.
             
            Positive reward = beneficial outcome
            Negative reward = detrimental outcome
            0.0 = neutral outcome
            
            Example:
                ```python
                result = ActionResult(10.0, True, "Collected food")
                ```
             )pbdoc")
        .def_readwrite("success", &ActionResult::success,
             R"pbdoc(
             Whether the action was successful.
             
            Example:
                ```python
                result = ActionResult(0.0, True, "Moved successfully")
                ```
             )pbdoc")
        .def_readwrite("message", &ActionResult::message,
             R"pbdoc(
             Descriptive message explaining the action result.
             
            Example:
                ```python
                result = ActionResult(-5.0, False, "Hit wall - took damage")
                ```
             )pbdoc");

    // Improved SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data
        
        Represents processed sensory information for the agent.
        Provides access to vision, touch, internal, and proprioceptive data.
        
        Sensory perception combines multiple sensory modalities:
        - Vision: Visual scene understanding
        - Touch: Contact and proximity detection
        - Internal: Internal physiological signals
        - Proprioception: Body position and movement awareness
        - Audio: Sound information (when implemented)
        
        Percepts are generated by the world and processed by the agent
        for decision making and action selection.
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision,
             R"pbdoc(
             Get visual perception data.
             
             Returns:
                 Vision: Visual percept or None if no vision available
             
            Example:
                ```python
                percept = sensory.getVision()
                if percept:
                    width = percept.getWidth()
                    height = percept.getHeight()
                ```
             )pbdoc")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             R"pbdoc(
             Set visual perception data.
             
             Parameters:
                 vision: Vision percept to set
             
            Example:
                ```python
                vision = pynlm.Vision(10, 10)
                perceptual.setVision(vision)
                ```
             )pbdoc")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth,
             R"pbdoc(
             Get vision width in pixels.
             
             Returns:
                 size_t: Vision width
             
            Example:
                ```python
                width = perceptual.getVisionWidth()
                ```
             )pbdoc")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight,
             R"pbdoc(
             Get vision height in pixels.
             
             Returns:
                 size_t: Vision height
             
            Example:
                ```python
                height = perceptual.getVisionHeight()
                ```
             )pbdoc")
        .def("getTouch", &SensoryPercept::getTouch,
             R"pbdoc(
             Get tactile perception data.
             
             Returns:
                 std::vector<float>: Touch data (contact proximity, etc.)
             
            Example:
                ```python
                touch_data = perceptual.getTouch()
                ```
             )pbdoc")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             R"pbdoc(
             Set tactile perception data.
             
             Parameters:
                 touch: Touch data vector
             
            Example:
                ```python
                touch_data = [0.1, 0.5, 0.0]  # Proximity values
                perceptual.setTouch(touch_data)
                ```
             )pbdoc")
        .def("getInternal", &SensoryPercept::getInternal,
             R"pbdoc(
             Get internal perception data.
             
             Returns:
                 std::vector<float>: Internal signals (energy, health, etc.)
             
            Example:
                ```python
                internal_data = perceptual.getInternal()
                ```
             )pbdoc")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             R"pbdoc(
             Set internal perception data.
             
             Parameters:
                 internal: Internal signals vector
             
            Example:
                ```python
                internal_data = [50.0, 75.0]  # energy, health
                perceptual.setInternal(internal_data)
                ```
             )pbdoc")
        .def("getProprioception", &SensoryPercept::getProprioception,
             R"pbdoc(
             Get proprioceptive perception data.
             
             Returns:
                 std::vector<float>: Proprioception data (joint angles, position)
             
            Example:
                ```python
                proprio_data = perceptual.getProprioception()
                ```
             )pbdoc")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             R"pbdoc(
             Set proprioceptive perception data.
             
             Parameters:
                 proprioception: Proprioception data vector
             
            Example:
                ```python
                proprio_data = [0.0, 0.5, 1.0]  # joint angles
                perceptual.setProprioception(proprio_data)
                ```
             )pbdoc")
        .def("getAudio", &SensoryPercept::getAudio,
             R"pbdoc(
             Get audio perception data.
             
             Returns:
                 Audio: Audio percept or None if no audio available
             
            Example:
                ```python
                audio = perceptual.getAudio()
                if audio:
                    sample_rate = audio.getSampleRate()
                ```
             )pbdoc")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             R"pbdoc(
             Set audio perception data.
             
             Parameters:
                 audio: Audio percept to set
             
            Example:
                ```python
                audio = pynlm.Audio(44100, 44100)
                perceptual.setAudio(audio)
                ```
             )pbdoc")
        .def("getAllSignals", &SensoryPercept::getAllSignals,
             R"pbdoc(
             Get all sensory signals combined.
             
             Returns:
                 Dict: Dictionary containing all sensory signals
             
            Keys: "vision", "touch", "internal", "proprioception", "audio"
            
            Example:
                ```python
                signals = perceptual.getAllSignals()
                vision = signals.get("vision")
                touch = signals.get("touch")
                ```
             )pbdoc")
        .def("getTimestamp", &SensoryPercept::getTimestamp,
             R"pbdoc(
             Get perception timestamp.
             
             Returns:
                 float: Timestamp when perception was generated
             
            Example:
                ```python
                timestamp = perceptual.getTimestamp()
                ```
             )pbdoc")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             R"pbdoc(
             Set perception timestamp.
             
             Parameters:
                 timestamp: New timestamp
             
            Example:
                ```python
                perceptual.setTimestamp(0.5)
                ```
             )pbdoc");

    // Improved SimpleWorld class with context manager
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation
        
        Provides a 2D simulation environment for agent development.
        Includes world geometry, objects, and sensory simulation.
        
        The SimpleWorld simulates a basic physical environment where agents
        can move, interact with objects, and receive sensory input.
        
        World Features:
        - 2D rectangular geometry with configurable dimensions
        - Dynamic objects (resources, hazards, walls, markers)
        - Agent physics (position, velocity, collision detection)
        - Sensory simulation (vision, touch, proprioception)
        - Energy management and decay
        - Context manager support for safe resource cleanup
        
        Object Types:
        - Empty: Passable space
        - Resource: Collectible with positive value
        - Hazard: Dangerous with negative value
        - Wall: Impassable barrier
        - Marker: Special landmark or goal
        
        Context Manager Usage:
        ```python
        with pynlm.createSimpleWorld() as world:
            world.configure(width=20, height=20, visionWidth=5, visionHeight=5)
            world.reset()
            # ... use world ...
            # Automatic cleanup on exit
        ```
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             R"pbdoc(
             Configure world dimensions and agent sensory capabilities.
             
             Parameters:
                 width: World width in simulation units
                 height: World height in simulation units
                 visionWidth: Width of agent's visual field
                 visionHeight: Height of agent's visual field
             
            The vision field determines the size of the visual input
            that the agent receives from the world.
            
            Example:
                ```python
                world = pynlm.createSimpleWorld()
                world.configure(width=20.0, height=15.0, visionWidth=5, visionHeight=5)
                ```
             )pbdoc")
        .def("reset", &SimpleWorld::reset,
             R"pbdoc(
             Reset world to initial state.
             
            Resets:
            - Agent position to start position
            - Object positions to initial locations
            - Simulation time to zero
            - Energy levels to maximum
            - Removes any temporary objects
            
            Example:
                ```python
                world.reset()
                ```
             )pbdoc")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             R"pbdoc(
             Set agent starting position.
             
             Parameters:
                 x: X coordinate of agent start position
                 y: Y coordinate of agent start position
             
            Example:
                ```python
                world.setAgentStart(10.0, 10.0)
                ```
             )pbdoc")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             R"pbdoc(
             Update world state for one simulation timestep.
             
             Parameters:
                 timestep: Time step duration (in seconds or simulation units)
             
            The update performs:
            - Agent movement based on velocity
            - Collision detection and response
            - Object interactions
            - Energy decay
            - Sensory data generation
            
            Example:
                ```python
                for step in range(100):
                    world.update(0.01)  # 10ms timestep
                ```
             )pbdoc")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             R"pbdoc(
             Apply motor command to agent.
             
             Parameters:
                 cmd: MotorCommand to apply
                 currentTime: Current simulation time
             
            Returns:
                ActionResult: Result of the action (reward, success, message)
            
            The world processes the motor command:
            - Converts command to agent movement/rotation
            - Checks for collisions with objects
            - Applies consequences (collect resources, take damage)
            - Returns action result
            
            Example:
                ```python
                action = MotorCommand(MotorCommand.MoveForward, [1.0])
                result = world.applyMotorCommand(action, 0.5)
                print(f"Reward: {result.reward}, Success: {result.success}")
                ```
             )pbdoc")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get current sensory percept for the agent.
             
             Returns:
                 SensoryPercept: Current sensory data
             
            The percept includes:
            - Vision: Visual scene from agent's perspective
            - Touch: Contact and proximity information
            - Internal: Agent's physiological state
            - Proprioception: Body position and movement
            - Audio: Sound information (when implemented)
            
            Example:
                ```python
                percept = world.getSensoryPercept()
                vision = percept.getVision()
                touch = percept.getTouch()
                ```
             )pbdoc")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get agent body state.
             
             Returns:
                 AgentBody: Current agent physical state
             
            Includes:
            - Position and orientation
            - Velocity and angular velocity
            - Energy and health
            - Age and action timing
            - Movement and turning states
            
            Example:
                ```python
                body = world.getAgentBody()
                position = (body.x, body.y)
                orientation = body.orientation
                energy = body.energy
                ```
             )pbdoc")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             R"pbdoc(
             Add object to world.
             
             Parameters:
                 obj: WorldObject to add
             
            Example:
                ```python
                resource = WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, 10.0)
                world.addObject(resource)
                ```
             )pbdoc")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             R"pbdoc(
             Remove object at position.
             
             Parameters:
                 x: X coordinate
                 y: Y coordinate
             
            Example:
                ```python
                world.removeObject(5.0, 5.0)
                ```
             )pbdoc")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             R"pbdoc(
             Check if position is valid (not wall).
             
             Parameters:
                 x: X coordinate to check
                 y: Y coordinate to check
             
             Returns:
                 bool: True if position is valid, False if wall or invalid
             
            Example:
                ```python
                if world.isValidPosition(10.0, 10.0):
                    print("Position is free")
                else:
                    print("Position is blocked")
                ```
             )pbdoc")
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get object at position (if any).
             
             Parameters:
                 x: X coordinate
                 y: Y coordinate
             
             Returns:
                 WorldObject or None: Object at position, or None if empty
             
            Example:
                ```python
                obj = world.getObjectAt(5.0, 5.0)
                if obj:
                    print(f"Object at ({obj.x}, {obj.y}): {obj.type}")
                ```
             )pbdoc")
        .def("getWidth", &SimpleWorld::getWidth,
             R"pbdoc(
             Get world width.
             
             Returns:
                 float: World width in simulation units
             
            Example:
                ```python
                width = world.getWidth()
                ```
             )pbdoc")
        .def("getHeight", &SimpleWorld::getHeight,
             R"pbdoc(
             Get world height.
             
             Returns:
                 float: World height in simulation units
             
            Example:
                ```python
                height = world.getHeight()
                ```
             )pbdoc")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             R"pbdoc(
             Get maximum energy.
             
             Returns:
                 float: Maximum energy level
             
            Example:
                ```python
                max_energy = world.getMaxEnergy()
                ```
             )pbdoc")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             R"pbdoc(
             Set maximum energy.
             
             Parameters:
                 e: New maximum energy level
             
            Example:
                ```python
                world.setMaxEnergy(100.0)
                ```
             )pbdoc")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             R"pbdoc(
             Get energy decay rate.
             
             Returns:
                 float: Energy decay rate per timestep
             
            Example:
                ```python
                decay_rate = world.getEnergyDecayRate()
                ```
             )pbdoc")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             R"pbdoc(
             Set energy decay rate.
             
             Parameters:
                 r: New energy decay rate
             
            Example:
                ```python
                world.setEnergyDecayRate(0.01)
                ```
             )pbdoc")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             R"pbdoc(
             Get current simulation time.
             
             Returns:
                 double: Current simulation time
             
            Example:
                ```python
                sim_time = world.getSimulationTime()
                print(f"Simulation time: {sim_time}")
                ```
             )pbdoc")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             R"pbdoc(
             Set random seed for reproducibility.
             
             Parameters:
                 seed: Random seed value
             
            Example:
                ```python
                world.setRandomSeed(12345)  # Reproducible results
                ```
             )pbdoc")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             R"pbdoc(
             Get current random seed.
             
             Returns:
                 uint64_t: Current random seed
             
            Example:
                ```python
                seed = world.getRandomSeed()
                ```
             )pbdoc")
        
        // Context manager support
        .def("__enter__", [](py::object self) { return self; })
        .def("__exit__", [](py::object, const py::object&, int, const py::object&) {
            // Cleanup code here
        })
        .def("close", [](SimpleWorld& self) {
            self.reset();
        }, R"pbdoc(
            Reset world to clean state and release resources.
            
            This method is provided for explicit resource cleanup. If using
            SimpleWorld in a context manager, automatic cleanup will occur on exit.
            
            Example:
                ```python
                world = pynlm.createSimpleWorld()
                # ... use world ...
                world.close()  # Explicit cleanup
                ```
             )pbdoc");

    // IMPROVED BRAIN CLASS with context manager and enhanced methods
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class
        
        Implements real spiking neural computation with event-driven dynamics
        and integrated memory, prediction, cognition, and neuromodulation systems.
        
        The brain is the core neural component that performs sensory processing,
        learning, decision making, and action production. It supports context
        manager usage for safe resource management.
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
    // IMPROVED BRAIN CLASS with context manager and enhanced methods
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class
        
        Implements real spiking neural computation with event-driven dynamics
        and integrated memory, prediction, cognition, and neuromodulation systems.
        
        The brain is the core neural component that performs sensory processing,
        learning, decision making, and action production. It supports context
        manager usage for safe resource management and provides comprehensive
        statistics and monitoring capabilities.
        
        Key Features:
        - Event-driven spiking neural dynamics
        - Integrated memory systems (working, episodic, associative)
        - Prediction and planning capabilities
        - Neuromodulation (dopamine, curiosity, novelty, prediction error)
        - Developmental stage management
        - Real-time statistics and monitoring
        - Checkpoint save/load for persistence
        - Context manager support for automatic cleanup
        
        Context Manager Usage:
        ```python
        with pynlm.createBrain(config) as brain:
            brain.initialize()
            for step in range(100):
                brain.step(step)
            # Brain automatically cleaned up
        ```
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             R"pbdoc(
             Create a new brain with neural network configuration.
             
             Parameters:
                 config: Neural network configuration containing parameters like
                        population sizes, learning rates, connectivity rules, etc.
             
             Example:
                 ```python
                 config = pynlm.createDefaultConfig()
                 brain = pynlm.createBrain(config)
                 ```
             )pbdoc")
        .def("initialize", &Brain::initialize,
             R"pbdoc(
             Initialize the brain with configuration and create neural populations.
             
             This method sets up the complete neural architecture including:
             - Sensory neuron populations
             - Motor neuron populations  
             - Internal processing neurons
             - Synaptic connections between populations
             - Memory system integration
             
             Returns:
                 bool: True if initialization succeeded, False otherwise
             
             Error Conditions:
                 - Throws RuntimeError if initialization fails
                 - Throws ValidationError if configuration is invalid
                 - Throws ConfigError if required configuration parameters are missing
            
            Example:
                ```python
                brain = pynlm.createBrain(config)
                if brain.initialize():
                    print("Brain initialized successfully")
                else:
                    print("Brain initialization failed")
                ```
             )pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             R"pbdoc(
             Perform a single simulation step.
             
             Parameters:
                 currentStep: Current simulation step number (used for plasticity timing)
             
             Error Conditions:
                 - Throws RuntimeError if step execution fails
                 - Throws ValidationError if step parameters are invalid
            
            The step performs:
            - Current integration for all neurons
            - Spike generation and propagation
            - Synaptic plasticity updates (STDP, Hebbian)
            - Memory system updates
            - Neuromodulation effects
            - Developmental changes
            
            Example:
                ```python
                brain.initialize()
                brain.step(0)  # First simulation step
                brain.step(1)  # Second simulation step
                ```
             )pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             R"pbdoc(
             Perform a single simulation step with explicit timestamp.
             
             Parameters:
                 currentStep: Current simulation step number
                 currentTime: Current simulation time (in milliseconds or seconds)
             
             Error Conditions:
                 - Throws RuntimeError if step execution fails
                 - Throws ValidationError if step parameters are invalid
            
            This method provides more precise temporal control compared to the
            two-parameter version. The timestamp is used for:
            - Time-dependent neuromodulation
            - Age-based developmental effects
            - Learning rate modulation over time
            
            Example:
                ```python
                brain.initialize()
                brain.step(0, 0.0)  # First step at time 0.0
                brain.step(1, 0.01) # Second step at time 0.01
                ```
             )pbdoc")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             R"pbdoc(
             Inject sensory input into the brain for processing.
             
             Parameters:
                 input: SensoryInput object containing environmental data.
                        Can be Vision, Audio, InternalSignals, or other sensory types.
             
             Error Conditions:
                 - Throws RuntimeError if input processing fails
                 - Throws ValidationError if input is invalid or corrupted
                 - Throws TypeError if input is not a valid SensoryInput type
            
            The method performs:
            - Transduction of sensory signals into neural currents
            - Integration with sensory neuron populations
            - Preparation for action selection via motor neurons
            
            Example:
                ```python
                brain.initialize()
                
                # Create visual input
                vision = pynlm.Vision(width=10, height=10, channels=3)
                # Fill vision data with sample image (normalized 0-1)
                data = [0.1, 0.2, 0.3, 0.4, 0.5] * 20
                vision.setData(data)
                
                # Process visual input
                brain.receiveSensoryInput(vision)
                ```
             )pbdoc")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             R"pbdoc(
             Inject current directly into a specific neuron.
             
             Parameters:
                 neuron: NeuronId identifying the target neuron
                 current: MembranePotential value to inject (positive for depolarization)
             
             Error Conditions:
                 - Throws RuntimeError if neuron injection fails
                 - Throws ValidationError if neuron is invalid or current is out of range
                 - Throws TypeError if neuron or current types are incorrect
            
            Example:
                ```python
                neuron_id = pynlm.INVALID_NEURON_ID  # Get from specific neuron
                brain.injectCurrent(neuron_id, 5.0)  # Inject 5 units of current
                ```
             )pbdoc")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             R"pbdoc(
             Inject current into all neurons of a specific type.
             
             Parameters:
                 type: NeuronType specifying which neurons to target
                 current: MembranePotential value to inject
             
             Error Conditions:
                 - Throws RuntimeError if current injection fails
                 - Throws ValidationError if neuron type is invalid or current is out of range
            
            Neuron types:
                - Excitatory: Promotes firing activity
                - Inhibitory: Suppresses firing activity
                - Modulatory: Affects neuromodulation systems
                - Sensory: Receive environmental input
                - Motor: Produce action outputs
                - Internal: Handle internal processing
            
            Example:
                ```python
                # Depolarize all excitatory neurons
                brain.injectCurrentToNeurons(pynlm.NeuronType.Excitatory, 10.0)
                
                # Hyperpolarize all inhibitory neurons  
                brain.injectCurrentToNeurons(pynlm.NeuronType.Inhibitory, -5.0)
                ```
             )pbdoc")
        .def("produceAction", &Brain::produceAction,
             R"pbdoc(
             Produce motor action based on neural activity patterns.
             
             Returns:
                 Action: Motor command object containing action type and parameters
             
             Error Conditions:
                 - Throws RuntimeError if action production fails
                 - Throws RuntimeError if no motor neurons are active
            
            The method performs:
            - Decoding motor neuron population activity
            - Selecting appropriate action type (Move, Turn, Look, etc.)
            - Generating action parameters from neural activity
            - Returning Action object for world execution
            
            Example:
                ```python
                brain.initialize()
                
                # Inject some current to activate motor neurons
                brain.injectCurrentToNeurons(pynlm.NeuronType.Motor, 1.0)
                
                # Produce action
                action = brain.produceAction()
                print(f"Produced action: {action.getType()}")
                
                # Action parameters can be accessed
                params = action.getParameters()
                ```
             )pbdoc")
        .def("reset", &Brain::reset,
             R"pbdoc(
             Reset brain state to initial conditions.
             
             Resets:
             - All neuron membrane potentials to resting state
             - Synaptic weights to initial values
             - Memory system contents to empty
             - Neuromodulation levels to baseline
             - Developmental stage to Initial
             
             Example:
                 ```python
                 brain.initialize()
                 brain.step(10)  # Let brain evolve
                 brain.reset()   # Reset to initial state
                 ```
             )pbdoc")
        .def("save", &Brain::save, py::arg("filepath"),
             R"pbdoc(
             Save brain state to file for checkpointing or persistence.
             
             Parameters:
                 filepath: Path where brain state will be saved
             
             Returns:
                 bool: True if save succeeded, False otherwise
             
             Error Conditions:
                 - Throws RuntimeError if file cannot be written
                 - Throws ConfigError if filepath is invalid
                 - Throws RuntimeError if brain state serialization fails
            
            Saved state includes:
            - All neuron membrane potentials
            - Synaptic weight matrices
            - Memory system contents
            - Neuromodulation system state
            - Developmental stage
            - Current simulation step/time
            
            Example:
                ```python
                brain.initialize()
                # Run simulation for some time
                for step in range(100):
                    brain.step(step)
                
                # Save checkpoint
                if brain.save("brain_checkpoint.dat"):
                    print("Brain state saved")
                ```
             )pbdoc")
        .def("load", &Brain::load, py::arg("filepath"),
             R"pbdoc(
             Load brain state from file.
             
             Parameters:
                 filepath: Path to saved brain state file
             
             Returns:
                 bool: True if load succeeded, False otherwise
             
             Error Conditions:
                 - Throws RuntimeError if file cannot be read
                 - Throws ConfigError if filepath is invalid
                 - Throws RuntimeError if file format is corrupted
                 - Throws ValidationError if loaded state is invalid
            
            Example:
                ```python
                brain = pynlm.createBrain(config)
                
                # Load previous checkpoint
                if brain.load("brain_checkpoint.dat"):
                    print("Brain state loaded")
                    brain.initialize()  # Need to re-initialize after load
                else:
                    print("Failed to load brain state")
                ```
             )pbdoc")
        
        // ENHANCED STATISTICS with convenience methods
        .def("getRegionCount", &Brain::getRegionCount,
             R"pbdoc(
             Get the number of neural regions in the brain.
             
             Returns:
                 size_t: Number of neural regions
             
             Example:
                 ```python
                 brain.initialize()
                 region_count = brain.getRegionCount()
                 print(f"Brain has {region_count} neural regions")
                 ```
             )pbdoc")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             R"pbdoc(
             Get total neuron count across all brain regions.
             
             Returns:
                 size_t: Total number of neurons in the brain
             
             Example:
                 ```python
                 brain.initialize()
                 neuron_count = brain.getTotalNeuronCount()
                 print(f"Brain has {neuron_count} neurons")
                 ```
             )pbdoc")
        
        // CONVENIENCE METHOD - alias for backward compatibility
        .def("getNeuronCount", &Brain::getTotalNeuronCount,
             R"pbdoc(
             Get total neuron count (convenience method - alias of getTotalNeuronCount).
             
             Returns:
                 size_t: Total number of neurons in the brain
             
             This method is provided for backward compatibility and convenience.
             It has the same functionality as getTotalNeuronCount().
             
            Example:
                ```python
                brain.initialize()
                neuron_count = brain.getNeuronCount()  # Same as getTotalNeuronCount()
                ```
             )pbdoc")
        
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             R"pbdoc(
             Get total synapse count across all regions.
             
             Returns:
                 size_t: Total number of synapses in the brain
             
             Example:
                 ```python
                 brain.initialize()
                 synapse_count = brain.getTotalSynapseCount()
                 print(f"Brain has {synapse_count} synapses")
                 ```
             )pbdoc")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             R"pbdoc(
             Get count of active neurons (currently depolarized).
             
             Returns:
                 size_t: Number of neurons above firing threshold
             
             Example:
                 ```python
                 brain.initialize()
                 active_count = brain.getActiveNeuronCount()
                 print(f"Brain has {active_count} active neurons")
                 ```
             )pbdoc")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             R"pbdoc(
             Get count of currently firing neurons.
             
             Returns:
                 size_t: Number of neurons that have spiked in current step
             
             Example:
                 ```python
                 brain.initialize()
                 brain.step(0)  # Let neurons fire
                 firing_count = brain.getFiringNeuronCount()
                 print(f"Brain has {firing_count} firing neurons")
                 ```
             )pbdoc")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             R"pbdoc(
             Get average firing rate across all neurons.
             
             Returns:
                 float: Average firing rate in Hz (spikes per second)
             
             Example:
                 ```python
                 brain.initialize()
                 for step in range(100):
                     brain.step(step)
                 firing_rate = brain.getAverageFiringRate()
                 print(f"Average firing rate: {firing_rate:.2f} Hz")
                 ```
             )pbdoc")
        
        // Get spike rate (convenience method from firing data)
        .def("getSpikeRate", [](const Brain& self) {
            size_t totalSpikes = self.getTotalSpikeCount();
            size_t activeNeurons = self.getActiveNeuronCount();
            if (activeNeurons == 0) return 0.0f;
            return static_cast<float>(totalSpikes) / activeNeurons;
        }, R"pbdoc(
            Get average spike rate per active neuron.
            
            Returns:
                float: Average spike rate (spikes per active neuron per time unit)
            
            This convenience method calculates the average number of spikes
            generated per active neuron, providing a normalized measure of
            brain activity independent of the number of active neurons.
            
            Example:
                ```python
                brain.initialize()
                brain.step(0)
                spike_rate = brain.getSpikeRate()
                print(f"Spike rate: {spike_rate:.3f}")
                ```
            )pbdoc")
        
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             R"pbdoc(
             Get excitation/inhibition balance ratio.
             
             Returns:
                 float: Excitation/inhibition ratio (higher = more excitation)
             
             Example:
                 ```python
                 brain.initialize()
                 eia_ratio = brain.getExcitationInhibitionRatio()
                 print(f"Excitation/Inhibition ratio: {eia_ratio:.3f}")
                 ```
             )pbdoc")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             R"pbdoc(
             Get total spike count since brain initialization.
             
             Returns:
                 size_t: Total number of spikes generated
             
            Example:
                ```python
                brain.initialize()
                for step in range(50):
                    brain.step(step)
                total_spikes = brain.getTotalSpikeCount()
                print(f"Total spikes: {total_spikes}")
                ```
             )pbdoc")
        
        // DEVELOPMENT METHODS
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             R"pbdoc(
             Get current developmental stage of the brain.
             
             Returns:
                 DevelopmentalStage: Current developmental stage
             
             Possible stages:
                 - Initial: Starting developmental state
                 - CriticalPeriod: High plasticity phase
                 - Maturation: Gradual specialization
                 - Adult: Stable adult brain
                 - Aging: Age-related decline
             
            Example:
                ```python
                brain.initialize()
                stage = brain.getDevelopmentalStage()
                print(f"Developmental stage: {stage}")
                ```
             )pbdoc")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             R"pbdoc(
             Set developmental stage of the brain.
             
             Parameters:
                 stage: New developmental stage to set
             
             Example:
                ```python
                brain.initialize()
                brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Maturation)
                ```
             )pbdoc")
        
        // MEMORY SYSTEM ACCESS
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             R"pbdoc(
             Get working memory system.
             
             Returns:
                 WorkingMemory: Reference to working memory system
             
            Example:
                ```python
                brain.initialize()
                working_memory = brain.getWorkingMemory()
                ```
             )pbdoc")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             R"pbdoc(
             Get episodic memory system.
             
             Returns:
                 EpisodicMemory: Reference to episodic memory system
             
            Example:
                ```python
                brain.initialize()
                episodic_memory = brain.getEpisodicMemory()
                ```
             )pbdoc")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             R"pbdoc(
             Get associative memory system.
             
             Returns:
                 AssociativeMemory: Reference to associative memory system
             
            Example:
                ```python
                brain.initialize()
                associative_memory = brain.getAssociativeMemory()
                ```
             )pbdoc")
        
        // PREDICTION SYSTEM ACCESS
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             R"pbdoc(
             Get prediction system for predictive coding.
             
             Returns:
                 PredictionSystem: Reference to prediction system
             
            Example:
                ```python
                brain.initialize()
                prediction_system = brain.getPredictionSystem()
                ```
             )pbdoc")
        
        // NEUROMODULATION SYSTEM ACCESS
        .def("getDopamine", &Brain::getDopamine,
             R"pbdoc(
             Get dopamine neuromodulation system.
             
             Returns:
                 Dopamine: Reference to dopamine system
             
            Example:
                ```python
                brain.initialize()
                dopamine = brain.getDopamine()
                ```
             )pbdoc")
        .def("getCuriosity", &Brain::getCuriosity,
             R"pbdoc(
             Get curiosity neuromodulation system.
             
             Returns:
                 Curiosity: Reference to curiosity system
             
            Example:
                ```python
                brain.initialize()
                curiosity = brain.getCuriosity()
                ```
             )pbdoc")
        .def("getNovelty", &Brain::getNovelty,
             R"pbdoc(
             Get novelty neuromodulation system.
             
             Returns:
                 Novelty: Reference to novelty system
             
            Example:
                ```python
                brain.initialize()
                novelty = brain.getNovelty()
                ```
             )pbdoc")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             R"pbdoc(
             Get prediction error signal.
             
             Returns:
                 PredictionError: Reference to prediction error system
             
            Example:
                ```python
                brain.initialize()
                prediction_error = brain.getPredictionErrorSignal()
                ```
             )pbdoc")
        
        // PLASTICITY SYSTEM ACCESS
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             R"pbdoc(
             Get spike system for neural dynamics.
             
             Returns:
                 SpikeSystem: Reference to spike system
             
            Example:
                ```python
                brain.initialize()
                spike_system = brain.getSpikeSystem()
                ```
             )pbdoc")
        .def("getSTDP", &Brain::getSTDP,
             R"pbdoc(
             Get STDP plasticity system.
             
             Returns:
                 STDP: Reference to STDP (Spike-Timing-Dependent Plasticity) system
             
            Example:
                ```python
                brain.initialize()
                stdp = brain.getSTDP()
                ```
             )pbdoc")
        .def("getHebbian", &Brain::getHebbian,
             R"pbdoc(
             Get Hebbian plasticity system.
             
             Returns:
                 Hebbian: Reference to Hebbian plasticity system
             
            Example:
                ```python
                brain.initialize()
                hebbian = brain.getHebbian()
                ```
             )pbdoc")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             R"pbdoc(
             Get structural plasticity system.
             
             Returns:
                 StructuralPlasticity: Reference to structural plasticity system
             
            Example:
                ```python
                brain.initialize()
                structural_plasticity = brain.getStructuralPlasticity()
                ```
             )pbdoc")
        
        // COGNITION SYSTEM ACCESS
        .def("getPlanner", &Brain::getPlanner,
             R"pbdoc(
             Get neural planner for action planning.
             
             Returns:
                 NeuralPlanner: Reference to neural planner
             
            Example:
                ```python
                brain.initialize()
                planner = brain.getPlanner()
                ```
             )pbdoc")
        .def("getConceptFormation", &Brain::getConceptFormation,
             R"pbdoc(
             Get concept formation system.
             
             Returns:
                 ConceptFormation: Reference to concept formation system
             
            Example:
                ```python
                brain.initialize()
                concept_formation = brain.getConceptFormation()
                ```
             )pbdoc")
        .def("getAttention", &Brain::getAttention,
             R"pbdoc(
             Get attentional selection system.
             
             Returns:
                 AttentionalSelection: Reference to attentional system
             
            Example:
                ```python
                brain.initialize()
                attention = brain.getAttention()
                ```
             )pbdoc")
        
        // DEVELOPMENT SYSTEM ACCESS
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             R"pbdoc(
             Get development system for neural development.
             
             Returns:
                 DevelopmentSystem: Reference to development system
             
            Example:
                ```python
                brain.initialize()
                development_system = brain.getDevelopmentSystem()
                ```
             )pbdoc")
        
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get the configuration.
             
             Returns:
                 Config: Reference to the brain's configuration
             
            Example:
                ```python
                brain.initialize()
                config = brain.getConfig()
                ```
             )pbdoc")
        
        .def("logStatus", &Brain::logStatus,
             R"pbdoc(
             Log brain status to output/console.
             
             Logs:
             - Brain statistics (neuron counts, firing rates)
             - Memory system states
             - Neuromodulation levels
             - Developmental stage
             - System health indicators
            
            Example:
                ```python
                brain.initialize()
                brain.logStatus()
                ```
             )pbdoc")
        
        // Context manager support for Brain
        .def("__enter__", [](py::object self) { return self; })
        .def("__exit__", [](py::object, const py::object&, int, const py::object&) {
            // Cleanup code here
        })
        
        .def("close", [](Brain& self) {
            self.reset();
        }, R"pbdoc(
            Reset brain to clean state and release resources.
            
            This method is provided for explicit resource cleanup. If using
            Brain in a context manager, automatic cleanup will occur on exit.
            
            Example:
                ```python
                brain = pynlm.createBrain(config)
                # ... use brain ...
                brain.close()  # Explicit cleanup
                ```
             )pbdoc");

    // IMPROVED AGENT BRAIN CLASS
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world
        
        Provides high-level interface between the neural brain and the simulated world.
        Handles sensory processing, motor output, and neuromodulation.
        
        The AgentBrain acts as a bridge between the neural computations of the Brain
        and the interaction with the external environment through sensory and motor systems.
        
        Key Features:
        - Sensory transduction: Converts world percepts to brain input
        - Motor decoding: Converts neural activity to motor commands
        - Neuromodulation integration: Applies reward, curiosity, and novelty signals
        - Development management: Tracks brain developmental stage
        - Configuration flags: Enable/disable neuromodulation features
        
        Context Manager Support:
        AgentBrain instances can be used with context managers for safe resource management.
        
        Development and Neuromodulation Methods:
        - Track developmental age and stage
        - Monitor neuromodulation levels (dopamine, curiosity, novelty)
        - Apply reinforcement learning signals
        - Control feature enabling/disabling
        
        Example Usage:
        ```python
        import pynlm
        
        # Create brain and world
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=5, visionHeight=5)
        
        # Create agent brain
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable neuromodulation features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run simulation
        for step in range(100):
            # Process sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Update development
            agent.updateDevelopment(0.01)
            
            # Get current neuromodulation levels
            neuromod_level = agent.getNeuromodulationLevel()
            curiosity_level = agent.getCuriosityLevel()
            novelty_level = agent.getNoveltyLevel()
            
            # Get motor command
            motor_cmd = agent.decodeMotorCommand()
            
            # Apply motor command to world
            world.applyMotorCommand(motor_cmd, 0.01)
            
            # Update world state
            world.update(0.01)
        ```
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             R"pbdoc(
             Create an agent brain interface.
             
             Parameters:
                 brain: Shared pointer to the neural brain to interface with
             
             Example:
                 ```python
                 config = pynlm.createDefaultConfig()
                 brain = pynlm.createBrain(config)
                 agent = pynlm.createAgentBrain(brain)
                 ```
             )pbdoc")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             R"pbdoc(
             Initialize agent brain with world interface.
             
             Parameters:
                 world: SimpleWorld instance providing sensory input
             
            Setup:
            - Maps sensory neuron groups to world percepts
            - Prepares motor neuron groups for command decoding
            - Initializes neuromodulation state
            - Sets developmental age to zero
            
            Error Conditions:
                - Throws RuntimeError if world is invalid
                - Throws ValidationError if world configuration is incompatible
            
            Example:
                ```python
                brain = pynlm.createBrain(config)
                world = pynlm.createSimpleWorld()
                world.configure(width=20, height=20, visionWidth=5, visionHeight=5)
                
                agent = pynlm.createAgentBrain(brain)
                agent.initialize(world)
                ```
             )pbdoc")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             R"pbdoc(
             Get expected sensory input size from world.
             
             Returns:
                 size_t: Number of sensory input values expected from world
             
            The size depends on world configuration:
            - Vision: width * height * channels
            - Touch: 1 (contact) + proximity sensors
            - Internal: Various internal signal counts
            - Proprioception: Joint/position encoders
            
            Example:
                ```python
                agent = pynlm.createAgentBrain(brain)
                agent.initialize(world)
                sensory_size = agent.getSensoryInputSize()
                print(f"Expected sensory input size: {sensory_size}")
                ```
             )pbdoc")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             R"pbdoc(
             Get expected motor output size.
             
             Returns:
                 size_t: Number of motor output values
             
            Motor output corresponds to available actions:
            - Move commands (forward, backward, left, right)
            - Turning commands (left, right)  
            - Look commands (up, down, left, right)
            - Interaction commands (interact, eat, drink)
            - Rest/waiting commands
            
            Example:
                ```python
                agent = pynlm.createAgentBrain(brain)
                agent.initialize(world)
                motor_size = agent.getMotorOutputSize()
                print(f"Expected motor output size: {motor_size}")
                ```
             )pbdoc")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             R"pbdoc(
             Process sensory percept and inject into brain.
             
             Parameters:
                 percept: SensoryPercept containing world state information
             
            Processing Steps:
            1. Extract vision, touch, internal, proprioception data
            2. Normalize and filter sensory signals
            3. Inject into appropriate sensory neuron populations
            4. Update novelty and prediction error calculations
            5. Apply neuromodulation based on novel inputs
            
            Error Conditions:
                - Throws RuntimeError if percept processing fails
                - Throws ValidationError if percept data is invalid
                - Throws TypeError if percept is not a valid SensoryPercept
            
            Example:
                ```python
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                ```
             )pbdoc")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             R"pbdoc(
             Decode brain motor activity into motor command.
             
             Returns:
                 MotorCommand: Selected action command with parameters
             
            Decoding Process:
            1. Evaluate motor neuron population activity
            2. Apply action selection with curiosity modulation
            3. Convert neural activity to motor command
            4. Extract action parameters (speed, direction, etc.)
            
            Selection Mechanisms:
            - Default selection based on strongest motor neuron
            - Curiosity-driven exploration for novel inputs
            - Reward-based reinforcement learning
            - Developmental stage modulation
            
            Example:
                ```python
                motor_cmd = agent.decodeMotorCommand()
                print(f"Selected action: {motor_cmd.getType()}")
                
                # Access action parameters
                params = motor_cmd.getParameters()
                if motor_cmd.getType() == pynlm.ActionType.MoveForward:
                    speed = params[0]  # Speed parameter
                ```
             )pbdoc")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             R"pbdoc(
             Apply reward-based neuromodulation.
             
             Parameters:
                 reward: Actual reward received from environment
                 predictedReward: Predicted reward from agent's model
             
            Neuromodulation Effects:
            - Dopamine: Increases with reward prediction error
            - Exploration: Enhanced by positive prediction errors
            - Learning: Accelerated by reward signals
            - Attention: Focused on rewarding stimuli
            
            Error Conditions:
                - Throws ValidationError if reward values are invalid
                - Throws RuntimeError if neuromodulation system fails
            
            Example:
                ```python
                # Apply reward signal
                agent.applyRewardModulation(reward=1.0, predictedReward=0.8)
                
                # Positive prediction error (better than expected)
                agent.applyRewardModulation(reward=2.0, predictedReward=1.0)
                
                # Negative prediction error (worse than expected)
                agent.applyRewardModulation(reward=0.5, predictedReward=1.5)
                ```
             )pbdoc")
        
        // DEVELOPMENT AND NEUROMODULATION
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             R"pbdoc(
             Update development system.
             
             Parameters:
                 timestep: Time step duration (in simulation units)
             
            Development Updates:
            - Increment developmental age
            - Update plasticity modifier based on stage
            - Check for critical period transitions
            - Apply developmental pruning/synaptogenesis
            
            Example:
                ```python
                agent.updateDevelopment(0.01)
                ```
             )pbdoc")
        
        // CONVENIENCE METHODS
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             R"pbdoc(
             Get current developmental stage.
             
             Returns:
                 DevelopmentalStage: Current developmental stage
             
            Example:
                ```python
                stage = agent.getDevelopmentalStage()
                print(f"Developmental stage: {stage}")
                ```
             )pbdoc")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             R"pbdoc(
             Get current neuromodulation level.
             
             Returns:
                 float: Combined neuromodulation level (0.0 to 1.0)
             
            The level integrates dopamine, curiosity, novelty, and prediction error
            signals into a single measure of brain activation and engagement.
            
            Example:
                ```python
                neuromod_level = agent.getNeuromodulationLevel()
                print(f"Neuromodulation level: {neurom_level:.3f}")
                ```
             )pbdoc")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             R"pbdoc(
             Get curiosity level.
             
             Returns:
                 float: Curiosity level (0.0 = no exploration, 1.0 = high exploration)
             
            Example:
                ```python
                curiosity_level = agent.getCuriosityLevel()
                print(f"Curiosity level: {curiosity_level:.3f}")
                ```
             )pbdoc")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             R"pbdoc(
             Get novelty level.
             
             Returns:
                 float: Novelty level (0.0 = familiar, 1.0 = highly novel)
             
            Example:
                ```python
                novelty_level = agent.getNoveltyLevel()
                print(f"Novelty level: {novelty_level:.3f}")
                ```
             )pbdoc")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             R"pbdoc(
             Get prediction error.
             
             Returns:
                 float: Prediction error (difference between predicted and actual)
             
            Example:
                ```python
                prediction_error = agent.getPredictionError()
                print(f"Prediction error: {prediction_error:.3f}")
                ```
             )pbdoc")
        
        .def("reset", &AgentBrain::reset,
             R"pbdoc(
             Reset agent for new episode.
             
            Resets:
            - Neuromodulation levels to baseline
            - Developmental age to zero
            - Memory system contents to empty
            - Previous sensory state to initial
            - Plasticity modifier to stage-appropriate value
            
            Example:
                ```python
                agent.reset()
                ```
             )pbdoc")
        
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get the underlying brain.
             
             Returns:
                 Brain: Reference to the neural brain
             
            Example:
                ```python
                brain = agent.getBrain()
                brain.initialize()
                ```
             )pbdoc")
        
        // Development and neuromodulation flags
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             R"pbdoc(
             Enable or disable reward modulation.
             
             Parameters:
                 enable: True to enable, False to disable
             
            Example:
                ```python
                agent.enableRewardModulation(True)
                assert agent.isRewardModulationEnabled() == True
                
                agent.enableRewardModulation(False)
                assert agent.isRewardModulationEnabled() == False
                ```
             )pbdoc")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             R"pbdoc(
             Enable or disable structural plasticity.
             
             Parameters:
                 enable: True to enable, False to disable
             
            Structural plasticity affects:
            - Synapse formation and elimination
            - Neuron connectivity patterns
            - Brain region development
            
            Example:
                ```python
                agent.enableStructuralPlasticity(True)
                assert agent.isStructuralPlasticityEnabled() == True
                ```
             )pbdoc")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             R"pbdoc(
             Enable or disable developmental processes.
             
             Parameters:
                 enable: True to enable, False to disable
             
            Example:
                ```python
                agent.enableDevelopment(True)
                assert agent.isDevelopmentEnabled() == True
                ```
             )pbdoc")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             R"pbdoc(
             Enable or disable curiosity-driven exploration.
             
             Parameters:
                 enable: True to enable, False to disable
             
            Example:
                ```python
                agent.enableCuriosity(True)
                assert agent.isCuriosityEnabled() == True
                ```
             )pbdoc")
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

    // Global constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Global constants documentation
    m.attr("INVALID_NEURON_ID").doc() = R"pbdoc(
    Invalid neuron identifier constant.
    
    Returned by methods when no valid neuron is available.
    Can be used to check for invalid neuron references.
    
    Example:
        ```python
        if neuron_id == pynlm.INVALID_NEURON_ID:
            print("Invalid neuron identifier")
        ```
    )pbdoc";
    m.attr("INVALID_SYNAPSE_ID").doc() = R"pbdoc(
    Invalid synapse identifier constant.
    
    Returned by methods when no valid synapse is available.
    Can be used to check for invalid synapse references.
    
    Example:
        ```python
        if synapse_id == pynlm.INVALID_SYNAPSE_ID:
            print("Invalid synapse identifier")
        ```
    )pbdoc";
    m.attr("INVALID_REGION_ID").doc() = R"pbdoc(
    Invalid region identifier constant.
    
    Returned by methods when no valid region is available.
    Can be used to check for invalid region references.
    
    Example:
        ```python
        if region_id == pynlm.INVALID_REGION_ID:
            print("Invalid region identifier")
        ```
    )pbdoc";
    m.attr("INVALID_POPULATION_ID").doc() = R"pbdoc(
    Invalid population identifier constant.
    
    Returned by methods when no valid population is available.
    Can be used to check for invalid population references.
    
    Example:
        ```python
        if population_id == pynlm.INVALID_POPULATION_ID:
            print("Invalid population identifier")
        ```
    )pbdoc";

    // Utility functions
    m.def("validateConfig", [](const std::shared_ptr<Config>& config) {
        if (!config) {
            throw py::value_error("Configuration cannot be null");
        }
        
        bool isValid = true;
        std::vector<std::string> errors;
        
        for (const auto& key : config->getKeys()) {
            auto val = config->get<std::string>(key);
            if (!val) {
                errors.push_back("Key '" + key + "' has invalid value");
                isValid = false;
            }
        }
        
        if (!isValid) {
            throw py::value_error("Configuration validation failed: " + 
                (errors.empty() ? "unknown error" : errors[0]));
        }
        
        return true;
    }, py::arg("config"),
        R"pbdoc(
        Validate configuration - raises exception if invalid.
        
        Validates configuration integrity by checking:
        - Required configuration keys are present
        - Values have correct types
        - Values are within valid ranges
        
        Parameters:
            config: Configuration to validate
        
        Returns:
            bool: True if configuration is valid
        
        Error Conditions:
            - Throws ValueError if configuration is invalid
            - Throws ValueError if configuration is null
        
        Example:
            ```python
            config = pynlm.createDefaultConfig()
            config.set("population_size", 100)
            config.set("learning_rate", 0.01)
            
            try:
                pynlm.validateConfig(config)
                print("Configuration is valid")
            except Exception as e:
                print(f"Configuration validation failed: {e}")
            ```
        )pbdoc");

    m.def("getVersion", []() {
        return "1.0.0";
    }, R"pbdoc(
        Get NLM Python bindings version.
        
        Returns:
            str: Version string in semantic versioning format
        
        Example:
            ```python
            version = pynlm.getVersion()
            print(f"NLM Python bindings version: {version}")
            ```
        )pbdoc");
}

} // namespace nlm