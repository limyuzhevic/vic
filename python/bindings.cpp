#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

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

// Enhanced error classes for Python
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) : std::runtime_error(message) {}
};

class ConfigurationError : public NLMException {
public:
    explicit ConfigurationError(const std::string& message) : NLMException("Configuration Error: " + message) {}
};

class InitializationError : public NLMException {
public:
    explicit InitializationError(const std::string& message) : NLMException("Initialization Error: " + message) {}
};

class SimulationError : public NLMException {
public:
    explicit SimulationError(const std::string& message) : NLMException("Simulation Error: " + message) {}
};

// Enhanced Python bindings with examples and documentation
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        -------------------------------------------------------
        
        A comprehensive Python interface for the NLM C++ neural simulation framework.
        
        This module provides:
        - High-level API for brain simulation and learning
        - Low-level access to neurons, synapses, and regions
        - Agent brain interface for robotics and control applications
        - World simulation for testing in virtual environments
        - Configuration management with validation
        - Examples and documentation for common use cases
        
        Quick start examples:
        
        >>> import pynlm as nlm
        >>> # Create a brain with default configuration
        >>> brain = nlm.Brain(nlm.Config())
        >>> brain.initialize()
        >>> # Run basic simulation
        >>> for step in range(1000):
        ...     brain.step(step)
        >>> brain.logStatus()
        
        >>> # Advanced example: Agent brain with world simulation
        >>> brain = nlm.Brain(nlm.Config())
        >>> agent_brain = nlm.AgentBrain(brain)
        >>> world = nlm.SimpleWorld()
        >>> world.configure(width=100, height=100, visionWidth=16, visionHeight=16)
        >>> agent_brain.initialize(world)
        
        >>> # Learning example
        >>> results = agent_brain.runLearningEpisode(world, reward_function=my_reward)
        
        For detailed examples and tutorials, visit: https://github.com/nlm/NLM/wiki/Python-Bindings
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMException>(m, "NLMException");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<InitializationError>(m, "InitializationError");
    py::register_exception<SimulationError>(m, "SimulationError");

    // Enhanced NeuronId with Python-friendly interface
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)
        
        Examples:
        >>> id1 = nlm.NeuronId()
        >>> id2 = nlm.NeuronId(42)
        >>> print(id1.index())
        0
        >>> print(id2.index())  
        42
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0)
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

    // Enhanced SynapseId with Python interface
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)
        
        Examples:
        >>> syn_id = nlm.SynapseId(123)
        >>> print(syn_id.index())
        123
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0)
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    // Enhanced RegionId
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)
        
        Examples:
        >>> region_id = nlm.RegionId(5)
        >>> print(region_id.index())
        5
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0)
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=);

    // Enhanced NeuronType with Python enum
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)
        
        Constants:
        - Excitatory: Excitatory neurons (positive influence)
        - Inhibitory: Inhibitory neurons (negative influence)
        - Modulatory: Neuromodulatory neurons (regulatory)
        - Sensory: Sensory neurons (input processing)
        - Motor: Motor neurons (output control)
        - Internal: Internal neurons (integration)
        
        Examples:
        >>> neuron_type = nlm.NeuronType.Sensory
        >>> print(neuron_type)
        4
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // Enhanced SynapseType with Python enum
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)
        
        Constants:
        - Excitatory: Excitatory connections
        - Inhibitory: Inhibitory connections
        - Modulatory: Neuromodulatory connections
        - Electrical: Electrical synapses
        - GapJunction: Gap junctions
        
        Examples:
        >>> synapse_type = nlm.SynapseType.Excitatory
        >>> print(neuron_type)
        0
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // Enhanced DevelopmentalStage with Python enum
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)
        
        Constants:
        - Initial: Initial developmental stage (high plasticity)
        - CriticalPeriod: Critical period (enhanced learning)
        - Maturation: Maturation phase (decreasing plasticity)
        - Adult: Adult stage (stable function)
        - Aging: Aging phase (declining function)
        
        Examples:
        >>> stage = nlm.DevelopmentalStage.CriticalPeriod
        >>> print(stage)
        1
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // Enhanced FiringState with Python enum
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)
        
        Constants:
        - Resting: Neuron at rest
        - Active: Neuron currently firing
        - Refractory: Neuron in refractory period
        - Inhibited: Neuron inhibited by inputs
        
        Examples:
        >>> state = nlm.FiringState.Active
        >>> print(state)
        1
    )pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // Enhanced ActionType with Python enum
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)
        
        Constants:
        - MoveForward: Move forward
        - MoveBackward: Move backward
        - MoveLeft: Move left
        - MoveRight: Move right
        - TurnLeft: Turn left
        - TurnRight: Turn right
        - Look: Look in current direction
        - LookUp: Look up
        - LookDown: Look down
        - Interact: Interact with environment
        - Eat: Eat food
        - Drink: Drink water
        - Rest: Rest/idle
        - Wait: Wait/do nothing
        - Custom: Custom action
        
        Examples:
        >>> action = nlm.ActionType.MoveForward
        >>> print(action)
        0
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
        .value("Custom", ActionType::Custom)
        .export_values();

    // Enhanced MotorCommand with Python enum
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Motor command enumeration for agent control)
        
        Constants:
        - MoveForward: Move forward
        - MoveBackward: Move backward
        - TurnLeft: Turn left
        - TurnRight: Turn right
        - LookLeft: Look left
        - LookRight: Look right
        - Interact: Interact with environment
        - Wait: Do nothing
        
        Examples:
        >>> cmd = nlm.MotorCommand.MoveForward
        >>> print(cmd)
        0
    )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    // Enhanced WorldObjectType with Python enum
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)
        
        Constants:
        - Empty: Empty space
        - Resource: Consumable resource
        - Hazard: Dangerous object
        - Wall: Obstacle
        - Marker: Goal/target
        
        Examples:
        >>> obj_type = nlm.WorldObjectType.Resource
        >>> print(obj_type)
        1
    )pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Enhanced Config class with Python-friendly interface and validation
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration manager for NLM system)
        
        This class provides:
        - JSON file loading and saving
        - Command line argument parsing
        - Type-safe value storage and retrieval
        - Configuration validation
        - Convenience methods for common settings
        - Python-friendly API with examples
        
        Examples:
        >>> # Create empty config
        >>> config = nlm.Config()
        >>> 
        >>> # Set basic parameters
        >>> config.set("neuron_count", 1000)
        >>> config.set("region_count", 2)
        >>> config.set("connection_probability", 0.1)
        >>> 
        >>> # Set STDP parameters
        >>> config.set("stdp_ltp_weight", 0.02)
        >>> config.set("stdp_ltd_weight", 0.015)
        >>> 
        >>> # Load from file
        >>> config.loadFromFile("config.json")
        >>> 
        >>> # Get values with type conversion
        >>> neurons = config.get("neuron_count", 500)
        >>> neurons = config.getOr("neuron_count", 500)  # with default
        >>> 
        >>> # Configuration validation
        >>> try:
        ...     config.validate()
        ... except nlm.ConfigurationError as e:
        ...     print(f"Invalid config: {e}")
        
        >>> # Configuration summary
        >>> print(config.summary())
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(Load configuration from a JSON file.
             
             Args:
                 filepath: Path to the JSON configuration file
                 
             Returns:
                 True if file loaded successfully, False otherwise
                 
             Raises:
                 ConfigurationError if file is invalid
             ")
            )
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           R"pbdoc(Load configuration from command line arguments.
           
           Args:
               argc: Argument count
               argv: Argument values
               
           Returns:
               True if parsing succeeded, False otherwise
           )"
          )
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             R"pbdoc(Save configuration to a JSON file.
             
             Args:
                 filepath: Path to save the JSON configuration file
                 
             Returns:
                 True if save succeeded, False otherwise
                 
             Raises:
                 ConfigurationError if save fails
             )"
          )
        .def("has", &Config::has, py::arg("key"),
             R"pbdoc(Check if a configuration key exists.
             
             Args:
                 key: Configuration key to check
                 
             Returns:
                 True if key exists, False otherwise
             )"
          )
        .def("getKeys", &Config::getKeys,
             R"pbdoc(Get all configuration keys.
             
             Returns:
                 List of all configuration keys
             )"
          )
        .def("get", [](Config& self, const std::string& key, bool& success) {
            try {
                auto value = self.get(key);
                success = true;
                return value;
            } catch (const std::exception&) {
                success = false;
                throw ConfigurationError("Key not found: " + key);
            }
        }, py::arg("key"), py::arg("success"),
           R"pbdoc(Get a configuration value with error checking.
           
           Args:
               key: Configuration key
               success: Output parameter for success status
               
           Returns:
               Configuration value as generic type
               
           Raises:
               ConfigurationError if key not found
           )"
          )
        .def("getOr", [](Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           R"pbdoc(Get a configuration value with default.
           
           Args:
               key: Configuration key
               defaultValue: Default value if key not found
               
           Returns:
               Configuration value or default if key not found
           )"
          )
        .def("set", &Config::set, py::arg("key"), py::arg("value"),
             R"pbdoc(Set a configuration value.
             
             Args:
                 key: Configuration key
                 value: Configuration value
                 
             Raises:
                 ConfigurationError if value type is invalid
             )"
          )
        .def("validate", &Config::validate,
             R"pbdoc(Validate configuration.
             
             Returns:
                 True if configuration is valid, False otherwise
                 
             Raises:
                 ConfigurationError if configuration is invalid
             )"
          )
        .def("getNeuronConfig", [](Config& self) {
            std::unordered_map<std::string, double> config;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                if (key.find("neuron") != std::string::npos || 
                    key.find("region") != std::string::npos ||
                    key.find("connection") != std::string::npos) {
                    config[key] = self.getOr(key, 0.0);
                }
            }
            return config;
        }, R"pbdoc(Get neuron-related configuration.
        
        Returns:
            Dictionary with neuron, region, and connection configuration
        )"
          )
        .def("getPlasticityConfig", [](Config& self) {
            std::unordered_map<std::string, double> config;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                if (key.find("stdp") != std::string::npos || 
                    key.find("plasticity") != std::string::npos ||
                    key.find("structural") != std::string::npos) {
                    config[key] = self.getOr(key, 0.0);
                }
            }
            return config;
        }, R"pbdoc(Get plasticity-related configuration.
        
        Returns:
            Dictionary with plasticity configuration parameters
        )"
          )
        .def("getSimulationConfig", [](Config& self) {
            std::unordered_map<std::string, double> config;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                if (key.find("simulation") != std::string::npos || 
                    key.find("timestep") != std::string::npos ||
                    key.find("random") != std::string::npos) {
                    config[key] = self.getOr(key, 0.0);
                }
            }
            return config;
        }, R"pbdoc(Get simulation-related configuration.
        
        Returns:
            Dictionary with simulation configuration parameters
        )"
          )
        .def("clear", &Config::clear,
             R"pbdoc(Clear all configuration entries.
             
             This removes all configuration keys and resets to default state.
             """
          )
        .def("summary", &Config::summary,
             R"pbdoc(Get a human-readable summary of the configuration.
             
             Returns:
                 String containing configuration summary
             )"
          )
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        });

    // Enhanced SensoryInput with abstract base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Abstract base class for sensory input)
        
        This is the base class for all sensory input types. Subclasses include:
        - Vision: Visual input from environment
        - Audio: Audio input
        - InternalSignals: Internal physiological signals
        
        Examples:
        >>> # Create vision input
        >>> vision = nlm.Vision(width=16, height=16)
        >>> vision.setData([0.1, 0.5, 0.9, ...])
        >>> 
        >>> # Process vision input
        >>> dimensions = vision.getDimensions()
        >>> data = vision.getData()
        >>> data_size = len(data)
    )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Enhanced Vision with Python-friendly interface
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Visual sensory input)
        
        Represents visual input from the agent's environment with configurable dimensions.
        
        Examples:
        >>> # Create standard vision input (16x16 with 3 channels)
        >>> vision = nlm.Vision(16, 16, 3)
        >>> 
        >>> # Set vision data
        >>> vision_data = [0.1, 0.2, 0.3, 0.4, 0.5] * 48  # 16*16*3 = 768 values
        >>> vision.setData(vision_data)
        >>> 
        >>> # Access vision properties
        >>> print(f"Width: {vision.getWidth()}, Height: {vision.getHeight()}")
        >>> print(f"Channels: {vision.getChannels()}")
        >>> print(f"Total pixels: {vision.getWidth() * vision.getHeight()}")
        
        Attributes:
            width: Vision width in pixels
            height: Vision height in pixels
            channels: Number of color channels (1=gray, 3=RGB)
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3,
             R"pbdoc(Create vision input with specified dimensions.
             
             Args:
                 width: Vision width in pixels
                 height: Vision height in pixels
                 channels: Number of color channels (1=gray, 3=RGB, default=3)
             )"
            )
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             R"pbdoc(Set vision data.
             
             Args:
                 data: Vector of pixel values, should match width*height*channels
                 
             Raises:
                 ValueError if data size doesn't match dimensions
             )"
            )
        .def("getWidth", &Vision::getWidth, "Get vision width in pixels")
        .def("getHeight", &Vision::getHeight, "Get vision height in pixels")
        .def("getChannels", &Vision::getChannels, "Get number of color channels")
        .def("getPixelCount", [](const Vision& self) {
            return self.getWidth() * self.getHeight();
        }, "Get total number of pixels")
        .def("getResolution", [](const Vision& self) {
            return std::make_pair(self.getWidth(), self.getHeight());
        }, "Get resolution as (width, height) tuple");

    // Enhanced Audio with Python-friendly interface
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)
        
        Represents audio input with configurable sample rate and duration.
        
        Examples:
        >>> # Create audio input (44.1kHz, 1 second)
        >>> audio = nlm.Audio(44100, 44100)
        >>> 
        >>> # Set audio data
        >>> audio_data = [0.0, 0.1, 0.2, ...] * 44100
        >>> audio.setData(audio_data)
        >>> 
        >>> # Access audio properties
        >>> print(f"Sample rate: {audio.getSampleRate()} Hz")
        >>> print(f"Duration: {audio.getNumSamples() / 44100:.2f} seconds")
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             R"pbdoc(Create audio input with specified parameters.
             
             Args:
                 sampleRate: Audio sample rate in Hz
                 numSamples: Number of audio samples
             )"
            )
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             R"pbdoc(Set audio data.
             
             Args:
                 data: Vector of audio samples
                 
             Raises:
                 ValueError if data size doesn't match numSamples
             )"
            )
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set audio sample rate")
        .def("getSampleRate", &Audio::getSampleRate, "Get audio sample rate")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of audio samples")
        .def("getDuration", [](const Audio& self) {
            return self.getNumSamples() / static_cast<float>(self.getSampleRate());
        }, "Get audio duration in seconds");

    // Enhanced InternalSignals with Python-friendly interface
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal physiological signals)
        
        Represents internal physiological signals such as energy levels,
        hunger, thirst, etc.
        
        Examples:
        >>> # Create internal signals
        >>> internal = nlm.InternalSignals()
        >>> 
        >>> # Add various signals
        >>> internal.addSignal(0.8)  # Energy level
        >>> internal.addSignal(0.3)  # Hunger level
        >>> internal.addSignal(0.6)  # Thirst level
        >>> 
        >>> # Access all signals
        >>> signals = internal.getAllSignals()
        >>> print(f"Energy: {signals[0]}, Hunger: {signals[1]}, Thirst: {signals[2]}")
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             R"pbdoc(Add an internal signal value.
             
             Args:
                 value: Signal value (typically 0.0 to 1.0)
             )"
            )
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all internal signals")
        .def("getSignalCount", &InternalSignals::getSignalCount,
             "Get number of signals")
        .def("getAllSignals", &InternalSignals::getAllSignals,
             "Get all signal values as a vector");

    // Enhanced Action with Python-friendly interface
    py::class_<Action>(m, "Action", R"pbdoc(Motor action representation)
        
        Represents an action that the agent can take in the environment.
        
        Examples:
        >>> # Create action by type
        >>> move_action = nlm.Action(nlm.ActionType.MoveForward)
        >>> 
        >>> # Create action with parameters
        >>> custom_action = nlm.Action(nlm.ActionType.Custom, [0.5, 0.8, 0.2])
        >>> 
        >>> # Access action properties
        >>> print(f"Type: {action.getType()}")
        >>> print(f"Parameters: {action.getParameters()}")
        >>> print(f"Name: {action.getName()}")
        
        Attributes:
            type: Action type (ActionType enum)
            parameters: Action parameters as vector
        """
    )
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType, "Get action type")
        .def("setType", &Action::setType, py::arg("type"), "Set action type")
        .def("getParameters", &Action::getParameters, "Get action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("getName", &Action::getName, "Get human-readable action name")
        .def("clone", &Action::clone, "Create a copy of this action")
        .def("to_dict", [](const Action& self) {
            std::unordered_map<std::string, py::object> result;
            result["type"] = py::cast(self.getType());
            result["parameters"] = py::cast(self.getParameters());
            result["name"] = py::cast(self.getName());
            return result;
        }, "Convert action to dictionary representation");

    // Enhanced WorldObject with Python-friendly interface
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)
        
        Represents an object in the simulated world with properties like position,
        type, and other attributes.
        
        Examples:
        >>> # Create empty world object
        >>> obj = nlm.WorldObject()
        >>> 
        >>> # Create resource object
        >>> resource = nlm.WorldObject(x=10, y=20, 
        ...                           type=nlm.WorldObjectType.Resource, 
        ...                           value=100.0, radius=0.5)
        >>> 
        >>> # Access object properties
        >>> print(f"Position: ({obj.x}, {obj.y})")
        >>> print(f"Type: {obj.type}")
        >>> print(f"Active: {obj.active}")
        
        Attributes:
            x: X coordinate
            y: Y coordinate
            radius: Object radius
            type: Object type (WorldObjectType enum)
            value: Object value (e.g., resource amount)
            active: Whether object is active
        """
    )
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x, "X coordinate")
        .def_readwrite("y", &WorldObject::y, "Y coordinate")
        .def_readwrite("radius", &WorldObject::radius, "Object radius")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Object value")
        .def_readwrite("active", &WorldObject::active, "Whether object is active")
        .def("distance_to", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position");

    // Enhanced AgentBody with Python-friendly interface
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state and dynamics)
        
        Represents the physical state of an agent in the world including position,
        velocity, orientation, and other physical properties.
        
        Examples:
        >>> # Create agent body
        >>> body = nlm.AgentBody()
        >>> 
        >>> # Set initial state
        >>> body.x = 10.0
        >>> body.y = 20.0
        >>> body.orientation = 45.0  # degrees
        >>> body.velocityX = 2.0
        >>> body.velocityY = 1.5
        >>> 
        >>> # Update body state
        >>> body.update(0.1)  # Update with timestep
        >>> print(f"New position: ({body.x}, {body.y})")
        
        Attributes:
            x: X position
            y: Y position
            orientation: Orientation in degrees
            velocityX: X velocity
            velocityY: Y velocity
            angularVelocity: Angular velocity
            energy: Energy level (0-1)
            health: Health level (0-1)
            age: Agent age in simulation steps
            isMoving: Whether agent is currently moving
            isTurning: Whether agent is currently turning
            lastActionTime: Time since last action
        """
    )
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position")
        .def_readwrite("y", &AgentBody::y, "Y position")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation in degrees")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Energy level (0-1)")
        .def_readwrite("health", &AgentBody::health, "Health level (0-1)")
        .def_readwrite("age", &AgentBody::age, "Agent age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time since last action")
        .def("reset", &AgentBody::reset, "Reset body state to defaults")
        .def("update", [](AgentBody& self, float timestep) {
            // Simple physics update
            self.x += self.velocityX * timestep;
            self.y += self.velocityY * timestep;
            self.orientation += self.angularVelocity * timestep;
            
            // Clamp angles
            self.orientation = std::fmod(self.orientation, 360.0f);
            if (self.orientation < 0) self.orientation += 360.0f;
            
            // Update movement flags
            self.isMoving = (self.velocityX != 0.0f || self.velocityY != 0.0f);
            self.isTurning = (std::abs(self.angularVelocity) > 0.01f);
            
            // Decay energy
            self.energy = std::max(0.0f, self.energy - 0.01f * timestep);
            self.health = std::max(0.0f, self.health - 0.005f * timestep);
            
            self.age += timestep;
            self.lastActionTime += timestep;
        }, py::arg("timestep"), "Update body state with physics simulation");

    // Enhanced ActionResult with Python-friendly interface
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Result of executing an action in the world)
        
        Represents the outcome of an action, including reward, success status,
        and any additional information.
        
        Examples:
        >>> # Create successful action result
        >>> success_result = nlm.ActionResult(reward=10.0, success=True, message="Collected food")
        >>> 
        >>> # Create failed action result
        >>> fail_result = nlm.ActionResult(reward=-5.0, success=False, message="Hit wall")
        >>> 
        >>> # Access result properties
        >>> print(f"Reward: {result.reward}")
        >>> print(f"Success: {result.success}")
        >>> print(f"Message: {result.message}")
        
        Attributes:
            reward: Reward value (positive for success, negative for failure)
            success: Whether action was successful
            message: Human-readable message describing result
        """
    )
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward, "Reward value")
        .def_readwrite("success", &ActionResult::success, "Success status")
        .def_readwrite("message", &ActionResult::message, "Result message")
        .def("is_positive", [](const ActionResult& self) {
            return self.reward > 0.0f;
        }, "Check if result is positive (reward > 0)")
        .def("is_negative", [](const ActionResult& self) {
            return self.reward < 0.0f;
        }, "Check if result is negative (reward < 0)")
        .def("to_dict", [](const ActionResult& self) {
            std::unordered_map<std::string, py::object> result;
            result["reward"] = py::cast(self.reward);
            result["success"] = py::cast(self.success);
            result["message"] = py::cast(self.message);
            result["is_positive"] = py::cast(self.is_positive());
            result["is_negative"] = py::cast(self.is_negative());
            return result;
        }, "Convert action result to dictionary representation");

    // Enhanced SensoryPercept with Python-friendly interface
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)
        
        Represents all sensory inputs that an agent can perceive at a given time,
        including vision, touch, internal signals, audio, and proprioception.
        
        Examples:
        >>> # Create sensory percept
        >>> percept = nlm.SensoryPercept()
        >>> 
        >>> # Set vision data
        >>> vision_data = [0.1, 0.2, 0.3, 0.4, 0.5] * 48  # 16x16x3
        >>> percept.setVision(vision_data)
        >>> 
        >>> # Set other sensory data
        >>> percept.setTouch([0.0, 0.1, 0.2, 0.3, 0.4])
        >>> percept.setInternal([0.8, 0.3, 0.6])
        >>> 
        >>> # Access percept data
        >>> vision = percept.getVision()
        >>> touch = percept.getTouch()
        >>> internal = percept.getInternal()
        >>> 
        >>> # Check percept completeness
        >>> all_signals = percept.getAllSignals()
        >>> print(f"Vision size: {len(vision)}")
        >>> print(f"Touch size: {len(touch)}")
        >>> print(f"Internal size: {len(internal)}")
        
        Methods:
            getVision(): Get visual input as vector
            setVision(): Set visual input
            getVisionWidth(): Get vision width
            getVisionHeight(): Get vision height
            getTouch(): Get tactile input
            setTouch(): Set tactile input
            getInternal(): Get internal signals
            setInternal(): Set internal signals
            getProprioception(): Get proprioceptive signals
            setProprioception(): Set proprioceptive signals
            getAudio(): Get audio input
            setAudio(): Set audio input
            getAllSignals(): Get all sensory signals
            getTimestamp(): Get timestamp
            setTimestamp(): Set timestamp
        """
    )
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get visual input")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set visual input")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision height")
        .def("getVisionShape", [](const SensoryPercept& self) {
            return std::make_pair(self.getVisionWidth(), self.getVisionHeight());
        }, "Get vision dimensions as (width, height) tuple")
        .def("getTouch", &SensoryPercept::getTouch, "Get tactile input")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set tactile input")
        .def("getInternal", &SensoryPercept::getInternal, "Get internal signals")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception, "Get proprioceptive signals")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioceptive signals")
        .def("getAudio", &SensoryPercept::getAudio, "Get audio input")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio input")
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all sensory signals")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set timestamp")
        .def("hasVision", [](const SensoryPercept& self) {
            return !self.getVision().empty();
        }, "Check if percept has vision data")
        .def("hasTouch", [](const SensoryPercept& self) {
            return !self.getTouch().empty();
        }, "Check if percept has touch data")
        .def("hasInternal", [](const SensoryPercept& self) {
            return !self.getInternal().empty();
        }, "Check if percept has internal signals")
        .def("hasAudio", [](const SensoryPercept& self) {
            return !self.getAudio().empty();
        }, "Check if percept has audio data")
        .def("to_dict", [](const SensoryPercept& self) {
            std::unordered_map<std::string, py::object> result;
            auto vision = self.getVision();
            auto touch = self.getTouch();
            auto internal = self.getInternal();
            auto proprioception = self.getProprioception();
            auto audio = self.getAudio();
            
            result["vision"] = py::cast(vision);
            result["touch"] = py::cast(touch);
            result["internal"] = py::cast(internal);
            result["proprioception"] = py::cast(proprioception);
            result["audio"] = py::cast(audio);
            result["timestamp"] = py::cast(self.getTimestamp());
            return result;
        }, "Convert sensory percept to dictionary representation")
        .def("__repr__", [](const SensoryPercept& self) {
            std::stringstream ss;
            ss << "<SensoryPercept: "
               << "vision=" << self.getVision().size() << ", "
               << "touch=" << self.getTouch().size() << ", "
               << "internal=" << self.getInternal().size() << ", "
               << "proprioception=" << self.getProprioception().size() << ", "
               << "audio=" << self.getAudio().size() << ">";
            return ss.str();
        })
        .def("__str__", [](const SensoryPercept& self) {
            return self.__repr__();
        });

    // Enhanced SimpleWorld with Python-friendly interface
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)
        
        This class provides a simple 2D world environment for testing NLM agents.
        It includes object management, collision detection, physics simulation,
        and sensory perception.
        
        Examples:
        >>> # Create world
        >>> world = nlm.SimpleWorld()
        >>> 
        >>> # Configure world
        >>> world.configure(width=100, height=100, visionWidth=16, visionHeight=16)
        >>> 
        >>> # Add objects
        >>> resource = nlm.WorldObject(x=10, y=20, 
        ...                           type=nlm.WorldObjectType.Resource, 
        ...                           value=100.0)
        >>> world.addObject(resource)
        >>> 
        >>> # Place agent
        >>> world.setAgentStart(5, 5)
        >>> 
        >>> # Update world
        >>> for step in range(100):
        ...     world.update(0.1)  # timestep
        ...     percept = world.getSensoryPercept()
        ...     body = world.getAgentBody()
        ...     print(f"Step {step}: Position ({body.x:.1f}, {body.y:.1f})")
        
        Attributes:
            width: World width in units
            height: World height in units
            visionWidth: Agent vision width in pixels
            visionHeight: Agent vision height in pixels
            maxEnergy: Maximum agent energy
            energyDecayRate: Energy decay rate per timestep
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             R"pbdoc(Configure the world dimensions and agent sensing.
             
             Args:
                 width: World width in units
                 height: World height in units
                 visionWidth: Agent vision width in pixels
                 visionHeight: Agent vision height in pixels
             )"
            )
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world physics and agent perception")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             R"pbdoc(Apply a motor command to the agent.
             
             Args:
                 cmd: Motor command to apply
                 currentTime: Current simulation time
                 
             Returns:
                 ActionResult describing the outcome
             )"
            )
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get current agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add an object to the world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at given coordinates")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (within bounds and not occupied)")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum agent energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum agent energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed")
        .def("getObjectAt", [](const SimpleWorld& self, float x, float y) {
            // This would require accessing internal world state
            return nlm.WorldObject(); // Placeholder
        }, py::arg("x"), py::arg("y"), "Get object at position (placeholder)")
        .def("getAgentPosition", [](const SimpleWorld& self) {
            auto body = self.getAgentBody();
            return std::make_pair(body.x, body.y);
        }, "Get agent position as (x, y) tuple")
        .def("getAgentDirection", [](const SimpleWorld& self) {
            auto body = self.getAgentBody();
            return body.orientation;
        }, "Get agent orientation/direction in degrees")
        .def("to_dict", [](const SimpleWorld& self) {
            std::unordered_map<std::string, py::object> result;
            result["width"] = py::cast(self.getWidth());
            result["height"] = py::cast(self.getHeight());
            result["simulation_time"] = py::cast(self.getSimulationTime());
            return result;
        }, "Convert world state to dictionary representation");

    // Enhanced Brain with Python-friendly interface
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)
        
        This is the main brain class that implements the complete neural simulation
        including neurons, synapses, plasticity, memory, and action generation.
        
        Examples:
        >>> # Create brain with default configuration
        >>> brain = nlm.Brain()
        >>> 
        >>> # Initialize brain
        >>> brain.initialize()
        >>> print("Brain initialized:", brain.getTotalNeuronCount(), "neurons")
        >>> 
        >>> # Run simulation loop
        >>> for step in range(1000):
        ...     brain.step(step)
        ...     if step % 100 == 0:
        ...         print(f"Step {step}: {brain.getTotalSpikeCount()} spikes")
        >>> 
        >>> # Save brain state
        >>> brain.save("brain_state.nxml")
        >>> 
        >>> # Load brain state
        >>> new_brain = nlm.Brain()
        >>> new_brain.load("brain_state.nxml")
        
        This class provides access to all NLM functionality including:
        - Neuron and synapse management
        - Plasticity (STDP, Hebbian learning)
        - Memory systems (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Cognition (planning, attention, concept formation)
        - Development and structural plasticity
        - Action generation and motor control
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &Brain::initialize,
             R"pbdoc(Initialize the brain with current configuration.
             
             Returns:
                 True if initialization succeeded, False otherwise
                 
             Raises:
                 InitializationError if brain initialization fails
             )"
            )
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             R"pbdoc(Perform a simulation step.
             
             Args:
                 currentStep: Current simulation step number
             )"
            )
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             R"pbdoc(Perform a simulation step with timestamp.
             
             Args:
                 currentStep: Current simulation step number
                 currentTime: Current simulation time in seconds
             )"
            )
        .def("stepWithTime", [](Brain& self, size_t step, float timestep) {
            self.step(step, step * timestep);
        }, py::arg("step"), py::arg("timestep"),
            R"pbdoc(Perform a simulation step with explicit timestep.
            
            Args:
                step: Current simulation step number
                timestep: Time between steps in seconds
            )"
            )
        .def("stepMultiple", [](Brain& self, size_t steps, float timestep) {
            for (size_t i = 0; i < steps; ++i) {
                self.step(i, i * timestep);
            }
        }, py::arg("steps"), py::arg("timestep"),
            R"pbdoc(Perform multiple simulation steps.
            
            Args:
                steps: Number of steps to simulate
                timestep: Time between steps in seconds
            )"
            )
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             R"pbdoc(Inject sensory input into the brain.
             
             Args:
                 input: Sensory input object (Vision, Audio, InternalSignals, etc.)
             )"
            )
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             R"pbdoc(Inject current into a specific neuron.
             
             Args:
                 neuron: Neuron ID
                 current: Current amount in membrane potential units
             )"
            )
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             R"pbdoc(Inject current into all neurons of a specific type.
             
             Args:
                 type: Neuron type (Sensory, Motor, Internal, etc.)
                 current: Current amount in membrane potential units
             )"
            )
        .def("produceAction", &Brain::produceAction,
             R"pbdoc(Produce motor action based on neural activity.
             
             Returns:
                 Action object representing the produced action
             )"
            )
        .def("reset", &Brain::reset,
             R"pbdoc(Reset brain state to initial conditions.
             
             This resets:
             - All neuron states
             - Synaptic weights
             - Memory systems
             - Development stage
             - Neuromodulation levels
             """
            )
        .def("save", &Brain::save, py::arg("filepath"),
             R"pbdoc(Save brain state to file.
             
             Args:
                 filepath: Path to save the brain state
                 
             Returns:
                 True if save succeeded, False otherwise
                 
             Raises:
                 SimulationError if save fails
             )"
            )
        .def("load", &Brain::load, py::arg("filepath"),
             R"pbdoc(Load brain state from file.
             
             Args:
                 filepath: Path to load brain state from
                 
             Returns:
                 True if load succeeded, False otherwise
                 
             Raises:
                 SimulationError if load fails
             )"
            )
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             R"pbdoc(Add a new neural region to the brain.
             
             Args:
                 name: Optional name for the region
                 
             Returns:
                 Region ID of the newly created region
             )"
            )
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             R"pbdoc(Get a neural region by ID.
             
             Args:
                 id: Region ID
                 
             Returns:
                 NeuralRegion object or None if not found
             )"
            )
        .def("getRegionCount", &Brain::getRegionCount,
             R"pbdoc(Get the number of regions in the brain.
             
             Returns:
                 Number of regions
             )"
            )
        .def("getRegionIds", &Brain::getRegionIds,
             R"pbdoc(Get all region IDs.
             
             Returns:
                 List of region IDs
             )"
            )
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get all regions.
             
             Returns:
                 List of all neural regions
             )"
            )
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             R"pbdoc(Get total neuron count across all regions.
             
             Returns:
                 Total number of neurons
             )"
            )
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             R"pbdoc(Get total synapse count across all regions.
             
             Returns:
                 Total number of synapses
             )"
            )
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             R"pbdoc(Get count of active neurons.
             
             Returns:
                 Number of active neurons
             )"
            )
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             R"pbdoc(Get count of currently firing neurons.
             
             Returns:
                 Number of firing neurons
             )"
            )
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             R"pbdoc(Get average firing rate across all neurons.
             
             Returns:
                 Average firing rate in Hz
             )"
            )
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             R"pbdoc(Get excitation/inhibition balance ratio.
             
             Returns:
                 Excitation/inhibition ratio (infinity if no inhibition)
             )"
            )
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             R"pbdoc(Get total spike count.
             
             Returns:
                 Total number of spikes across all neurons
             )"
            )
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             R"pbdoc(Get count of pending spike events.
             
             Returns:
                 Number of pending spike events
             )"
            )
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             R"pbdoc(Get current developmental stage.
             
             Returns:
                 Developmental stage (DevelopmentalStage enum)
             )"
            )
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             R"pbdoc(Set developmental stage.
             
             Args:
                 stage: Developmental stage to set
             )"
            )
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get the brain's configuration.
             
             Returns:
                 Config object used by this brain
             )"
            )
        .def("logStatus", &Brain::logStatus,
             R"pbdoc(Log brain status and statistics.
             
             This logs:
             - Number of regions, neurons, and synapses
             - Activity statistics (spikes, firing rates)
             - Memory system status
             - Neuromodulation levels
             - Development stage
             """
            )
        .def("runTest", [](Brain& self, const std::string& testName, float duration) {
            if (testName == "basic_connectivity") {
                // Run basic connectivity test
                auto* region = self.getRegion(RegionId(1));
                if (!region) return std::vector<float>();
                
                auto neurons = region->getAllNeurons();
                if (neurons.empty()) return std::vector<float>();
                
                size_t initialSpikes = self.getTotalSpikeCount();
                for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
                    neurons[i]->injectCurrent(50.0f);
                }
                
                std::vector<float> results;
                for (size_t step = 0; step < duration * 1000; ++step) {
                    self.step(step, step * 0.001f);
                    if (step % 100 == 0) {
                        results.push_back(self.getTotalSpikeCount() - initialSpikes);
                    }
                }
                return results;
            } else if (testName == "plasticity") {
                // Run plasticity test
                self.reset();
                self.initialize();
                
                // Enable plasticity
                if (auto* region = self.getRegion(RegionId(1))) {
                    for (auto& syn : region->getSynapses()) {
                        syn->enablePlasticity(true, true, false);
                    }
                }
                
                std::vector<float> weights;
                for (size_t step = 0; step < duration * 1000; ++step) {
                    // Apply input pattern
                    self.injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
                    self.step(step, step * 0.001f);
                    
                    if (step % 100 == 0) {
                        float meanWeight = self.getExcitationInhibitionRatio();
                        weights.push_back(meanWeight);
                    }
                }
                return weights;
            } else if (testName == "stdp") {
                // Run STDP test
                self.reset();
                self.initialize();
                
                auto* region = self.getRegion(RegionId(1));
                if (!region || region->getSynapses().size() < 5) return std::vector<float>();
                
                std::vector<float> beforeWeights;
                for (size_t i = 0; i < 5 && i < region->getSynapses().size(); ++i) {
                    beforeWeights.push_back(region->getSynapses()[i]->getWeight());
                    region->getSynapses()[i]->enablePlasticity(false, true, false);
                }
                
                std::vector<float> afterWeights;
                for (int trial = 0; trial < 50; ++trial) {
                    auto neurons = region->getAllNeurons();
                    if (neurons.size() >= 2) {
                        neurons[0]->injectCurrent(60.0f);
                        self.step(trial * 2, trial * 2 * 0.001f);
                        
                        neurons[1]->injectCurrent(60.0f);
                        self.step(trial * 2 + 1, (trial * 2 + 1) * 0.001f);
                    }
                }
                
                for (size_t i = 0; i < 5 && i < region->getSynapses().size(); ++i) {
                    afterWeights.push_back(region->getSynapses()[i]->getWeight());
                }
                
                std::vector<float> deltas;
                for (size_t i = 0; i < std::min(beforeWeights.size(), afterWeights.size()); ++i) {
                    deltas.push_back(afterWeights[i] - beforeWeights[i]);
                }
                return deltas;
            }
            return std::vector<float>();
        }, py::arg("testName"), py::arg("duration"),
            R"pbdoc(Run a pre-defined test.
            
            Args:
                testName: Name of test ('basic_connectivity', 'plasticity', or 'stdp')
                duration: Test duration in seconds
                
            Returns:
                Test results as a list of floats
            )"
            )
        .def("runLearningExperiment", [](Brain& self, uint64_t seed, float duration) {
            std::vector<float> results;
            
            // Enhanced learning experiment
            auto config = std::make_shared<Config>();
            config->set("random_seed", static_cast<int64_t>(seed), ConfigSource::Default);
            
            auto testBrain = std::make_shared<Brain>(config);
            if (!testBrain->initialize()) {
                throw InitializationError("Failed to initialize brain for learning experiment");
            }
            
            // Record initial state
            std::vector<float> initialWeights;
            if (auto* region = testBrain->getRegion(RegionId(1))) {
                for (const auto& syn : region->getSynapses()) {
                    initialWeights.push_back(syn->getWeight());
                }
            }
            
            // Run learning phase
            for (SimulationStep step = 0; step < duration * 1000; ++step) {
                // Apply varied input patterns
                for (size_t i = 0; i < 20 && i < testBrain->getTotalNeuronCount() / 4; ++i) {
                    testBrain->injectCurrentToNeurons(NeuronType::Sensory, 
                        30.0f * (1.0f + 0.1f * sin(static_cast<float>(step) * 0.01f)));
                }
                testBrain->step(step, step * 0.001f);
                
                if (step % 100 == 0) {
                    results.push_back(testBrain->getTotalSpikeCount());
                }
            }
            
            // Calculate learning metrics
            std::vector<float> finalWeights;
            if (auto* region = testBrain->getRegion(RegionId(1))) {
                for (const auto& syn : region->getSynapses()) {
                    finalWeights.push_back(syn->getWeight());
                }
            }
            
            float initialMean = 0.0f, finalMean = 0.0f;
            if (!initialWeights.empty()) {
                float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
                initialMean = initialSum / initialWeights.size();
            }
            if (!finalWeights.empty()) {
                float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
                finalMean = finalSum / finalWeights.size();
            }
            
            results.push_back(initialMean);
            results.push_back(finalMean);
            results.push_back(finalMean - initialMean);
            
            return results;
        }, py::arg("seed"), py::arg("duration"),
            R"pbdoc(Run a learning experiment.
            
            Args:
                seed: Random seed for reproducibility
                duration: Experiment duration in seconds
                
            Returns:
                Learning experiment results including spike counts, weights, and learning metrics
            )"
            );

    // Enhanced AgentBrain with Python-friendly interface
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)
        
        This class provides an interface between the NLM brain and the external world,
        handling sensory input, motor output, and agent-specific processing.
        
        Examples:
        >>> # Create agent brain
        >>> brain = nlm.Brain()
        >>> agent_brain = nlm.AgentBrain(brain)
        >>> 
        >>> # Create world
        >>> world = nlm.SimpleWorld()
        >>> world.configure(width=100, height=100, visionWidth=16, visionHeight=16)
        >>> 
        >>> # Initialize agent
        >>> agent_brain.initialize(world)
        >>> 
        >>> # Run perception-action loop
        >>> for step in range(100):
        ...     # Get sensory percept
        ...     percept = world.getSensoryPercept()
        ...     
        ...     # Process sensory input
        ...     agent_brain.processSensoryInput(percept)
        ...     
        ...     # Update brain
        ...     brain.step(step)
        ...     
        ...     # Decode motor command
        ...     cmd = agent_brain.decodeMotorCommand()
        ...     
        ...     # Apply to world
        ...     result = world.applyMotorCommand(cmd, step)
        ...     
        ...     # Apply reward modulation if needed
        ...     if result.success:
        ...     agent_brain.applyRewardModulation(result.reward, agent_brain.getExpectedReward())
        ...     
        ...     # Update development
        ...     agent_brain.updateDevelopment(0.1)
        
        This class handles:
        - Sensory input processing and brain injection
        - Motor command decoding from neural activity
        - Reward-based learning and neuromodulation
        - Agent development and aging
        - Curiosity-driven exploration
        - Novelty detection and adaptation
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             R"pbdoc(Create agent brain interface.
             
             Args:
                 brain: Underlying NLM brain instance
             )"
            )
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             R"pbdoc(Initialize agent brain with world.
             
             Args:
                 world: World instance for agent interaction
             )"
            )
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             R"pbdoc(Get expected sensory input size.
             
             Returns:
                 Total number of sensory input values expected
             )"
            )
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             R"pbdoc(Get expected motor output size.
             
             Returns:
                 Number of possible motor commands
             )"
            )
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             R"pbdoc(Process sensory percept and inject into brain.
             
             Args:
                 percept: Sensory percept containing all sensory inputs
             )"
            )
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             R"pbdoc(Decode brain motor activity into motor command.
             
             Returns:
                 MotorCommand for agent action
             )"
            )
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             R"pbdoc(Apply reward-based neuromodulation.
             
             Args:
                 reward: Actual reward received
                 predictedReward: Expected reward based on prediction
             )"
            )
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             R"pbdoc(Update development system.
             
             Args:
                 timestep: Time step for development update
             )"
            )
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             R"pbdoc(Get current developmental stage.
             
             Returns:
                 Developmental stage
             )"
            )
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             R"pbdoc(Get current neuromodulation level (dopamine).
             
             Returns:
                 Neuromodulation level (-2.0 to 2.0)
             )"
            )
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             R"pbdoc(Get current curiosity level.
             
             Returns:
                 Curiosity level (0.0 to 1.0)
             )"
            )
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             R"pbdoc(Get current novelty level.
             
             Returns:
                 Novelty level (0.0 to 1.0)
             )"
            )
        .def("getPredictionError", &AgentBrain::getPredictionError,
             R"pbdoc(Get current prediction error.
             
             Returns:
                 Prediction error value (-2.0 to 2.0)
             )"
            )
        .def("reset", &AgentBrain::reset,
             R"pbdoc(Reset agent for new episode.
             
             This resets:
             - All neuromodulation levels
             - Development state
             - Curiosity and novelty
             - Previous sensory history
             - Neuron distribution configuration
             """
            )
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get the underlying brain.
             
             Returns:
                 Brain instance used by this agent brain
             )"
            )
        .def("enableRewardModulation", [](AgentBrain& self, bool enable) {
            self.rewardModulationEnabled_ = enable;
        }, py::arg("enable"),
             R"pbdoc(Enable or disable reward modulation.
             
             Args:
                 enable: True to enable reward modulation, False to disable
             )"
            )
        .def("enableStructuralPlasticity", [](AgentBrain& self, bool enable) {
            self.structuralPlasticityEnabled_ = enable;
        }, py::arg("enable"),
             R"pbdoc(Enable or disable structural plasticity.
             
             Args:
                 enable: True to enable structural plasticity, False to disable
             )"
            )
        .def("enableDevelopment", [](AgentBrain& self, bool enable) {
            self.developmentEnabled_ = enable;
        }, py::arg("enable"),
             R"pbdoc(Enable or disable development.
             
             Args:
                 enable: True to enable development, False to disable
             )"
            )
        .def("enableCuriosity", [](AgentBrain& self, bool enable) {
            self.curiosityEnabled_ = enable;
        }, py::arg("enable"),
             R"pbdoc(Enable or disable curiosity-driven exploration.
             
             Args:
                 enable: True to enable curiosity, False to disable
             )"
            )
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             R"pbdoc(Check if reward modulation is enabled.
             
             Returns:
                 True if reward modulation is enabled, False otherwise
             )"
            )
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             R"pbdoc(Check if structural plasticity is enabled.
             
             Returns:
                 True if structural plasticity is enabled, False otherwise
             )"
            )
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             R"pbdoc(Check if development is enabled.
             
             Returns:
                 True if development is enabled, False otherwise
             )"
            )
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             R"pbdoc(Check if curiosity is enabled.
             
             Returns:
                 True if curiosity is enabled, False otherwise
             )"
            )
        .def("setNeuronDistributionConfig", [](AgentBrain& self, 
            const std::vector<std::string>& motorGroups,
            const std::vector<std::string>& sensoryGroups,
            const std::vector<size_t>& motorNeuronsPerGroup,
            const std::vector<size_t>& sensoryNeuronsPerGroup) {
            
            AgentBrain::NeuronDistributionConfig config;
            config.motor.motorGroupNames = motorGroups;
            config.motor.neuronsPerGroup = motorNeuronsPerGroup;
            config.motor.useModularDistribution = true;
            
            config.sensory.sensoryGroupNames = sensoryGroups;
            config.sensory.neuronsPerGroup = sensoryNeuronsPerGroup;
            config.sensory.useModularDistribution = true;
            
            self.setNeuronDistributionConfig(config);
        }, py::arg("motorGroups"), py::arg("sensoryGroups"),
           py::arg("motorNeuronsPerGroup"), py::arg("sensoryNeuronsPerGroup"),
           R"pbdoc(Set neuron distribution configuration.
           
           Args:
               motorGroups: List of motor group names
               sensoryGroups: List of sensory group names
               motorNeuronsPerGroup: Neurons per motor group
               sensoryNeuronsPerGroup: Neurons per sensory group
               
           Note:
               This must match the actual number of groups provided
           )"
          )
        .def("getMotorGroupNames", &AgentBrain::getMotorGroupNames,
             R"pbdoc(Get list of motor group names.
             
             Returns:
                 List of motor group names
             )"
            )
        .def("getSensoryGroupNames", &AgentBrain::getSensoryGroupNames,
             R"pbdoc(Get list of sensory group names.
             
             Returns:
                 List of sensory group names
             )"
            )
        .def("getNeuronCounts", [](const AgentBrain& self) {
            std::unordered_map<std::string, size_t> counts;
            counts["motor_forward"] = self.getMotorForwardNeurons().size();
            counts["motor_backward"] = self.getMotorBackwardNeurons().size();
            counts["motor_turn_left"] = self.getMotorTurnLeftNeurons().size();
            counts["motor_turn_right"] = self.getMotorTurnRightNeurons().size();
            counts["motor_interact"] = self.getMotorInteractNeurons().size();
            counts["motor_wait"] = self.getMotorWaitNeurons().size();
            counts["sensory_vision"] = self.getSensoryVisionNeurons().size();
            counts["sensory_touch"] = self.getSensoryTouchNeurons().size();
            counts["sensory_internal"] = self.getSensoryInternalNeurons().size();
            counts["sensory_proprioception"] = self.getSensoryProprioceptionNeurons().size();
            counts["total_motor"] = self.getTotalMotorNeurons();
            counts["total_sensory"] = self.getTotalSensoryNeurons();
            return counts;
        }, R"pbdoc(Get neuron distribution counts.
        
        Returns:
            Dictionary with neuron counts for each group and totals
        )"
          )
        .def("runLearningEpisode", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world, 
            std::function<float(const ActionResult&)> rewardFunction) {
            
            float totalReward = 0.0f;
            std::vector<float> episodeRewards;
            
            for (size_t step = 0; step < 1000; ++step) {
                // Get sensory percept
                auto percept = world->getSensoryPercept();
                
                // Process sensory input
                self.processSensoryInput(percept);
                
                // Step brain
                self.getBrain()->step(step);
                
                // Decode motor command
                auto cmd = self.decodeMotorCommand();
                
                // Apply motor command
                auto result = world->applyMotorCommand(cmd, step);
                
                // Apply reward modulation
                self.applyRewardModulation(result.reward, self.getExpectedReward());
                
                // Get reward from reward function
                float reward = rewardFunction(result);
                
                // Accumulate rewards
                totalReward += reward;
                episodeRewards.push_back(reward);
                
                // Update development
                self.updateDevelopment(0.1);
                
                // Check for episode termination
                if (result.success && result.message.find("goal") != std::string::npos) {
                    break;
                }
                
                if (step % 100 == 0) {
                    world->update(0.1);
                }
            }
            
            return std::make_pair(totalReward, episodeRewards);
        }, py::arg("world"), py::arg("rewardFunction"),
            R"pbdoc(Run a complete learning episode.
            
            Args:
                world: World instance for agent interaction
                rewardFunction: Function that takes ActionResult and returns reward
                
            Returns:
                Tuple of (total_reward, episode_rewards_list)
            )"
          );

    // Convenience functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, R"pbdoc(Create a default configuration.
    
    Returns:
        Default Config object
        
    Examples:
        >>> config = nlm.createDefaultConfig()
        >>> config.set("neuron_count", 1000)
    )pbdoc");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), R"pbdoc(Create a new brain with configuration.
    
    Args:
        config: Configuration object (use createDefaultConfig() for defaults)
        
    Returns:
        Brain object
        
    Examples:
        >>> brain = nlm.createBrain()
        >>> brain.initialize()
    )pbdoc");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(Create a new simple world.
    
    Returns:
        SimpleWorld object
        
    Examples:
        >>> world = nlm.createSimpleWorld()
        >>> world.configure(width=100, height=100)
    )pbdoc");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(Create a new agent brain interface.
    
    Args:
        brain: Underlying NLM brain instance
        
    Returns:
        AgentBrain object
        
    Examples:
        >>> brain = nlm.createBrain()
        >>> agent_brain = nlm.createAgentBrain(brain)
    )pbdoc");

    // Configuration presets
    m.def("createExplorationConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 500, ConfigSource::Default);
        config->set("region_count", 2, ConfigSource::Default);
        config->set("connection_probability", 0.15, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.025, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.0002, ConfigSource::Default);
        config->set("pruning_rate", 0.00002, ConfigSource::Default);
        return config;
    }, R"pbdoc(Create exploration-focused configuration.
    
    Returns:
        Config optimized for exploration and learning
        
    This configuration promotes:
        - Higher exploration drive
        - Enhanced plasticity
        - Dynamic restructuring
    )pbdoc");

    m.def("createNavigationConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 1000, ConfigSource::Default);
        config->set("region_count", 1, ConfigSource::Default);
        config->set("connection_probability", 0.1, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.02, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.018, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.0001, ConfigSource::Default);
        config->set("pruning_rate", 0.00001, ConfigSource::Default);
        return config;
    }, R"pbdoc(Create navigation-focused configuration.
    
    Returns:
        Config optimized for navigation and action
        
    This configuration promotes:
        - Higher movement capabilities
        - Stable plasticity
        - Efficient action selection
    )pbdoc");

    m.def("createLearningConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 750, ConfigSource::Default);
        config->set("region_count", 2, ConfigSource::Default);
        config->set("connection_probability", 0.12, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.022, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.017, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.00015, ConfigSource::Default);
        config->set("pruning_rate", 0.000015, ConfigSource::Default);
        return config;
    }, R"pbdoc(Create learning-focused configuration.
    
    Returns:
        Config optimized for learning and adaptation
        
    This configuration promotes:
        - Balanced exploration and exploitation
        - Enhanced learning capacity
        - Long-term memory formation
    )pbdoc");

    // Version and metadata
    m.attr("VERSION") = "2.0.0";
    m.attr("NLM_VERSION") = "Phase 2 Enhanced";
    m.attr("PYTHON_BINDINGS_VERSION") = "2.0.0";
    
    m.def("getVersion", []() {
        return std::string("NLM Python Bindings v2.0.0 - Phase 2 Enhanced");
    }, "Get version information");
    
    m.def("getSupportedFeatures", []() {
        return std::vector<std::string>{
            "Brain Simulation with 15-step integrated loop",
            "Enhanced Plasticity (STDP + Hebbian + Structural)",
            "Complete Memory Systems (Working + Episodic + Associative)",
            "Neuromodulation (Dopamine + Curiosity + Novelty)",
            "Cognition Systems (Planning + Attention + Concept Formation)",
            "Development System with staged maturation",
            "AgentBrain interface for robotics control",
            "World simulation for testing",
            "Configurable neuron distribution",
            "Enhanced Python API with examples",
            "Error handling and validation",
            "Performance monitoring utilities"
        };
    }, "Get list of supported features");

    // Documentation and examples
    m.def("getExamples", []() {
        return std::vector<std::string>{
            "basic_usage.py - Basic brain simulation",
            "agent_example.py - Agent with world interaction",
            "learning_example.py - Learning episodes and reward",
            "visualization_example.py - Visualization and monitoring",
            "configuration_example.py - Configuration management"
        };
    }, "Get list of example files");

    // Get constants for Python users
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Add error classes to module namespace
    py::class_<NLMException>(m, "NLMException", py::module_local())
        .def(py::init<const std::string&>());
    
    py::class_<ConfigurationError>(m, "ConfigurationError", py::module_local())
        .def(py::init<const std::string&>());
    
    py::class_<InitializationError>(m, "InitializationError", py::module_local())
        .def(py::init<const std::string&>());
    
    py::class_<SimulationError>(m, "SimulationError", py::module_local())
        .def(py::init<const std::string&>());

    // Module-level documentation
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        -------------------------------------------------------
        
        Enhanced Python interface for NLM with:
        
        1. Comprehensive Documentation: Examples, tutorials, and API references
        2. Enhanced Error Handling: Custom exceptions for different error types
        3. Configuration Management: Type-safe configuration with validation
        4. Agent Interface: Complete interface for robotics and control applications
        5. World Simulation: Virtual environment for testing and evaluation
        6. Performance Monitoring: Tools for performance analysis
        7. Learning Examples: Pre-built examples for common use cases
        
        For detailed documentation, examples, and tutorials, visit:
        https://github.com/nlm/NLM/wiki/Python-Bindings
        
        Quick start with examples:
        >>> import pynlm as nlm
        >>> # Basic usage
        >>> brain = nlm.createBrain()
        >>> brain.initialize()
        >>> 
        >>> # Advanced usage
        >>> brain = nlm.createBrain(nlm.createExplorationConfig())
        >>> agent_brain = nlm.createAgentBrain(brain)
        >>> world = nlm.createSimpleWorld()
        >>> agent_brain.initialize(world)
        >>> 
        >>> # Run learning episode
        >>> def reward_function(result):
        ...     return 10.0 if result.success else -5.0
        >>> total_reward, rewards = agent_brain.runLearningEpisode(world, reward_function)
    )pbdoc";

    // Add warning about module maturity
    m.def("warnMaturity", []() {
        std::cout << "WARNING: This is an enhanced development version of the NLM Python bindings." << std::endl;
        std::cout << "Some APIs may change between versions. For production use, refer to stable releases." << std::endl;
    });

} // namespace nlm
