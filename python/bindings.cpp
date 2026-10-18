#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <stdexcept>

// Include NLM headers
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

// Forward declarations for forward references in docstrings
class Brain;
class Config;
class AgentBrain;
class SimpleWorld;
class SensoryInput;
class Vision;
class Audio;
class InternalSignals;
class Action;
class WorldObject;
class AgentBody;
class ActionResult;
class SensoryPercept;

// Exception classes for better error handling
class PyNLMError : public std::runtime_error {
public:
    explicit PyNLMError(const std::string& message) : std::runtime_error(message) {}
};

class ValidationError : public PyNLMError {
public:
    explicit ValidationError(const std::string& message) : PyNLMError("Validation Error: " + message) {}
};

class ConfigurationError : public PyNLMError {
public:
    explicit ConfigurationError(const std::string& message) : PyNLMError("Configuration Error: " + message) {}
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A high-performance Python interface to the NLM C++ neural simulation framework.
        
        This module provides access to:
        - Brain simulation with spiking neural networks
        - Agent-world interaction and learning
        - Sensory processing (vision, audio, internal signals)
        - Motor control and action execution
        - Memory systems and neuromodulation
        - Developmental stages and plasticity
        
        Features:
        - Pythonic API with context managers and properties
        - Comprehensive error handling with specific exception types
        - Optional NumPy integration for efficient data handling
        - Extensive documentation and examples
        - Checkpoint/restore functionality
        
        Quick start:
        ```python
        import pynlm
        
        # Create a simple brain
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Run simulation
        for step in range(100):
            brain.step(step)
            
        print(f"Total spikes: {brain.getTotalSpikeCount()}")
        ```
    )pbdoc";

    // Type aliases for better documentation
    using VectorFloat = std::vector<float>;
    using VectorDouble = std::vector<double>;
    using VectorString = std::vector<std::string>;

    // Exception registration
    py::register_exception<PyNLMError>(m, "NLMError");
    py::register_exception<ValidationError>(m, "ValidationError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");

    // Configuration class with enhanced Pythonic features
    py::class_<Config>(m, "Config")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file.\n\n"
             "Args:\n"
             "    filepath: Path to the JSON configuration file\n\n"
             "Returns:\n"
             "    bool: True if successful, False otherwise")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments.\n\n"
            "Args:\n"
            "    argc: Number of command line arguments\n"
            "    argv: Command line arguments array\n\n"
            "Returns:\n"
            "    bool: True if successful, False otherwise")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file.\n\n"
             "Args:\n"
             "    filepath: Path where to save the configuration\n\n"
             "Returns:\n"
             "    bool: True if successful, False otherwise")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists.\n\n"
             "Args:\n"
             "    key: Configuration key to check\n\n"
             "Returns:\n"
             "    bool: True if the key exists, False otherwise")
        .def("get", &Config::template get<int>, py::arg("key"),
             "Get an integer configuration value.\n\n"
             "Args:\n"
             "    key: Configuration key\n\n"
             "Returns:\n"
             "    int or None: The value if found, None otherwise")
        .def("get", &Config::template get<double>, py::arg("key"),
             "Get a float/double configuration value.\n\n"
             "Args:\n"
             "    key: Configuration key\n\n"
             "Returns:\n"
             "    float or None: The value if found, None otherwise")
        .def("get", &Config::template get<bool>, py::arg("key"),
             "Get a boolean configuration value.\n\n"
             "Args:\n"
             "    key: Configuration key\n\n"
             "Returns:\n"
             "    bool or None: The value if found, None otherwise")
        .def("get", &Config::template get<std::string>, py::arg("key"),
             "Get a string configuration value.\n\n"
             "Args:\n"
             "    key: Configuration key\n\n"
             "Returns:\n"
             "    str or None: The value if found, None otherwise")
        .def("getOr", &Config::template getOr<int>, py::arg("key"), py::arg("defaultValue"),
             "Get a configuration value with a default.\n\n"
             "Args:\n"
             "    key: Configuration key\n"
             "    defaultValue: Default value if key not found\n\n"
             "Returns:\n"
             "    The value or default")
        .def("set", &Config::set, py::arg("key"), py::arg("value"),
             "Set a configuration value.\n\n"
             "Args:\n"
             "    key: Configuration key\n"
             "    value: Configuration value (int, float, bool, str, or list)")
        .def("setInt", &Config::set, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set an integer configuration value.\n\n"
             "Args:\n"
             "    key: Configuration key\n"
             "    value: Integer value\n"
             "    source: Configuration source (optional)")
        .def("setFloat", &Config::set, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set a float configuration value.\n\n"
             "Args:\n"
             "    key: Configuration key\n"
             "    value: Float value\n"
             "    source: Configuration source (optional)")
        .def("setBool", &Config::set, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set a boolean configuration value.\n\n"
             "Args:\n"
             "    key: Configuration key\n"
             "    value: Boolean value\n"
             "    source: Configuration source (optional)")
        .def("setString", &Config::set, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set a string configuration value.\n\n"
             "Args:\n"
             "    key: Configuration key\n"
             "    value: String value\n"
             "    source: Configuration source (optional)")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove a configuration key.\n\n"
             "Args:\n"
             "    key: Configuration key to remove")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys.\n\n"
             "Returns:\n"
             "    list: List of all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries.")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration.\n\n"
             "Returns:\n"
             "    str: Configuration summary")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Sensory Input classes
    py::class_<SensoryInput>(m, "SensoryInput")
        .def("getType", &SensoryInput::getType,
             "Get the type of sensory input.\n\n"
             "Returns:\n"
             "    NeuronType: Type of sensory input")
        .def("getData", &SensoryInput::getData,
             "Get the raw data as a vector.\n\n"
             "Returns:\n"
             "    list[float]: Raw sensory data")
        .def("getDimensions", &SensoryInput::getDimensions,
             "Get the dimensionality of the sensory data.\n\n"
             "Returns:\n"
             "    int: Number of dimensions")
        .def("getTimestamp", &SensoryInput::getTimestamp,
             "Get the timestamp of the sensory input.\n\n"
             "Returns:\n"
             "    float: Timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp of the sensory input.\n\n"
             "Args:\n"
             "    timestamp: Timestamp value");

    // Vision sensory input
    py::class_<Vision, SensoryInput>(m, "Vision")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3,
             "Initialize a Vision sensory input.\n\n"
             "Args:\n"
             "    width: Image width in pixels\n"
             "    height: Image height in pixels\n"
             "    channels: Number of color channels (default: 3 for RGB)")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             "Set the vision data.\n\n"
             "Args:\n"
             "    data: 2D array of RGB values flattened to 1D (width * height * channels)")
        .def("getWidth", &Vision::getWidth,
             "Get the image width.\n\n"
             "Returns:\n"
             "    int: Image width in pixels")
        .def("getHeight", &Vision::getHeight,
             "Get the image height.\n\n"
             "Returns:\n"
             "    int: Image height in pixels")
        .def("getChannels", &Vision::getChannels,
             "Get the number of color channels.\n\n"
             "Returns:\n"
             "    int: Number of color channels");

    // Audio sensory input
    py::class_<Audio, SensoryInput>(m, "Audio")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Initialize an Audio sensory input.\n\n"
             "Args:\n"
             "    sampleRate: Audio sample rate (Hz)\n"
             "    numSamples: Number of audio samples")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             "Set the audio data.\n\n"
             "Args:\n"
             "    data: Audio samples as a 1D array")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set the audio sample rate.\n\n"
             "Args:\n"
             "    sampleRate: Sample rate in Hz")
        .def("getSampleRate", &Audio::getSampleRate,
             "Get the audio sample rate.\n\n"
             "Returns:\n"
             "    int: Sample rate in Hz")
        .def("getNumSamples", &Audio::getNumSamples,
             "Get the number of audio samples.\n\n"
             "Returns:\n"
             "    int: Number of audio samples");

    // Internal signals sensory input
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value.\n\n"
             "Args:\n"
             "    value: Signal value to add")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all signals.");

    // Action class
    py::class_<Action>(m, "Action")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Initialize an Action with a type.\n\n"
             "Args:\n"
             "    type: Action type (ActionType enum)")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Initialize an Action with type and parameters.\n\n"
             "Args:\n"
             "    type: Action type (ActionType enum)\n"
             "    parameters: Action parameters")
        .def("getType", &Action::getType,
             "Get the action type.\n\n"
             "Returns:\n"
             "    ActionType: Action type")
        .def("setType", &Action::setType, py::arg("type"),
             "Set the action type.\n\n"
             "Args:\n"
             "    type: Action type")
        .def("getParameters", &Action::getParameters,
             "Get the action parameters.\n\n"
             "Returns:\n"
             "    list[float]: Action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set the action parameters.\n\n"
             "Args:\n"
             "    params: Action parameters")
        .def("getName", &Action::getName,
             "Get the action name.\n\n"
             "Returns:\n"
             "    str: Action name")
        .def("clone", &Action::clone,
             "Create a copy of this action.\n\n"
             "Returns:\n"
             "    Action: Copy of this action");

    // WorldObject class
    py::class_<WorldObject>(m, "WorldObject")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f,
             "Initialize a WorldObject.\n\n"
             "Args:\n"
             "    x: X coordinate\n"
             "    y: Y coordinate\n"
             "    type: World object type\n"
             "    value: Object value (default: 0.0)\n"
             "    radius: Object radius (default: 0.5)")
        .def_readwrite("x", &WorldObject::x,
             "X coordinate of the object")
        .def_readwrite("y", &WorldObject::y,
             "Y coordinate of the object")
        .def_readwrite("radius", &WorldObject::radius,
             "Object radius")
        .def_readwrite("type", &WorldObject::type,
             "Object type (WorldObjectType enum)")
        .def_readwrite("value", &WorldObject::value,
             "Object value")
        .def_readwrite("active", &WorldObject::active,
             "Whether the object is active");

    // AgentBody class
    py::class_<AgentBody>(m, "AgentBody")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x,
             "X position")
        .def_readwrite("y", &AgentBody::y,
             "Y position")
        .def_readwrite("orientation", &AgentBody::orientation,
             "Orientation in radians")
        .def_readwrite("velocityX", &AgentBody::velocityX,
             "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY,
             "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity,
             "Angular velocity (rad/s)")
        .def_readwrite("energy", &AgentBody::energy,
             "Energy level")
        .def_readwrite("health", &AgentBody::health,
             "Health level")
        .def_readwrite("age", &AgentBody::age,
             "Age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving,
             "Whether the agent is moving")
        .def_readwrite("isTurning", &AgentBody::isTurning,
             "Whether the agent is turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime,
             "Time of last action")
        .def("reset", &AgentBody::reset,
             "Reset agent body to initial state");

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Initialize an ActionResult.\n\n"
             "Args:\n"
             "    reward: Reward received\n"
             "    success: Whether the action succeeded\n"
             "    message: Optional message")
        .def_readwrite("reward", &ActionResult::reward,
             "Reward received")
        .def_readwrite("success", &ActionResult::success,
             "Whether the action succeeded")
        .def_readwrite("message", &ActionResult::message,
             "Optional message");

    // SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision,
             "Get the vision data.\n\n"
             "Returns:\n"
             "    Vision: Vision data")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set the vision data.\n\n"
             "Args:\n"
             "    vision: Vision data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth,
             "Get the vision width.\n\n"
             "Returns:\n"
             "    int: Vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight,
             "Get the vision height.\n\n"
             "Returns:\n"
             "    int: Vision height")
        .def("getTouch", &SensoryPercept::getTouch,
             "Get the touch data.\n\n"
             "Returns:\n"
             "    list[float]: Touch data")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set the touch data.\n\n"
             "Args:\n"
             "    touch: Touch data")
        .def("getInternal", &SensoryPercept::getInternal,
             "Get the internal signals.\n\n"
             "Returns:\n"
             "    list[float]: Internal signals")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set the internal signals.\n\n"
             "Args:\n"
             "    internal: Internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception,
             "Get the proprioception data.\n\n"
             "Returns:\n"
             "    list[float]: Proprioception data")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set the proprioception data.\n\n"
             "Args:\n"
             "    proprioception: Proprioception data")
        .def("getAudio", &SensoryPercept::getAudio,
             "Get the audio data.\n\n"
             "Returns:\n"
             "    list[float]: Audio data")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set the audio data.\n\n"
             "Args:\n"
             "    audio: Audio data")
        .def("getAllSignals", &SensoryPercept::getAllSignals,
             "Get all sensory signals.\n\n"
             "Returns:\n"
             "    dict: Dictionary with all sensory data")
        .def("getTimestamp", &SensoryPercept::getTimestamp,
             "Get the percept timestamp.\n\n"
             "Returns:\n"
             "    float: Timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set the percept timestamp.\n\n"
             "Args:\n"
             "    timestamp: Timestamp");

    // SimpleWorld class with context manager support
    py::class_<SimpleWorld>(m, "SimpleWorld")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure the world simulation.\n\n"
             "Args:\n"
             "    width: World width\n"
             "    height: World height\n"
             "    visionWidth: Vision sensor width\n"
             "    visionHeight: Vision sensor height")
        .def("reset", &SimpleWorld::reset,
             "Reset the world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set the agent starting position.\n\n"
             "Args:\n"
             "    x: X coordinate\n"
             "    y: Y coordinate")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update the world simulation.\n\n"
             "Args:\n"
             "    timestep: Time step duration")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply a motor command to the world.\n\n"
             "Args:\n"
             "    cmd: Motor command\n"
             "    currentTime: Current simulation time")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get the current sensory percept.\n\n"
             "Returns:\n"
             "    SensoryPercept: Current sensory data")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get the agent body state.\n\n"
             "Returns:\n"
             "    AgentBody: Agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add a world object.\n\n"
             "Args:\n"
             "    obj: World object to add")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove a world object.\n\n"
             "Args:\n"
             "    x: X coordinate\n"
             "    y: Y coordinate")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if a position is valid.\n\n"
             "Args:\n"
             "    x: X coordinate\n"
             "    y: Y coordinate\n\n"
             "Returns:\n"
             "    bool: True if position is valid")
        .def("getWidth", &SimpleWorld::getWidth,
             "Get the world width.\n\n"
             "Returns:\n"
             "    int: World width")
        .def("getHeight", &SimpleWorld::getHeight,
             "Get the world height.\n\n"
             "Returns:\n"
             "    int: World height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             "Get the maximum energy.\n\n"
             "Returns:\n"
             "    float: Maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set the maximum energy.\n\n"
             "Args:\n"
             "    e: Maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get the energy decay rate.\n\n"
             "Returns:\n"
             "    float: Energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set the energy decay rate.\n\n"
             "Args:\n"
             "    r: Energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get the current simulation time.\n\n"
             "Returns:\n"
             "    float: Current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set the random seed.\n\n"
             "Args:\n"
             "    seed: Random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get the random seed.\n\n"
             "Returns:\n"
             "    int: Random seed");

    // Brain class with context manager support
    py::class_<Brain>(m, "Brain")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create a Brain with configuration.\n\n"
             "Args:\n"
             "    config: Brain configuration")
        .def(py::init<py::object>(), py::arg("config_obj"),
             "Create a Brain from a Python config object.\n\n"
             "This is a convenience constructor for use in Python scripts.\n\n"
             "Args:\n"
             "    config_obj: Python dict, Config object, or config file path\n\n"
             "Returns:\n"
             "    Brain: Brain instance")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step.\n\n"
             "Args:\n"
             "    currentStep: Current simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp.\n\n"
             "Args:\n"
             "    currentStep: Current simulation step\n"
             "    currentTime: Current simulation time")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain.\n\n"
             "Args:\n"
             "    input: Sensory input data")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron.\n\n"
             "Args:\n"
             "    neuron: Target neuron ID\n"
             "    current: Current to inject")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type.\n\n"
             "Args:\n"
             "    type: Neuron type\n"
             "    current: Current to inject")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity.\n\n"
             "Returns:\n"
             "    Action: Produced action")
        .def("reset", &Brain::reset,
             "Reset brain state to initial condition")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file.\n\n"
             "Args:\n"
             "    filepath: Path to save file\n\n"
             "Returns:\n"
             "    bool: True if successful, False otherwise")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file.\n\n"
             "Args:\n"
             "    filepath: Path to load file\n\n"
             "Returns:\n"
             "    bool: True if successful, False otherwise")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region.\n\n"
             "Args:\n"
             "    name: Region name (optional)\n\n"
             "Returns:\n"
             "    RegionId: New region ID")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID.\n\n"
             "Args:\n"
             "    id: Region ID\n\n"
             "Returns:\n"
             "    NeuralRegion: Region data")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions.\n\n"
             "Returns:\n"
             "    int: Number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs.\n\n"
             "Returns:\n"
             "    list: List of region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions.\n\n"
             "Returns:\n"
             "    list: List of all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions.\n\n"
             "Returns:\n"
             "    int: Total neuron count")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions.\n\n"
             "Returns:\n"
             "    int: Total synapse count")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons.\n\n"
             "Returns:\n"
             "    int: Active neuron count")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons.\n\n"
             "Returns:\n"
             "    int: Firing neuron count")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons.\n\n"
             "Returns:\n"
             "    float: Average firing rate")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio.\n\n"
             "Returns:\n"
             "    float: E/I ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count.\n\n"
             "Returns:\n"
             "    int: Total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage.\n\n"
             "Returns:\n"
             "    DevelopmentalStage: Current stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"),
             "Set developmental stage.\n\n"
             "Args:\n"
             "    stage: Developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration.\n\n"
             "Returns:\n"
             "    Config: Brain configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status to console")
        // Context manager support
        .def("__enter__", [](std::shared_ptr<Brain> self) {
            return self;
        })
        .def("__exit__", [](std::shared_ptr<Brain> self, py::args) {
            self->reset();
        });

    // AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create an AgentBrain with brain reference.\n\n"
             "Args:\n"
             "    brain: Brain instance")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world.\n\n"
             "Args:\n"
             "    world: SimpleWorld instance")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size.\n\n"
             "Returns:\n"
             "    int: Sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size.\n\n"
             "Returns:\n"
             "    int: Motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain.\n\n"
             "Args:\n"
             "    percept: Sensory percept")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command.\n\n"
             "Returns:\n"
             "    MotorCommand: Decoded motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation.\n\n"
             "Args:\n"
             "    reward: Actual reward\n"
             "    predictedReward: Predicted reward")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"),
             "Update development system.\n\n"
             "Args:\n"
             "    timestep: Time step")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage.\n\n"
             "Returns:\n"
             "    DevelopmentalStage: Current stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level.\n\n"
             "Returns:\n"
             "    float: Neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level.\n\n"
             "Returns:\n"
             "    float: Curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level.\n\n"
             "Returns:\n"
             "    float: Novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error.\n\n"
             "Returns:\n"
             "    float: Prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain.\n\n"
             "Returns:\n"
             "    Brain: Underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             "Enable/disable reward modulation.")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable/disable structural plasticity.")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable/disable development.")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable/disable curiosity.")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled.")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled.")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled.")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled.")
        // Pythonic properties for common agent attributes
        .def_property_readonly("brain", &AgentBrain::getBrain,
             "Get the underlying brain (Python property).")
        // Context manager support
        .def("__enter__", [](std::shared_ptr<AgentBrain> self) {
            return self;
        })
        .def("__exit__", [](std::shared_ptr<AgentBrain> self, py::args) {
            self->reset();
        });

    // Factory functions with comprehensive documentation
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration.\n\n"
      "Returns:\n"
      "    Config: Default configuration object")

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration.\n\n"
      "Args:\n"
      "    config: Configuration for the brain\n\n"
      "Returns:\n"
      "    Brain: New brain instance")

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world.\n\n"
      "Returns:\n"
      "    SimpleWorld: New world instance")

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface.\n\n"
      "Args:\n"
      "    brain: Brain instance to interface\n\n"
      "Returns:\n"
      "    AgentBrain: Agent brain interface")

    // Convenience factory functions
    m.def("createQuickSimulation", [](int neuronCount, float simulationTime) {
        auto config = std::make_shared<Config>();
        config->setInt("brain.neuron_count", neuronCount);
        config->setFloat("brain.v_thresh", -50.0f);
        config->setFloat("brain.v_rest", -70.0f);
        config->setFloat("brain.v_reset", -75.0f);
        config->setFloat("brain.tau_mem", 20.0f);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return std::make_tuple(brain, config);
    }, py::arg("neuronCount") = 1000, py::arg("simulationTime") = 100.0f,
      "Create a quick simulation setup with default parameters.\n\n"
      "Args:\n"
      "    neuronCount: Number of neurons (default: 1000)\n"
      "    simulationTime: Simulation time (default: 100.0)\n\n"
      "Returns:\n"
      "    tuple: (Brain, Config) tuple")

    m.def("createWorldForAgent", [](float width, float height, float visionWidth, float visionHeight) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }, py::arg("width") = 20.0f, py::arg("height") = 20.0f,
       py::arg("visionWidth") = 8.0f, py::arg("visionHeight") = 8.0f,
       "Create a world configured for agent simulation.\n\n"
       "Args:\n"
       "    width: World width (default: 20.0)\n"
       "    height: World height (default: 20.0)\n"
       "    visionWidth: Vision sensor width (default: 8.0)\n"
       "    visionHeight: Vision sensor height (default: 8.0)\n\n"
       "Returns:\n"
       "    SimpleWorld: Configured world instance")

    m.def("createCompleteAgentSetup", [](int neuronCount, float worldWidth, float worldHeight) {
        auto config = std::make_shared<Config>();
        config->setInt("brain.neuron_count", neuronCount);
        config->setBool("neuromod.dopamine.enable", true);
        config->setBool("neuromod.curiosity.enable", true);
        config->setBool("neuromod.novelty.enable", true);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(worldWidth, worldHeight, 8, 8);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("neuronCount") = 500, py::arg("worldWidth") = 15.0f, py::arg("worldHeight") = 15.0f,
      "Create a complete agent setup with brain, world, and agent.\n\n"
      "Args:\n"
      "    neuronCount: Number of neurons (default: 500)\n"
      "    worldWidth: World width (default: 15.0)\n"
      "    worldHeight: World height (default: 15.0)\n\n"
      "Returns:\n"
      "    tuple: (Brain, SimpleWorld, AgentBrain) tuple")

    // Preset configurations
    m.def("createBalancedConfig", []() {
        auto config = std::make_shared<Config>();
        config->setInt("brain.neuron_count", 1000);
        config->setFloat("brain.connection_probability", 0.05f);
        config->setFloat("brain.v_thresh", -50.0f);
        config->setFloat("brain.v_rest", -70.0f);
        config->setFloat("brain.v_reset", -75.0f);
        config->setFloat("brain.tau_mem", 20.0f);
        config->setFloat("brain.tau_ref", 2.0f);
        config->setFloat("plasticity.stdp.learning_rate", 0.001f);
        config->setFloat("plasticity.hebbian.learning_rate", 0.001f);
        config->setBool("neuromod.dopamine.enable", true);
        config->setBool("neuromod.curiosity.enable", true);
        config->setBool("neuromod.novelty.enable", true);
        return config;
    }, "Create a balanced configuration for general simulation.\n\n"
      "Returns:\n"
      "    Config: Balanced configuration")

    m.def("createHighPerformanceConfig", []() {
        auto config = std::make_shared<Config>();
        config->setInt("brain.neuron_count", 5000);
        config->setFloat("brain.connection_probability", 0.02f);
        config->setFloat("brain.v_thresh", -45.0f);
        config->setFloat("brain.v_rest", -65.0f);
        config->setFloat("brain.v_reset", -70.0f);
        config->setFloat("brain.tau_mem", 15.0f);
        config->setFloat("brain.tau_ref", 1.0f);
        config->setBool("plasticity.stdp.enable", true);
        config->setBool("plasticity.hebbian.enable", false);
        config->setBool("plasticity.structural.enable", true);
        config->setBool("neuromod.dopamine.enable", true);
        config->setBool("neuromod.curiosity.enable", true);
        config->setBool("neuromod.novelty.enable", true);
        config->setBool("neuromod.prediction_error.enable", true);
        return config;
    }, "Create a high-performance configuration optimized for large simulations.\n\n"
      "Returns:\n"
      "    Config: High-performance configuration")

    m.def("createLearningFocusedConfig", []() {
        auto config = std::make_shared<Config>();
        config->setInt("brain.neuron_count", 1500);
        config->setFloat("brain.connection_probability", 0.08f);
        config->setFloat("brain.v_thresh", -55.0f);
        config->setFloat("brain.v_rest", -70.0f);
        config->setFloat("brain.v_reset", -75.0f);
        config->setFloat("brain.tau_mem", 25.0f);
        config->setFloat("brain.tau_ref", 3.0f);
        config->setBool("plasticity.stdp.enable", true);
        config->setBool("plasticity.hebbian.enable", true);
        config->setFloat("plasticity.stdp.learning_rate", 0.002f);
        config->setFloat("plasticity.hebbian.learning_rate", 0.001f);
        config->setBool("neuromod.dopamine.enable", true);
        config->setBool("neuromod.curiosity.enable", true);
        config->setBool("neuromod.novelty.enable", true);
        config->setBool("neuromod.prediction_error.enable", true);
        return config;
    }, "Create a learning-focused configuration optimized for neural plasticity.\n\n"
      "Returns:\n"
      "    Config: Learning-focused configuration")

    // Preset world configurations
    m.def("createStandardWorld", []() {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20.0f, 20.0f, 8, 8);
        return world;
    }, "Create a standard world configuration.\n\n"
      "Returns:\n"
      "    SimpleWorld: Standard world")

    m.def("createLargeWorld", []() {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(50.0f, 50.0f, 16, 16);
        return world;
    }, "Create a large world configuration.\n\n"
      "Returns:\n"
      "    SimpleWorld: Large world")

    m.def("createSmallWorld", []() {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(10.0f, 10.0f, 4, 4);
        return world;
    }, "Create a small world configuration.\n\n"
      "Returns:\n"
      "    SimpleWorld: Small world")

    // Performance optimization functions
    m.def("optimizeForSpeed", [](std::shared_ptr<Brain> brain) {
        // Enable performance optimizations
        brain->getConfig()->setBool("performance.optimization.enabled", true);
        brain->getConfig()->setBool("performance.simulation.useSIMD", true);
        brain->getConfig()->setBool("performance.memory.usePools", true);
    }, py::arg("brain"),
      "Optimize brain performance for speed.\n\n"
      "Args:\n"
      "    brain: Brain to optimize")

    m.def("optimizeForAccuracy", [](std::shared_ptr<Brain> brain) {
        // Enable accuracy optimizations
        brain->getConfig()->setBool("performance.optimization.enabled", true);
        brain->getConfig()->setBool("performance.simulation.useHighPrecision", true);
        brain->getConfig()->setBool("performance.memory.useExact", true);
    }, py::arg("brain"),
      "Optimize brain performance for accuracy.\n\n"
      "Args:\n"
      "    brain: Brain to optimize")

    // Simulation convenience functions
    m.def("runSimulationSteps", [](std::shared_ptr<Brain> brain, int steps, float timeStep) {
        for (int i = 0; i < steps; ++i) {
            brain->step(i, timeStep * i);
        }
    }, py::arg("brain"), py::arg("steps"), py::arg("timeStep") = 0.1f,
      "Run a simulation for a given number of steps.\n\n"
      "Args:\n"
      "    brain: Brain to run\n"
      "    steps: Number of steps to run\n"
      "    timeStep: Time step duration (default: 0.1)")

    m.def("runAgentEpisode", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world, int steps) {
        world->reset();
        agent->initialize(*world);
        
        for (int i = 0; i < steps; ++i) {
            world->update(0.1f);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            agent->getBrain()->step(i);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
    }, py::arg("agent"), py::arg("world"), py::arg("steps"),
      "Run a complete agent episode.\n\n"
      "Args:\n"
      "    agent: Agent brain\n"
      "    world: World\n"
      "    steps: Number of steps")

    // Utility functions
    m.def("createVisualizationConfig", []() {
        auto config = std::make_shared<Config>();
        config->setBool("visualization.enabled", true);
        config->setInt("visualization.fps", 30);
        config->setFloat("visualization.scale", 1.0f);
        config->setString("visualization.output", "window");
        return config;
    }, "Create a configuration for visualization.\n\n"
      "Returns:\n"
      "    Config: Visualization configuration")

    m.def("createDebugConfig", []() {
        auto config = std::make_shared<Config>();
        config->setBool("debug.enabled", true);
        config->setBool("debug.log_level", 3);
        config->setBool("debug.validate_connections", true);
        config->setBool("debug.track_memory", true);
        return config;
    }, "Create a debug configuration.\n\n"
      "Returns:\n"
      "    Config: Debug configuration")

    // Exposure of invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Module metadata
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Project";
    m.attr("__description__") = "Python bindings for the NLM neural simulation framework";
}

} // namespace nlm
