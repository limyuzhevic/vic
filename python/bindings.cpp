#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

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

// Exception types for better error handling
class InitializationError : public std::runtime_error {
public:
    explicit InitializationError(const std::string& msg) : std::runtime_error(msg) {}
};

class ParameterError : public std::runtime_error {
public:
    explicit ParameterError(const std::string& msg) : std::runtime_error(msg) {}
};

class StateError : public std::runtime_error {
public:
    explicit StateError(const std::string& msg) : std::runtime_error(msg) {}
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        This module provides:
        - Brain simulation and neural computation
        - Agent-world interaction
        - Configuration management
        - Sensory and motor interfaces
        - Development and learning systems
        - Performance monitoring and utilities
    )pbdoc";

    // Register custom exceptions
    py::register_exception<InitializationError>(m, "InitializationError");
    py::register_exception<ParameterError>(m, "ParameterError");
    py::register_exception<StateError>(m, "StateError");
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron
        
        Attributes:
            value: The integer value of the neuron ID
        
        Methods:
            index(): Return the index part of the ID
            __eq__(): Compare equality with another NeuronId
            __ne__(): Compare inequality with another NeuronId
            __hash__(): Return hash value for use in dictionaries
            __repr__(): String representation
        
        Examples:
            >>> id1 = pynlm.NeuronId(42)
            >>> id2 = pynlm.NeuronId()
            >>> id1 == id2
            False
            >>> id1.value
            42
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0,
             "Create NeuronId with a specific value")
        .def_readwrite("value", &NeuronId::value,
                       "Get or set the neuron ID value")
        .def("index", &NeuronId::index,
             "Return the index part of the ID")
        .def("__eq__", &NeuronId::operator==,
             "Check equality with another NeuronId")
        .def("__ne__", &NeuronId::operator!=,
             "Check inequality with another NeuronId")
        .def("__hash__", [](const NeuronId& id) { 
            return std::hash<uint64_t>{}(id.value); },
             "Return hash value for use in dictionaries")
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        },
         "String representation of the NeuronId");

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse
        
        Attributes:
            value: The integer value of the synapse ID
        
        Methods:
            index(): Return the index part of the ID
            __eq__(): Compare equality with another SynapseId
            __ne__(): Compare inequality with another SynapseId
            __hash__(): Return hash value for use in dictionaries
            __repr__(): String representation
        
        Examples:
            >>> id1 = pynlm.SynapseId(42)
            >>> id2 = pynlm.SynapseId()
            >>> id1 == id2
            False
            >>> id1.value
            42
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0,
             "Create SynapseId with a specific value")
        .def_readwrite("value", &SynapseId::value,
                       "Get or set the synapse ID value")
        .def("index", &SynapseId::index,
             "Return the index part of the ID")
        .def("__eq__", &SynapseId::operator==,
             "Check equality with another SynapseId")
        .def("__ne__", &SynapseId::operator!=,
             "Check inequality with another SynapseId")
        .def("__hash__", [](const SynapseId& id) { 
            return std::hash<uint64_t>{}(id.value); },
             "Return hash value for use in dictionaries")
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        },
         "String representation of the SynapseId");

    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region
        
        Attributes:
            value: The integer value of the region ID
        
        Methods:
            index(): Return the index part of the ID
            __eq__(): Compare equality with another RegionId
            __ne__(): Compare inequality with another RegionId
            __hash__(): Return hash value for use in dictionaries
            __repr__(): String representation
        
        Examples:
            >>> id1 = pynlm.RegionId(42)
            >>> id2 = pynlm.RegionId()
            >>> id1 == id2
            False
            >>> id1.value
            42
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0,
             "Create RegionId with a specific value")
        .def_readwrite("value", &RegionId::value,
                       "Get or set the region ID value")
        .def("index", &RegionId::index,
             "Return the index part of the ID")
        .def("__eq__", &RegionId::operator==,
             "Check equality with another RegionId")
        .def("__ne__", &RegionId::operator!=,
             "Check inequality with another RegionId")
        .def("__hash__", [](const RegionId& id) { 
            return std::hash<uint64_t>{}(id.value); },
             "Return hash value for use in dictionaries")
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        },
         "String representation of the RegionId");

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population
        
        Attributes:
            value: The integer value of the population ID
        
        Methods:
            index(): Return the index part of the ID
            __eq__(): Compare equality with another PopulationId
            __ne__(): Compare inequality with another PopulationId
        
        Examples:
            >>> id1 = pynlm.PopulationId(42)
            >>> id2 = pynlm.PopulationId()
            >>> id1 == id2
            False
            >>> id1.value
            42
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0,
             "Create PopulationId with a specific value")
        .def_readwrite("value", &PopulationId::value,
                       "Get or set the population ID value")
        .def("index", &PopulationId::index,
             "Return the index part of the ID")
        .def("__eq__", &PopulationId::operator==,
             "Check equality with another PopulationId")
        .def("__ne__", &PopulationId::operator!=,
             "Check inequality with another PopulationId");

    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Enumeration of neuron types in the NLM system
        
        Values:
            Excitatory: Neuron that increases firing probability in target neurons
            Inhibitory: Neuron that decreases firing probability in target neurons
            Modulatory: Neuron that affects other neurons' excitability or plasticity
            Sensory: Neuron that processes sensory input
            Motor: Neuron that controls motor output
            Internal: Neuron for internal cognitive functions
        
        Examples:
            >>> neuron_type = pynlm.NeuronType.Excitatory
            >>> neuron_type == pynlm.NeuronType.Inhibitory
            False
        )pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Enumeration of synapse types in the NLM system
        
        Values:
            Excitatory: Synapse that increases firing probability
            Inhibitory: Synapse that decreases firing probability
            Modulatory: Synapse that affects neuron excitability or plasticity
            Electrical: Direct electrical coupling between neurons
            GapJunction: Gap junction coupling for electrical synapses
        
        Examples:
            >>> synapse_type = pynlm.SynapseType.Excitatory
            >>> synapse_type == pynlm.SynapseType.Inhibitory
            False
        )pbdoc")
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

    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system
        
        The Config class provides access to brain configuration parameters
        and allows loading, saving, and modifying configuration settings.
        
        Attributes:
            configuration: Internal configuration data storage
        
        Methods:
            __init__(): Create empty configuration
            loadFromFile(filepath): Load configuration from JSON file
            loadFromArgs(argc, argv): Load from command line arguments
            saveToFile(filepath): Save configuration to JSON file
            has(key): Check if configuration key exists
            get(key): Get configuration value by key
            set(key, value): Set configuration value
            getKeys(): Get all configuration keys
            clear(): Clear all configuration entries
            summary(): Get human-readable configuration summary
        
        Examples:
            >>> config = pynlm.createDefaultConfig()
            >>> config.set("brain.neuron_count", 1000)
            >>> config.set("plasticity.stdp.enable", True)
            >>> config.loadFromFile("config.json")
            >>> value = config.get("brain.neuron_count")
        
        Note:
            Common configuration keys include:
            - brain.neuron_count: Total number of neurons (int)
            - brain.synapse_density: Synaptic connection density (float)
            - brain.v_thresh: Neuron threshold potential (float)
            - plasticity.stdp.enable: Enable STDP learning (bool)
        )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file. Returns True on success.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
             "Load configuration from command line arguments. Returns True on success.")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file. Returns True on success.")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists. Returns True if key exists.")
        .def("get", [](const Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (value) {
                return *value;
            }
            return std::string();
        }, py::arg("key"),
             "Get configuration value as string. Returns empty string if key doesn't exist.")
        .def("getInt", [](const Config& self, const std::string& key, int defaultValue = 0) {
            auto value = self.get<int>(key);
            if (value) {
                return *value;
            }
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0,
             "Get configuration value as integer. Returns defaultValue if key doesn't exist.")
        .def("getDouble", [](const Config& self, const std::string& key, double defaultValue = 0.0) {
            auto value = self.get<double>(key);
            if (value) {
                return *value;
            }
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0.0,
             "Get configuration value as double. Returns defaultValue if key doesn't exist.")
        .def("getBool", [](const Config& self, const std::string& key, bool defaultValue = false) {
            auto value = self.get<bool>(key);
            if (value) {
                return *value;
            }
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = false,
             "Get configuration value as boolean. Returns defaultValue if key doesn't exist.")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             "Set configuration value as string")
        .def("set", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             "Set configuration value as integer")
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             "Set configuration value as double")
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             "Set configuration value as boolean")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys as a list of strings.")
        .def("clear", &Config::clear,
             "Clear all configuration entries.")
        .def("summary", &Config::summary,
             "Get a human-readable summary of the configuration.")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        },
         "String representation of the configuration");

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for sensory input in NLM
        
        Abstract base class for all sensory modalities (vision, audio, internal signals).
        Provides interface for accessing sensory data and metadata.
        
        Attributes:
            timestamp: Time when the sensory data was captured
        
        Methods:
            getType(): Return the sensory input type
            getData(): Return raw sensory data as list
            getDimensions(): Return data dimensions
            getTimestamp(): Return data timestamp
            setTimestamp(timestamp): Set data timestamp
        
        Note:
            This is an abstract base class. Use Vision, Audio, or InternalSignals
            for concrete implementations.
        
        Examples:
            >>> vision = pynlm.Vision(10, 10, 3)
            >>> vision.setData([...])  # Set pixel data
            >>> data = vision.getData()
            >>> timestamp = vision.getTimestamp()
        )pbdoc")
        .def("getType", &SensoryInput::getType,
             "Get the type of sensory input")
        .def("getData", &SensoryInput::getData,
             "Get the raw data as a vector of floats")
        .def("getDimensions", &SensoryInput::getDimensions,
             "Get the dimensionality of the sensory data")
        .def("getTimestamp", &SensoryInput::getTimestamp,
             "Get the timestamp when data was captured")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp when data was captured");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input (camera-like input)
        
        Represents visual input with width, height, and color channels.
        Provides methods for accessing and manipulating image data.
        
        Attributes:
            width: Image width in pixels
            height: Image height in pixels
            channels: Number of color channels (typically 3 for RGB)
        
        Methods:
            __init__(): Create empty Vision input
            __init__(width, height, channels): Create Vision with specified dimensions
            setData(data): Set image pixel data
            getWidth(): Return image width
            getHeight(): Return image height
            getChannels(): Return number of color channels
            getType(): Return 'vision' (inherited)
            getData(): Return image data as list (inherited)
            getDimensions(): Return (width, height, channels) (inherited)
        
        Examples:
            >>> vision = pynlm.Vision(10, 10, 3)
            >>> vision.setData([255, 0, 0] * 100)  # Red image
            >>> width = vision.getWidth()
            >>> height = vision.getHeight()
            >>> channels = vision.getChannels()
            >>> data = vision.getData()
        
        Note:
            Data should be provided as a flat list of floats in range [0, 1]
            For RGB images: [R1, G1, B1, R2, G2, B2, ...]
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3,
             "Create Vision input with specified dimensions. Default is 3 color channels.")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             "Set image pixel data. Data should be a flat list of floats in range [0, 1]. "
             "For RGB images: [R1, G1, B1, R2, G2, B2, ...]")
        .def("getWidth", &Vision::getWidth,
             "Return image width in pixels")
        .def("getHeight", &Vision::getHeight,
             "Return image height in pixels")
        .def("getChannels", &Vision::getChannels,
             "Return number of color channels (typically 3 for RGB)");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input (sound input)
        
        Represents audio input with sample rate and number of samples.
        Provides methods for accessing and manipulating audio data.
        
        Attributes:
            sampleRate: Audio sampling rate in Hz
            numSamples: Number of audio samples
        
        Methods:
            __init__(): Create empty Audio input
            __init__(sampleRate, numSamples): Create Audio with specified parameters
            setData(data): Set audio sample data
            getSampleRate(): Return sample rate in Hz
            getNumSamples(): Return number of audio samples
            getType(): Return 'audio' (inherited)
            getData(): Return audio data as list (inherited)
            getDimensions(): Return (numSamples,) (inherited)
            setSampleRate(sampleRate): Set audio sample rate
        
        Examples:
            >>> audio = pynlm.Audio(44100, 1024)
            >>> audio.setSampleRate(48000)
            >>> audio.setData([0.1, -0.2, 0.3, ...])  # Audio samples
            >>> sample_rate = audio.getSampleRate()
            >>> num_samples = audio.getNumSamples()
            >>> data = audio.getData()
        
        Note:
            Audio data should be provided as a list of float samples in range [-1, 1]
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Create Audio input with specified sample rate and number of samples")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             "Set audio sample data. Data should be a list of floats in range [-1, 1].")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set the audio sample rate in Hz")
        .def("getSampleRate", &Audio::getSampleRate,
             "Return the audio sample rate in Hz")
        .def("getNumSamples", &Audio::getNumSamples,
             "Return the number of audio samples");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal signals sensory input (brain state information)
        
        Represents internal brain state information such as working memory,
        attention, or other internal neural states. Provides methods for
        accessing and manipulating internal signal data.
        
        Methods:
            __init__(): Create empty InternalSignals input
            addSignal(value): Add a signal value to the collection
            clearSignals(): Clear all signal values
            getType(): Return 'internal' (inherited)
            getData(): Return signal data as list (inherited)
            getDimensions(): Return number of signals (inherited)
        
        Examples:
            >>> internal = pynlm.InternalSignals()
            >>> internal.addSignal(0.5)  # Add attention level
            >>> internal.addSignal(0.8)  # Add arousal level
            >>> internal.clearSignals()
            >>> data = internal.getData()
        
        Note:
            Internal signals typically represent brain state variables
            like attention, arousal, working memory content, etc.
        )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value to the internal signals collection")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all signal values from the internal signals collection");

    py::class_<Action>(m, "Action", R"pbdoc(
        Action representation for motor output
        
        Represents a motor action that can be executed by the agent in the environment.
        Actions can be specified by type and optional parameters.
        
        Attributes:
            type: The type of action (ActionType enum)
            parameters: List of action parameters (varies by action type)
        
        Methods:
            __init__(): Create empty Action
            __init__(action_type): Create Action with specified type
            __init__(action_type, parameters): Create Action with type and parameters
            getType(): Return the action type
            setType(action_type): Set the action type
            getParameters(): Return action parameters
            setParameters(parameters): Set action parameters
            getName(): Return human-readable action name
            clone(): Create a copy of this Action
        
        Examples:
            >>> action = pynlm.Action(pynlm.ActionType.MoveForward)
            >>> action.getType()
            <ActionType.MoveForward: 1>
            >>> action.setType(pynlm.ActionType.TurnLeft)
            >>> action.getName()
            'TurnLeft'
            >>> action_clone = action.clone()
        
        Note:
            ActionType enumeration includes movement, perception, interaction,
            and other possible agent behaviors.
        )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create Action with specified type")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create Action with specified type and parameters")
        .def("getType", &Action::getType,
             "Return the action type (ActionType enum)")
        .def("setType", &Action::setType, py::arg("type"),
             "Set the action type")
        .def("getParameters", &Action::getParameters,
             "Return the action parameters as a list of floats")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set the action parameters")
        .def("getName", &Action::getName,
             "Return the human-readable name of the action type")
        .def("clone", &Action::clone,
             "Create a copy of this Action");

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        World object representation
        
        Represents an object in the simulated environment. Objects can be resources,
        hazards, walls, markers, or empty spaces.
        
        Attributes:
            x: X coordinate of the object center
            y: Y coordinate of the object center
            type: Type of object (WorldObjectType enum)
            value: Object value (positive for resources, negative for hazards)
            radius: Object radius for collision detection
            active: Whether the object is active in the environment
        
        Methods:
            __init__(): Create empty WorldObject
            __init__(x, y, type, value, radius): Create WorldObject with specified parameters
            x: Get or set X coordinate
            y: Get or set Y coordinate
            type: Get or set object type
            value: Get or set object value
            radius: Get or set object radius
            active: Get or set object active state
        
        Examples:
            >>> resource = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, 10.0)
            >>> hazard = pynlm.WorldObject(8.0, 8.0, pynlm.WorldObjectType.Hazard, -15.0)
            >>> wall = pynlm.WorldObject(2.0, 2.0, pynlm.WorldObjectType.Wall)
            >>> obj_x = resource.x
            >>> obj_type = resource.type
            >>> obj_value = resource.value
        
        Note:
            WorldObjectType enumeration includes Empty, Resource, Hazard, Wall, and Marker.
            Resources provide positive rewards when collected.
            Hazards provide negative rewards when collided with.
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Create WorldObject with specified parameters")
        .def_readwrite("x", &WorldObject::x,
                       "Get or set the X coordinate of the object")
        .def_readwrite("y", &WorldObject::y,
                       "Get or set the Y coordinate of the object")
        .def_readwrite("type", &WorldObject::type,
                       "Get or set the object type (WorldObjectType enum)")
        .def_readwrite("value", &WorldObject::value,
                       "Get or set the object value (positive for resources, negative for hazards)")
        .def_readwrite("radius", &WorldObject::radius,
                       "Get or set the object radius for collision detection")
        .def_readwrite("active", &WorldObject::active,
                       "Get or set whether the object is active in the environment");

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state
        
        Represents the physical state of the agent in the environment including
        position, orientation, movement, energy, and other body-related properties.
        
        Attributes:
            x: X coordinate position
            y: Y coordinate position
            orientation: Orientation angle in radians (-π to π)
            velocityX: Linear velocity in X direction
            velocityY: Linear velocity in Y direction
            angularVelocity: Angular velocity (radians per timestep)
            energy: Current energy level
            health: Current health (0-100, higher is better)
            age: Agent age in timesteps
            isMoving: Whether the agent is currently moving
            isTurning: Whether the agent is currently turning
            lastActionTime: Time since last action was executed
        
        Methods:
            __init__(): Create empty AgentBody
            reset(): Reset body to initial state
            x: Get or set X coordinate
            y: Get or set Y coordinate
            orientation: Get or set orientation angle
            velocityX: Get or set X velocity
            velocityY: Get or set Y velocity
            angularVelocity: Get or set angular velocity
            energy: Get or set energy level
            health: Get or set health
            age: Get or set age
            isMoving: Get or set moving state
            isTurning: Get or set turning state
            lastActionTime: Get or set last action time
        
        Examples:
            >>> body = pynlm.AgentBody()
            >>> body.x = 5.0
            >>> body.y = 5.0
            >>> body.orientation = 0.5
            >>> body.energy = 100.0
            >>> body.health = 100.0
            >>> body.isMoving = True
            >>> body.reset()
        
        Note:
            Agent body properties change over time based on actions,
            energy decay, health degradation, and other factors.
        )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x,
                       "Get or set the X coordinate position")
        .def_readwrite("y", &AgentBody::y,
                       "Get or set the Y coordinate position")
        .def_readwrite("orientation", &AgentBody::orientation,
                       "Get or set the orientation angle in radians (-π to π)")
        .def_readwrite("velocityX", &AgentBody::velocityX,
                       "Get or set the linear velocity in X direction")
        .def_readwrite("velocityY", &AgentBody::velocityY,
                       "Get or set the linear velocity in Y direction")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity,
                       "Get or set the angular velocity in radians per timestep")
        .def_readwrite("energy", &AgentBody::energy,
                       "Get or set the current energy level")
        .def_readwrite("health", &AgentBody::health,
                       "Get or set the current health (0-100)")
        .def_readwrite("age", &AgentBody::age,
                       "Get or set the agent age in timesteps")
        .def_readwrite("isMoving", &AgentBody::isMoving,
                       "Get or set whether the agent is currently moving")
        .def_readwrite("isTurning", &AgentBody::isTurning,
                       "Get or set whether the agent is currently turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime,
                       "Get or set the time since last action was executed")
        .def("reset", &AgentBody::reset,
             "Reset the agent body to its initial state");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Action result from world
        
        Represents the outcome of executing an action in the environment, including
        reward, success status, and descriptive message.
        
        Attributes:
            reward: Numerical reward value (positive for good outcomes, negative for bad)
            success: Whether the action was successful
            message: Descriptive message explaining the outcome
        
        Methods:
            __init__(): Create empty ActionResult
            __init__(reward, success, message): Create ActionResult with specified parameters
            reward: Get or set the reward value
            success: Get or set the success status
            message: Get or set the descriptive message
        
        Examples:
            >>> result = pynlm.ActionResult(10.0, True, "Collected resource")
            >>> reward = result.reward
            >>> success = result.success
            >>> message = result.message
            >>> result2 = pynlm.ActionResult(-5.0, False, "Hit hazard")
        
        Note:
            Action results are used by the agent's reward modulation system
            to learn from experience and adjust future behavior.
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create ActionResult with specified reward, success status, and message")
        .def_readwrite("reward", &ActionResult::reward,
                       "Get or set the numerical reward value")
        .def_readwrite("success", &ActionResult::success,
                       "Get or set whether the action was successful")
        .def_readwrite("message", &ActionResult::message,
                       "Get or set the descriptive message explaining the outcome");

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Sensory percept data
        
        Represents all sensory inputs available to the agent at a given moment,
        including vision, touch, internal signals, and audio data.
        
        Methods:
            __init__(): Create empty SensoryPercept
            getVision(): Return visual input (Vision object)
            setVision(vision): Set visual input
            getVisionWidth(): Return visual input width
            getVisionHeight(): Return visual input height
            getTouch(): Return touch input (InternalSignals)
            setTouch(touch): Set touch input
            getInternal(): Return internal signals (InternalSignals)
            setInternal(internal): Set internal signals
            getProprioception(): Return proprioceptive input (InternalSignals)
            setProprioception(proprioception): Set proprioceptive input
            getAudio(): Return audio input (Audio)
            setAudio(audio): Set audio input
            getAllSignals(): Return all sensory signals as a dictionary
            getTimestamp(): Return data timestamp
            setTimestamp(timestamp): Set data timestamp
        
        Examples:
            >>> percept = world.getSensoryPercept()
            >>> vision = percept.getVision()
            >>> vision_width = percept.getVisionWidth()
            >>> vision_height = percept.getVisionHeight()
            >>> touch = percept.getTouch()
            >>> internal = percept.getInternal()
            >>> audio = percept.getAudio()
            >>> all_signals = percept.getAllSignals()
        
        Note:
            SensoryPercept combines all sensory modalities into a single object
            that can be processed by the agent brain to produce actions.
        )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision,
             "Return visual input (Vision object)")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set visual input")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth,
             "Return visual input width in pixels")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight,
             "Return visual input height in pixels")
        .def("getTouch", &SensoryPercept::getTouch,
             "Return touch input (InternalSignals object)")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch input")
        .def("getInternal", &SensoryPercept::getInternal,
             "Return internal signals (InternalSignals object)")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception,
             "Return proprioceptive input (InternalSignals object)")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioceptive input")
        .def("getAudio", &SensoryPercept::getAudio,
             "Return audio input (Audio object)")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio input")
        .def("getAllSignals", &SensoryPercept::getAllSignals,
             "Return all sensory signals as a dictionary")
        .def("getTimestamp", &SensoryPercept::getTimestamp,
             "Return data timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set data timestamp");

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation
        
        Represents a 2D simulated environment where agents can live, move, and interact
        with objects. Provides the physical environment for agent-brain interaction.
        
        Attributes:
            width: World width in simulation units
            height: World height in simulation units
            visionWidth: Agent's visual field width
            visionHeight: Agent's visual field height
            maxEnergy: Maximum agent energy level
            energyDecayRate: Rate at which energy decays over time
            simTime: Current simulation time
        
        Methods:
            __init__(): Create empty SimpleWorld
            configure(width, height, visionWidth, visionHeight): Configure world dimensions
            reset(): Reset world to initial state
            setAgentStart(x, y): Set agent's starting position
            update(timestep): Update world state by given timestep
            applyMotorCommand(cmd, currentTime): Apply motor command to agent
            getSensoryPercept(): Get agent's sensory input
            getAgentBody(): Get agent's physical state
            addObject(obj): Add object to world
            removeObject(x, y): Remove object at position
            isValidPosition(x, y): Check if position is valid
            getWidth(): Return world width
            getHeight(): Return world height
            getMaxEnergy(): Return maximum energy
            setMaxEnergy(e): Set maximum energy
            getEnergyDecayRate(): Return energy decay rate
            setEnergyDecayRate(r): Set energy decay rate
            getSimulationTime(): Return current simulation time
            setRandomSeed(seed): Set random seed for reproducible behavior
            getRandomSeed(): Return current random seed
        
        Examples:
            >>> world = pynlm.createSimpleWorld()
            >>> world.configure(20.0, 20.0, 8, 8)
            >>> world.reset()
            >>> world.setAgentStart(10.0, 10.0)
            >>> world.update(0.1)  # Update world
            >>> percept = world.getSensoryPercept()
            >>> body = world.getAgentBody()
            >>> world.applyMotorCommand(pynlm.MotorCommand.MoveForward, 0.1)
        
        Note:
            SimpleWorld manages agent position, object placement, collision detection,
            and sensory perception. It's designed to work with the NLM agent-brain system.
        )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and agent's visual field")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state with agent and objects")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent's starting position in the world")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state by given timestep. Returns True if successful.")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to agent. Returns ActionResult.")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get agent's sensory input from the environment")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent's physical state (position, velocity, etc.)")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add an object to the world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at specified position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (within bounds and not occupied)")
        .def("getWidth", &SimpleWorld::getWidth,
             "Return world width in simulation units")
        .def("getHeight", &SimpleWorld::getHeight,
             "Return world height in simulation units")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             "Return maximum agent energy level")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy level")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Return energy decay rate per timestep")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate per timestep")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Return current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible object placement and behavior")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Return current random seed");

    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class
        
        The main brain class that implements real spiking neural computation with
        event-driven dynamics and integrated memory, prediction, cognition, and
        neuromodulation systems. This is the core computational engine of the NLM.
        
        Attributes:
            brain: Internal brain state storage
        
        Methods:
            __init__(config): Create brain with configuration
            initialize(): Initialize the brain with configuration
            step(step_number): Perform one simulation step
            step(step_number, time): Perform step with timestamp
            receiveSensoryInput(input): Inject sensory input into brain
            injectCurrent(neuron, current): Inject current into specific neuron
            injectCurrentToNeurons(type, current): Inject current into neurons of type
            produceAction(): Produce motor action based on neural activity
            reset(): Reset brain state
            save(filepath): Save brain state to file
            load(filepath): Load brain state from file
            addRegion(name): Add a new neural region
            getRegion(id): Get a region by ID
            getRegionCount(): Get number of regions
            getRegionIds(): Get all region IDs
            getRegions(): Get all regions
            getTotalNeuronCount(): Get total neuron count
            getTotalSynapseCount(): Get total synapse count
            getActiveNeuronCount(): Get count of active neurons
            getFiringNeuronCount(): Get count of firing neurons
            getAverageFiringRate(): Get average firing rate
            getExcitationInhibitionRatio(): Get E/I balance ratio
            getTotalSpikeCount(): Get total spike count
            getDevelopmentalStage(): Get current developmental stage
            setDevelopmentalStage(stage): Set developmental stage
            getConfig(): Get configuration
            logStatus(): Log brain status
            
            # Convenience methods (added improvements)
            runSimulation(steps, timestep): Run complete simulation
            resetAndInitialize(): Combined reset and initialize
            getStats(): Get comprehensive brain statistics
            validate(): Validate brain state
            isInitialized(): Check if brain is initialized
            
            # Memory system access
            getWorkingMemory(): Access working memory system
            getEpisodicMemory(): Access episodic memory system
            getAssociativeMemory(): Access associative memory system
            
            # Prediction system access
            getPredictionSystem(): Access prediction system
            
            # Cognition system access
            getPlanner(): Access neural planner
            getConceptFormation(): Access concept formation
            getAttention(): Access attentional selection
            
            # Development system access
            getDevelopmentSystem(): Access development system
            
            # Neuromodulation system access
            getDopamine(): Access dopamine system
            getCuriosity(): Access curiosity system
            getNovelty(): Access novelty system
            getPredictionErrorSignal(): Access prediction error signal
        
        Examples:
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> brain.step(0)
            >>> brain.step(100, 0.1)
            >>> stats = brain.getStats()
            >>> region_id = brain.addRegion("cortex")
            >>> neurons = brain.getTotalNeuronCount()
        
        Note:
            Brain is the central computational engine that processes sensory
            input, produces motor output, and learns from experience. It integrates
            multiple neural systems including memory, prediction, cognition, and
            neuromodulation to create a complete artificial brain.
        )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def("initialize", &Brain::initialize,
             "Initialize brain with configuration. Returns True on success.")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform one simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of specified type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity. Returns Action object.")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file. Returns True on success.")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file. Returns True on success.")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region. Returns RegionId.")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID. Returns NeuralRegion object.")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs as a list")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions as a list")
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
             "Get the configuration object")
        .def("logStatus", &Brain::logStatus,
             "Log brain status to output")
        // Convenience methods
        .def("runSimulation", [](Brain& self, int steps, double timestep) {
            for (int step = 0; step < steps; ++step) {
                self.step(step, timestep * step);
            }
        }, py::arg("steps"), py::arg("timestep"),
             "Run complete simulation for specified steps. Returns total spike count.")
        .def("resetAndInitialize", [](Brain& self, std::shared_ptr<Config> config) {
            self.reset();
            self.initialize();
        }, py::arg("config"),
             "Reset brain and reinitialize with configuration")
        .def("getStats", [](const Brain& self) {
            std::map<std::string, std::string> stats;
            stats["total_neuron_count"] = std::to_string(self.getTotalNeuronCount());
            stats["total_synapse_count"] = std::to_string(self.getTotalSynapseCount());
            stats["firing_neuron_count"] = std::to_string(self.getFiringNeuronCount());
            stats["active_neuron_count"] = std::to_string(self.getActiveNeuronCount());
            stats["total_spike_count"] = std::to_string(self.getTotalSpikeCount());
            stats["average_firing_rate"] = std::to_string(self.getAverageFiringRate());
            stats["excitation_inhibition_ratio"] = std::to_string(self.getExcitationInhibitionRatio());
            stats["developmental_stage"] = std::to_string(static_cast<int>(self.getDevelopmentalStage()));
            stats["region_count"] = std::to_string(self.getRegionCount());
            return stats;
        }, "Get comprehensive brain statistics as a dictionary")
        .def("validate", [](Brain& self) {
            if (!self.getConfig()) {
                throw InitializationError("Brain not properly initialized");
            }
            // Additional validation logic can be added here
            return true;
        }, "Validate brain state. Returns True if valid.")
        .def("isInitialized", [](Brain& self) {
            // Check if brain has been initialized
            return self.getTotalNeuronCount() > 0;
        }, "Check if brain is initialized. Returns True if brain is ready for simulation.")
        // Memory system access
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Access working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Access episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Access associative memory system")
        // Prediction system access
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Access prediction system")
        // Cognition system access
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Access neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Access concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Access attentional selection system")
        // Development system access
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Access development system")
        // Neuromodulation system access
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Access dopamine system")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Access curiosity system")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Access novelty system")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Access prediction error signal system");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world
        
        The AgentBrain class serves as an intermediary between the computational brain
        and the simulated world. It processes sensory input from the world and
        decodes motor commands for the world. This class manages the interaction
        between the neural processing and environmental interaction.
        
        Methods:
            __init__(brain): Create AgentBrain with underlying brain
            initialize(world): Initialize agent brain with world
            getSensoryInputSize(): Get expected sensory input size
            getMotorOutputSize(): Get expected motor output size
            processSensoryInput(percept): Process sensory percept and inject into brain
            decodeMotorCommand(): Decode brain motor activity into motor command
            applyRewardModulation(reward, predictedReward): Apply reward-based neuromodulation
            updateDevelopment(timestep): Update development system
            getDevelopmentalStage(): Get current developmental stage
            getNeuromodulationLevel(): Get current neuromodulation level
            getCuriosityLevel(): Get curiosity level
            getNoveltyLevel(): Get novelty level
            getPredictionError(): Get prediction error
            reset(): Reset agent for new episode
            getBrain(): Get the underlying brain
            
            # Subsystem enable/disable
            enableRewardModulation(enable): Enable/disable reward modulation
            enableStructuralPlasticity(enable): Enable/disable structural plasticity
            enableDevelopment(enable): Enable/disable development
            enableCuriosity(enable): Enable/disable curiosity
            
            # Status checks
            isRewardModulationEnabled(): Check if reward modulation is enabled
            isStructuralPlasticityEnabled(): Check if structural plasticity is enabled
            isDevelopmentEnabled(): Check if development is enabled
            isCuriosityEnabled(): Check if curiosity is enabled
            
            # Convenience methods
            runEpisode(world, steps): Run complete agent episode
            getPerformanceMetrics(): Get performance metrics
            validate(): Validate agent brain state
        
        Examples:
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> agent = pynlm.createAgentBrain(brain)
            >>> world = pynlm.createSimpleWorld()
            >>> world.configure(20.0, 20.0, 8, 8)
            >>> world.reset()
            >>> agent.initialize(world)
            >>> 
            >>> # Run complete episode
            >>> for step in range(100):
            ...     world.update(0.1)
            ...     agent.processSensoryInput(world.getSensoryPercept())
            ...     brain.step(step)
            ...     action = agent.decodeMotorCommand()
            ...     world.applyMotorCommand(action, world.getSimulationTime())
            ...     
            >>> # Enable learning systems
            >>> agent.enableRewardModulation(True)
            >>> agent.enableCuriosity(True)
            >>> agent.enableDevelopment(True)
        
        Note:
            AgentBrain manages the interface between neural computation and environmental
            interaction. It coordinates sensory processing, motor output, and learning
            systems to create an adaptive agent that can learn from experience.
        )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create AgentBrain with underlying brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize agent brain with world. Returns True on success.")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command. Returns ActionType.")
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
        // Subsystem enable/disable
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             "Enable or disable reward modulation system")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable or disable structural plasticity system")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable or disable development system")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable or disable curiosity system")
        // Status checks
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        // Convenience methods
        .def("runEpisode", [](AgentBrain& self, std::shared_ptr<SimpleWorld> world, int steps) {
            for (int step = 0; step < steps; ++step) {
                world->update(0.1);
                auto percept = world->getSensoryPercept();
                self.processSensoryInput(percept);
                world->applyMotorCommand(self.decodeMotorCommand(), world->getSimulationTime());
            }
            return self.getPredictionError();
        }, py::arg("world"), py::arg("steps"),
             "Run complete agent episode. Returns final prediction error.")
        .def("getPerformanceMetrics", [](AgentBrain& self) {
            std::map<std::string, double> metrics;
            metrics["curiosity_level"] = self.getCuriosityLevel();
            metrics["novelty_level"] = self.getNoveltyLevel();
            metrics["prediction_error"] = self.getPredictionError();
            metrics["neuromodulation_level"] = self.getNeuromodulationLevel();
            return metrics;
        }, "Get performance metrics as a dictionary")
        .def("validate", [](AgentBrain& self) {
            if (!self.getBrain()) {
                throw InitializationError("AgentBrain not properly initialized");
            }
            // Additional validation logic can be added here
            return true;
        }, "Validate agent brain state. Returns True if valid.")
        .def("isInitialized", [](AgentBrain& self) {
            // Check if agent brain has been initialized
            auto brain = self.getBrain();
            return brain && brain->getTotalNeuronCount() > 0;
        }, "Check if agent brain is initialized. Returns True if ready for operation.");

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

    // Utility functions for advanced usage
    m.def("run_agent_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                     std::shared_ptr<Config> config, int num_steps) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        for (int step = 0; step < num_steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            if (step % 100 == 0) {
                // Log progress periodically
                auto stats = brain->getTotalSpikeCount();
                auto firing = brain->getFiringNeuronCount();
                auto curiosity = agent->getCuriosityLevel();
                auto novelty = agent->getNoveltyLevel();
            }
        }
        
        return std::make_tuple(brain->getTotalSpikeCount(), 
                              agent->getCuriosityLevel(),
                              agent->getPredictionError());
    }, py::arg("brain"), py::arg("world"), py::arg("config"), py::arg("num_steps"),
         "Run a complete agent simulation. Returns tuple of (total_spikes, curiosity_level, prediction_error).")

    m.def("benchmark_brain", [](std::shared_ptr<Brain> brain, std::shared_ptr<Config> config, 
                               int num_steps, double timestep) {
        brain->initialize();
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < num_steps; ++step) {
            brain->step(step, timestep * step);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        std::map<std::string, double> results;
        results["steps_performed"] = static_cast<double>(num_steps);
        results["total_time_ms"] = static_cast<double>(duration.count());
        results["steps_per_second"] = (num_steps * 1000.0) / duration.count();
        results["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
        results["average_firing_rate"] = brain->getAverageFiringRate();
        
        return results;
    }, py::arg("brain"), py::arg("config"), py::arg("num_steps"), py::arg("timestep"),
         "Benchmark brain performance. Returns dictionary with timing and performance metrics.")

    m.def("validate_configuration", [](std::shared_ptr<Config> config) {
        bool valid = true;
        std::vector<std::string> errors;
        
        // Check essential configuration parameters
        if (config->has("brain.neuron_count")) {
            auto neuron_count = config->get<int64_t>("brain.neuron_count");
            if (neuron_count <= 0) {
                errors.push_back("brain.neuron_count must be positive");
                valid = false;
            }
        }
        
        if (config->has("brain.synapse_density")) {
            auto density = config->get<double>("brain.synapse_density");
            if (density < 0.0 || density > 1.0) {
                errors.push_back("brain.synapse_density must be between 0.0 and 1.0");
                valid = false;
            }
        }
        
        if (config->has("brain.connection_probability")) {
            auto prob = config->get<double>("brain.connection_probability");
            if (prob < 0.0 || prob > 1.0) {
                errors.push_back("brain.connection_probability must be between 0.0 and 1.0");
                valid = false;
            }
        }
        
        if (!valid) {
            throw ParameterError("Configuration validation failed: " + 
                               (errors.empty() ? "Unknown error" : errors[0]));
        }
        
        return std::make_pair(valid, errors);
    }, py::arg("config"),
         "Validate configuration parameters. Returns tuple of (is_valid, error_messages).")

    m.def("create_test_environment", []() {
        auto config = std::make_shared<Config>();
        
        // Set up reasonable default configuration
        config->set("brain.neuron_count", 500);
        config->set("brain.synapse_density", 0.1);
        config->set("brain.connection_probability", 0.05);
        config->set("brain.v_thresh", -50.0);
        config->set("brain.v_rest", -70.0);
        config->set("brain.v_reset", -75.0);
        config->set("brain.tau_mem", 20.0);
        config->set("brain.tau_ref", 2.0);
        
        // Enable important plasticity mechanisms
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.001);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        
        // Enable neuromodulation
        config->set("neuromod.dopamine.scale", 1.0);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        
        return config;
    }, "Create a test-ready configuration with sensible defaults.")

    m.def("export_brain_stats", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, std::string> stats;
        
        stats["timestamp"] = std::to_string(std::time(nullptr));
        stats["total_neurons"] = std::to_string(brain->getTotalNeuronCount());
        stats["total_synapses"] = std::to_string(brain->getTotalSynapseCount());
        stats["active_neurons"] = std::to_string(brain->getActiveNeuronCount());
        stats["firing_neurons"] = std::to_string(brain->getFiringNeuronCount());
        stats["total_spikes"] = std::to_string(brain->getTotalSpikeCount());
        stats["average_firing_rate"] = std::to_string(brain->getAverageFiringRate());
        stats["excitation_inhibition_ratio"] = std::to_string(brain->getExcitationInhibitionRatio());
        stats["developmental_stage"] = std::to_string(static_cast<int>(brain->getDevelopmentalStage()));
        stats["region_count"] = std::to_string(brain->getRegionCount());
        
        return stats;
    }, py::arg("brain"),
         "Export comprehensive brain statistics as a dictionary.")

    m.def("load_example_config", []() {
        // Load example configuration from file or return defaults
        auto config = std::make_shared<Config>();
        
        // Example configuration that demonstrates good defaults
        config->set("brain.neuron_count", 1000);
        config->set("brain.synapse_density", 0.15);
        config->set("brain.connection_probability", 0.08);
        config->set("brain.initial_weight_mean", 0.5);
        config->set("brain.initial_weight_std", 0.1);
        config->set("brain.v_thresh", -50.0);
        config->set("brain.v_rest", -70.0);
        config->set("brain.v_reset", -75.0);
        config->set("brain.tau_mem", 20.0);
        config->set("brain.tau_ref", 2.0);
        
        // Plasticity settings
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.001);
        config->set("plasticity.stdp.tau_plus", 20.0);
        config->set("plasticity.stdp.tau_minus", 20.0);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.hebbian.strength", 0.01);
        config->set("plasticity.structural.enable", true);
        config->set("plasticity.structural.rate", 0.001);
        
        // Neuromodulation settings
        config->set("neuromod.dopamine.scale", 1.0);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.curiosity.base_rate", 0.1);
        config->set("neuromod.novelty.enable", true);
        config->set("neuromod.novelty.threshold", 0.5);
        
        // Development settings
        config->set("development.initial_stage", "initial");
        config->set("development.enable_plasticity_decay", true);
        config->set("development.plasticity_decay_rate", 0.01);
        
        return config;
    }, "Create an example configuration with well-tuned parameters.")

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Add version information
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Project Authors";
}

} // namespace nlm
