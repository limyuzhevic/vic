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
        
        This module provides an interface to the NLM neural simulation framework, allowing
        users to create and simulate virtual brains with neurons, synapses, and learning capabilities.
        The API is designed to be intuitive and Pythonic, with comprehensive error handling
        and documentation.
        
        Key features:
        - Create and configure virtual brains with LIF neurons
        - Real-time neural simulation with plasticity (STDP, Hebbian, reward modulation)
        - Agent-based interaction with environments
        - Developmental stages and maturation
        - Checkpoint saving/loading for persistence
        - Advanced visualization and analysis tools
        
        For beginners, use the easy_usage.md guide. For advanced features, see HOW_TO_USE.md.
    )pbdoc";

    // Register Python exceptions for better error handling
    py::register_exception<std::runtime_error>(m, "RuntimeError", "Runtime error in NLM simulation");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError", "Invalid argument provided to NLM simulation");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError", "Index out of range in NLM simulation");
    py::register_exception<std::logic_error>(m, "LogicError", "Logical error in NLM simulation");

    // Neuron ID - Unique identifier for a neuron
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index, "Get the index part of the neuron ID")
        .def("__eq__", &NeuronId::operator==, "Check if two neuron IDs are equal")
        .def("__ne__", &NeuronId::operator!=, "Check if two neuron IDs are not equal")
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); }, 
             "Get hash of neuron ID for use in sets and dictionaries")
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        }, "String representation of neuron ID");

    // Synapse ID - Unique identifier for a synapse
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index, "Get the index part of the synapse ID")
        .def("__eq__", &SynapseId::operator==, "Check if two synapse IDs are equal")
        .def("__ne__", &SynapseId::operator!=, "Check if two synapse IDs are not equal")
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); }, 
             "Get hash of synapse ID for use in sets and dictionaries")
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        }, "String representation of synapse ID");

    // Region ID - Unique identifier for a brain region
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index, "Get the index part of the region ID")
        .def("__eq__", &RegionId::operator==, "Check if two region IDs are equal")
        .def("__ne__", &RegionId::operator!=, "Check if two region IDs are not equal")
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); }, 
             "Get hash of region ID for use in sets and dictionaries")
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        }, "String representation of region ID");

    // Population ID - Unique identifier for a neuron population
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index, "Get the index part of the population ID")
        .def("__eq__", &PopulationId::operator==, "Check if two population IDs are equal")
        .def("__ne__", &PopulationId::operator!=, "Check if two population IDs are not equal");

    // Neuron Type enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)")
        .value("Excitatory", NeuronType::Excitatory, "Excitatory neuron")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neuron")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neuron (neuromodulator)")
        .value("Sensory", NeuronType::Sensory, "Sensory neuron (receives external input)")
        .value("Motor", NeuronType::Motor, "Motor neuron (produces actions)")
        .value("Internal", NeuronType::Internal, "Internal neuron (local processing)")
        .export_values();

    // Synapse Type enumeration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory synapse")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synapse")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synapse")
        .value("Electrical", SynapseType::Electrical, "Electrical synapse (gap junction)")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction synapse")
        .export_values();

    // Developmental Stage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage (high plasticity)")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period (enhanced learning)")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation phase")
        .value("Adult", DevelopmentalStage::Adult, "Adult stage (stable performance)")
        .value("Aging", DevelopmentalStage::Aging, "Aging stage (declining plasticity)")
        .export_values();

    // Firing State enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)")
        .value("Resting", FiringState::Resting, "Neuron at rest (not firing)")
        .value("Active", FiringState::Active, "Neuron is active (potential above threshold)")
        .value("Refractory", FiringState::Refractory, "Neuron in refractory period")
        .value("Inhibited", FiringState::Inhibited, "Neuron inhibited by strong input")
        .export_values();

    // Action Type enumeration
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)")
        .value("MoveForward", ActionType::MoveForward, "Move forward action")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward action")
        .value("MoveLeft", ActionType::MoveLeft, "Move left action")
        .value("MoveRight", ActionType::MoveRight, "Move right action")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left action")
        .value("TurnRight", ActionType::TurnRight, "Turn right action")
        .value("Look", ActionType::Look, "Look action")
        .value("LookUp", ActionType::LookUp, "Look up action")
        .value("LookDown", ActionType::LookDown, "Look down action")
        .value("Interact", ActionType::Interact, "Interact action")
        .value("Eat", ActionType::Eat, "Eat action")
        .value("Drink", ActionType::Drink, "Drink action")
        .value("Rest", ActionType::Rest, "Rest action")
        .value("Wait", ActionType::Wait, "Wait action")
        .value("Custom", ActionType::Custom, "Custom action")
        .export_values();

    // Motor Command enumeration
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right")
        .value("LookLeft", MotorCommand::LookLeft, "Look left")
        .value("LookRight", MotorCommand::LookRight, "Look right")
        .value("Interact", MotorCommand::Interact, "Interact with environment")
        .value("Wait", MotorCommand::Wait, "Wait/idle action")
        .export_values();

    // World Object Type enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)")
        .value("Empty", WorldObjectType::Empty, "Empty space")
        .value("Resource", WorldObjectType::Resource, "Resource object (e.g., food)")
        .value("Hazard", WorldObjectType::Hazard, "Hazard object (e.g., poison)")
        .value("Wall", WorldObjectType::Wall, "Wall/obstacle")
        .value("Marker", WorldObjectType::Marker, "Marker/landmark")
        .export_values();

    // Configuration class for NLM system
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)")
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
        }, "String representation of configuration")
        .def("set", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value, ConfigSource::Default);
        }, py::arg("key"), py::arg("value"),
            "Set an integer configuration parameter")
        .def("set", [](Config& self, const std::string& key, float value) {
            self.set(key, value, ConfigSource::Default);
        }, py::arg("key"), py::arg("value"),
            "Set a float configuration parameter")
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value, ConfigSource::Default);
        }, py::arg("key"), py::arg("value"),
            "Set a double configuration parameter")
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value, ConfigSource::Default);
        }, py::arg("key"), py::arg("value"),
            "Set a boolean configuration parameter")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, ConfigSource::Default);
        }, py::arg("key"), py::arg("value"),
            "Set a string configuration parameter")
        .def("get", [](Config& self, const std::string& key, int64_t defaultValue) {
            return self.getOr<int64_t>(key, defaultValue);
        }, py::arg("key"), py::arg("default") = 0,
            "Get an integer configuration parameter with default")
        .def("get", [](Config& self, const std::string& key, float defaultValue) {
            return self.getOr<float>(key, defaultValue);
        }, py::arg("key"), py::arg("default") = 0.0f,
            "Get a float configuration parameter with default")
        .def("get", [](Config& self, const std::string& key, double defaultValue) {
            return self.getOr<double>(key, defaultValue);
        }, py::arg("key"), py::arg("default") = 0.0,
            "Get a double configuration parameter with default")
        .def("get", [](Config& self, const std::string& key, bool defaultValue) {
            return self.getOr<bool>(key, defaultValue);
        }, py::arg("key"), py::arg("default") = false,
            "Get a boolean configuration parameter with default")
        .def("get", [](Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("default") = "",
            "Get a string configuration parameter with default");

    // Sensory Input base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision sensory input
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth, "Get the width of the vision data")
        .def("getHeight", &Vision::getHeight, "Get the height of the vision data")
        .def("getChannels", &Vision::getChannels, "Get the number of channels in the vision data");

    // Audio sensory input
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate, "Get the audio sample rate")
        .def("getNumSamples", &Audio::getNumSamples, "Get the number of audio samples");

    // Internal signals sensory input
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value to the internal signals")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all internal signals");

    // Action representation for motor output
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType, "Get the action type")
        .def("setType", &Action::setType, py::arg("type"), "Set the action type")
        .def("getParameters", &Action::getParameters, "Get the action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set the action parameters")
        .def("getName", &Action::getName, "Get the action name")
        .def("clone", &Action::clone, "Create a copy of this action");

    // World object representation
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x, "Object x coordinate")
        .def_readwrite("y", &WorldObject::y, "Object y coordinate")
        .def_readwrite("radius", &WorldObject::radius, "Object radius")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Object value")
        .def_readwrite("active", &WorldObject::active, "Whether object is active");

    // Agent body state
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position")
        .def_readwrite("y", &AgentBody::y, "Y position")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation angle in radians")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Current energy level")
        .def_readwrite("health", &AgentBody::health, "Health status (0-100)")
        .def_readwrite("age", &AgentBody::age, "Age in simulation time")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time of last action")
        .def("reset", &AgentBody::reset, "Reset agent body to initial state");

    // Action result from world
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward, "Reward received from action")
        .def_readwrite("success", &ActionResult::success, "Whether action was successful")
        .def_readwrite("message", &ActionResult::message, "Message describing action result");

    // Sensory percept data
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)")
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
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get sensory percept timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set sensory percept timestamp");

    // Simple world for NLM simulation
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure the world with dimensions and sensor range")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set the agent's starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world for given timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply a motor command to the world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get the current sensory percept from the world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get the agent's body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add an object to the world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove an object from the world at given coordinates")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid in the world")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed for world")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed used by world");

    // Brain class - central neural simulation brain
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create a brain with configuration")
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
             "Add a new neural region with optional name")
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
             "Log brain status")
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
             "Get novelty detection system")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error signal");

    // AgentBrain class - connects NLM brain to world
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface with underlying brain")
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
             "Enable or disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable or disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable or disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable or disable curiosity-driven exploration")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled");

    // Convenience factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with standard NLM parameters");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world for NLM simulation");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Constants and invalid IDs
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Error handling utilities
    py::register_exception<std::runtime_error>(m, "RuntimeError", "Runtime error in NLM simulation");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError", "Invalid argument provided to NLM simulation");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError", "Index out of range in NLM simulation");
    py::register_exception<std::logic_error>(m, "LogicError", "Logical error in NLM simulation");

    // Example utilities for quick testing
    m.def("run_simple_demo", [](int num_steps = 100) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        for (int step = 0; step < num_steps; ++step) {
            brain->step(step);
        }
        
        return brain->getTotalSpikeCount();
    }, py::arg("num_steps") = 100,
        "Run a simple demonstration simulation with default parameters");

    // Advanced utility for creating learning environments
    m.def("create_learning_environment", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 3);
        config->set("connection_probability", 0.15);
        config->set("plasticity_stdp_enable", true);
        config->set("plasticity_hebbian_enable", true);
        config->set("neuromod_curiosity_enable", true);
        config->set("neuromod_novelty_enable", true);
        
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        return std::make_tuple(brain, world, agent);
    }, "Create a learning environment with all advanced features enabled");

    // Utility for statistical analysis
    m.def("compute_neural_statistics", [](std::shared_ptr<Brain> brain) {
        std::unordered_map<std::string, double> stats;
        
        stats["total_neurons"] = static_cast<double>(brain->getTotalNeuronCount());
        stats["total_synapses"] = static_cast<double>(brain->getTotalSynapseCount());
        stats["firing_neurons"] = static_cast<double>(brain->getFiringNeuronCount());
        stats["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        
        if (brain->getWorkingMemory()) {
            stats["working_memory_size"] = brain->getWorkingMemory()->getCapacity();
        }
        
        if (brain->getEpisodicMemory()) {
            stats["episodic_memory_episodes"] = brain->getEpisodicMemory()->getEpisodeCount();
        }
        
        return stats;
    }, py::arg("brain"), "Compute comprehensive neural network statistics");

    // Performance monitoring and analysis utilities
    m.def("monitor_neural_activity", [](std::shared_ptr<Brain> brain, int duration_steps = 100) {
        std::vector<double> firing_rates;
        std::vector<size_t> spike_counts;
        
        for (int step = 0; step < duration_steps; ++step) {
            brain->step(step);
            firing_rates.push_back(brain->getAverageFiringRate());
            spike_counts.push_back(brain->getTotalSpikeCount());
        }
        
        std::unordered_map<std::string, std::vector<double>> monitoring_data;
        monitoring_data["firing_rate"] = firing_rates;
        monitoring_data["spike_count"] = spike_counts;
        
        return monitoring_data;
    }, py::arg("brain"), py::arg("duration_steps") = 100,
        "Monitor neural activity over time, returning firing rate and spike count history");

    // Configuration comparison utility
    m.def("compare_configs", [](const std::shared_ptr<Config>& config1, const std::shared_ptr<Config>& config2) {
        std::unordered_map<std::string, std::vector<std::pair<std::string, std::string>>> differences;
        
        // Get all keys from both configs
        auto keys1 = config1->getKeys();
        auto keys2 = config2->getKeys();
        
        // Check keys in config1 but not in config2
        for (const auto& key : keys1) {
            if (!config2->has(key)) {
                differences["missing_in_config2"].push_back({key, config1->getOr<std::string>(key, "")});
            }
        }
        
        // Check keys in config2 but not in config1
        for (const auto& key : keys2) {
            if (!config1->has(key)) {
                differences["missing_in_config1"].push_back({key, config2->getOr<std::string>(key, "")});
            }
        }
        
        // Check different values for common keys
        std::vector<std::string> common_keys;
        for (const auto& key : keys1) {
            if (config2->has(key)) {
                common_keys.push_back(key);
            }
        }
        
        for (const auto& key : common_keys) {
            auto value1 = config1->getOr<std::string>(key, "");
            auto value2 = config2->getOr<std::string>(key, "");
            if (value1 != value2) {
                differences["different_values"].push_back({key, value1 + " -> " + value2});
            }
        }
        
        return differences;
    }, py::arg("config1"), py::arg("config2"),
        "Compare two configurations and return differences");

    // Advanced simulation benchmarking
    m.def("benchmark_simulation", [](int duration_steps, const std::string& description = "") {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 3);
        config->set("connection_probability", 0.1);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain in benchmark");
        }
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < duration_steps; ++step) {
            brain->step(step);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        std::unordered_map<std::string, double> benchmark_results;
        benchmark_results["duration_ms"] = static_cast<double>(duration.count());
        benchmark_results["steps_per_second"] = static_cast<double>(duration_steps) * 1000.0 / duration.count();
        benchmark_results["neurons"] = static_cast<double>(brain->getTotalNeuronCount());
        benchmark_results["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
        benchmark_results["final_firing_rate"] = brain->getAverageFiringRate();
        
        if (!description.empty()) {
            benchmark_results["description"] = description;
        }
        
        return benchmark_results;
    }, py::arg("duration_steps"), py::arg("description") = "",
        "Run a simulation benchmark and return performance metrics");

    // Memory profiling utility
    m.def("profile_memory_usage", [](std::shared_ptr<Brain> brain, int steps = 100) {
        std::vector<size_t> memory_usage;
        
        for (int step = 0; step < steps; ++step) {
            brain->step(step);
            memory_usage.push_back(brain->getTotalNeuronCount());
        }
        
        std::unordered_map<std::string, std::vector<size_t>> profile_data;
        profile_data["neuron_count"] = memory_usage;
        
        return profile_data;
    }, py::arg("brain"), py::arg("steps") = 100,
        "Profile memory usage during simulation, tracking neuron count over time");

    // Advanced analysis functions
    m.def("analyze_learning_dynamics", [](std::shared_ptr<Brain> brain, int steps = 500) {
        std::vector<double> firing_rates;
        std::vector<size_t> spike_counts;
        std::vector<double> eir;
        
        for (int step = 0; step < steps; ++step) {
            brain->step(step);
            firing_rates.push_back(brain->getAverageFiringRate());
            spike_counts.push_back(brain->getTotalSpikeCount());
            eir.push_back(brain->getExcitationInhibitionRatio());
        }
        
        std::unordered_map<std::string, std::vector<double>> analysis_results;
        analysis_results["firing_rate"] = firing_rates;
        analysis_results["spike_count"] = spike_counts;
        analysis_results["eir"] = eir;
        
        // Calculate summary statistics
        double avg_firing_rate = std::accumulate(firing_rates.begin(), firing_rates.end(), 0.0) / firing_rates.size();
        double avg_spike_count = std::accumulate(spike_counts.begin(), spike_counts.end(), 0.0) / spike_counts.size();
        double avg_eir = std::accumulate(eir.begin(), eir.end(), 0.0) / eir.size();
        
        analysis_results["avg_firing_rate"] = avg_firing_rate;
        analysis_results["avg_spike_count"] = avg_spike_count;
        analysis_results["avg_eir"] = avg_eir;
        
        // Calculate stability metrics (coefficient of variation)
        double firing_rate_var = 0.0, spike_count_var = 0.0, eir_var = 0.0;
        for (size_t i = 0; i < firing_rates.size(); ++i) {
            firing_rate_var += (firing_rates[i] - avg_firing_rate) * (firing_rates[i] - avg_firing_rate);
            spike_count_var += (static_cast<double>(spike_counts[i]) - avg_spike_count) * (static_cast<double>(spike_counts[i]) - avg_spike_count);
            eir_var += (eir[i] - avg_eir) * (eir[i] - avg_eir);
        }
        
        double firing_rate_cv = std::sqrt(firing_rate_var / firing_rates.size()) / avg_firing_rate;
        double spike_count_cv = std::sqrt(spike_count_var / spike_counts.size()) / avg_spike_count;
        double eir_cv = std::sqrt(eir_var / eir.size()) / avg_eir;
        
        analysis_results["firing_rate_cv"] = firing_rate_cv;
        analysis_results["spike_count_cv"] = spike_count_cv;
        analysis_results["eir_cv"] = eir_cv;
        
        return analysis_results;
    }, py::arg("brain"), py::arg("steps") = 500,
        "Analyze learning dynamics including firing rates, spike counts, and E/I balance");

    // Batch simulation runner for parameter sweeps
    m.def("run_batch_simulation", [](const std::vector<std::unordered_map<std::string, double>>& parameters, int base_steps = 100) {
        std::vector<std::unordered_map<std::string, double>> results;
        
        for (const auto& param_set : parameters) {
            auto config = std::make_shared<Config>();
            
            // Apply parameters
            if (param_set.find("neuron_count") != param_set.end()) {
                config->set("neuron_count", static_cast<int64_t>(param_set.at("neuron_count")));
            }
            if (param_set.find("region_count") != param_set.end()) {
                config->set("region_count", static_cast<int64_t>(param_set.at("region_count")));
            }
            if (param_set.find("connection_probability") != param_set.end()) {
                config->set("connection_probability", param_set.at("connection_probability"));
            }
            
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                throw std::runtime_error("Failed to initialize brain in batch simulation");
            }
            
            // Run simulation
            for (int step = 0; step < base_steps; ++step) {
                brain->step(step);
            }
            
            // Collect results
            std::unordered_map<std::string, double> result;
            result["neuron_count"] = static_cast<double>(brain->getTotalNeuronCount());
            result["region_count"] = static_cast<double>(brain->getRegionCount());
            result["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
            result["final_firing_rate"] = brain->getAverageFiringRate();
            result["eir"] = brain->getExcitationInhibitionRatio();
            
            // Add parameter set to results
            for (const auto& param : param_set) {
                result["param_" + param.first] = param.second;
            }
            
            results.push_back(result);
        }
        
        return results;
    }, py::arg("parameters"), py::arg("base_steps") = 100,
        "Run batch simulations with different parameters and return results");

    // Error recovery and validation
    m.def("validate_simulation_state", [](std::shared_ptr<Brain> brain) {
        std::unordered_map<std::string, std::vector<std::string>> validation_results;
        
        // Validate basic brain state
        std::vector<std::string> warnings;
        
        if (brain->getTotalNeuronCount() == 0) {
            warnings.push_back("Brain has no neurons");
        }
        
        if (brain->getTotalSynapseCount() == 0) {
            warnings.push_back("Brain has no synapses");
        }
        
        if (brain->getRegionCount() == 0) {
            warnings.push_back("Brain has no regions");
        }
        
        // Validate neuron health
        if (brain->getActiveNeuronCount() > brain->getTotalNeuronCount()) {
            warnings.push_back("Active neuron count exceeds total neuron count");
        }
        
        // Validate simulation consistency
        float eir = brain->getExcitationInhibitionRatio();
        if (std::isnan(eir) || std::isinf(eir)) {
            warnings.push_back("Invalid excitation-inhibition ratio");
        }
        
        float firing_rate = brain->getAverageFiringRate();
        if (std::isnan(firing_rate) || std::isinf(firing_rate)) {
            warnings.push_back("Invalid firing rate");
        }
        
        validation_results["warnings"] = warnings;
        validation_results["is_valid"] = warnings.empty();
        
        return validation_results;
    }, py::arg("brain"),
        "Validate simulation state and return any warnings or issues");

    // Advanced configuration analysis
    m.def("analyze_config", [](const std::shared_ptr<Config>& config) {
        std::unordered_map<std::string, std::vector<std::pair<std::string, double>>> analysis;
        
        auto keys = config->getKeys();
        
        // Categorize configuration parameters
        std::vector<std::pair<std::string, double>> neuron_params;
        std::vector<std::pair<std::string, double>> plasticity_params;
        std::vector<std::pair<std::string, double>> neuromod_params;
        std::vector<std::pair<std::string, double>> development_params;
        
        for (const auto& key : keys) {
            auto value = config->getOr<double>(key, 0.0);
            
            if (key.find("neuron") != std::string::npos) {
                neuron_params.push_back({key, value});
            } else if (key.find("plasticity") != std::string::npos) {
                plasticity_params.push_back({key, value});
            } else if (key.find("neuromod") != std::string::npos || key.find("dopamine") != std::string::npos || 
                      key.find("curiosity") != std::string::npos || key.find("novelty") != std::string::npos) {
                neuromod_params.push_back({key, value});
            } else if (key.find("development") != std::string::npos || key.find("age") != std::string::npos) {
                development_params.push_back({key, value});
            }
        }
        
        analysis["neuron_parameters"] = neuron_params;
        analysis["plasticity_parameters"] = plasticity_params;
        analysis["neuromodulation_parameters"] = neuromod_params;
        analysis["development_parameters"] = development_params;
        
        // Calculate derived metrics
        std::unordered_map<std::string, double> metrics;
        
        // Calculate plasticity balance
        double total_plasticity = 0.0;
        for (const auto& param : plasticity_params) {
            total_plasticity += param.second;
        }
        metrics["total_plasticity"] = total_plasticity;
        
        // Calculate neuromodulation level
        double total_neuromod = 0.0;
        for (const auto& param : neuromod_params) {
            total_neuromod += param.second;
        }
        metrics["total_neuromodulation"] = total_neuromod;
        
        // Calculate developmental stage (rough estimate based on age)
        double age = 0.0;
        for (const auto& param : development_params) {
            if (param.first.find("age") != std::string::npos) {
                age = param.second;
                break;
            }
        }
        metrics["developmental_age"] = age;
        
        analysis["derived_metrics"] = metrics;
        
        return analysis;
    }, py::arg("config"),
        "Analyze configuration parameters and categorize them by system type");

    // Simulation checkpoint utility
    m.def("checkpoint_and_resume", [](std::shared_ptr<Brain> brain, const std::string& filepath, bool save = true) {
        std::unordered_map<std::string, std::string> checkpoint_info;
        
        if (save) {
            // Save checkpoint
            bool saved = brain->save(filepath);
            if (saved) {
                checkpoint_info["status"] = "success";
                checkpoint_info["filepath"] = filepath;
                checkpoint_info["neuron_count"] = std::to_string(brain->getTotalNeuronCount());
                checkpoint_info["total_spikes"] = std::to_string(brain->getTotalSpikeCount());
                checkpoint_info["timestamp"] = std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count());
            } else {
                checkpoint_info["status"] = "failed";
                checkpoint_info["error"] = "Failed to save brain state";
            }
        } else {
            // Load checkpoint
            bool loaded = brain->load(filepath);
            if (loaded) {
                checkpoint_info["status"] = "loaded";
                checkpoint_info["filepath"] = filepath;
                checkpoint_info["neuron_count"] = std::to_string(brain->getTotalNeuronCount());
                checkpoint_info["total_spikes"] = std::to_string(brain->getTotalSpikeCount());
            } else {
                checkpoint_info["status"] = "failed";
                checkpoint_info["error"] = "Failed to load brain state";
            }
        }
        
        return checkpoint_info;
    }, py::arg("brain"), py::arg("filepath"), py::arg("save") = true,
        "Save or load brain state to/from file (checkpointing utility)");

    // Compatibility mode for legacy code
    m.def("legacy_mode", []() {
        auto config = std::make_shared<Config>();
        // Configure for legacy compatibility
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("connection_probability", 0.05);
        config->set("plasticity_stdp_enable", true);
        config->set("plasticity_hebbian_enable", false);
        config->set("neuromod_curiosity_enable", false);
        config->set("neuromod_novelty_enable", false);
        
        return config;
    }, "Create a configuration optimized for legacy compatibility");

    // Random seed utility for reproducible research
    m.def("set_reproducible_random_seed", [](int seed) {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(seed));
        return config;
    }, py::arg("seed"),
        "Create a configuration with reproducible random seed");

    // Performance optimization suggestions
    m.def("get_performance_suggestions", [](std::shared_ptr<Brain> brain) {
        std::vector<std::string> suggestions;
        
        // Check neuron count
        if (brain->getTotalNeuronCount() > 1000) {
            suggestions.push_back("Consider reducing neuron count for better performance with larger simulations");
        }
        
        // Check region count
        if (brain->getRegionCount() > 10) {
            suggestions.push_back("Consider reducing region count for better network connectivity");
        }
        
        // Check firing rate
        float firing_rate = brain->getAverageFiringRate();
        if (firing_rate > 10.0) {
            suggestions.push_back("High firing rate detected. Consider adjusting synaptic weights for stability");
        }
        
        if (firing_rate < 0.1) {
            suggestions.push_back("Very low firing rate. Check connectivity or adjust neuron parameters");
        }
        
        // Check E/I balance
        float eir = brain->getExcitationInhibitionRatio();
        if (eir < 0.5 || eir > 2.0) {
            suggestions.push_back("E/I ratio outside optimal range (0.5-2.0). Adjust plasticity parameters");
        }
        
        // Check spike count
        if (brain->getTotalSpikeCount() > 10000) {
            suggestions.push_back("Very high spike count. Consider reducing simulation time or adjusting neuron thresholds");
        }
        
        return suggestions;
    }, py::arg("brain"),
        "Get performance optimization suggestions based on current brain state");

    // Experimental parameter explorer
    m.def("explore_parameters", [](const std::vector<std::unordered_map<std::string, double>>& parameter_ranges, int num_trials = 10) {
        std::vector<std::unordered_map<std::string, double>> results;
        
        std::random_device rd;
        std::mt19937 gen(rd());
        
        for (int trial = 0; trial < num_trials; ++trial) {
            std::unordered_map<std::string, double> trial_params;
            std::unordered_map<std::string, double> trial_results;
            
            // Sample parameters from ranges
            for (const auto& range : parameter_ranges) {
                std::string param_name = range.at("name");
                double min_value = range.at("min");
                double max_value = range.at("max");
                
                std::uniform_real_distribution<double> dist(min_value, max_value);
                double sampled_value = dist(gen);
                
                trial_params[param_name] = sampled_value;
                
                // Create temporary config for this trial
                auto config = std::make_shared<Config>();
                config->set(param_name, sampled_value);
                
                // Run quick simulation
                auto temp_brain = std::make_shared<Brain>(config);
                if (temp_brain->initialize()) {
                    for (int step = 0; step < 50; ++step) {
                        temp_brain->step(step);
                    }
                    
                    trial_results["firing_rate"] = temp_brain->getAverageFiringRate();
                    trial_results["total_spikes"] = static_cast<double>(temp_brain->getTotalSpikeCount());
                }
            }
            
            trial_results["trial_id"] = static_cast<double>(trial);
            results.push_back(trial_results);
        }
        
        return results;
    }, py::arg("parameter_ranges"), py::arg("num_trials") = 10,
        "Explore parameter space by sampling from parameter ranges and running quick simulations");

    // Advanced neural network analysis
    m.def("analyze_network_topology", [](std::shared_ptr<Brain> brain) {
        std::unordered_map<std::string, double> topology_metrics;
        
        // Get all regions and populations
        auto regions = brain->getRegions();
        
        if (regions.empty()) {
            return topology_metrics; // Return empty metrics for empty brain
        }
        
        // Analyze region connectivity
        double total_inter_region_connections = 0.0;
        for (const auto& region_ptr : regions) {
            if (region_ptr) {
                // Count inter-region connections
                total_inter_region_connections += static_cast<double>(region_ptr->getSynapseCount()) * 0.1; // Rough estimate
            }
        }
        
        topology_metrics["estimated_inter_region_connections"] = total_inter_region_connections;
        
        // Analyze neuron distribution
        double total_neurons = static_cast<double>(brain->getTotalNeuronCount());
        double avg_neurons_per_region = total_neurons / regions.size();
        topology_metrics["avg_neurons_per_region"] = avg_neurons_per_region;
        
        // Calculate clustering coefficient (rough estimate)
        double clustering_coefficient = 0.0;
        if (total_neurons > 2) {
            clustering_coefficient = 0.5 + (total_inter_region_connections / total_neurons) * 0.3;
        }
        topology_metrics["estimated_clustering_coefficient"] = clustering_coefficient;
        
        // Calculate small-worldness (rough estimate)
        double small_worldness = 1.0;
        if (clustering_coefficient > 0.0) {
            small_worldness = clustering_coefficient / std::max(0.1, total_inter_region_connections / total_neurons);
        }
        topology_metrics["estimated_small_worldness"] = small_worldness;
        
        return topology_metrics;
    }, py::arg("brain"),
        "Analyze network topology including connectivity, clustering, and small-world properties");

    // Advanced learning curve analysis
    m.def("analyze_learning_curve", [](std::shared_ptr<Brain> brain, int steps = 1000, int sample_interval = 50) {
        std::vector<double> learning_metrics;
        std::vector<double> time_points;
        
        double previous_firing_rate = brain->getAverageFiringRate();
        double previous_eir = brain->getExcitationInhibitionRatio();
        
        for (int step = 0; step < steps; ++step) {
            brain->step(step);
            
            if (step % sample_interval == 0) {
                double current_firing_rate = brain->getAverageFiringRate();
                double current_eir = brain->getExcitationInhibitionRatio();
                
                // Calculate learning rate metrics
                double firing_rate_change = current_firing_rate - previous_firing_rate;
                double eir_change = current_eir - previous_eir;
                
                double stability_index = 1.0 / (1.0 + std::abs(firing_rate_change) + std::abs(eir_change));
                
                learning_metrics.push_back(stability_index);
                time_points.push_back(static_cast<double>(step));
                
                previous_firing_rate = current_firing_rate;
                previous_eir = current_eir;
            }
        }
        
        std::unordered_map<std::string, std::vector<double>> learning_analysis;
        learning_analysis["stability_index"] = learning_metrics;
        learning_analysis["time_points"] = time_points;
        
        // Calculate learning metrics
        double avg_stability = std::accumulate(learning_metrics.begin(), learning_metrics.end(), 0.0) / learning_metrics.size();
        learning_analysis["average_stability"] = avg_stability;
        
        // Calculate learning rate (change in stability)
        double learning_rate = 0.0;
        if (learning_metrics.size() > 1) {
            for (size_t i = 1; i < learning_metrics.size(); ++i) {
                learning_rate += learning_metrics[i] - learning_metrics[i-1];
            }
            learning_rate /= (learning_metrics.size() - 1);
        }
        learning_analysis["learning_rate"] = learning_rate;
        
        // Determine learning phase
        std::string learning_phase = "unknown";
        if (learning_rate > 0.01) {
            learning_phase = "exploration";
        } else if (learning_rate < -0.01) {
            learning_phase = "consolidation";
        } else {
            learning_phase = "stable";
        }
        learning_analysis["learning_phase"] = std::vector<double>{static_cast<double>(learning_phase == "exploration" ? 2 : learning_phase == "consolidation" ? 1 : 0)};
        
        return learning_analysis;
    }, py::arg("brain"), py::arg("steps") = 1000, py::arg("sample_interval") = 50,
        "Analyze learning dynamics over time, tracking stability and learning rates");
}

} // namespace nlm

} // namespace nlm
