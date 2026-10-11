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
#include <chrono>
#include <stdexcept>
#include <limits>

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
        
        This binding provides a complete API for creating and simulating artificial brains
        with spiking neural networks, learning capabilities, memory systems, and
        sensorimotor loops.
        
        Key Features:
        - Robust error handling with Python-specific exceptions
        - Comprehensive documentation and examples
        - Performance profiling tools
        - Advanced utility functions
        - Type-safe conversions
        - Checkpointing and serialization support
        
        See the examples directory for usage demonstrations.
    )pbdoc";

    // Register custom exceptions for better error handling
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "IndexError");
    py::register_exception<std::logic_error>(m, "LogicError");
    
    // Version information
    m.attr("__version__") = "1.0.0";
    m.attr("__author__") = "NLM Project Team";
    m.attr("__description__") = "An experimental artificial developmental brain";

    // === CORE IDENTIFIERS ===
    
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
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        });

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
        .def("__str__", [](const SynapseId& id) {
            return "SynapseId(" + std::to_string(id.value) + ")";
        });

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
        .def("__str__", [](const RegionId& id) {
            return "RegionId(" + std::to_string(id.value) + ")";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // === ENUMERATIONS ===

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
        .value("Resting", FiringState::Resting, "Neuron at rest")
        .value("Active", FiringState::Active, "Neuron actively firing")
        .value("Refractory", FiringState::Refractory, "Neuron in refractory period")
        .value("Inhibited", FiringState::Inhibited, "Neuron inhibited by external signals")
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward")
        .value("MoveLeft", ActionType::MoveLeft, "Move left")
        .value("MoveRight", ActionType::MoveRight, "Move right")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left")
        .value("TurnRight", ActionType::TurnRight, "Turn right")
        .value("Look", ActionType::Look, "Look around")
        .value("LookUp", ActionType::LookUp, "Look up")
        .value("LookDown", ActionType::LookDown, "Look down")
        .value("Interact", ActionType::Interact, "Interact with object")
        .value("Eat", ActionType::Eat, "Eat food")
        .value("Drink", ActionType::Drink, "Drink water")
        .value("Rest", ActionType::Rest, "Rest")
        .value("Wait", ActionType::Wait, "Wait/do nothing")
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
        .value("Empty", WorldObjectType::Empty, "Empty space")
        .value("Resource", WorldObjectType::Resource, "Resource (positive reward)")
        .value("Hazard", WorldObjectType::Hazard, "Hazard (negative reward)")
        .value("Wall", WorldObjectType::Wall, "Wall/boundary")
        .value("Marker", WorldObjectType::Marker, "Navigation marker")
        .export_values();

    // === CONFIGURATION ===

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(Load configuration from a JSON file. Returns True if successful.
             
             Args:
                 filepath: Path to the configuration file
                 
             Returns:
                 bool: True if file was loaded successfully, False otherwise
             )pbdoc")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           R"pbdoc(Load configuration from command line arguments.
           
           Args:
               argc: Argument count
               argv: Argument values
           
           Returns:
               bool: True if arguments were parsed successfully
           )pbdoc")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             R"pbdoc(Save configuration to a JSON file.
             
             Args:
                 filepath: Path where to save the configuration
                 
             Returns:
                 bool: True if saved successfully, False otherwise
             )pbdoc")
        .def("has", &Config::has, py::arg("key"),
             R"pbdoc(Check if a configuration key exists.
             
             Args:
                 key: Configuration key to check
                 
             Returns:
                 bool: True if key exists, False otherwise
             )pbdoc")
        .def("getKeys", &Config::getKeys,
             R"pbdoc(Get all configuration keys.
             
             Returns:
                 list[str]: List of all configuration keys
             )pbdoc")
        .def("clear", &Config::clear,
             R"pbdoc(Clear all configuration entries.)pbdoc")
        .def("summary", &Config::summary,
             R"pbdoc(Get a summary string of the configuration.
             
             Returns:
                 str: Summary of all configuration entries
             )pbdoc")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        });

    // === SENSORY INPUTS ===

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3,
             R"pbdoc(Create a vision sensory input.
             
             Args:
                 width: Image width in pixels
                 height: Image height in pixels  
                 channels: Number of color channels (default: 3)
             )pbdoc")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             R"pbdoc(Set vision data.
             
             Args:
                 data: Vector of pixel values (flattened)
             )pbdoc")
        .def("getWidth", &Vision::getWidth, "Get image width")
        .def("getHeight", &Vision::getHeight, "Get image height")
        .def("getChannels", &Vision::getChannels, "Get number of channels");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             R"pbdoc(Create audio sensory input.
             
             Args:
                 sampleRate: Audio sample rate (Hz)
                 numSamples: Number of audio samples
             )pbdoc")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             R"pbdoc(Set audio data.
             
             Args:
                 data: Vector of audio samples
             )pbdoc")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set audio sample rate")
        .def("getSampleRate", &Audio::getSampleRate, "Get audio sample rate")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of audio samples");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             R"pbdoc(Add an internal signal.
             
             Args:
                 value: Signal value
             )pbdoc")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all signals");

    // === ACTIONS ===

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             R"pbdoc(Create an action with a specific type.
             
             Args:
                 type: Action type
             )pbdoc")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             R"pbdoc(Create an action with type and parameters.
             
             Args:
                 type: Action type
                 parameters: Action parameters
             )pbdoc")
        .def("getType", &Action::getType, "Get action type")
        .def("setType", &Action::setType, py::arg("type"), "Set action type")
        .def("getParameters", &Action::getParameters, "Get action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("getName", &Action::getName, "Get action name")
        .def("clone", &Action::clone, "Create a copy of this action")
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + " at " + std::to_string(action.getType()) + ">";
        });

    // === WORLD OBJECTS ===

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             R"pbdoc(Create a world object.
             
             Args:
                 x: X coordinate
                 y: Y coordinate
                 type: Object type
                 value: Object value (reward for resources, cost for hazards)
                 radius: Object radius (default: 0.5)
             )pbdoc")
        .def_readwrite("x", &WorldObject::x, "X coordinate")
        .def_readwrite("y", &WorldObject::y, "Y coordinate")
        .def_readwrite("radius", &WorldObject::radius, "Object radius")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Object value")
        .def_readwrite("active", &WorldObject::active, "Whether object is active")
        .def("__repr__", [](const WorldObject& obj) {
            return "<WorldObject: " + std::to_string(obj.x) + "," + std::to_string(obj.y) + 
                   " type=" + std::to_string(static_cast<int>(obj.type)) + 
                   " value=" + std::to_string(obj.value) + ">";
        });

    // === AGENT BODY ===

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position")
        .def_readwrite("y", &AgentBody::y, "Y position")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation (radians)")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Energy level (0-1)")
        .def_readwrite("health", &AgentBody::health, "Health (0-1)")
        .def_readwrite("age", &AgentBody::age, "Developmental age")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Is currently moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Is currently turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time of last action")
        .def("reset", &AgentBody::reset, "Reset agent body to initial state");

    // === ACTION RESULTS ===

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             R"pbdoc(Create an action result.
             
             Args:
                 reward: Reward received from action
                 success: Whether action was successful
                 message: Optional message about action result
             )pbdoc")
        .def_readwrite("reward", &ActionResult::reward, "Reward value")
        .def_readwrite("success", &ActionResult::success, "Success status")
        .def_readwrite("message", &ActionResult::message, "Result message")
        .def("__repr__", [](const ActionResult& result) {
            return "<ActionResult: reward=" + std::to_string(result.reward) + 
                   " success=" + std::to_string(result.success) + 
                   " message='" + result.message + "'>";
        });

    // === SENSORY PERCEPT ===

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get vision data")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set vision data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision height")
        .def("getTouch", &SensoryPercept::getTouch, "Get touch data")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch data")
        .def("getInternal", &SensoryPercept::getInternal, "Get internal signals")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception, "Get proprioception")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioception")
        .def("getAudio", &SensoryPercept::getAudio, "Get audio data")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio data")
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all sensory signals")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get sensory timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set sensory timestamp");

    // === SIMPLE WORLD ===

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             R"pbdoc(Configure the world.
             
             Args:
                 width: World width
                 height: World height
                 visionWidth: Vision sensor width
                 visionHeight: Vision sensor height
             )pbdoc")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             R"pbdoc(Set agent starting position.
             
             Args:
                 x: Starting X coordinate
                 y: Starting Y coordinate
             )pbdoc")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             R"pbdoc(Update world state.
             
             Args:
                 timestep: Time step for update
             )pbdoc")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             R"pbdoc(Apply a motor command.
             
             Args:
                 cmd: Motor command to apply
                 currentTime: Current simulation time
                 
             Returns:
                 ActionResult: Result of the action
             )pbdoc")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add an object to the world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             R"pbdoc(Remove object at position.
             
             Args:
                 x: X coordinate
                 y: Y coordinate
             )pbdoc")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             R"pbdoc(Check if position is valid in world.
             
             Args:
                 x: X coordinate
                 y: Y coordinate
                 
             Returns:
                 bool: True if position is valid
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

    // === BRAIN ===

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             R"pbdoc(Create a brain with configuration.
             
             Args:
                 config: Brain configuration
             )pbdoc")
        .def("initialize", &Brain::initialize,
             R"pbdoc(Initialize the brain with configuration.)pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             R"pbdoc(Perform a simulation step.
             
             Args:
                 currentStep: Current simulation step number
             )pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             R"pbdoc(Perform a simulation step with timestamp.
             
             Args:
                 currentStep: Current simulation step number
                 currentTime: Current simulation time
             )pbdoc")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"),
             R"pbdoc(Inject sensory input into the brain.
             
             Args:
                 input: Sensory input to process
             )pbdoc")
        .def("injectCurrent", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"),
             R"pbdoc(Inject current into a specific neuron.
             
             Args:
                 neuron: Target neuron ID
                 current: Current to inject (mA)
             )pbdoc")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             R"pbdoc(Inject current into all neurons of a specific type.
             
             Args:
                 type: Neuron type
                 current: Current to inject (mA)
             )pbdoc")
        .def("produceAction", &Brain::produceAction,
             R"pbdoc(Produce motor action based on neural activity.
             
             Returns:
                 Action: Motor action
             )pbdoc")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             R"pbdoc(Save brain state to file.
             
             Args:
                 filepath: Path to save file
                 
             Returns:
                 bool: True if saved successfully
             )pbdoc")
        .def("load", &Brain::load, py::arg("filepath"),
             R"pbdoc(Load brain state from file.
             
             Args:
                 filepath: Path to load file
                 
             Returns:
                 bool: True if loaded successfully
             )pbdoc")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             R"pbdoc(Add a new neural region.
             
             Args:
                 name: Optional region name
                 
             Returns:
                 RegionId: ID of the new region
             )pbdoc")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             R"pbdoc(Get a region by ID.
             
             Args:
                 id: Region ID
                 
             Returns:
                 NeuralRegion*: Region pointer
             )pbdoc")
        .def("getRegionCount", &Brain::getRegionCount,
             R"pbdoc(Get the number of regions.
             
             Returns:
                 size_t: Number of regions
             )pbdoc")
        .def("getRegionIds", &Brain::getRegionIds,
             R"pbdoc(Get all region IDs.
             
             Returns:
                 list[RegionId]: List of all region IDs
             )pbdoc")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get all regions.
             
             Returns:
                 list[NeuralRegion*]: List of all regions
             )pbdoc")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             R"pbdoc(Get total neuron count across all regions.
             
             Returns:
                 size_t: Total neuron count
             )pbdoc")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             R"pbdoc(Get total synapse count across all regions.
             
             Returns:
                 size_t: Total synapse count
             )pbdoc")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             R"pbdoc(Get count of active neurons.
             
             Returns:
                 size_t: Active neuron count
             )pbdoc")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             R"pbdoc(Get count of currently firing neurons.
             
             Returns:
                 size_t: Firing neuron count
             )pbdoc")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             R"pbdoc(Get average firing rate across all neurons.
             
             Returns:
                 float: Average firing rate (Hz)
             )pbdoc")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             R"pbdoc(Get excitation/inhibition balance ratio.
             
             Returns:
                 float: E/I ratio
             )pbdoc")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             R"pbdoc(Get total spike count.
             
             Returns:
                 size_t: Total spike count
             )pbdoc")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             R"pbdoc(Get current developmental stage.
             
             Returns:
                 DevelopmentalStage: Current developmental stage
             )pbdoc")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"),
             R"pbdoc(Set developmental stage.
             
             Args:
                 stage: New developmental stage
             )pbdoc")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get the configuration.
             
             Returns:
                 Config*: Configuration object
             )pbdoc")
        .def("logStatus", &Brain::logStatus,
             R"pbdoc(Log brain status and statistics.)pbdoc")
        .def("__repr__", [](const Brain& brain) {
            return "<Brain: " + std::to_string(brain.getTotalNeuronCount()) + 
                   " neurons, " + std::to_string(brain.getTotalSynapseCount()) + 
                   " synapses, " + std::to_string(brain.getTotalSpikeCount()) + " spikes>";
        });

    // === AGENT BRAIN ===

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             R"pbdoc(Create an agent brain interface.
             
             Args:
                 brain: Underlying brain object
             )pbdoc")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             R"pbdoc(Initialize with world.
             
             Args:
                 world: World object
             )pbdoc")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             R"pbdoc(Get expected sensory input size.
             
             Returns:
                 size_t: Expected sensory input size
             )pbdoc")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             R"pbdoc(Get expected motor output size.
             
             Returns:
                 size_t: Expected motor output size
             )pbdoc")
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"),
             R"pbdoc(Process sensory percept and inject into brain.
             
             Args:
                 percept: Sensory percept to process
             )pbdoc")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             R"pbdoc(Decode brain motor activity into motor command.
             
             Returns:
                 MotorCommand: Decoded motor command
             )pbdoc")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             R"pbdoc(Apply reward-based neuromodulation.
             
             Args:
                 reward: Actual reward received
                 predictedReward: Predicted reward
             )pbdoc")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"),
             R"pbdoc(Update development system.
             
             Args:
                 timestep: Time step for development
             )pbdoc")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             R"pbdoc(Get current developmental stage.
             
             Returns:
                 DevelopmentalStage: Current developmental stage
             )pbdoc")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             R"pbdoc(Get current neuromodulation level.
             
             Returns:
                 float: Neuromodulation level
             )pbdoc")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             R"pbdoc(Get curiosity level.
             
             Returns:
                 float: Curiosity level
             )pbdoc")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             R"pbdoc(Get novelty level.
             
             Returns:
                 float: Novelty level
             )pbdoc")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             R"pbdoc(Get prediction error.
             
             Returns:
                 float: Prediction error
             )pbdoc")
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get the underlying brain.
             
             Returns:
                 Brain*: Underlying brain object
             )pbdoc")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"),
             R"pbdoc(Enable or disable reward modulation.
             
             Args:
                 enable: Whether to enable reward modulation
             )pbdoc")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"),
             R"pbdoc(Enable or disable structural plasticity.
             
             Args:
                 enable: Whether to enable structural plasticity
             )pbdoc")
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"),
             R"pbdoc(Enable or disable development.
             
             Args:
                 enable: Whether to enable development
             )pbdoc")
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"),
             R"pbdoc(Enable or disable curiosity.
             
             Args:
                 enable: Whether to enable curiosity
             )pbdoc")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             R"pbdoc(Check if reward modulation is enabled.
             
             Returns:
                 bool: True if reward modulation is enabled
             )pbdoc")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             R"pbdoc(Check if structural plasticity is enabled.
             
             Returns:
                 bool: True if structural plasticity is enabled
             )pbdoc")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             R"pbdoc(Check if development is enabled.
             
             Returns:
                 bool: True if development is enabled
             )pbdoc")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             R"pbdoc(Check if curiosity is enabled.
             
             Returns:
                 bool: True if curiosity is enabled
             )pbdoc")
        .def("__repr__", [](const AgentBrain& agent) {
            return "<AgentBrain: connected to brain>";
        });

    // === FACTORY FUNCTIONS ===

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, R"pbdoc(Create a default configuration.
    
    Returns:
        Config: Default configuration object
    )pbdoc");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"),
       R"pbdoc(Create a new brain with configuration.
    
    Args:
        config: Brain configuration
        
    Returns:
        Brain: New brain object
    )pbdoc");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(Create a new simple world.
    
    Returns:
        SimpleWorld: New world object
    )pbdoc");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"),
       R"pbdoc(Create a new agent brain interface.
    
    Args:
        brain: Underlying brain object
        
    Returns:
        AgentBrain: New agent brain interface
    )pbdoc");

    // === CONSTANTS ===

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // === UTILITY FUNCTIONS ===

    // Performance timing utilities
    m.def("time_function", [](std::function<void()> func) {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        return duration.count();
    }, py::arg("func"),
       R"pbdoc(Measure execution time of a function.
    
    Args:
        func: Function to time
        
    Returns:
        int: Execution time in microseconds
    )pbdoc");

    // Configuration validation utility
    m.def("validate_config", [](const Config& config) {
        std::vector<std::string> errors;
        
        if (!config.has("brain.neuron_count")) {
            errors.push_back("Missing required key: brain.neuron_count");
        }
        
        if (config.has("brain.neuron_count")) {
            auto count = config.getOr<int64_t>("brain.neuron_count", 0);
            if (count <= 0) {
                errors.push_back("brain.neuron_count must be positive");
            } else if (count > 1000000) {
                errors.push_back("brain.neuron_count too large (max: 1,000,000)");
            }
        }
        
        if (!config.has("brain.v_thresh")) {
            errors.push_back("Missing recommended key: brain.v_thresh");
        }
        
        return errors;
    }, py::arg("config"),
       R"pbdoc(Validate configuration and return errors.
    
    Args:
        config: Configuration to validate
        
    Returns:
        list[str]: List of validation errors (empty if valid)
    )pbdoc");

    // Configuration preset utilities
    m.def("create_explorer_config", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 1000, ConfigSource::User);
        config->set("brain.v_thresh", -55.0f, ConfigSource::User);
        config->set("brain.v_rest", -70.0f, ConfigSource::User);
        config->set("brain.tau_mem", 20.0f, ConfigSource::User);
        config->set("plasticity.stdp.enable", true, ConfigSource::User);
        config->set("plasticity.hebbian.enable", true, ConfigSource::User);
        config->set("neuromod.dopamine.scale", 1.0f, ConfigSource::User);
        config->set("neuromod.curiosity.enable", true, ConfigSource::User);
        return config;
    }, R"pbdoc(Create an explorer configuration optimized for exploration.
    
    Returns:
        Config: Explorer configuration
    )pbdoc");

    m.def("create_expert_config", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 500, ConfigSource::User);
        config->set("brain.v_thresh", -50.0f, ConfigSource::User);
        config->set("brain.v_rest", -65.0f, ConfigSource::User);
        config->set("brain.tau_mem", 15.0f, ConfigSource::User);
        config->set("plasticity.stdp.enable", true, ConfigSource::User);
        config->set("plasticity.hebbian.enable", true, ConfigSource::User);
        config->set("plasticity.structural.enable", true, ConfigSource::User);
        config->set("neuromod.dopamine.scale", 0.8f, ConfigSource::User);
        config->set("neuromod.curiosity.enable", false, ConfigSource::User);  // Less exploration
        config->set("development.enabled", true, ConfigSource::User);
        return config;
    }, R"pbdoc(Create an expert configuration optimized for stable performance.
    
    Returns:
        Config: Expert configuration
    )pbdoc");

    m.def("create_learner_config", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 2000, ConfigSource::User);
        config->set("brain.v_thresh", -45.0f, ConfigSource::User);
        config->set("brain.v_rest", -75.0f, ConfigSource::User);
        config->set("brain.tau_mem", 25.0f, ConfigSource::User);
        config->set("plasticity.stdp.enable", true, ConfigSource::User);
        config->set("plasticity.hebbian.enable", true, ConfigSource::User);
        config->set("plasticity.structural.enable", true, ConfigSource::User);
        config->set("neuromod.dopamine.scale", 1.2f, ConfigSource::User);
        config->set("neuromod.curiosity.enable", true, ConfigSource::User);
        config->set("neuromod.novelty.enable", true, ConfigSource::User);
        config->set("development.enabled", true, ConfigSource::User);
        return config;
    }, R"pbdoc(Create a learner configuration optimized for rapid learning.
    
    Returns:
        Config: Learner configuration
    )pbdoc");

    // === HELP AND DOCUMENTATION ===

    m.def("print_help", []() {
        std::cout << R"(
NLM Python Library Help
======================

=== Core Classes ===
Brain - Central neural simulation brain
Config - Configuration management
AgentBrain - Interface between brain and world
SimpleWorld - 2D simulation world
Vision - Visual sensory input
Audio - Audio sensory input
InternalSignals - Internal signal processing
Action - Motor actions
WorldObject - Objects in the world
AgentBody - Agent physical state
SensoryPercept - Processed sensory data

=== Key Methods ===
brain.initialize() - Initialize brain
brain.step(step) - Run simulation step
brain.getTotalNeuronCount() - Get neuron count
brain.getTotalSpikeCount() - Get spike count
brain.getAverageFiringRate() - Get average firing rate

agent.processSensoryInput(percept) - Process sensory input
agent.decodeMotorCommand() - Get motor command
agent.applyRewardModulation(reward, predicted) - Apply reward modulation

world.update(timestep) - Update world state
world.getSensoryPercept() - Get sensory percept
world.applyMotorCommand(cmd, time) - Apply motor command

=== Configuration Presets ===
createDefaultConfig() - Basic configuration
create_explorer_config() - Optimized for exploration
create_expert_config() - Optimized for stable performance
create_learner_config() - Optimized for rapid learning

=== Example Usage ===
import pynlm

# Create brain and world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"Simulation complete: {brain.getTotalSpikeCount()} spikes")

=== Error Handling ===
All methods raise Python exceptions on error:
- RuntimeError for runtime issues
- ValueError for invalid values
- IndexError for out-of-bounds access
- LogicError for invalid operations

=== Performance Tips ===
- Use larger batch sizes for simulation steps
- Pre-allocate large arrays when possible
- Use numpy arrays for vector operations
- Monitor memory usage during long simulations
        )" << std::endl;
    }, R"pbdoc(Print comprehensive help and usage examples.)pbdoc");
}

} // namespace nlm
