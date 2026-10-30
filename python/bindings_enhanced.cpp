#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
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
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"

namespace py = pybind11;
namespace nlm {

// Custom Python exceptions
class PyNLMException : public std::runtime_error {
public:
    explicit PyNLMException(const std::string& message)
        : std::runtime_error("NLM Error: " + message) {}
};

class PyValueError : public std::invalid_argument {
public:
    explicit PyValueError(const std::string& message)
        : std::invalid_argument("NLM ValueError: " + message) {}
};

class PyIndexError : public std::out_of_range {
public:
    explicit PyIndexError(const std::string& message)
        : std::out_of_range("NLM IndexError: " + message) {}
};

class PyConfigError : public PyNLMException {
public:
    explicit PyConfigError(const std::string& message)
        : PyNLMException("Config Error: " + message) {}
};

PYBIND11_MODULE(pynlm, m) {
    // Register custom Python exceptions
    py::register_exception<PyNLMException>(m, "NLMException");
    py::register_exception<PyValueError>(m, "ValueError");
    py::register_exception<PyIndexError>(m, "IndexError");
    py::register_exception<PyConfigError>(m, "ConfigError");
    
    // Enhanced module documentation with examples
    m.doc() = R"pbdoc(
NLM (Neural Learning Machine) Python Bindings
---------------------------------------------

A Python binding for the NLM C++ neural simulation framework.

This module provides a comprehensive interface to NLM's neural simulation capabilities,
including brain simulation, configuration management, sensory processing, motor control,
and integrated memory systems.

Key Features:
- Brain simulation with LIF neurons and integrated cognitive systems
- Configuration management with JSON/YAML support
- Sensory processing (vision, audio, internal signals)
- Motor control and action execution
- Memory systems (working, episodic, associative)
- Neuromodulation (reward, curiosity, novelty)
- Developmental systems and plasticity
- World simulation environment

Quick Start Example:
-------------------

>>> import pynlm
>>> 
>>> # Create a configuration
>>> config = pynlm.Config()
>>> config.set("neuron_count", 1000)
>>> config.set("region_count", 2)
>>> 
>>> # Create a brain with the configuration
>>> brain = pynlm.Brain(config)
>>> brain.initialize()
>>> 
>>> # Create a simulation world
>>> world = pynlm.SimpleWorld()
>>> world.configure(800, 600, 80, 60)  # width, height, vision_width, vision_height
>>> 
>>> # Create agent brain interface
>>> agent_brain = pynlm.AgentBrain(brain)
>>> agent_brain.initialize(world)
>>> 
>>> # Main simulation loop
>>> for step in range(1000):
...     # Get sensory input
...     percept = world.getSensoryPercept()
...     
...     # Process sensory input
...     agent_brain.processSensoryInput(percept)
...     
...     # Get action from brain
...     action = brain.produceAction()
...     
...     # Apply action to world
...     result = world.applyMotorCommand(action, step)
...     
...     # Optional: provide reward signal
...     # agent_brain.applyRewardModulation(reward, predicted_reward)
...     
...     # Update development
...     agent_brain.updateDevelopment(1)
>>> 
>>> # Save brain state
>>> brain.save("brain_state.json")
>>> 
>>> # Statistics
>>> print(f"Neurons: {brain.getTotalNeuronCount()}")
>>> print(f"Total spikes: {brain.getTotalSpikeCount()}")
>>> print(f"Active neurons: {brain.getActiveNeuronCount()}")

Installation:
-------------

    pip install pynlm

Building from source:
--------------------

    mkdir build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make -j4
    pip install .

API Reference:
--------------
";

    // Core identifier types with enhanced functionality
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
Neuron identifier with enhanced functionality

Attributes:
    value: Unique neuron identifier (uint64_t)

Methods:
    index(): Return the neuron index
    __eq__(): Equality comparison
    __ne__(): Inequality comparison
    __hash__(): Hash function for use in dictionaries
    __repr__(): String representation

Example:
    >>> nid = pynlm.NeuronId(42)
    >>> print(f"Neuron ID: {nid}")
    >>> print(f"Index: {nid.index()}")
)pbdoc")
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
            return std::to_string(id.value);
        });

    // Enhanced Config class with validation and type safety
    py::class_<Config>(m, "Config", R"pbdoc(
Configuration manager with JSON/YAML support and validation

Provides type-safe access to configuration values with automatic type conversion
and validation. Supports multiple file formats (simple key=value, JSON, YAML, INI).

Attributes:
    is_loaded: Read-only property indicating if configuration is loaded
    entry_count: Read-only property returning number of configuration entries

Methods:
    loadFromFile(filepath): Load configuration from file (auto-detects format)
    loadFromArgs(argc, argv): Load from command line arguments
    saveToFile(filepath): Save configuration to file
    get(key): Get value with type-safe access (returns Optional)
    getOr(key, default): Get value with default fallback
    set(key, value, source=Runtime): Set configuration value
    has(key): Check if key exists
    remove(key): Remove configuration key
    clear(): Clear all configuration
    validate(): Validate configuration
    getValidatedKeys(): Get list of valid keys
    getAllEntries(): Get all configuration entries
    getStats(): Get configuration statistics
    serialize(format): Serialize to string in specified format
    __repr__(): String representation

Examples:
    >>> config = pynlm.Config()
    >>> config.set("neuron_count", 1000)
    >>> config.set("learning_rate", 0.01)
    >>> config.set("enabled", True)
    >>> 
    >>> count = config.getOr("neuron_count", 100)
    >>> print(f"Neuron count: {count}")
    >>> 
    >>> config.saveToFile("brain_config.json")
    >>> 
    >>> loaded_config = pynlm.Config()
    >>> loaded_config.loadFromFile("brain_config.json")
)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from file (auto-detects format)")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to file (auto-detects format)")
        .def("get", py::overload_cast<const std::string&, int>(&Config::get),
             py::arg("key"), py::arg("default_value") = nullptr,
             "Get configuration value with type-safe access")
        .def("get", py::overload_cast<const std::string&, int64_t>(&Config::get),
             py::arg("key"), py::arg("default_value") = nullptr,
             "Get configuration value with type-safe access")
        .def("get", py::overload_cast<const std::string&, double>(&Config::get),
             py::arg("key"), py::arg("default_value") = nullptr,
             "Get configuration value with type-safe access")
        .def("get", py::overload_cast<const std::string&, bool>(&Config::get),
             py::arg("key"), py::arg("default_value") = nullptr,
             "Get configuration value with type-safe access")
        .def("get", py::overload_cast<const std::string&, std::string>(&Config::get),
             py::arg("key"), py::arg("default_value") = nullptr,
             "Get configuration value with type-safe access")
        .def("getOr", [](Config& self, const std::string& key, int defaultValue) {
            return self.getOr<int>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"), "Get value with default")
        .def("getOr", [](Config& self, const std::string& key, double defaultValue) {
            return self.getOr<double>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"), "Get value with default")
        .def("getOr", [](Config& self, const std::string& key, bool defaultValue) {
            return self.getOr<bool>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"), "Get value with default")
        .def("getOr", [](Config& self, const std::string& key, std::string defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"), "Get value with default")
        .def("set", [](Config& self, const std::string& key, const std::string& value, 
               py::object source) {
            ConfigSource src = ConfigSource::Runtime;
            if (source.is(py::str("File"))) src = ConfigSource::File;
            else if (source.is(py::str("CommandLine"))) src = ConfigSource::CommandLine;
            else if (source.is(py::str("Default"))) src = ConfigSource::Default;
            self.set(key, value, src);
        }, py::arg("key"), py::arg("value"), py::arg("source") = "Runtime",
           "Set configuration value with source")
        .def("set", [](Config& self, const std::string& key, int value, 
               py::object source) {
            ConfigSource src = ConfigSource::Runtime;
            if (source.is(py::str("File"))) src = ConfigSource::File;
            else if (source.is(py::str("CommandLine"))) src = ConfigSource::CommandLine;
            else if (source.is(py::str("Default"))) src = ConfigSource::Default;
            self.set(key, value, src);
        }, py::arg("key"), py::arg("value"), py::arg("source") = "Runtime",
           "Set configuration value with source")
        .def("set", [](Config& self, const std::string& key, double value, 
               py::object source) {
            ConfigSource src = ConfigSource::Runtime;
            if (source.is(py::str("File"))) src = ConfigSource::File;
            else if (source.is(py::str("CommandLine"))) src = ConfigSource::CommandLine;
            else if (source.is(py::str("Default"))) src = ConfigSource::Default;
            self.set(key, value, src);
        }, py::arg("key"), py::arg("value"), py::arg("source") = "Runtime",
           "Set configuration value with source")
        .def("set", [](Config& self, const std::string& key, bool value, 
               py::object source) {
            ConfigSource src = ConfigSource::Runtime;
            if (source.is(py::str("File"))) src = ConfigSource::File;
            else if (source.is(py::str("CommandLine"))) src = ConfigSource::CommandLine;
            else if (source.is(py::str("Default"))) src = ConfigSource::Default;
            self.set(key, value, src);
        }, py::arg("key"), py::arg("value"), py::arg("source") = "Runtime",
           "Set configuration value with source")
        .def("has", &Config::has, py::arg("key"), "Check if key exists")
        .def("remove", &Config::remove, py::arg("key"), "Remove configuration key")
        .def("getKeys", &Config::getKeys, "Get all configuration keys")
        .def("clear", &Config::clear, "Clear all configuration")
        .def("validate", &Config::validate, "Validate configuration")
        .def("hasNonDefaultValues", &Config::hasNonDefaultValues,
             "Check if config has non-default values")
        .def("getValidatedKeys", &Config::getValidatedKeys,
             "Get list of valid keys")
        .def("getAllEntries", &Config::getAllEntries,
             "Get all configuration entries")
        .def("getStats", &Config::getStats, "Get configuration statistics")
        .def("serialize", &Config::serialize, py::arg("format") = "",
             "Serialize configuration to string")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Enhanced Neuron class with type safety and validation
    py::class_<Neuron>(m, "Neuron", R"pbdoc(
LIF neuron with type safety and enhanced functionality

Implements Leaky Integrate-and-Fire (LIF) neuron dynamics with support for
plasticity, neuromodulation, and developmental stages.

Attributes:
    id: Neuron identifier
    type: Neuron type (Excitatory, Inhibitory, etc.)
    membranePotential: Current membrane potential
    firingState: Current firing state
    threshold: Firing threshold

Methods:
    setType(type): Set neuron type
    setMembranePotential(potential): Set membrane potential
    addToMembranePotential(delta): Add to membrane potential
    checkThreshold(): Check if neuron has reached threshold
    stepLIF(currentTime, dt): Perform LIF integration step
    recordSpike(timestamp): Record a spike event
    reset(): Reset neuron state
    enablePlasticity(hebbian, stdp, rewardModulated): Enable plasticity mechanisms

Example:
    >>> neuron = Neuron()
    >>> neuron.setType(pynlm.NeuronType.Excitatory)
    >>> neuron.setMembranePotential(-70.0)
    >>> neuron.setThreshold(-55.0)
    >>> fired = neuron.stepLIF(0.1, 0.001)
    >>> if fired:
    ...     print("Neuron fired!")
)pbdoc")
        .def(py::init<NeuronId>(), py::arg("id"))
        .def("setType", &Neuron::setType, py::arg("type"), "Set neuron type")
        .def("setMembranePotential", &Neuron::setMembranePotential,
             py::arg("potential"), "Set membrane potential")
        .def("addToMembranePotential", &Neuron::addToMembranePotential,
             py::arg("delta"), "Add to membrane potential")
        .def("setThreshold", &Neuron::setThreshold,
             py::arg("threshold"), "Set firing threshold")
        .def("checkThreshold", &Neuron::checkThreshold,
             "Check if neuron has reached threshold")
        .def("stepLIF", &Neuron::stepLIF, py::arg("currentTime"), py::arg("dt"),
             "Perform LIF integration step")
        .def("recordSpike", &Neuron::recordSpike, py::arg("timestamp"),
             "Record a spike event")
        .def("reset", &Neuron::reset, "Reset neuron state")
        .def("enablePlasticity", &Neuron::enablePlasticity,
             py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"),
             "Enable plasticity mechanisms")
        .def("getState", &Neuron::getState, "Get neuron state")
        .def("getPlasticityFlags", &Neuron::getPlasticityFlags,
             "Get plasticity flags");

    // Enhanced SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
Base class for sensory input with enhanced type safety

Provides common interface for all sensory modalities (vision, audio, internal signals).

Attributes:
    type: Sensory input type
    dimensions: Number of dimensions
    timestamp: Time stamp

Methods:
    getType(): Get sensory input type
    getData(): Get raw data as list
    getDimensions(): Get data dimensions
    getTimestamp(): Get time stamp
    setTimestamp(timestamp): Set time stamp
    validateData(): Validate data ranges and dimensions

Example:
    >>> vision = pynlm.Vision(640, 480, 3)
    >>> data = [0.5] * (640 * 480 * 3)
    >>> vision.setData(data)
    >>> print(f"Width: {vision.getWidth()}")
    >>> print(f"Height: {vision.getHeight()}")
)pbdoc")
        .def("getType", &SensoryInput::getType, "Get sensory input type")
        .def("getData", [](const SensoryInput& input) {
            return input.getData();
        }, "Get raw data as list")
        .def("getDimensions", &SensoryInput::getDimensions,
             "Get data dimensions")
        .def("getTimestamp", &SensoryInput::getTimestamp,
             "Get time stamp")
        .def("setTimestamp", &SensoryInput::setTimestamp,
             py::arg("timestamp"), "Set time stamp")
        .def("validateData", &SensoryInput::validateData,
             "Validate data ranges and dimensions");

    // Enhanced Vision class with validation
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
Vision sensory input representing camera-like sensor

Provides 2D image input with configurable dimensions and color channels.

Attributes:
    width: Image width in pixels
    height: Image height in pixels
    channels: Number of color channels (1=gray, 3=RGB)

Methods:
    setData(data): Set image data as list of floats [0.0, 1.0]
    getWidth(): Get image width
    getHeight(): Get image height
    getChannels(): Get number of channels
    validateData(): Validate image dimensions and data

Example:
    >>> vision = Vision(640, 480, 3)
    >>> data = [0.5] * (640 * 480 * 3)
    >>> vision.setData(data)
    >>> print(f"Image shape: {vision.getWidth()}x{vision.getHeight()}x{vision.getChannels()}")
)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("validateData", &Vision::validateData,
             "Validate image dimensions and data");

    // Enhanced Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
Audio sensory input representing sound sensor

Provides audio signal input with configurable sample rate and duration.

Attributes:
    sampleRate: Sampling rate in Hz
    numSamples: Number of audio samples

Methods:
    setData(data): Set audio data as list of floats
    setSampleRate(sampleRate): Set sample rate
    getSampleRate(): Get sample rate
    getNumSamples(): Get number of samples
    validateData(): Validate audio data

Example:
    >>> audio = Audio(44100, 44100)  # 1 second of audio at 44.1kHz
    >>> data = [0.1 * math.sin(2 * math.pi * 440 * i / 44100) for i in range(44100)]
    >>> audio.setData(data)
    >>> audio.setSampleRate(44100)
)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set sample rate")
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("validateData", &Audio::validateData, "Validate audio data");

    // Enhanced Action class with type safety
    py::class_<Action>(m, "Action", R"pbdoc(
Action representation for motor output with enhanced type safety

Represents motor commands with type, parameters, and execution metadata.

Attributes:
    type: Action type (MoveForward, Look, Interact, etc.)
    parameters: Action parameters as list of floats

Methods:
    getType(): Get action type
    setType(type): Set action type
    getParameters(): Get action parameters
    setParameters(params): Set action parameters
    getName(): Get action name
    clone(): Create a copy of this action
    validate(): Validate action parameters

Example:
    >>> action = Action(ActionType.MoveForward)
    >>> action.setType(ActionType.TurnLeft)
    >>> action.setParameters([1.57])  # 90 degrees in radians
    >>> print(f"Action: {action.getName()}")
    >>> print(f"Parameters: {action.getParameters()}")
)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("validate", &Action::validate);

    // Enhanced SimpleWorld class with Pythonic interface
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
Simple 2D world for NLM simulation with enhanced Pythonic interface

Provides a simulation environment with sensory input and motor output capabilities.

Attributes:
    width: World width in units
    height: World height in units
    visionWidth: Vision sensor width
    visionHeight: Vision sensor height
    maxEnergy: Maximum energy for agents
    energyDecayRate: Energy decay rate per step

Methods:
    configure(width, height, visionWidth, visionHeight): Configure world dimensions
    reset(): Reset world to initial state
    setAgentStart(x, y): Set agent starting position
    update(timestep): Update world state
    applyMotorCommand(cmd, currentTime): Apply motor command
    getSensoryPercept(): Get sensory percept
    getAgentBody(): Get agent body state
    addObject(obj): Add world object
    removeObject(x, y): Remove object at position
    isValidPosition(x, y): Check if position is valid
    __enter__(): Context manager entry
    __exit__(): Context manager exit

Example:
    >>> world = SimpleWorld()
    >>> world.configure(800, 600, 80, 60)
    >>> world.setAgentStart(400, 300)
    >>> 
    >>> for step in range(100):
    ...     # Update world
    ...     world.update(0.001)
    ...     
    ...     # Get percept
    ...     percept = world.getSensoryPercept()
    ...     
    ...     # Apply action
    ...     action = Action(ActionType.MoveForward)
    ...     world.applyMotorCommand(action, step)
    ...     
    ...     # Check if agent is still in world
    ...     body = world.getAgentBody()
    ...     if body.energy <= 0:
    ...         print("Agent has died!")
    ...         break
)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions")
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart,
             py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &SimpleWorld::removeObject,
             py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition,
             py::arg("x"), py::arg("y"))
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // Enhanced AgentBrain class with Pythonic interface
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
Agent brain interface connecting NLM brain to world with enhanced functionality

Provides bridge between NLM brain and world simulation with sensory processing
and motor command decoding.

Attributes:
    brain: Underlying NLM brain

Methods:
    initialize(world): Initialize with world
    getSensoryInputSize(): Get expected sensory input size
    getMotorOutputSize(): Get expected motor output size
    processSensoryInput(percept): Process sensory percept
    decodeMotorCommand(): Decode brain motor activity
    applyRewardModulation(reward, predictedReward): Apply reward modulation
    updateDevelopment(timestep): Update development system
    getDevelopmentalStage(): Get developmental stage
    getNeuromodulationLevel(): Get neuromodulation level
    getCuriosityLevel(): Get curiosity level
    getNoveltyLevel(): Get novelty level
    getPredictionError(): Get prediction error
    reset(): Reset agent for new episode
    enableRewardModulation(enable): Enable/disable reward modulation
    enableStructuralPlasticity(enable): Enable/disable structural plasticity
    enableDevelopment(enable): Enable/disable development
    enableCuriosity(enable): Enable/disable curiosity
    __enter__(): Context manager entry
    __exit__(): Context manager exit

Example:
    >>> brain = pynlm.Brain(config)
    >>> world = pynlm.SimpleWorld()
    >>> brain.initialize()
    >>> world.configure(800, 600, 80, 60)
    >>> 
    >>> agent_brain = pynlm.AgentBrain(brain)
    >>> agent_brain.initialize(world)
    >>> 
    >>> for episode in range(10):
    ...     agent_brain.reset()
    ...     world.reset()
    ...     
    ...     for step in range(100):
    ...         percept = world.getSensoryPercept()
    ...         agent_brain.processSensoryInput(percept)
    ...         
    ...         action = brain.produceAction()
    ...         world.applyMotorCommand(action, step)
    ...         
    ...         reward = 0.0  # Calculate reward based on outcome
    ...         predicted_reward = 0.0  # Predict reward
    ...         agent_brain.applyRewardModulation(reward, predicted_reward)
    ...         
    ...         agent_brain.updateDevelopment(1)
    ...         
    ...         if step >= 99:
    ...             break
)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // Enhanced Brain class with comprehensive API
    py::class_<Brain>(m, "Brain", R"pbdoc(
Central neural simulation brain class with comprehensive functionality

The main brain class orchestrating all neural components including memory,
prediction, cognition, development, and neuromodulation systems.

Attributes:
    config: Brain configuration

Methods:
    initialize(): Initialize the brain with configuration
    step(currentStep): Perform a simulation step
    step(currentStep, currentTime): Perform simulation step with timestamp
    receiveSensoryInput(input): Inject sensory input
    injectCurrent(neuron, current): Inject current into specific neuron
    injectCurrentToNeurons(type, current): Inject current to all neurons of type
    produceAction(): Produce motor action
    reset(): Reset brain state
    save(filepath): Save brain state
    load(filepath): Load brain state
    addRegion(name=""): Add a new neural region
    getRegion(id): Get region by ID
    getRegionCount(): Get number of regions
    getRegionIds(): Get all region IDs
    getRegions(): Get all regions
    getTotalNeuronCount(): Get total neuron count
    getTotalSynapseCount(): Get total synapse count
    getActiveNeuronCount(): Get count of active neurons
    getFiringNeuronCount(): Get count of firing neurons
    getAverageFiringRate(): Get average firing rate
    getExcitationInhibitionRatio(): Get excitation/inhibition balance
    getTotalSpikeCount(): Get total spike count
    getDevelopmentalStage(): Get developmental stage
    setDevelopmentalStage(stage): Set developmental stage
    getConfig(): Get configuration
    logStatus(): Log brain status
    __enter__(): Context manager entry
    __exit__(): Context manager exit

Example:
    >>> config = pynlm.Config()
    >>> config.set("neuron_count", 1000)
    >>> brain = pynlm.Brain(config)
    >>> brain.initialize()
    >>> 
    >>> for step in range(1000):
    ...     brain.step(step)
    ...     
    ...     action = brain.produceAction()
    ...     
    ...     if step % 100 == 0:
    ...         brain.logStatus()
    ...         
    ...         # Save periodically
    ...         if step % 500 == 0:
    ...             brain.save(f"brain_step_{step}.json")
)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
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
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status");

    // Module-level utility functions
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

    // Sensory percept creation utilities
    m.def("createVisionPercept", [](const std::vector<float>& data,
                                 size_t width, size_t height,
                                 size_t channels = 3) -> std::unique_ptr<SensoryInput> {
        auto vision = std::make_unique<Vision>(width, height, channels);
        vision->setData(data);
        return vision;
    }, py::arg("data"), py::arg("width"), py::arg("height"), 
       py::arg("channels") = 3,
       "Create a vision percept from image data");

    m.def("createAudioPercept", [](const std::vector<float>& data,
                                 size_t sampleRate) -> std::unique_ptr<SensoryInput> {
        auto audio = std::make_unique<Audio>(sampleRate, data.size());
        audio->setData(data);
        audio->setSampleRate(sampleRate);
        return audio;
    }, py::arg("data"), py::arg("sampleRate"),
       "Create an audio percept from sound data");

    m.def("createInternalSignalsPercept", []() -> std::unique_ptr<SensoryInput> {
        return std::make_unique<InternalSignals>();
    }, "Create empty internal signals percept");

    // Action creation utilities
    m.def("createAction", [](ActionType type, const std::vector<float>& params = {}) {
        return Action(type, params);
    }, py::arg("type"), py::arg("params") = std::vector<float>(),
       "Create an action with specified type and parameters");

    m.def("createActionFromString", [](const std::string& actionName) {
        // This would need to be implemented to map string names to ActionType
        return Action();
    }, py::arg("actionName"), "Create action from string name");

    // Utility functions for common operations
    m.def("validateAction", [](const Action& action) {
        return action.validate();
    }, py::arg("action"), "Validate action parameters");

    m.def("calculateDistance", [](float x1, float y1, float x2, float y2) {
        float dx = x2 - x1;
        float dy = y2 - y1;
        return std::sqrt(dx*dx + dy*dy);
    }, py::arg("x1"), py::arg("y1"), py::arg("x2"), py::arg("y2"),
       "Calculate Euclidean distance between two points");

    m.def("clamp", [](float value, float min, float max) {
        return std::clamp(value, min, max);
    }, py::arg("value"), py::arg("min"), py::arg("max"),
       "Clamp value to specified range");

    m.def("normalize", [](float value, float min, float max) {
        if (max == min) return 0.0f;
        return (value - min) / (max - min);
    }, py::arg("value"), py::arg("min"), py::arg("max"),
       "Normalize value to [0, 1] range");

    // World object creation utilities
    m.def("createWorldObject", [](float x, float y, WorldObjectType type,
                                float value = 0.0f, float radius = 0.5f) {
        return WorldObject(x, y, type, value, radius);
    }, py::arg("x"), py::arg("y"), py::arg("type"),
       py::arg("value") = 0.0f, py::arg("radius") = 0.5f,
       "Create a world object");

    m.def("createMarkerObject", [](float x, float y, float radius = 0.1f) {
        WorldObject obj(x, y, WorldObjectType::Marker, 0.0f, radius);
        return obj;
    }, py::arg("x"), py::arg("y"), py::arg("radius") = 0.1f,
       "Create a marker world object");

    m.def("createHazardObject", [](float x, float y, float radius = 0.5f) {
        WorldObject obj(x, y, WorldObjectType::Hazard, 1.0f, radius);
        return obj;
    }, py::arg("x"), py::arg("y"), py::arg("radius") = 0.5f,
       "Create a hazard world object");

    // Enhanced agent body utility
    m.def("createAgentBody", [](float x, float y, float orientation = 0.0f) {
        AgentBody body;
        body.x = x;
        body.y = y;
        body.orientation = orientation;
        return body;
    }, py::arg("x"), py::arg("y"), py::arg("orientation") = 0.0f,
       "Create an agent body with initial position and orientation");

    // Python constants and convenience functions
    m.attr("PI") = 3.14159265358979323846;
    m.attr("EPSILON") = 1e-6;
    
    // Define action type constants for easier access
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
Action type enumeration for motor commands
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

    // Define world object type constants
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
World object type enumeration
)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Define neuron type constants
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
Neuron type enumeration
)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // Define synapse type constants
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
Synapse type enumeration
)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // Define developmental stage constants
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
Developmental stage enumeration
)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // Define firing state constants
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
Neuron firing state enumeration
)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // Define invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Context manager for automatic cleanup
    py::class_<std::unique_ptr<Config>>(m, "ConfigPtr", R"pbdoc(
Smart pointer for Config objects with automatic cleanup
)pbdoc")
        .def(py::init<>());

    py::class_<std::unique_ptr<Brain>>(m, "BrainPtr", R"pbdoc(
Smart pointer for Brain objects with automatic cleanup
)pbdoc")
        .def(py::init<>());

    py::class_<std::unique_ptr<SimpleWorld>>(m, "WorldPtr", R"pbdoc(
Smart pointer for SimpleWorld objects with automatic cleanup
)pbdoc")
        .def(py::init<>());

    py::class_<std::unique_ptr<AgentBrain>>(m, "AgentBrainPtr", R"pbdoc(
Smart pointer for AgentBrain objects with automatic cleanup
)pbdoc")
        .def(py::init<>());

    // Version and metadata
    m.attr("__version__") = "1.0.0";
    m.attr("__author__") = "NLM Project";
    m.attr("__description__") = "Python bindings for NLM neural simulation framework";
    
    // Performance monitoring (if available)
    m.def("getBuildInfo", []() {
        return std::map<std::string, std::string>{
            {"version", "1.0.0"},
            {"build_type", "Release"},
            {"compiler", "C++20"}
        };
    }, "Get build information");
}

} // namespace nlm
