#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <utility>

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
#include "../src/memory/Memory.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/development/Synaptogenesis.hpp"
#include "../src/development/Pruning.hpp"
#include "../src/development/Maturation.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/prediction/NeuralPrediction.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/brain/NeuralRegion.hpp"
#include "../src/performance/MemoryPool.hpp"
#include "src/experiments/ExperimentRunner.hpp"
#include "src/experiments/Metrics.hpp"

namespace py = pybind11;
namespace nlm {

// Forward declarations for complex types
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class SemanticMemory;
class ProceduralMemory;
class NeuralAssociativeMemory;
class Synaptogenesis;
class Pruning;
class Maturation;
class DevelopmentSystem;
class NeuralPrediction;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class MemoryPool;
class EventQueue;
class CheckpointSystem;

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A comprehensive Python API for the NLM C++ neural simulation framework.
        
        This API provides complete access to all brain systems including:
        - Neural computation and dynamics
        - Memory systems (working, episodic, semantic, procedural, associative)
        - Neuromodulation (dopamine, reward, curiosity, novelty, prediction error)
        - Cognitive functions (attention, planning, concept formation)
        - Developmental processes (synaptogenesis, pruning, maturation)
        - Prediction and learning systems
        - Performance optimization (memory pools, checkpointing)
        - Experiment control and metrics
        
        For detailed documentation, see:
        - easy_usage.md: Quick start for beginners
        - HOW_TO_USE.md: Comprehensive guide
        - docs/ARCHITECTURE.md: System architecture
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ===== ID TYPES =====
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

    // ===== ENUMERATIONS =====
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

    py::enum_<PlasticityFlags>(m, "PlasticityFlags", R"pbdoc(Plasticity mechanism flags)pbdoc")
        .value("STDP", PlasticityFlags::STDP)
        .value("Hebbian", PlasticityFlags::Hebbian)
        .value("Structural", PlasticityFlags::Structural)
        .value("Developmental", PlasticityFlags::Developmental)
        .export_values();

    py::enum_<SimulationStep>(m, "SimulationStep", R"pbdoc(Simulation step counter)pbdoc")
        .value("MAX", SimulationStep::MAX)
        .export_values();

    // ===== CONFIGURATION =====
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
        })
        .def("set", &Config::set, py::arg("key"), py::arg("value"),
             "Set a configuration value");

    // ===== SENSORY INPUT =====
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

    // ===== MOTOR SYSTEMS =====
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
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
            return "<Action: " + action.getName() + ">";
        });

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // ===== WORLD SYSTEMS =====
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
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

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
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

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
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

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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

    // ===== MEMORY SYSTEMS =====
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Neural working memory system)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"))
        .def("store", &NeuralWorkingMemory::store, py::arg("pattern"), py::arg("strength") = 1.0f)
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron, py::arg("neuron"), py::arg("activation"))
        .def("retrieve", &NeuralWorkingMemory::retrieve)
        .def("contains", &NeuralWorkingMemory::contains, py::arg("neuron"))
        .def("getNeuronActivation", &NeuralWorkingMemory::getNeuronActivation, py::arg("neuron"))
        .def("update", &NeuralWorkingMemory::update, py::arg("dt"))
        .def("clear", &NeuralWorkingMemory::clear)
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces)
        .def("getCapacity", &NeuralWorkingMemory::getCapacity)
        .def("setCapacity", &NeuralWorkingMemory::setCapacity, py::arg("capacity"))
        .def("getDecayRate", &NeuralWorkingMemory::getDecayRate)
        .def("setDecayRate", &NeuralWorkingMemory::setDecayRate, py::arg("rate"))
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons)
        .def("strengthenMemory", &NeuralWorkingMemory::strengthenMemory, py::arg("factor"))
        .def("runCompetition", &NeuralWorkingMemory::runCompetition)
        .def("isWinning", &NeuralWorkingMemory::isWinning, py::arg("neuron"))
        .def("getMemoryActivity", &NeuralWorkingMemory::getMemoryActivity)
        .def("__repr__", [](const NeuralWorkingMemory& memory) {
            return "<NeuralWorkingMemory: " + std::to_string(memory.getActiveTraces()) + " traces>";
        });

    py::class_<EpisodicMemoryItem>(m, "EpisodicMemoryItem", R"pbdoc(Neural episodic memory item)pbdoc")
        .def(py::init<>())
        .def_readwrite("timestamp", &EpisodicMemoryItem::timestamp)
        .def_readwrite("sensoryState", &EpisodicMemoryItem::sensoryState)
        .def_readwrite("positionX", &EpisodicMemoryItem::positionX)
        .def_readwrite("positionY", &EpisodicMemoryItem::positionY)
        .def_readwrite("orientation", &EpisodicMemoryItem::orientation)
        .def_readwrite("action", &EpisodicMemoryItem::action)
        .def_readwrite("reward", &EpisodicMemoryItem::reward)
        .def_readwrite("energy", &EpisodicMemoryItem::energy)
        .def_readwrite("novelty", &EpisodicMemoryItem::novelty)
        .def_readwrite("resultingSensoryState", &EpisodicMemoryItem::resultingSensoryState)
        .def_readwrite("resultingReward", &EpisodicMemoryItem::resultingReward)
        .def_readwrite("activeNeurons", &EpisodicMemoryItem::activeNeurons)
        .def_readwrite("neuronActivations", &EpisodicMemoryItem::neuronActivations)
        .def_readwrite("age", &EpisodicMemoryItem::age)
        .def("__repr__", [](const EpisodicMemoryItem& item) {
            return "<EpisodicMemoryItem: t=" + std::to_string(item.timestamp) + ", reward=" + 
                   std::to_string(item.reward) + ">";
        });

    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Neural episodic memory system)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralEpisodicMemory::initialize, py::arg("brain"))
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode, py::arg("episode"))
        .def("retrieveSimilar", &NeuralEpisodicMemory::retrieveSimilar, 
             py::arg("sensoryPattern"), py::arg("maxResults") = 5)
        .def("retrieveTemporal", &NeuralEpisodicMemory::retrieveTemporal,
             py::arg("startTime"), py::arg("endTime"), py::arg("maxResults") = 10)
        .def("retrieveByLocation", &NeuralEpisodicMemory::retrieveByLocation,
             py::arg("x"), py::arg("y"), py::arg("radius"), py::arg("maxResults") = 5)
        .def("retrieveAfterAction", &NeuralEpisodicMemory::retrieveAfterAction,
             py::arg("action"), py::arg("maxResults") = 5)
        .def("replayEpisode", &NeuralEpisodicMemory::replayEpisode, py::arg("episode"))
        .def("updateRelevance", &NeuralEpisodicMemory::updateRelevance,
             py::arg("episodeId"), py::arg("relevanceDelta"))
        .def("getEpisode", &NeuralEpisodicMemory::getEpisode, py::arg("index"))
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount)
        .def("getRecentEpisodes", &NeuralEpisodicMemory::getRecentEpisodes, py::arg("count"))
        .def("getAverageReward", &NeuralEpisodicMemory::getAverageReward)
        .def("consolidate", &NeuralEpisodicMemory::consolidate, py::arg("threshold"))
        .def("clear", &NeuralEpisodicMemory::clear)
        .def("enableReplay", &NeuralEpisodicMemory::enableReplay, py::arg("enable"))
        .def("isReplayEnabled", &NeuralEpisodicMemory::isReplayEnabled)
        .def("getEpisodesForReplay", &NeuralEpisodicMemory::getEpisodesForReplay, py::arg("count"))
        .def("replaySequence", &NeuralEpisodicMemory::replaySequence, py::arg("episodeIds"))
        .def("__repr__", [](const NeuralEpisodicMemory& memory) {
            return "<NeuralEpisodicMemory: " + std::to_string(memory.getEpisodeCount()) + " episodes>";
        });

    py::class_<NeuralAssociativeMemory>(m, "NeuralAssociativeMemory", R"pbdoc(Neural associative memory system)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralAssociativeMemory::initialize, py::arg("brain"))
        .def("associate", &NeuralAssociativeMemory::associate,
             py::arg("patternA"), py::arg("patternB"), py::arg("strength") = 1.0f)
        .def("associateFromExperience", &NeuralAssociativeMemory::associateFromExperience,
             py::arg("episode"))
        .def("retrieve", &NeuralAssociativeMemory::retrieve,
             py::arg("queryPattern"), py::arg("maxResults") = 5)
        .def("getAssociationStrength", &NeuralAssociativeMemory::getAssociationStrength,
             py::arg("patternA"), py::arg("patternB"))
        .def("updateAssociation", &NeuralAssociativeMemory::updateAssociation,
             py::arg("patternA"), py::arg("patternB"), py::arg("delta"))
        .def("spreadActivation", &NeuralAssociativeMemory::spreadActivation,
             py::arg("cuePattern"), py::arg("steps") = 2)
        .def("clear", &NeuralAssociativeMemory::clear)
        .def("getAssociationCount", &NeuralAssociativeMemory::getAssociationCount)
        .def("__repr__", [](const NeuralAssociativeMemory& memory) {
            return "<NeuralAssociativeMemory: " + std::to_string(memory.getAssociationCount()) + " associations>";
        });

    // ===== NEUROMODULATION SYSTEMS =====
    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(Abstract neuromodulator)pbdoc")
        .def("getName", &Neuromodulator::getName)
        .def("getLevel", &Neuromodulator::getLevel)
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor)
        .def("update", &Neuromodulator::update, py::arg("dt"));

    py::class_<Dopamine, Neuromodulator>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator)pbdoc")
        .def(py::init<>())
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"))
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"));

    py::class_<Reward>(m, "Reward", R"pbdoc(Reward system)pbdoc")
        .def(py::init<>())
        .def("getValue", &Reward::getValue)
        .def("setValue", &Reward::setValue, py::arg("value"))
        .def("add", &Reward::add, py::arg("delta"))
        .def("reset", &Reward::reset)
        .def("getHistory", &Reward::getHistory)
        .def("clearHistory", &Reward::clearHistory);

    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error system)pbdoc")
        .def(py::init<>());

    py::class_<Novelty>(m, "Novelty", R"pbdoc( novelty detection system)pbdoc")
        .def(py::init<>());

    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity neuromodulator)pbdoc")
        .def(py::init<>());

    // ===== DEVELOPMENT SYSTEMS =====
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Development system)pbdoc")
        .def(py::init<>());

    py::class_<Synaptogenesis>(m, "Synaptogenesis", R"pbdoc(Synaptogenesis system)pbdoc")
        .def(py::init<>());

    py::class_<Pruning>(m, "Pruning", R"pbdoc(Pruning system)pbdoc")
        .def(py::init<>());

    py::class_<Maturation>(m, "Maturation", R"pbdoc(Maturation system)pbdoc")
        .def(py::init<>());

    // ===== PREDICTION SYSTEMS =====
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Prediction system)pbdoc")
        .def(py::init<>());

    py::class_<NeuralPrediction>(m, "NeuralPrediction", R"pbdoc(Neural prediction system)pbdoc")
        .def(py::init<>());

    // ===== COGNITION SYSTEMS =====
    py::class_<AttentionalSelection>(m, "AttentionalSelection", R"pbdoc(Attentional selection system)pbdoc")
        .def(py::init<>())
        .def("initialize", &AttentionalSelection::initialize, py::arg("brain"))
        .def("processCompetition", &AttentionalSelection::processCompetition,
             py::arg("competitors"), py::arg("globalInhibition") = 0.5f)
        .def("focusOnRegion", &AttentionalSelection::focusOnRegion, py::arg("region"))
        .def("releaseAttention", &AttentionalSelection::releaseAttention)
        .def("getAttendedRegions", &AttentionalSelection::getAttendedRegions)
        .def("setInhibitionStrength", &AttentionalSelection::setInhibitionStrength, py::arg("strength"))
        .def("setExcitationStrength", &AttentionalSelection::setExcitationStrength, py::arg("strength"))
        .def("setCompetitionThreshold", &AttentionalSelection::setCompetitionThreshold, py::arg("threshold"))
        .def("getInhibitionFor", &AttentionalSelection::getInhibitionFor, py::arg("neuron"))
        .def("getExcitationFor", &AttentionalSelection::getExcitationFor, py::arg("neuron"))
        .def("update", &AttentionalSelection::update, py::arg("dt"))
        .def("isAttended", &AttentionalSelection::isAttended, py::arg("neuron"))
        .def("getWinners", &AttentionalSelection::getWinners)
        .def("applyTopDownBias", &AttentionalSelection::applyTopDownBias, py::arg("neuron"), py::arg("biasStrength"))
        .def("applyBottomUpSalience", &AttentionalSelection::applyBottomUpSalience, py::arg("neuron"), py::arg("salienceStrength"))
        .def("reset", &AttentionalSelection::reset);

    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planning system)pbdoc")
        .def(py::init<>());

    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation system)pbdoc")
        .def(py::init<>());

    // ===== NEURAL SYSTEMS =====
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
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
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
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
        .def("getWorkingMemory", &Brain::getWorkingMemory, "Get working memory")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory, "Get episodic memory")
        .def("getDopamine", &Brain::getDopamine, "Get dopamine neuromodulator")
        .def("getCuriosity", &Brain::getCuriosity, "Get curiosity neuromodulator")
        .def("getNovelty", &Brain::getNovelty, "Get novelty detection system")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal, "Get prediction error signal")
        .def("getPredictionSystem", &Brain::getPredictionSystem, "Get prediction system")
        .def("getPlanner", &Brain::getPlanner, "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation, "Get concept formation system")
        .def("getAttention", &Brain::getAttention, "Get attentional selection system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem, "Get development system")
        .def("getNeuromodulationLevel", &Brain::getNeuromodulationLevel, "Get neuromodulation level")
        .def("getCuriosityLevel", &Brain::getCuriosityLevel, "Get curiosity level")
        .def("getNoveltyLevel", &Brain::getNoveltyLevel, "Get novelty level")
        .def("getPredictionError", &Brain::getPredictionError, "Get prediction error")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage, "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig, py::return_value_policy::reference_internal, "Get the configuration")
        .def("logStatus", &Brain::logStatus, "Log brain status");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"), "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize, "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize, "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand, "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation, py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"), "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage, "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel, "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError, "Get prediction error")
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain, py::return_value_policy::reference_internal, "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // ===== FACTORY FUNCTIONS =====
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

    // ===== CONSTANTS =====
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // ===== ADVANCED RESEARCH API =====
    
    // Experiment API
    py::class_<Phase6Config>(m, "Phase6Config", R"pbdoc(Phase 6 experiment configuration)pbdoc")
        .def(py::init<>())
        .def_readwrite("neuronCount", &Phase6Config::neuronCount)
        .def_readwrite("maxSteps", &Phase6Config::maxSteps)
        .def_readwrite("enableCheckpointing", &Phase6Config::enableCheckpointing)
        .def_readwrite("enableReplay", &Phase6Config::enableReplay)
        .def_readwrite("enableDevelopment", &Phase6Config::enableDevelopment)
        .def_readwrite("checkpointPath", &Phase6Config::checkpointPath);

    py::class_<Phase6IntegrationResult>(m, "Phase6IntegrationResult", R"pbdoc(Phase 6 integration experiment results)pbdoc")
        .def(py::init<>())
        .def_readwrite("totalReward", &Phase6IntegrationResult::totalReward)
        .def_readwrite("avgFiringRate", &Phase6IntegrationResult::avgFiringRate)
        .def_readwrite("memoryEpisodesStored", &Phase6IntegrationResult::memoryEpisodesStored)
        .def_readwrite("noveltyLevel", &Phase6IntegrationResult::noveltyLevel)
        .def_readwrite("curiosityLevel", &Phase6IntegrationResult::curiosityLevel)
        .def_readwrite("dopamineLevel", &Phase6IntegrationResult::dopamineLevel)
        .def_readwrite("memoryWorkingMemoryIntegrated", &Phase6IntegrationResult::memoryWorkingMemoryIntegrated)
        .def_readwrite("memoryEpisodicMemoryIntegrated", &Phase6IntegrationResult::memoryEpisodicMemoryIntegrated)
        .def_readwrite("neuromodulationIntegrated", &Phase6IntegrationResult::neuromodulationIntegrated)
        .def_readwrite("predictionIntegrated", &Phase6IntegrationResult::predictionIntegrated)
        .def_readwrite("developmentIntegrated", &Phase6IntegrationResult::developmentIntegrated)
        .def_readwrite("checkpointingWorks", &Phase6IntegrationResult::checkpointingWorks)
        .def_readwrite("totalWallClockTime", &Phase6IntegrationResult::totalWallClockTime);

    py::class_<Phase6IntegratedExperiment>(m, "Phase6IntegratedExperiment", R"pbdoc(Phase 6 integration experiment)pbdoc")
        .def(py::init<>())
        .def("verifyIntegration", &Phase6IntegratedExperiment::verifyIntegration)
        .def("testMemoryIntegration", &Phase6IntegratedExperiment::testMemoryIntegration)
        .def("testNeuromodulationIntegration", &Phase6IntegratedExperiment::testNeuromodulationIntegration)
        .def("testCheckpointing", &Phase6IntegratedExperiment::testCheckpointing)
        .def("testReplay", &Phase6IntegratedExperiment::testReplay)
        .def("run", &Phase6IntegratedExperiment::run, py::arg("config"));

    // Convenience simulation runner
    m.def("run_agent_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                                    std::shared_ptr<AgentBrain> agent, size_t numSteps) {
        for (size_t step = 0; step < numSteps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        return true;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps"),
       "Run a complete agent simulation loop");

    m.def("run_brain_dynamics", [](std::shared_ptr<Brain> brain, size_t numSteps) {
        for (size_t step = 0; step < numSteps; ++step) {
            brain->step(step);
        }
        return brain->getTotalSpikeCount();
    }, py::arg("brain"), py::arg("numSteps"),
       "Run a brain dynamics simulation");

    m.def("get_brain_metrics", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, float> metrics;
        metrics["totalNeurons"] = static_cast<float>(brain->getTotalNeuronCount());
        metrics["totalSynapses"] = static_cast<float>(brain->getTotalSynapseCount());
        metrics["activeNeurons"] = static_cast<float>(brain->getActiveNeuronCount());
        metrics["firingNeurons"] = static_cast<float>(brain->getFiringNeuronCount());
        metrics["averageFiringRate"] = brain->getAverageFiringRate();
        metrics["excitatoryInhibitionRatio"] = brain->getExcitationInhibitionRatio();
        metrics["totalSpikes"] = static_cast<float>(brain->getTotalSpikeCount());
        return metrics;
    }, py::arg("brain"), "Get comprehensive brain metrics");

    // ===== PERFORMANCE SYSTEMS =====
    py::class_<MemoryPool>(m, "MemoryPool", R"pbdoc(Optimized memory pool)pbdoc")
        .def(py::init<>());

    py::class_<EventQueue>(m, "EventQueue", R"pbdoc(Event-driven simulation queue)pbdoc")
        .def(py::init<>());

    py::class_<CheckpointSystem>(m, "CheckpointSystem", R"pbdoc(Checkpoint and recovery system)pbdoc")
        .def(py::init<>());

    // ===== EXPERIMENTATION TOOLS =====
    py::class_<Metrics>(m, "Metrics", R"pbdoc(Experiment metrics collector)pbdoc")
        .def(py::init<>());

    py::class_<ExperimentRunner>(m, "ExperimentRunner", R"pbdoc(Experiment execution framework)pbdoc")
        .def(py::init<>());
}

} // namespace nlm
