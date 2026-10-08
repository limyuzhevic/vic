#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <iostream>

// Error handling for Python bindings
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

#define PYBIND_NLM_ERROR_CHECK(expr, msg) \
    do { \
        if (!(expr)) { \
            throw NLMError(msg); \
        } \
    } while(0)

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
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        
        This module provides access to all NLM systems:
        - Core: Brain simulation with spiking neurons
        - Memory: Working, episodic, and associative memory
        - Plasticity: STDP, Hebbian, and structural learning
        - Neuromodulation: Dopamine, curiosity, novelty detection
        - Prediction: Forward models and prediction error
        - Cognition: Planning, concept formation, attention
        - Development: Maturation and stage-based plasticity
        - Environment: Simple world for agent interaction
        - Sensorimotor: Perception and action systems
        
        Features:
        - Type-safe configuration management
        - Comprehensive error handling
        - Convenience functions for common patterns
        - Advanced APIs for power users
        - Full integration with all NLM subsystems
    )pbdoc";

    // Register custom exception for NLM errors
    py::register_exception<NLMError>(m, "NLMError");

    // =============================================================================
    // CORE TYPES AND ENUMERATIONS
    // =============================================================================

    // Neuron Identifiers
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index, R"pbdoc(Get the neuron index)pbdoc")
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index, R"pbdoc(Get the synapse index)pbdoc")
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
        .def("index", &RegionId::index, R"pbdoc(Get the region index)pbdoc")
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
        .def("index", &PopulationId::index, R"pbdoc(Get the population index)pbdoc")
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // System enumerations
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Excitatory neurons")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neurons")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neurons")
        .value("Sensory", NeuronType::Sensory, "Sensory neurons")
        .value("Motor", NeuronType::Motor, "Motor neurons")
        .value("Internal", NeuronType::Internal, "Internal neurons")
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory synapses")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synapses")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synapses")
        .value("Electrical", SynapseType::Electrical, "Electrical synapses")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction synapses")
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period stage")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation stage")
        .value("Adult", DevelopmentalStage::Adult, "Adult stage")
        .value("Aging", DevelopmentalStage::Aging, "Aging stage")
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting, "Resting state")
        .value("Active", FiringState::Active, "Active firing state")
        .value("Refractory", FiringState::Refractory, "Refractory period")
        .value("Inhibited", FiringState::Inhibited, "Inhibited state")
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
        .value("MoveForward", MotorCommand::MoveForward, "Move forward")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right")
        .value("LookLeft", MotorCommand::LookLeft, "Look left")
        .value("LookRight", MotorCommand::LookRight, "Look right")
        .value("Interact", MotorCommand::Interact, "Interact")
        .value("Wait", MotorCommand::Wait, "Wait")
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty world cell")
        .value("Resource", WorldObjectType::Resource, "Resource object")
        .value("Hazard", WorldObjectType::Hazard, "Hazard object")
        .value("Wall", WorldObjectType::Wall, "Wall object")
        .value("Marker", WorldObjectType::Marker, "Marker object")
        .export_values();

    // Invalid identifiers (for convenience)
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // =============================================================================
    // CONFIGURATION SYSTEM
    // =============================================================================

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)

        Configuration management for NLM with type-safe value access.
        Supports various value types including primitive types,
        strings, and vector types.
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(Load configuration from a JSON file)

             Args:
                 filepath: Path to the configuration file

             Returns:
                 True if configuration was loaded successfully, False otherwise
             )pbdoc")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            bool success = self.loadFromArgs(argc, argv);
            if (!success) {
                throw NLMError("Failed to load configuration from command line arguments");
            }
            return success;
        }, py::arg("argc"), py::arg("argv"),
            R"pbdoc(Load configuration from command line arguments)

            Args:
                argc: Number of command line arguments
                argv: Array of command line argument strings

            Returns:
                True if configuration was loaded successfully

            Raises:
                NLMError: If configuration loading failed
            )pbdoc")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             R"pbdoc(Save configuration to a JSON file)

             Args:
                 filepath: Path where to save the configuration

             Returns:
                 True if configuration was saved successfully, False otherwise
             )pbdoc")
        .def("has", &Config::has, py::arg("key"),
             R"pbdoc(Check if a configuration key exists)

             Args:
                 key: Configuration key to check

             Returns:
                 True if key exists, False otherwise
             )pbdoc")
        .def("getKeys", &Config::getKeys,
             R"pbdoc(Get all configuration keys)

             Returns:
                 List of all configuration keys
             )pbdoc")
        .def("clear", &Config::clear,
             R"pbdoc(Clear all configuration entries)")
        .def("summary", &Config::summary,
             R"pbdoc(Get a summary string of the configuration)

             Returns:
                 Configuration summary string
             )pbdoc")
        // Type-safe get methods with error checking
        .def("getInt", [](const Config& self, const std::string& key) -> int {
            auto value = self.get<int>(key);
            if (!value.has_value()) {
                throw NLMError("Configuration key not found: " + key);
            }
            return *value;
        }, py::arg("key"),
             R"pbdoc(Get an integer configuration value)

             Args:
                 key: Configuration key

             Returns:
                 Integer value

             Raises:
                 NLMError: If key not found or wrong type
             )pbdoc")
        .def("getDouble", [](const Config& self, const std::string& key) -> double {
            auto value = self.get<double>(key);
            if (!value.has_value()) {
                throw NLMError("Configuration key not found: " + key);
            }
            return *value;
        }, py::arg("key"),
             R"pbdoc(Get a double configuration value)

             Args:
                 key: Configuration key

             Returns:
                 Double value

             Raises:
                 NLMError: If key not found or wrong type
             )pbdoc")
        .def("getBool", [](const Config& self, const std::string& key) -> bool {
            auto value = self.get<bool>(key);
            if (!value.has_value()) {
                throw NLMError("Configuration key not found: " + key);
            }
            return *value;
        }, py::arg("key"),
             R"pbdoc(Get a boolean configuration value)

             Args:
                 key: Configuration key

             Returns:
                 Boolean value

             Raises:
                 NLMError: If key not found or wrong type
             )pbdoc")
        .def("getString", [](const Config& self, const std::string& key) -> std::string {
            auto value = self.get<std::string>(key);
            if (!value.has_value()) {
                throw NLMError("Configuration key not found: " + key);
            }
            return *value;
        }, py::arg("key"),
             R"pbdoc(Get a string configuration value)

             Args:
                 key: Configuration key

             Returns:
                 String value

             Raises:
                 NLMError: If key not found or wrong type
             )pbdoc")
        .def("getOrInt", [](const Config& self, const std::string& key, int defaultValue) -> int {
            return self.getOr<int>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
             R"pbdoc(Get an integer configuration value or return default)

             Args:
                 key: Configuration key
                 defaultValue: Value to return if key not found

             Returns:
                 Integer value or default value
             )pbdoc")
        .def("getOrDouble", [](const Config& self, const std::string& key, double defaultValue) -> double {
            return self.getOr<double>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
             R"pbdoc(Get a double configuration value or return default)

             Args:
                 key: Configuration key
                 defaultValue: Value to return if key not found

             Returns:
                 Double value or default value
             )pbdoc")
        .def("getOrBool", [](const Config& self, const std::string& key, bool defaultValue) -> bool {
            return self.getOr<bool>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
             R"pbdoc(Get a boolean configuration value or return default)

             Args:
                 key: Configuration key
                 defaultValue: Value to return if key not found

             Returns:
                 Boolean value or default value
             )pbdoc")
        .def("getOrString", [](const Config& self, const std::string& key, const std::string& defaultValue) -> std::string {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
             R"pbdoc(Get a string configuration value or return default)

             Args:
                 key: Configuration key
                 defaultValue: Value to return if key not found

             Returns:
                 String value or default value
             )pbdoc")
        .def("setInt", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             R"pbdoc(Set an integer configuration value)

             Args:
                 key: Configuration key
                 value: Integer value
             )pbdoc")
        .def("setDouble", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             R"pbdoc(Set a double configuration value)

             Args:
                 key: Configuration key
                 value: Double value
             )pbdoc")
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             R"pbdoc(Set a boolean configuration value)

             Args:
                 key: Configuration key
                 value: Boolean value
             )pbdoc")
        .def("setString", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             R"pbdoc(Set a string configuration value)

             Args:
                 key: Configuration key
                 value: String value
             )pbdoc")
        .def("remove", &Config::remove, py::arg("key"),
             R"pbdoc(Remove a configuration key)

             Args:
                 key: Configuration key to remove
             )pbdoc")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // =============================================================================
    // SENSORY SYSTEMS
    // =============================================================================

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)

        Abstract base class representing sensory input to the brain.
        Contains methods for accessing sensor data and metadata.
    )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)

        Vision-based sensory input representing a visual scene as grayscale values.
        Used for visual perception by the agent.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3,
             R"pbdoc(Construct a Vision object)

             Args:
                 width: Vision width in pixels
                 height: Vision height in pixels
                 channels: Number of color channels (default: 3)
             )pbdoc")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), R"pbdoc(Set vision data)

             Args:
                 data: Vision data as vector of floats
             )pbdoc")
        .def("getWidth", &Vision::getWidth, "Get vision width")
        .def("getHeight", &Vision::getHeight, "Get vision height")
        .def("getChannels", &Vision::getChannels, "Get number of channels");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)

        Audio-based sensory input representing sound signals.
        Used for auditory perception by the agent.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             R"pbdoc(Construct an Audio object)

             Args:
                 sampleRate: Audio sample rate
                 numSamples: Number of audio samples
             )pbdoc")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), R"pbdoc(Set audio data)

             Args:
                 data: Audio data as vector of floats
             )pbdoc")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set audio sample rate")
        .def("getSampleRate", &Audio::getSampleRate, "Get audio sample rate")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of audio samples");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)

        Internal signals representing homeostatic and metabolic state.
        Used for internal state perception by the agent.
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             R"pbdoc(Add a signal value)

             Args:
                 value: Signal value to add
             )pbdoc")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all signals");

    // =============================================================================
    // ACTION AND BEHAVIOR
    // =============================================================================

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)

        Represents an action that an agent can perform in the environment.
        Contains action type and parameters for fine-grained control.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             R"pbdoc(Construct an Action with type)

             Args:
                 type: Action type
             )pbdoc")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             R"pbdoc(Construct an Action with type and parameters)

             Args:
                 type: Action type
                 parameters: Action parameters as vector of floats
             )pbdoc")
        .def("getType", &Action::getType, "Get action type")
        .def("setType", &Action::setType, py::arg("type"), "Set action type")
        .def("getParameters", &Action::getParameters, "Get action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("getName", &Action::getName, "Get action name")
        .def("clone", &Action::clone, "Create a copy of this action");

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)

        Represents an object in the simulated world.
        Contains position, type, and value information.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             R"pbdoc(Construct a WorldObject)

             Args:
                 x: X coordinate
                 y: Y coordinate
                 type: Object type
                 value: Object value
                 radius: Object radius (default: 0.5)
             )pbdoc")
        .def_readwrite("x", &WorldObject::x, "X coordinate")
        .def_readwrite("y", &WorldObject::y, "Y coordinate")
        .def_readwrite("radius", &WorldObject::radius, "Object radius")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Object value")
        .def_readwrite("active", &WorldObject::active, "Whether object is active");

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)

        Represents the physical state of an agent in the world.
        Contains position, velocity, orientation, and physiological state.
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position")
        .def_readwrite("y", &AgentBody::y, "Y position")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation angle")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Energy level")
        .def_readwrite("health", &AgentBody::health, "Health level")
        .def_readwrite("age", &AgentBody::age, "Age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time of last action")
        .def("reset", &AgentBody::reset, "Reset body state to initial values");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)

        Represents the outcome of an agent's action in the environment.
        Contains reward, success status, and message information.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             R"pbdoc(Construct an ActionResult)

             Args:
                 reward: Reward value
                 success: Whether action was successful
                 message: Optional message
             )pbdoc")
        .def_readwrite("reward", &ActionResult::reward, "Reward value")
        .def_readwrite("success", &ActionResult::success, "Success status")
        .def_readwrite("message", &ActionResult::message, "Result message");

    // =============================================================================
    // PERCEPTION SYSTEMS
    // =============================================================================

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)

        Sensory percept data containing all modalities perceived by the agent.
        Combines vision, touch, internal state, proprioception, and audio signals.
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get vision data")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set vision data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision height")
        .def("getTouch", &SensoryPercept::getTouch, "Get touch data")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch data")
        .def("getInternal", &SensoryPercept::getInternal, "Get internal signals")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception, "Get proprioception data")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioception data")
        .def("getAudio", &SensoryPercept::getAudio, "Get audio data")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio data")
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all signals concatenated")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set timestamp");

    // =============================================================================
    // WORLD SYSTEM
    // =============================================================================

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)

        A simple 2D simulated world where agents interact with objects and environment.
        Provides sensory input, motor output, and state management.
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             R"pbdoc(Configure the world dimensions)

             Args:
                 width: World width
                 height: World height
                 visionWidth: Vision sensor width
                 visionHeight: Vision sensor height
             )pbdoc")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             R"pbdoc(Set agent starting position)

             Args:
                 x: Starting X position
                 y: Starting Y position
             )pbdoc")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             R"pbdoc(Update world state)

             Args:
                 timestep: Time step duration
             )pbdoc")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             R"pbdoc(Apply a motor command to the world)

             Args:
                 cmd: Motor command to apply
                 currentTime: Current simulation time
             )pbdoc")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get current sensory percept)

             Returns:
                 Current sensory percept
             )pbdoc")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get current agent body state)

             Returns:
                 Current agent body state
             )pbdoc")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             R"pbdoc(Add an object to the world)

             Args:
                 obj: World object to add
             )pbdoc")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             R"pbdoc(Remove an object from the world)

             Args:
                 x: X coordinate
                 y: Y coordinate
             )pbdoc")
        .def("isValidPosition", &SimpleWorld::isValidPosition,
             py::arg("x"), py::arg("y"),
             R"pbdoc(Check if a position is valid)

             Args:
                 x: X coordinate
                 y: Y coordinate

             Returns:
                 True if position is valid, False otherwise
             )pbdoc")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed");

    // =============================================================================
    // BRAIN CORE SYSTEM
    // =============================================================================

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)

        The central neural simulation brain implementing spiking neural computation
        with integrated memory, prediction, cognition, and neuromodulation systems.
        This is the core class for NLM neural simulation.
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             R"pbdoc(Construct a Brain with configuration)

             Args:
                 config: Brain configuration
             )pbdoc")
        .def("initialize", &Brain::initialize,
             R"pbdoc(Initialize the brain with configuration)

             Returns:
                 True if initialization succeeded, False otherwise
             )pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             R"pbdoc(Perform a simulation step)

             Args:
                 currentStep: Current simulation step number
             )pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             R"pbdoc(Perform a simulation step with timestamp)

             Args:
                 currentStep: Current simulation step number
                 currentTime: Current simulation time
             )pbdoc")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"),
             R"pbdoc(Inject sensory input into the brain)

             Args:
                 input: Sensory input to inject
             )pbdoc")
        .def("injectCurrent", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"),
             R"pbdoc(Inject current directly into a specific neuron)

             Args:
                 neuron: Target neuron ID
                 current: Current to inject
             )pbdoc")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             R"pbdoc(Inject current into all neurons of a specific type)

             Args:
                 type: Neuron type to target
                 current: Current to inject
             )pbdoc")
        .def("produceAction", &Brain::produceAction,
             R"pbdoc(Produce motor action based on neural activity)

             Returns:
                 Produced action
             )pbdoc")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             R"pbdoc(Save brain state to file)

             Args:
                 filepath: Path to save file

             Returns:
                 True if save succeeded, False otherwise
             )pbdoc")
        .def("load", &Brain::load, py::arg("filepath"),
             R"pbdoc(Load brain state from file)

             Args:
                 filepath: Path to load file

             Returns:
                 True if load succeeded, False otherwise
             )pbdoc")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             R"pbdoc(Add a new neural region)

             Args:
                 name: Region name (optional)

             Returns:
                 Region ID
             )pbdoc")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             R"pbdoc(Get a region by ID)

             Args:
                 id: Region ID

             Returns:
                 Neural region pointer
             )pbdoc")
        .def("getRegionCount", &Brain::getRegionCount, "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds, "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal, "Get all regions")
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
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount, "Get total spike count")
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount, "Get pending spike event count")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount, "Get active neuron count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus, "Log brain status")
        // Access to brain subsystems
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get the spike system")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get the STDP plasticity system")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get the Hebbian plasticity system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get the structural plasticity system")
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get the working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get the episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get the associative memory system")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get the prediction system")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get the neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get the concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get the attentional selection system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get the development system")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get the dopamine neuromodulator")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get the curiosity neuromodulator")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get the novelty neuromodulator")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get the prediction error signal")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get the random generator")
        .def("updatePlasticity", &Brain::updatePlasticity,
             "Update plasticity rules (called automatically in step)")
        .def("develop", &Brain::develop,
             "Apply developmental changes (called automatically in step)")
        .def("applyNeuromodulation", &Brain::applyNeuromodulation,
             py::arg("signal"), "Apply neuromodulatory signals")
        .def("addInterRegionConnection", &Brain::addInterRegionConnection,
             py::arg("source"), py::arg("target"), py::arg("weight") = 0.0f, py::arg("delay") = 1,
             "Add an inter-region connection")
        .def("removeInterRegionConnection", &Brain::removeInterRegionConnection,
             py::arg("source"), py::arg("target"),
             "Remove an inter-region connection")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount, "Get total neuron count")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount, "Get total synapse count")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount, "Get firing neuron count")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate, "Get average firing rate");

    // =============================================================================
    // AGENT BRAIN INTERFACE
    // =============================================================================

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)

        Agent brain interface that connects the NLM brain to the simulated world.
        Handles sensory transduction and motor decoding for agent-environment interaction.
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             R"pbdoc(Construct an AgentBrain with brain)

             Args:
                 brain: Underlying brain instance
             )pbdoc")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             R"pbdoc(Initialize with world)

             Args:
                 world: World instance to connect to
             )pbdoc")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             R"pbdoc(Get expected sensory input size)

             Returns:
                 Expected sensory input size
             )pbdoc")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             R"pbdoc(Get expected motor output size)

             Returns:
                 Expected motor output size
             )pbdoc")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), R"pbdoc(Process sensory percept and inject into brain)

             Args:
                 percept: Sensory percept to process
             )pbdoc")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             R"pbdoc(Decode brain motor activity into motor command)

             Returns:
                 Decoded motor command
             )pbdoc")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             R"pbdoc(Apply reward-based neuromodulation)

             Args:
                 reward: Current reward signal
                 predictedReward: Predicted reward signal
             )pbdoc")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"), R"pbdoc(Update development system)

             Args:
                 timestep: Development timestep
             )pbdoc")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             R"pbdoc(Get current developmental stage)

             Returns:
                 Current developmental stage
             )pbdoc")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             R"pbdoc(Get current neuromodulation level)

             Returns:
                 Current neuromodulation level
             )pbdoc")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             R"pbdoc(Get curiosity level)

             Returns:
                 Curiosity level
             )pbdoc")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             R"pbdoc(Get novelty level)

             Returns:
                 Novelty level
             )pbdoc")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             R"pbdoc(Get prediction error)

             Returns:
                 Prediction error
             )pbdoc")
        .def("reset", &AgentBrain::reset,
             R"pbdoc(Reset agent for new episode)")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get the underlying brain)

             Returns:
                 Underlying brain instance
             )pbdoc")
        // Configuration getters (previously missing!)
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             R"pbdoc(Check if reward modulation is enabled)

             Returns:
                 True if reward modulation is enabled
             )pbdoc")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             R"pbdoc(Check if structural plasticity is enabled)

             Returns:
                 True if structural plasticity is enabled
             )pbdoc")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             R"pbdoc(Check if development is enabled)

             Returns:
                 True if development is enabled
             )pbdoc")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             R"pbdoc(Check if curiosity is enabled)

             Returns:
                 True if curiosity is enabled
             )pbdoc")
        // Configuration setters
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), R"pbdoc(Enable/disable reward modulation)

             Args:
                 enable: Whether to enable reward modulation
             )pbdoc")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), R"pbdoc(Enable/disable structural plasticity)

             Args:
                 enable: Whether to enable structural plasticity
             )pbdoc")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), R"pbdoc(Enable/disable development)

             Args:
                 enable: Whether to enable development
             )pbdoc")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), R"pbdoc(Enable/disable curiosity)

             Args:
                 enable: Whether to enable curiosity
             )pbdoc")

    // =============================================================================
    // FACTORY FUNCTIONS AND UTILITIES
    // =============================================================================

    // Configuration factory
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, R"pbdoc(Create a default configuration)

        Creates a new configuration with default settings.

        Returns:
            Default configuration instance
        )pbdoc");

    // Brain factory
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), R"pbdoc(Create a new brain with configuration)

        Args:
            config: Brain configuration

        Returns:
            Brain instance
        )pbdoc");

    // World factory
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(Create a new simple world)

        Creates a new simple 2D world for NLM simulation.

        Returns:
            SimpleWorld instance
        )pbdoc");

    // Agent brain factory
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(Create a new agent brain interface)

        Args:
            brain: Underlying brain instance

        Returns:
            AgentBrain instance
        )pbdoc");

    // Version information
    m.attr("__version__") = "2.0.0";
    m.attr("__author__") = "NLM Project Team";
    m.attr("__description__") = "Comprehensive Python bindings for NLM neural simulation framework";
}

} // namespace nlm
