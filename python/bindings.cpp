// Python bindings improvements for better usability
#include "BrainStepOrchestrator.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/agent/AgentBody.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

namespace py = pybind11;
namespace nlm {

// Enhanced Python bindings with better API design and usability improvements

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Key improvements:
        - Pythonic API design with type hints and better naming
        - Enhanced error handling and validation
        - Convenience methods for common operations
        - Better documentation strings
        - Support for both numpy and native Python arrays
        - Integration with Python's memory management
    )pbdoc";

    // Register custom exception for NLM errors
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // === ID TYPES ===
    
    // NeuronId with Pythonic interface
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron.
        
        This class provides a simple identifier for neurons with methods
        for comparison, hashing, and string representation suitable for Python use.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index, "Get the index part of the neuron ID")
        .def("__eq__", &NeuronId::operator==, "Check equality with another NeuronId")
        .def("__ne__", &NeuronId::operator!=, "Check inequality with another NeuronId")
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    // SynapseId with Pythonic interface
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse.
        
        This class provides a simple identifier for synapses with methods
        for comparison, hashing, and string representation suitable for Python use.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index, "Get the index part of the synapse ID")
        .def("__eq__", &SynapseId::operator==, "Check equality with another SynapseId")
        .def("__ne__", &SynapseId::operator!=, "Check inequality with another SynapseId")
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        });

    // RegionId with Pythonic interface
    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region.
        
        This class provides a simple identifier for brain regions with methods
        for comparison, hashing, and string representation suitable for Python use.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index, "Get the index part of the region ID")
        .def("__eq__", &RegionId::operator==, "Check equality with another RegionId")
        .def("__ne__", &RegionId::operator!=, "Check inequality with another RegionId")
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

    // PopulationId with Pythonic interface
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population.
        
        This class provides a simple identifier for neuron populations with methods
        for comparison, hashing, and string representation suitable for Python use.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index, "Get the index part of the population ID")
        .def("__eq__", &PopulationId::operator==, "Check equality with another PopulationId")
        .def("__ne__", &PopulationId::operator!=, "Check inequality with another PopulationId");

    // === ENUMERATIONS ===
    
    // NeuronType with Python-friendly names
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Types of neurons in the NLM brain model.
        
        These types determine how neurons behave and interact with each other.
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Excitatory neurons that promote firing")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neurons that suppress firing")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neurons that influence plasticity")
        .value("Sensory", NeuronType::Sensory, "Sensory neurons that receive external input")
        .value("Motor", NeuronType::Motor, "Motor neurons that control actions")
        .value("Internal", NeuronType::Internal, "Internal neurons for internal processing")
        .export_values();

    // SynapseType with Python-friendly names
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Types of synapses in the NLM brain model.
        
        These types determine how synaptic connections function.
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory synapses that increase firing")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synapses that decrease firing")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synapses that influence plasticity")
        .value("Electrical", SynapseType::Electrical, "Electrical synapses that allow direct current flow")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction synapses for electrical coupling")
        .export_values();

    // DevelopmentalStage with Python-friendly names
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Developmental stages of the NLM brain.
        
        These stages influence neural development, plasticity, and learning capabilities.
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period for learning")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation phase")
        .value("Adult", DevelopmentalStage::Adult, "Adult developmental stage")
        .value("Aging", DevelopmentalStage::Aging, "Aging phase")
        .export_values();

    // FiringState with Python-friendly names
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        States of neuron firing in the NLM brain model.
        
        These states track the current activity state of neurons.
    )pbdoc")
        .value("Resting", FiringState::Resting, "Neuron is at rest (not firing)")
        .value("Active", FiringState::Active, "Neuron is active (ready to fire)")
        .value("Refractory", FiringState::Refractory, "Neuron is in refractory period (cannot fire)")
        .value("Inhibited", FiringState::Inhibited, "Neuron is inhibited (suppressed)")
        .export_values();

    // ActionType with Python-friendly names
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Types of actions that can be performed by the agent.
        
        These actions represent possible behaviors in the environment.
    )pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward in the environment")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward in the environment")
        .value("MoveLeft", ActionType::MoveLeft, "Move left in the environment")
        .value("MoveRight", ActionType::MoveRight, "Move right in the environment")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left in the environment")
        .value("TurnRight", ActionType::TurnRight, "Turn right in the environment")
        .value("Look", ActionType::Look, "Look in current direction")
        .value("LookUp", ActionType::LookUp, "Look upward")
        .value("LookDown", ActionType::LookDown, "Look downward")
        .value("Interact", ActionType::Interact, "Interact with environment object")
        .value("Eat", ActionType::Eat, "Eat object in environment")
        .value("Drink", ActionType::Drink, "Drink from object in environment")
        .value("Rest", ActionType::Rest, "Rest (do nothing)")
        .value("Wait", ActionType::Wait, "Wait for next step")
        .value("Custom", ActionType::Custom, "Custom action (user-defined)")
        .export_values();

    // MotorCommand with Python-friendly names
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Low-level motor commands for agent control.
        
        These commands are used for direct motor neuron control.
    )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right")
        .value("LookLeft", MotorCommand::LookLeft, "Look left")
        .value("LookRight", MotorCommand::LookRight, "Look right")
        .value("Interact", MotorCommand::Interact, "Interact with environment")
        .value("Wait", MotorCommand::Wait, "Wait/do nothing")
        .export_values();

    // WorldObjectType with Python-friendly names
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        Types of objects that can exist in the world.
        
        These objects represent elements in the NLM environment.
    )pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty space (no object)")
        .value("Resource", WorldObjectType::Resource, "Resource object (e.g., food, water)")
        .value("Hazard", WorldObjectType::Hazard, "Hazard object (e.g., obstacle, danger)")
        .value("Wall", WorldObjectType::Wall, "Wall or barrier")
        .value("Marker", WorldObjectType::Marker, "Marker or target")
        .export_values();

    // === CONFIGURATION ===
    
    // Enhanced Config class with Python-friendly interface
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for the NLM system.
        
        This class provides access to NLM simulation parameters and settings.
        It supports loading from JSON files, command line arguments, and programmatic setup.
        
        Key features:
        - Type-safe access to configuration values
        - Default values for all parameters
        - Support for both getter and setter operations
        - Validation of configuration values
        - Easy conversion to/from JSON
    )pbdoc")
        .def(py::init<>())
        
        // File-based operations
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        
        // Command line operations
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments")
        
        // Configuration access methods
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        
        // Enhanced type-safe getters with better Python integration
        .def("get_int", [](Config& self, const std::string& key) {
            auto opt = self.get<int>(key);
            if (opt) return *opt;
            throw py::value_error("Configuration key not found or has wrong type: " + key);
        }, py::arg("key"), "Get integer value (throws if key doesn't exist)")
        
        .def("get_float", [](Config& self, const std::string& key) {
            auto opt = self.get<float>(key);
            if (opt) return *opt;
            throw py::value_error("Configuration key not found or has wrong type: " + key);
        }, py::arg("key"), "Get float value (throws if key doesn't exist)")
        
        .def("get_string", [](Config& self, const std::string& key) {
            auto opt = self.get<std::string>(key);
            if (opt) return *opt;
            throw py::value_error("Configuration key not found or has wrong type: " + key);
        }, py::arg("key"), "Get string value (throws if key doesn't exist)")
        
        .def("get_bool", [](Config& self, const std::string& key) {
            auto opt = self.get<bool>(key);
            if (opt) return *opt;
            throw py::value_error("Configuration key not found or has wrong type: " + key);
        }, py::arg("key"), "Get boolean value (throws if key doesn't exist)")
        
        // Type-safe getters with default values
        .def("get_int_or", [](Config& self, const std::string& key, int default_value) {
            return self.getOr<int>(key, default_value);
        }, py::arg("key"), py::arg("default_value"), "Get integer value or default if key doesn't exist")
        
        .def("get_float_or", [](Config& self, const std::string& key, float default_value) {
            return self.getOr<float>(key, default_value);
        }, py::arg("key"), py::arg("default_value"), "Get float value or default if key doesn't exist")
        
        .def("get_string_or", [](Config& self, const std::string& key, const std::string& default_value) {
            return self.getOr<std::string>(key, default_value);
        }, py::arg("key"), py::arg("default_value"), "Get string value or default if key doesn't exist")
        
        .def("get_bool_or", [](Config& self, const std::string& key, bool default_value) {
            return self.getOr<bool>(key, default_value);
        }, py::arg("key"), py::arg("default_value"), "Get boolean value or default if key doesn't exist")
        
        // Set methods for configuration
        .def("set_int", [](Config& self, const std::string& key, int value) {
            self.set<int>(key, value);
        }, py::arg("key"), py::arg("value"), "Set integer value")
        
        .def("set_float", [](Config& self, const std::string& key, float value) {
            self.set<float>(key, value);
        }, py::arg("key"), py::arg("value"), "Set float value")
        
        .def("set_string", [](Config& self, const std::string& key, const std::string& value) {
            self.set<std::string>(key, value);
        }, py::arg("key"), py::arg("value"), "Set string value")
        
        .def("set_bool", [](Config& self, const std::string& key, bool value) {
            self.set<bool>(key, value);
        }, py::arg("key"), py::arg("value"), "Set boolean value")
        
        // Python representation
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // === SENSORY INPUTS ===
    
    // Base SensoryInput class with Python-friendly interface
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for all sensory input in the NLM system.
        
        This abstract base class provides common interface for sensory data
        such as vision, audio, and internal signals.
    )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision class with Python-friendly interface
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input for visual processing.
        
        This class represents visual input from the environment, typically
        as a 2D array of pixel values (grayscale or color).
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set vision data from a vector of floats")
        .def("getWidth", &Vision::getWidth, "Get the width of the vision data")
        .def("getHeight", &Vision::getHeight, "Get the height of the vision data")
        .def("getChannels", &Vision::getChannels, "Get the number of color channels")
        
        // Convenience methods for Python users
        .def("getShape", [](const Vision& self) {
            return std::make_tuple(self.getWidth(), self.getHeight(), self.getChannels());
        }, "Get the shape of the vision data as (width, height, channels)")
        .def("getNumPixels", [](const Vision& self) {
            return self.getWidth() * self.getHeight();
        }, "Get the total number of pixels");

    // Audio class with Python-friendly interface
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input for auditory processing.
        
        This class represents audio input from the environment, typically
        as a time series of sound samples.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data from a vector of floats")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"), "Set the sample rate")
        .def("getSampleRate", &Audio::getSampleRate, "Get the sample rate")
        .def("getNumSamples", &Audio::getNumSamples, "Get the number of audio samples")
        
        // Convenience methods for Python users
        .def("getDuration", [](const Audio& self) {
            return self.getNumSamples() / static_cast<float>(self.getSampleRate());
        }, "Get the duration of the audio in seconds");

    // InternalSignals class with Python-friendly interface
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal signals sensory input for neural state monitoring.
        
        This class represents internal neural signals such as firing rates,
        membrane potentials, and other internal state variables.
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"), "Add a signal value")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all signals")
        .def("getSignalCount", &InternalSignals::getData, "Get the number of signals");

    // === ACTIONS ===
    
    // Action class with Python-friendly interface
    py::class_<Action>(m, "Action", R"pbdoc(
        Action representation for motor output.
        
        This class represents actions that can be performed by the agent
        in the environment, including both movement and interaction actions.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType, "Get the action type")
        .def("setType", &Action::setType, py::arg("type"), "Set the action type")
        .def("getParameters", &Action::getParameters, "Get the action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set the action parameters")
        .def("getName", &Action::getName, "Get the human-readable name of the action")
        .def("clone", &Action::clone, "Create a copy of this action")
        
        // Convenience methods for Python users
        .def("isMovement", [](const Action& self) {
            return self.getType() == ActionType::MoveForward ||
                   self.getType() == ActionType::MoveBackward ||
                   self.getType() == ActionType::MoveLeft ||
                   self.getType() == ActionType::MoveRight ||
                   self.getType() == ActionType::TurnLeft ||
                   self.getType() == ActionType::TurnRight;
        }, "Check if action is a movement action")
        
        .def("isInteraction", [](const Action& self) {
            return self.getType() == ActionType::Interact ||
                   self.getType() == ActionType::Eat ||
                   self.getType() == ActionType::Drink;
        }, "Check if action is an interaction action")
        
        .def("isLooking", [](const Action& self) {
            return self.getType() == ActionType::Look ||
                   self.getType() == ActionType::LookUp ||
                   self.getType() == ActionType::LookDown;
        }, "Check if action is a looking action")
        
        .def("isResting", [](const Action& self) {
            return self.getType() == ActionType::Rest ||
                   self.getType() == ActionType::Wait;
        }, "Check if action is a resting action")
        
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + ">";
        });

    // === WORLD OBJECTS ===
    
    // WorldObject class with Python-friendly interface
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        World object representation in the NLM environment.
        
        This class represents objects that can exist in the NLM world,
        such as resources, hazards, walls, and markers.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x, "X position of the object")
        .def_readwrite("y", &WorldObject::y, "Y position of the object")
        .def_readwrite("radius", &WorldObject::radius, "Radius of the object")
        .def_readwrite("type", &WorldObject::type, "Type of the object")
        .def_readwrite("value", &WorldObject::value, "Value of the object (e.g., food quantity)")
        .def_readwrite("active", &WorldObject::active, "Whether the object is active")
        
        // Convenience methods for Python users
        .def("getPosition", [](const WorldObject& obj) {
            return std::make_pair(obj.x, obj.y);
        }, "Get the position as (x, y)")
        .def("setPosition", [](WorldObject& obj, float x, float y) {
            obj.x = x;
            obj.y = y;
        }, "Set the position")
        .def("isActive", &WorldObject::active, "Check if the object is active")
        .def("activate", [](WorldObject& obj) {
            obj.active = true;
        }, "Activate the object")
        .def("deactivate", [](WorldObject& obj) {
            obj.active = false;
        }, "Deactivate the object")
        .def("getTypeName", [](const WorldObject& obj) {
            // Map WorldObjectType to string names
            switch (obj.type) {
                case WorldObjectType::Empty: return "Empty";
                case WorldObjectType::Resource: return "Resource";
                case WorldObjectType::Hazard: return "Hazard";
                case WorldObjectType::Wall: return "Wall";
                case WorldObjectType::Marker: return "Marker";
                default: return "Unknown";
            }
        }, "Get the human-readable name of the object type");

    // === AGENT SYSTEMS ===
    
    // AgentBody class with Python-friendly interface
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state representation.
        
        This class represents the physical state of the agent in the NLM world,
        including position, orientation, velocity, and other body-related variables.
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X coordinate of the agent")
        .def_readwrite("y", &AgentBody::y, "Y coordinate of the agent")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation angle (radians)")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Energy level (0.0 to 1.0)")
        .def_readwrite("health", &AgentBody::health, "Health level (0.0 to 1.0)")
        .def_readwrite("age", &AgentBody::age, "Age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether the agent is moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether the agent is turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time since last action")
        .def("reset", &AgentBody::reset, "Reset the agent body to initial state")
        
        // Convenience methods for Python users
        .def("getPosition", [](const AgentBody& body) {
            return std::make_pair(body.x, body.y);
        }, "Get position as (x, y)")
        .def("setPosition", [](AgentBody& body, float x, float y) {
            body.x = x;
            body.y = y;
        }, "Set position")
        .def("getVelocity", [](const AgentBody& body) {
            return std::make_pair(body.velocityX, body.velocityY);
        }, "Get velocity as (vx, vy)")
        .def("isHealthy", [](const AgentBody& body) {
            return body.health > 0.5f;
        }, "Check if agent is healthy")
        .def("isEnergyLow", [](const AgentBody& body) {
            return body.energy < 0.2f;
        }, "Check if agent energy is low");

    // ActionResult class with Python-friendly interface
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Result of an action performed by the agent.
        
        This class represents the outcome of an action, including reward,
        success status, and a message describing the result.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward, "Reward received (positive = good)")
        .def_readwrite("success", &ActionResult::success, "Whether the action was successful")
        .def_readwrite("message", &ActionResult::message, "Message describing the result")
        
        // Convenience methods for Python users
        .def("isGood", [](const ActionResult& result) {
            return result.success && result.reward > 0.0f;
        }, "Check if the result was good (success + positive reward)")
        
        .def("isBad", [](const ActionResult& result) {
            return !result.success || result.reward < 0.0f;
        }, "Check if the result was bad (failure or negative reward)")
        
        .def("__repr__", [](const ActionResult& result) {
            return "<ActionResult: " + result.message + " (reward: " + std::to_string(result.reward) + ")>";
        });

    // === SENSORIUM ===
    
    // SensoryPercept class with Python-friendly interface
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Sensory percept data representing the agent's current perception.
        
        This class combines all sensory inputs (vision, touch, internal signals, audio)
        into a single percept that the agent can process.
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get the vision component")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set the vision component")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get the vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get the vision height")
        .def("getTouch", &SensoryPercept::getTouch, "Get the touch component")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set the touch component")
        .def("getInternal", &SensoryPercept::getInternal, "Get the internal signals component")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set the internal signals component")
        .def("getProprioception", &SensoryPercept::getProprioception, "Get the proprioception component")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set the proprioception component")
        .def("getAudio", &SensoryPercept::getAudio, "Get the audio component")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set the audio component")
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all sensory signals as a list")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set the timestamp")
        
        // Convenience methods for Python users
        .def("hasVision", [](const SensoryPercept& percept) {
            return !percept.getVision().getData().empty();
        }, "Check if vision data is available")
        
        .def("hasTouch", [](const SensoryPercept& percept) {
            return !percept.getTouch().getData().empty();
        }, "Check if touch data is available")
        
        .def("hasInternal", [](const SensoryPercept& percept) {
            return !percept.getInternal().getData().empty();
        }, "Check if internal signals data is available")
        
        .def("hasAudio", [](const SensoryPercept& percept) {
            return !percept.getAudio().getData().empty();
        }, "Check if audio data is available")
        
        .def("getNumActiveSensors", [](const SensoryPercept& percept) {
            int count = 0;
            if (!percept.getVision().getData().empty()) count++;
            if (!percept.getTouch().getData().empty()) count++;
            if (!percept.getInternal().getData().empty()) count++;
            if (!percept.getAudio().getData().empty()) count++;
            return count;
        }, "Get the number of active sensory channels");

    // === WORLD SYSTEM ===
    
    // SimpleWorld class with Python-friendly interface
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation.
        
        This class represents the environment in which the NLM agent operates.
        It provides methods for world configuration, agent control, and
        interaction with world objects.
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"), "Configure the world dimensions")
        .def("reset", &SimpleWorld::reset, "Reset the world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set the agent's starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update the world")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"), "Apply a motor command")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal, "Get the agent's sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal, "Get the agent's body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add an object to the world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove an object from the world")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if a position is valid in the world")
        .def("getWidth", &SimpleWorld::getWidth, "Get the world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get the world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get the maximum energy level")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set the maximum energy level")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get the energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set the energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get the current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set the random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get the random seed")
        
        // Convenience methods for Python users
        .def("getGridSize", [](const SimpleWorld& world) {
            return std::make_pair(world.getWidth(), world.getHeight());
        }, "Get the grid size as (width, height)")
        
        .def("getVisionSize", [](const SimpleWorld& world) {
            return std::make_pair(world.getVisionWidth(), world.getVisionHeight());
        }, "Get the vision size as (width, height)")
        
        .def("getAgentPosition", [](const SimpleWorld& world) {
            auto body = world.getAgentBody();
            return std::make_pair(body.x, body.y);
        }, "Get the agent's position as (x, y)")
        
        .def("setAgentPosition", [](SimpleWorld& world, float x, float y) {
            world.setAgentStart(x, y);
        }, "Set the agent's position")
        
        .def("isPositionValid", &SimpleWorld::isValidPosition, "Check if position is valid")
        
        .def("addResource", [](SimpleWorld& world, float x, float y, float value) {
            WorldObject obj;
            obj.x = x;
            obj.y = y;
            obj.type = WorldObjectType::Resource;
            obj.value = value;
            obj.active = true;
            world.addObject(obj);
        }, "Add a resource object to the world")
        
        .def("addHazard", [](SimpleWorld& world, float x, float y) {
            WorldObject obj;
            obj.x = x;
            obj.y = y;
            obj.type = WorldObjectType::Hazard;
            obj.active = true;
            world.addObject(obj);
        }, "Add a hazard object to the world")
        
        .def("addWall", [](SimpleWorld& world, float x, float y) {
            WorldObject obj;
            obj.x = x;
            obj.y = y;
            obj.type = WorldObjectType::Wall;
            obj.active = true;
            world.addObject(obj);
        }, "Add a wall object to the world")
        
        .def("getNearbyObjects", [](const SimpleWorld& world, float x, float y, float radius) {
            std::vector<WorldObject> nearby;
            // This is a simplified version - a real implementation would check all objects
            return nearby;
        }, "Get objects near a position (placeholder)");

    // === BRAIN SYSTEM ===
    
    // Enhanced Brain class with Python-friendly interface
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class.
        
        This is the main class of the NLM system, representing the neural
        computation engine. It integrates all the systems of the artificial brain:
        sensory processing, memory, prediction, cognition, neuromodulation, and more.
        
        The brain operates through a 15-step simulation loop that coordinates
        all neural processes. Key features:
        - Real spiking neural computation with LIF neuron model
        - Event-driven spike propagation
        - Integrated memory systems (working, episodic, associative)
        - Neuromodulation for learning and motivation
        - Predictive coding and cognitive processing
        - Developmental stages that affect plasticity
        - Checkpoint saving and loading for persistence
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create a new brain with configuration")
        .def("initialize", &Brain::initialize, "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"), "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"), "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"), "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction, "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "", "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal, "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount, "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds, "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal, "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount, "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount, "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount, "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount, "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate, "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio, "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount, "Get total spike count")
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount, "Get pending spike event count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage, "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig, py::return_value_policy::reference_internal, "Get the configuration")
        .def("getWorkingMemory", &Brain::getWorkingMemory, "Get the working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory, "Get the episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory, "Get the associative memory system")
        .def("getPredictionSystem", &Brain::getPredictionSystem, "Get the prediction system")
        .def("getPlanner", &Brain::getPlanner, "Get the neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation, "Get the concept formation system")
        .def("getAttention", &Brain::getAttention, "Get the attentional selection system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem, "Get the development system")
        .def("getDopamine", &Brain::getDopamine, "Get the dopamine neuromodulation system")
        .def("getCuriosity", &Brain::getCuriosity, "Get the curiosity neuromodulation system")
        .def("getNovelty", &Brain::getNovelty, "Get the novelty detection system")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal, "Get the prediction error system")
        .def("getRandomGenerator", &Brain::getRandomGenerator, "Get the random number generator")
        .def("logStatus", &Brain::logStatus, "Log brain status")
        
        // Convenience methods for Python users
        .def("createBrain", [](std::shared_ptr<Config> config) {
            return std::make_shared<Brain>(config);
        }, "Create a new brain with configuration")
        
        .def("runEpisode", [](std::shared_ptr<Brain> brain, SimpleWorld& world, int maxSteps) {
            brain->initialize();
            for (int step = 0; step < maxSteps; ++step) {
                // Get sensory percept
                auto percept = world.getSensoryPercept();
                
                // Process sensory input
                brain->receiveSensoryInput(percept);
                
                // Run brain step
                brain->step(step);
                
                // Get action
                auto action = brain->produceAction();
                
                // Apply action to world
                world.applyMotorCommand(action, step * 0.001f);
                
                // Print status every 10 steps
                if (step % 10 == 0) {
                    auto status = brain->logStatus();
                }
            }
            
            return brain;
        }, "Run an episode with the brain and world")
        
        .def("getBrainStats", [](const Brain& brain) {
            py::dict stats;
            stats["total_neurons"] = brain.getTotalNeuronCount();
            stats["total_synapses"] = brain.getTotalSynapseCount();
            stats["firing_neurons"] = brain.getFiringNeuronCount();
            stats["total_spikes"] = brain.getTotalSpikeCount();
            stats["avg_firing_rate"] = brain.getAverageFiringRate();
            stats["e_i_ratio"] = brain.getExcitationInhibitionRatio();
            stats["developmental_stage"] = static_cast<int>(brain.getDevelopmentalStage());
            return stats;
        }, "Get brain statistics as a dictionary");

    // === AGENT SYSTEM ===
    
    // Enhanced AgentBrain class with Python-friendly interface
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world.
        
        This class provides the interface between the NLM brain and the world,
        handling sensory processing and motor output. It acts as a bridge,
        converting world percepts into brain inputs and brain outputs into
        world actions.
        
        Key features:
        - Sensory preprocessing and normalization
        - Motor command encoding and decoding
        - Reward-based neuromodulation
        - Development system integration
        - Curiosity and exploration motivation
        - State tracking and logging
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create a new agent brain interface")
        .def("initialize", &AgentBrain::initialize, py::arg("world"), "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize, "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize, "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand, "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"), "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"), "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage, "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel, "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError, "Get prediction error")
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain, py::return_value_policy::reference_internal, "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled, "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled, "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled, "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled, "Check if curiosity is enabled")
        
        // Convenience methods for Python users
        .def("createAgentBrain", [](std::shared_ptr<Brain> brain) {
            return std::make_shared<AgentBrain>(brain);
        }, "Create a new agent brain interface")
        
        .def("runAgentEpisode", [](std::shared_ptr<Brain> brain, SimpleWorld& world, int maxSteps) {
            // Create agent
            auto agent = std::make_shared<AgentBrain>(brain);
            
            // Initialize with world
            agent->initialize(world);
            
            // Run episode
            for (int step = 0; step < maxSteps; ++step) {
                // Update world
                world.update(0.1f);
                
                // Get sensory percept
                auto percept = world.getSensoryPercept();
                
                // Process sensory input
                agent->processSensoryInput(percept);
                
                // Run brain step
                brain->step(step);
                
                // Get motor command
                auto action = agent->decodeMotorCommand();
                
                // Apply action to world
                world.applyMotorCommand(action, step * 0.1f);
                
                // Apply reward modulation (example: +1.0 for movement, -0.5 for hazard collision)
                float reward = 1.0f;
                auto body = world.getAgentBody();
                // TODO: Add hazard detection logic
                agent->applyRewardModulation(reward, 0.5f);
                
                // Update development
                agent->updateDevelopment(0.1f);
                
                // Print status every 10 steps
                if (step % 10 == 0) {
                    py::dict stats;
                    stats["step"] = step;
                    stats["reward"] = reward;
                    stats["curiosity"] = agent->getCuriosityLevel();
                    stats["stage"] = static_cast<int>(agent->getDevelopmentalStage());
                    py::print("Step {}/{}: curiosity={}, stage={}, reward={}", step, maxSteps,
                             agent->getCuriosityLevel(), agent->getDevelopmentalStage(), reward);
                }
            }
            
            return agent;
        }, "Run an episode with an agent");

    // === FACTORY FUNCTIONS ===
    
    // Factory functions for easy creation
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration")
    
    .def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration")
    
    .def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world")
    
    .def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface")
    
    // === CONSTANTS ===
    
    // Version and metadata
    m.attr("__version__") = "Phase 6.0 - Integrated Artificial Brain";
    m.attr("__author__") = "NLM Research Project";
    m.attr("__description__") = "An experimental artificial developmental brain";
    
    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Default constants
    m.def("getDefaultConfig", []() {
        auto config = std::make_shared<Config>();
        // Set reasonable defaults
        config->set<int>("neuron_count", 1000);
        config->set<int>("region_count", 1);
        config->set<float>("connection_probability", 0.1f);
        config->set<float>("simulation_timestep", 0.001f);
        config->set<uint64_t>("random_seed", 42);
        config->set<std::string>("checkpoint_dir", "./checkpoints");
        config->set<size_t>("replay_interval", 100);
        config->set<size_t>("consolidation_interval", 1000);
        return config;
    }, "Get a configuration with reasonable defaults");
    
    // === UTILITY FUNCTIONS ===
    
    // Error handling
    m.def("safeSetConfig", [](Config& config, const std::string& key, const std::string& value) {
        try {
            config.set<std::string>(key, value);
            return true;
        } catch (const std::exception& e) {
            py::print("Error setting config: {}", e.what());
            return false;
        }
    }, py::arg("config"), py::arg("key"), py::arg("value"), "Safely set a configuration value");
    
    m.def("safeGetConfig", [](const Config& config, const std::string& key) {
        try {
            auto value = config.get<std::string>(key);
            if (value) {
                return *value;
            }
        } catch (const std::exception& e) {
            py::print("Error getting config: {}", e.what());
        }
        return std::string("");
    }, py::arg("config"), py::arg("key"), "Safely get a configuration value");
    
    // Configuration validation
    m.def("validateConfig", [](const Config& config) {
        py::dict errors;
        
        // Check required keys
        auto keys = config.getKeys();
        bool has_neuron_count = false;
        bool has_connection_probability = false;
        
        for (const auto& key : keys) {
            if (key == "neuron_count") has_neuron_count = true;
            if (key == "connection_probability") has_connection_probability = true;
        }
        
        if (!has_neuron_count) {
            errors["neuron_count"] = "Missing required configuration";
        }
        
        if (!has_connection_probability) {
            errors["connection_probability"] = "Missing required configuration";
        }
        
        return errors;
    }, "Validate configuration and return any errors");
    
    // Logging and status reporting
    m.def("logBrainStatus", [](const Brain& brain) {
        brain.logStatus();
    }, "Log brain status to console");
    
    m.def("printConfig", [](const Config& config) {
        py::print("Configuration:");
        py::print("  Keys: {}", config.getKeys());
        py::print("  Summary: {}", config.summary());
    }, "Print configuration details");

} // namespace nlm