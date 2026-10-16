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

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

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

// Custom exception for NLM errors
class NLMError : public std::runtime_error {
public:
    NLMError(const std::string& message) : std::runtime_error(message) {}
};

// Helper function to convert JSON to ConfigValue
static ConfigValue jsonToConfigValue(const nlohmann::json& j) {
    if (j.is_number_integer()) {
        int64_t val = j.get<int64_t>();
        if (val <= std::numeric_limits<int>::max() && val >= std::numeric_limits<int>::min()) {
            return static_cast<int>(val);
        }
        return val;
    } else if (j.is_number_float()) {
        return j.get<double>();
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_array()) {
        if (!j.empty()) {
            if (j[0].is_number_integer()) {
                return j.get<std::vector<int>>();
            } else if (j[0].is_number_float()) {
                return j.get<std::vector<double>>();
            } else if (j[0].is_string()) {
                return j.get<std::vector<std::string>>();
            }
        }
    }
    return std::string();
}

// Helper function to convert ConfigValue to JSON
static nlohmann::json configValueToJson(const ConfigValue& value) {
    return std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        return nlohmann::json(arg);
    }, value);
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Key Features:
        - Neural network simulation with spiking neurons
        - Memory systems (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Prediction and planning systems
        - Developmental stages and plasticity
        - Agent-based behavior simulation
        - Advanced configuration management
        - Performance monitoring and metrics
        
        Usage Examples:
        ```python
        import pynlm
        
        # Create a brain with configuration
        config = pynlm.Config()
        brain = pynlm.Brain(config)
        
        # Create a world and agent
        world = pynlm.SimpleWorld()
        agent = pynlm.AgentBrain(brain)
        
        # Run simulation
        brain.initialize()
        for step in range(100):
            brain.step(step)
            
        # Get predictions and actions
        action = brain.produceAction()
        ```
    )pbdoc";

    // Register custom exception
    py::register_exception<NLMError>(m, "NLMError");

    // Neuron identifiers with better documentation
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron in the neural network.
        
        Each neuron has a unique integer identifier that is automatically assigned
        by the brain system. NeuronIds can be used to reference specific neurons
        for direct current injection or monitoring.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index,
             "Get the index representation of this neuron ID")
        .def("__eq__", &NeuronId::operator==,
             "Check if two neuron IDs are equal")
        .def("__ne__", &NeuronId::operator!=,
             "Check if two neuron IDs are not equal")
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        });

    // Synapse identifiers
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synaptic connection between neurons.
        
        Synapses have unique IDs that allow for targeted manipulation
        of specific synaptic connections, including weight changes
        and eligibility trace management.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index,
             "Get the index representation of this synapse ID")
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    // Region identifiers
    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a neural region in the brain.
        
        Brain regions are clusters of neuron populations that perform
        specific functions. Regions can be connected via inter-region
        pathways to form complex neural circuits.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index,
             "Get the index representation of this region ID")
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    // Population identifiers
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population within a region.
        
        Neuron populations contain multiple neurons that share
        similar properties and functions. Populations are the basic
        building blocks of neural computation in NLM.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index,
             "Get the index representation of this population ID")
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Neuron type enumeration with enhanced documentation
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Enumeration of neuron types in the neural network.
        
        Neuron types determine the functional properties and behavior
        of individual neurons within the network.
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory,
               "Neuron that increases firing probability of postsynaptic neurons")
        .value("Inhibitory", NeuronType::Inhibitory,
               "Neuron that decreases firing probability of postsynaptic neurons")
        .value("Modulatory", NeuronType::Modulatory,
               "Neuron that modulates the overall excitability of neural populations")
        .value("Sensory", NeuronType::Sensory,
               "Neuron that receives and processes external sensory input")
        .value("Motor", NeuronType::Motor,
               "Neuron that generates motor commands for action execution")
        .value("Internal", NeuronType::Internal,
               "Neuron that maintains internal state and homeostasis")
        .export_values();

    // Synapse type enumeration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Enumeration of synaptic connection types.
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // Developmental stage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Enumeration of developmental stages in the neural system's lifecycle.
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial,
               "Initial developmental stage with high plasticity")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod,
               "Critical period for rapid learning and development")
        .value("Maturation", DevelopmentalStage::Maturation,
               "Stage of neural maturation and stabilization")
        .value("Adult", DevelopmentalStage::Adult,
               "Adult developmental stage with stable performance")
        .value("Aging", DevelopmentalStage::Aging,
               "Aging stage with gradual decline in plasticity")
        .export_values();

    // Firing state enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Enumeration of neuron firing states.
    )pbdoc")
        .value("Resting", FiringState::Resting,
               "Neuron is at resting membrane potential")
        .value("Active", FiringState::Active,
               "Neuron is actively firing action potentials")
        .value("Refractory", FiringState::Refractory,
               "Neuron is in refractory period after firing")
        .value("Inhibited", FiringState::Inhibited,
               "Neuron is inhibited by strong inhibitory input")
        .export_values();

    // Action type enumeration with detailed descriptions
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Enumeration of action types that can be produced by the motor system.
    )pbdoc")
        .value("MoveForward", ActionType::MoveForward,
               "Move forward in the world")
        .value("MoveBackward", ActionType::MoveBackward,
               "Move backward in the world")
        .value("MoveLeft", ActionType::MoveLeft,
               "Move left in the world")
        .value("MoveRight", ActionType::MoveRight,
               "Move right in the world")
        .value("TurnLeft", ActionType::TurnLeft,
               "Turn left (rotate)")
        .value("TurnRight", ActionType::TurnRight,
               "Turn right (rotate)")
        .value("Look", ActionType::Look,
               "Look in a specific direction")
        .value("LookUp", ActionType::LookUp,
               "Look up")
        .value("LookDown", ActionType::LookDown,
               "Look down")
        .value("Interact", ActionType::Interact,
               "Interact with nearby object")
        .value("Eat", ActionType::Eat,
               "Eat or consume object")
        .value("Drink", ActionType::Drink,
               "Drink or consume liquid")
        .value("Rest", ActionType::Rest,
               "Rest and conserve energy")
        .value("Wait", ActionType::Wait,
               "Wait without performing any action")
        .value("Custom", ActionType::Custom,
               "Custom action with parameters")
        .export_values();

    // Motor command enumeration
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Enumeration of low-level motor commands that the brain can produce.
        
        These are innate muscle-like signals, not semantic actions.
    )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward,
               "Propel forward")
        .value("MoveBackward", MotorCommand::MoveBackward,
               "Propel backward")
        .value("TurnLeft", MotorCommand::TurnLeft,
               "Rotate left")
        .value("TurnRight", MotorCommand::TurnRight,
               "Rotate right")
        .value("LookLeft", MotorCommand::LookLeft,
               "Pan sensor left")
        .value("LookRight", MotorCommand::LookRight,
               "Pan sensor right")
        .value("Interact", MotorCommand::Interact,
               "Interact with nearby object")
        .value("Wait", MotorCommand::Wait,
               "No movement")
        .export_values();

    // World object type enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        Enumeration of object types in the world (for rendering/debug only, NOT given to brain).
    )pbdoc")
        .value("Empty", WorldObjectType::Empty,
               "Empty space in the world")
        .value("Resource", WorldObjectType::Resource,
               "Positive reward object")
        .value("Hazard", WorldObjectType::Hazard,
               "Negative reward object")
        .value("Wall", WorldObjectType::Wall,
               "Boundary/wall")
        .value("Marker", WorldObjectType::Marker,
               "Navigation aid")
        .export_values();

    // Configuration class with enhanced functionality
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system with advanced JSON/YAML support.
        
        Provides comprehensive configuration management including:
        - JSON file loading and saving
        - Command-line argument parsing
        - Runtime configuration modification
        - Configuration validation
        - Export/import functionality
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON or YAML file")
        .def("loadFromJson", &Config::loadFromJson, py::arg("jsonString"),
             "Load configuration from a JSON string")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON or YAML file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("exportToJson", &Config::exportToJson,
             "Export all configuration entries to JSON")
        .def("importFromJson", &Config::importFromJson, py::arg("jsonData"),
             "Load configuration from JSON data")
        .def("validate", &Config::validate,
             "Validate configuration and return list of errors")
        .def("apply", &Config::apply, py::arg("filepath"),
             "Apply configuration from file with validation")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("set", (void (Config::*)(const std::string&, const std::string&, ConfigSource)) &Config::set,
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set a configuration value (string)")
        .def("set", (void (Config::*)(const std::string&, int, ConfigSource)) &Config::set,
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set a configuration value (int)")
        .def("set", (void (Config::*)(const std::string&, double, ConfigSource)) &Config::set,
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set a configuration value (float)")
        .def("set", (void (Config::*)(const std::string&, bool, ConfigSource)) &Config::set,
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set a configuration value (bool)")
        .def("get", (std::optional<int> (Config::*)(const std::string&) const) &Config::get<int>,
             py::arg("key"), "Get an integer configuration value")
        .def("get", (std::optional<double> (Config::*)(const std::string&) const) &Config::get<double>,
             py::arg("key"), "Get a float configuration value")
        .def("get", (std::optional<bool> (Config::*)(const std::string&) const) &Config::get<bool>,
             py::arg("key"), "Get a boolean configuration value")
        .def("get", (std::optional<std::string> (Config::*)(const std::string&) const) &Config::get<std::string>,
             py::arg("key"), "Get a string configuration value")
        .def("getOr", (int (Config::*)(const std::string&, int) const) &Config::getOr<int>,
             py::arg("key"), py::arg("defaultValue"), "Get configuration value with default (int)")
        .def("getOr", (double (Config::*)(const std::string&, double) const) &Config::getOr<double>,
             py::arg("key"), py::arg("defaultValue"), "Get configuration value with default (float)")
        .def("getOr", (bool (Config::*)(const std::string&, bool) const) &Config::getOr<bool>,
             py::arg("key"), py::arg("defaultValue"), "Get configuration value with default (bool)")
        .def("getOr", (std::string (Config::*)(const std::string&, const std::string&) const) &Config::getOr<std::string>,
             py::arg("key"), py::arg("defaultValue"), "Get configuration value with default (string)");

    // Sensory input base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for all sensory input types.
        
        Sensory input provides the agent with information about the external
        and internal environment. All sensory modalities inherit from this class.
    )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision sensory input
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input representing visual perception.
        
        Vision input is a grayscale intensity grid representing what the agent
        sees in its environment. The data is formatted as row-major with values
        ranging from 0.0 (black) to 1.0 (white).
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width") = 16, py::arg("height") = 16, py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set vision data")
        .def("getWidth", &Vision::getWidth, "Get vision width in pixels")
        .def("getHeight", &Vision::getHeight, "Get vision height in pixels")
        .def("getChannels", &Vision::getChannels, "Get number of color channels");

    // Audio sensory input
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input representing sound perception.
        
        Audio input contains sound waveform data sampled at a specific
        rate. This allows agents to perceive auditory information in their environment.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate") = 44100, py::arg("numSamples") = 1024)
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"), "Set sample rate")
        .def("getSampleRate", &Audio::getSampleRate, "Get sample rate")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of samples");

    // Internal signals sensory input
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal signals sensory input representing homeostatic state.
        
        Internal signals provide information about the agent's physiological
        state, including energy levels, health, and other internal variables.
        These signals are used for self-regulation and motivation.
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"), "Add an internal signal value")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all internal signals");

    // Action class with enhanced functionality
    py::class_<Action>(m, "Action", R"pbdoc(
        Action representation for motor output.
        
        Actions represent the agent's output to the environment. They contain
        both the type of action and any associated parameters. Actions can be
        executed by the world to produce effects and rewards.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType, "Get the action type")
        .def("setType", &Action::setType, py::arg("type"), "Set the action type")
        .def("getParameters", &Action::getParameters, "Get action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("getName", &Action::getName, "Get human-readable action name")
        .def("clone", &Action::clone, "Create a copy of this action")
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + ">";
        });

    // World object class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        World object representation.
        
        World objects represent entities in the agent's environment. They have
        physical properties like position, size, and type. Objects can provide
        rewards or hazards to the agent based on interactions.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Create a world object at position (x, y) with type and properties")
        .def_readwrite("x", &WorldObject::x, "X coordinate")
        .def_readwrite("y", &WorldObject::y, "Y coordinate")
        .def_readwrite("radius", &WorldObject::radius, "Radius")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Object value (reward/hazard magnitude)")
        .def_readwrite("active", &WorldObject::active, "Whether the object is active");

    // Agent body class with enhanced properties
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state - contains physical properties and internal state.
        
        This represents the agent's physical embodiment in the world, including
        position, orientation, velocity, and physiological state. The body interacts
        with the world and provides feedback to the agent brain.
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position in world")
        .def_readwrite("y", &AgentBody::y, "Y position in world")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation in radians")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Energy level (0-1)")
        .def_readwrite("health", &AgentBody::health, "Health level (0-1)")
        .def_readwrite("age", &AgentBody::age, "Developmental age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether the agent is moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether the agent is turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time since last action")
        .def("reset", &AgentBody::reset, "Reset body to initial state");

    // Action result class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Action result from the world's perspective.
        
        ActionResult contains information about the outcome of executing
        an action, including the reward received, whether the action
        was successful, and any descriptive message.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create an action result with reward, success flag, and message")
        .def_readwrite("reward", &ActionResult::reward, "Reward value")
        .def_readwrite("success", &ActionResult::success, "Whether action was successful")
        .def_readwrite("message", &ActionResult::message, "Result message");

    // Sensory percept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Sensory data packet - what the agent perceives.
        
        Contains only low-level sensory signals, NO semantic labels.
        The percept represents the raw sensory input that the agent's
        brain processes to make decisions.
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get vision sensory data")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set vision sensory data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision height")
        .def("getTouch", &SensoryPercept::getTouch, "Get touch sensory data")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch sensory data")
        .def("getInternal", &SensoryPercept::getInternal, "Get internal sensory data")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal sensory data")
        .def("getProprioception", &SensoryPercept::getProprioception, "Get proprioception data")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioception data")
        .def("getAudio", &SensoryPercept::getAudio, "Get audio sensory data")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio sensory data")
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all sensory signals concatenated")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set timestamp");

    // Simple world class with enhanced functionality
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation.
        
        The world provides the environment in which the agent operates. It contains
        physical space, objects, and handles the execution of agent actions. The world
        generates sensory percepts that are fed to the agent's brain.
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth") = 16, py::arg("visionHeight") = 16,
             "Configure world dimensions and sensory parameters")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent's starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world state")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply a motor command to the world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept from the world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent's body state from the world")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add an object to the world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position (x, y)")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid in the world")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy capacity")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum energy capacity")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed for reproducibility")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed");

    // Brain class with enhanced Python interface
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class.
        
        The brain is the core computational engine of NLM. It implements real
        spiking neural computation with event-driven dynamics and integrated
        memory, prediction, cognition, and neuromodulation systems. The brain
        receives sensory input and produces motor actions through a closed-loop
        control system.
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
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
             "Save brain state to file (checkpointing)")
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
             "Log brain status and statistics")
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
             "Get dopamine neuromodulator")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity neuromodulator")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty neuromodulator")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error signal");

    // AgentBrain class with enhanced Python interface
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world.
        
        AgentBrain handles sensory transduction and motor decoding, providing
        a simplified interface for agents to interact with the NLM brain.
        It processes sensory input and produces motor commands for world interaction.
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface with underlying brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get sensory input size expected by brain")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get motor output size expected")
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
             py::arg("enable"),
             "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled");

    // Factory functions for easy object creation
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

    // Convenience constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Advanced utilities
    m.def("benchmark_neural_network", [](const std::string& config_file, int steps) {
        // This would call the benchmark function from benchmark.cpp
        return "Benchmark not implemented in bindings";
    }, py::arg("config_file"), py::arg("steps"),
       "Run neural network benchmark");

    m.def("validate_config", [](const std::string& config_file) {
        Config config;
        if (config.loadFromFile(config_file)) {
            auto errors = config.validate();
            if (errors.empty()) {
                return "Configuration is valid";
            } else {
                return "Configuration errors:\n" + std::accumulate(errors.begin(), errors.end(), std::string(),
                    [](const std::string& acc, const std::string& err) { return acc + "- " + err + "\n"; });
            }
        }
        return "Failed to load configuration";
    }, py::arg("config_file"), "Validate configuration file");

    m.def("export_config_to_json", [](const std::string& config_file) {
        Config config;
        if (config.loadFromFile(config_file)) {
            nlohmann::json json = config.exportToJson();
            return json.dump(4);
        }
        return "{}";
    }, py::arg("config_file"), "Export configuration to JSON");
}

} // namespace nlm

} // namespace nlm
