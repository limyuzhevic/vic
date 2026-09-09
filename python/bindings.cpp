#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

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
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

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

    py::register_exception<ConfigValidationError>(m, "ConfigValidationError", PyExc_ValueError);
    py::register_exception<BrainInitializationError>(m, "BrainInitializationError", PyExc_RuntimeError);
    py::register_exception<InvalidWorldConfiguration>(m, "InvalidWorldConfiguration", PyExc_ValueError);
    py::register_exception<InvalidAgentConfiguration>(m, "InvalidAgentConfiguration", PyExc_ValueError);
    py::register_exception<OperationError>(m, "OperationError", PyExc_RuntimeError);

    // Config class - improved with validation and error handling
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system
        
        Manages system configuration parameters with validation and type safety.
        Supports loading from JSON files, command line arguments, and runtime
        parameter setting with comprehensive error checking.
        
        Raises:
            ConfigValidationError: If configuration values are invalid or out of range
            OperationError: If file operations fail
        )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(
             Load configuration from a JSON file
             
             Args:
                 filepath: Path to the JSON configuration file
                 
             Returns:
                 bool: True if loading succeeded, False otherwise
                 
             Raises:
                 OperationError: If file cannot be read or parsed
                 ConfigValidationError: If configuration values are invalid
             )pbdoc")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           R"pbdoc(
           Load configuration from command line arguments
           
           Args:
               argc: Argument count (typically from main())
               argv: Argument vector (typically from main())
               
           Returns:
               bool: True if loading succeeded, False otherwise
               
           Raises:
               ConfigValidationError: If command line values are invalid
           )pbdoc")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             R"pbdoc(
             Save configuration to a JSON file
             
             Args:
                 filepath: Path to save the JSON configuration file
                 
             Returns:
                 bool: True if saving succeeded, False otherwise
                 
             Raises:
                 OperationError: If file cannot be written
             )pbdoc")
        .def("has", &Config::has, py::arg("key"),
             R"pbdoc(
             Check if a configuration key exists
             
             Args:
                 key: Configuration key to check
                 
             Returns:
                 bool: True if the key exists, False otherwise
             )pbdoc")
        .def("getKeys", &Config::getKeys,
             R"pbdoc(
             Get all configuration keys
             
             Returns:
                 list: List of all configuration keys
             )pbdoc")
        .def("clear", &Config::clear,
             R"pbdoc(
             Clear all configuration entries
             
             Note: This operation cannot be undone
             )pbdoc")
        .def("setInt", [](Config& self, const std::string& key, int value, 
                         ConfigSource source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           R"pbdoc(
           Set an integer configuration value with validation
           
           Args:
               key: Configuration key
               value: Integer value to set
               source: Configuration source (optional, defaults to Runtime)
               
           Raises:
               ConfigValidationError: If key is empty or value is invalid
           )pbdoc")
        .def("setDouble", [](Config& self, const std::string& key, double value,
                            ConfigSource source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           R"pbdoc(
           Set a double configuration value with validation
           
           Args:
               key: Configuration key
               value: Double value to set
               source: Configuration source (optional, defaults to Runtime)
               
           Raises:
               ConfigValidationError: If key is empty or value is invalid
           )pbdoc")
        .def("setBool", [](Config& self, const std::string& key, bool value,
                          ConfigSource source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           R"pbdoc(
           Set a boolean configuration value with validation
           
           Args:
               key: Configuration key
               value: Boolean value to set
               source: Configuration source (optional, defaults to Runtime)
               
           Raises:
               ConfigValidationError: If key is empty
           )pbdoc")
        .def("setString", [](Config& self, const std::string& key, 
                            const std::string& value, ConfigSource source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           R"pbdoc(
           Set a string configuration value with validation
           
           Args:
               key: Configuration key
               value: String value to set
               source: Configuration source (optional, defaults to Runtime)
               
           Raises:
               ConfigValidationError: If key is empty or value is invalid
           )pbdoc")
        .def("getInt", [](const Config& self, const std::string& key, int defaultValue) {
            auto value = self.get<int>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
           R"pbdoc(
           Get an integer configuration value with default
           
           Args:
               key: Configuration key to retrieve
               defaultValue: Value to return if key doesn't exist
               
           Returns:
               int: Configuration value or default
               
           Raises:
               ConfigValidationError: If key exists but value type is wrong
           )pbdoc")
        .def("getDouble", [](const Config& self, const std::string& key, double defaultValue) {
            auto value = self.get<double>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
           R"pbdoc(
           Get a double configuration value with default
           
           Args:
               key: Configuration key to retrieve
               defaultValue: Value to return if key doesn't exist
               
           Returns:
               float: Configuration value or default
               
           Raises:
               ConfigValidationError: If key exists but value type is wrong
           )pbdoc")
        .def("getBool", [](const Config& self, const std::string& key, bool defaultValue) {
            auto value = self.get<bool>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
           R"pbdoc(
           Get a boolean configuration value with default
           
           Args:
               key: Configuration key to retrieve
               defaultValue: Value to return if key doesn't exist
               
           Returns:
               bool: Configuration value or default
               
           Raises:
               ConfigValidationError: If key exists but value type is wrong
           )pbdoc")
        .def("getString", [](const Config& self, const std::string& key,
                            const std::string& defaultValue) {
            auto value = self.get<std::string>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
           R"pbdoc(
           Get a string configuration value with default
           
           Args:
               key: Configuration key to retrieve
               defaultValue: Value to return if key doesn't exist
               
           Returns:
               str: Configuration value or default
               
           Raises:
               ConfigValidationError: If key exists but value type is wrong
           )pbdoc")
        .def("getListInt", [](const Config& self, const std::string& key,
                             const std::vector<int>& defaultValue) {
            auto value = self.get<std::vector<int>>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
           R"pbdoc(
           Get an integer list configuration value with default
           
           Args:
               key: Configuration key to retrieve
               defaultValue: Value to return if key doesn't exist
               
           Returns:
               list: List of integers or default list
               
           Raises:
               ConfigValidationError: If key exists but value type is wrong
           )pbdoc")
        .def("getListDouble", [](const Config& self, const std::string& key,
                                const std::vector<double>& defaultValue) {
            auto value = self.get<std::vector<double>>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
           R"pbdoc(
           Get a double list configuration value with default
           
           Args:
               key: Configuration key to retrieve
               defaultValue: Value to return if key doesn't exist
               
           Returns:
               list: List of doubles or default list
               
           Raises:
               ConfigValidationError: If key exists but value type is wrong
           )pbdoc")
        .def("remove", &Config::remove, py::arg("key"),
             R"pbdoc(
             Remove a configuration key
             
             Args:
                 key: Configuration key to remove
             )pbdoc")
        .def("summary", &Config::summary,
             R"pbdoc(
             Get a summary string of the configuration
             
             Returns:
                 str: Human-readable summary of configuration entries
             )pbdoc")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Config source enumeration
    py::enum_<ConfigSource>(m, "ConfigSource", R"pbdoc(Configuration source enumeration)pbdoc")
        .value("Default", ConfigSource::Default)
        .value("File", ConfigSource::File)
        .value("CommandLine", ConfigSource::CommandLine)
        .value("Runtime", ConfigSource::Runtime)
        .export_values();

// SensoryInput class - base for all sensory modalities
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for sensory input
        
        Abstract base class representing sensory data from the environment.
        Contains common functionality for all sensory modalities.
        
        Raises:
            OperationError: If operation fails
        )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision class - visual sensory input
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input for visual perception
        
        Represents visual input with width, height, and color channels.
        Provides methods for accessing and manipulating pixel data.
        
        Args:
            width: Image width in pixels
            height: Image height in pixels
            channels: Number of color channels (3 for RGB, 1 for grayscale)
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3,
             R"pbdoc(
             Initialize Vision sensory input
             
             Args:
                 width: Image width in pixels
                 height: Image height in pixels
                 channels: Number of color channels (default: 3 for RGB)
                 
             Raises:
                 OperationError: If initialization fails
             )pbdoc")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             R"pbdoc(
             Set pixel data for the vision input
             
             Args:
                 data: Vector of float values representing pixel data
                 
             Raises:
                 OperationError: If data size doesn't match expected dimensions
                 ConfigValidationError: If data is empty or invalid
             )pbdoc")
        .def("getWidth", &Vision::getWidth,
             R"pbdoc(
             Get image width in pixels
             
             Returns:
                 int: Image width
             )pbdoc")
        .def("getHeight", &Vision::getHeight,
             R"pbdoc(
             Get image height in pixels
             
             Returns:
                 int: Image height
             )pbdoc")
        .def("getChannels", &Vision::getChannels,
             R"pbdoc(
             Get number of color channels
             
             Returns:
                 int: Number of channels
             )pbdoc");

    // Audio class - auditory sensory input
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input for sound perception
        
        Represents audio input with sample rate and duration.
        Provides methods for accessing and manipulating audio samples.
        
        Args:
            sampleRate: Audio sample rate in Hz
            numSamples: Number of audio samples
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             R"pbdoc(
             Initialize Audio sensory input
             
             Args:
                 sampleRate: Audio sample rate in Hz
                 numSamples: Number of audio samples
                 
             Raises:
                 OperationError: If initialization fails
             )pbdoc")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             R"pbdoc(
             Set audio sample data
             
             Args:
                 data: Vector of float values representing audio samples
                 
             Raises:
                 OperationError: If data size doesn't match expected samples
                 ConfigValidationError: If data is empty or invalid
             )pbdoc")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             R"pbdoc(
             Set audio sample rate
             
             Args:
                 sampleRate: Audio sample rate in Hz
                 
             Raises:
                 OperationError: If sample rate is invalid
             )pbdoc")
        .def("getSampleRate", &Audio::getSampleRate,
             R"pbdoc(
             Get audio sample rate
             
             Returns:
                 int: Sample rate in Hz
             )pbdoc")
        .def("getNumSamples", &Audio::getNumSamples,
             R"pbdoc(
             Get number of audio samples
             
             Returns:
                 int: Number of samples
             )pbdoc");

    // InternalSignals class - internal state signals
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal signals sensory input
        
        Represents internal brain state signals such as neuromodulators,
        internal representations, and cognitive states.
        )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             R"pbdoc(
             Add a signal value to the internal signals
             
             Args:
                 value: Signal value to add
             )pbdoc")
        .def("clearSignals", &InternalSignals::clearSignals,
             R"pbdoc(
             Clear all internal signals
             )pbdoc");

    // Action class - motor output representation
    py::class_<Action>(m, "Action", R"pbdoc(
        Action representation for motor output
        
        Represents a motor command or action with type and parameters.
        Used by the brain to produce motor outputs for the agent.
        
        Raises:
            OperationError: If action creation fails
        )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType,
             R"pbdoc(
             Get the action type
             
             Returns:
                 ActionType: The type of action
             )pbdoc")
        .def("setType", &Action::setType, py::arg("type"),
             R"pbdoc(
             Set the action type
             
             Args:
                 type: The action type
             )pbdoc")
        .def("getParameters", &Action::getParameters,
             R"pbdoc(
             Get the action parameters
             
             Returns:
                 list: List of parameter values
             )pbdoc")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             R"pbdoc(
             Set the action parameters
             
             Args:
                 params: List of parameter values
             )pbdoc")
        .def("getName", &Action::getName,
             R"pbdoc(
             Get the human-readable name of the action
             
             Returns:
                 str: Action name
             )pbdoc")
        .def("clone", &Action::clone,
             R"pbdoc(
             Create a copy of this action
             
             Returns:
                 Action: A copy of this action
             )pbdoc");

// WorldObject class - object in the world
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        World object representation
        
        Represents an object in the world with position, type, and properties.
        Objects can be resources, hazards, walls, markers, etc.
        
        Raises:
            InvalidWorldConfiguration: If object configuration is invalid
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             R"pbdoc(
             Initialize a world object
             
             Args:
                 x: X position
                 y: Y position
                 type: Object type
                 value: Object value (reward/damage)
                 radius: Object radius (default: 0.5)
                 
             Raises:
                 InvalidWorldConfiguration: If position or type is invalid
             )pbdoc")
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);

    // AgentBody class - agent state representation
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state
        
        Represents the physical state of the agent in the world.
        Includes position, orientation, velocity, energy, and other state variables.
        
        Raises:
            InvalidAgentConfiguration: If agent state is invalid
        )pbdoc")
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
        .def("reset", &AgentBody::reset,
             R"pbdoc(
             Reset agent body to initial state
             
             Note: This operation cannot be undone
             )pbdoc");

    // ActionResult class - result of a motor command
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Action result from world
        
        Represents the result of applying a motor command to the world.
        Contains reward, success status, and message.
        
        Raises:
            OperationError: If action result creation fails
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             R"pbdoc(
             Initialize action result
             
             Args:
                 reward: Reward value (positive for good, negative for bad)
                 success: Whether the action succeeded
                 message: Description of the result
                 
             Raises:
                 OperationError: If reward is invalid
             )pbdoc")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // SensoryPercept class - unified sensory perception
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Sensory percept data
        
        Represents unified sensory perception from all modalities:
        vision, touch, internal signals, proprioception, and audio.
        
        Raises:
            OperationError: If percept creation or manipulation fails
        )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             R"pbdoc(
             Set vision sensory data
             
             Args:
                 vision: Vision sensory input object
                 
             Raises:
                 OperationError: If vision data is invalid
             )pbdoc")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth,
             R"pbdoc(
             Get vision image width
             
             Returns:
                 int: Vision width
             )pbdoc")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight,
             R"pbdoc(
             Get vision image height
             
             Returns:
                 int: Vision height
             )pbdoc")
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             R"pbdoc(
             Set touch sensory data
             
             Args:
                 touch: Touch sensory input object
             )pbdoc")
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             R"pbdoc(
             Set internal signals data
             
             Args:
                 internal: Internal signals sensory input object
             )pbdoc")
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             R"pbdoc(
             Set proprioception data
             
             Args:
                 proprioception: Proprioception sensory input object
             )pbdoc")
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             R"pbdoc(
             Set audio sensory data
             
             Args:
                 audio: Audio sensory input object
             )pbdoc")
        .def("getAllSignals", &SensoryPercept::getAllSignals,
             R"pbdoc(
             Get all internal signals
             
             Returns:
                 list: List of all signal values
             )pbdoc")
        .def("getTimestamp", &SensoryPercept::getTimestamp,
             R"pbdoc(
             Get sensory percept timestamp
             
             Returns:
                 float: Timestamp
             )pbdoc")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             R"pbdoc(
             Set sensory percept timestamp
             
             Args:
                 timestamp: New timestamp
                 
             Raises:
                 OperationError: If timestamp is invalid
             )pbdoc");

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // Brain class - improved with validation and error handling
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class
        
        Implements real spiking neural computation with event-driven dynamics
        and integrated memory, prediction, cognition, and neuromodulation systems.
        
        Core functionality:
        - Neural simulation with spiking dynamics
        - Sensory input processing and current injection
        - Motor action production based on neural activity
        - Developmental stage management
        - Plasticity and neuromodulation
        
        Configuration requirements:
        - Must have config object with simulation parameters
        - Can contain multiple neural regions
        - Supports inter-region connectivity
        
        Raises:
            BrainInitializationError: If brain cannot be initialized
            ConfigValidationError: If configuration is invalid
            OperationError: If simulation operations fail
        )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             R"pbdoc(
             Create a new brain instance with configuration
             
             Args:
                 config: Configuration object for the brain
                 
             Raises:
                 BrainInitializationError: If config is null or invalid
                 ConfigValidationError: If configuration is invalid
             )pbdoc")
        .def("initialize", &Brain::initialize,
             R"pbdoc(
             Initialize the brain with configuration
             
             Returns:
                 bool: True if initialization succeeded
                 
             Raises:
                 BrainInitializationError: If initialization fails
                 ConfigValidationError: If configuration is invalid
             )pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             R"pbdoc(
             Perform a simulation step
             
             Args:
                 currentStep: Current simulation step number
                 
             Raises:
                 OperationError: If simulation step fails
             )pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             R"pbdoc(
             Perform a simulation step with timestamp
             
             Args:
                 currentStep: Current simulation step number
                 currentTime: Current simulation time
                 
             Raises:
                 OperationError: If simulation step fails
             )pbdoc")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             R"pbdoc(
             Receive and process sensory input from environment
             
             Args:
                 input: Sensory input object
                 
             Raises:
                 OperationError: If input processing fails
                 InvalidAgentConfiguration: If input is invalid
             )pbdoc")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             R"pbdoc(
             Inject current directly into a specific neuron
             
             Args:
                 neuron: Neuron ID to inject current into
                 current: Current value to inject (should be positive for excitation)
                 
             Raises:
                 InvalidAgentConfiguration: If neuron ID is invalid
                 OperationError: If current injection fails
             )pbdoc")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             R"pbdoc(
             Inject current into all neurons of a specific type
             
             Args:
                 type: Type of neurons to inject into
                 current: Current value to inject
                 
             Raises:
                 InvalidAgentConfiguration: If neuron type is invalid
                 OperationError: If current injection fails
             )pbdoc")
        .def("produceAction", &Brain::produceAction,
             R"pbdoc(
             Produce motor action based on neural activity
             
             Returns:
                 Action: Generated motor action
                 
             Raises:
                 OperationError: If action production fails
             )pbdoc")
        .def("reset", &Brain::reset,
             R"pbdoc(
             Reset brain state to initial conditions
             
             Note: This operation cannot be undone and preserves configuration
             )pbdoc")
        .def("save", &Brain::save, py::arg("filepath"),
             R"pbdoc(
             Save brain state to file (checkpointing)
             
             Args:
                 filepath: Path to save the brain state
                 
             Returns:
                 bool: True if saving succeeded
                 
             Raises:
                 OperationError: If file cannot be written
             )pbdoc")
        .def("load", &Brain::load, py::arg("filepath"),
             R"pbdoc(
             Load brain state from file
             
             Args:
                 filepath: Path to load the brain state from
                 
             Returns:
                 bool: True if loading succeeded
                 
             Raises:
                 OperationError: If file cannot be read or is corrupted
             )pbdoc")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             R"pbdoc(
             Add a new neural region to the brain
             
             Args:
                 name: Optional name for the region (defaults to empty)
                 
             Returns:
                 RegionId: Unique ID of the new region
                 
             Raises:
                 ConfigValidationError: If region name is invalid
                 OperationError: If region creation fails
             )pbdoc")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get a region by ID
             
             Args:
                 id: Region ID to retrieve
                 
             Returns:
                 NeuralRegion: The requested region
                 
             Raises:
                 InvalidAgentConfiguration: If region ID is invalid
             )pbdoc")
        .def("getRegionCount", &Brain::getRegionCount,
             R"pbdoc(
             Get the number of regions in the brain
             
             Returns:
                 int: Number of regions
             )pbdoc")
        .def("getRegionIds", &Brain::getRegionIds,
             R"pbdoc(
             Get all region IDs in the brain
             
             Returns:
                 list: List of all region IDs
             )pbdoc")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get all regions in the brain
             
             Returns:
                 list: List of all neural regions
             )pbdoc")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             R"pbdoc(
             Get total neuron count across all regions
             
             Returns:
                 int: Total number of neurons
             )pbdoc")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             R"pbdoc(
             Get total synapse count across all regions
             
             Returns:
                 int: Total number of synapses
             )pbdoc")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             R"pbdoc(
             Get count of active neurons
             
             Returns:
                 int: Number of active neurons
             )pbdoc")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             R"pbdoc(
             Get count of currently firing neurons
             
             Returns:
                 int: Number of firing neurons
             )pbdoc")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             R"pbdoc(
             Get average firing rate across all neurons
             
             Returns:
                 float: Average firing rate
             )pbdoc")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             R"pbdoc(
             Get excitation/inhibition balance ratio
             
             Returns:
                 float: E/I ratio
                 
             Note: Values > 1.0 indicate excitation dominance
                   Values < 1.0 indicate inhibition dominance
             )pbdoc")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             R"pbdoc(
             Get total spike count
             
             Returns:
                 int: Total number of spikes
             )pbdoc")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             R"pbdoc(
             Get current developmental stage
             
             Returns:
                 DevelopmentalStage: Current developmental stage
             )pbdoc")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             R"pbdoc(
             Set developmental stage
             
             Args:
                 stage: New developmental stage
                 
             Raises:
                 ConfigValidationError: If stage is invalid
             )pbdoc")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get the configuration used by this brain
             
             Returns:
                 Config: Brain configuration object
             )pbdoc")
        .def("logStatus", &Brain::logStatus,
             R"pbdoc(
             Log brain status and statistics
             
             Note: Outputs diagnostic information to the console
             )pbdoc")
        // Helper methods for common operations
        .def("getAllNeurons", &Brain::getTotalNeuronCount,
             R"pbdoc(
             Get total neuron count across all regions
             
             Returns:
                 int: Total number of neurons
                 
             Note: Alias for getTotalNeuronCount
             )pbdoc")
        .def("getAllRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get all regions in the brain
             
             Returns:
                 list: List of all neural regions
                 
             Note: Alias for getRegions
             )pbdoc")
        .def("getActiveRegionCount", &Brain::getRegionCount,
             R"pbdoc(
             Get count of neural regions
             
             Returns:
                 int: Number of regions
                 
             Note: Alias for getRegionCount
             )pbdoc");

    // AgentBrain class - improved with validation and error handling
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world
        
        Bridges the gap between neural computation and environmental interaction.
        Handles sensory transduction, motor decoding, and reward-based learning.
        
        Key responsibilities:
        - Process sensory input and inject into brain
        - Decode neural activity into motor commands
        - Apply reward modulation and developmental changes
        - Maintain neuromodulation state
        
        Raises:
            InvalidAgentConfiguration: If brain or world configuration is invalid
            OperationError: If processing or decoding fails
        )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             R"pbdoc(
             Create a new agent brain interface
             
             Args:
                 brain: Underlying brain instance to interface with
                 
             Raises:
                 InvalidAgentConfiguration: If brain is null
             )pbdoc")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             R"pbdoc(
             Initialize agent brain with world context
             
             Args:
                 world: World instance providing environmental context
                 
             Raises:
                 InvalidWorldConfiguration: If world configuration is invalid
                 InvalidAgentConfiguration: If initialization fails
             )pbdoc")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             R"pbdoc(
             Get expected sensory input size from the brain
             
             Returns:
                 int: Expected sensory input size
             )pbdoc")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             R"pbdoc(
             Get expected motor output size from the brain
             
             Returns:
                 int: Expected motor output size
             )pbdoc")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             R"pbdoc(
             Process sensory percept and inject into brain
             
             Args:
                 percept: Sensory percept from world
                 
             Raises:
                 OperationError: If input processing fails
                 InvalidAgentConfiguration: If percept is invalid
             )pbdoc")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             R"pbdoc(
             Decode brain motor activity into motor command
             
             Returns:
                 MotorCommand: Decoded motor command
                 
             Raises:
                 OperationError: If decoding fails
             )pbdoc")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             R"pbdoc(
             Apply reward-based neuromodulation
             
             Args:
                 reward: Actual reward received
                 predictedReward: Expected reward
                 
             Raises:
                 OperationError: If reward modulation fails
                 InvalidAgentConfiguration: If reward values are invalid
             )pbdoc")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             R"pbdoc(
             Update development system
             
             Args:
                 timestep: Time step for development update
                 
             Raises:
                 OperationError: If development update fails
             )pbdoc")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             R"pbdoc(
             Get current developmental stage
             
             Returns:
                 DevelopmentalStage: Current developmental stage
             )pbdoc")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             R"pbdoc(
             Get current neuromodulation level
             
             Returns:
                 float: Current neuromodulation level
             )pbdoc")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             R"pbdoc(
             Get curiosity level
             
             Returns:
                 float: Current curiosity level
             )pbdoc")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             R"pbdoc(
             Get novelty level
             
             Returns:
                 float: Current novelty level
             )pbdoc")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             R"pbdoc(
             Get prediction error
             
             Returns:
                 float: Current prediction error
             )pbdoc")
        .def("reset", &AgentBrain::reset,
             R"pbdoc(
             Reset agent for new episode
             
             Note: Resets internal state but preserves brain and world connections
             )pbdoc")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get the underlying brain instance
             
             Returns:
                 Brain: The underlying brain
                 
             Raises:
                 InvalidAgentConfiguration: If brain is null
             )pbdoc")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             R"pbdoc(
             Enable or disable reward modulation
             
             Args:
                 enable: True to enable, False to disable
             )pbdoc")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             R"pbdoc(
             Enable or disable structural plasticity
             
             Args:
                 enable: True to enable, False to disable
             )pbdoc")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             R"pbdoc(
             Enable or disable development
             
             Args:
                 enable: True to enable, False to disable
             )pbdoc")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             R"pbdoc(
             Enable or disable curiosity-driven exploration
             
             Args:
                 enable: True to enable, False to disable
             )pbdoc")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             R"pbdoc(
             Check if reward modulation is enabled
             
             Returns:
                 bool: True if reward modulation is enabled
             )pbdoc")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             R"pbdoc(
             Check if structural plasticity is enabled
             
             Returns:
                 bool: True if structural plasticity is enabled
             )pbdoc")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             R"pbdoc(
             Check if development is enabled
             
             Returns:
                 bool: True if development is enabled
             )pbdoc")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             R"pbdoc(
             Check if curiosity is enabled
             
             Returns:
                 bool: True if curiosity is enabled
             )pbdoc");

    // Helper factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration",
         R"pbdoc(
         Create a default configuration object
         
         Returns:
             Config: Default configuration object
         )pbdoc");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            throw InvalidAgentConfiguration("Config cannot be null");
        }
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration",
       R"pbdoc(
       Create a new brain instance with configuration
       
       Args:
           config: Configuration object for the brain
           
       Returns:
           Brain: New brain instance
           
       Raises:
           InvalidAgentConfiguration: If config is null
       )pbdoc");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world",
         R"pbdoc(
         Create a new simple world instance
         
         Returns:
             SimpleWorld: New world instance
         )pbdoc");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw InvalidAgentConfiguration("Brain cannot be null");
        }
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface",
       R"pbdoc(
       Create a new agent brain interface
       
       Args:
           brain: Brain instance to interface with
           
       Returns:
           AgentBrain: New agent brain interface
           
       Raises:
           InvalidAgentConfiguration: If brain is null
       )pbdoc");

    // Type shortcuts
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
