#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

// Include all the NLM headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/SemanticMemory.hpp"
#include "../src/memory/ProceduralMemory.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/development/Maturation.hpp"
#include "../src/development/Synaptogenesis.hpp"
#include "../src/development/Pruning.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"
#include "../src/dynamics/NeuralDynamics.hpp"
#include "../src/dynamics/SpikeSystem.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/prediction/NeuralPrediction.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        --------------------------------------------------------
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        
        This enhanced version includes:
        - Complete memory system support (Working, Episodic, Associative, Semantic, Procedural)
        - Full neuromodulation system (Dopamine, Curiosity, Novelty, PredictionError, Reward)
        - Complete brain development system (Development, Maturation, Synaptogenesis, Pruning)
        - Full cognition system (NeuralPlanner, ConceptFormation, AttentionalSelection)
        - Complete prediction system (PredictionSystem, NeuralPrediction)
        - Full plasticity system (STDP, Hebbian, StructuralPlasticity)
        - Neural dynamics and event handling (NeuralDynamics, SpikeSystem)
        - Enhanced error handling and Pythonic APIs
        - Comprehensive documentation with examples
        
        The NLM implements a brain-inspired spiking neural network simulator with
        event-driven dynamics, multiple learning mechanisms, and integrated memory
        systems for learning, prediction, and decision-making.
    )pbdoc";

    // Register exception classes for better error handling
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");

    // ===================================================================
    // BASIC TYPES AND ENUMS
    // ===================================================================

    // Neuron and synapse identifiers
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

    // Neuron types and classifications
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // Synapse types
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // Developmental stages
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // Firing states
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // Action types
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

    // Motor commands
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

    // World object types
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // ===================================================================
    // CONFIGURATION SYSTEM
    // ===================================================================

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
        });

    // ===================================================================
    // SENSORY SYSTEMS
    // ===================================================================

    // Base sensory input class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision system
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    // Audio system
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // Internal signals
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // ===================================================================
    // ACTION SYSTEM
    // ===================================================================

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + " parameters=" + std::to_string(action.getParameters().size()) + ">";
        });

    // ===================================================================
    // WORLD OBJECT SYSTEM
    // ===================================================================

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);

    // Agent body state
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocityX", &AgentBody::velocityX)
        .def_readwrite("velocityY", &AgentBody::velocityY)
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("isMoving", &AgentBody::isMoving)
        .def_readwrite("isTurning", &AgentBody::isTurning)
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset);

    // Action result
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // ===================================================================
    // PERCEPT SYSTEM
    // ===================================================================

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    // ===================================================================
    // WORLD SYSTEM
    // ===================================================================

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // ===================================================================
    // BRAIN SYSTEM
    // ===================================================================

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
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
        // Memory system access
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory")
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
             "Get novelty detection system")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error signal system")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get STDP plasticity system")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get Hebbian learning system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get structural plasticity system")
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get spike system")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get random generator")
        .def("__repr__", [](const Brain& brain) {
            std::ostringstream oss;
            oss << "<Brain: neurons=" << brain.getTotalNeuronCount()
                << " regions=" << brain.getRegionCount()
                << " totalSpikes=" << brain.getTotalSpikeCount() << ">";
            return oss.str();
        });

    // ===================================================================
    // MEMORY SYSTEMS
    // ===================================================================

    // Neural Working Memory
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Neural working memory using persistent activity)")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize,
             py::arg("brain"),
             "Initialize with brain reference")
        .def("store", &NeuralWorkingMemory::store, py::arg("pattern"), py::arg("strength") = 1.0f,
             "Store information in working memory")
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron,
             py::arg("neuron"), py::arg("activation"),
             "Store specific neuron activation")
        .def("retrieve", &NeuralWorkingMemory::retrieve,
             "Retrieve current working memory content")
        .def("contains", &NeuralWorkingMemory::contains, py::arg("neuron"),
             "Check if specific neuron is in working memory")
        .def("getNeuronActivation", &NeuralWorkingMemory::getNeuronActivation, py::arg("neuron"),
             "Get activation level of a specific neuron")
        .def("update", &NeuralWorkingMemory::update, py::arg("dt"),
             "Update working memory (maintenance and decay)")
        .def("clear", &NeuralWorkingMemory::clear,
             "Clear working memory")
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces,
             "Get number of active memory traces")
        .def("getCapacity", &NeuralWorkingMemory::getCapacity,
             "Get working memory capacity")
        .def("setCapacity", &NeuralWorkingMemory::setCapacity, py::arg("capacity"),
             "Set working memory capacity")
        .def("getDecayRate", &NeuralWorkingMemory::getDecayRate,
             "Get working memory decay rate")
        .def("setDecayRate", &NeuralWorkingMemory::setDecayRate, py::arg("rate"),
             "Set working memory decay rate")
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons,
             "Get neurons currently in working memory")
        .def("strengthenMemory", &NeuralWorkingMemory::strengthenMemory, py::arg("factor"),
             "Strengthen working memory representation")
        .def("runCompetition", &NeuralWorkingMemory::runCompetition,
             "Run competition between memory traces")
        .def("isWinning", &NeuralWorkingMemory::isWinning, py::arg("neuron"),
             "Check if neuron is part of winning population")
        .def("getMemoryActivity", &NeuralWorkingMemory::getMemoryActivity,
             "Get overall memory activity level");

    // Neural Episodic Memory
    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Neural episodic memory storing experiences)")
        .def(py::init<>())
        .def("initialize", &NeuralEpisodicMemory::initialize,
             py::arg("brain"),
             "Initialize with brain reference")
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode, py::arg("episode"),
             "Store a new experience episode")
        .def("retrieveSimilar", &NeuralEpisodicMemory::retrieveSimilar,
             py::arg("sensoryPattern"), py::arg("maxResults") = 5,
             "Retrieve episodes similar to query pattern")
        .def("retrieveTemporal", &NeuralEpisodicMemory::retrieveTemporal,
             py::arg("startTime"), py::arg("endTime"), py::arg("maxResults") = 10,
             "Retrieve episodes from specific time window")
        .def("retrieveByLocation", &NeuralEpisodicMemory::retrieveByLocation,
             py::arg("x"), py::arg("y"), py::arg("radius"), py::arg("maxResults") = 5,
             "Retrieve episodes by location")
        .def("retrieveAfterAction", &NeuralEpisodicMemory::retrieveAfterAction,
             py::arg("action"), py::arg("maxResults") = 5,
             "Retrieve episodes following a specific action")
        .def("replayEpisode", &NeuralEpisodicMemory::replayEpisode,
             py::arg("episode"),
             "Replay an episode - reactivates neural patterns")
        .def("updateRelevance", &NeuralEpisodicMemory::updateRelevance,
             py::arg("episodeId"), py::arg("relevanceDelta"),
             "Update episode relevance based on recent use")
        .def("getEpisode", &NeuralEpisodicMemory::getEpisode, py::arg("index"),
             "Get episode by index")
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount,
             "Get total number of episodes")
        .def("getRecentEpisodes", &NeuralEpisodicMemory::getRecentEpisodes, py::arg("count"),
             "Get recent episodes")
        .def("getAverageReward", &NeuralEpisodicMemory::getAverageReward,
             "Get average reward from episodes")
        .def("consolidate", &NeuralEpisodicMemory::consolidate, py::arg("relevanceThreshold"),
             "Clear old episodes")
        .def("clear", &NeuralEpisodicMemory::clear,
             "Clear all episodes")
        .def("enableReplay", &NeuralEpisodicMemory::enableReplay, py::arg("enable"),
             "Enable/disable replay")
        .def("isReplayEnabled", &NeuralEpisodicMemory::isReplayEnabled,
             "Check if replay is enabled")
        .def("getEpisodesForReplay", &NeuralEpisodicMemory::getEpisodesForReplay, py::arg("count"),
             "Get episodes for replay (selection based on relevance and recency)")
        .def("replaySequence", &NeuralEpisodicMemory::replaySequence, py::arg("episodeIds"),
             "Replay multiple episodes (for sleep-like consolidation)")
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount,
             "Get total number of episodes");

    // Semantic Memory
    py::class_<SemanticMemory>(m, "SemanticMemory", R"pbdoc(Semantic memory for conceptual knowledge)")
        .def(py::init<>())
        .def("initialize", &SemanticMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("storeConcept", &SemanticMemory::storeConcept,
             py::arg("name"), py::arg("activationPattern"), py::arg("description") = "",
             "Store conceptual knowledge")
        .def("retrieveConcept", &SemanticMemory::retrieveConcept, py::arg("name"),
             "Retrieve concept by name")
        .def("hasConcept", &SemanticMemory::hasConcept, py::arg("name"),
             "Check if concept exists")
        .def("getConceptDescription", &SemanticMemory::getConceptDescription, py::arg("name"),
             "Get concept description")
        .def("getConceptNames", &SemanticMemory::getConceptNames,
             "Get all concept names")
        .def("clear", &SemanticMemory::clear,
             "Clear all concepts")
        .def("findSimilarConcepts", &SemanticMemory::findSimilarConcepts,
             py::arg("pattern"), py::arg("maxResults") = 5,
             "Find similar concepts (pattern completion)")
        .def("linkConcepts", &SemanticMemory::linkConcepts,
             py::arg("conceptA"), py::arg("conceptB"), py::arg("relationshipType") = "",
             py::arg("strength") = 1.0f,
             "Create relationship between concepts")
        .def("getRelatedConcepts", &SemanticMemory::getRelatedConcepts,
             py::arg("conceptName"), py::arg("relationshipType") = "",
             "Get related concepts")
        .def("updateConcept", &SemanticMemory::updateConcept,
             py::arg("name"), py::arg("newPattern"), py::arg("learningRate") = 0.1f,
             "Update concept based on feedback")
        .def("getConceptActivation", &SemanticMemory::getConceptActivation, py::arg("name"),
             "Get concept activation level")
        .def("encodeFromSensoryInput", &SemanticMemory::encodeFromSensoryInput,
             py::arg("sensoryPattern"), py::arg("suggestedName") = "",
             "Encode new concept from sensory input")
        .def("explainConcept", &SemanticMemory::explainConcept, py::arg("name"),
             "Generate explanation of concept");

    // Procedural Memory
    py::class_<ProceduralMemory>(m, "ProceduralMemory", R"pbdoc(Procedural memory for skills and habits)")
        .def(py::init<>())
        .def("initialize", &ProceduralMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("learnSkill", &ProceduralMemory::learnSkill,
             py::arg("name"), py::arg("neuralPattern"), py::arg("initialProficiency") = 1.0f,
             "Learn a new skill")
        .def("getSkill", &ProceduralMemory::getSkill, py::arg("name"),
             "Get skill by name")
        .def("getAllSkills", &ProceduralMemory::getAllSkills,
             "Get all skills")
        .def("updateProficiency", &ProceduralMemory::updateProficiency,
             py::arg("name"), py::arg("delta"),
             "Update skill proficiency")
        .def("executeSkill", &ProceduralMemory::executeSkill, py::arg("name"),
             "Execute skill (retrieve neural pattern)")
        .def("hasSkill", &ProceduralMemory::hasSkill, py::arg("name"),
             "Check if skill exists")
        .def("clear", &ProceduralMemory::clear,
             "Clear all skills")
        .def("getAverageProficiency", &ProceduralMemory::getAverageProficiency,
             "Get average proficiency across skills")
        .def("findSimilarSkills", &ProceduralMemory::findSimilarSkills,
             py::arg("pattern"), py::arg("maxResults") = 5,
             "Find similar skills (pattern similarity)")
        .def("transferSkill", &ProceduralMemory::transferSkill,
             py::arg("sourceSkill"), py::arg("targetSkill"), py::arg("transferStrength") = 0.5f,
             "Transfer skill to similar task")
        .def("getSkillsByProficiency", &ProceduralMemory::getSkillsByProficiency, py::arg("minProficiency"),
             "Get skills by proficiency level")
        .def("practiceSkill", &ProceduralMemory::practiceSkill,
             py::arg("name"), py::arg("practiceQuality") = 1.0f,
             "Practice skill to improve proficiency")
        .def("getSkillExecutions", &ProceduralMemory::getSkillExecutions, py::arg("name"),
             "Get skill execution count");

    // ===================================================================
    // NEUROMODULATION SYSTEMS
    // ===================================================================

    // Neuromodulator base class
    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(Abstract base for neuromodulatory signals)")
        .def(py::init<>())
        .def("getName", &Neuromodulator::getName,
             "Get modulator name")
        .def("getLevel", &Neuromodulator::getLevel,
             "Get current concentration/level")
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"),
             "Set current concentration/level")
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor,
             "Get neuromodulatory effect on plasticity")
        .def("update", &Neuromodulator::update, py::arg("dt"),
             "Update neuromodulator state");

    // Dopamine system
    py::class_<Dopamine, Neuromodulator>(m, "Dopamine", R"pbdoc(Dopamine - reward and reinforcement learning signal)")
        .def(py::init<>())
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"),
             "Signal reward reception")
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"),
             "Signal reward prediction error")
        .def("getBaseline", &Dopamine::getLevel, // Simplified - level is baseline in our implementation
             "Get baseline dopamine level")
        .def("setBaseline", &Dopamine::setLevel, py::arg("baseline"),
             "Set baseline dopamine level");

    // Curiosity system
    py::class_<Curiosity, Neuromodulator>(m, "Curiosity", R"pbdoc(Curiosity - exploration motivation)")
        .def(py::init<>())
        .def("increaseCuriosity", &Curiosity::setLevel, py::arg("level"),
             "Increase curiosity level")
        .def("decreaseCuriosity", [](Curiosity& self, float amount) {
            float current = self.getLevel();
            self.setLevel(std::max(0.0f, current - amount));
        }, py::arg("amount"),
             "Decrease curiosity level")
        .def("getCuriosityLevel", &Curiosity::getLevel,
             "Get current curiosity level");

    // Novelty detection
    py::class_<Novelty, Neuromodulator>(m, "Novelty", R"pbdoc(Novelty - novelty detection)")
        .def(py::init<>())
        .def("detectNovelty", [](Novelty& self, const std::vector<float>& pattern) {
            // Simple novelty detection based on pattern magnitude
            float novelty = 0.0f;
            for (float value : pattern) {
                novelty += std::abs(value);
            }
            novelty = std::min(1.0f, novelty / static_cast<float>(pattern.size()));
            self.setLevel(novelty);
        }, py::arg("pattern"),
             "Detect novelty in input pattern")
        .def("getNoveltyLevel", &Novelty::getLevel,
             "Get current novelty level");

    // Prediction Error signal
    py::class_<PredictionError, Neuromodulator>(m, "PredictionError", R"pbdoc(Prediction error signal)")
        .def(py::init<>())
        .def("setError", &PredictionError::setLevel, py::arg("error"),
             "Set prediction error value")
        .def("getError", &PredictionError::getLevel,
             "Get current prediction error value")
        .def("resetError", [](PredictionError& self) {
            self.setLevel(0.0f);
        }, "Reset prediction error to zero");

    // Reward signal
    py::class_<Reward, Neuromodulator>(m, "Reward", R"pbdoc(Reward signal)")
        .def(py::init<>())
        .def("addReward", &Reward::setLevel, py::arg("reward"),
             "Add reward to current level")
        .def("getTotalReward", &Reward::getLevel,
             "Get total accumulated reward");

    // ===================================================================
    // DEVELOPMENT SYSTEM
    // ===================================================================

    // Development system
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Brain development system)")
        .def(py::init<>())
        .def("update", &DevelopmentSystem::update,
             py::arg("brain"), py::arg("rng"), py::arg("dt"),
             "Update development system")
        .def("isDeveloping", [](const DevelopmentSystem& self) {
            return self.getCurrentStage() != DevelopmentalStage::Adult;
        }, "Check if brain is still developing");

    // Maturation system
    py::class_<Maturation>(m, "Maturation", R"pbdoc(Neural maturation system)")
        .def(py::init<>())
        .def("update", &Maturation::update,
             py::arg("brain"), py::arg("rng"), py::arg("dt"),
             "Update maturation")
        .def("isMature", [](const Maturation& self) {
            return self.isComplete();
        }, "Check if maturation is complete");

    // Synaptogenesis system
    py::class_<Synaptogenesis>(m, "Synaptogenesis", R"pbdoc(Synapse formation system)")
        .def(py::init<>())
        .def("update", &Synaptogenesis::update,
             py::arg("brain"), py::arg("rng"), py::arg("dt"),
             "Update synaptogenesis")
        .def("getFormationRate", &Synaptogenesis::getFormationRate,
             "Get synapse formation rate");

    // Pruning system
    py::class_<Pruning>(m, "Pruning", R"pbdoc(Synapse elimination system)")
        .def(py::init<>())
        .def("update", &Pruning::update,
             py::arg("brain"), py::arg("rng"), py::arg("dt"),
             "Update pruning")
        .def("getPruningRate", &Pruning::getPruningRate,
             "Get synapse pruning rate");

    // ===================================================================
    // PLASTICITY SYSTEMS
    // ===================================================================

    // STDP system
    py::class_<STDP>(m, "STDP", R"pbdoc(Spike-Timing-Dependent Plasticity)")
        .def(py::init<>())
        .def("update", &STDP::update,
             py::arg("synapse"), py::arg("preSpikes"), py::arg("postSpikes"), py::arg("dt"),
             "Update synapse using STDP rule")
        .def("getLTPWeight", &STDP::getLTPWeight,
             "Get long-term potentiation weight")
        .def("setLTPWeight", &STDP::setLTPWeight, py::arg("weight"),
             "Set long-term potentiation weight")
        .def("getLTDWeight", &STDP::getLTDWeight,
             "Get long-term depression weight")
        .def("setLTDWeight", &STDP::setLTDWeight, py::arg("weight"),
             "Set long-term depression weight");

    // Hebbian learning system
    py::class_<Hebbian>(m, "Hebbian", R"pbdoc(Hebbian learning)")
        .def(py::init<>())
        .def("update", &Hebbian::update,
             py::arg("synapse"), py::arg("preSpikes"), py::arg("postSpikes"), py::arg("dt"),
             "Update synapse using Hebbian learning")
        .def("setLearningRate", &Hebbian::setLearningRate, py::arg("rate"),
             "Set Hebbian learning rate");

    // Structural plasticity system
    py::class_<StructuralPlasticity>(m, "StructuralPlasticity", R"pbdoc(Structural plasticity for synapse formation)")
        .def(py::init<>())
        .def("update", &StructuralPlasticity::update,
             py::arg("brain"), py::arg("rng"),
             "Update structural plasticity")
        .def("getSynaptogenesisRate", &StructuralPlasticity::getSynaptogenesisRate,
             "Get synaptogenesis rate")
        .def("setSynaptogenesisRate", &StructuralPlasticity::setSynaptogenesisRate, py::arg("rate"),
             "Set synaptogenesis rate")
        .def("getPruningRate", &StructuralPlasticity::getPruningRate,
             "Get pruning rate")
        .def("setPruningRate", &StructuralPlasticity::setPruningRate, py::arg("rate"),
             "Set pruning rate");

    // ===================================================================
    // DYNAMICS AND EVENT SYSTEMS
    // ===================================================================

    // Spike system
    py::class_<SpikeSystem>(m, "SpikeSystem", R"pbdoc(Event-driven spike propagation)")
        .def(py::init<>())
        .def("registerHandler", &SpikeSystem::registerHandler,
             "Register handler for spike events")
        .def("registerDelayedHandler", &SpikeSystem::registerDelayedHandler,
             "Register handler for delayed spike events")
        .def("queueSpike", &SpikeSystem::queueSpike, py::arg("event"),
             "Queue a spike event")
        .def("queueDelayedSpike", &SpikeSystem::queueDelayedSpike, py::arg("event"),
             "Queue a delayed spike event")
        .def("processSpikes", &SpikeSystem::processSpikes, py::arg("step"),
             "Process pending spike events")
        .def("processDelayedSpikes", &SpikeSystem::processDelayedSpikes,
             py::arg("step"), py::arg("time"),
             "Process pending delayed spike events")
        .def("getPendingSpikeCount", &SpikeSystem::getPendingSpikeCount,
             "Get number of pending spike events")
        .def("getPendingDelayedCount", &SpikeSystem::getPendingDelayedCount,
             "Get number of pending delayed spike events")
        .def("reset", &SpikeSystem::reset,
             "Reset spike system state");

    // Neural dynamics
    py::class_<NeuralDynamics>(m, "NeuralDynamics", R"pbdoc(Neural dynamics simulation)")
        .def(py::init<>())
        .def("update", &NeuralDynamics::update,
             py::arg("brain"), py::arg("dt"),
             "Update neural dynamics")
        .def("getIntegrationTimeStep", &NeuralDynamics::getIntegrationTimeStep,
             "Get integration time step");

    // ===================================================================
    // COGNITION SYSTEMS
    // ===================================================================

    // Neural planner
    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural action planning)")
        .def(py::init<>())
        .def("initialize", &NeuralPlanner::initialize,
             py::arg("brain"),
             "Initialize with brain reference")
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth, py::arg("depth"),
             "Set planning depth")
        .def("planAction", [](NeuralPlanner& self, const std::vector<float>& state) {
            return self.planAction(state);
        }, py::arg("state"),
             "Plan action based on current state")
        .def("getPlan", &NeuralPlanner::getPlan,
             "Get current plan");

    // Concept formation
    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation from neural activity)")
        .def(py::init<>())
        .def("initialize", &ConceptFormation::initialize,
             py::arg("brain"),
             "Initialize with brain reference")
        .def("update", &ConceptFormation::update, py::arg("dt"),
             "Update concept formation")
        .def("getConcepts", &ConceptFormation::getConcepts,
             "Get current concepts")
        .def("getConceptsCount", &ConceptFormation::getConceptsCount,
             "Get number of concepts");

    // Attentional selection
    py::class_<AttentionalSelection>(m, "AttentionalSelection", R"pbdoc(Neural attentional selection)")
        .def(py::init<>())
        .def("initialize", &AttentionalSelection::initialize,
             py::arg("brain"),
             "Initialize with brain reference")
        .def("setInhibitionStrength", &AttentionalSelection::setInhibitionStrength, py::arg("strength"),
             "Set inhibition strength")
        .def("setExcitationStrength", &AttentionalSelection::setExcitationStrength, py::arg("strength"),
             "Set excitation strength")
        .def("setCompetitionThreshold", &AttentionalSelection::setCompetitionThreshold, py::arg("threshold"),
             "Set competition threshold")
        .def("getWinners", &AttentionalSelection::getWinners,
             "Get current attention winners")
        .def("isAttended", &AttentionalSelection::isAttended, py::arg("neuron"),
             "Check if neuron is currently attended")
        .def("processCompetition", &AttentionalSelection::processCompetition,
             py::arg("competitors"), py::arg("globalInhibition") = 0.5f,
             "Process competition between neurons")
        .def("focusOnRegion", &AttentionalSelection::focusOnRegion, py::arg("region"),
             "Focus attention on specific region")
        .def("releaseAttention", &AttentionalSelection::releaseAttention,
             "Release attention focus")
        .def("getAttendedRegions", &AttentionalSelection::getAttendedRegions,
             "Get currently attended regions");

    // ===================================================================
    // PREDICTION SYSTEM
    // ===================================================================

    // Prediction system
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Sensory prediction and error computation)")
        .def(py::init<>())
        .def("update", &PredictionSystem::update,
             py::arg("brain"), py::arg("sensoryInput"),
             "Update prediction system with sensory input")
        .def("getPredictionError", &PredictionSystem::getPredictionError,
             "Get current prediction error")
        .def("getPredictedSensory", &PredictionSystem::getPredictedSensory,
             "Get predicted sensory input")
        .def("setPredictionError", &PredictionSystem::setPredictionError, py::arg("error"),
             "Set prediction error value")
        .def("getPredictionAccuracy", &PredictionSystem::getPredictionAccuracy,
             "Get prediction accuracy measure");

    // Neural prediction
    py::class_<NeuralPrediction>(m, "NeuralPrediction", R"pbdoc(Neural prediction using internal models)")
        .def(py::init<>())
        .def("initialize", &NeuralPrediction::initialize,
             py::arg("brain"),
             "Initialize with brain reference")
        .def("predict", &NeuralPrediction::predict, py::arg("sensoryState"),
             "Make prediction based on sensory state")
        .def("getPredictedAction", &NeuralPrediction::getPredictedAction,
             "Get predicted action")
        .def("getPredictionConfidence", &NeuralPrediction::getPredictionConfidence,
             "Get prediction confidence")
        .def("update", &NeuralPrediction::update, py::arg("dt"),
             "Update prediction model");

    // ===================================================================
    // FACTORY FUNCTIONS
    // ===================================================================

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

    m.def("createAgentBody", []() -> std::shared_ptr<AgentBody> {
        return std::make_shared<AgentBody>();
    }, "Create a new agent body");

    m.def("createAction", [](ActionType type, const std::vector<float>& params) -> std::shared_ptr<Action> {
        return std::make_shared<Action>(type, params);
    }, py::arg("type"), py::arg("parameters"), "Create a new action");

    m.def("createActionResult", [](float reward, bool success, const std::string& message) -> std::shared_ptr<ActionResult> {
        return std::make_shared<ActionResult>(reward, success, message);
    }, py::arg("reward"), py::arg("success"), py::arg("message") = "",
        "Create a new action result");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // ===================================================================
    // UTILITY FUNCTIONS
    // ===================================================================

    m.def("version", []() {
        return "NLM Python Bindings v1.0.0 (Enhanced)";
    }, "Get NLM Python bindings version");

    m.def("print_banner", []() {
        std::cout << "=== NLM Python Bindings (Enhanced) ===" << std::endl;
        std::cout << "Complete neural simulation framework with:" << std::endl;
        std::cout << "  - Full memory systems (Working, Episodic, Associative, Semantic, Procedural)" << std::endl;
        std::cout << "  - Neuromodulation systems (Dopamine, Curiosity, Novelty, PredictionError, Reward)" << std::endl;
        std::cout << "  - Brain development systems (Development, Maturation, Synaptogenesis, Pruning)" << std::endl;
        std::cout << "  - Cognition systems (NeuralPlanner, ConceptFormation, AttentionalSelection)" << std::endl;
        std::cout << "  - Prediction and learning systems" << std::endl;
        std::cout << "  - Enhanced error handling and Pythonic APIs" << std::endl;
    }, "Print NLM Python bindings banner");

    // Example simulation
    m.def("run_simple_example", []() {
        std::cout << "=== NLM Simple Example ===" << std::endl;
        
        // Create brain with default config
        auto config = createDefaultConfig();
        auto brain = createBrain(config);
        brain->initialize();
        
        std::cout << "Brain created with " << brain->getTotalNeuronCount() << " neurons" << std::endl;
        
        // Create world
        auto world = createSimpleWorld();
        world->configure(20, 20, 8, 8);
        world->reset();
        
        std::cout << "World configured: 20x20 grid with 8x8 vision" << std::endl;
        
        // Create agent
        auto agent = createAgentBrain(brain);
        agent->initialize(world);
        
        std::cout << "Agent created and initialized" << std::endl;
        
        // Enable some subsystems
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        
        // Run simulation
        std::cout << "Running simulation for 100 steps..." << std::endl;
        for (int step = 0; step < 100; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            if (step % 25 == 0) {
                std::cout << "Step " << step << ": "
                          << brain->getFiringNeuronCount() << " firing neurons, "
                          << agent->getCuriosityLevel() << " curiosity, "
                          << brain->getDevelopmentalStage() << " development stage" << std::endl;
            }
        }
        
        std::cout << "=== Simulation Complete ===" << std::endl;
        std::cout << "Final stats: " << brain->getTotalSpikeCount() << " total spikes" << std::endl;
        std::cout << "Curiosity: " << agent->getCuriosityLevel() << std::endl;
        std::cout << "Novelty: " << agent->getNoveltyLevel() << std::endl;
        std::cout << "Prediction Error: " << agent->getPredictionError() << std::endl;
        
        return std::make_tuple(brain, world, agent);
    }, "Run a simple example simulation");

    } // namespace nlm

} // namespace pybind11
