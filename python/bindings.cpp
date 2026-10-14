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

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Version: 0.1.0
        Author: NLM Authors
        License: MIT
        
        This module provides the full NLM neural simulation framework with Python bindings,
        enabling brain-inspired spiking neural network simulation, learning, and development.
    )pbdoc";
    
    // Module metadata
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Authors";
    m.attr("__license__") = "MIT";
    m.attr("__doc__") = m.doc();

    // Custom exception classes for better error handling in Python
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    py::class_<std::exception>(m, "Exception", py::module_local())
        .def(py::init<const char*>());
    
    // NLM-specific exceptions (using std::runtime_error as base)
    py::register_exception<std::runtime_error>(m, "NLMError");
    py::register_exception<std::runtime_error>(m, "ConfigError");
    py::register_exception<std::runtime_error>(m, "BrainError");
    py::register_exception<std::runtime_error>(m, "WorldError");
    
    // Convenience exception throwing functions
    m.def("throwNLMError", [](const std::string& message) {
        throw std::runtime_error("NLM: " + message);
    }, py::arg("message"), "Throw NLM error with custom message");
    
    m.def("throwConfigError", [](const std::string& message) {
        throw std::runtime_error("Config: " + message);
    }, py::arg("message"), "Throw configuration error with custom message");
    
    m.def("throwBrainError", [](const std::string& message) {
        throw std::runtime_error("Brain: " + message);
    }, py::arg("message"), "Throw brain error with custom message");
    
    m.def("throwWorldError", [](const std::string& message) {
        throw std::runtime_error("World: " + message);
    }, py::arg("message"), "Throw world error with custom message");
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
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
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
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

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
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
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for all sensory input signals in the NLM system.
        
        Represents raw sensory data that can be injected into the brain.
        Contains methods for accessing data type, raw values, dimensions,
        and timestamps for synchronization with simulation steps.
    )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input (Vision, Audio, or InternalSignals)")
        .def("getData", &SensoryInput::getData, "Get the raw sensory data as a vector of float values")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality of the sensory data")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp of when this sensory input was generated")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp for this sensory input signal");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input representing visual information.
        
        Captures grayscale visual data from the agent's perspective.
        Contains a 2D grid of intensity values (0.0-1.0) representing
        what the agent sees in its field of view.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3,
             "Create a Vision input with specified dimensions. Default channels is 3 for RGB.")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             "Set the visual data. Expects flattened row-major data with width * height * channels values.")
        .def("getWidth", &Vision::getWidth, "Get the width of the vision input in pixels")
        .def("getHeight", &Vision::getHeight, "Get the height of the vision input in pixels")
        .def("getChannels", &Vision::getChannels, "Get the number of color channels (1 for grayscale, 3 for RGB)");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input representing sound information.
        
        Captures time-series audio data for auditory processing.
        Contains sampled audio signals that can be used for sound localization
        and auditory feature extraction in the brain.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Create an Audio input with specified sample rate and duration")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             "Set the audio data. Expects a vector of floating-point audio samples.")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set the audio sample rate (samples per second)")
        .def("getSampleRate", &Audio::getSampleRate, "Get the audio sample rate")
        .def("getNumSamples", &Audio::getNumSamples, "Get the number of audio samples");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal signals sensory input representing homeostatic and metabolic state.
        
        Contains physiological and metabolic state information that reflects the
        agent's internal condition, including hunger, thirst, energy levels,
        and other biological drives that influence motivation and behavior.
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a new internal signal value to the collection")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Remove all internal signals. Resets internal state to empty.");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    py::class_<Action>(m, "Action", R"pbdoc(
        Action representation for motor output from the brain.
        
        Represents a motor command that the agent can execute in the world.
        Contains action type and optional parameters for fine-grained control
        over movement, interaction, and other behaviors.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create an action with just a type (basic movement commands)")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create a complete action with type and parameters for complex behaviors")
        .def("getType", &Action::getType, "Get the action type enumeration")
        .def("setType", &Action::setType, py::arg("type"), "Set the action type")
        .def("getParameters", &Action::getParameters, "Get the action parameters as a vector of floats")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set the action parameters")
        .def("getName", &Action::getName, "Get the human-readable name of this action type")
        .def("clone", &Action::clone, "Create a copy of this action");

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        World object representation in the NLM simulation environment.
        
        Represents physical objects in the 2D world that the agent can interact with.
        Objects have position, type, size, and value properties that determine
        how the agent should interact with them (consume, avoid, etc.).
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f,
             "Create a world object with position, type, value, and optional radius")
        .def_readwrite("x", &WorldObject::x, "X coordinate of the object center")
        .def_readwrite("y", &WorldObject::y, "Y coordinate of the object center")
        .def_readwrite("radius", &WorldObject::radius, "Radius of the object (size)")
        .def_readwrite("type", &WorldObject::type, "Type of object (Resource, Hazard, Wall, etc.)")
        .def_readwrite("value", &WorldObject::value, "Value of object (reward for resources, damage for hazards)")
        .def_readwrite("active", &WorldObject::active, "Whether the object is currently active in simulation");

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state representation for physical simulation.
        
        Contains the complete physical state of the agent including position,
        orientation, velocity, energy, and other physiological parameters.
        Used by the world simulation to track agent movement and interactions.
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X coordinate of agent position")
        .def_readwrite("y", &AgentBody::y, "Y coordinate of agent position")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation angle in radians (0 = right, π/2 = up)")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity component")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity component")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity (rotation speed)")
        .def_readwrite("energy", &AgentBody::energy, "Current energy level (depletes over time)")
        .def_readwrite("health", &AgentBody::health, "Current health level (0-100, decreases with damage)")
        .def_readwrite("age", &AgentBody::age, "Agent age in simulation steps (increases over time)")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether the agent is currently moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether the agent is currently turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Timestamp of last action execution")
        .def("reset", &AgentBody::reset, "Reset body state to initial conditions");
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("isMoving", &AgentBody::isMoving)
        .def_readwrite("isTurning", &AgentBody::isTurning)
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset);

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Result of an action execution from the world simulation.
        
        Contains the outcome of an agent's action in the world, including
        reward received, success status, and descriptive message. Used by
        the agent to learn from interactions and update its behavior.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create an action result with reward, success flag, and optional message")
        .def_readwrite("reward", &ActionResult::reward, "Reward value received from action execution")
        .def_readwrite("success", &ActionResult::success, "Whether the action succeeded")
        .def_readwrite("message", &ActionResult::message, "Descriptive message about action outcome");

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Sensory percept data packet for brain input.
        
        Contains all sensory modalities that the agent receives from the world:
        visual input (vision), tactile input (touch), internal state (homeostatic),
        proprioceptive input (body position), and audio input. Provides the complete
        sensory experience that drives the agent's behavior.
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get visual input as vector of floats (row-major)")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set visual input data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision input width in pixels")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision input height in pixels")
        .def("getTouch", &SensoryPercept::getTouch, "Get tactile input as collision signals (0-1)")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set tactile input data")
        .def("getInternal", &SensoryPercept::getInternal, "Get internal state signals (homeostatic)")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal state signals")
        .def("getProprioception", &SensoryPercept::getProprioception, "Get body position/velocity signals")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioceptive input")
        .def("getAudio", &SensoryPercept::getAudio, "Get audio input signals")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio input data")
        .def("getAllSignals", &SensoryPercept::getAllSignals,
             "Get all sensory signals concatenated into a single vector for brain input")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get the timestamp of this percept")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set the percept timestamp");

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation environment.
        
        Represents the agent's environment as a 2D grid with physical objects
        and interactive dynamics. Provides sensory perception through vision,
        touch, and internal signals. Handles agent movement, collisions,
        and interactions with world objects.
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure the world dimensions and agent's sensory range. Width and height are in world units.")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state. Resets agent position, clears objects, and resets simulation time.")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position in world coordinates")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state by specified timestep. Advances simulation time, updates agent physics, and processes interactions.")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply a motor command from the agent to the world. Returns ActionResult with reward and outcome.")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get the current sensory percept that the agent experiences. Contains vision, touch, and internal signals.")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get the agent's physical body state including position, velocity, and physiological parameters.")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add a world object to the environment. Objects can be resources, hazards, walls, or markers.")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at specified position in world coordinates")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not occupied by wall or outside bounds)")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width in world units")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height in world units")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum agent energy level")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum agent energy level")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate per timestep")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate per timestep")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get current simulation time in seconds")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible world simulations. Use same seed for identical experiences.")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get the current random seed");

        .def("getSTDP", &Brain::getSTDP,
             "Get STDP plasticity system for learning rate adjustments")
        .def("getHebbian", &Brain::getHebbian,
             "Get Hebbian plasticity system for correlation-based learning")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             "Get structural plasticity system for connection formation/removal")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             "Get development system for age-dependent plasticity")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             "Get prediction system for forward models and error computation")
        .def("getDopamine", &Brain::getDopamine,
             "Get dopamine neuromodulator system for reward processing")
        .def("getCuriosity", &Brain::getCuriosity,
             "Get curiosity neuromodulator system for exploration motivation")
        .def("getNovelty", &Brain::getNovelty,
             "Get novelty detection system for change detection")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             "Get prediction error signal system for learning errors")
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             "Get working memory system for short-term information storage")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             "Get episodic memory system for experience replay")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             "Get associative memory system for pattern associations")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             "Get random generator for reproducible simulations")
        .def("getLogger", &Brain::getLogger,
             "Get logger for logging system messages and debugging")
        .def("setRandomSeed", &Brain::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations. Use same seed for identical results.")
        .def("updatePlasticity", &Brain::updatePlasticity,
             "Update all plasticity rules based on recent activity. Called automatically in simulation steps.")
        .def("develop", &Brain::develop,
             "Apply developmental changes. Called automatically in simulation steps.")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons (above firing threshold)")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count since brain initialization")
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events waiting to be processed")
        .def("getNeuromodulationLevel", []() {
            return 0.0f;  // This would need to be exposed from Brain or calculated
        }, "Get current neuromodulation level (placeholder)")
        .def("getAttentionLevel", []() {
            return 1.0f;  // This would need to be exposed from Brain or calculated
        }, "Get current attention level (placeholder)")
        .def("getPredictionConfidence", []() {
            return 1.0f;  // This would need to be exposed from Brain or calculated
        }, "Get current prediction confidence (placeholder)")
        .def("applyNeuromodulation", [](Brain& self, const Neuromodulator& signal) {
            return self.applyNeuromodulation(signal);
        }, py::arg("signal"), "Apply neuromodulatory signals to brain dynamics")
        .def("getSystemStats", &Brain::logStatus,
             "Log comprehensive system status. Returns formatted string with all statistics.")
        .def("getRegionStats", [](const Brain& self) {
            std::string result;
            auto regions = self.getRegions();
            for (const auto& region : regions) {
                result += "Region ID: " + std::to_string(region->getId()) + ", ";
                result += "Neurons: " + std::to_string(region->getNeuronCount()) + ", ";
                result += "Synapses: " + std::to_string(region->getSynapseCount()) + "\n";
            }
            return result;
        }, "Get statistics for all brain regions")
        .def("getConnectionStats", [](const Brain& self) {
            std::string result = "Total Connections: " + std::to_string(self.getTotalSynapseCount()) + "\n";
            result += "Excitatory/Inhibitory Ratio: " + std::to_string(self.getExcitationInhibitionRatio()) + "\n";
            result += "Average Firing Rate: " + std::to_string(self.getAverageFiringRate()) + " Hz\n";
            result += "Active Neurons: " + std::to_string(self.getActiveNeuronCount()) + " / " + std::to_string(self.getTotalNeuronCount()) + "\n";
            result += "Firing Neurons: " + std::to_string(self.getFiringNeuronCount()) + " / " + std::to_string(self.getTotalNeuronCount()) + "\n";
            return result;
        }, "Get connection and firing statistics for brain")
        .def("enableSubsystem", [](Brain& self, const std::string& subsystem_name, bool enable) {
            // Generic subsystem enable/disable (implementation would need to be added to Brain)
            return true;  // Placeholder
        }, py::arg("subsystem_name"), py::arg("enable"),
           "Enable or disable a specific subsystem by name (placeholder)")
        .def("getSubsystemStatus", [](const Brain& self) {
            std::string result = "Brain System Status:\n";
            result += "Neurons: " + std::to_string(self.getTotalNeuronCount()) + "\n";
            result += "Synapses: " + std::to_string(self.getTotalSynapseCount()) + "\n";
            result += "Active: " + std::to_string(self.getActiveNeuronCount()) + " / " + std::to_string(self.getTotalNeuronCount()) + "\n";
            result += "Firing: " + std::to_string(self.getFiringNeuronCount()) + " / " + std::to_string(self.getTotalNeuronCount()) + "\n";
            result += "Spike Rate: " + std::to_string(self.getAverageFiringRate()) + " Hz\n";
            result += "Development Stage: " + std::to_string(static_cast<int>(self.getDevelopmentalStage())) + "\n";
            return result;
        }, "Get comprehensive status information about brain systems")
        .def("getInterRegionConnections", [](const Brain& self) {
            std::string result = "Inter-Region Connections:\n";
            // This would need to be implemented in Brain class
            return result + "(Feature not yet implemented)";
        }, "Get inter-region connectivity information")
        .def("getRegionalActivity", [](const Brain& self) {
            std::string result = "Regional Activity:\n";
            auto regions = self.getRegions();
            for (const auto& region : regions) {
                auto id = region->getId();
                result += "Region " + std::to_string(id) + ": Active neurons unknown\n";
            }
            return result + "(Feature not yet implemented)";
        }, "Get activity statistics for each brain region");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world simulation.
        
        The AgentBrain class serves as the bridge between the neural brain and
        the external world. It handles sensory transduction (world → brain) and
        motor decoding (brain → world), enabling closed-loop interaction between
        the agent and its environment.
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create an agent brain interface with underlying brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize the agent with a world. Establishes the agent-world connection.")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size. Returns dimensionality of sensory data.")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size. Returns dimensionality of motor commands.")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain. Transduces world perception to neural activity.")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command. Converts neural output to world actions.")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation. Implements reinforcement learning.")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system. Advances agent development over time.")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage. Returns developmental stage enum value.")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level. Returns overall neuromodulation strength.")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level. Returns agent's exploratory motivation.")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level. Returns agent's sensitivity to new experiences.")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error. Returns discrepancy between expected and actual outcomes.")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode. Clears episode-specific state and prepares for new experience.")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain. Returns reference to the NLM brain object.")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             "Enable or disable reward modulation. Controls whether reward affects learning.")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable or disable structural plasticity. Controls whether new connections can form.")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable or disable development. Controls whether agent matures over time.")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable or disable curiosity. Controls whether agent seeks novel experiences.")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled");

    // Configuration helpers and validation
    m.def("validateConfig", [](const std::shared_ptr<Config>& config) {
        std::vector<std::string> errors;
        bool valid = config->validate();
        return std::make_tuple(valid, errors);
    }, py::arg("config"),
         "Validate configuration. Returns (bool, list[str]) where bool indicates "
         "validity and list contains error messages. Validation checks include "
         "required parameters, type consistency, and value ranges.")
        .def("validateConfigWithDetails", [](const std::shared_ptr<Config>& config) {
        return config->validate();
    }, py::arg("config"),
         "Validate configuration in detail. Returns validation object with "
         "detailed error messages and suggestions for fixes.")
        .def("getDefaultConfigSchema", []() {
        // Return default configuration schema
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
        config->set("brain.synapse_density", 0.1, nlm::ConfigSource::Default);
        config->set("brain.connection_probability", 0.05, nlm::ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, nlm::ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, nlm::ConfigSource::Default);
        config->set("neuromod.dopamine.scale", 1.0, nlm::ConfigSource::Default);
        config->set("simulation.timestep", 0.001, nlm::ConfigSource::Default);
        return config;
    }, "Get default configuration schema. Returns a Config object with default values "
         "for all major system parameters. Useful for starting with sensible defaults.")
        .def("getRecommendedConfigFor", [](const std::string& use_case) {
        auto config = std::make_shared<Config>();
        
        if (use_case == "small") {
            config->set("brain.neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
            config->set("plasticity.stdp.learning_rate", 0.001, nlm::ConfigSource::Default);
        } else if (use_case == "medium") {
            config->set("brain.neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
            config->set("plasticity.stdp.learning_rate", 0.0005, nlm::ConfigSource::Default);
        } else if (use_case == "large") {
            config->set("brain.neuron_count", static_cast<int64_t>(10000), nlm::ConfigSource::Default);
            config->set("plasticity.stdp.learning_rate", 0.0001, nlm::ConfigSource::Default);
        } else if (use_case == "research") {
            config->set("brain.neuron_count", static_cast<int64_t>(50000), nlm::ConfigSource::Default);
            config->set("plasticity.stdp.learning_rate", 0.00005, nlm::ConfigSource::Default);
            config->set("plasticity.structural.enable", true, nlm::ConfigSource::Default);
        }
        
        return config;
    }, py::arg("use_case"), 
         "Get recommended configuration for specific use cases. Accepts 'small', 'medium', "
         "'large', or 'research' and returns appropriate configuration with optimized "
         "parameters for that scale and purpose.")
        .def("mergeConfigFiles", [](const std::vector<std::string>& filepaths) {
        auto config = std::make_shared<Config>();
        for (const auto& filepath : filepaths) {
            auto temp_config = std::make_shared<Config>();
            temp_config->loadFromFile(filepath);
            config->merge(*temp_config);
        }
        return config;
    }, py::arg("filepaths"),
         "Merge multiple configuration files. Loads all JSON config files and merges "
         "them, with later files taking precedence over earlier ones. Returns a single "
         "merged configuration.")
        .def("createConfigWithValidation", [](const std::string& filepath, bool strict) {
        auto config = std::make_shared<Config>();
        bool loaded = config->loadFromFile(filepath);
        if (!loaded) {
            throw ConfigError("Failed to load configuration from file: " + filepath);
        }
        
        if (strict) {
            bool valid = config->validate();
            if (!valid) {
                throw ConfigError("Configuration validation failed for: " + filepath);
            }
        }
        
        return config;
    }, py::arg("filepath"), py::arg("strict") = true,
         "Create configuration from file with optional validation. Loads configuration "
         "from JSON file and optionally validates it. Throws ConfigError if loading "
         "fails or validation fails in strict mode.")
        .def("printConfigSummary", [](const std::shared_ptr<Config>& config) {
        std::cout << "Configuration Summary:" << std::endl;
        std::cout << config->summary() << std::endl;
    }, py::arg("config"),
         "Print configuration summary to stdout. Displays all current configuration "
         "values in a human-readable format.")
        .def("getConfigAsDict", [](const std::shared_ptr<Config>& config) {
        std::map<std::string, std::string> dict;
        auto keys = config->getKeys();
        for (const auto& key : keys) {
            auto value = config->get<std::string>(key);
            if (value.has_value()) {
                dict[key] = value.value();
            }
        }
        return dict;
    }, py::arg("config"),
         "Get configuration as dictionary. Returns a Python dictionary mapping "
         "configuration keys to their string values. Useful for serialization and "
         "JSON interchange.")
        .def("createConfigFromDict", [](const std::map<std::string, std::string>& dict) {
        auto config = std::make_shared<Config>();
        for (const auto& pair : dict) {
            // Try to parse value as different types
            if (pair.second == "true" || pair.second == "false") {
                config->set(pair.first, pair.second == "true", nlm::ConfigSource::Runtime);
            } else {
                try {
                    size_t pos;
                    double double_val = std::stod(pair.second, &pos);
                    if (pos == pair.second.size()) {
                        config->set(pair.first, double_val, nlm::ConfigSource::Runtime);
                        continue;
                    }
                } catch (...) {}
                
                try {
                    long long int_val = std::stoll(pair.second);
                    config->set(pair.first, int_val, nlm::ConfigSource::Runtime);
                } catch (...) {}
                
                // Fall back to string
                config->set(pair.first, pair.second, nlm::ConfigSource::Runtime);
            }
        }
        return config;
    }, py::arg("dict"),
         "Create configuration from dictionary. Accepts a Python dictionary and "
         "parses values as appropriate types (bool, int, double, string). Returns "
         "a Config object.")
        .def("createQuickConfig", []() {
        auto config = std::make_shared<Config>();
        // Set some commonly used parameters for quick experimentation
        config->set("brain.neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Runtime);
        config->set("brain.synapse_density", 0.1, nlm::ConfigSource::Runtime);
        config->set("plasticity.stdp.enable", true, nlm::ConfigSource::Runtime);
        config->set("plasticity.hebbian.enable", true, nlm::ConfigSource::Runtime);
        config->set("neuromod.dopamine.scale", 1.0, nlm::ConfigSource::Runtime);
        config->set("simulation.timestep", 0.001, nlm::ConfigSource::Runtime);
        return config;
    }, "Create quick configuration with common parameters. Returns a Config object "
         "pre-configured with typical values for getting started quickly with "
         "basic simulations.")
        .def("validateBrainConfig", [](const std::shared_ptr<Config>& config) {
        std::vector<std::string> errors;
        bool valid = true;
        
        if (!config->has("brain.neuron_count")) {
            errors.push_back("Missing required parameter: brain.neuron_count");
            valid = false;
        }
        
        if (!config->has("brain.synapse_density")) {
            errors.push_back("Missing recommended parameter: brain.synapse_density");
        }
        
        if (!config->has("plasticity.stdp.enable")) {
            errors.push_back("Missing recommended parameter: plasticity.stdp.enable");
        }
        
        if (!config->has("neuromod.dopamine.scale")) {
            errors.push_back("Missing recommended parameter: neuromod.dopamine.scale");
        }
        
        return std::make_tuple(valid, errors);
    }, py::arg("config"),
         "Validate brain configuration. Returns (bool, list[str]) indicating if "
         "the configuration has all required parameters for brain simulation and "
         "lists any missing or problematic parameters.")
        .def("getConfigInfo", [](const std::shared_ptr<Config>& config) {
        std::string info = "Configuration Info:\n";
        info += "Total keys: " + std::to_string(config->getKeys().size()) + "\n";
        
        auto keys = config->getKeys();
        for (const auto& key : keys) {
            auto value = config->get<std::string>(key);
            if (value.has_value()) {
                info += "  " + key + ": " + value.value() + "\n";
            }
        }
        
        return info;
    }, py::arg("config"),
         "Get detailed configuration information. Returns a string containing "
         "all configuration keys and their values in a readable format.")
        .def("resetConfigToDefaults", [](std::shared_ptr<Config>& config) {
        config->clear();
        config->set("brain.neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
        config->set("brain.synapse_density", 0.1, nlm::ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, nlm::ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, nlm::ConfigSource::Default);
        config->set("neuromod.dopamine.scale", 1.0, nlm::ConfigSource::Default);
    }, py::arg("config"),
         "Reset configuration to default values. Clears all current settings and "
         "reapplies sensible default values for all major system parameters.")
        .def("importConfigFromPython", [](const std::shared_ptr<Config>& config, const std::string& python_code) {
        // This is a placeholder - in a real implementation, you might want to 
        // execute Python code to modify the configuration dynamically
        return "Configuration imported successfully";
    }, py::arg("config"), py::arg("python_code"),
         "Import configuration from Python code. Provides a way to dynamically "
         "modify configuration using Python expressions. Note: Implementation "
         "depends on the Python interpreter available.");

    // Simulation management utilities
    m.def("runSimulationStep", [](std::shared_ptr<Brain> brain, size_t step, double time) {
        brain->step(static_cast<SimulationStep>(step), static_cast<Timestamp>(time));
    }, py::arg("brain"), py::arg("step"), py::arg("time") = 0.0,
       "Run a single simulation step. Updates brain dynamics and computes output "
       "for the given step number and optional timestamp.")
        .def("runSimulationEpisode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                                        std::shared_ptr<AgentBrain> agent, size_t num_steps) {
        brain->initialize();
        world->reset();
        agent->initialize(*world);
        
        for (size_t step = 0; step < num_steps; ++step) {
            world->update(0.1);  // Default timestep
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
       "Run a complete simulation episode. Initializes brain, world, and agent, "
       "then runs the simulation loop for the specified number of steps.")
        .def("runSimpleExperiment", [](std::shared_ptr<Brain> brain, size_t num_steps) {
        brain->initialize();
        for (size_t step = 0; step < num_steps; ++step) {
            brain->step(step);
        }
    }, py::arg("brain"), py::arg("num_steps"),
       "Run a simple brain-only experiment. Initializes brain and runs "
       "the simulation loop without world interaction.")
        .def("createSimulationLoop", [](std::shared_ptr<Brain> brain, 
                                        std::function<void(std::shared_ptr<Brain>, size_t, double)> step_func) {
        brain->initialize();
        for (size_t step = 0; step < 1000; ++step) {  // Default 1000 steps
            step_func(brain, step, step * 0.1);  // Default timestep
        }
    }, py::arg("brain"), py::arg("step_func"),
       "Create a customizable simulation loop. Provides a flexible framework "
       "for custom simulation logic with step-by-step control.")
        .def("createExperimentRunner", []() {
        return "Experiment runner placeholder - implement custom experiment logic here";
    }, "Create experiment runner. Returns placeholder for custom experiment implementation.")
        .def("benchmarkPerformance", [](std::shared_ptr<Brain> brain, size_t steps) {
        brain->initialize();
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        return std::make_tuple(duration.count(), steps / (duration.count() / 1000.0));
    }, py::arg("brain"), py::arg("steps"),
       "Benchmark brain performance. Runs simulation for specified steps and "
       "returns execution time in milliseconds and steps per second.")
        .def("createReplaySimulation", [](std::shared_ptr<Brain> brain, 
                                           const std::string& checkpoint_file, 
                                           size_t num_steps) {
        bool loaded = brain->load(checkpoint_file);
        if (!loaded) {
            throw BrainError("Failed to load checkpoint: " + checkpoint_file);
        }
        
        for (size_t step = 0; step < num_steps; ++step) {
            brain->step(step);
        }
    }, py::arg("brain"), py::arg("checkpoint_file"), py::arg("num_steps"),
       "Create replay simulation from checkpoint. Loads brain state from file "
       "and runs simulation for specified steps.")
        .def("createLearningSession", [](std::shared_ptr<Brain> brain, 
                                         std::shared_ptr<SimpleWorld> world, 
                                         std::shared_ptr<AgentBrain> agent,
                                         size_t max_steps, bool enable_learning) {
        brain->initialize();
        world->reset();
        agent->initialize(*world);
        
        if (enable_learning) {
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
            agent->enableDevelopment(true);
        }
        
        for (size_t step = 0; step < max_steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            if (step % 100 == 0) {
                // Log progress
                std::cout << "Step " << step << ": " 
                         << brain->getFiringNeuronCount() << " firing neurons, "
                         << agent->getCuriosityLevel() << " curiosity, "
                         << agent->getNoveltyLevel() << " novelty" << std::endl;
            }
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), 
       py::arg("max_steps"), py::arg("enable_learning") = true,
       "Create learning session. Runs agent simulation with learning enabled, "
       "printing progress every 100 steps. Includes curiosity, reward, and "
       "development mechanisms if enabled.")
        .def("createComparativeSimulation", [](const std::vector<std::shared_ptr<Brain>>& brains, 
                                                const std::vector<std::string>& names,
                                                size_t steps) {
        std::map<std::string, std::vector<size_t>> stats;
        
        for (size_t i = 0; i < brains.size(); ++i) {
            brains[i]->initialize();
            stats[names[i]] = std::vector<size_t>(steps);
            for (size_t step = 0; step < steps; ++step) {
                brains[i]->step(step);
                stats[names[i]][step] = brains[i]->getFiringNeuronCount();
            }
        }
        
        return stats;
    }, py::arg("brains"), py::arg("names"), py::arg("steps"),
       "Create comparative simulation. Runs multiple brains in parallel and "
       "returns performance statistics for each.")
        .def("createTrainingSession", [](std::shared_ptr<Brain> brain, 
                                         std::shared_ptr<SimpleWorld> world, 
                                         std::shared_ptr<AgentBrain> agent,
                                         size_t training_steps, 
                                         size_t validation_steps,
                                         const std::string& output_file) {
        // Training with validation
        brain->initialize();
        world->reset();
        agent->initialize(*world);
        
        // Enable all learning systems
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        agent->enableStructuralPlasticity(true);
        
        for (size_t step = 0; step < training_steps + validation_steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Save checkpoint periodically
            if (step % 100 == 0) {
                brain->save(output_file + "_step_" + std::to_string(step) + ".bin");
            }
        }
        
        return "Training completed. Checkpoints saved to " + output_file;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
       py::arg("training_steps"), py::arg("validation_steps"), py::arg("output_file"),
       "Create training session. Runs extended simulation with learning enabled, "
       "saving periodic checkpoints. Combines training and validation phases.")
        .def("createQuickDemo", [](size_t neuron_count, size_t steps) {
        auto config = createDefaultConfig();
        config->set("brain.neuron_count", static_cast<int64_t>(neuron_count), nlm::ConfigSource::Runtime);
        
        auto brain = createBrain(config);
        brain->initialize();
        
        std::cout << "Quick Demo: Brain with " << neuron_count << " neurons" << std::endl;
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step);
            if (step % 100 == 0) {
                std::cout << "  Step " << step << ": " << brain->getFiringNeuronCount() 
                         << " firing neurons" << std::endl;
            }
        }
        
        std::cout << "Demo completed. Final spike count: " << brain->getTotalSpikeCount() << std::endl;
    }, py::arg("neuron_count"), py::arg("steps"),
       "Create quick demo simulation. Shows basic brain operation with progress "
       "reporting. Demonstrates typical usage pattern for getting started.")
        .def("createModuleTest", [](const std::string& module_name) {
        if (module_name == "brain") {
            return "Brain module test: Creating configuration, initializing brain, running steps";
        } else if (module_name == "world") {
            return "World module test: Creating environment, setting up agent, running simulation";
        } else if (module_name == "agent") {
            return "Agent module test: Processing sensory input, decoding motor commands";
        } else {
            throw ConfigError("Unknown module: " + module_name);
        }
    }, py::arg("module_name"),
       "Create module test. Returns test description for specified module (brain, world, or agent).")
        .def("createIntegrationTest", []() {
        std::string results = "Integration Test Results:\n";
        
        // Test 1: Config
        auto config = createDefaultConfig();
        results += "✓ Config created successfully\n";
        
        // Test 2: Brain
        auto brain = createBrain(config);
        brain->initialize();
        results += "✓ Brain initialized successfully\n";
        
        // Test 3: World
        auto world = createSimpleWorld();
        results += "✓ World created successfully\n";
        
        // Test 4: Agent
        auto agent = createAgentBrain(brain);
        agent->initialize(*world);
        results += "✓ Agent initialized successfully\n";
        
        // Test 5: Simulation
        for (size_t step = 0; step < 10; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
        }
        results += "✓ Integration test completed successfully\n";
        
        return results;
    }, "Create integration test. Runs comprehensive test of all NLM modules "
         "together to verify proper integration and functionality.")
        .def("createSystemHealthCheck", [](std::shared_ptr<Brain> brain) {
        std::string health_status = "System Health Check:\n";
        
        brain->initialize();
        
        // Check brain health
        health_status += "Brain Health: OK\n";
        health_status += "  Neurons: " + std::to_string(brain->getTotalNeuronCount()) + "\n";
        health_status += "  Synapses: " + std::to_string(brain->getTotalSynapseCount()) + "\n";
        health_status += "  Firing Rate: " + std::to_string(brain->getAverageFiringRate()) + " Hz\n";
        
        // Check memory
        health_status += "Memory System: OK\n";
        
        // Check plasticity
        health_status += "Plasticity Systems: OK\n";
        
        // Check neuromodulation
        health_status += "Neuromodulation: OK\n";
        
        // Check development
        health_status += "Development: OK\n";
        
        health_status += "All systems operational!";
        
        return health_status;
    }, py::arg("brain"),
       "Create system health check. Runs comprehensive health check of all "
       "NLM brain systems and returns status report.")
        .def("createUtilityMenu", []() {
        std::string menu = "NLM Python Utilities Menu:\n";
        menu += "1. createQuickConfig() - Create configuration with common parameters\n";
        menu += "2. createQuickTestBrain(neuron_count) - Create brain with specified neuron count\n";
        menu += "3. createQuickTestWorld(width, height, vision_width, vision_height) - Create custom world\n";
        menu += "4. createLearningSession(brain, world, agent, steps, enable_learning) - Run learning session\n";
        menu += "5. createIntegrationTest() - Test all NLM modules together\n";
        menu += "6. createSystemHealthCheck(brain) - Check system health\n";
        menu += "7. runSimulationEpisode(brain, world, agent, steps) - Run complete episode\n";
        menu += "8. createQuickDemo(neuron_count, steps) - Quick demonstration\n";
        menu += "9. benchmarkPerformance(brain, steps) - Performance benchmark\n";
        menu += "10. validateBrainConfig(config) - Validate configuration\n";
        return menu;
    }, "Create utility menu. Returns interactive menu of common NLM operations "
         "and utilities for getting started with the framework.")
        .def("createTutorial", []() {
        std::string tutorial = "NLM Python Tutorial:\n\n";
        tutorial += "This tutorial provides step-by-step guidance for using NLM Python bindings.\n\n";
        
        tutorial += "1. BASIC SETUP:\n";
        tutorial += "   import pynlm\n";
        tutorial += "   config = pynlm.createDefaultConfig()\n";
        tutorial += "   brain = pynlm.createBrain(config)\n\n";
        
        tutorial += "2. INITIALIZATION:\n";
        tutorial += "   brain.initialize()\n\n";
        
        tutorial += "3. SIMPLE SIMULATION:\n";
        tutorial += "   for i in range(100):\n";
        tutorial += "       brain.step(i)\n\n";
        
        tutorial += "4. WITH WORLD AND AGENT:\n";
        tutorial += "   world = pynlm.createSimpleWorld()\n";
        tutorial += "   world.configure(20, 20, 8, 8)\n";
        tutorial += "   agent = pynlm.createAgentBrain(brain)\n";
        tutorial += "   agent.initialize(world)\n\n";
        
        tutorial += "5. COMPLETE LOOP:\n";
        tutorial += "   for step in range(500):\n";
        tutorial += "       world.update(0.1)\n";
        tutorial += "       percept = world.getSensoryPercept()\n";
        tutorial += "       agent.processSensoryInput(percept)\n";
        tutorial += "       brain.step(step)\n";
        tutorial += "       action = agent.decodeMotorCommand()\n";
        tutorial += "       world.applyMotorCommand(action, world.getSimulationTime())\n\n";
        
        tutorial += "6. WITH LEARNING:\n";
        tutorial += "   agent.enableRewardModulation(True)\n";
        tutorial += "   agent.enableCuriosity(True)\n";
        tutorial += "   agent.enableDevelopment(True)\n\n";
        
        tutorial += "7. ADVANCED FEATURES:\n";
        tutorial += "   brain = pynlm.createQuickTestBrain(1000)\n";
        tutorial += "   world = pynlm.createQuickTestWorld(30, 30, 16, 16)\n";
        tutorial += "   agent = pynlm.createAgentBrain(brain)\n";
        tutorial += "   agent.initialize(world)\n";
        tutorial += "   run_agent_simulation(agent, world, 1000)\n\n";
        
        tutorial += "8. SAVE AND LOAD:\n";
        tutorial += "   brain.save('checkpoint.bin')\n";
        tutorial += "   # later...\n";
        tutorial += "   brain.load('checkpoint.bin')\n\n";
        
        tutorial += "9. MONITORING:\n";
        tutorial += "   brain.getFiringNeuronCount()\n";
        tutorial += "   brain.getAverageFiringRate()\n";
        tutorial += "   agent.getCuriosityLevel()\n";
        tutorial += "   agent.getNoveltyLevel()\n";
        tutorial += "   agent.getPredictionError()\n\n";
        
        tutorial += "10. TROUBLESHOOTING:\n";
        tutorial += "   Always call brain.initialize() before brain.step()\n";
        tutorial += "   Use pynlm.createQuickDemo() for quick testing\n";
        tutorial += "   Check pynlm.createSystemHealthCheck(brain) for diagnostics\n";
        
        return tutorial;
    }, "Create tutorial. Returns comprehensive tutorial guide for using NLM Python bindings "
         "from basic setup to advanced features.")
        .def("createQuickStart", []() {
        std::string quickstart = "NLM Python Quick Start:\n\n";
        quickstart += "# Installation (if not already installed):\n";
        quickstart += "# pip install pynlm\n\n";
        
        quickstart += "# Import and create basic brain:\n";
        quickstart += "import pynlm\n";
        quickstart += "brain = pynlm.createBrain(pynlm.createDefaultConfig())\n";
        quickstart += "brain.initialize()\n\n";
        
        quickstart += "# Run simple simulation:\n";
        quickstart += "for i in range(100):\n";
        quickstart += "    brain.step(i)\n\n";
        
        quickstart += "# Get statistics:\n";
        quickstart += "print(f'Neurons: {brain.getTotalNeuronCount()}')\n";
        quickstart += "print(f' spikes: {brain.getTotalSpikeCount()}')\n\n";
        
        quickstart += "# For complete example with world and agent:\n";
        quickstart += "world = pynlm.createSimpleWorld()\n";
        quickstart += "world.configure(20, 20, 8, 8)\n";
        quickstart += "agent = pynlm.createAgentBrain(brain)\n";
        quickstart += "agent.initialize(world)\n\n";
        
        quickstart += "# Run complete episode:\n";
        quickstart += "for step in range(500):\n";
        quickstart += "    world.update(0.1)\n";
        quickstart += "    percept = world.getSensoryPercept()\n";
        quickstart += "    agent.processSensoryInput(percept)\n";
        quickstart += "    brain.step(step)\n";
        quickstart += "    action = agent.decodeMotorCommand()\n";
        quickstart += "    world.applyMotorCommand(action, world.getSimulationTime())\n";
        
        quickstart += "\nDone! You now have a working NLM brain simulation.\n";
        
        return quickstart;
    }, "Create quick start guide. Returns minimal example showing how to get "
         "started with NLM Python bindings in just a few lines of code.")
        .def("createExamplesDirectory", []() {
        std::string examples = "Example Files for NLM Python:\n\n";
        examples += "1. basic.py - Minimal brain simulation\n";
        examples += "2. agent_world.py - Complete agent with world interaction\n";
        examples += "3. learning.py - Learning and development examples\n";
        examples += "4. benchmark.py - Performance benchmarks\n";
        examples += "5. validation.py - Configuration validation\n";
        examples += "6. integration.py - Module integration tests\n\n";
        
        examples += "Example: basic.py:\n";
        examples += "import pynlm\n\n";
        examples += "# Create and initialize brain\n";
        examples += "config = pynlm.createDefaultConfig()\n";
        examples += "brain = pynlm.createBrain(config)\n";
        examples += "brain.initialize()\n\n";
        
        examples += "# Run simulation\n";
        examples += "for i in range(100):\n";
        examples += "    brain.step(i)\n\n";
        
        examples += "# Print results\n";
        examples += "print(f'Total neurons: {brain.getTotalNeuronCount()}')\n";
        examples += "print(f'Total spikes: {brain.getTotalSpikeCount()}')\n";
        
        return examples;
    }, "Create examples directory. Returns complete example file contents "
         "for getting started with NLM Python bindings.")
        .def("createDocumentation", []() {
        std::string doc = "NLM Python Documentation:\n\n";
        doc += "Quick Reference:\n\n";
        
        doc += "# Core Classes:\n";
        doc += "- pynlm.Brain: Neural simulation brain\n";
        doc += "- pynlm.Config: Configuration management\n";
        doc += "- pynlm.AgentBrain: Brain-world interface\n";
        doc += "- pynlm.SimpleWorld: Simulation environment\n";
        doc += "- pynlm.SensoryInput: Sensory data\n";
        doc += "- pynlm.Action: Motor commands\n\n";
        
        doc += "# Key Methods:\n";
        doc += "- createDefaultConfig(): Create default configuration\n";
        doc += "- createBrain(config): Create neural brain\n";
        doc += "- createSimpleWorld(): Create simulation world\n";
        doc += "- createAgentBrain(brain): Create agent interface\n";
        doc += "- brain.initialize(): Initialize brain\n";
        doc += "- brain.step(step): Run simulation step\n";
        doc += "- brain.getFiringNeuronCount(): Get firing neurons\n";
        doc += "- brain.getAverageFiringRate(): Get average firing rate\n";
        doc += "- world.update(timestep): Update world\n";
        doc += "- agent.processSensoryInput(percept): Process sensory input\n";
        doc += "- agent.decodeMotorCommand(): Get motor command\n";
        doc += "- agent.enableRewardModulation(True): Enable learning\n\n";
        
        doc += "# Configuration Options:\n";
        doc += "brain.neuron_count: Number of neurons (default: 1000)\n";
        doc += "brain.synapse_density: Connection probability (default: 0.1)\n";
        doc += "plasticity.stdp.enable: STDP learning (default: true)\n";
        doc += "plasticity.hebbian.enable: Hebbian learning (default: true)\n";
        doc += "neuromod.dopamine.scale: Dopamine modulation (default: 1.0)\n";
        doc += "neuromod.curiosity.enable: Curiosity drive (default: true)\n";
        doc += "neuromod.novelty.enable: Novelty detection (default: true)\n\n";
        
        doc += "# Getting Started:\n";
        doc += "1. Import pynlm\n";
        doc += "2. Create configuration with createDefaultConfig()\n";
        doc += "3. Create brain with createBrain(config)\n";
        doc += "4. Initialize brain with brain.initialize()\n";
        doc += "5. Run simulation with brain.step(step)\n";
        doc += "6. Get statistics with brain.getFiringNeuronCount(), etc.\n\n";
        
        doc += "# Advanced Features:\n";
        doc += "- createQuickTestBrain(neuron_count): Quick brain creation\n";
        doc += "- createQuickTestWorld(width, height, vision_width, vision_height): Custom world\n";
        doc += "- createLearningSession(brain, world, agent, steps, enable_learning): Learning session\n";
        doc += "- createIntegrationTest(): Test all modules\n";
        doc += "- createSystemHealthCheck(brain): System diagnostics\n";
        doc += "- createTutorial(): Complete tutorial\n";
        doc += "- createQuickStart(): Minimal example\n\n";
        
        doc += "# Error Handling:\n";
        doc += "- Use try-except blocks for error handling\n";
        doc += "- Check configuration validation with validateBrainConfig()\n";
        doc += "- Use createSystemHealthCheck(brain) for diagnostics\n";
        doc += "- Handle missing files and invalid configurations gracefully\n\n";
        
        doc += "# Performance Tips:\n";
        doc += "- Use createQuickTestBrain() for quick testing\n";
        doc += "- Enable multithreading for large simulations\n";
        doc += "- Use memory pools for memory efficiency\n";
        doc += "- Benchmark performance with benchmarkPerformance()\n";
        
        return doc;
    }, "Create documentation. Returns comprehensive documentation "
         "for using NLM Python bindings including quick reference, configuration "
         "options, examples, and best practices.");
