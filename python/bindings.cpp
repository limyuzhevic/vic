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

// Helper function to convert Python list to std::vector
inline std::vector<float> pyListToFloatVector(const py::list& py_list) {
    std::vector<float> result;
    result.reserve(py_list.size());
    for (const auto& item : py_list) {
        result.push_back(item.cast<float>());
    }
    return result;
}

// Helper function to convert Python int to NeuronType
inline NeuronType pyIntToNeuronType(int value) {
    if (value < 0 || value >= static_cast<int>(NeuronType::Internal) + 1) {
        throw std::invalid_argument("Invalid NeuronType value: " + std::to_string(value));
    }
    return static_cast<NeuronType>(value);
}

// Helper function to convert Python string to ActionType
inline ActionType pyStringToActionType(const std::string& str) {
    if (str == "MoveForward") return ActionType::MoveForward;
    if (str == "MoveBackward") return ActionType::MoveBackward;
    if (str == "MoveLeft") return ActionType::MoveLeft;
    if (str == "MoveRight") return ActionType::MoveRight;
    if (str == "TurnLeft") return ActionType::TurnLeft;
    if (str == "TurnRight") return ActionType::TurnRight;
    if (str == "Look") return ActionType::Look;
    if (str == "LookUp") return ActionType::LookUp;
    if (str == "LookDown") return ActionType::LookDown;
    if (str == "Interact") return ActionType::Interact;
    if (str == "Eat") return ActionType::Eat;
    if (str == "Drink") return ActionType::Drink;
    if (str == "Rest") return ActionType::Rest;
    if (str == "Wait") return ActionType::Wait;
    if (str == "Custom") return ActionType::Custom;
    throw std::invalid_argument("Invalid ActionType string: " + str);
}

// Python bindings for NLM (Neural Learning Machine)
// ====================================================
//
// This file provides comprehensive Python bindings for the NLM C++ neural simulation framework.
// It implements the complete integrated artificial brain system with all its major components:
//
// 1. Core neural computation (LIF neurons, spike propagation, plasticity)
// 2. Memory systems (working, episodic, associative memory)
// 3. Prediction and cognitive systems
// 4. Neuromodulation (dopamine, curiosity, novelty)
// 5. Sensorimotor integration and action selection
// 6. Developmental and learning mechanisms
//
// The bindings follow modern Python conventions and provide:
// - Type-safe conversions between Python and C++
// - Comprehensive error handling and validation
// - Rich documentation with examples
// - Pythonic interfaces (properties, iterators, context managers)
// - Integration with the existing pybind11 ecosystem
//
// Usage: `import pynlm` to access all bindings and helper functions
//
// Files: pynlm/bindings.cpp contains all bindings
//        pynlm/CMakeLists.txt builds the Python module

    // Register custom exception types for better error handling
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "IndexError");
    
    // === BASIC TYPES AND ENUMERATIONS ===
    
    // NeuronId - Unique identifier for a neuron
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron.
        
        Provides thread-safe identification of neurons in the brain network.
        Neurons are identified by a unique 64-bit integer value.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0,
             "Create NeuronId with specified value")
        .def_readwrite("value", &NeuronId::value, "Neuron identifier value")
        .def("index", &NeuronId::index, "Get the index component of the ID")
        .def("__eq__", &NeuronId::operator==, "Check if two NeuronId objects are equal")
        .def("__ne__", &NeuronId::operator!=, "Check if two NeuronId objects are not equal")
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });
    
    // SynapseId - Unique identifier for a synapse
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse.
        
        Synapses are identified by a unique 64-bit integer value.
        Used for referencing specific synaptic connections in the brain.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0,
             "Create SynapseId with specified value")
        .def_readwrite("value", &SynapseId::value, "Synapse identifier value")
        .def("index", &SynapseId::index, "Get the index component of the ID")
        .def("__eq__", &SynapseId::operator==, "Check if two SynapseId objects are equal")
        .def("__ne__", &SynapseId::operator!=, "Check if two SynapseId objects are not equal")
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        });
    
    // RegionId - Unique identifier for a brain region
    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region.
        
        Brain regions are identified by a unique 64-bit integer value.
        Used for organizing neurons into functional and anatomical regions.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0,
             "Create RegionId with specified value")
        .def_readwrite("value", &RegionId::value, "Region identifier value")
        .def("index", &RegionId::index, "Get the index component of the ID")
        .def("__eq__", &RegionId::operator==, "Check if two RegionId objects are equal")
        .def("__ne__", &RegionId::operator!=, "Check if two RegionId objects are not equal")
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });
    
    // PopulationId - Unique identifier for a neuron population
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population.
        
        Neuron populations are groups of neurons with similar properties
        or functions (e.g., sensory neurons, motor neurons).
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0,
             "Create PopulationId with specified value")
        .def_readwrite("value", &PopulationId::value, "Population identifier value")
        .def("index", &PopulationId::index, "Get the index component of the ID")
        .def("__eq__", &PopulationId::operator==, "Check if two PopulationId objects are equal")
        .def("__ne__", &PopulationId::operator!=, "Check if two PopulationId objects are not equal");
    
    // Neuron type enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Enumeration of neuron types in the NLM system.
        
        Neurons are categorized based on their function and connectivity patterns:
        - Excitatory: Promotes neural activity
        - Inhibitory: Reduces neural activity
        - Modulatory: Affects plasticity and other global properties
        - Sensory: Receives external input
        - Motor: Generates motor output
        - Internal: Performs internal computations
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();
    
    // Synapse type enumeration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Enumeration of synapse types in the NLM system.
        
        Synapses can be classified by their connection type:
        - Excitatory: Increases postsynaptic neuron activity
        - Inhibitory: Decreases postsynaptic neuron activity
        - Modulatory: Affects plasticity and other global properties
        - Electrical: Direct electrical coupling
        - GapJunction: Direct electrical coupling
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();
    
    // Developmental stage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Enumeration of developmental stages in the NLM system.
        
        The brain develops through distinct phases:
        - Initial: Birth/development
        - CriticalPeriod: Sensitive period for learning
        - Maturation: Refinement of connections
        - Adult: Stable functionality
        - Aging: Degeneration and adaptation
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();
    
    // Firing state enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Enumeration of neuron firing states.
        
        Neurons can be in different states:
        - Resting: Not ready to fire
        - Active: Ready to fire
        - Refractory: Cannot fire (just fired)
        - Inhibited: Actively prevented from firing
    )pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();
    
    // Action type enumeration
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Enumeration of action types available to the agent.
        
        Actions are high-level behavioral commands:
        - Movement actions (MoveForward, MoveBackward, etc.)
        - Orientation actions (TurnLeft, TurnRight)
        - Sensory actions (Look, LookUp, LookDown)
        - Interaction actions (Interact, Eat, Drink)
        - State actions (Rest, Wait, Custom)
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
    
    // Motor command enumeration
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Enumeration of low-level motor commands.
        
        Motor commands are executable by the environment:
        - Movement commands (MoveForward, MoveBackward, etc.)
        - Orientation commands (TurnLeft, TurnRight)
        - Sensory commands (LookLeft, LookRight)
        - Interaction commands (Interact, Wait)
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
    
    // World object type enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        Enumeration of object types in the simulation world.
        
        World objects have different properties and behaviors:
        - Empty: No object at this location
        - Resource: Valuable object that can be collected
        - Hazard: Dangerous object to avoid
        - Wall: Obstacle that cannot be passed
        - Marker: Special location or waypoint
    )pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();
    
    // === CONFIGURATION SYSTEM ===
    
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration system for the NLM neural simulation framework.
        
        The Config class manages simulation parameters and settings,
        allowing for flexible configuration of brain parameters,
        neural dynamics, and simulation behavior.
        
        Configuration supports:
        - Key-value pairs for arbitrary parameters
        - Type-safe get/set operations
        - File-based configuration loading
        - Command-line argument parsing
        - Configuration validation and validation
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, const py::list& args_list) {
            // Convert Python list to C++ array
            std::vector<std::string> args;
            for (const auto& item : args_list) {
                args.push_back(item.cast<std::string>());
            }
            return self.loadFromArgs(args.size(), args.data());
        }, py::arg("args"),
             "Load configuration from Python argument list")
        .def("loadFromArgs", [](Config& self, const py::kwargs& kwargs) {
            // Handle keyword arguments
            for (const auto& item : kwargs) {
                std::string key = item.first.cast<std::string>();
                if (key == "random_seed") {
                    int64_t value = item.second.cast<int64_t>();
                    self.set(key, value, ConfigSource::CommandLine);
                } else if (key == "neuron_count") {
                    int64_t value = item.second.cast<int64_t>();
                    self.set(key, value, ConfigSource::CommandLine);
                } else if (key == "simulation_timestep") {
                    double value = item.second.cast<double>();
                    self.set(key, value, ConfigSource::CommandLine);
                }
                // Add other configuration options as needed
            }
            return true;
        }, py::arg("kwargs") = py::kwargs(),
             "Load configuration from Python keyword arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get", [](Config& self, const std::string& key, const std::string& default_value) {
            // Try to get as different types
            if (auto int_val = self.getOr<int64_t>(key)) return py::cast(*int_val);
            if (auto float_val = self.getOr<double>(key)) return py::cast(*float_val);
            if (auto str_val = self.getOr<std::string>(key)) return py::cast(*str_val);
            return default_value;
        }, py::arg("key"), py::arg("default_value"), "Get configuration value with fallback")
        .def("getInt", [](Config& self, const std::string& key, int64_t default_value) {
            return self.getOr(key, default_value);
        }, py::arg("key"), py::arg("default_value"), "Get integer configuration value")
        .def("getFloat", [](Config& self, const std::string& key, double default_value) {
            return self.getOr(key, default_value);
        }, py::arg("key"), py::arg("default_value"), "Get float configuration value")
        .def("getString", [](Config& self, const std::string& key, const std::string& default_value) {
            return self.getOr(key, default_value);
        }, py::arg("key"), py::arg("default_value"), "Get string configuration value")
        .def("getKeys", &Config::getKeys, "Get all configuration keys")
        .def("set", &Config::set, py::arg("key"), py::arg("value"), py::arg("source"),
             "Set configuration value with specified source")
        .def("clear", &Config::clear, "Clear all configuration entries")
        .def("summary", &Config::summary, "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });
    
    // === SENSORY INPUT ===
    
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for sensory input in the NLM system.
        
        SensoryInput represents external or internal sensory information
        that is injected into the brain for processing. It provides access
        to raw sensor data and metadata.
        
        Common implementations include:
        - Vision: Visual sensor input (pixels, depth, etc.)
        - Audio: Auditory sensor input (samples, frequency, etc.)
        - Internal: Internal state signals (energy, hormones, etc.)
    )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a Python list")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality of the data")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp of the input")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp of the input");
    
    // Vision sensory input
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Visual sensory input for the NLM system.
        
        Vision represents visual sensor data with resolution and color channels.
        Supports various image formats and processing modes.
        
        Attributes:
        - width: Image width in pixels
        - height: Image height in pixels  
        - channels: Number of color channels (1=grayscale, 3=RGB, 4=RGBA)
        - data: Pixel intensity values (0.0-1.0 normalized)
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width") = 640,
             py::arg("height") = 480, py::arg("channels") = 3,
             "Create Vision sensor with specified dimensions")
        .def("setData", [](Vision& self, const py::list& data_list) {
            self.setData(pyListToFloatVector(data_list));
        }, py::arg("data"), "Set pixel data from Python list")
        .def("getWidth", &Vision::getWidth, "Get image width")
        .def("getHeight", &Vision::getHeight, "Get image height")
        .def("getChannels", &Vision::getChannels, "Get number of color channels");
    
    // Audio sensory input
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input for the NLM system.
        
        Audio represents sound sensor data with sample rate and duration.
        Supports various audio formats and processing modes.
        
        Attributes:
        - sampleRate: Sampling rate in Hz
        - numSamples: Number of audio samples
        - data: Audio amplitude values (-1.0 to 1.0)
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate") = 44100,
             py::arg("numSamples") = 1024,
             "Create Audio sensor with specified parameters")
        .def("setData", [](Audio& self, const py::list& data_list) {
            self.setData(pyListToFloatVector(data_list));
        }, py::arg("data"), "Set audio data from Python list")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set audio sampling rate")
        .def("getSampleRate", &Audio::getSampleRate, "Get audio sampling rate")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of audio samples");
    
    // Internal signals sensory input
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal signals sensory input for the NLM system.
        
        InternalSignals represents internal state information such as
        energy levels, hormone concentrations, or other physiological signals.
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all signals");
    
    // === MOTOR ACTIONS ===
    
    py::class_<Action>(m, "Action", R"pbdoc(
        Representation of motor actions in the NLM system.
        
        Actions are high-level behavioral commands that can be executed
        by the agent in the environment. They represent the agent's
        intentions and goals.
        
        Attributes:
        - type: Action type (MoveForward, TurnLeft, etc.)
        - parameters: Action-specific parameters
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create Action with specified type")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"),
             py::arg("parameters") = std::vector<float>(),
             "Create Action with type and parameters")
        .def_readwrite("type", &Action::type, "Action type")
        .def_readwrite("parameters", &Action::parameters, "Action parameters")
        .def("getType", &Action::getType, "Get action type")
        .def("setType", &Action::setType, py::arg("type"), "Set action type")
        .def("getParameters", &Action::getParameters, "Get action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("getName", &Action::getName, "Get action name")
        .def("clone", &Action::clone, "Create a copy of this action");
    
    // World object
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        Representation of objects in the simulation world.
        
        WorldObject represents physical objects in the NLM environment,
        including resources, hazards, walls, and markers.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"),
             py::arg("value") = 0.0f, py::arg("radius") = 0.5f,
             "Create WorldObject with position, type, and properties")
        .def_readwrite("x", &WorldObject::x, "X position")
        .def_readwrite("y", &WorldObject::y, "Y position")
        .def_readwrite("radius", &WorldObject::radius, "Object radius")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Object value (e.g., resource amount)")
        .def_readwrite("active", &WorldObject::active, "Whether object is active");
    
    // Agent body state
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        State representation of the agent's physical body.
        
        AgentBody represents the agent's current physical state including
        position, orientation, velocity, and physiological state.
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position")
        .def_readwrite("y", &AgentBody::y, "Y position")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation in radians")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Energy level (0.0-1.0)")
        .def_readwrite("health", &AgentBody::health, "Health level (0.0-1.0)")
        .def_readwrite("age", &AgentBody::age, "Age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time since last action")
        .def("reset", &AgentBody::reset, "Reset body state to initial values");
    
    // Action result
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Result of executing an action in the world.
        
        ActionResult contains the outcome of an action including
        reward, success status, and descriptive message.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward") = 0.0f, py::arg("success") = false,
             py::arg("message") = "",
             "Create ActionResult with reward, success, and message")
        .def_readwrite("reward", &ActionResult::reward, "Reward received (higher is better)")
        .def_readwrite("success", &ActionResult::success, "Whether action was successful")
        .def_readwrite("message", &ActionResult::message, "Descriptive message of result");
    
    // === BRAIN SYSTEM ===
    
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Combined sensory input from all modalities.
        
        SensoryPercept integrates information from vision, audio, and internal
        signals into a unified perceptual representation that can be processed
        by the brain.
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get vision component")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set vision component")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision height")
        .def("getTouch", &SensoryPercept::getTouch, "Get touch component")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch component")
        .def("getInternal", &SensoryPercept::getInternal, "Get internal signals")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception, "Get proprioception")
        .def("setProprioception", &SensoryPercept::setProprioception,
             py::arg("proprioception"), "Set proprioception")
        .def("getAudio", &SensoryPercept::getAudio, "Get audio component")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio component")
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all sensory signals as a combined list")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get perception timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set perception timestamp");
    
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation.
        
        SimpleWorld provides a basic 2D environment with basic physics,
        object management, and interaction capabilities. It includes
        vision and audio sensors for the agent.
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and sensor parameters")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent's starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world physics for one timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept from the environment")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent's current body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at specified position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not blocked)")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum agent energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum agent energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed for world")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed");
    
    // === AGENT BRAIN INTERFACE ===
    
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class.
        
        Brain implements the core neural computation engine for the NLM system.
        It provides:
        - Real spiking neural dynamics (LIF neurons)
        - Event-driven spike propagation with synaptic delays
        - Integrated memory systems (working, episodic, associative)
        - Prediction systems for forward modeling
        - Cognitive systems (planning, attention, concept formation)
        - Neuromodulation systems (dopamine, curiosity, novelty)
        - Development system for plasticity regulation
        - Sensorimotor integration and action selection
        
        The brain operates as a closed-loop system that continuously
        processes sensory input, generates actions, and learns from experience.
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config") = nullptr,
             "Create brain with configuration (creates default if None)")
        .def("initialize", &Brain::initialize, "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"), "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with explicit timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction, "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal, "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount, "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds, "Get all region IDs")
        .def("getRegions", &Brain::getRegions, py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount, "Get total neuron count")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount, "Get total synapse count")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount, "Get active neuron count")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount, "Get firing neuron count")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate, "Get average firing rate")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount, "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage, "Get developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig, py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus, "Log brain status");
    
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world.
        
        AgentBrain provides the interface between the neural brain and the
        simulated environment. It handles sensory processing, action decoding,
        and neuromodulation based on the agent's experience and goals.
        
        Key functions:
        - Initialize with world context
        - Process sensory percepts from environment
        - Decode motor commands from neural activity
        - Apply reward-based neuromodulation
        - Update developmental state
        - Manage curiosity and novelty detection
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain with reference to neural brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world reference")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize, "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize, "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand, "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage, "Get developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel, "Get neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError, "Get prediction error")
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain, py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"),
             "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"),
             "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"),
             "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"),
             "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled");
    
    // === FACTORY FUNCTIONS ===
    
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with sensible defaults",
    py::return_value_policy::reference_internal);
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config").noconvert() = nullptr, "Create a new brain with configuration",
    py::return_value_policy::reference_internal);
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world",
    py::return_value_policy::reference_internal);
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain").noconvert(), "Create a new agent brain interface",
    py::return_value_policy::reference_internal);
    
    // === CONSTANTS ===
    
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // === UTILITY FUNCTIONS ===
    
    // Convert Python list to NLM data format
    m.def("to_float_list", [](const py::list& py_list) {
        return pyListToFloatVector(py_list);
    }, py::arg("py_list"), "Convert Python list to NLM float vector");
    
    // Create action from Python data
    m.def("create_action", [](const std::string& action_type_str, const py::list& parameters = py::list()) {
        ActionType action_type = pyStringToActionType(action_type_str);
        std::vector<float> params = pyListToFloatVector(parameters);
        return std::make_unique<Action>(Action(action_type, params));
    }, py::arg("action_type"), py::arg("parameters") = py::list(),
     "Create action from Python string and parameters");
    
    // Create vision from Python data
    m.def("create_vision", [](size_t width, size_t height, size_t channels = 3, const py::list& data = py::list()) {
        auto vision = std::make_unique<Vision>(width, height, channels);
        if (!data.is_empty()) {
            vision->setData(pyListToFloatVector(data));
        }
        return vision;
    }, py::arg("width") = 640, py::arg("height") = 480, py::arg("channels") = 3,
     py::arg("data") = py::list(), "Create vision sensor from Python data");
    
    // Create audio from Python data
    m.def("create_audio", [](size_t sampleRate, size_t numSamples, const py::list& data = py::list()) {
        auto audio = std::make_unique<Audio>(sampleRate, numSamples);
        if (!data.is_empty()) {
            audio->setData(pyListToFloatVector(data));
        }
        return audio;
    }, py::arg("sampleRate") = 44100, py::arg("numSamples") = 1024,
     py::arg("data") = py::list(), "Create audio sensor from Python data");
}

} // namespace nlm
