#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

// Version information
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define PYBIND11_PLUGIN_VERSION "0.1.0"

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
        
        A Python binding for the NLM C++ neural simulation framework.
        
        This module provides access to a brain-inspired spiking neural network
        simulator that implements Leaky Integrate-and-Fire (LIF) neurons with
        event-driven spike propagation, synaptic delays, and multiple plasticity
        mechanisms including STDP, Hebbian learning, and structural plasticity.
        
        Key Features:
        - Event-driven neural simulation
        - Multiple neuron and synapse types
        - Multiple plasticity mechanisms
        - Neuromodulation (dopamine, curiosity, reward prediction)
        - Memory systems (working, episodic, semantic, procedural)
        - Developmental stages
        - Replay and consolidation
        - Closed-loop world interaction
        
        For detailed documentation, visit: https://nlm.readthedocs.io
        
        Version: " PBIND11_PLUGIN_VERSION "
        "Author: NLM Authors"
        "License: MIT"
    )pbdoc";

    // Version attribute
    m.attr("__version__") = PYBIND11_PLUGIN_VERSION;

    // Register runtime error exception
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Neuron ID with improved Python interface
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron.
        
        Attributes:
            value (int): Internal integer value of the neuron ID
        "pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index,
             "Get the index part of the neuron ID")
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        });

    // Synapse ID
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse.
        
        Attributes:
            value (int): Internal integer value of the synapse ID
        "pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return "SynapseId(" + std::to_string(id.value) + ")";
        });

    // Region ID
    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region.
        
        Attributes:
            value (int): Internal integer value of the region ID
        "pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return "RegionId(" + std::to_string(id.value) + ")";
        });

    // Population ID
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population.
        
        Attributes:
            value (int): Internal integer value of the population ID
        "pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Neuron type enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Type of neuron in the NLM system.
        
        Values:
            Excitatory: Regular excitatory neuron
            Inhibitory: Regular inhibitory neuron  
            Modulatory: Neuromodulatory neuron
            Sensory: Sensory input neuron
            Motor: Motor output neuron
            Internal: Internal computation neuron
        "pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Regular excitatory neuron")
        .value("Inhibitory", NeuronType::Inhibitory, "Regular inhibitory neuron")
        .value("Modulatory", NeuronType::Modulatory, "Neuromodulatory neuron")
        .value("Sensory", NeuronType::Sensory, "Sensory input neuron")
        .value("Motor", NeuronType::Motor, "Motor output neuron")
        .value("Internal", NeuronType::Internal, "Internal computation neuron")
        .export_values();

    // Synapse type enumeration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Type of synapse in the NLM system.
        
        Values:
            Excitatory: Standard excitatory synapse
            Inhibitory: Standard inhibitory synapse
            Modulatory: Neuromodulatory synapse
            Electrical: Electrical synapse
            GapJunction: Gap junction synapse
        "pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Standard excitatory synapse")
        .value("Inhibitory", SynapseType::Inhibitory, "Standard inhibitory synapse")
        .value("Modulatory", SynapseType::Modulatory, "Neuromodulatory synapse")
        .value("Electrical", SynapseType::Electrical, "Electrical synapse")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction synapse")
        .export_values();

    // Developmental stage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Developmental stage of the brain.
        
        Values:
            Initial: Early development, high plasticity
            CriticalPeriod: Sensitive learning period
            Maturation: Refining connections
            Adult: Stable adult brain
            Aging: Age-related decline
        "pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Early development, high plasticity")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Sensitive learning period")
        .value("Maturation", DevelopmentalStage::Maturation, "Refining connections")
        .value("Adult", DevelopmentalStage::Adult, "Stable adult brain")
        .value("Aging", DevelopmentalStage::Aging, "Age-related decline")
        .export_values();

    // Firing state enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Current firing state of a neuron.
        
        Values:
            Resting: Not firing
            Active: Currently firing
            Refractory: Cannot fire temporarily
            Inhibited: Suppressed from firing
        "pbdoc")
        .value("Resting", FiringState::Resting, "Not firing")
        .value("Active", FiringState::Active, "Currently firing")
        .value("Refractory", FiringState::Refractory, "Cannot fire temporarily")
        .value("Inhibited", FiringState::Inhibited, "Suppressed from firing")
        .export_values();

    // Action type enumeration
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Type of action that can be performed.
        
        Values:
            MoveForward, MoveBackward, MoveLeft, MoveRight
            TurnLeft, TurnRight, Look, LookUp, LookDown
            Interact, Eat, Drink, Rest, Wait, Custom
        "pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward")
        .value("MoveLeft", ActionType::MoveLeft, "Move left")
        .value("MoveRight", ActionType::MoveRight, "Move right")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left")
        .value("TurnRight", ActionType::TurnRight, "Turn right")
        .value("Look", ActionType::Look, "Look")
        .value("LookUp", ActionType::LookUp, "Look up")
        .value("LookDown", ActionType::LookDown, "Look down")
        .value("Interact", ActionType::Interact, "Interact with object")
        .value("Eat", ActionType::Eat, "Eat food")
        .value("Drink", ActionType::Drink, "Drink water")
        .value("Rest", ActionType::Rest, "Rest")
        .value("Wait", ActionType::Wait, "Wait")
        .value("Custom", ActionType::Custom, "Custom action")
        .export_values();

    // Motor command enumeration
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Motor command for agent action selection.
        
        Values:
            MoveForward, MoveBackward, TurnLeft, TurnRight
            LookLeft, LookRight, Interact, Wait
        "pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right")
        .value("LookLeft", MotorCommand::LookLeft, "Look left")
        .value("LookRight", MotorCommand::LookRight, "Look right")
        .value("Interact", MotorCommand::Interact, "Interact with object")
        .value("Wait", MotorCommand::Wait, "Wait/idle")
        .export_values();

    // World object type enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        Type of object in the world.
        
        Values:
            Empty, Resource, Hazard, Wall, Marker
        "pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty space")
        .value("Resource", WorldObjectType::Resource, "Resource object")
        .value("Hazard", WorldObjectType::Hazard, "Hazard object")
        .value("Wall", WorldObjectType::Wall, "Wall")
        .value("Marker", WorldObjectType::Marker, "Marker object")
        .export_values();

    // Configuration class with enhanced Python interface
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system.
        
        The Config class manages key-value pairs for brain configuration.
        It supports loading from files, command line arguments, and programmatic
        manipulation.
        
        Examples:
            >>> config = pynlm.createDefaultConfig()
            >>> config.has("neuronCount")
            True
            >>> config.get("neuronCount")
            1000
        "pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys as a list")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__contains__", [](const Config& self, const std::string& key) {
            return self.has(key);
        }, py::arg("key"));

    // Vision sensory input
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input representing visual data.
        
        Represents a 2D visual field with width x height x channels.
        
        Attributes:
            width (int): Vision field width
            height (int): Vision field height  
            channels (int): Number of color channels
        "pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3,
             "Create vision input with specified dimensions")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             "Set vision data from a flat vector")
        .def("getWidth", &Vision::getWidth,
             "Get vision field width")
        .def("getHeight", &Vision::getHeight,
             "Get vision field height")
        .def("getChannels", &Vision::getChannels,
             "Get number of color channels");

    // Audio sensory input
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input representing sound data.
        
        Attributes:
            sampleRate (int): Audio sample rate
            numSamples (int): Number of audio samples
        "pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Create audio input with specified sample rate and length")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             "Set audio data from a flat vector")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set audio sample rate")
        .def("getSampleRate", &Audio::getSampleRate,
             "Get audio sample rate")
        .def("getNumSamples", &Audio::getNumSamples,
             "Get number of audio samples");

    // Internal signals sensory input
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal signals sensory input representing brain state.
        
        Used for communication between brain modules.
        "pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all signals");

    // Action representation
    py::class_<Action>(m, "Action", R"pbdoc(
        Action representation for motor output.
        
        Attributes:
            type (ActionType): Type of action
            parameters (list[float]): Action parameters
        "pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create action with specified type")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create action with type and parameters")
        .def("getType", &Action::getType,
             "Get action type")
        .def("setType", &Action::setType, py::arg("type"),
             "Set action type")
        .def("getParameters", &Action::getParameters,
             "Get action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set action parameters")
        .def("getName", &Action::getName,
             "Get human-readable action name")
        .def("clone", &Action::clone,
             "Create a copy of this action");

    // World object representation
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        World object representation.
        
        Attributes:
            x (float): X position
            y (float): Y position
            type (WorldObjectType): Object type
            value (float): Object value (e.g., resource amount)
            radius (float): Object radius
            active (bool): Whether object is active
        "pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Create world object with position, type, and optional value/radius")
        .def_readwrite("x", &WorldObject::x, "X position")
        .def_readwrite("y", &WorldObject::y, "Y position")
        .def_readwrite("radius", &WorldObject::radius, "Object radius")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Object value")
        .def_readwrite("active", &WorldObject::active, "Whether object is active");

    // Agent body state
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state representing physical agent.
        
        Attributes:
            x (float): X position
            y (float): Y position
            orientation (float): Facing direction in radians
            velocityX (float): X velocity
            velocityY (float): Y velocity
            angularVelocity (float): Angular velocity
            energy (float): Current energy level
            health (float): Health status (0-1)
            age (float): Current age in simulation time
            isMoving (bool): Whether agent is moving
            isTurning (bool): Whether agent is turning
            lastActionTime (float): Time since last action
        "pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position")
        .def_readwrite("y", &AgentBody::y, "Y position")
        .def_readwrite("orientation", &AgentBody::orientation, "Facing direction (radians)")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Current energy level")
        .def_readwrite("health", &AgentBody::health, "Health status (0-1)")
        .def_readwrite("age", &AgentBody::age, "Current age in simulation time")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time since last action")
        .def("reset", &AgentBody::reset, "Reset agent body to initial state");

    // Action result
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Result from applying an action to the world.
        
        Attributes:
            reward (float): Reward received from action
            success (bool): Whether action succeeded
            message (str): Description of action result
        "pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create action result with reward, success, and optional message")
        .def_readwrite("reward", &ActionResult::reward, "Reward received")
        .def_readwrite("success", &ActionResult::success, "Whether action succeeded")
        .def_readwrite("message", &ActionResult::message, "Action result description");

    // Sensory percept data
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Sensory percept data from the world.
        
        Contains all sensory inputs that the agent receives.
        
        Attributes:
            vision (Vision): Visual input
            touch (list[float]): Touch/collision input
            internal (list[float]): Internal brain state
            proprioception (list[float]): Body position/velocity
            audio (Audio): Audio input
        "pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision,
             "Get visual input")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set visual input")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth,
             "Get vision field width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight,
             "Get vision field height")
        .def("getTouch", &SensoryPercept::getTouch,
             "Get touch/collision input")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch/collision input")
        .def("getInternal", &SensoryPercept::getInternal,
             "Get internal brain state")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal brain state")
        .def("getProprioception", &SensoryPercept::getProprioception,
             "Get body position/velocity")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set body position/velocity")
        .def("getAudio", &SensoryPercept::getAudio,
             "Get audio input")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio input")
        .def("getAllSignals", &SensoryPercept::getAllSignals,
             "Get all sensory signals as a dictionary")
        .def("getTimestamp", &SensoryPercept::getTimestamp,
             "Get percept timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set percept timestamp");

    // Simple world simulation
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation.
        
        The world provides sensory input and executes motor commands.
        It manages agent position, objects, and the simulation loop.
        
        Attributes:
            width (int): World width in cells
            height (int): World height in cells
            visionWidth (int): Agent vision width
            visionHeight (int): Agent vision height
        "pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and agent vision")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world simulation by timestep seconds")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command and get result")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             "Get agent's current sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             "Get agent's body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add an object to the world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth,
             "Get world width")
        .def("getHeight", &SimpleWorld::getHeight,
             "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             "Get maximum energy level")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy level")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulation")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get current random seed");

    // Brain class with enhanced Python interface
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class.
        
        The Brain class implements the complete neural computation pipeline,
        including LIF neuron dynamics, spike propagation, synaptic plasticity,
        memory systems, and neuromodulation.
        
        Attributes:
            neuronCount (int): Number of neurons in brain
            regionCount (int): Number of neural regions
        "pbdoc")
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
             "Inject current into all neurons of specified type")
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

    // AgentBrain class - brain-world interface
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world.
        
        AgentBrain bridges the gap between the brain and world by handling
        sensory transduction (world → brain) and motor decoding (brain → world).
        
        Attributes:
            sensoryInputSize (int): Size of expected sensory input
            motorOutputSize (int): Size of expected motor output
        "pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface with brain")
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

    // Factory functions for easy Python API
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with sensible settings",
        py::return_value_policy::reference_internal);

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration",
        py::return_value_policy::reference_internal);

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world with default settings",
        py::return_value_policy::reference_internal);

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface",
        py::return_value_policy::reference_internal);

    // Version information
    m.attr("__version__") = PYBIND11_PLUGIN_VERSION;
    m.attr("VERSION") = PYBIND11_PLUGIN_VERSION;

    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Module-level constants
    m.attr("NLM_VERSION_MAJOR") = 0;
    m.attr("NLM_VERSION_MINOR") = 1;
    m.attr("NLM_VERSION_PATCH") = 0;
    m.attr("NLM_VERSION_STRING") = PYBIND11_PLUGIN_VERSION;

    // Common constants
    m.attr("SIMULATION_TIME_STEP") = 0.1f;
    m.attr("MAX_ENERGY") = 100.0f;
    m.attr("ENERGY_DECAY_RATE") = 0.95f;

    // Utility functions
    m.def("getVersionInfo", []() {
        py::dict info;
        info["version"] = PYBIND11_PLUGIN_VERSION;
        info["major"] = 0;
        info["minor"] = 1;
        info["patch"] = 0;
        info["description"] = "NLM Python Bindings";
        return info;
    }, "Get version information as a dictionary");

    m.def("createExampleBrain", []() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, "Create and initialize a simple example brain");
}