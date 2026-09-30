#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <stdexcept>
#include <vector>
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

// Validation helper functions
inline void validatePositive(const std::string& name, double value, double min = 0.0) {
    if (value <= min) {
        throw std::invalid_argument(name + " must be > " + std::to_string(min) + ", got " + std::to_string(value));
    }
}

inline void validatePositive(const std::string& name, size_t value, size_t min = 1) {
    if (value < min) {
        throw std::invalid_argument(name + " must be >= " + std::to_string(min) + ", got " + std::to_string(value));
    }
}

inline void validatePosition(float x, float y, float minX = 0.0f, float maxX = 100.0f, float minY = 0.0f, float maxY = 100.0f) {
    if (x < minX || x >= maxX) {
        throw std::invalid_argument("Position x out of bounds: " + std::to_string(x) + ". Must be in [" + 
            std::to_string(minX) + ", " + std::to_string(maxX) + ")");
    }
    if (y < minY || y >= maxY) {
        throw std::invalid_argument("Position y out of bounds: " + std::to_string(y) + ". Must be in [" + 
            std::to_string(minY) + ", " + std::to_string(maxY) + ")");
    }
}

inline void validateDimensions(float width, float height, float minDim = 1.0f) {
    if (width <= minDim) {
        throw std::invalid_argument("Width must be > " + std::to_string(minDim) + ", got " + std::to_string(width));
    }
    if (height <= minDim) {
        throw std::invalid_argument("Height must be > " + std::to_string(minDim) + ", got " + std::to_string(height));
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgument");

    // Template function for ID type bindings
    template<typename T>
    void bindIdType(py::module_& m, const std::string& typeName, const std::string& docString) {
        py::class_<T>(m, typeName.c_str(), docString)
            .def(py::init<>(), R"pbdoc(Default constructor.
                Creates an invalid ID with value 0.
            )pbdoc")
            .def(py::init<uint64_t>(), py::arg("value"),
                R"pbdoc(Constructor with value.
                
                Args:
                    value: The unique identifier value
                
                Returns:
                    ID instance with specified value
                
                Example:
                    >>> neuron_id = NeuronId(42)
                    >>> print(neuron_id.value)
                    42
                )pbdoc")
            .def_readwrite("value", &T::value,
                R"pbdoc(ID value.
                
                Read-only access to the numeric identifier value.
                The value uniquely identifies this ID instance.
                "pbdoc")
            .def("index", &T::index,
                R"pbdoc(Get index of this ID.
                
                Returns:
                    Index of this ID (same as value)
                
                Note:
                    The index is used for internal array lookups in the brain
                    and is typically the same as the value.
                )pbdoc")
            .def("__eq__", &T::operator==,
                R"pbdoc(Check equality with another ID.
                
                Args:
                    other: ID to compare with
                
                Returns:
                    True if IDs are equal (same value), False otherwise
                )pbdoc")
            .def("__ne__", &T::operator!=,
                R"pbdoc(Check inequality with another ID.
                
                Args:
                    other: ID to compare with
                
                Returns:
                    True if IDs are different, False if equal
                )pbdoc")
            .def("__hash__", [](const T& id) { return std::hash<uint64_t>{}(id.value); },
                R"pbdoc(Generate hash for use in dictionaries and sets.
                
                Returns:
                    Hash value for this ID (based on value)
                )pbdoc")
            .def("__repr__", [](const T& id) {
                return "<" + typeName + ": " + std::to_string(id.value) + ">";
            },
                R"pbdoc(String representation for debugging.
                
                Returns:
                    Formatted string like <NeuronId: 42>
                )pbdoc")
            .def("__str__", [](const T& id) {
                return std::to_string(id.value);
            },
                R"pbdoc(String representation.
                
                Returns:
                    String representation of ID value (e.g., "42")
                )pbdoc")
            .def("to_string", [](const T& id) {
                return std::to_string(id.value);
            },
                R"pbdoc(Convert ID to string.
                
                Returns:
                    String representation of ID value
                
                Example:
                    >>> id_str = neuron_id.to_string()
                    >>> print(id_str)
                    "42"
                )pbdoc")
            .def_static("from_string", [](const std::string& str) {
                try {
                    return T(static_cast<uint64_t>(std::stoull(str)));
                } catch (const std::exception& e) {
                    throw py::python_error("Invalid string for ID type: " + std::string(e.what()));
                }
            }, py::arg("str"),
                R"pbdoc(Create an ID from a string.
                
                Args:
                    str: String representation of the ID value
                
                Returns:
                    ID instance created from string
                
                Raises:
                    InvalidArgument: If string cannot be parsed as a number
                
                Example:
                    >>> neuron_id = NeuronId.from_string("42")
                    >>> print(neuron_id.value)
                    42
                )pbdoc");
    }

    // Bind all ID types with backward compatibility
    bindIdType<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc");
    bindIdType<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc");
    bindIdType<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc");
    bindIdType<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc");

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

    // Config class with error handling
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system
    
    Central configuration management for neural simulation parameters and settings.
    Manages brain initialization, simulation parameters, and runtime configuration.
    Supports JSON file loading, command-line argument parsing, and key-value access.
    
    Key Features:
    - Load/save JSON configuration files
    - Parse command-line arguments
    - Check for configuration keys
    - Get/set string values
    - Generate configuration summaries
    - Clear/reset configuration
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", [](Config& self, const std::string& filepath) {
            try {
                if (!self.loadFromFile(filepath)) {
                    throw std::runtime_error("Failed to load configuration from file: " + filepath);
                }
                return true;
            } catch (const std::exception& e) {
                throw py::python_error(std::string("Config.loadFromFile error: ") + e.what());
            }
        }, py::arg("filepath"), R"pbdoc(Load configuration from a JSON file
        
        Args:
            filepath: Path to the JSON configuration file
        
        Returns:
            True if configuration was loaded successfully, False otherwise
        
        Raises:
            RuntimeError: If the file cannot be opened or parsed
            py::python_error: If there's an exception during loading
        
        Examples:
            >>> config = Config()
            >>> config.loadFromFile("brain_config.json")
            >>> print("Configuration loaded successfully")
        
        File Format Example (brain_config.json):
        ```json
        {
            "brain": {
                "learningRate": 0.01,
                "weightDecay": 0.0001
            },
            "world": {
                "width": 100.0,
                "height": 100.0,
                "maxEnergy": 100.0
            },
            "neuron": {
                "numLayers": 3,
                "numNeuronsPerLayer": [64, 32, 1]
            }
        }
        ```
        )pbdoc")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            try {
                return self.loadFromArgs(argc, argv);
            } catch (const std::exception& e) {
                throw py::python_error(std::string("Config.loadFromArgs error: ") + e.what());
            }
        }, py::arg("argc"), py::arg("argv"), R"pbdoc(Load configuration from command line arguments
        
        Args:
            argc: Number of command line arguments (from main)
            argv: Array of command line argument strings
        
        Returns:
            True if configuration was loaded successfully, False otherwise
        
        Raises:
            py::python_error: If there's an exception during parsing
        
        Command Line Usage Examples:
            >>> config.loadFromArgs(sys.argv.count, sys.argv.data)
            
        Common Command Line Options:
        --brain.learningRate 0.01
        --world.width 100.0
        --neuron.numLayers 3
        --verbose
        
        Configuration can also be loaded from a JSON file using --config <filename>
        )pbdoc")
        .def("saveToFile", [](Config& self, const std::string& filepath) {
            try {
                if (!self.saveToFile(filepath)) {
                    throw std::runtime_error("Failed to save configuration to file: " + filepath);
                }
                return true;
            } catch (const std::exception& e) {
                throw py::python_error(std::string("Config.saveToFile error: ") + e.what());
            }
        }, py::arg("filepath"), R"pbdoc(Save configuration to a JSON file
        
        Args:
            filepath: Path where the JSON configuration file will be saved
        
        Returns:
            True if configuration was saved successfully, False otherwise
        
        Raises:
            RuntimeError: If the file cannot be written
            py::python_error: If there's an exception during saving
        
        Returns:
            True if save was successful
        
        Generated File Format Example (brain_config.json):
        ```json
        {
            "brain": {
                "learningRate": 0.01,
                "weightDecay": 0.0001,
                "useRewardModulation": true
            },
            "world": {
                "width": 100.0,
                "height": 100.0,
                "maxEnergy": 100.0,
                "energyDecayRate": 0.01
            },
            "neuron": {
                "numLayers": 3,
                "numNeuronsPerLayer": [64, 32, 1],
                "neuronType": "Excitatory"
            },
            "development": {
                "enableDevelopment": false,
                "enableCuriosity": false,
                "enableStructuralPlasticity": false
            }
        }
        ```
        )pbdoc")
        .def("has", &Config::has, py::arg("key"), R"pbdoc(Check if a configuration key exists
        
        Args:
            key: Dot-separated configuration key path (e.g., "brain.learningRate")
        
        Returns:
            True if the key exists in configuration, False otherwise
        
        Note:
            Keys use dot notation for nested configuration. For example:
            - "brain.learningRate" checks for brain.learningRate
            - "world.width" checks for world.width
            - "neuron.numLayers" checks for neuron.numLayers
            
            The has() method performs a case-sensitive key lookup and returns
            True only if the exact key path exists in the configuration.
        
        Examples:
            >>> config.has("brain.learningRate")
            True
            >>> config.has("nonexistent.key")
            False
        )pbdoc")
        .def("getKeys", &Config::getKeys, R"pbdoc(Get all configuration keys
        
        Returns:
            List of all configuration keys (strings)
        
        Returns:
            List of all dot-separated keys in the configuration
        
        Note:
            Returns all keys at all nesting levels. For nested configurations:
            - Returns: ["brain.learningRate", "brain.weightDecay", "world.width", "world.height", "neuron.numLayers"]
        
        Examples:
            >>> all_keys = config.getKeys()
            >>> print(all_keys)
            ['brain.learningRate', 'brain.weightDecay', 'world.width', 'world.height']
        )pbdoc")
        .def("clear", &Config::clear, R"pbdoc(Clear all configuration entries
        
        Removes all key-value pairs from the configuration, resetting it to an empty state.
        
        Note:
            This completely clears the configuration. After calling clear(),
            has() will return False for all keys and get() will throw KeyError.
            The config is restored to a default-initialized state.
        
        Examples:
            >>> config.has("brain.learningRate")
            True
            >>> config.clear()
            >>> config.has("brain.learningRate")
            False
        )pbdoc")
        .def("summary", &Config::summary, R"pbdoc(Get a summary string of the configuration
        
        Returns:
            String summary of configuration state
        
        Returns:
            Formatted string containing configuration statistics and overview
        
        Note:
            The summary includes the number of configuration entries and
            can be useful for debugging and logging.
        
        Examples:
            >>> summary = config.summary()
            >>> print(summary)
            "Configuration: 5 keys loaded"
        )pbdoc")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        })
        .def("__getitem__", [](const Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (!value) {
                throw py::python_error("Key not found: " + key);
            }
            return *value;
        }, py::arg("key"))
        .def("__setitem__", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"));

    // SensoryInput classes
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input
    
    Abstract base class representing neural sensory input data.
    Provides common interface for all sensory modalities (Vision, Audio, Touch, etc.).
    Contains core sensory data properties and timestamp information.
    
    Subclasses implement modality-specific data access methods and properties.
    
    Key Features:
    - Unified sensory input interface
    - Timestamp tracking
    - Data access abstraction
    - Dimensionality information
    
    Derived Classes:
    - Vision: Visual input from agent cameras
    - Audio: Auditory input from agent ears
    - Touch: Tactile input from sensors
    - InternalSignals: Internal neural signals
    )pbdoc")
        .def("getType", &SensoryInput::getType, R"pbdoc(Get the type of sensory input
        
        Returns:
            NeuronType enum value representing the sensory modality
        
        Returns:
            The NeuronType enum value for this sensory input (e.g., NeuronType.Vision)
        
        Examples:
            >>> vision_input = SensoryPercept().getVision()
            >>> input_type = vision_input.getType()
            >>> print(input_type)
            <NeuronType.Vision: 3>
        
        Common Types:
        - NeuronType.Vision: Visual sensory input
        - NeuronType.Audio: Auditory sensory input
        - NeuronType.Touch: Tactile sensory input
        - NeuronType.Internal: Internal neural signals
        )pbdoc")
        .def("getData", &SensoryInput::getData, R"pbdoc(Get the raw data as a vector
        
        Returns:
            Vector of float values representing sensory data
        
        Returns:
            Raw sensory data as vector of floating-point values
        
        Returns:
            Vector containing sensory data values
        
        Data Format:
        - For Vision: Flattened RGB values (width × height × channels)
        - For Audio: Audio samples (sampleRate × numSamples)
        - For Touch: Touch sensor readings
        
        Examples:
            >>> vision = Vision()
            >>> data = vision.getData()
            >>> print(f"Vision data shape: {len(data)} values")
        )pbdoc")
        .def("getDimensions", &SensoryInput::getDimensions, R"pbdoc(Get the dimensionality
        
        Returns:
            Vector of integers representing data dimensions
        
        Returns:
            List of dimension sizes for each data axis
        
        Dimensions:
        - Vision: [width, height, channels]
        - Audio: [sampleRate, numSamples]
        - Touch: Varies by sensor configuration
        
        Examples:
            >>> vision = Vision(640, 480, 3)
            >>> dims = vision.getDimensions()
            >>> print(f"Vision dimensions: {dims}")
        )pbdoc")
        .def("getTimestamp", &SensoryInput::getTimestamp, R"pbdoc(Get the timestamp
        
        Returns:
            Double-precision timestamp of sensory input
        
        Returns:
            Simulation time when sensory input was generated
        
        Timestamp Format:
        - Measured in simulation time units
        - Floating-point precision for fine-grained timing
        - Monotonically increasing with simulation steps
        
        Examples:
            >>> vision = Vision()
            >>> timestamp = vision.getTimestamp()
            >>> print(f"Input timestamp: {timestamp}")
        )pbdoc")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"), R"pbdoc(Set the timestamp
        
        Args:
            timestamp: Double-precision timestamp to set
        
        Sets the simulation time when sensory input was generated.
        
        Args:
            timestamp: Simulation time for the sensory input
        
        Timestamp Notes:
        - Measured in simulation time units
        - Should represent when the sensory input occurred
        - Used for temporal alignment in neural processing
        
        Examples:
            >>> vision = Vision()
            >>> vision.setTimestamp(1234.567)
            >>> print(f"Timestamp set to: {vision.getTimestamp()}")
        )pbdoc");

    // Vision class with parameter validation
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input
    
    Visual sensory input representing camera images or visual data.
    Represents 2D visual information with RGB channels.
    
    Key Features:
    - 2D visual grid with configurable resolution
    - RGB color channels (configurable number)
    - Context manager for temporary vision data handling
    - Automatic parameter validation
    
    Vision Properties:
    - width: Horizontal resolution in pixels
    - height: Vertical resolution in pixels
    - channels: Number of color channels (typically 3 for RGB)
    
    Data Format:
    - Flattened array: width × height × channels
    - RGB channel order (configurable)
    - Data type: 32-bit floating point
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3,
             R"pbdoc(Initialize Vision with validated parameters
             
             Args:
                 width: Horizontal resolution in pixels (must be > 0)
                 height: Vertical resolution in pixels (must be > 0)
                 channels: Number of color channels (default: 3 for RGB)
             
             Raises:
                 InvalidArgument: If width or height is <= 0
             
             Examples:
                 >>> vision = Vision()  # Default: 0×0×3
                 >>> vision = Vision(640, 480, 3)  # Standard HD RGB
                 >>> vision = Vision(320, 240, 1)  # Grayscale
                 
                 >>> print(f"Vision: {vision.getWidth()}×{vision.getHeight()}×{vision.getChannels()} pixels")
             
             Typical Values:
             - Standard RGB: (640, 480, 3)
             - High Res: (1920, 1080, 3)
             - Low Res: (160, 120, 3)
             - Grayscale: (320, 240, 1)
             )pbdoc")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), R"pbdoc(Set visual data with validation
        
        Args:
            data: Vector of float values containing RGB visual data
        
        Data Format:
        - Expected size: width × height × channels
        - RGB channel order (configurable)
        - 32-bit floating point values (0.0-1.0 typical range)
        
        Validation:
        - Checks data vector size against image dimensions
        - Throws InvalidArgument if size mismatch
        
        Examples:
            >>> vision = Vision(640, 480, 3)
            >>> data = [0.1, 0.2, 0.3, 0.4, 0.5, 0.6]  # Sample RGB values
            >>> vision.setData(data)  # Will raise error for insufficient data
            
        Creating Valid Vision Data:
        ```python
        >>> width, height, channels = 2, 2, 3
        >>> # Create 2×2 RGB image with red, green, blue pixels
        >>> data = [1.0, 0.0, 0.0,  # Red pixel
                   0.0, 1.0, 0.0,  # Green pixel
                   0.0, 0.0, 1.0,  # Blue pixel
                   1.0, 1.0, 0.0]  # Yellow pixel
        >>> vision = Vision(width, height, channels)
        >>> vision.setData(data)
        ```
        )pbdoc")
        .def("getWidth", &Vision::getWidth,
             R"pbdoc(Get image width in pixels
             
             Returns:
                 Horizontal resolution in pixels
             
             Returns:
                 Width as size_t
             
             Examples:
                 >>> vision = Vision(640, 480, 3)
                 >>> print(vision.getWidth())
                 640
             )pbdoc")
        .def("getHeight", &Vision::getHeight,
             R"pbdoc(Get image height in pixels
             
             Returns:
                 Vertical resolution in pixels
             
             Returns:
                 Height as size_t
             
             Examples:
                 >>> vision = Vision(640, 480, 3)
                 >>> print(vision.getHeight())
                 480
             )pbdoc")
        .def("getChannels", &Vision::getChannels,
             R"pbdoc(Get number of color channels
             
             Returns:
                 Number of color channels (typically 3 for RGB)
             
             Returns:
                 Number of channels as size_t
             
             Examples:
                 >>> vision = Vision(640, 480, 1)  # Grayscale
                 >>> print(vision.getChannels())
                 1
                 
                 >>> vision = Vision(640, 480, 3)  # RGB
                 >>> print(vision.getChannels())
                 3
             )pbdoc")
        .def_readonly("width", &Vision::getWidth)
        .def_readonly("height", &Vision::getHeight)
        .def_readonly("channels", &Vision::getChannels)
        .def("__enter__", [](Vision& self) { return &self; },
             R"pbdoc(Context manager entry.
             
             Returns:
                 Self reference for use in with statement
             
             Enables "with Vision() as vision:" usage pattern for temporary vision objects.
             )pbdoc")
        .def("__exit__", [](Vision&, py::args) {},
             R"pbdoc(Context manager exit.
             
             Handles cleanup when exiting context manager.
             
             Currently no cleanup required, but ensures proper resource management.
             )pbdoc");

    // Audio class with parameter validation
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input
    
    Auditory sensory input representing sound or audio data.
    Represents audio signals with configurable sampling rate and duration.
    
    Key Features:
    - Audio signal processing with configurable sample rate
    - Time-domain audio representation
    - Context manager for temporary audio data handling
    - Automatic parameter validation
    
    Audio Properties:
    - sampleRate: Sampling frequency in Hertz (Hz)
    - numSamples: Number of audio samples in the signal
    
    Data Format:
    - Time-domain signal: Array of floating point samples
    - Sample rate determines time resolution
    - Data type: 32-bit floating point
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             R"pbdoc(Initialize Audio with validated parameters
             
             Args:
                 sampleRate: Sampling frequency in Hertz (Hz) (must be > 0)
                 numSamples: Number of audio samples in the signal (must be > 0)
             
             Raises:
                 InvalidArgument: If sampleRate or numSamples is <= 0
             
             Examples:
                 >>> audio = Audio()  # Default: 0 Hz, 0 samples
                 >>> audio = Audio(44100, 1024)  # Standard CD quality: 44.1kHz, 1024 samples
                 >>> audio = Audio(48000, 2048)  # Professional audio: 48kHz, 2048 samples
                 
                 >>> print(f"Audio: {audio.getSampleRate()} Hz, {audio.getNumSamples()} samples")
             
             Typical Values:
             - CD Quality: (44100, 1024)
             - Professional: (48000, 2048)
             - Telephone: (8000, 320)
             - High Res: (192000, 4096)
             )pbdoc")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), R"pbdoc(Set audio data with validation
        
        Args:
            data: Vector of float values containing audio samples
        
        Data Format:
        - Time-domain signal: Sequential audio samples
        - Sample rate determines time scale
        - Data type: 32-bit floating point (normalized typically -1.0 to 1.0)
        
        Validation:
        - Checks data vector size against numSamples
        - Throws InvalidArgument if size mismatch
        
        Examples:
            >>> audio = Audio(44100, 1024)
            >>> # Create sine wave at 440 Hz for 1 second
            >>> import math
            >>> data = [math.sin(2 * math.pi * 440 * i / 44100) for i in range(1024)]
            >>> audio.setData(data)  # Will raise error if length != 1024
            
        Audio Signal Examples:
        ```python
        >>> import math
        >>> # Create 1-second silence (all zeros)
        >>> silence = [0.0] * 44100
        >>> audio = Audio(44100, 44100)
        >>> audio.setData(silence)
        
        >>> # Create white noise
        >>> import random
        >>> noise = [random.uniform(-1.0, 1.0) for _ in range(2048)]
        >>> audio = Audio(48000, 2048)
        >>> audio.setData(noise)
        ```
        )pbdoc")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"), R"pbdoc(Set audio sample rate
        
        Args:
            sampleRate: New sampling frequency in Hertz (Hz)
        
        Updates the sample rate of the audio signal.
        
        Sample Rate Considerations:
        - Higher sample rates provide better frequency resolution
        - Common rates: 8kHz (telephone), 44.1kHz (CD), 48kHz (professional)
        - Must be > 0
        
        Examples:
            >>> audio = Audio(44100, 1024)  # 44.1kHz
            >>> print(audio.getSampleRate())
            44100
            >>> audio.setSampleRate(48000)  # Upgrade to 48kHz
            >>> print(audio.getSampleRate())
            48000
        )pbdoc")
        .def("getSampleRate", &Audio::getSampleRate,
             R"pbdoc(Get audio sample rate
             
             Returns:
                 Sampling frequency in Hertz (Hz)
             
             Returns:
                 Sample rate as size_t
             
             Returns:
                 Sample rate in Hz
             
             Examples:
                 >>> audio = Audio(44100, 1024)
                 >>> print(audio.getSampleRate())
                 44100
             )pbdoc")
        .def("getNumSamples", &Audio::getNumSamples,
             R"pbdoc(Get number of audio samples
             
             Returns:
                 Number of samples in the audio signal
             
             Returns:
                 Number of samples as size_t
             
             Examples:
                 >>> audio = Audio(44100, 1024)
                 >>> print(audio.getNumSamples())
                 1024
             )pbdoc")
        .def_readonly("sampleRate", &Audio::getSampleRate)
        .def_readonly("numSamples", &Audio::getNumSamples)
        .def("__enter__", [](Audio& self) { return &self; },
             R"pbdoc(Context manager entry.
             
             Returns:
                 Self reference for use in with statement
             
             Enables "with Audio() as audio:" usage pattern for temporary audio objects.
             )pbdoc")
        .def("__exit__", [](Audio&, py::args) {},
             R"pbdoc(Context manager exit.
             
             Handles cleanup when exiting context manager.
             
             Currently no cleanup required, but ensures proper resource management.
             )pbdoc");

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input
    
    Internal neural signals and neuromodulation data.
    Represents internal brain state, neuromodulation, and internal processing signals.
    
    Key Features:
    - Signal collection and management
    - Context manager for temporary signal handling
    - Neuromodulation signal processing
    - Internal state tracking
    
    Signal Types:
    - Reward prediction errors
    - Neuromodulation signals
    - Internal monitoring signals
    - Plasticity signals
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"), R"pbdoc(Add a signal value
        
        Args:
            value: Signal value to add to the internal signals
        
        Internal signal value to add to the collection.
        
        Signal Values:
        - Reward prediction error (positive = better than expected)
        - Neuromodulation level (0.0-1.0 typical range)
        - Internal monitoring metrics
        - Plasticity-related signals
        
        Examples:
            >>> signals = InternalSignals()
            >>> signals.addSignal(0.5)  # Add reward prediction error
            >>> signals.addSignal(1.0)  # Add maximum neuromodulation
        )pbdoc")
        .def("clearSignals", &InternalSignals::clearSignals, R"pbdoc(Clear all internal signals
        
        Removes all signal values from the internal signals collection.
        
        Use Cases:
        - Reset internal state for new episode
        - Clear accumulated signals
        - Prepare for new simulation
        
        Examples:
            >>> signals.addSignal(0.5)
            >>> signals.addSignal(1.0)
            >>> signals.clearSignals()
            >>> print(len(signals.getData()))  # Should be 0
        )pbdoc")
        .def("__enter__", [](InternalSignals& self) { return &self; },
             R"pbdoc(Context manager entry.
             
             Returns:
                 Self reference for use in with statement
             
             Enables "with InternalSignals() as signals:" usage pattern for temporary signal objects.
             )pbdoc")
        .def("__exit__", [](InternalSignals&, py::args) {},
             R"pbdoc(Context manager exit.
             
             Handles cleanup when exiting context manager.
             
             Currently no cleanup required, but ensures proper resource management.
             )pbdoc");

    // Action class
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output
    
    Motor action for agent control and world interaction.
    Represents discrete motor commands with parameters for execution.
    
    Key Features:
    - Action type enumeration (MoveForward, TurnLeft, etc.)
    - Configurable parameters for action execution
    - Action cloning for state management
    - Name generation for human-readable action descriptions
    
    Action Types:
    - MoveForward, MoveBackward: Locomotion actions
    - TurnLeft, TurnRight: Rotation actions
    - LookUp, LookDown: Gaze control actions
    - Interact, Eat, Drink: Interaction actions
    - Rest, Wait: Inactivity actions
    - Custom: User-defined actions
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"), R"pbdoc(Initialize Action with type
        
        Args:
            type: ActionType enumeration value (e.g., ActionType.MoveForward)
        
        Creates a simple action with only the type specified.
        
        Args:
            type: Action type (e.g., MoveForward, TurnLeft)
        
        Action Examples:
        ```python
        >>> action = Action(ActionType.MoveForward)
        >>> print(action.getName())
        "Move Forward"
        
        >>> action = Action(ActionType.TurnLeft, [45.0])
        >>> print(action.getName())
        "Turn Left 45.0 degrees"
        ```
        )pbdoc")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"), R"pbdoc(Initialize Action with type and parameters
        
        Args:
            type: ActionType enumeration value
            parameters: Vector of float parameters for the action
        
        Creates a complete action with type and associated parameters.
        
        Args:
            type: Action type (e.g., Look, Custom)
            parameters: List of action-specific parameters
        
        Common Parameter Patterns:
        - Movement: [distance] (for MoveForward)
        - Rotation: [angle] (for TurnLeft/TurnRight)
        - Gaze: [x, y] (for Look, LookUp, LookDown)
        - Custom: Variable length parameters
        
        Action Examples:
        ```python
        >>> move_action = Action(ActionType.MoveForward, [1.0])  # Move 1.0 unit forward
        >>> turn_action = Action(ActionType.TurnLeft, [45.0])   # Turn 45 degrees left
        >>> look_action = Action(ActionType.Look, [0.5, 0.5])  # Look at position (0.5, 0.5)
        ```
        )pbdoc")
        .def("getType", &Action::getType, R"pbdoc(Get the action type
        
        Returns:
            ActionType enumeration value
        
        Returns:
            The action type (e.g., ActionType.MoveForward)
        
        Returns:
            Action type as ActionType enum
        
        Action Type References:
        - ActionType.MoveForward: Move forward action
        - ActionType.TurnLeft: Turn left action
        - ActionType.Look: Look/gaze action
        - ActionType.Interact: Interaction action
        - ActionType.Custom: Custom action
        
        Examples:
            >>> action = Action(ActionType.MoveForward)
            >>> action_type = action.getType()
            >>> print(action_type)
            <ActionType.MoveForward: 0>
        )pbdoc")
        .def("setType", &Action::setType, py::arg("type"), R"pbdoc(Set the action type
        
        Args:
            type: New ActionType enumeration value
        
        Updates the action type and resets parameters appropriately.
        
        Args:
            type: New action type
        
        Note:
            Changing the action type may invalidate existing parameters.
            Consider resetting parameters after type change if needed.
        
        Examples:
            >>> action = Action(ActionType.MoveForward)
            >>> action.setType(ActionType.TurnLeft)  # Change to turn action
            >>> print(action.getName())
            "Turn Left"
        )pbdoc")
        .def("getParameters", &Action::getParameters, R"pbdoc(Get action parameters
        
        Returns:
            Vector of float parameters for the action
        
        Returns:
            List of action-specific parameters
        
        Parameter Documentation:
        - Movement actions: [distance]
        - Rotation actions: [angle]
        - Gaze actions: [x, y] or [x, y, z]
        - Interaction actions: [objectId] or [force]
        
        Examples:
            >>> action = Action(ActionType.TurnLeft, [45.0])
            >>> params = action.getParameters()
            >>> print(params)  # [45.0]
        )pbdoc")
        .def("setParameters", &Action::setParameters, py::arg("params"), R"pbdoc(Set action parameters
        
        Args:
            params: New vector of float parameters
        
        Updates the action parameters.
        
        Args:
            params: New parameters list
        
        Parameter Validation:
        - Parameter type depends on action type
        - Empty parameters allowed for some action types
        - Parameter ranges vary by action type
        
        Examples:
            >>> action = Action(ActionType.MoveForward)
            >>> action.setParameters([2.0, 0.5])  # Set distance and speed
        )pbdoc")
        .def("getName", &Action::getName, R"pbdoc(Get human-readable action name
        
        Returns:
            String representation of the action
        
        Returns:
            Human-readable action name
        
        Action Name Format:
        - Movement: "Move Forward", "Move Backward"
        - Rotation: "Turn Left 45.0 degrees", "Turn Right 90.0 degrees"
        - Gaze: "Look Up", "Look Down", "Look at position"
        - Interaction: "Interact with object", "Eat food"
        - Inactivity: "Rest", "Wait"
        
        Examples:
            >>> action = Action(ActionType.MoveForward, [1.0])
            >>> action.getName()
            "Move Forward 1.0 units"
            
            >>> action = Action(ActionType.TurnLeft, [45.0])
            >>> action.getName()
            "Turn Left 45.0 degrees"
        )pbdoc")
        .def("clone", &Action::clone, R"pbdoc(Create a copy of this action
        
        Returns:
            Cloned Action instance
        
        Creates a deep copy of the action with the same type and parameters.
        
        Cloning Use Cases:
        - Save action state before modification
        - Queue actions for later execution
        - Implement action history
        - Branch simulation for exploration
        
        Examples:
            >>> original = Action(ActionType.Look, [0.5, 0.5])
            >>> cloned = original.clone()
            >>> cloned.setParameters([1.0, 1.0])  # Modify clone, keep original
        )pbdoc");

    // WorldObject class with position validation
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation
    
    Physical object in the 2D simulation world.
    Represents entities like resources, hazards, walls, and markers.
    
    Key Features:
    - Position in world coordinates (x, y)
    - Visual properties (radius, type, value)
    - Active/inactive state management
    - Position and radius validation
    
    Object Types:
    - WorldObjectType.Empty: Empty space
    - WorldObjectType.Resource: Collectible resource
    - WorldObjectType.Hazard: Dangerous entity
    - WorldObjectType.Wall: Impassable boundary
    - WorldObjectType.Marker: Important landmark
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f, R"pbdoc(Initialize WorldObject with validated parameters
             
             Args:
                 x: X-coordinate in world space (0.0-100.0)
                 y: Y-coordinate in world space (0.0-100.0)
                 type: WorldObjectType enumeration (e.g., WorldObjectType.Resource)
                 value: Object value or quantity (default: 0.0)
                 radius: Object radius for collision detection (default: 0.5)
             
             Raises:
                 InvalidArgument: If position is out of bounds
             
             Examples:
                 >>> resource = WorldObject(50.0, 50.0, WorldObjectType.Resource, 100.0, 1.0)
                 >>> hazard = WorldObject(10.0, 10.0, WorldObjectType.Hazard, 50.0)
                 >>> wall = WorldObject(0.0, 0.0, WorldObjectType.Wall, 0.0, 2.0)
                 
                 >>> print(f"Resource at ({resource.x}, {resource.y}) with value {resource.value}")
             
             Position Constraints:
             - x: [0.0, 100.0) meters
             - y: [0.0, 100.0) meters
             - radius: [0.0, ∞) meters
             )pbdoc")
        .def_readwrite("x", &WorldObject::x, R"pbdoc(X-coordinate in world space
        
        Returns:
            X-coordinate (meters)
        
        Returns:
            X position as float
        
        X-Coordinate Details:
        - Range: [0.0, 100.0) meters
        - World bounds: [0, 100) in both dimensions
        - Used for collision detection and rendering
        - Update triggers collision checks
        
        Examples:
            >>> obj = WorldObject()
            >>> obj.x = 50.0
            >>> print(obj.x)
            50.0
        )pbdoc")
        .def_readwrite("y", &WorldObject::y, R"pbdoc(Y-coordinate in world space
        
        Returns:
            Y-coordinate (meters)
        
        Returns:
            Y position as float
        
        Y-Coordinate Details:
        - Range: [0.0, 100.0) meters
        - World bounds: [0, 100) in both dimensions
        - Used for collision detection and rendering
        - Update triggers collision checks
        
        Examples:
            >>> obj = WorldObject()
            >>> obj.y = 30.0
            >>> print(obj.y)
            30.0
        )pbdoc")
        .def_readwrite("radius", &WorldObject::radius, R"pbdoc(Object radius for collision detection
        
        Returns:
            Object radius (meters)
        
        Returns:
            Radius as float
        
        Radius Details:
        - Range: [0.0, ∞) meters
        - Affects collision detection area
        - Used for rendering and interaction
        - Zero radius = point object
        
        Typical Values:
        - Resource: 0.5-2.0 meters
        - Hazard: 1.0-3.0 meters
        - Wall: 0.1-1.0 meters (thin to thick)
        - Marker: 0.3-0.8 meters
        
        Examples:
            >>> obj = WorldObject(0.0, 0.0, WorldObjectType.Resource, 10.0, 1.5)
            >>> print(f"Object radius: {obj.radius} meters")
        )pbdoc")
        .def_readwrite("type", &WorldObject::type, R"pbdoc(Object type enumeration
        
        Returns:
            WorldObjectType enumeration value
        
        Returns:
            Object type
        
        Object Types:
        - WorldObjectType.Empty: Empty space (passable)
        - WorldObjectType.Resource: Valuable item (collectible)
        - WorldObjectType.Hazard: Dangerous entity (harmful)
        - WorldObjectType.Wall: Barrier (impassable)
        - WorldObjectType.Marker: Important landmark (visual)
        
        Type Examples:
        ```python
        >>> empty = WorldObject(10, 10, WorldObjectType.Empty)
        >>> resource = WorldObject(50, 50, WorldObjectType.Resource, 100.0)
        >>> hazard = WorldObject(20, 20, WorldObjectType.Hazard, 50.0)
        >>> wall = WorldObject(0, 0, WorldObjectType.Wall, 0.0, 2.0)
        ```
        )pbdoc")
        .def_readwrite("value", &WorldObject::value, R"pbdoc(Object value or quantity
        
        Returns:
            Object value (units depend on type)
        
        Returns:
            Value as float
        
        Value Interpretation:
        - Resource: Quantity or quality (e.g., 100.0 energy units)
        - Hazard: Severity or danger level (e.g., 50.0 damage)
        - Wall: Thickness or strength (e.g., 2.0 meters)
        - Marker: Importance or significance (e.g., 1.0 high importance)
        
        Value Range:
        - Resource: [0.0, ∞) (more is better)
        - Hazard: [0.0, ∞) (higher is worse)
        - Wall: [0.0, ∞) (thicker is stronger)
        - Marker: [0.0, ∞) (higher is more important)
        
        Examples:
            >>> food = WorldObject(30, 30, WorldObjectType.Resource, 50.0)
            >>> poison = WorldObject(40, 40, WorldObjectType.Hazard, 75.0)
        )pbdoc")
        .def_readwrite("active", &WorldObject::active, R"pbdoc(Object active state
        
        Returns:
            True if object is active, False otherwise
        
        Returns:
            Active state as bool
        
        Active State Meaning:
        - True: Object participates in simulation
        - False: Object is ignored (can be used for removed objects)
        - Toggle: Use to temporarily disable/enable objects
        - Use for: Resource depletion, hazard activation, etc.
        
        Examples:
            >>> obj = WorldObject()
            >>> obj.active = True
            >>> print(obj.active)
            True
        )pbdoc");

    // AgentBody class
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

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // SensoryPercept class
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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        .def("shape", [](const SensoryPercept& percept) {
            return py::make_tuple(percept.getVisionWidth(), percept.getVisionHeight(), 1);
        })
        .def("getType", [](const SensoryPercept& percept, const std::string& modality) {
            if (modality == "vision") {
                return percept.getVision().empty() ? "empty" : "vision";
            } else if (modality == "touch") {
                return percept.getTouch().empty() ? "empty" : "touch";
            } else if (modality == "internal") {
                return percept.getInternal().empty() ? "empty" : "internal";
            } else if (modality == "audio") {
                return percept.getAudio().empty() ? "empty" : "audio";
            } else {
                throw py::python_error("Invalid modality: " + modality);
            }
        }, py::arg("modality"))
        .def("getData", [](const SensoryPercept& percept, const std::string& modality) {
            if (modality == "vision") return percept.getVision();
            else if (modality == "touch") return percept.getTouch();
            else if (modality == "internal") return percept.getInternal();
            else if (modality == "audio") return percept.getAudio();
            else throw py::python_error("Invalid modality: " + modality);
        }, py::arg("modality"))
        .def("__repr__", [](const SensoryPercept& percept) {
            return "<SensoryPercept: vision=" + std::to_string(percept.getVisionWidth()) + 
                   "x" + std::to_string(percept.getVisionHeight()) + ">";
        })
        .def("__str__", [](const SensoryPercept& percept) {
            return "SensoryPercept with vision " + std::to_string(percept.getVisionWidth()) + 
                   "x" + std::to_string(percept.getVisionHeight());
        });

    // SimpleWorld class with validation
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation
    
    2D simulation environment for neural learning agents.
    Manages world state, objects, agent body, and sensory perception.
    
    Key Features:
    - 2D grid world with configurable dimensions
    - World object management (add/remove)
    - Agent body state tracking
    - Sensory perception management
    - Position validation and bounds checking
    - Random seed management
    - Energy and resource management
    
    World Properties:
    - width: World width in meters (0.0-100.0)
    - height: World height in meters (0.0-100.0)
    - visionWidth: Agent vision horizontal resolution
    - visionHeight: Agent vision vertical resolution
    - objects_: Collection of world objects
    
    Simulation State:
    - Agent position and orientation
    - Agent energy and health
    - Simulation time tracking
    - Random seed for reproducibility
    )pbdoc")
        .def(py::init<>())
        .def("configure", [](SimpleWorld& self, float width, float height, size_t visionWidth, size_t visionHeight) {
            validateDimensions(width, height);
            validatePositive("visionWidth", visionWidth);
            validatePositive("visionHeight", visionHeight);
            self.configure(width, height, visionWidth, visionHeight);
        }, py::arg("width"), py::arg("height"),
            py::arg("visionWidth"), py::arg("visionHeight"),
            R"pbdoc(Configure world dimensions and agent vision
            
            Args:
                width: World width in meters (1.0-100.0)
                height: World height in meters (1.0-100.0)
                visionWidth: Agent vision horizontal resolution (pixels)
                visionHeight: Agent vision vertical resolution (pixels)
            
            Validation:
            - Validates world dimensions (must be > 0)
            - Validates vision resolution (must be > 0)
            - Resets agent position to center
            
            Examples:
                >>> world = SimpleWorld()
                >>> world.configure(100.0, 80.0, 640, 480)  # Standard HD vision
                >>> world.configure(50.0, 50.0, 320, 240)    # Smaller world
                
                >>> print(f"World: {world.getWidth()}×{world.getHeight()} meters")
                >>> print(f"Vision: {world.getSensoryPercept().getVisionWidth()}×{world.getSensoryPercept().getVisionHeight()} pixels")
            
            Typical Values:
            - Standard: (100.0, 80.0, 640, 480)
            - Small: (50.0, 50.0, 320, 240)
            - Large: (200.0, 150.0, 1280, 960)
            )pbdoc")
        .def("reset", &SimpleWorld::reset, R"pbdoc(Reset world to initial state
        
        Resets the world to its configured dimensions and initial state.
        
        Resets:
        - Agent to center position with default orientation
        - Agent energy and health to maximum
        - Simulation time to zero
        - All world objects to initial state
        
        Use Cases:
        - Start new episode
        - Reset after failure
        - Clear world state
        
        Examples:
            >>> world = SimpleWorld()
            >>> world.configure(100.0, 80.0, 640, 480)
            >>> world.reset()
            >>> print("World reset to initial state")
        )pbdoc")
        .def("setAgentStart", [](SimpleWorld& self, float x, float y) {
            validatePosition(x, y, 0.0f, self.getWidth(), 0.0f, self.getHeight());
            self.setAgentStart(x, y);
        }, py::arg("x"), py::arg("y"), R"pbdoc(Set agent starting position
        
        Args:
            x: X-coordinate in world space (0.0-100.0)
            y: Y-coordinate in world space (0.0-100.0)
        
        Validation:
        - Checks position is within world bounds
        - Validates position is not on hazard objects
        
        Agent Start Position Details:
        - Range: [0.0, width) in x, [0.0, height) in y
        - Must be valid position (not on wall or hazard)
        - Used as initial position for reset
        
        Examples:
            >>> world = SimpleWorld()
            >>> world.configure(100.0, 80.0, 640, 480)
            >>> world.setAgentStart(25.0, 25.0)  # Set specific start
            >>> world.reset()
            >>> print(f"Agent started at: {world.getAgentBody().x}, {world.getAgentBody().y}")
        )pbdoc")
        .def("update", [](SimpleWorld& self, double timestep) {
            validatePositive("timestep", timestep, 0.0);
            self.update(timestep);
        }, py::arg("timestep"), R"pbdoc(Update world simulation state
        
        Args:
            timestep: Time step duration in seconds (must be >= 0)
        
        Updates all world components:
        - Agent physics and movement
        - World object interactions
        - Energy decay
        - Random events
        
        Timestep Considerations:
        - Larger timesteps: Faster simulation, less accurate
        - Smaller timesteps: Slower simulation, more accurate
        - Must be non-negative
        
        Examples:
            >>> world = SimpleWorld()
            >>> world.configure(100.0, 80.0, 640, 480)
            >>> world.update(0.1)   # 100ms time step
            >>> world.update(0.05)  # 50ms time step
        )pbdoc")
        .def("applyMotorCommand", [](SimpleWorld& self, MotorCommand cmd, double currentTime) {
            validatePositive("currentTime", currentTime, 0.0);
            return self.applyMotorCommand(cmd, currentTime);
        }, py::arg("cmd"), py::arg("currentTime"), R"pbdoc(Apply motor command to agent
        
        Args:
            cmd: MotorCommand enumeration value
            currentTime: Current simulation time
        
        Applies motor command to agent:
        - Updates agent position and orientation
        - Handles collisions with world objects
        - Updates agent energy
        - Returns action result
        
        MotorCommand Types:
        - MotorCommand.MoveForward: Move forward
        - MotorCommand.TurnLeft: Rotate left
        - MotorCommand.TurnRight: Rotate right
        - MotorCommand.LookLeft: Look left
        - MotorCommand.LookRight: Look right
        - MotorCommand.Interact: Interact with object
        - MotorCommand.Wait: No action
        
        Command Parameters:
        - Most commands take implicit parameters
        - Turn actions use internal angle constants
        - Interaction actions target nearby objects
        
        Examples:
            >>> world = SimpleWorld()
            >>> world.configure(100.0, 80.0, 640, 480)
            >>> world.setAgentStart(50.0, 40.0)
            >>> world.update(0.1)  # Let agent see the world
            
            >>> percept = world.getSensoryPercept()
            >>> if percept.getAudio().getNumSamples() > 0:
            ...     result = world.applyMotorCommand(MotorCommand.Interact, world.getSimulationTime())
            >>> print(f"Action result: success={result.success}, reward={result.reward}")
        )pbdoc")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get current sensory percept
             
             Returns:
                 SensoryPercept object containing all sensory inputs
             
             Returns:
                 Sensory percept from agent's perspective
             
             Sensory Percept Contents:
             - Vision: Camera image (Vision object)
             - Touch: Proximity sensors (InternalSignals)
             - Internal: Internal brain signals (InternalSignals)
             - Audio: Sound input (Audio object)
             
             Returns:
                 SensoryPercept instance
             
             Examples:
                 >>> world = SimpleWorld()
                 >>> world.configure(100.0, 80.0, 640, 480)
                 >>> world.update(0.1)
                 >>> percept = world.getSensoryPercept()
                 >>> print(f"Vision size: {percept.getVisionWidth()}×{percept.getVisionHeight()}")
             )pbdoc")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get agent body state
             
             Returns:
                 AgentBody object with position and orientation
             
             Returns:
                 Agent body state reference
             
             Agent Body State:
             - x, y: Position in world coordinates
             - orientation: Facing angle (radians)
             - velocityX, velocityY: Linear velocity
             - angularVelocity: Rotational velocity
             - energy: Current energy level
             - health: Health status
             - isMoving, isTurning: Movement states
             
             Returns:
                 AgentBody instance
             
             Examples:
                 >>> world = SimpleWorld()
                 >>> world.configure(100.0, 80.0, 640, 480)
                 >>> body = world.getAgentBody()
                 >>> print(f"Agent at ({body.x}, {body.y}) facing {body.orientation} radians")
             )pbdoc")
        .def("addObject", &SimpleWorld::addObject,
             R"pbdoc(Add object to world
             
             Args:
                 object: WorldObject to add
             
             Adds object to world if position is valid.
             
             Validation:
            - Checks position is within world bounds
            - Validates position is not overlapping with other objects
            
            Object Types:
            - WorldObjectType.Resource: Collectible item
            - WorldObjectType.Hazard: Dangerous entity
            - WorldObjectType.Wall: Impassable boundary
            - WorldObjectType.Marker: Important landmark
            
            Examples:
                >>> world = SimpleWorld()
                >>> world.configure(100.0, 80.0, 640, 480)
                >>> resource = WorldObject(50.0, 50.0, WorldObjectType.Resource, 100.0)
                >>> world.addObject(resource)
                >>> print(f"World has {len(world)} objects")
            )pbdoc")
        .def("removeObject", &SimpleWorld::removeObject,
             R"pbdoc(Remove object from world
             
             Args:
                 object: WorldObject to remove
             
            Removes object from world if present.
            
            Use Cases:
            - Remove collected resources
            - Remove expired hazards
            - Clean up world state
            
            Examples:
                >>> world = SimpleWorld()
                >>> world.configure(100.0, 80.0, 640, 480)
                >>> resource = WorldObject(50.0, 50.0, WorldObjectType.Resource, 100.0)
                >>> world.addObject(resource)
                >>> world.removeObject(resource)
                >>> print(f"World now has {len(world)} objects")
            )pbdoc")
        .def("isValidPosition", [](SimpleWorld& self, float x, float y) {
            validatePosition(x, y, 0.0f, self.getWidth(), 0.0f, self.getHeight());
            return self.isValidPosition(x, y);
        }, py::arg("x"), py::arg("y"), R"pbdoc(Check if position is valid
        
        Args:
            x: X-coordinate to check
            y: Y-coordinate to check
        
        Validation:
        - Checks position is within world bounds
        - Validates position is not on wall or hazard objects
        
        Returns:
            True if position is valid, False otherwise
        
        Valid Position Criteria:
        - Within world bounds [0, width) × [0, height)
        - Not on WorldObjectType.Wall
        - Not on WorldObjectType.Hazard
        - Not overlapping other objects
        
        Examples:
            >>> world = SimpleWorld()
            >>> world.configure(100.0, 80.0, 640, 480)
            >>> print(world.isValidPosition(50.0, 40.0))  # Valid
            True
            >>> print(world.isValidPosition(150.0, 40.0))  # Out of bounds
            False
        )pbdoc")
        .def("getWidth", &SimpleWorld::getWidth,
             R"pbdoc(Get world width
             
             Returns:
                 World width in meters
             
             Returns:
                 Width as float
             
             Returns:
                 World width
             
             Examples:
                 >>> world = SimpleWorld()
                 >>> world.configure(100.0, 80.0, 640, 480)
                 >>> print(f"World width: {world.getWidth()} meters")
             )pbdoc")
        .def("getHeight", &SimpleWorld::getHeight,
             R"pbdoc(Get world height
             
             Returns:
                 World height in meters
             
             Returns:
                 Height as float
             
             Returns:
                 World height
             
             Examples:
                 >>> world = SimpleWorld()
                 >>> world.configure(100.0, 80.0, 640, 480)
                 >>> print(f"World height: {world.getHeight()} meters")
             )pbdoc")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             R"pbdoc(Get maximum agent energy
             
             Returns:
                 Maximum energy level
             
             Returns:
                 Max energy as float
             
             Returns:
                 Maximum energy
             
             Examples:
                 >>> world = SimpleWorld()
                 >>> world.configure(100.0, 80.0, 640, 480)
                 >>> print(f"Max energy: {world.getMaxEnergy()}")
             )pbdoc")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             R"pbdoc(Set maximum agent energy
             
             Args:
                 e: Maximum energy level (must be >= 0)
             
            Energy Constraints:
            - Must be non-negative
            - Used for energy decay calculations
            - Affects agent stamina and capabilities
            
            Examples:
                >>> world = SimpleWorld()
                >>> world.configure(100.0, 80.0, 640, 480)
                >>> world.setMaxEnergy(200.0)
                >>> print(f"Max energy set to: {world.getMaxEnergy()}")
            )pbdoc")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             R"pbdoc(Get energy decay rate
             
             Returns:
                 Energy decay rate per timestep
             
             Returns:
                 Decay rate as float
             
             Returns:
                 Energy decay rate
             
             Decay Rate Meaning:
            - Energy lost per timestep
            - Higher values: Faster energy loss
            - Typical values: 0.01-0.1 per timestep
            
            Examples:
                >>> world = SimpleWorld()
                >>> world.configure(100.0, 80.0, 640, 480)
                >>> print(f"Energy decay rate: {world.getEnergyDecayRate()}")
             )pbdoc")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             R"pbdoc(Set energy decay rate
             
             Args:
                 r: Energy decay rate per timestep (must be >= 0)
             
            Decay Rate Considerations:
            - Controls how quickly agent loses energy
            - Higher values: More challenging simulation
            - Lower values: More forgiving simulation
            - Used in update() calculations
            
            Typical Values:
            - Low: 0.001 (slow decay)
            - Medium: 0.01 (standard decay)
            - High: 0.1 (fast decay)
            
            Examples:
                >>> world = SimpleWorld()
                >>> world.configure(100.0, 80.0, 640, 480)
                >>> world.setEnergyDecayRate(0.05)
                >>> print(f"Energy decay rate set to: {world.getEnergyDecayRate()}")
            )pbdoc")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             R"pbdoc(Get current simulation time
             
             Returns:
                 Current simulation time in seconds
             
             Returns:
                 Simulation time as double
             
             Returns:
                 Current simulation time
             
             Time Tracking:
            - Starts at 0.0 when world is configured
            - Increments by timestep in each update()
            - Used for temporal alignment
            - Measured in seconds
            
            Examples:
                >>> world = SimpleWorld()
                >>> world.configure(100.0, 80.0, 640, 480)
                >>> print(f"Simulation time: {world.getSimulationTime()} seconds")
                >>> world.update(0.1)
                >>> print(f"Simulation time after update: {world.getSimulationTime()} seconds")
             )pbdoc")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             R"pbdoc(Set random seed for reproducibility
             
             Args:
                 seed: Random seed value
             
            Random Seed Purpose:
            - Ensures reproducible simulation results
            - Used for random events and agent behavior
            - Allows deterministic testing
            - Affects all random number generation
            
            Seed Examples:
            - 42: Common seed for testing
            - 12345: Unique simulation seed
            - None: Use system time (non-deterministic)
            
            Examples:
                >>> world = SimpleWorld()
                >>> world.configure(100.0, 80.0, 640, 480)
                >>> world.setRandomSeed(42)
                >>> world.update(0.1)  # Deterministic results
            )pbdoc")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             R"pbdoc(Get random seed
             
             Returns:
                 Current random seed value
             
             Returns:
                 Random seed as size_t
             
             Returns:
                 Random seed
             
             Seed Information:
            - Used for reproducible simulations
            - Controls random events
            - Affects agent behavior
            - Used in testing and debugging
            
            Examples:
                >>> world = SimpleWorld()
                >>> world.configure(100.0, 80.0, 640, 480)
                >>> world.setRandomSeed(123)
                >>> print(f"Random seed: {world.getRandomSeed()}")
            )pbdoc")
        .def("getObjectAt", &SimpleWorld::getObjectAt,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get object at position
             
             Args:
                 x: X-coordinate to search
                 y: Y-coordinate to search
             
             Returns:
                 WorldObject at position or nullptr if none
             
             Returns:
                 WorldObject at position or None
             
             Position Search:
            - Checks all objects in world
            - Uses radius for collision detection
            - Returns first object found
            - Returns nullptr if no object at position
            
            Use Cases:
            - Check object at agent position
            - Validate interactions
            - Debug world state
            
            Examples:
                >>> world = SimpleWorld()
                >>> world.configure(100.0, 80.0, 640, 480)
                >>> resource = WorldObject(50.0, 50.0, WorldObjectType.Resource, 100.0)
                >>> world.addObject(resource)
                >>> obj = world.getObjectAt(50.0, 50.0)
                >>> print(f"Object at (50, 50): {obj.type if obj else None}")
            )pbdoc")
        .def("findObjectsByType", [](SimpleWorld& self, WorldObjectType type) {
            std::vector<WorldObject> result;
            for (const auto& obj : self.objects_) {
                if (obj.type == type && obj.active) {
                    result.push_back(obj);
                }
            }
            return result;
        }, py::arg("type"), R"pbdoc(Find all active objects of a specific type
        
        Args:
            type: WorldObjectType to search for
        
        Returns:
            List of active WorldObject instances
        
        Object Search:
        - Searches all world objects
        - Only returns active objects
        - Returns complete object copies
        - Order matches world storage
        
        Search Examples:
        ```python
        >>> world = SimpleWorld()
        >>> world.configure(100.0, 80.0, 640, 480)
        
        >>> # Add various objects
        >>> resource = WorldObject(50, 50, WorldObjectType.Resource, 100.0)
        >>> hazard = WorldObject(60, 60, WorldObjectType.Hazard, 50.0)
        >>> marker = WorldObject(70, 70, WorldObjectType.Marker, 1.0)
        
        >>> world.addObject(resource)
        >>> world.addObject(hazard)
        >>> world.addObject(marker)
        
        >>> # Find all resources
        >>> resources = world.findObjectsByType(WorldObjectType.Resource)
        >>> print(f"Found {len(resources)} resources")
        
        >>> # Find all hazards
        >>> hazards = world.findObjectsByType(WorldObjectType.Hazard)
        >>> print(f"Found {len(hazards)} hazards")
        ```
        )pbdoc")
        .def("getObjects", &SimpleWorld::getObjects,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get all objects in the world
             
             Returns:
                 List of all WorldObject instances
             
             Returns:
                 All world objects
             
             World Object List:
            - Contains all objects added to world
            - Only returns active objects
            - Returns complete object copies
            - Order matches world storage
            
            Use Cases:
            - Debug world state
            - Iterate over all objects
            - Check world occupancy
            
            Examples:
                >>> world = SimpleWorld()
                >>> world.configure(100.0, 80.0, 640, 480)
                >>> 
                >>> # Add objects
                >>> for i in range(5):
                ...     obj = WorldObject(i*20+10, i*20+10, WorldObjectType.Resource, i*10.0)
                ...     world.addObject(obj)
                >>>
                >>> # Get all objects
                >>> all_objects = world.getObjects()
                >>> print(f"World has {len(all_objects)} objects")
                >>> for obj in all_objects:
                ...     print(f"  {obj.type} at ({obj.x}, {obj.y})")
            )pbdoc")
        .def("__len__", [](const SimpleWorld& self) {
            return self.objects_.size();
        })
        .def("__getitem__", [](const SimpleWorld& self, size_t index) {
            if (index >= self.objects_.size()) {
                throw py::python_error("Object index out of bounds");
            }
            return self.objects_[index];
        }, py::arg("index"))
        .def("__iter__", [](SimpleWorld& self) {
            return py::make_iterator(self.objects_.begin(), self.objects_.end());
        }, py::return_value_policy::reference_internal);

    // Brain class with error handling
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class
    
    Core neural simulation engine for agent control and learning.
    Manages neural network, sensory processing, motor output, and development.
    
    Key Features:
    - Neural network simulation with neurons and synapses
    - Sensory input processing and injection
    - Motor action production and output
    - Current injection for neural control
    - State persistence (save/load)
    - Region management
    - Developmental stage tracking
    - Status logging and monitoring
    
    Brain Components:
    - Regions: Neural regions containing neurons and synapses
    - Neurons: Individual processing units with firing capabilities
    - Synapses: Connections between neurons with weights
    - Developmental stage: Agent life stage (Initial, CriticalPeriod, etc.)
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             R"pbdoc(Initialize Brain with configuration
             
             Args:
                 config: Configuration object for brain initialization
             
             Creates a new brain instance with the specified configuration.
             
             Args:
                 config: Brain configuration
             
             Configuration Notes:
            - Brain configuration controls learning parameters
            - Affects neural development and behavior
            - Required for proper brain initialization
            
            Examples:
                >>> config = createDefaultConfig()
                >>> brain = Brain(config)
                >>> brain.initialize()
            )pbdoc")
        .def("initialize", &Brain::initialize,
             R"pbdoc(Initialize the brain with configuration
             
             Initializes the brain neural network based on configuration.
             
             Initialization Process:
            - Creates neural regions based on config
            - Sets up neurons and synapses
            - Applies developmental parameters
            - Prepares for simulation
            
            Use Cases:
            - Before starting simulation
            - After loading saved state
            - For new brain instances
            
            Examples:
                >>> brain = Brain(config)
                >>> brain.initialize()  # Sets up neural network
            )pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             R"pbdoc(Perform a simulation step
             
             Args:
                 currentStep: Current simulation step identifier
             
             Executes one simulation step with the specified step ID.
             
             Simulation Step Process:
            - Advances neural network state
            - Processes sensory inputs
            - Computes motor outputs
            - Updates neural activity
            
            Args:
                currentStep: Step number/index
            
            Step Examples:
                >>> brain = Brain(config)
                >>> brain.initialize()
                >>> brain.step(1)      # First step
                >>> brain.step(100)    # Step 100
            )pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             R"pbdoc(Perform a simulation step with timestamp
             
             Args:
                 currentStep: Current simulation step identifier
                 currentTime: Current simulation time in seconds
             
             Executes one simulation step with temporal precision.
             
             Temporal Step Process:
            - Includes timing information in neural computations
            - Aligns sensory input with simulation time
            - Tracks developmental progression
            - Supports time-based behavior
            
             Args:
                currentStep: Step number/index
                currentTime: Time in seconds (0.0+)
            
             Time-Aware Step Examples:
                >>> brain = Brain(config)
                >>> brain.initialize()
                >>> brain.step(1, 0.0)      # Step at time zero
                >>> brain.step(2, 0.1)      # Step at 0.1 seconds
            )pbdoc")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"), R"pbdoc(Inject sensory input into the brain
             
             Args:
                 input: SensoryInput object to process
             
             Processes sensory input from the environment.
             
             Input Processing:
            - Converts sensory data to neural signals
            - Injects current into appropriate neurons
            - Updates neural activity patterns
            - Triggers action potential generation
            
             Args:
                 input: Sensory data (Vision, Audio, etc.)
            
             Input Types:
            - Vision: Visual camera data
            - Audio: Sound input
            - Touch: Tactile sensor data
            - Internal: Internal neural signals
            
            Examples:
                >>> vision = Vision(640, 480, 3)
                >>> vision.setData([0.1, 0.2, 0.3, ...])
                >>> brain.receiveSensoryInput(vision)
            )pbdoc")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"), R"pbdoc(Inject current into a specific neuron
             
             Args:
                 neuron: Neuron ID to inject current into
                 current: Current value to inject (positive for excitation, negative for inhibition)
             
            Direct neural control method.
            
            Current Injection:
            - Directly affects neuron membrane potential
            - Triggers action potentials when threshold reached
            - Can excite or inhibit neural activity
            - Useful for controlled experiments
            
             Args:
                 neuron: Target neuron ID
                 current: Current value (microamps typically)
            
             Current Range:
            - Excitation: Positive values (> 0)
            - Inhibition: Negative values (< 0)
            - Typical magnitude: ±0.1 to ±10.0
            
            Examples:
                >>> brain = Brain(config)
                >>> brain.initialize()
                >>> brain.injectCurrent(neuron_id, 1.0)    # Excite neuron
                >>> brain.injectCurrent(neuron_id, -0.5)   # Inhibit neuron
            )pbdoc")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"), R"pbdoc(Inject current into all neurons of a specific type
             
             Args:
                 type: NeuronType (e.g., NeuronType.Excitatory, NeuronType.Inhibitory)
                 current: Current value to inject
             
            Mass neural control method.
            
            Bulk Current Injection:
            - Affects all neurons of specified type
            - Useful for global network modulation
            - Controls overall excitation/inhibition balance
            - Used in neuromodulation experiments
            
             Args:
                 type: Target neuron type
                 current: Current value for all neurons
            
             NeuronType Options:
            - NeuronType.Excitatory: Glial cells, positive neurons
            - NeuronType.Inhibitory: Negative neurons, interneurons
            - NeuronType.Sensory: Sensory neurons
            - NeuronType.Motor: Motor neurons
            
            Examples:
                >>> brain = Brain(config)
                >>> brain.initialize()
                >>> brain.injectCurrentToNeurons(NeuronType.Excitatory, 0.1)   # Global excitation
                >>> brain.injectCurrentToNeurons(NeuronType.Inhibitory, -0.1)  # Global inhibition
            )pbdoc")
        .def("produceAction", &Brain::produceAction,
             R"pbdoc(Produce motor action based on neural activity
             
             Produces motor action based on current neural network state.
             
             Action Production:
            - Decodes neural activity to motor commands
            - Maps neural patterns to action types
            - Generates action parameters
            - Returns ActionResult with reward
            
             Action Generation:
            - Reads neural firing patterns
            - Applies learned mappings
            - Considers action costs and benefits
            - Incorporates developmental state
            
            Returns:
                ActionResult containing action, success, and reward
            
            Examples:
                >>> brain = Brain(config)
                >>> brain.initialize()
                >>> result = brain.produceAction()
                >>> print(f"Action: {result.action.getName()}")
                >>> print(f"Success: {result.success}, Reward: {result.reward}")
            )pbdoc")
        .def("reset", &Brain::reset,
             R"pbdoc(Reset brain state
             
             Resets the brain to its initial state.
             
             Reset Process:
            - Clears neural activity
            - Resets synaptic weights
            - Returns to initial developmental stage
            - Preserves configuration
            
            Use Cases:
            - Start new episode
            - Reset after failure
            - Clear accumulated state
            - Prepare for testing
            
            Examples:
                >>> brain = Brain(config)
                >>> brain.initialize()
                >>> brain.step(100)  # Accumulate state
                >>> brain.reset()    # Back to initial state
            )pbdoc")
        .def("save", [](Brain& self, const std::string& filepath) {
            try {
                if (!self.save(filepath)) {
                    throw std::runtime_error("Failed to save brain state to file: " + filepath);
                }
                return true;
            } catch (const std::exception& e) {
                throw py::python_error(std::string("Brain.save error: ") + e.what());
            }
        }, py::arg("filepath"), R"pbdoc(Save brain state to file with error handling
        
        Args:
            filepath: Path where brain state will be saved
        
        Saves the complete brain state including:
        - Neural network weights and connections
        - Synaptic states
        - Developmental stage
        - Configuration settings
        - Internal neural states
        
        Saved State Components:
        - Neuron states and weights
        - Synapse weights and connections
        - Brain configuration
        - Developmental parameters
        - Random seed
        
        Args:
            filepath: Output file path
        
        File Format:
        - Binary format for efficiency
        - Platform-independent serialization
        - Complete state restoration
        
        Examples:
            >>> brain = Brain(config)
            >>> brain.initialize()
            >>> brain.step(1000)
            >>> brain.save("brain_state.bin")
            >>> print("Brain state saved successfully")
        )pbdoc")
        .def("load", [](Brain& self, const std::string& filepath) {
            try {
                if (!self.load(filepath)) {
                    throw std::runtime_error("Failed to load brain state from file: " + filepath);
                }
                return true;
            } catch (const std::exception& e) {
                throw py::python_error(std::string("Brain.load error: ") + e.what());
            }
        }, py::arg("filepath"), R"pbdoc(Load brain state from file with error handling
        
        Args:
            filepath: Path to saved brain state file
        
        Restores the complete brain state from file.
        
        Loaded State Components:
        - Neural network weights and connections
        - Synaptic states
        - Developmental stage
        - Configuration settings
        - Internal neural states
        
        Args:
            filepath: Input file path
        
        File Format:
        - Binary format
        - Platform-independent
        - Complete state restoration
        
        Examples:
            >>> brain = Brain(config)
            >>> brain.initialize()
            >>> brain.save("brain_state.bin")
            >>> 
            >>> new_brain = Brain(config)
            >>> new_brain.load("brain_state.bin")
            >>> print("Brain state loaded successfully")
        )pbdoc")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             R"pbdoc(Add a new neural region
             
             Args:
                 name: Name for the new region (default: "")
             
            Creates a new neural region with default parameters.
            
            Region Creation:
            - Creates new region with unique ID
            - Adds to brain's region collection
            - Initializes with default neuron types
            - Returns region ID for reference
            
             Args:
                 name: Region name (optional)
            
            Examples:
                >>> brain = Brain(config)
                >>> brain.initialize()
                >>> region_id = brain.addRegion("Cortex")
                >>> print(f"Created region with ID: {region_id}")
            )pbdoc")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             R"pbdoc(Get a region by ID
             
             Args:
                 id: Region ID to retrieve
             
            Retrieves a specific neural region.
            
            Region Lookup:
            - Searches brain's region collection
            - Returns region reference
            - Throws if region not found
            
             Args:
                 id: Region identifier
             
             Returns:
                 Region reference for manipulation
             
            Examples:
                >>> brain = Brain(config)
                >>> brain.initialize()
                >>> region_id = brain.addRegion("Hippocampus")
                >>> region = brain.getRegion(region_id)
                >>> print(f"Retrieved region: {region}")
            )pbdoc")
        .def("getRegionCount", &Brain::getRegionCount,
             R"pbdoc(Get the number of regions
             
             Returns:
                 Number of neural regions in brain
             
             Returns:
                 Region count
             
             Returns:
                 Number of regions
             
             Examples:
                >>> brain = Brain(config)
                >>> brain.initialize()
                >>> count = brain.getRegionCount()
                >>> print(f"Brain has {count} regions")
             )pbdoc")
        .def("getRegionIds", &Brain::getRegionIds,
             R"pbdoc(Get all region IDs
             
             Returns:
                 List of all region IDs
             
             Returns:
                 List of region identifiers
             
             Returns:
                 All region IDs
             
             Examples:
                >>> brain = Brain(config)
                >>> brain.initialize()
                >>> region_ids = brain.getRegionIds()
                >>> print(f"Region IDs: {region_ids}")
             )pbdoc")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get all regions
             
             Returns:
                 List of all region references
             
             Returns:
                 All region references
             
             Returns:
                 All neural regions
             
             Examples:
                >>> brain = Brain(config)
                >>> brain.initialize()
                >>> regions = brain.getRegions()
                >>> print(f"Found {len(regions)} regions")
             )pbdoc")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             R"pbdoc(Get total neuron count across all regions
             
             Returns:
                 Total number of neurons in brain
             
             Returns:
                 Total neuron count
             
             Returns:
                 Total neurons
             
             Examples:
                >>> brain = Brain(config)
                >>> brain.initialize()
                >>> total_neurons = brain.getTotalNeuronCount()
                >>> print(f"Total neurons: {total_neurons}")
             )pbdoc")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             R"pbdoc(Get total synapse count across all regions
             
             Returns:
                 Total number of synapses in brain
             
             Returns:
                 Total synapse count
             
             Returns:
                 Total synapses
             
             Examples:
                >>> brain = Brain(config)
                >>> brain.initialize()
                >>> total_synapses = brain.getTotalSynapseCount()
                >>> print(f"Total synapses: {total_synapses}")
             )pbdoc")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             R"pbdoc(Get count of active neurons
             
             Returns:
                 Number of currently active neurons
             
             Returns:
                 Active neuron count
             
             Returns:
                 Active neurons
             
             Examples:
                >>> brain = Brain(config)
                >>> brain.initialize()
                >>> brain.step(1)
                >>> active_count = brain.getActiveNeuronCount()
                >>> print(f"Active neurons: {active_count}")
             )pbdoc")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             R"pbdoc(Get count of currently firing neurons
             
             Returns:
                 Number of neurons currently firing action potentials
             
             Returns:
                 Firing neuron count
             
             Returns:
                 Firing neurons
             
             Examples:
                >>> brain = Brain(config)
                >>> brain.initialize()
                >>> brain.step(1)
                >>> firing_count = brain.getFiringNeuronCount()
                >>> print(f"Firing neurons: {firing_count}")
             )pbdoc")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             R"pbdoc(Get average firing rate across all neurons
             
             Returns:
                 Average firing rate (Hz)
             
             Returns:
                 Average firing rate
             
             Returns:
                 Firing rate in Hz
             
             Examples:
                >>> brain = Brain(config)
                >>> brain.initialize()
                >>> brain.step(1)
                >>> avg_rate = brain.getAverageFiringRate()
                >>> print(f"Average firing rate: {avg_rate} Hz")
             )pbdoc")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             R"pbdoc(Get excitation/inhibition balance ratio
             
             Returns:
                 Excitation/inhibition ratio
             
             Returns:
                 E/I ratio
             
             Returns:
                 E/I ratio
             
             Balance Ratio:
            - High ratio: More excitation than inhibition
            - Low ratio: More inhibition than excitation
            - Healthy balance: Typically 1.0-2.0
            - Important for stability
            
            Examples:
                >>> brain = Brain(config)
                >>> brain.initialize()
                >>> brain.step(1)
                >>> ei_ratio = brain.getExcitationInhibitionRatio()
                >>> print(f"E/I ratio: {ei_ratio}")
             )pbdoc")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             R"pbdoc(Get total spike count
             
             Returns:
                 Total number of action potentials
             
             Returns:
                 Total spike count
             
             Returns:
                 Total spikes
             
             Examples:
                >>> brain = Brain(config)
                >>> brain.initialize()
                >>> brain.step(1)
                >>> total_spikes = brain.getTotalSpikeCount()
                >>> print(f"Total spikes: {total_spikes}")
             )pbdoc")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             R"pbdoc(Get current developmental stage
             
             Returns:
                 Current developmental stage
             
             Returns:
                 Developmental stage
             
             Returns:
                 Developmental stage
             
             Developmental Stages:
            - DevelopmentalStage.Initial: New brain, no experience
            - DevelopmentalStage.CriticalPeriod: Sensitive learning period
            - DevelopmentalStage.Maturation: Physical development
            - DevelopmentalStage.Adult: Mature capabilities
            - DevelopmentalStage.Aging: Declining capabilities
            
            Examples:
                >>> brain = Brain(config)
                >>> brain.initialize()
                >>> stage = brain.getDevelopmentalStage()
                >>> print(f"Developmental stage: {stage}")
             )pbdoc")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"), R"pbdoc(Set developmental stage
             
             Args:
                 stage: New developmental stage
             
            Manually set the brain's developmental stage.
            
            Stage Considerations:
            - Stage changes affect neural development
            - Can accelerate or delay maturation
            - Affects learning capabilities
            - Used in development experiments
            
             Args:
                 stage: New developmental stage
            
            Examples:
                >>> brain = Brain(config)
                >>> brain.initialize()
                >>> brain.setDevelopmentalStage(DevelopmentalStage.Adult)
                >>> print(f"New stage: {brain.getDevelopmentalStage()}")
            )pbdoc")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get the configuration
             
             Returns:
                 Configuration reference
             
             Returns:
                 Configuration
             
             Returns:
                 Brain configuration
             
             Configuration Details:
            - Contains all brain parameters
            - Used for initialization
            - Manages neural development
            - Controls simulation behavior
            
            Examples:
                >>> brain = Brain(config)
                >>> brain.initialize()
                >>> retrieved_config = brain.getConfig()
                >>> print(f"Config keys: {retrieved_config.getKeys()}")
            )pbdoc")
        .def("logStatus", &Brain::logStatus,
             R"pbdoc(Log brain status
             
             Logs current brain state for debugging and monitoring.
             
             Status Log Contents:
            - Neural network statistics
            - Region information
            - Developmental stage
            - Configuration summary
            - Performance metrics
            
            Examples:
                >>> brain = Brain(config)
                >>> brain.initialize()
                >>> brain.step(1)
                >>> brain.logStatus()  # Prints status to console
            )pbdoc")
        .def("getNeuronById", [](Brain& self, NeuronId id) {
            for (const auto& region : self.getRegions()) {
                for (const auto& neuron : region->getNeurons()) {
                    if (neuron->getId() == id) {
                        return neuron.get();
                    }
                }
                for (const auto& synapse : region->getSynapses()) {
                    if (synapse->getSourceNeuron() == id || synapse->getTargetNeuron() == id) {
                        return synapse->getSourceNeuron() == id ? 
                            self.getRegion(region->getId())->getNeurons()[synapse->getSourceNeuron().index()].get() : 
                            self.getRegion(region->getId())->getNeurons()[synapse->getTargetNeuron().index()].get();
                    }
                }
            }
            throw py::python_error("Neuron not found: " + std::to_string(id.value));
        }, py::arg("id"), R"pbdoc(Get neuron by ID
        
        Args:
            id: Neuron ID to search for
        
        Searches brain's neural network for a specific neuron.
        
        Neuron Search:
        - Scans all regions
        - Checks both neurons and synapses
        - Returns neuron reference
        - Throws if neuron not found
        
        Args:
            id: Neuron identifier
        
        Returns:
            Neuron pointer (nullptr if not found)
        
        Search Logic:
        - First checks neurons directly
        - Then checks synapses (source or target)
        - Returns the corresponding neuron
        
        Examples:
            >>> brain = Brain(config)
            >>> brain.initialize()
            >>> # Find neuron by ID
            >>> neuron = brain.getNeuronById(neuron_id)
            >>> if neuron:
            ...     print(f"Found neuron: {neuron}")
            >>> else:
            ...     print("Neuron not found")
        )pbdoc")
        .def("getSynapseById", [](Brain& self, SynapseId id) {
            for (const auto& region : self.getRegions()) {
                for (const auto& synapse : region->getSynapses()) {
                    if (synapse->getId() == id) {
                        return synapse.get();
                    }
                }
            }
            throw py::python_error("Synapse not found: " + std::to_string(id.value));
        }, py::arg("id"), R"pbdoc(Get synapse by ID
        
        Args:
            id: Synapse ID to search for
        
        Searches brain's neural network for a specific synapse.
        
        Synapse Search:
        - Scans all regions
        - Returns synapse reference
        - Throws if synapse not found
        
        Args:
            id: Synapse identifier
        
        Returns:
            Synapse pointer (nullptr if not found)
        
        Examples:
            >>> brain = Brain(config)
            >>> brain.initialize()
            >>> # Find synapse by ID
            >>> synapse = brain.getSynapseById(synapse_id)
            >>> if synapse:
            ...     print(f"Found synapse: {synapse}")
            >>> else:
            ...     print("Synapse not found")
        )pbdoc")
        .def("findNeuronsByType", [](Brain& self, NeuronType type) {
            std::vector<Neuron*> result;
            for (const auto& region : self.getRegions()) {
                for (const auto& neuron : region->getNeurons()) {
                    if (neuron->getType() == type) {
                        result.push_back(neuron.get());
                    }
                }
            }
            return result;
        }, py::arg("type"), R"pbdoc(Find neurons by type
        
        Args:
            type: NeuronType to search for
        
        Searches brain's neural network for neurons of a specific type.
        
        Neuron Type Search:
        - Scans all regions
        - Returns vector of neuron pointers
        - Returns empty vector if none found
        
        Args:
            type: Neuron type to search for
        
        Returns:
            List of neuron pointers of specified type
        
        NeuronType Options:
        - NeuronType.Excitatory: Glial cells, positive neurons
        - NeuronType.Inhibitory: Negative neurons, interneurons
        - NeuronType.Sensory: Sensory neurons
        - NeuronType.Motor: Motor neurons
        
        Examples:
            >>> brain = Brain(config)
            >>> brain.initialize()
            >>> # Find all excitatory neurons
            >>> excitatory_neurons = brain.findNeuronsByType(NeuronType.Excitatory)
            >>> print(f"Found {len(excitatory_neurons)} excitatory neurons")
            >>> 
            >>> # Find all motor neurons
            >>> motor_neurons = brain.findNeuronsByType(NeuronType.Motor)
            >>> print(f"Found {len(motor_neurons)} motor neurons")
        )pbdoc");

    // AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", [](AgentBrain& self, double timestep) {
            validatePositive("timestep", timestep, 0.0);
            self.updateDevelopment(timestep);
        }, py::arg("timestep"), "Update development system with validated timestep")
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        .def("getLastAction", &AgentBrain::decodeMotorCommand,
             "Get last action produced")
        .def("getLastPercept", [](AgentBrain& self) {
            return self.decodeMotorCommand();
        }, "Get last percept processed")
        .def("getReward", [](AgentBrain& self) {
            return self.getPredictionError();
        }, "Get last reward received");

    // Factory functions
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

    // Module attributes
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

} // namespace nlm
