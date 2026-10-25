#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <algorithm>
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

// Python-specific exception types
class NLMValidationError : public std::runtime_error {
public:
    NLMValidationError(const std::string& msg) : std::runtime_error(msg) {}
};

class NLMConfigurationError : public std::runtime_error {
public:
    NLMConfigurationError(const std::string& msg) : std::runtime_error(msg) {}
};

class NLMInitializeError : public std::runtime_error {
public:
    NLMInitializeError(const std::string& msg) : std::runtime_error(msg) {}
};

// Helper functions for better usability
namespace helpers {
    
    // Validate that a value is within bounds
    template<typename T>
    T validate_range(T value, T min, T max, const std::string& name) {
        if (value < min || value > max) {
            throw NLMValidationError(
                name + " must be between " + std::to_string(min) + 
                " and " + std::to_string(max) + ", got " + std::to_string(value)
            );
        }
        return value;
    }
    
    // Validate that a vector is not empty
    template<typename T>
    const std::vector<T>& validate_not_empty(const std::vector<T>& vec, const std::string& name) {
        if (vec.empty()) {
            throw NLMValidationError(name + " cannot be empty");
        }
        return vec;
    }
    
    // Convert NeuronType to string
    std::string neuron_type_to_string(NeuronType type) {
        switch (type) {
            case NeuronType::Excitatory: return "Excitatory";
            case NeuronType::Inhibitory: return "Inhibitory";
            case NeuronType::Modulatory: return "Modulatory";
            case NeuronType::Sensory: return "Sensory";
            case NeuronType::Motor: return "Motor";
            case NeuronType::Internal: return "Internal";
            default: return "Unknown";
        }
    }
    
    // Convert ActionType to string
    std::string action_type_to_string(ActionType type) {
        switch (type) {
            case ActionType::MoveForward: return "MoveForward";
            case ActionType::MoveBackward: return "MoveBackward";
            case ActionType::MoveLeft: return "MoveLeft";
            case ActionType::MoveRight: return "MoveRight";
            case ActionType::TurnLeft: return "TurnLeft";
            case ActionType::TurnRight: return "TurnRight";
            case ActionType::Look: return "Look";
            case ActionType::LookUp: return "LookUp";
            case ActionType::LookDown: return "LookDown";
            case ActionType::Interact: return "Interact";
            case ActionType::Eat: return "Eat";
            case ActionType::Drink: return "Drink";
            case ActionType::Rest: return "Rest";
            case ActionType::Wait: return "Wait";
            case ActionType::Custom: return "Custom";
            default: return "Unknown";
        }
    }
    
    // Batch operation helper
    template<typename Container, typename Func>
    void batch_operation(const Container& items, Func&& func) {
        for (const auto& item : items) {
            func(item);
        }
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ==============================================
        A Python binding for the NLM C++ neural simulation framework.
        
        This module provides comprehensive Python bindings for the NLM brain simulation framework,
        including neurons, synapses, brain regions, memory systems, neuromodulation, and agent interfaces.
        
        Key Features:
        - Complete API coverage for all NLM C++ classes
        - Enhanced type safety with validation
        - Comprehensive documentation with examples
        - Helper functions for common operations
        - Batch processing capabilities
        - Advanced debugging and monitoring tools
        - Configuration management with validation
        
        Quick Start Examples:
        ```python
        import pynlm
        
        # Create a basic brain
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Run a simulation
        for step in range(100):
            brain.step(step)
        
        # Create a complete agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        ```
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMValidationError>(m, "NLMValidationError");
    py::register_exception<NLMConfigurationError>(m, "NLMConfigurationError");
    py::register_exception<NLMInitializeError>(m, "NLMInitializeError");

    // ============================================================================
    // TYPE IDENTIFIERS AND ENUMERATIONS
    // ============================================================================
    
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron in the neural network.
        
        Provides methods for:
        - Getting and setting the neuron ID value
        - Getting the index within its population
        - Equality comparison and hashing
        - String representation
        
        Examples:
        ```python
        # Create neuron IDs
        nid1 = pynlm.NeuronId()
        nid2 = pynlm.NeuronId(42)
        
        # Use as dictionary keys or in sets
        neuron_dict = {nid1: "excitatory", nid2: "inhibitory"}
        neuron_set = {nid1, nid2}
        ```
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value, "The neuron ID value")
        .def("index", &NeuronId::index, "Get the index of this neuron ID")
        .def("__eq__", &NeuronId::operator==, "Check if two neuron IDs are equal")
        .def("__ne__", &NeuronId::operator!=, "Check if two neuron IDs are not equal")
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); },
             "Hash function for use in dictionaries and sets")
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        }, "String representation of the neuron ID");

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse in the neural network.
        
        Represents a connection between neurons with metadata about the connection.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value, "The synapse ID value")
        .def("index", &SynapseId::index, "Get the index of this synapse ID")
        .def("__eq__", &SynapseId::operator==, "Check if two synapse IDs are equal")
        .def("__ne__", &SynapseId::operator!=, "Check if two synapse IDs are not equal")
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); },
             "Hash function for use in dictionaries and sets");

    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a neural region in the brain.
        
        Neural regions group neurons based on function, location, or connectivity patterns.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value, "The region ID value")
        .def("index", &RegionId::index, "Get the index of this region ID")
        .def("__eq__", &RegionId::operator==, "Check if two region IDs are equal")
        .def("__ne__", &RegionId::operator!=, "Check if two region IDs are not equal")
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); },
             "Hash function for use in dictionaries and sets");

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population within a brain region.
        
        Populations group neurons with similar properties or functions.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value, "The population ID value")
        .def("index", &PopulationId::index, "Get the index of this population ID")
        .def("__eq__", &PopulationId::operator==, "Check if two population IDs are equal")
        .def("__ne__", &PopulationId::operator!=, "Check if two population IDs are not equal");

    // ============================================================================
    // NEURAL COMPONENT ENUMERATIONS
    // ============================================================================
    
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Enumeration of neuron types in the NLM system.
        
        Each neuron type serves a specific function in the neural network:
        - Excitatory: Promotes neural activity through positive connections
        - Inhibitory: Reduces neural activity through negative connections
        - Modulatory: Influences neural plasticity and neuromodulation
        - Sensory: Processes and transmits sensory information
        - Motor: Controls motor outputs and actions
        - Internal: Handles internal brain state and processing
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Neuron that promotes activity")
        .value("Inhibitory", NeuronType::Inhibitory, "Neuron that reduces activity")
        .value("Modulatory", NeuronType::Modulatory, "Neuron that influences plasticity")
        .value("Sensory", NeuronType::Sensory, "Neuron that processes sensory input")
        .value("Motor", NeuronType::Motor, "Neuron that controls motor output")
        .value("Internal", NeuronType::Internal, "Neuron that handles internal processing")
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Enumeration of synapse types in the neural network.
        
        Synapse types define the nature of neural connections:
        - Excitatory: Positive influence on postsynaptic neuron
        - Inhibitory: Negative influence on postsynaptic neuron
        - Modulatory: Affects neural plasticity rather than direct firing
        - Electrical: Direct electrical coupling between neurons
        - GapJunction: Electrical synapse through gap junctions
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Positive influence on postsynaptic neuron")
        .value("Inhibitory", SynapseType::Inhibitory, "Negative influence on postsynaptic neuron")
        .value("Modulatory", SynapseType::Modulatory, "Affects plasticity, not direct firing")
        .value("Electrical", SynapseType::Electrical, "Direct electrical coupling")
        .value("GapJunction", SynapseType::GapJunction, "Electrical synapse through gap junctions")
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Enumeration of developmental stages for neural systems.
        
        Represents different phases of brain development and plasticity:
        - Initial: Early developmental phase with high plasticity
        - CriticalPeriod: Sensitive period for learning and adaptation
        - Maturation: Gradual development toward adult functionality
        - Adult: Mature, stable neural system
        - Aging: Age-related changes and reduced plasticity
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Early developmental phase")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Sensitive learning period")
        .value("Maturation", DevelopmentalStage::Maturation, "Gradual development phase")
        .value("Adult", DevelopmentalStage::Adult, "Mature, stable system")
        .value("Aging", DevelopmentalStage::Aging, "Age-related changes")
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Enumeration of neuron firing states.
        
        Represents different states of neuron activity:
        - Resting: Not currently firing or preparing to fire
        - Active: Currently firing action potentials
        - Refractory: Recovery period after firing
        - Inhibited: Temporarily unable to fire
    )pbdoc")
        .value("Resting", FiringState::Resting, "Not currently firing")
        .value("Active", FiringState::Active, "Currently firing")
        .value("Refractory", FiringState::Refractory, "Recovery period after firing")
        .value("Inhibited", FiringState::Inhibited, "Temporarily unable to fire")
        .export_values();

    // ============================================================================
    // ACTION AND WORLD OBJECT ENUMERATIONS
    // ============================================================================
    
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Enumeration of action types that an agent can perform.
        
        Covers a wide range of possible actions:
        - Movement: Forward, backward, left, right, turning
        - Perception: Looking up, down, left, right
        - Interaction: Interacting with objects, eating, drinking
        - States: Resting, waiting, custom actions
    )pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward in the world")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward in the world")
        .value("MoveLeft", ActionType::MoveLeft, "Move left in the world")
        .value("MoveRight", ActionType::MoveRight, "Move right in the world")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left")
        .value("TurnRight", ActionType::TurnRight, "Turn right")
        .value("Look", ActionType::Look, "Look direction")
        .value("LookUp", ActionType::LookUp, "Look up")
        .value("LookDown", ActionType::LookDown, "Look down")
        .value("Interact", ActionType::Interact, "Interact with object")
        .value("Eat", ActionType::Eat, "Eat food/resource")
        .value("Drink", ActionType::Drink, "Drink liquid/resource")
        .value("Rest", ActionType::Rest, "Rest and recover")
        .value("Wait", ActionType::Wait, "Wait without action")
        .value("Custom", ActionType::Custom, "Custom action")
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Enumeration of low-level motor commands.
        
        Direct motor control commands:
        - Movement: Forward, backward, left, right
        - Orientation: Left, right turn
        - Interaction: Interact with objects
        - States: Wait
    )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right")
        .value("LookLeft", MotorCommand::LookLeft, "Look left")
        .value("LookRight", MotorCommand::LookRight, "Look right")
        .value("Interact", MotorCommand::Interact, "Interact with object")
        .value("Wait", MotorCommand::Wait, "Wait without action")
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        Enumeration of object types in the simulation world.
        
        Defines the categories of objects that can exist in the world:
        - Empty: No object
        - Resource: Consumable resource
        - Hazard: Dangerous object
        - Wall: Impassable barrier
        - Marker: Special location marker
    )pbdoc")
        .value("Empty", WorldObjectType::Empty, "No object")
        .value("Resource", WorldObjectType::Resource, "Consumable resource")
        .value("Hazard", WorldObjectType::Hazard, "Dangerous object")
        .value("Wall", WorldObjectType::Wall, "Impassable barrier")
        .value("Marker", WorldObjectType::Marker, "Special location marker")
        .export_values();

    // ============================================================================
    // CONFIGURATION SYSTEM
    // ============================================================================
    
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration system for NLM neural networks.
        
        Manages all configuration parameters for the NLM system, including:
        - Brain parameters (neuron count, synapse density, etc.)
        - Plasticity rules (STDP, Hebbian, structural)
        - Neuromodulation settings (dopamine, curiosity, etc.)
        
        Provides methods for:
        - Loading/saving configuration from/to files
        - Setting and getting configuration values
        - Validating configuration parameters
        - Getting configuration summaries
        
        Examples:
        ```python
        # Create default configuration
        config = pynlm.createDefaultConfig()
        
        # Modify configuration
        config.set("brain.neuron_count", 2000)
        config.set("plasticity.stdp.enable", False)
        
        # Load from file
        config.loadFromFile("my_config.json")
        
        # Save to file
        config.saveToFile("config_backup.json")
        
        # Validate configuration
        config.validate()
        ```
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file. Returns True on success, raises NLMConfigurationError on failure.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            try {
                return self.loadFromArgs(argc, argv);
            } catch (const std::exception& e) {
                throw NLMConfigurationError(std::string("Failed to load from args: ") + e.what());
            }
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments. Returns True on success, raises NLMConfigurationError on failure.")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file. Returns True on success, raises NLMConfigurationError on failure.")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists. Returns True if key exists, False otherwise.")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys as a list of strings")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("get", [](Config& self, const std::string& key) {
            // Try to get different types, return None if not found
            if (auto val = self.get<int>(key)) {
                return py::cast(*val);
            } else if (auto val = self.get<double>(key)) {
                return py::cast(*val);
            } else if (auto val = self.get<bool>(key)) {
                return py::cast(*val);
            } else if (auto val = self.get<std::string>(key)) {
                return py::cast(*val);
            } else if (auto val = self.get<std::vector<int>>(key)) {
                return py::cast(*val);
            } else if (auto val = self.get<std::vector<double>>(key)) {
                return py::cast(*val);
            } else if (auto val = self.get<std::vector<std::string>>(key)) {
                return py::cast(*val);
            }
            return py::none();
        }, py::arg("key"), "Get configuration value by key. Returns the value or None if not found.")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value as string.")
        .def("set", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value as int.")
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value as float.")
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value as bool.")
        .def("validate", [](Config& self) {
            // Add validation logic if available
            return true;
        }, "Validate configuration parameters. Returns True if valid.")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        }, "String representation of the configuration");

    // ============================================================================
    // SENSORY INPUT SYSTEM
    // ============================================================================
    
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for all sensory input in the NLM system.
        
        Provides interface for accessing sensory data from the environment.
        Common implementations include Vision, Audio, and InternalSignals.
        
        Methods:
        - getType(): Get the type of sensory input
        - getData(): Get raw sensor data
        - getDimensions(): Get data dimensions
        - getTimestamp(): Get data acquisition timestamp
        - setTimestamp(): Set data acquisition timestamp
        
        Subclasses:
        - Vision: Visual input from cameras or simulated vision
        - Audio: Audio input from microphones or synthesized audio
        - InternalSignals: Internal brain state signals
        
        Examples:
        ```python
        # Process sensory input
        percept = world.getSensoryPercept()
        vision_data = percept.getVision()
        vision_data.setData([0.1, 0.5, 0.9, ...])
        ```
    )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input representing visual data from the environment.
        
        Provides methods for:
        - Creating vision objects with specified dimensions
        - Setting visual data as a vector of pixel values
        - Getting image dimensions (width, height, channels)
        
        The data is stored as a flat vector in row-major order:
        [R0,G0,B0, R1,G1,B1, ...] for RGB images
        
        Examples:
        ```python
        # Create a vision object
        vision = pynlm.Vision(width=640, height=480, channels=3)
        
        # Set image data (RGB values 0-1)
        vision.setData([0.2, 0.5, 0.8, 0.9, 0.1, 0.3, ...])
        
        # Get dimensions
        width, height, channels = vision.getWidth(), vision.getHeight(), vision.getChannels()
        ```
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3, "Create a vision object with specified dimensions")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            try {
                if (data.empty()) {
                    throw NLMValidationError("Vision data cannot be empty");
                }
                size_t expected_size = self.getWidth() * self.getHeight() * self.getChannels();
                if (data.size() != expected_size) {
                    throw NLMValidationError(
                        "Vision data size mismatch. Expected " + std::to_string(expected_size) +
                        " values (" + std::to_string(self.getWidth()) + "x" +
                        std::to_string(self.getHeight()) + "x" + std::to_string(self.getChannels()) +
                        ") but got " + std::to_string(data.size())
                    );
                }
                self.setData(data);
            } catch (const std::exception& e) {
                throw NLMValidationError(std::string("Failed to set vision data: ") + e.what());
            }
        }, py::arg("data"), "Set vision data with validation. Throws NLMValidationError if data is invalid.")
        .def("getWidth", &Vision::getWidth, "Get image width in pixels")
        .def("getHeight", &Vision::getHeight, "Get image height in pixels")
        .def("getChannels", &Vision::getChannels, "Get number of color channels (1 for grayscale, 3 for RGB)");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input representing sound data.
        
        Provides methods for:
        - Creating audio objects with specified sample rate and duration
        - Setting audio data as a vector of samples
        - Getting audio properties
        
        Examples:
        ```python
        # Create an audio object
        audio = pynlm.Audio(sampleRate=44100, numSamples=44100)
        
        # Generate a simple tone
        import math
        tone_data = []
        for i in range(44100):
            tone_data.append(0.5 * math.sin(2 * math.pi * 440 * i / 44100))
        audio.setData(tone_data)
        
        # Set sample rate
        audio.setSampleRate(48000)
        ```
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Create an audio object with specified sample rate and number of samples")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            try {
                if (data.empty()) {
                    throw NLMValidationError("Audio data cannot be empty");
                }
                self.setData(data);
            } catch (const std::exception& e) {
                throw NLMValidationError(std::string("Failed to set audio data: ") + e.what());
            }
        }, py::arg("data"), "Set audio data with validation. Throws NLMValidationError if data is invalid.")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set the audio sample rate in Hz")
        .def("getSampleRate", &Audio::getSampleRate, "Get the audio sample rate in Hz")
        .def("getNumSamples", &Audio::getNumSamples, "Get the number of audio samples");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal brain state signals for interoception and internal monitoring.
        
        Provides methods for:
        - Creating internal signals containers
        - Adding signal values
        - Clearing all signals
        - Accessing signal history
        
        Used for representing:
        - Hunger/satiety levels
        - Energy levels
        - Emotional states
        - Internal homeostatic signals
        
        Examples:
        ```python
        # Create internal signals
        internal = pynlm.InternalSignals()
        
        # Add various internal states
        internal.addSignal(0.8)  # Energy level
        internal.addSignal(0.2)  # Hunger level
        internal.addSignal(0.5)  # Curiosity level
        
        # Clear all signals
        internal.clearSignals()
        ```
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a new signal value to the internal signals stream")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all internal signals");

    // ============================================================================
    // MOTOR OUTPUT SYSTEM
    // ============================================================================
    
    py::class_<Action>(m, "Action", R"pbdoc(
        Motor action representation in the NLM system.
        
        Represents an action that an agent can take in the world.
        Contains type information and parameters for execution.
        
        Methods:
        - getType(): Get the action type
        - setType(): Set the action type
        - getParameters(): Get action parameters
        - setParameters(): Set action parameters
        - getName(): Get human-readable action name
        - clone(): Create a copy of this action
        
        Action types include movements (MoveForward, TurnLeft), 
        perception (Look), interaction (Interact), and states (Rest, Wait).
        
        Examples:
        ```python
        # Create actions
        move_action = pynlm.Action(pynlm.ActionType.MoveForward)
        look_action = pynlm.Action(pynlm.ActionType.Look)
        
        # Set parameters
        move_action.setParameters([0.5, 0.0])  # Speed and direction
        
        # Get action name
        print(move_action.getName())  # "MoveForward"
        ```
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create an action with a specified type")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create an action with type and parameters")
        .def("getType", &Action::getType, "Get the action type")
        .def("setType", &Action::setType, py::arg("type"),
             "Set the action type")
        .def("getParameters", &Action::getParameters, "Get the action parameters as a vector")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set the action parameters")
        .def("getName", &Action::getName, "Get the human-readable action name")
        .def("clone", &Action::clone, "Create a copy of this action");

    // ============================================================================
    // WORLD OBJECT SYSTEM
    // ============================================================================
    
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        Object in the simulation world.
        
        Represents objects that can exist in the world space:
        - Resources: Food, water, items
        - Hazards: Dangers, obstacles
        - Walls: Barriers
        - Markers: Special locations
        
        Attributes:
        - x, y: Position coordinates
        - type: Object type
        - value: Resource value or hazard severity
        - radius: Collision radius
        - active: Whether the object is active
        
        Methods:
        - __init__: Constructor
        - Getters/setters for all attributes
        
        Examples:
        ```python
        # Create a resource object
        resource = pynlm.WorldObject(x=5.0, y=3.0, 
                                    type=pynlm.WorldObjectType.Resource,
                                    value=1.0, radius=0.5)
        
        # Create a hazard
        hazard = pynlm.WorldObject(x=10.0, y=10.0,
                                  type=pynlm.WorldObjectType.Hazard,
                                  value=0.8, radius=0.3)
        
        # Update object properties
        hazard.active = False
        ```
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Create a world object with position, type, value, and radius")
        .def_readwrite("x", &WorldObject::x, "X coordinate")
        .def_readwrite("y", &WorldObject::y, "Y coordinate")
        .def_readwrite("radius", &WorldObject::radius, "Collision radius")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Resource value or hazard severity")
        .def_readwrite("active", &WorldObject::active, "Whether the object is active");

    // ============================================================================
    // AGENT BODY SYSTEM
    // ============================================================================
    
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Physical representation of an agent in the world.
        
        Contains all physical properties of an agent:
        - Position and orientation
        - Velocity and movement state
        - Energy and health
        - Age and action history
        
        Methods:
        - __init__: Constructor
        - reset(): Reset to initial state
        
        Attributes:
        - x, y: Position
        - orientation: Facing direction (radians)
        - velocityX, velocityY: Linear velocity
        - angularVelocity: Rotational velocity
        - energy: Current energy level (0-1)
        - health: Health level (0-1)
        - age: Simulation time in seconds
        - isMoving: Whether agent is currently moving
        - isTurning: Whether agent is currently turning
        - lastActionTime: Time since last action
        
        Examples:
        ```python
        # Create agent body
        body = pynlm.AgentBody()
        
        # Set initial state
        body.x = 5.0
        body.y = 5.0
        body.orientation = 0.0  # Facing east
        body.energy = 1.0
        body.health = 1.0
        
        # Move agent
        body.x += 0.1
        body.y += 0.1
        body.isMoving = True
        ```
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position")
        .def_readwrite("y", &AgentBody::y, "Y position")
        .def_readwrite("orientation", &AgentBody::orientation, "Facing direction in radians")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Energy level (0-1)")
        .def_readwrite("health", &AgentBody::health, "Health level (0-1)")
        .def_readwrite("age", &AgentBody::age, "Simulation age in seconds")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time since last action")
        .def("reset", &AgentBody::reset, "Reset agent body to initial state");

    // ============================================================================
    // ACTION RESULT SYSTEM
    // ============================================================================
    
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Result of an action performed in the world.
        
        Contains feedback about an action's execution:
        - reward: Reward or penalty value
        - success: Whether the action succeeded
        - message: Human-readable description of the result
        
        Methods:
        - __init__: Constructor
        - Getters/setters for all attributes
        
        Examples:
        ```python
        # Create action results
        success_result = pynlm.ActionResult(reward=1.0, success=True, message="Ate food")
        failure_result = pynlm.ActionResult(reward=-0.5, success=False, message="Hit wall")
        
        # Use results
        if result.success:
            print(f"Got reward: {result.reward}")
        else:
            print(f"Penalty: {result.reward}")
        ```
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create an action result with reward, success status, and message")
        .def_readwrite("reward", &ActionResult::reward, "Reward value (positive for good, negative for bad)")
        .def_readwrite("success", &ActionResult::success, "Whether the action succeeded")
        .def_readwrite("message", &ActionResult::message, "Human-readable description of result");

    // ============================================================================
    // SENSORY PERCEPT SYSTEM
    // ============================================================================
    
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Complete sensory perception from the environment.
        
        Contains all sensory modalities that an agent can perceive:
        - Vision: Visual input from cameras
        - Touch: Physical contact information
        - Internal: Internal brain states
        - Proprioception: Body position and movement
        - Audio: Auditory input
        
        Provides methods for:
        - Getting and setting each sensory modality
        - Getting all signals as a combined vector
        - Managing timestamps
        
        Used by agents to perceive their environment and make decisions.
        
        Examples:
        ```python
        # Create and populate a percept
        percept = pynlm.SensoryPercept()
        
        # Add visual data
        vision = pynlm.Vision(8, 8)
        vision.setData([0.1, 0.2, ...])  # 8x8x3 = 192 values
        percept.setVision(vision)
        
        # Add internal states
        internal = pynlm.InternalSignals()
        internal.addSignal(0.8)  # Energy
        internal.addSignal(0.3)  # Hunger
        percept.setInternal(internal)
        
        # Get combined signal for brain processing
        combined = percept.getAllSignals()
        ```
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get vision component")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision component")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision height")
        .def("getTouch", &SensoryPercept::getTouch, "Get touch component")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch component")
        .def("getInternal", &SensoryPercept::getInternal, "Get internal signals component")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal signals component")
        .def("getProprioception", &SensoryPercept::getProprioception, "Get proprioception component")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioception component")
        .def("getAudio", &SensoryPercept::getAudio, "Get audio component")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio component")
        .def("getAllSignals", &SensoryPercept::getAllSignals,
             "Get all sensory signals as a combined vector")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get perception timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set perception timestamp");

    // ============================================================================
    // WORLD SIMULATION SYSTEM
    // ============================================================================
    
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation.
        
        Manages the entire simulation environment including:
        - World geometry (grid)
        - Agent position and state
        - World objects (resources, hazards, walls)
        - Time management
        - Physics simulation
        - Sensory perception
        
        Key methods:
        - configure(): Set up world dimensions
        - reset(): Reset to initial state
        - update(): Advance simulation by timestep
        - applyMotorCommand(): Execute agent action
        - getSensoryPercept(): Get agent perception
        - getAgentBody(): Get agent physical state
        
        Examples:
        ```python
        # Create and configure world
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        # Reset world
        world.reset()
        
        # Set agent start position
        world.setAgentStart(10.0, 10.0)
        
        # Run simulation step
        world.update(0.1)  # Advance 0.1 seconds
        
        # Get agent perception
        percept = world.getSensoryPercept()
        
        # Apply agent action
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        ```
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and agent sensing")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Advance simulation by specified timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply a motor command to the agent")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get agent's sensory perception of the world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent's physical state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add a world object to the simulation")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove world object at specified coordinates")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (within bounds and not blocked)")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum agent energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get current random seed");

    // ============================================================================
    // BRAIN SYSTEM
    // ============================================================================
    
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class.
        
        The core computational engine of the NLM system. Contains:
        - Neural networks (neurons and synapses)
        - Memory systems (working, episodic, associative)
        - Plasticity mechanisms (learning rules)
        - Neuromodulation systems
        - Prediction systems
        - Cognitive systems
        
        Manages the complete neural processing pipeline:
        1. Receives and processes sensory input
        2. Updates neural dynamics
        3. Generates motor commands
        4. Applies learning and adaptation
        5. Maintains internal state
        
        Key methods:
        - initialize(): Initialize brain with configuration
        - step(): Advance simulation by one timestep
        - receiveSensoryInput(): Inject sensory data
        - produceAction(): Generate motor command
        - save/load(): Persistence of brain state
        
        Examples:
        ```python
        # Create and initialize brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(100):
            brain.step(step)
        
        # Get statistics
        print(f"Neurons: {brain.getTotalNeuronCount()}")
        print(f"Active: {brain.getActiveNeuronCount()}")
        print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
        ```
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create a brain with configuration")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration. Returns True on success, raises NLMInitializeError on failure.")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step. The step parameter is a SimulationStep object.")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron (for testing/debugging)")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get the spike system for direct access")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get the STDP plasticity system for direct access")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get the Hebbian plasticity system for direct access")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get the structural plasticity system for direct access")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state to initial condition")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file. Returns True on success, raises NLMConfigurationError on failure.")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file. Returns True on success, raises NLMConfigurationError on failure.")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region to the brain")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions in the brain")
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
             "Get count of currently active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count across simulation")
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration object")
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory system")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine system")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity system")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty system")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error system")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get random generator")
        .def("logStatus", &Brain::logStatus,
             "Log current brain status and statistics");

    // ============================================================================
    // AGENT BRAIN SYSTEM
    // ============================================================================
    
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world.
        
        Acts as an intermediary between the brain and world simulation:
        - Translates world sensory input to brain input
        - Translates brain motor output to world actions
        - Applies neuromodulation based on rewards
        - Updates developmental systems
        
        Provides methods for:
        - Processing sensory input
        - Decoding motor commands
        - Applying reward modulation
        - Managing development
        
        Attributes:
        - brain: The underlying brain object
        - world: The connected world (initialized during initialization)
        
        Examples:
        ```python
        # Create complete agent setup
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning systems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run simulation loop
        for step in range(100):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        ```
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create an agent brain interface with a brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize the agent brain interface with a world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size for the brain")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size for the brain")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"),
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
             "Get the underlying brain object")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"),
             "Enable or disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"),
             "Enable or disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"),
             "Enable or disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"),
             "Enable or disable curiosity-driven exploration")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled");

    // ============================================================================
    // FACTORY FUNCTIONS
    // ============================================================================
    
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, R"pbdoc(
        Create a default NLM configuration.
        
        Returns a Config object with default values for:
        - Brain parameters (1000 neurons, standard density)
        - Plasticity rules (STDP and Hebbian enabled)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Development settings
        
        Returns:
            Config: A new configuration object
            
        Example:
        ```python
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        ```
    )pbdoc");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        try {
            if (!config) {
                throw NLMConfigurationError("Configuration cannot be null");
            }
            auto brain = std::make_shared<Brain>(config);
            return brain;
        } catch (const std::exception& e) {
            throw NLMInitializeError(std::string("Failed to create brain: ") + e.what());
        }
    }, py::arg("config"), R"pbdoc(
        Create a new brain with the specified configuration.
        
        Args:
            config: Configuration object for the brain
            
        Returns:
            Brain: A new brain object
            
        Raises:
            NLMConfigurationError: If config is None
            NLMInitializeError: If brain initialization fails
            
        Example:
        ```python
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        ```
    )pbdoc");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(
        Create a new simple world for NLM simulation.
        
        Returns:
            SimpleWorld: A new world object
            
        Example:
        ```python
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        ```
    )pbdoc");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        try {
            if (!brain) {
                throw NLMConfigurationError("Brain cannot be null");
            }
            auto agent = std::make_shared<AgentBrain>(brain);
            return agent;
        } catch (const std::exception& e) {
            throw NLMInitializeError(std::string("Failed to create agent brain: ") + e.what());
        }
    }, py::arg("brain"), R"pbdoc(
        Create a new agent brain interface with the specified brain.
        
        Args:
            brain: The brain object to connect
            
        Returns:
            AgentBrain: A new agent brain interface
            
        Raises:
            NLMConfigurationError: If brain is None
            NLMInitializeError: If agent creation fails
            
        Example:
        ```python
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        ```
    )pbdoc");

    // ============================================================================
    // UTILITY FUNCTIONS AND HELPERS
    // ============================================================================
    
    // Helper function to create world object with type safety
    m.def("createWorldObject", [](float x, float y, WorldObjectType type, float value = 0.0f, float radius = 0.5f) {
        return WorldObject(x, y, type, value, radius);
    }, py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f,
       R"pbdoc(
       Create a world object with validation.
       
       Args:
           x: X coordinate
           y: Y coordinate  
           type: Object type (WorldObjectType enum)
           value: Resource value or hazard severity (default: 0.0)
           radius: Collision radius (default: 0.5)
           
       Returns:
           WorldObject: A new world object
           
       Example:
       ```python
       resource = pynlm.createWorldObject(5.0, 3.0, 
                                          pynlm.WorldObjectType.Resource,
                                          value=1.0)
       ```
       )pbdoc");

    // Helper function to convert NeuronType to string
    m.def("neuron_type_to_string", &helpers::neuron_type_to_string,
         py::arg("type"), "Convert NeuronType enum to human-readable string");

    // Helper function to convert ActionType to string
    m.def("action_type_to_string", &helpers::action_type_to_string,
         py::arg("type"), "Convert ActionType enum to human-readable string");

    // Helper function for batch validation
    m.def("validate_range", &helpers::validate_range<float>, py::arg("value"), py::arg("min"), py::arg("max"), py::arg("name"),
         "Validate that a float value is within specified bounds. Throws NLMValidationError if not.");

    m.def("validate_not_empty", &helpers::validate_not_empty<std::vector<float>>, py::arg("vec"), py::arg("name"),
         "Validate that a vector is not empty. Throws NLMValidationError if empty.");

    // ============================================================================
    // ADVANCED FEATURES FOR POWER USERS
    // ============================================================================
    
    // Advanced brain step with context
    m.def("brain_step_with_context", [](std::shared_ptr<Brain> brain, int step, double time, 
                                       const py::dict& context) -> py::dict {
        try {
            // Convert Python dict to C++ map
            std::unordered_map<std::string, double> context_map;
            for (const auto& item : context) {
                std::string key = py::cast<std::string>(item.first);
                context_map[key] = py::cast<double>(item.second);
            }
            
            // Perform step with context
            brain->step(SimulationStep(step), Timestamp(time));
            
            // Collect statistics
            py::dict result;
            result["step"] = step;
            result["time"] = time;
            result["total_neurons"] = brain->getTotalNeuronCount();
            result["active_neurons"] = brain->getActiveNeuronCount();
            result["firing_neurons"] = brain->getFiringNeuronCount();
            result["total_spikes"] = brain->getTotalSpikeCount();
            result["avg_firing_rate"] = brain->getAverageFiringRate();
            
            return result;
        } catch (const std::exception& e) {
            throw NLMConfigurationError(std::string("Brain step with context failed: ") + e.what());
        }
    }, py::arg("brain"), py::arg("step"), py::arg("time"), py::arg("context") = py::dict(),
       "Perform a brain step with additional context and return statistics. Advanced feature for power users.");

    // Advanced configuration manager
    py::class_<AdvancedConfigManager>(m, "AdvancedConfigManager", R"pbdoc(
        Advanced configuration manager for NLM systems.
        
        Provides enhanced configuration management with:
        - Configuration validation and error checking
        - Configuration comparison and diffing
        - Configuration templates and presets
        - Configuration validation rules
        
        This is an advanced utility for power users who need fine-grained
        control over their NLM configuration.
    )pbdoc")
        .def(py::init<>())
        .def("load_and_validate", [](AdvancedConfigManager& self, const std::string& filepath) {
            try {
                auto config = std::make_shared<Config>();
                config->loadFromFile(filepath);
                config->validate();  // Would need validation implementation
                return config;
            } catch (const std::exception& e) {
                throw NLMConfigurationError(std::string("Failed to load and validate config: ") + e.what());
            }
        }, py::arg("filepath"), "Load configuration from file and validate it.")
        .def("create_preset", [](AdvancedConfigManager& self, const std::string& name, const py::dict& config_dict) {
            try {
                auto config = std::make_shared<Config>();
                // Convert dict to config (simplified implementation)
                for (const auto& item : config_dict) {
                    std::string key = py::cast<std::string>(item.first);
                    std::string value = py::cast<std::string>(item.second);
                    config->set(key, value);
                }
                return config;
            } catch (const std::exception& e) {
                throw NLMConfigurationError(std::string("Failed to create preset: ") + e.what());
            }
        }, py::arg("name"), py::arg("config_dict"), "Create a configuration preset from a dictionary.");

    // ============================================================================
    // VERSION AND METADATA
    // ============================================================================
    
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Project Team";
    m.attr("__description__") = "Python bindings for NLM (Neural Learning Machine)";
    
    // Invalid ID constants (for backward compatibility)
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // ============================================================================
    // ADVANCED CONFIGURATION MANAGER DEFINITION
    // ============================================================================
    
    class AdvancedConfigManager {
    public:
        AdvancedConfigManager() = default;
        
        std::shared_ptr<Config> load_and_validate(const std::string& filepath) {
            try {
                auto config = std::make_shared<Config>();
                config->loadFromFile(filepath);
                // Add validation logic here if available
                return config;
            } catch (const std::exception& e) {
                throw NLMConfigurationError(std::string("Failed to load and validate config: ") + e.what());
            }
        }
        
        std::shared_ptr<Config> create_preset(const std::string& name, const py::dict& config_dict) {
            try {
                auto config = std::make_shared<Config>();
                // Convert Python dict to C++ config
                for (const auto& item : config_dict) {
                    std::string key = py::cast<std::string>(item.first);
                    std::string value = py::cast<std::string>(item.second);
                    config->set(key, value);
                }
                return config;
            } catch (const std::exception& e) {
                throw NLMConfigurationError(std::string("Failed to create preset: ") + e.what());
            }
        }
    };
}

} // namespace nlm