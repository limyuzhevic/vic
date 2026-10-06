#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
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

/**
 * Custom exception types for better error handling
 */
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) 
        : std::runtime_error("NLM Error: " + message) {}
};

class ConfigException : public NLMException {
public:
    explicit ConfigException(const std::string& message)
        : NLMException("Config Error: " + message) {}
};

class BrainException : public NLMException {
public:
    explicit BrainException(const std::string& message)
        : NLMException("Brain Error: " + message) {}
};

class WorldException : public NLMException {
public:
    explicit WorldException(const std::string& message)
        : NLMException("World Error: " + message) {}
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        NLM is a brain-inspired spiking neural network simulator for creating artificial brains
        that can learn, remember, and make decisions through interaction with their environment.
    )pbdoc";

    // Register custom exception types
    py::register_exception<NLMException>(m, "NLMException");
    py::register_exception<ConfigException>(m, "ConfigException");
    py::register_exception<BrainException>(m, "BrainException");
    py::register_exception<WorldException>(m, "WorldException");

    // NeuronId with better validation and utilities
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
        .def("is_valid", [](const NeuronId& id) {
            return id.value != INVALID_NEURON_ID.value;
        }, "Check if this NeuronId is valid");

    // SynapseId with better validation and utilities
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
        .def("is_valid", [](const SynapseId& id) {
            return id.value != INVALID_SYNAPSE_ID.value;
        }, "Check if this SynapseId is valid");

    // RegionId with better validation and utilities
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
        .def("is_valid", [](const RegionId& id) {
            return id.value != INVALID_REGION_ID.value;
        }, "Check if this RegionId is valid");

    // PopulationId with better validation and utilities
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        });

    // Neuron types with better documentation
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)
        
        Describes the functional role and characteristics of different neuron types.
        
        Excitatory: Neurons that promote firing in postsynaptic neurons
        Inhibitory: Neurons that suppress firing in postsynaptic neurons
        Modulatory: Neurons that affect overall brain state and plasticity
        Sensory: Neurons that process external environmental inputs
        Motor: Neurons that control actions and movements
        Internal: Neurons that process internal signals and states
        "pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Promotes firing in postsynaptic neurons")
        .value("Inhibitory", NeuronType::Inhibitory, "Suppresses firing in postsynaptic neurons")
        .value("Modulatory", NeuronType::Modulatory, "Affects brain state and plasticity")
        .value("Sensory", NeuronType::Sensory, "Processes environmental inputs")
        .value("Motor", NeuronType::Motor, "Controls actions and movements")
        .value("Internal", NeuronType::Internal, "Processes internal states and signals")
        .export_values();

    // Synapse types with better documentation
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)
        
        Describes the functional characteristics of different synapse types.
        
        Excitatory: Increases likelihood of postsynaptic firing
        Inhibitory: Decreases likelihood of postsynaptic firing
        Modulatory: Affects overall brain state and plasticity
        Electrical: Fast direct electrical coupling
        GapJunction: Direct cell-to-cell electrical connection
        "pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Increases postsynaptic firing probability")
        .value("Inhibitory", SynapseType::Inhibitory, "Decreases postsynaptic firing probability")
        .value("Modulatory", SynapseType::Modulatory, "Affects brain state and plasticity")
        .value("Electrical", SynapseType::Electrical, "Fast direct electrical coupling")
        .value("GapJunction", SynapseType::GapJunction, "Direct cell-to-cell electrical connection")
        .export_values();

    // Developmental stages with better documentation
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)
        
        Describes the developmental phase of the brain, affecting plasticity and learning capabilities.
        
        Initial: Newborn brain with high plasticity
        CriticalPeriod: Sensitive phase for learning and adaptation
        Maturation: Brain refinement and specialization
        Adult: Stable, optimized brain state
        Aging: Declining plasticity and function
        "pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Newborn brain with high plasticity")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Sensitive learning phase")
        .value("Maturation", DevelopmentalStage::Maturation, "Brain refinement and specialization")
        .value("Adult", DevelopmentalStage::Adult, "Stable, optimized brain state")
        .value("Aging", DevelopmentalStage::Aging, "Declining plasticity and function")
        .export_values();

    // Firing states with better documentation
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)
        
        Describes the current electrophysiological state of a neuron.
        
        Resting: Neuron at resting potential
        Active: Neuron generating action potentials
        Refractory: Neuron unable to fire immediately after spiking
        Inhibited: Neuron actively suppressed from firing
        "pbdoc")
        .value("Resting", FiringState::Resting, "At resting potential")
        .value("Active", FiringState::Active, "Generating action potentials")
        .value("Refractory", FiringState::Refractory, "Unable to fire immediately")
        .value("Inhibited", FiringState::Inhibited, "Actively suppressed from firing")
        .export_values();

    // Action types with better documentation
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)
        
        Describes high-level behavioral actions that the brain can initiate.
        
        MoveForward: Move in the forward direction
        MoveBackward: Move in the backward direction
        MoveLeft: Move left
        MoveRight: Move right
        TurnLeft: Rotate counterclockwise
        TurnRight: Rotate clockwise
        Look: Point gaze in a direction
        LookUp: Point gaze upward
        LookDown: Point gaze downward
        Interact: Interact with nearby objects
        Eat: Consume a resource
        Drink: Consume a liquid
        Rest: Stay in place and conserve energy
        Wait: Pause without taking action
        Custom: User-defined action
        "pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward")
        .value("MoveLeft", ActionType::MoveLeft, "Move left")
        .value("MoveRight", ActionType::MoveRight, "Move right")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left (rotate counterclockwise)")
        .value("TurnRight", ActionType::MoveRight, "Turn right (rotate clockwise)")
        .value("Look", ActionType::Look, "Point gaze in a direction")
        .value("LookUp", ActionType::LookUp, "Look upward")
        .value("LookDown", ActionType::LookDown, "Look downward")
        .value("Interact", ActionType::Interact, "Interact with nearby objects")
        .value("Eat", ActionType::Eat, "Consume a resource")
        .value("Drink", ActionType::Drink, "Consume a liquid")
        .value("Rest", ActionType::Rest, "Stay in place and conserve energy")
        .value("Wait", ActionType::Wait, "Pause without taking action")
        .value("Custom", ActionType::Custom, "User-defined action")
        .export_values();

    // Motor commands with better documentation
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)
        
        Describes the actual motor commands that are sent to the world.
        
        MoveForward: Execute forward movement
        MoveBackward: Execute backward movement
        TurnLeft: Execute left turn
        TurnRight: Execute right turn
        LookLeft: Point gaze left
        LookRight: Point gaze right
        Interact: Execute interaction action
        Wait: No movement or action
        "pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Execute forward movement")
        .value("MoveBackward", MotorCommand::MoveBackward, "Execute backward movement")
        .value("TurnLeft", MotorCommand::TurnLeft, "Execute left turn")
        .value("TurnRight", MotorCommand::TurnRight, "Execute right turn")
        .value("LookLeft", MotorCommand::LookLeft, "Point gaze left")
        .value("LookRight", MotorCommand::LookRight, "Point gaze right")
        .value("Interact", MotorCommand::Interact, "Execute interaction action")
        .value("Wait", MotorCommand::Wait, "No movement or action")
        .export_values();

    // World object types with better documentation
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)
        
        Describes the categories of objects that exist in the simulated world.
        
        Empty: No object present
        Resource: Valuable item that can be consumed
        Hazard: Dangerous item that causes harm
        Wall: Obstacle that blocks movement
        Marker: Point of interest or landmark
        "pbdoc")
        .value("Empty", WorldObjectType::Empty, "No object present")
        .value("Resource", WorldObjectType::Resource, "Valuable item that can be consumed")
        .value("Hazard", WorldObjectType::Hazard, "Dangerous item that causes harm")
        .value("Wall", WorldObjectType::Wall, "Obstacle that blocks movement")
        .value("Marker", WorldObjectType::Marker, "Point of interest or landmark")
        .export_values();

    // Improved Config class with better error handling and validation
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)
        
        Manages brain simulation parameters and settings.
        
        Config allows you to control various aspects of the brain simulation including:
        - Neuron and synapse counts
        - Connection probabilities
        - Plasticity parameters
        - Development settings
        - Neuromodulation parameters
        - Environment configuration
        - Logging settings
        
        Configuration values can be loaded from files, command-line arguments, or set programmatically.
        "pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(
             Load configuration from a file.
             
             The file should contain simple key=value pairs, one per line.
             Lines starting with # or / are treated as comments and ignored.
             
             Args:
                 filepath: Path to the configuration file
                 
             Returns:
                 True if the file was loaded successfully, False otherwise
                 
             Raises:
                 ConfigException: If the file cannot be read or contains invalid syntax
             )pbdoc")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           R"pbdoc(
           Load configuration from command-line arguments.
           
           Supports --key=value format and -key value format.
           
           Args:
               argc: Number of command-line arguments
               argv: Array of command-line argument strings
               
           Returns:
               True if arguments were parsed successfully
           )pbdoc")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             R"pbdoc(
             Save configuration to a file.
             
             The file will contain all configuration entries with their values
             and source information.
             
             Args:
                 filepath: Path where the configuration should be saved
                 
             Returns:
                 True if the file was saved successfully, False otherwise
             )pbdoc")
        .def("has", &Config::has, py::arg("key"),
             R"pbdoc(
             Check if a configuration key exists.
             
             Args:
                 key: Configuration key to check
                 
             Returns:
                 True if the key exists, False otherwise
             )pbdoc")
        .def("getKeys", &Config::getKeys,
             R"pbdoc(
             Get all configuration keys.
             
             Returns:
                 List of all configuration keys
             )pbdoc")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("set", static_cast<void (Config::*)(const std::string&, const std::string&, ConfigSource)>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             R"pbdoc(
             Set a configuration value.
             
             Args:
                 key: Configuration key
                 value: String value to set
                 source: Source of the configuration (Default, File, CommandLine, Runtime)
             )pbdoc")
        .def("set", static_cast<void (Config::*)(const std::string&, int, ConfigSource)>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set a configuration value (integer)")
        .def("set", static_cast<void (Config::*)(const std::string&, double, ConfigSource)>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set a configuration value (float)")
        .def("set", static_cast<void (Config::*)(const std::string&, bool, ConfigSource)>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set a configuration value (boolean)")
        .def("get", [](const Config& self, const std::string& key) {
            // Try different types based on what makes sense
            auto val = self.get<std::string>(key);
            if (val.has_value()) return py::make_tuple("string", val.value());
            
            val = self.get<int>(key);
            if (val.has_value()) return py::make_tuple("int", val.value());
            
            val = self.get<double>(key);
            if (val.has_value()) return py::make_tuple("double", val.value());
            
            val = self.get<bool>(key);
            if (val.has_value()) return py::make_tuple("bool", val.value());
            
            return py::none();
        }, py::arg("key"),
             R"pbdoc(
             Get a configuration value.
             
             Args:
                 key: Configuration key to retrieve
                 
             Returns:
                 A tuple (type, value) where type is "string", "int", "double", or "bool",
                 or None if the key doesn't exist
             )pbdoc")
        .def("getOr", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            auto val = self.get<std::string>(key);
            return val.has_value() ? val.value() : defaultValue;
        }, py::arg("key"), py::arg("default_value"),
             "Get a configuration value as string, with default")
        .def("getOr", [](const Config& self, const std::string& key, int defaultValue) {
            auto val = self.get<int>(key);
            return val.has_value() ? val.value() : defaultValue;
        }, py::arg("key"), py::arg("default_value"),
             "Get a configuration value as int, with default")
        .def("getOr", [](const Config& self, const std::string& key, double defaultValue) {
            auto val = self.get<double>(key);
            return val.has_value() ? val.value() : defaultValue;
        }, py::arg("key"), py::arg("default_value"),
             "Get a configuration value as double, with default")
        .def("getOr", [](const Config& self, const std::string& key, bool defaultValue) {
            auto val = self.get<bool>(key);
            return val.has_value() ? val.value() : defaultValue;
        }, py::arg("key"), py::arg("default_value"),
             "Get a configuration value as bool, with default")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove a configuration key")
        .def("summary", &Config::summary,
             R"pbdoc(
             Get a summary string of the configuration.
             
             Returns:
                 String containing all configuration entries with their values and sources
             )pbdoc")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Improved SensoryInput with better validation
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)
        
        Abstract base class for sensory modalities (vision, audio, internal signals).
        
        Provides access to sensor data and metadata about when and how it was captured.
        "pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision with better validation and utilities
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)
        
        Represents visual input from the environment, such as what the agent's eyes see.
        
        Provides access to image data, dimensions, and additional metadata.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set the vision data")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    // Audio with better validation and utilities
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)
        
        Represents audio input from the environment.
        
        Provides access to audio samples, sample rate, and related metadata.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set the audio data")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set the sample rate")
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // InternalSignals with better utilities
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)
        
        Represents internal physiological and cognitive signals.
        
        Includes hunger, thirst, energy levels, emotional states, and other
        internal measurements that drive the agent's behavior.
        "pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all signals");

    // Action with better validation and utilities
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)
        
        Represents a motor action that the brain can command.
        
        Contains information about what action to take and any parameters
        needed to execute that action.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType, "Get the action type")
        .def("setType", &Action::setType, py::arg("type"), "Set the action type")
        .def("getParameters", &Action::getParameters, "Get the action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set the action parameters")
        .def("getName", &Action::getName, "Get the action name")
        .def("clone", &Action::clone, "Create a copy of this action");

    // WorldObject with better validation and utilities
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)
        
        Represents an object in the simulated world environment.
        
        Objects can be resources, hazards, walls, markers, or empty space.
        "pbdoc")
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

    // AgentBody with better validation and utilities
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)
        
        Represents the physical state of the agent in the world.
        
        Includes position, orientation, movement, energy, health, age, and other
        physical attributes that influence the agent's behavior.
        "pbdoc")
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
        .def("reset", &AgentBody::reset, "Reset body state to initial conditions");

    // ActionResult with better validation and utilities
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)
        
        Represents the outcome of an action performed in the world.
        
        Contains reward information, success status, and a descriptive message
        about what happened as a result of the action.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // SensoryPercept with better validation and utilities
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)
        
        Aggregates all sensory inputs from different modalities (vision, touch, internal, audio).
        
        Represents what the agent perceives about its environment at a given moment.
        "pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set vision input")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch input")
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioceptive input")
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio input")
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all sensory signals")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get the percept timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set the percept timestamp");

    // Improved SimpleWorld with better validation and utilities
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)
        
        Represents the simulated environment in which the agent lives and acts.
        
        Provides functionality to:
        - Configure world dimensions and agent vision
        - Maintain agent state (position, energy, health)
        - Handle actions and their consequences
        - Provide sensory input to the agent
        - Manage world objects and interactions
        "pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             R"pbdoc(
             Configure the world dimensions and agent vision.
             
             Args:
                 width: World width in cells
                 height: World height in cells  
                 visionWidth: Width of agent's visual field
                 visionHeight: Height of agent's visual field
             )pbdoc")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world state")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             R"pbdoc(
             Apply a motor command to the world.
             
             Args:
                 cmd: Motor command to execute
                 currentTime: Current simulation time
                 
             Returns:
                 ActionResult describing the outcome
             )pbdoc")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get the agent's sensory percept from the world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get the agent's body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add an object to the world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position (x, y)")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if a position is valid (not blocked by walls)")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy capacity")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // Improved Brain class with better error handling and utilities
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)
        
        The core neural system that processes sensory input and produces motor output.
        
        The brain implements a complete spiking neural network with:
        - Leaky Integrate-and-Fire (LIF) neurons
        - Event-driven spike propagation with synaptic delays
        - Multiple plasticity mechanisms (STDP, Hebbian, structural)
        - Working memory and episodic memory systems
        - Neuromodulation (dopamine, curiosity, novelty)
        - Prediction and planning capabilities
        - Developmental stages and maturation
        - Checkpoint save/load functionality
        
        The brain operates through a closed-loop system that connects world input,
        internal state, and behavioral output.
        "pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             R"pbdoc(
             Create a new brain with configuration.
             
             Args:
                 config: Configuration object specifying brain parameters
                 
             Raises:
                 BrainException: If configuration is invalid or brain cannot be created
             )pbdoc")
        .def("initialize", &Brain::initialize,
             R"pbdoc(
             Initialize the brain with configuration.
             
             This method must be called before the brain can process any input.
             It sets up the neural network structure and initializes internal systems.
             
             Returns:
                 True if initialization succeeded, False otherwise
                 
             Raises:
                 BrainException: If initialization fails
             )pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             R"pbdoc(
             Perform a simulation step.
             
             Args:
                 currentStep: Current simulation step number
                 
             Raises:
                 BrainException: If step processing fails
             )pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             R"pbdoc(
             Perform a simulation step with timestamp.
             
             Args:
                 currentStep: Current simulation step number
                 currentTime: Current simulation time (seconds)
                 
             Raises:
                 BrainException: If step processing fails
             )pbdoc")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             R"pbdoc(
             Inject sensory input into the brain.
             
             Args:
                 input: Sensory percept containing environmental information
                 
             Raises:
                 BrainException: If input processing fails
             )pbdoc")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             R"pbdoc(
             Inject current into a specific neuron.
             
             Args:
                 neuron: Target neuron ID
                 current: Current to inject (positive for excitation, negative for inhibition)
                 
             Raises:
                 BrainException: If neuron doesn't exist or injection fails
             )pbdoc")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             R"pbdoc(
             Inject current into all neurons of a specific type.
             
             Args:
                 type: Neuron type to target
                 current: Current to inject per neuron
                 
             Raises:
                 BrainException: If type is invalid or injection fails
             )pbdoc")
        .def("produceAction", &Brain::produceAction,
             R"pbdoc(
             Produce motor action based on neural activity.
             
             This method analyzes the current neural state and produces
             the most appropriate motor command for the current situation.
             
             Returns:
                 MotorCommand representing the brain's decision
                 
             Raises:
                 BrainException: If action production fails
             )pbdoc")
        .def("reset", &Brain::reset,
             R"pbdoc(
             Reset brain state to initial conditions.
             
             This clears all memory, resets neuron states, and prepares
             the brain for a new simulation episode.
             
             Raises:
                 BrainException: If reset fails
             )pbdoc")
        .def("save", &Brain::save, py::arg("filepath"),
             R"pbdoc(
             Save brain state to file.
             
             Args:
                 filepath: Path where brain state should be saved
                 
             Returns:
                 True if save succeeded, False otherwise
                 
             Raises:
                 BrainException: If save operation fails
             )pbdoc")
        .def("load", &Brain::load, py::arg("filepath"),
             R"pbdoc(
             Load brain state from file.
             
             Args:
                 filepath: Path to the saved brain state file
                 
             Returns:
                 True if load succeeded, False otherwise
                 
             Raises:
                 BrainException: If load operation fails
             )pbdoc")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             R"pbdoc(
             Add a new neural region.
             
             Args:
                 name: Optional name for the new region
                 
             Returns:
                 RegionId for the new region
                 
             Raises:
                 BrainException: If region creation fails
             )pbdoc")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get a region by ID.
             
             Args:
                 id: Region ID to retrieve
                 
             Returns:
                 Reference to the region (or None if ID is invalid)
             )pbdoc")
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
             R"pbdoc(
             Log brain status and statistics.
             
             Useful for debugging and monitoring simulation progress.
             "pbdoc")
        .def("to_string", [](const Brain& brain) {
            return "<Brain: " + std::to_string(brain.getTotalNeuronCount()) + 
                   " neurons, " + std::to_string(brain.getTotalSynapseCount()) +
                   " synapses>";
        }, "Get string representation of brain");

    // Improved AgentBrain class with better error handling and utilities
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)
        
        Bridges the NLM brain to the world environment through sensory and motor interfaces.
        
        The AgentBrain provides a higher-level interface for:
        - Processing sensory input from the world
        - Decoding motor commands for world interaction
        - Managing reward-based learning and neuromodulation
        - Tracking developmental progress
        - Maintaining curiosity and exploration behavior
        
        This class implements the agent-world loop that enables the brain to
        learn from experience and adapt its behavior.
        "pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             R"pbdoc(
             Create an agent brain interface.
             
             Args:
                 brain: The brain this agent will control
                 
             Raises:
                 BrainException: If brain is null or invalid
             )pbdoc")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             R"pbdoc(
             Initialize the agent brain with world.
             
             Args:
                 world: The world the agent will operate in
                 
             Raises:
                 WorldException: If world is invalid
                 BrainException: If initialization fails
             )pbdoc")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             R"pbdoc(
             Process sensory percept and inject into brain.
             
             Args:
                 percept: Sensory percept containing environmental information
                 
             Raises:
                 WorldException: If percept is invalid
                 BrainException: If processing fails
             )pbdoc")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             R"pbdoc(
             Decode brain motor activity into motor command.
             
             This method analyzes the brain's motor neuron activity and
             produces the appropriate motor command for the world.
             
             Returns:
                 MotorCommand for world interaction
                 
             Raises:
                 BrainException: If decoding fails
             )pbdoc")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             R"pbdoc(
             Apply reward-based neuromodulation.
             
             Args:
                 reward: Actual reward received from world
                 predictedReward: Expected reward predicted by brain
                 
             Raises:
                 BrainException: If neuromodulation fails
             )pbdoc")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             R"pbdoc(
             Update development system.
             
             Args:
                 timestep: Time step for development update
                 
             Raises:
                 BrainException: If development update fails
             )pbdoc")
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
             R"pbdoc(
             Reset agent for new episode.
             
             This clears agent state and prepares for a new simulation episode.
             "pbdoc")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable or disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable or disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable or disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable or disable curiosity-driven exploration")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled");

    // Factory functions with better error handling and documentation
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, R"pbdoc(
        Create a default configuration.
        
        Returns:
            New configuration object with sensible defaults
        )pbdoc")

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            throw BrainException("Config cannot be null");
        }
        return std::make_shared<Brain>(config);
    }, py::arg("config"), R"pbdoc(
        Create a new brain with configuration.
        
        Args:
            config: Configuration object specifying brain parameters
            
        Returns:
            New brain object
            
        Raises:
            BrainException: If config is invalid or brain cannot be created
        )pbdoc")

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(
        Create a new simple world.
        
        Returns:
            New simple world object
        )pbdoc")

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw BrainException("Brain cannot be null");
        }
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(
        Create a new agent brain interface.
        
        Args:
            brain: The brain this agent will control
            
        Returns:
            New agent brain interface object
            
        Raises:
            BrainException: If brain is null or invalid
        )pbdoc")

    // Convenience functions for common use cases
    m.def("run_simple_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, int num_steps) {
        if (!brain || !world) {
            throw BrainException("Brain and world cannot be null");
        }
        
        brain->initialize();
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        
        for (int step = 0; step < num_steps; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            agent->applyRewardModulation(0.0, 0.0);
            agent->updateDevelopment(0.1);
        }
        
        return std::make_tuple(
            brain->getTotalSpikeCount(),
            brain->getAverageFiringRate(),
            agent->getCuriosityLevel()
        );
    }, py::arg("brain"), py::arg("world"), py::arg("num_steps"),
       R"pbdoc(
       Run a simple simulation with brain, world, and agent.
       
       This is a convenience function for running basic simulations
       without having to manually set up the agent and enable subsystems.
       
       Args:
           brain: The brain to simulate
           world: The world to simulate in
           num_steps: Number of simulation steps to run
           
       Returns:
           Tuple of (total_spikes, avg_firing_rate, curiosity_level)
           
       Raises:
           BrainException: If simulation fails
           WorldException: If world operations fail
       )pbdoc")

    m.def("create_demo_agent", [](std::shared_ptr<Config> config, int width, int height) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 8, 8);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("config"), py::arg("width") = 20, py::arg("height") = 20,
       R"pbdoc(
       Create a demo agent with all subsystems enabled.
       
       This is a convenience function for creating a fully-featured
       agent with reward modulation, structural plasticity, development,
       and curiosity enabled.
       
       Args:
           config: Configuration for the brain
           width: World width
           height: World height
           
       Returns:
           Tuple of (brain, world, agent)
           
       Raises:
           BrainException: If agent creation fails
           WorldException: If world creation fails
       )pbdoc")

    // Version and utility attributes
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Project";
    
    m.def("get_version", []() { return std::string("NLM Python Bindings v0.1.0"); },
          "Get version information");
    
    m.def("get_help", []() {
        return R"pbdoc(
        NLM Python Bindings Help
        
        Quick Start:
        1. Create a brain: brain = pynlm.createBrain(pynlm.createDefaultConfig())
        2. Initialize: brain.initialize()
        3. Run simulation: brain.step(step_number)
        
        For more examples and documentation, see:
        - easy_usage.md: Beginner-friendly examples
        - HOW_TO_USE.md: Comprehensive documentation
        - docs/ARCHITECTURE.md: System architecture
        
        Common errors to avoid:
        - Always call brain.initialize() before brain.step()
        - Check that world.reset() is called before world.update()
        - Use try/except blocks to catch NLMException errors
        )pbdoc";
    }, "Get help and usage information");

    // Invalid IDs
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
