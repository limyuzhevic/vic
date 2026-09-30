#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

// Custom exception classes for Python bindings
class NLMConfigException : public std::runtime_error {
public:
    explicit NLMConfigException(const std::string& msg) : std::runtime_error(msg) {}
};

class NLMBrainException : public std::runtime_error {
public:
    explicit NLMBrainException(const std::string& msg) : std::runtime_error(msg) {}
};

class NLMWorldException : public std::runtime_error {
public:
    explicit NLMWorldException(const std::string& msg) : std::runtime_error(msg) {}
};

class NLMInputException : public std::runtime_error {
public:
    explicit NLMInputException(const std::string& msg) : std::runtime_error(msg) {}
};

// Helper functions for error handling and validation
namespace nlm {
    
    inline void validateIndex(size_t index, size_t maxSize, const std::string& operation) {
        if (index >= maxSize) {
            throw NLMInputException("Index out of bounds in " + operation + ": " + 
                                  std::to_string(index) + " >= " + std::to_string(maxSize));
        }
    }
    
    inline void validateNotNull(const void* ptr, const std::string& operation) {
        if (ptr == nullptr) {
            throw NLMInputException("Null pointer in " + operation + ": pointer must not be null");
        }
    }
    
    inline void validateRange(float value, float min, float max, const std::string& operation) {
        if (value < min || value > max) {
            throw NLMInputException("Value out of range in " + operation + ": " +
                                  std::to_string(value) + " not in [" + std::to_string(min) +
                                  ", " + std::to_string(max) + "]");
        }
    }
    
    inline void validateSize(size_t size, size_t expected, const std::string& operation) {
        if (size != expected) {
            throw NLMInputException("Size mismatch in " + operation + ": " +
                                  std::to_string(size) + " != " + std::to_string(expected));
        }
    }
    
    inline std::string formatNeuronId(const NeuronId& id) {
        return "<NeuronId: " + std::to_string(id.value) + ">";
    }
    
    inline std::string formatSynapseId(const SynapseId& id) {
        return "<SynapseId: " + std::to_string(id.value) + ">";
    }
    
    inline std::string formatRegionId(const RegionId& id) {
        return "<RegionId: " + std::to_string(id.value) + ">";
    }
    
    inline std::string formatPopulationId(const PopulationId& id) {
        return "<PopulationId: " + std::to_string(id.value) + ">";
    }
}

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Key Features:
        - Enhanced error handling with custom exception types
        - Comprehensive input validation and bounds checking
        - Detailed documentation with usage examples
        - Improved resource management and exception safety
        - Factory methods and convenience functions
        
        Usage Example:
        ```python
        import pynlm
        
        # Create a brain with default configuration
        brain = pynlm.createBrain()
        
        # Create a simple world
        world = pynlm.createSimpleWorld()
        world.configure(100, 100, 20, 20)
        
        # Create agent brain interface
        agent_brain = pynlm.createAgentBrain(brain)
        
        # Initialize and run simulation
        agent_brain.initialize(world)
        for step in range(100):
            percept = world.getSensoryPercept()
            agent_brain.processSensoryInput(percept)
            action = agent_brain.decodeMotorCommand()
            world.applyMotorCommand(action, step)
        ```
    )pbdoc";

    // Register custom exceptions for Python
    py::register_exception<NLMConfigException>(m, "NLMConfigError");
    py::register_exception<NLMBrainException>(m, "NLMBrainError");
    py::register_exception<NLMWorldException>(m, "NLMWorldError");
    py::register_exception<NLMInputException>(m, "NLMInputError");
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // NeuronId class with enhanced documentation
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron in the neural network.
        
        Attributes:
            value (int): The internal integer identifier for the neuron
        
        Examples:
            >>> nid = NeuronId(42)
            >>> print(nid)
            <NeuronId: 42>
        "pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"), 
             "Create a NeuronId with the given value")
        .def_readwrite("value", &NeuronId::value, "The neuron's identifier value")
        .def("index", &NeuronId::index, 
             "Get the neuron index (value truncated to 32 bits)")
        .def("__eq__", &NeuronId::operator==, py::is_operator(), "Check equality")
        .def("__ne__", &NeuronId::operator!=, py::is_operator(), "Check inequality")
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); },
             "Get hash for use in dictionaries and sets")
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        }, "String representation")
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        }, "String representation");

    // SynapseId class with enhanced documentation
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse in the neural network.
        
        Attributes:
            value (int): The internal integer identifier for the synapse
        
        Examples:
            >>> sid = SynapseId(100)
            >>> print(sid)
            <SynapseId: 100>
        "pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"), 
             "Create a SynapseId with the given value")
        .def_readwrite("value", &SynapseId::value, "The synapse identifier value")
        .def("index", &SynapseId::index, 
             "Get the synapse index (value truncated to 32 bits)")
        .def("__eq__", &SynapseId::operator==, py::is_operator(), "Check equality")
        .def("__ne__", &SynapseId::operator!=, py::is_operator(), "Check inequality")
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); },
             "Get hash for use in dictionaries and sets")
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        }, "String representation")
        .def("__str__", [](const SynapseId& id) {
            return "SynapseId(" + std::to_string(id.value) + ")";
        }, "String representation");

    // RegionId class with enhanced documentation
    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a neural region in the brain.
        
        Attributes:
            value (int): The internal integer identifier for the region
        
        Examples:
            >>> rid = RegionId(1)
            >>> print(rid)
            <RegionId: 1>
        "pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"), 
             "Create a RegionId with the given value")
        .def_readwrite("value", &RegionId::value, "The region identifier value")
        .def("index", &RegionId::index, 
             "Get the region index (value truncated to 32 bits)")
        .def("__eq__", &RegionId::operator==, py::is_operator(), "Check equality")
        .def("__ne__", &RegionId::operator!=, py::is_operator(), "Check inequality")
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); },
             "Get hash for use in dictionaries and sets")
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        }, "String representation")
        .def("__str__", [](const RegionId& id) {
            return "RegionId(" + std::to_string(id.value) + ")";
        }, "String representation");

    // PopulationId class with enhanced documentation
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population in a neural region.
        
        Attributes:
            value (int): The internal integer identifier for the population
        "pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"), "Create a PopulationId")
        .def_readwrite("value", &PopulationId::value, "The population identifier value")
        .def("index", &PopulationId::index, "Get the population index")
        .def("__eq__", &PopulationId::operator==, py::is_operator())
        .def("__ne__", &PopulationId::operator!=, py::is_operator());

    // NeuronType enum with enhanced documentation
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Type enumeration for different neuron kinds in the network.
        
        Enum values:
            - Excitatory: Standard excitatory neurons
            - Inhibitory: Standard inhibitory neurons  
            - Modulatory: Neuromodulatory neurons
            - Sensory: Sensory input neurons
            - Motor: Motor output neurons
            - Internal: Internal processing neurons
        "pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Standard excitatory neurons")
        .value("Inhibitory", NeuronType::Inhibitory, "Standard inhibitory neurons")
        .value("Modulatory", NeuronType::Modulatory, "Neuromodulatory neurons")
        .value("Sensory", NeuronType::Sensory, "Sensory input neurons")
        .value("Motor", NeuronType::Motor, "Motor output neurons")
        .value("Internal", NeuronType::Internal, "Internal processing neurons")
        .export_values();

    // SynapseType enum with enhanced documentation
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Type enumeration for different synapse connection types.
        
        Enum values:
            - Excitatory: Standard chemical excitatory synapses
            - Inhibitory: Standard chemical inhibitory synapses
            - Modulatory: Neuromodulatory synapses
            - Electrical: Electrical synapses (gap junctions)
            - GapJunction: Specialized electrical coupling
        "pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Standard chemical excitatory synapses")
        .value("Inhibitory", SynapseType::Inhibitory, "Standard chemical inhibitory synapses")
        .value("Modulatory", SynapseType::Modulatory, "Neuromodulatory synapses")
        .value("Electrical", SynapseType::Electrical, "Electrical synapses")
        .value("GapJunction", SynapseType::GapJunction, "Specialized electrical coupling")
        .export_values();

    // DevelopmentalStage enum with enhanced documentation
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Enumeration of brain developmental stages.
        
        Enum values:
            - Initial: Just after birth/initialization
            - CriticalPeriod: Period of high plasticity
            - Maturation: Continued development and refinement
            - Adult: Fully mature system
            - Aging: Age-related changes and decline
        "pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "High plasticity period")
        .value("Maturation", DevelopmentalStage::Maturation, "Continued development")
        .value("Adult", DevelopmentalStage::Adult, "Fully mature system")
        .value("Aging", DevelopmentalStage::Aging, "Age-related changes")
        .export_values();

    // FiringState enum with enhanced documentation
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Current firing state of a neuron.
        
        Enum values:
            - Resting: Not currently firing
            - Active: Currently firing action potentials
            - Refractory: Post-spike recovery period
            - Inhibited: Temporarily unable to fire
        "pbdoc")
        .value("Resting", FiringState::Resting, "Not currently firing")
        .value("Active", FiringState::Active, "Currently firing action potentials")
        .value("Refractory", FiringState::Refractory, "Post-spike recovery period")
        .value("Inhibited", FiringState::Inhibited, "Temporarily unable to fire")
        .export_values();

    // ActionType enum with enhanced documentation
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Type of motor action that can be executed.
        
        Enum values:
            - MoveForward: Move forward in the environment
            - MoveBackward: Move backward
            - MoveLeft: Move left
            - MoveRight: Move right
            - TurnLeft: Rotate counterclockwise
            - TurnRight: Rotate clockwise
            - Look: Change gaze direction
            - LookUp: Look upward
            - LookDown: Look downward
            - Interact: Interact with world object
            - Eat: Consume food resource
            - Drink: Consume drink resource
            - Rest: Stay stationary
            - Wait: Do nothing for this timestep
            - Custom: User-defined action
        "pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward in the environment")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward")
        .value("MoveLeft", ActionType::MoveLeft, "Move left")
        .value("MoveRight", ActionType::MoveRight, "Move right")
        .value("TurnLeft", ActionType::TurnLeft, "Rotate counterclockwise")
        .value("TurnRight", ActionType::TurnRight, "Rotate clockwise")
        .value("Look", ActionType::Look, "Change gaze direction")
        .value("LookUp", ActionType::LookUp, "Look upward")
        .value("LookDown", ActionType::LookDown, "Look downward")
        .value("Interact", ActionType::Interact, "Interact with world object")
        .value("Eat", ActionType::Eat, "Consume food resource")
        .value("Drink", ActionType::Drink, "Consume drink resource")
        .value("Rest", ActionType::Rest, "Stay stationary")
        .value("Wait", ActionType::Wait, "Do nothing for this timestep")
        .value("Custom", ActionType::Custom, "User-defined action")
        .export_values();

    // MotorCommand enum with enhanced documentation
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Low-level motor command for agent control.
        
        Enum values:
            - MoveForward: Move forward
            - MoveBackward: Move backward
            - TurnLeft: Rotate counterclockwise
            - TurnRight: Rotate clockwise
            - LookLeft: Look left
            - LookRight: Look right
            - Interact: Interact with object
            - Wait: Do nothing
        "pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward")
        .value("TurnLeft", MotorCommand::TurnLeft, "Rotate counterclockwise")
        .value("TurnRight", MotorCommand::TurnRight, "Rotate clockwise")
        .value("LookLeft", MotorCommand::LookLeft, "Look left")
        .value("LookRight", MotorCommand::LookRight, "Look right")
        .value("Interact", MotorCommand::Interact, "Interact with object")
        .value("Wait", MotorCommand::Wait, "Do nothing")
        .export_values();

    // WorldObjectType enum with enhanced documentation
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        Type of object in the world environment.
        
        Enum values:
            - Empty: No object at this position
            - Resource: Consumable resource (food, drink)
            - Hazard: Harmful object (poison, obstacle)
            - Wall: Impassable barrier
            - Marker: Special landmark or target
        "pbdoc")
        .value("Empty", WorldObjectType::Empty, "No object at this position")
        .value("Resource", WorldObjectType::Resource, "Consumable resource")
        .value("Hazard", WorldObjectType::Hazard, "Harmful object")
        .value("Wall", WorldObjectType::Wall, "Impassable barrier")
        .value("Marker", WorldObjectType::Marker, "Special landmark")
        .export_values();

    // Config class with enhanced documentation and error handling
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for the NLM system.
        
        This class manages configuration parameters with type-safe access
        and validation. It supports loading from files, command line arguments,
        and runtime settings.
        
        Examples:
            >>> config = Config()
            >>> config.loadFromFile("config.json")
            >>> learning_rate = config.getOr(0.01, "learning_rate")
            
        Attributes:
            Keys are stored as strings, values can be int, float, bool, string, or arrays
        "pbdoc")
        .def(py::init<>(), 
             "Create an empty configuration with default values")
        .def("loadFromFile", 
             [](Config& self, const std::string& filepath) {
                 try {
                     if (filepath.empty()) {
                         throw NLMConfigException("Filepath cannot be empty");
                     }
                     bool success = self.loadFromFile(filepath);
                     if (!success) {
                         throw NLMConfigException("Failed to load configuration from file: " + filepath);
                     }
                     return success;
                 } catch (const std::exception& e) {
                     throw NLMConfigException(std::string("loadFromFile failed: ") + e.what());
                 }
             },
             py::arg("filepath"),
             R"pbdoc(
             Load configuration from a JSON file.
             
             Args:
                 filepath: Path to the JSON configuration file
                 
             Returns:
                 bool: True if loaded successfully
                 
             Raises:
                 NLMConfigError: If file cannot be read, parsed, or is invalid
                 
             Examples:
                 >>> config.loadFromFile("my_config.json")
             "pbdoc")
        .def("loadFromArgs",
             [](Config& self, int argc, char** argv) {
                 try {
                     if (argc <= 0 || argv == nullptr) {
                         throw NLMInputException("Invalid command line arguments");
                     }
                     bool success = self.loadFromArgs(argc, argv);
                     if (!success) {
                         throw NLMConfigException("Failed to load configuration from command line arguments");
                     }
                     return success;
                 } catch (const std::exception& e) {
                     throw NLMConfigException(std::string("loadFromArgs failed: ") + e.what());
                 }
             },
             py::arg("argc"), py::arg("argv"),
             R"pbdoc(
             Load configuration from command line arguments.
             
             Args:
                 argc: Number of command line arguments
                 argv: Array of command line argument strings
                 
             Returns:
                 bool: True if loaded successfully
                 
             Raises:
                 NLMInputError: If arguments are invalid
                 NLMConfigError: If parsing fails
             "pbdoc")
        .def("saveToFile",
             [](Config& self, const std::string& filepath) {
                 try {
                     if (filepath.empty()) {
                         throw NLMInputException("Filepath cannot be empty");
                     }
                     bool success = self.saveToFile(filepath);
                     if (!success) {
                         throw NLMConfigException("Failed to save configuration to file: " + filepath);
                     }
                     return success;
                 } catch (const std::exception& e) {
                     throw NLMConfigException(std::string("saveToFile failed: ") + e.what());
                 }
             },
             py::arg("filepath"),
             R"pbdoc(
             Save configuration to a JSON file.
             
             Args:
                 filepath: Path where to save the configuration
                 
             Returns:
                 bool: True if saved successfully
                 
             Raises:
                 NLMConfigError: If file cannot be written
             "pbdoc")
        .def("has", &Config::has, py::arg("key"),
             R"pbdoc(
             Check if a configuration key exists.
             
             Args:
                 key: Configuration key to check
                 
             Returns:
                 bool: True if the key exists
             "pbdoc")
        .def("get", 
             [](const Config& self, const std::string& key) {
                 auto value = self.get<std::string>(key);
                 if (value) {
                     return *value;
                 } else {
                     throw NLMConfigException("Configuration key not found: " + key);
                 }
             },
             py::arg("key"),
             R"pbdoc(
             Get a configuration value as a string.
             
             Args:
                 key: Configuration key
                 
             Returns:
                 str: The configuration value as a string
                 
             Raises:
                 NLMConfigError: If the key does not exist
             "pbdoc")
        .def("getOr", 
             [](const Config& self, const std::string& key, const std::string& defaultValue) {
                 auto value = self.get<std::string>(key);
                 return value.value_or(defaultValue);
             },
             py::arg("key"), py::arg("defaultValue"),
             R"pbdoc(
             Get a configuration value with a default fallback.
             
             Args:
                 key: Configuration key
                 defaultValue: Default value if key does not exist
                 
             Returns:
                 str: The configuration value or default
             "pbdoc")
        .def("getKeys", &Config::getKeys,
             R"pbdoc(
             Get all configuration keys.
             
             Returns:
                 list[str]: List of all configuration keys
             "pbdoc")
        .def("clear", &Config::clear,
             R"pbdoc(
             Clear all configuration entries.
             
             This removes all configuration data from the Config instance.
             "pbdoc")
        .def("summary", &Config::summary,
             R"pbdoc(
             Get a summary string of the configuration.
             
             Returns:
                 str: Summary of all configuration entries
             "pbdoc")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        }, "String representation")
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        }, "String representation");

    // Vision class with enhanced documentation
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input representing a 2D image or visual field.
        
        This class represents visual information with dimensions for width,
        height, and color channels. It stores pixel data as floating-point values.
        
        Examples:
            >>> vision = Vision(80, 60, 3)  # 80x60 RGB image
            >>> vision.setData(pixel_array)
            >>> pixels = vision.getData()
        "pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(),
             py::arg("width"), py::arg("height"), py::arg("channels") = 3,
             "Create Vision with specified dimensions")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw NLMInputException("Vision data cannot be empty");
            }
            // Validate dimensions if vision was pre-allocated
            auto dims = self.getDimensions();
            if (dims > 0) {
                size_t expectedSize = (dims == 3) ? (self.getWidth() * self.getHeight() * self.getChannels())
                                                 : (self.getWidth() * self.getHeight());
                if (data.size() != expectedSize) {
                    throw NLMInputException("Data size mismatch: expected " + std::to_string(expectedSize) +
                                          " elements, got " + std::to_string(data.size()));
                }
            }
            self.setData(data);
        }, py::arg("data"),
             "Set vision data from a vector of float values")
        .def("setData", [](Vision& self, float* data, size_t size) {
            if (data == nullptr) {
                throw NLMInputException("Vision data pointer cannot be null");
            }
            if (size == 0) {
                throw NLMInputException("Vision data size cannot be zero");
            }
            self.setData(data, size);
        }, py::arg("data"), py::arg("size"),
             "Set vision data from a raw pointer")
        .def("getWidth", &Vision::getWidth,
             "Get image width in pixels")
        .def("getHeight", &Vision::getHeight,
             "Get image height in pixels")
        .def("getChannels", &Vision::getChannels,
             "Get number of color channels (e.g., 1=grayscale, 3=RGB)")
        .def("getType", &Vision::getType,
             "Get sensory input type (always returns 'Vision')")
        .def("getData", &Vision::getData,
             "Get the raw pixel data as a vector")
        .def("getDimensions", &Vision::getDimensions,
             "Get total number of elements in the data vector")
        .def("getTimestamp", &Vision::getTimestamp,
             "Get timestamp of when this data was captured")
        .def("setTimestamp", &Vision::setTimestamp, py::arg("timestamp"),
             "Set timestamp for this sensory input");

    // Audio class with enhanced documentation
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input representing sound or audio data.
        
        This class represents audio information with sample rate and duration.
        Audio data is stored as floating-point samples.
        
        Examples:
            >>> audio = Audio(44100, 2000)  # 44.1kHz sample rate, 2 seconds
            >>> audio.setData(audio_samples)
            >>> samples = audio.getData()
        "pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(),
             py::arg("sampleRate"), py::arg("numSamples"),
             "Create Audio with specified sample rate and duration")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw NLMInputException("Audio data cannot be empty");
            }
            // Validate dimensions if audio was pre-allocated
            auto dims = self.getDimensions();
            if (dims > 0) {
                if (data.size() != self.getNumSamples()) {
                    throw NLMInputException("Data size mismatch: expected " + std::to_string(self.getNumSamples()) +
                                          " samples, got " + std::to_string(data.size()));
                }
            }
            self.setData(data);
        }, py::arg("data"),
             "Set audio data from a vector of float samples")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set the audio sample rate in Hz")
        .def("getSampleRate", &Audio::getSampleRate,
             "Get the audio sample rate in Hz")
        .def("getNumSamples", &Audio::getNumSamples,
             "Get number of audio samples")
        .def("getType", &Audio::getType,
             "Get sensory input type (always returns 'Audio')")
        .def("getData", &Audio::getData,
             "Get the raw audio samples as a vector")
        .def("getDimensions", &Audio::getDimensions,
             "Get total number of audio samples")
        .def("getTimestamp", &Audio::getTimestamp,
             "Get timestamp of when this data was captured")
        .def("setTimestamp", &Audio::setTimestamp, py::arg("timestamp"),
             "Set timestamp for this sensory input");

    // InternalSignals class with enhanced documentation
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal signals sensory input representing brain-internal state.
        
        This class represents internal neuromodulatory and homeostatic signals
        generated by the brain's internal systems. Examples include dopamine,
        curiosity, novelty, and prediction error signals.
        
        Examples:
            >>> signals = InternalSignals()
            >>> signals.addSignal(0.5)
            >>> signals.addSignal(0.3)
            >>> data = signals.getData()
        "pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value to the internal signals vector")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all internal signals")
        .def("getType", &InternalSignals::getType,
             "Get sensory input type (always returns 'InternalSignals')")
        .def("getData", &InternalSignals::getData,
             "Get the internal signal values as a vector")
        .def("getDimensions", &InternalSignals::getDimensions,
             "Get number of internal signals")
        .def("getTimestamp", &InternalSignals::getTimestamp,
             "Get timestamp of when these signals were generated")
        .def("setTimestamp", &InternalSignals::setTimestamp, py::arg("timestamp"),
             "Set timestamp for these internal signals");

    // Action class with enhanced documentation
    py::class_<Action>(m, "Action", R"pbdoc(
        Motor action representation for agent control.
        
        An Action specifies what movement or behavior the agent should perform.
        It includes both the type of action and any associated parameters.
        
        Examples:
            >>> action = Action(ActionType.MoveForward)
            >>> action = Action(ActionType.Look, [0.5, 0.0])
            >>> print(action.getType())
            'MoveForward'
        "pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create Action with specified type only")
        .def(py::init<ActionType, std::vector<float>>(),
             py::arg("type"), py::arg("parameters"),
             "Create Action with type and parameters")
        .def("getType", &Action::getType,
             "Get the action type")
        .def("setType", &Action::setType, py::arg("type"),
             "Set the action type")
        .def("getParameters", &Action::getParameters,
             "Get action parameters as a list of floats")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set action parameters from a list of floats")
        .def("getName", &Action::getName,
             "Get human-readable name of the action type")
        .def("clone", &Action::clone,
             "Create a copy of this action");

    // WorldObject class with enhanced documentation
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        Representation of an object in the world environment.
        
        World objects have physical properties like position, size, and type,
        as well as semantic properties like value (reward) and activity state.
        
        Examples:
            >>> obj = WorldObject(1.0, 2.0, WorldObjectType.Resource, 10.0, 0.5)
            >>> print(f"Position: ({obj.x}, {obj.y}), Type: {obj.type}")
        "pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Create WorldObject with position, type, and optional value/radius")
        .def_readwrite("x", &WorldObject::x, "X coordinate position")
        .def_readwrite("y", &WorldObject::y, "Y coordinate position")
        .def_readwrite("radius", &WorldObject::radius, "Radius of the object")
        .def_readwrite("type", &WorldObject::type, "Type of object (WorldObjectType enum)")
        .def_readwrite("value", &WorldObject::value, 
                      "Value: reward for resources, damage for hazards")
        .def_readwrite("active", &WorldObject::active, "Whether object is active/enabled");

    // AgentBody class with enhanced documentation
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Physical state representation of an agent in the environment.
        
        Contains kinematic properties (position, velocity, orientation) and
        physiological properties (energy, health, age).
        
        Examples:
            >>> body = AgentBody()
            >>> body.x = 5.0
            >>> body.y = 3.0
            >>> body.velocityX = 1.2
            >>> body.orientation = 0.5  # radians
        "pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position")
        .def_readwrite("y", &AgentBody::y, "Y position")
        .def_readwrite("orientation", &AgentBody::orientation, 
                      "Orientation in radians (0 = facing east)")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity component")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity component")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity,
                      "Angular velocity (radians per timestep)")
        .def_readwrite("energy", &AgentBody::energy, "Current energy level (0-1)")
        .def_readwrite("health", &AgentBody::health, "Current health (0-1)")
        .def_readwrite("age", &AgentBody::age, "Agent age in timesteps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is currently moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is currently turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime,
                      "Time since last action was taken")
        .def("reset", &AgentBody::reset,
             "Reset agent body to default state");

    // ActionResult class with enhanced documentation
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Result of an action executed in the world.
        
        Contains the outcome of an action including reward received,
        whether the action succeeded, and any descriptive message.
        
        Examples:
            >>> result = ActionResult(1.0, True, "Successfully ate food")
            >>> print(f"Reward: {result.reward}, Success: {result.success}")
        "pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create ActionResult with reward, success flag, and message")
        .def_readwrite("reward", &ActionResult::reward, "Reward received from action")
        .def_readwrite("success", &ActionResult::success, "Whether action succeeded")
        .def_readwrite("message", &ActionResult::message, 
                      "Descriptive message about action outcome");

    // SensoryPercept class with enhanced documentation
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Complete sensory perception data from all modalities.
        
        Contains vision, touch, internal signals, and proprioception data
        captured in a single timestep. Includes timestamp for temporal alignment.
        
        Examples:
            >>> percept = SensoryPercept()
            >>> vision = Vision(20, 20)
            >>> percept.setVision(vision)
            >>> touch = [...]  # touch data
            >>> percept.setTouch(touch)
        "pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision,
             "Get vision sensory data")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision sensory data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth,
             "Get vision image width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight,
             "Get vision image height")
        .def("getTouch", &SensoryPercept::getTouch,
             "Get touch sensory data")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch sensory data")
        .def("getInternal", &SensoryPercept::getInternal,
             "Get internal signals sensory data")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal signals sensory data")
        .def("getProprioception", &SensoryPercept::getProprioception,
             "Get proprioception sensory data")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioception sensory data")
        .def("getAudio", &SensoryPercept::getAudio,
             "Get audio sensory data")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio sensory data")
        .def("getAllSignals", &SensoryPercept::getAllSignals,
             "Get all sensory signals as a combined vector")
        .def("getTimestamp", &SensoryPercept::getTimestamp,
             "Get timestamp of this perception")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set timestamp for this perception");

    // SimpleWorld class with enhanced documentation and error handling
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation.
        
        Provides a simulated environment with physics, objects, and sensory feedback.
        The world handles agent movement, collisions, and interaction with objects.
        
        Examples:
            >>> world = SimpleWorld()
            >>> world.configure(100, 100, 20, 20)
            >>> world.reset()
            >>> world.setAgentStart(50, 50)
            >>> for step in range(100):
            ...     percept = world.getSensoryPercept()
            ...     # Process percept with agent brain
            ...     action_result = world.applyMotorCommand(MotorCommand.MoveForward, step)
        "pbdoc")
        .def(py::init<>())
        .def("configure",
             [](SimpleWorld& self, float width, float height, 
                size_t visionWidth, size_t visionHeight) {
                 try {
                     if (width <= 0 || height <= 0) {
                         throw NLMWorldException("World dimensions must be positive");
                     }
                     if (visionWidth == 0 || visionHeight == 0) {
                         throw NLMWorldException("Vision dimensions must be positive");
                     }
                     self.configure(width, height, visionWidth, visionHeight);
                 } catch (const std::exception& e) {
                     throw NLMWorldException(std::string("configure failed: ") + e.what());
                 }
             },
             py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             R"pbdoc(
             Configure world dimensions and vision.
             
             Args:
                 width: World width in arbitrary units
                 height: World height in arbitrary units
                 visionWidth: Width of agent's vision field
                 visionHeight: Height of agent's vision field
                 
             Raises:
                 NLMWorldError: If dimensions are invalid
             "pbdoc")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", 
             [](SimpleWorld& self, float x, float y) {
                 try {
                     if (!self.isValidPosition(x, y)) {
                         throw NLMWorldException("Invalid agent start position");
                     }
                     self.setAgentStart(x, y);
                 } catch (const std::exception& e) {
                     throw NLMWorldException(std::string("setAgentStart failed: ") + e.what());
                 }
             },
             py::arg("x"), py::arg("y"),
             R"pbdoc(
             Set agent starting position.
             
             Args:
                 x: X coordinate
                 y: Y coordinate
                 
             Raises:
                 NLMWorldError: If position is invalid (e.g., in a wall)
             "pbdoc")
        .def("update", 
             [](SimpleWorld& self, double timestep) {
                 try {
                     if (timestep <= 0) {
                         throw NLMWorldException("Timestep must be positive");
                     }
                     self.update(timestep);
                 } catch (const std::exception& e) {
                     throw NLMWorldException(std::string("update failed: ") + e.what());
                 }
             },
             py::arg("timestep"),
             R"pbdoc(
             Update world state for one timestep.
             
             Args:
                 timestep: Time duration to advance (must be > 0)
                 
             Raises:
                 NLMWorldError: If update fails
             "pbdoc")
        .def("applyMotorCommand",
             [](SimpleWorld& self, MotorCommand cmd, double currentTime) {
                 try {
                     if (currentTime < 0) {
                         throw NLMWorldException("Current time cannot be negative");
                     }
                     return self.applyMotorCommand(cmd, currentTime);
                 } catch (const std::exception& e) {
                     throw NLMWorldException(std::string("applyMotorCommand failed: ") + e.what());
                 }
             },
             py::arg("cmd"), py::arg("currentTime"),
             R"pbdoc(
             Apply a motor command to the agent.
             
             Args:
                 cmd: Motor command to execute
                 currentTime: Current simulation time
                 
             Returns:
                 ActionResult: Outcome of the action
                 
             Raises:
                 NLMWorldError: If command execution fails
             "pbdoc")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get current sensory percept for the agent.
             
             Returns:
                 SensoryPercept: Current sensory observations
             "pbdoc")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get current agent body state.
             
             Returns:
                 AgentBody: Current agent physical state
             "pbdoc")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             R"pbdoc(
             Add an object to the world.
             
             Args:
                 obj: WorldObject to add
             "pbdoc")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             R"pbdoc(
             Remove object at position.
             
             Args:
                 x: X coordinate
                 y: Y coordinate
             "pbdoc")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             R"pbdoc(
             Check if position is valid (not inside a wall).
             
             Args:
                 x: X coordinate
                 y: Y coordinate
                 
             Returns:
                 bool: True if position is valid
             "pbdoc")
        .def("getWidth", &SimpleWorld::getWidth,
             R"pbdoc(
             Get world width.
             
             Returns:
                 float: World width
             "pbdoc")
        .def("getHeight", &SimpleWorld::getHeight,
             R"pbdoc(
             Get world height.
             
             Returns:
                 float: World height
             "pbdoc")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             R"pbdoc(
             Get maximum agent energy.
             
             Returns:
                 float: Maximum energy
             "pbdoc")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             R"pbdoc(
             Set maximum agent energy.
             
             Args:
                 e: Maximum energy value
             "pbdoc")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             R"pbdoc(
             Get energy decay rate.
             
             Returns:
                 float: Energy decay rate
             "pbdoc")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             R"pbdoc(
             Set energy decay rate.
             
             Args:
                 r: Energy decay rate
             "pbdoc")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             R"pbdoc(
             Get current simulation time.
             
             Returns:
                 float: Current simulation time
             "pbdoc")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             R"pbdoc(
             Set random seed for reproducible simulations.
             
             Args:
                 seed: Random seed value
             "pbdoc")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             R"pbdoc(
             Get current random seed.
             
             Returns:
                 int: Current random seed
             "pbdoc");

    // Brain class with enhanced documentation and error handling
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class.
        
        The Brain class implements real spiking neural computation with event-driven
        dynamics and integrated memory, prediction, cognition, and neuromodulation systems.
        
        Key Features:
        - Real spiking neuron dynamics with refractory periods
        - Multiple plasticity mechanisms (STDP, Hebbian, structural)
        - Memory systems (working, episodic, associative)
        - Prediction and planning systems
        - Developmental and neuromodulation systems
        
        Examples:
            >>> config = Config()
            >>> brain = Brain(config)
            >>> brain.initialize()
            >>> for step in range(100):
            ...     brain.step(step)
        "pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def("initialize",
             [](Brain& self) {
                 try {
                     bool success = self.initialize();
                     if (!success) {
                         throw NLMBrainException("Brain initialization failed");
                     }
                     return success;
                 } catch (const std::exception& e) {
                     throw NLMBrainException(std::string("initialize failed: ") + e.what());
                 }
             },
             "Initialize the brain with configuration",
             py::call_guard<py::call_guard<py::with_gil>>())
        .def("step", 
             static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             R"pbdoc(
             Perform a simulation step with step number only.
             
             Args:
                 currentStep: Current simulation step number
                 
             Raises:
                 NLMBrainError: If step execution fails
             "pbdoc")
        .def("step", 
             static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             R"pbdoc(
             Perform a simulation step with timestamp.
             
             Args:
                 currentStep: Current simulation step number
                 currentTime: Current simulation time
                 
             Raises:
                 NLMBrainError: If step execution fails
             "pbdoc")
        .def("receiveSensoryInput",
             [](Brain& self, const SensoryInput& input) {
                 try {
                     if (!input.getType() || std::string(input.getType()).empty()) {
                         throw NLMInputException("Sensory input type cannot be empty");
                     }
                     self.receiveSensoryInput(input);
                 } catch (const std::exception& e) {
                     throw NLMBrainException(std::string("receiveSensoryInput failed: ") + e.what());
                 }
             },
             py::arg("input"),
             R"pbdoc(
             Inject sensory input into the brain.
             
             Args:
                 input: Sensory input data (Vision, Audio, or InternalSignals)
                 
             Raises:
                 NLMInputError: If input is invalid
                 NLMBrainError: If injection fails
             "pbdoc")
        .def("injectCurrent",
             [](Brain& self, const NeuronId& neuron, MembranePotential current) {
                 try {
                     if (current == 0.0f) {
                         throw NLMInputException("Current cannot be zero");
                     }
                     self.injectCurrent(neuron, current);
                 } catch (const std::exception& e) {
                     throw NLMBrainException(std::string("injectCurrent failed: ") + e.what());
                 }
             },
             py::arg("neuron"), py::arg("current"),
             R"pbdoc(
             Inject current directly into a specific neuron.
             
             Args:
                 neuron: Neuron ID to inject into
                 current: Current value to inject
                 
             Raises:
                 NLMInputError: If current is invalid
                 NLMBrainError: If injection fails
             "pbdoc")
        .def("injectCurrentToNeurons",
             [](Brain& self, NeuronType type, MembranePotential current) {
                 try {
                     if (current == 0.0f) {
                         throw NLMInputException("Current cannot be zero");
                     }
                     self.injectCurrentToNeurons(type, current);
                 } catch (const std::exception& e) {
                     throw NLMBrainException(std::string("injectCurrentToNeurons failed: ") + e.what());
                 }
             },
             py::arg("type"), py::arg("current"),
             R"pbdoc(
             Inject current into all neurons of a specific type.
             
             Args:
                 type: Neuron type to target
                 current: Current value to inject
                 
             Raises:
                 NLMInputError: If current is invalid
                 NLMBrainError: If injection fails
             "pbdoc")
        .def("produceAction",
             [](Brain& self) {
                 try {
                     auto action = self.produceAction();
                     if (!action) {
                         throw NLMBrainException("Failed to produce action");
                     }
                     return action;
                 } catch (const std::exception& e) {
                     throw NLMBrainException(std::string("produceAction failed: ") + e.what());
                 }
             },
             R"pbdoc(
             Produce motor action based on neural activity.
             
             Returns:
                 Action: Motor action produced by the brain
                 
             Raises:
                 NLMBrainError: If action production fails
             "pbdoc")
        .def("reset", &Brain::reset,
             "Reset brain state to initial conditions")
        .def("save",
             [](Brain& self, const std::string& filepath) {
                 try {
                     if (filepath.empty()) {
                         throw NLMInputException("Filepath cannot be empty");
                     }
                     bool success = self.save(filepath);
                     if (!success) {
                         throw NLMBrainException("Failed to save brain state");
                     }
                     return success;
                 } catch (const std::exception& e) {
                     throw NLMBrainException(std::string("save failed: ") + e.what());
                 }
             },
             py::arg("filepath"),
             R"pbdoc(
             Save brain state to file.
             
             Args:
                 filepath: Path to save file
                 
             Returns:
                 bool: True if saved successfully
                 
             Raises:
                 NLMInputError: If filepath is invalid
                 NLMBrainError: If save operation fails
             "pbdoc")
        .def("load",
             [](Brain& self, const std::string& filepath) {
                 try {
                     if (filepath.empty()) {
                         throw NLMInputException("Filepath cannot be empty");
                     }
                     bool success = self.load(filepath);
                     if (!success) {
                         throw NLMBrainException("Failed to load brain state");
                     }
                     return success;
                 } catch (const std::exception& e) {
                     throw NLMBrainException(std::string("load failed: ") + e.what());
                 }
             },
             py::arg("filepath"),
             R"pbdoc(
             Load brain state from file.
             
             Args:
                 filepath: Path to load file from
                 
             Returns:
                 bool: True if loaded successfully
                 
             Raises:
                 NLMInputError: If filepath is invalid
                 NLMBrainError: If load operation fails
             "pbdoc")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             R"pbdoc(
             Add a new neural region.
             
             Args:
                 name: Optional region name
                 
             Returns:
                 RegionId: ID of the newly created region
             "pbdoc")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get a region by ID.
             
             Args:
                 id: Region ID
                 
             Returns:
                 NeuralRegion: The region object
                 
             Raises:
                 NLMBrainError: If region does not exist
             "pbdoc")
        .def("getRegionCount", &Brain::getRegionCount,
             R"pbdoc(
             Get the number of regions.
             
             Returns:
                 int: Number of regions
             "pbdoc")
        .def("getRegionIds", &Brain::getRegionIds,
             R"pbdoc(
             Get all region IDs.
             
             Returns:
                 list[RegionId]: List of all region IDs
             "pbdoc")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get all regions.
             
             Returns:
                 list[NeuralRegion]: All neural regions
             "pbdoc")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             R"pbdoc(
             Get total neuron count across all regions.
             
             Returns:
                 int: Total number of neurons
             "pbdoc")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             R"pbdoc(
             Get total synapse count across all regions.
             
             Returns:
                 int: Total number of synapses
             "pbdoc")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             R"pbdoc(
             Get count of active neurons.
             
             Returns:
                 int: Number of active neurons
             "pbdoc")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             R"pbdoc(
             Get count of currently firing neurons.
             
             Returns:
                 int: Number of firing neurons
             "pbdoc")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             R"pbdoc(
             Get average firing rate across all neurons.
             
             Returns:
                 float: Average firing rate (Hz)
             "pbdoc")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             R"pbdoc(
             Get excitation/inhibition balance ratio.
             
             Returns:
                 float: Excitation/inhibition ratio
             "pbdoc")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             R"pbdoc(
             Get total spike count.
             
             Returns:
                 int: Total number of spikes
             "pbdoc")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             R"pbdoc(
             Get current developmental stage.
             
             Returns:
                 DevelopmentalStage: Current developmental stage
             "pbdoc")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"),
             R"pbdoc(
             Set developmental stage.
             
             Args:
                 stage: New developmental stage
             "pbdoc")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get the configuration.
             
             Returns:
                 Config: The configuration object
             "pbdoc")
        .def("logStatus", &Brain::logStatus,
             R"pbdoc(
             Log brain status to stdout/stderr.
             
             This method outputs diagnostic information about the brain state
             including neuron counts, firing rates, and system status.
             "pbdoc");

    // AgentBrain class with enhanced documentation and error handling
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world.
        
        The AgentBrain class provides a high-level interface for connecting
        the neural brain to the world environment. It handles sensory transduction
        and motor decoding, enabling an agent to interact with its environment.
        
        Key Features:
        - Sensory input processing and injection into brain
        - Motor command decoding from neural activity
        - Reward-based neuromodulation
        - Developmental system updates
        - Curiosity and exploration mechanisms
        
        Examples:
            >>> brain = Brain(config)
            >>> agent_brain = AgentBrain(brain)
            >>> agent_brain.initialize(world)
            >>> for step in range(100):
            ...     percept = world.getSensoryPercept()
            ...     agent_brain.processSensoryInput(percept)
            ...     action = agent_brain.decodeMotorCommand()
        "pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create AgentBrain with underlying brain")
        .def("initialize",
             [](AgentBrain& self, const SimpleWorld& world) {
                 try {
                     self.initialize(world);
                 } catch (const std::exception& e) {
                     throw NLMBrainException(std::string("initialize failed: ") + e.what());
                 }
             },
             py::arg("world"),
             R"pbdoc(
             Initialize agent brain with world.
             
             Args:
                 world: World environment to connect to
                 
             Raises:
                 NLMBrainError: If initialization fails
             "pbdoc")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             R"pbdoc(
             Get expected sensory input size from brain.
             
             Returns:
                 int: Expected sensory input vector size
             "pbdoc")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             R"pbdoc(
             Get expected motor output size from brain.
             
             Returns:
                 int: Expected motor output vector size
             "pbdoc")
        .def("processSensoryInput",
             [](AgentBrain& self, const SensoryPercept& percept) {
                 try {
                     self.processSensoryInput(percept);
                 } catch (const std::exception& e) {
                     throw NLMBrainException(std::string("processSensoryInput failed: ") + e.what());
                 }
             },
             py::arg("percept"),
             R"pbdoc(
             Process sensory percept and inject into brain.
             
             Args:
                 percept: Complete sensory perception from world
                 
             Raises:
                 NLMBrainError: If processing fails
             "pbdoc")
        .def("decodeMotorCommand",
             [](AgentBrain& self) {
                 try {
                     MotorCommand cmd = self.decodeMotorCommand();
                     return cmd;
                 } catch (const std::exception& e) {
                     throw NLMBrainException(std::string("decodeMotorCommand failed: ") + e.what());
                 }
             },
             R"pbdoc(
             Decode brain motor activity into motor command.
             
             Returns:
                 MotorCommand: Decoded motor command
                 
             Raises:
                 NLMBrainError: If decoding fails
             "pbdoc")
        .def("applyRewardModulation",
             [](AgentBrain& self, float reward, float predictedReward) {
                 try {
                     if (reward < 0) {
                         throw NLMInputException("Reward cannot be negative");
                     }
                     if (predictedReward < 0) {
                         throw NLMInputException("Predicted reward cannot be negative");
                     }
                     self.applyRewardModulation(reward, predictedReward);
                 } catch (const std::exception& e) {
                     throw NLMBrainException(std::string("applyRewardModulation failed: ") + e.what());
                 }
             },
             py::arg("reward"), py::arg("predictedReward"),
             R"pbdoc(
             Apply reward-based neuromodulation.
             
             Args:
                 reward: Actual reward received
                 predictedReward: Predicted reward based on action
                 
             Raises:
                 NLMInputError: If reward values are invalid
                 NLMBrainError: If neuromodulation fails
             "pbdoc")
        .def("updateDevelopment",
             [](AgentBrain& self, double timestep) {
                 try {
                     if (timestep <= 0) {
                         throw NLMInputException("Timestep must be positive");
                     }
                     self.updateDevelopment(timestep);
                 } catch (const std::exception& e) {
                     throw NLMBrainException(std::string("updateDevelopment failed: ") + e.what());
                 }
             },
             py::arg("timestep"),
             R"pbdoc(
             Update development system.
             
             Args:
                 timestep: Time duration to advance development
                 
             Raises:
                 NLMInputError: If timestep is invalid
                 NLMBrainError: If development update fails
             "pbdoc")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             R"pbdoc(
             Get current developmental stage.
             
             Returns:
                 DevelopmentalStage: Current developmental stage
             "pbdoc")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             R"pbdoc(
             Get current neuromodulation level.
             
             Returns:
                 float: Current neuromodulation level (0-1)
             "pbdoc")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             R"pbdoc(
             Get curiosity level.
             
             Returns:
                 float: Current curiosity level (0-1)
             "pbdoc")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             R"pbdoc(
             Get novelty level.
             
             Returns:
                 float: Current novelty level (0-1)
             "pbdoc")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             R"pbdoc(
             Get prediction error.
             
             Returns:
                 float: Current prediction error
             "pbdoc")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable development system")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity system")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development system is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity system is enabled");

    // Factory methods with enhanced error handling
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        try {
            auto config = std::make_shared<Config>();
            if (!config) {
                throw NLMConfigException("Failed to create default configuration");
            }
            return config;
        } catch (const std::exception& e) {
            throw NLMConfigException(std::string("createDefaultConfig failed: ") + e.what());
        }
    }, "Create a default configuration with error handling", py::call_guard<py::call_guard<py::with_gil>>());

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        try {
            if (!config) {
                throw NLMInputException("Config cannot be null");
            }
            auto brain = std::make_shared<Brain>(config);
            if (!brain) {
                throw NLMBrainException("Failed to create brain");
            }
            return brain;
        } catch (const std::exception& e) {
            throw NLMBrainException(std::string("createBrain failed: ") + e.what());
        }
    }, py::arg("config"), "Create a new brain with configuration", py::call_guard<py::call_guard<py::with_gil>>());

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        try {
            auto world = std::make_shared<SimpleWorld>();
            if (!world) {
                throw NLMWorldException("Failed to create world");
            }
            return world;
        } catch (const std::exception& e) {
            throw NLMWorldException(std::string("createSimpleWorld failed: ") + e.what());
        }
    }, "Create a new simple world", py::call_guard<py::call_guard<py::with_gil>>());

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        try {
            if (!brain) {
                throw NLMInputException("Brain cannot be null");
            }
            auto agentBrain = std::make_shared<AgentBrain>(brain);
            if (!agentBrain) {
                throw NLMBrainException("Failed to create agent brain");
            }
            return agentBrain;
        } catch (const std::exception& e) {
            throw NLMBrainException(std::string("createAgentBrain failed: ") + e.what());
        }
    }, py::arg("brain"), "Create a new agent brain interface", py::call_guard<py::call_guard<py::with_gil>>());

    // Convenience functions
    m.def("initializeBrainWithConfig", [](std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
        try {
            if (!brain || !config) {
                throw NLMInputException("Brain and config cannot be null");
            }
            brain->reset();
            brain->getConfig()->clear();
            *brain->getConfig() = *config;
            if (!brain->initialize()) {
                throw NLMBrainException("Brain initialization failed");
            }
        } catch (const std::exception& e) {
            throw NLMBrainException(std::string("initializeBrainWithConfig failed: ") + e.what());
        }
    }, py::arg("brain"), py::arg("config"),
        "Initialize brain with configuration (convenience function)");

    m.def("runSimulationStep", [](std::shared_ptr<AgentBrain> agentBrain, 
                                 std::shared_ptr<SimpleWorld> world, 
                                 const SensoryPercept& percept, 
                                 double timestep) {
        try {
            if (!agentBrain || !world) {
                throw NLMInputException("AgentBrain and World cannot be null");
            }
            agentBrain->processSensoryInput(percept);
            MotorCommand action = agentBrain->decodeMotorCommand();
            ActionResult result = world->applyMotorCommand(action, timestep);
            return result;
        } catch (const std::exception& e) {
            throw NLMWorldException(std::string("runSimulationStep failed: ") + e.what());
        }
    }, py::arg("agentBrain"), py::arg("world"), py::arg("percept"), py::arg("timestep"),
        "Run a complete simulation step (convenience function)");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Version and build information
    m.attr("VERSION") = "1.0.0";
    m.attr("BUILD_INFO") = "Enhanced Python bindings with error handling and validation";
}

} // namespace nlm