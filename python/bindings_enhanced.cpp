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
        Provides comprehensive classes for Brain, Config, AgentBrain, SimpleWorld, 
        SensoryInput, and Action with enhanced error handling, documentation, 
        and Pythonic interfaces.

        Features:
        - Complete neural system simulation with memory, prediction, cognition
        - Agent-brain integration for embodied intelligence
        - Flexible configuration system
        - Context manager support for resource management
        - Comprehensive error handling and validation
        - Performance monitoring capabilities
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ============= BASIC TYPES =============

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
            return std::to_string(id.value);
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
            return std::to_string(id.value);
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
            return std::to_string(id.value);
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // ============= ENUMERATIONS =============

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

    // ============= CONFIGURATION CLASS =============

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)

        A flexible configuration system for neural network parameters.
        Supports JSON file loading, command-line parsing, and runtime modification.
        Provides type-safe access to all configuration values.
    )pbdoc")
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
        })
        // Convenience methods for common configuration values
        .def("set_neuron_count", &Config::set,
             "Set neuron count", py::arg("value"))
        .def("set_region_count", [](Config& self, int value) {
            self.set("region_count", value);
        }, py::arg("value"), "Set region count")
        .def("set_connection_probability", [](Config& self, float value) {
            self.set("connection_probability", value);
        }, py::arg("value"), "Set connection probability")
        .def("get_neuron_count", [](const Config& self) {
            return self.getOr<size_t>("neuron_count", 1000);
        }, "Get neuron count")
        .def("get_region_count", [](const Config& self) {
            return self.getOr<size_t>("region_count", 1);
        }, "Get region count");

    // ============= SENSORY INPUT CLASSES =============

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)

        Abstract base class representing sensory information from the environment.
        All concrete sensory modalities inherit from this class.
    )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)

        Visual sensory input representing a 2D image with RGB channels.
        Used for computer vision tasks and visual perception.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set visual data from RGB values")
        .def("getWidth", &Vision::getWidth, "Get image width")
        .def("getHeight", &Vision::getHeight, "Get image height")
        .def("getChannels", &Vision::getChannels, "Get number of color channels");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)

        Audio sensory input representing sound signals with sampling rate.
        Used for audio processing and auditory perception tasks.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set audio sampling rate")
        .def("getSampleRate", &Audio::getSampleRate, "Get audio sampling rate")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of audio samples");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)

        Internal neural signals representing brain state and activity.
        Includes attention, memory, and cognitive state information.
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add an internal signal value")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all internal signals");

    // ============= ACTION CLASS =============

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)

        Represents an action that can be executed by the agent in the environment.
        Contains action type and parameters for different action modalities.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType, "Get action type")
        .def("setType", &Action::setType, py::arg("type"), "Set action type")
        .def("getParameters", &Action::getParameters, "Get action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("getName", &Action::getName, "Get action name as string")
        .def("clone", &Action::clone, "Create a copy of this action");

    // ============= WORLD OBJECT CLASS =============

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)

        Represents an object in the simulated world with physical properties.
        Used for environment modeling and interaction with the agent.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x, "Object X position")
        .def_readwrite("y", &WorldObject::y, "Object Y position")
        .def_readwrite("radius", &WorldObject::radius, "Object radius")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Object value")
        .def_readwrite("active", &WorldObject::active, "Object active state");

    // ============= AGENT BODY CLASS =============

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)

        Physical state of the agent in the simulated world.
        Includes position, orientation, velocity, and physical properties.
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position")
        .def_readwrite("y", &AgentBody::y, "Y position")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation angle")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Energy level")
        .def_readwrite("health", &AgentBody::health, "Health status")
        .def_readwrite("age", &AgentBody::age, "Age")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Is moving flag")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Is turning flag")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Last action time")
        .def("reset", &AgentBody::reset, "Reset agent body to initial state");

    // ============= ACTION RESULT CLASS =============

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)

        Result of executing an action in the environment.
        Contains reward, success status, and explanatory message.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward, "Reward value")
        .def_readwrite("success", &ActionResult::success, "Success flag")
        .def_readwrite("message", &ActionResult::message, "Result message");

    // ============= SENSORY PERCEPT CLASS =============

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)

        Container for all sensory information received from the environment.
        Combines vision, audio, touch, and internal signals into a unified percept.
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get vision data")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set vision data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision height")
        .def("getTouch", &SensoryPercept::getTouch, "Get touch data")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch data")
        .def("getInternal", &SensoryPercept::getInternal, "Get internal signals")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception, "Get proprioception data")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioception data")
        .def("getAudio", &SensoryPercept::getAudio, "Get audio data")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio data")
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all sensory signals")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get percept timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set percept timestamp");

    // ============= WORLD CLASS =============

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)

        Simulated environment for agent-world interaction.
        Provides sensory input, motor output, and action execution capabilities.
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and vision parameters")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object from world")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed");

    // ============= BRAIN CLASS (Enhanced) =============

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)

        The central coordinator of the NLM neural system implementing:
        - Real spiking neural computation (LIF dynamics)
        - Event-driven spike propagation
        - Integrated memory systems (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Prediction and cognition systems
        - Plasticity rules (STDP, Hebbian, structural)
        - Development stages
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
             "Log brain status")
        // New enhanced methods
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             "Get episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             "Get associative memory system")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             "Get prediction system")
        .def("getPlanner", &Brain::getPlanner,
             "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             "Get attentional selection system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             "Get development system")
        .def("getDopamine", &Brain::getDopamine,
             "Get dopamine neuromodulator")
        .def("getCuriosity", &Brain::getCuriosity,
             "Get curiosity neuromodulator")
        .def("getNovelty", &Brain::getNovelty,
             "Get novelty neuromodulator")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             "Get prediction error neuromodulator")
        .def("getSTDP", &Brain::getSTDP,
             "Get STDP plasticity rule")
        .def("getHebbian", &Brain::getHebbian,
             "Get Hebbian plasticity rule")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             "Get structural plasticity rule")
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             "Get spike system")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             "Get random number generator")
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
        .def("applyNeuromodulation", &Brain::applyNeuromodulation,
             py::arg("signal"), "Apply neuromodulatory signal")
        .def("updatePlasticity", &Brain::updatePlasticity,
             "Update plasticity rules")
        .def("develop", &Brain::develop, "Apply developmental changes")
        // Pythonic properties and convenience methods
        .def_property_readonly("working_memory", &Brain::getWorkingMemory,
                              "Get working memory system")
        .def_property_readonly("episodic_memory", &Brain::getEpisodicMemory,
                              "Get episodic memory system")
        .def_property_readonly("associative_memory", &Brain::getAssociativeMemory,
                              "Get associative memory system")
        .def_property_readonly("prediction_system", &Brain::getPredictionSystem,
                              "Get prediction system")
        .def_property_readonly("planner", &Brain::getPlanner,
                              "Get neural planner")
        .def_property_readonly("concept_formation", &Brain::getConceptFormation,
                              "Get concept formation system")
        .def_property_readonly("attention", &Brain::getAttention,
                              "Get attentional selection system")
        .def_property_readonly("development_system", &Brain::getDevelopmentSystem,
                              "Get development system")
        .def_property_readonly("dopamine", &Brain::getDopamine,
                              "Get dopamine neuromodulator")
        .def_property_readonly("curiosity", &Brain::getCuriosity,
                              "Get curiosity neuromodulator")
        .def_property_readonly("novelty", &Brain::getNovelty,
                              "Get novelty neuromodulator")
        .def_property_readonly("prediction_error", &Brain::getPredictionErrorSignal,
                              "Get prediction error neuromodulator")
        .def_property_readonly("stdp", &Brain::getSTDP,
                              "Get STDP plasticity rule")
        .def_property_readonly("hebbian", &Brain::getHebbian,
                              "Get Hebbian plasticity rule")
        .def_property_readonly("structural_plasticity", &Brain::getStructuralPlasticity,
                              "Get structural plasticity rule")
        .def_property_readonly("spike_system", &Brain::getSpikeSystem,
                              "Get spike system")
        .def_property_readonly("random_generator", &Brain::getRandomGenerator,
                              "Get random number generator");

    // ============= AGENT BRAIN CLASS (Enhanced) =============

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)

        Interface layer connecting NLM brain to world environment and actions.
        Provides high-level agent control with sensory processing and motor output.
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain with underlying brain")
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
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        // Pythonic properties
        .def_property_readonly("brain", &AgentBrain::getBrain,
                             "Get the underlying brain")
        .def_property_readonly("neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
                             "Get current neuromodulation level")
        .def_property_readonly("curiosity_level", &AgentBrain::getCuriosityLevel,
                             "Get curiosity level")
        .def_property_readonly("novelty_level", &AgentBrain::getNoveltyLevel,
                             "Get novelty level")
        .def_property_readonly("prediction_error", &AgentBrain::getPredictionError,
                             "Get prediction error");

    // ============= FACTORY FUNCTIONS =============

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration", py::return_value_policy::reference_internal);

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration", py::return_value_policy::reference_internal);

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world", py::return_value_policy::reference_internal);

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface", py::return_value_policy::reference_internal);

    // ============= CONSTANTS =============

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // ============= UTILITY FUNCTIONS =============

    m.def("version", []() {
        return "NLM Phase 6.0 - Enhanced Python Bindings";
    }, "Get NLM version information");

    m.def("validate_config", [](const Config& config) {
        // Simple validation - can be expanded
        bool valid = true;
        if (config.has("neuron_count")) {
            auto count = config.get<size_t>("neuron_count");
            if (!count || *count < 1 || *count > 1000000) {
                valid = false;
            }
        }
        return valid;
    }, "Validate configuration parameters");

    m.def("benchmark", [](const std::shared_ptr<Brain>& brain, size_t steps) {
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i, i * 0.001);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        return double(duration.count()) / steps;  // Average milliseconds per step
    }, "Benchmark brain simulation performance", py::arg("brain"), py::arg("steps"));

    // ============= CONTEXT MANAGER SUPPORT =============

    py::class_<BrainContextManager>(m, "BrainContextManager")
        .def(py::init<std::shared_ptr<Brain>>())
        .def("__enter__", [](BrainContextManager& self) {
            return self.brain_.get();
        })
        .def("__exit__", [](BrainContextManager& self, py::object, py::object, py::object) {
            self.brain_.reset();
        });

    m.def("brain_context", [](std::shared_ptr<Brain> brain) {
        return BrainContextManager(brain);
    }, "Create context manager for brain simulation");

    // ============= ADVANCED FEATURES =============

    // Memory statistics collection
    m.def("collect_memory_stats", [](const std::shared_ptr<Brain>& brain) {
        py::dict stats;
        auto* wm = brain->getWorkingMemory();
        if (wm) {
            stats["working_memory_traces"] = wm->getActiveTraces();
        }
        auto* em = brain->getEpisodicMemory();
        if (em) {
            stats["episodic_memory_episodes"] = em->getEpisodeCount();
        }
        return stats;
    }, "Collect memory statistics from brain");

    // Simulation checkpoint utilities
    m.def("checkpoint_brain", [](const std::shared_ptr<Brain>& brain, const std::string& path) {
        return brain->save(path);
    }, "Create simulation checkpoint", py::arg("brain"), py::arg("path"));

    m.def("load_checkpoint", [](const std::shared_ptr<Config>& config, const std::string& path) {
        auto brain = std::make_shared<Brain>(config);
        if (brain->initialize()) {
            if (brain->load(path)) {
                return brain;
            }
        }
        return std::shared_ptr<Brain>(nullptr);
    }, "Load brain from checkpoint", py::arg("config"), py::arg("path"), py::return_value_policy::reference_internal);

    // ============= EXPERIMENTAL FEATURES =============

    // Enhanced visualization data collection
    m.def("collect_neural_activity", [](const std::shared_ptr<Brain>& brain) {
        py::dict activity;
        activity["total_spikes"] = brain->getTotalSpikeCount();
        activity["active_neurons"] = brain->getActiveNeuronCount();
        activity["firing_neurons"] = brain->getFiringNeuronCount();
        activity["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        return activity;
    }, "Collect neural activity statistics", py::arg("brain"));

    // Advanced plasticity control
    m.def("configure_plasticity", [](const std::shared_ptr<Brain>& brain, bool hebbian, bool stdp, bool structural) {
        auto* region = brain->getRegion(brain->getRegionIds()[0]);
        if (region) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(hebbian, stdp, structural);
            }
        }
    }, "Configure plasticity rules for all synapses", py::arg("brain"), py::arg("hebbian"), py::arg("stdp"), py::arg("structural"));

    // Development control
    m.def("set_development_stage", [](const std::shared_ptr<Brain>& brain, const DevelopmentalStage& stage) {
        brain->setDevelopmentalStage(stage);
    }, "Set brain developmental stage", py::arg("brain"), py::arg("stage"));

    // ============= PYTHONIC INTERFACES =============

    // Add Pythonic interfaces for enhanced usability
    py::class_<Configurable>(m, "Configurable")
        .def("configure", &Configurable::configure,
             "Configure neural parameters for simulation", py::arg("neuron_count"), py::arg("regions"));

    m.def("quick_simulate", [](const std::shared_ptr<Brain>& brain, size_t steps) {
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i, i * 0.001);
        }
        return brain->getTotalSpikeCount();
    }, "Quick simulation with default parameters", py::arg("brain"), py::arg("steps"));

    m.def("create_test_brain", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, "Create a test brain with default configuration", py::return_value_policy::reference_internal);

    m.def("create_demo_world", []() {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 10, 10);
        world->setAgentStart(10.0f, 10.0f);
        return world;
    }, "Create a demo world for testing", py::return_value_policy::reference_internal);

    // ============= DEPRECATION WARNINGS =============

    m.def("deprecated_warning", []() {
        py::print("Warning: Some older methods have been deprecated. "
                 "Use the enhanced interfaces instead.");
    });

    // Add deprecation notice for old usage patterns
    m.def("legacy_support", [](const std::shared_ptr<Brain>& brain) {
        py::print("Legacy support enabled. Consider using the new Pythonic interfaces.");
        return brain;
    }, "Enable legacy support mode");

} // namespace nlm

// ============= CONTEXT MANAGER IMPLEMENTATION =============

class BrainContextManager {
public:
    BrainContextManager(std::shared_ptr<Brain> brain) : brain_(std::move(brain)) {}
    
    Brain* get() {
        return brain_.get();
    }
    
    std::shared_ptr<Brain> brain_;
};

// ============= CONFIGURABLE CLASS =============

class Configurable {
public:
    void configure(size_t neuron_count, size_t regions) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neuron_count);
        config->set("region_count", static_cast<int64_t>(regions));
        config->set("connection_probability", 0.1f);
        // Would normally store config for later use
    }
};