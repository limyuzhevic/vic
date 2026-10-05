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
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "src/memory/NeuralAssociativeMemory.hpp"
#include "../src/memory/Memory.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/development/Synaptogenesis.hpp"
#include "../src/development/Pruning.hpp"
#include "../src/development/Maturation.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/prediction/NeuralPrediction.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/NeuralRegion.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"
#include "../src/plasticity/PlasticityRule.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

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

    // ========== MEMORY SYSTEM BINDINGS ==========

    py::class_<Memory, std::shared_ptr<Memory>>(m, "Memory", R"pbdoc(Base class for memory systems)pbdoc")
        .def("getType", &Memory::getType)
        .def("getCapacity", &Memory::getCapacity)
        .def("isInitialized", &Memory::isInitialized)
        .def("initialize", &Memory::initialize)
        .def("clear", &Memory::clear);

    py::class_<NeuralWorkingMemory, std::shared_ptr<NeuralWorkingMemory>>(m, "NeuralWorkingMemory", R"pbdoc(Working memory using persistent neural activity)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"))
        .def("store", &NeuralWorkingMemory::store, py::arg("pattern"), py::arg("strength") = 1.0f)
        .def("retrieve", &NeuralWorkingMemory::retrieve)
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron)
        .def("contains", &NeuralWorkingMemory::contains)
        .def("getNeuronActivation", &NeuralWorkingMemory::getNeuronActivation)
        .def("update", &NeuralWorkingMemory::update, py::arg("dt"))
        .def("clear", &NeuralWorkingMemory::clear)
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces)
        .def("getCapacity", &NeuralWorkingMemory::getCapacity)
        .def("setCapacity", &NeuralWorkingMemory::setCapacity)
        .def("getDecayRate", &NeuralWorkingMemory::getDecayRate)
        .def("setDecayRate", &NeuralWorkingMemory::setDecayRate)
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons)
        .def("strengthenMemory", &NeuralWorkingMemory::strengthenMemory)
        .def("runCompetition", &NeuralWorkingMemory::runCompetition)
        .def("isWinning", &NeuralWorkingMemory::isWinning)
        .def("getMemoryActivity", &NeuralWorkingMemory::getMemoryActivity);

    py::class_<EpisodicMemoryItem>(m, "EpisodicMemoryItem", R"pbdoc(Individual memory episode)pbdoc")
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
        .def_readwrite("age", &EpisodicMemoryItem::age);

    py::class_<NeuralEpisodicMemory, std::shared_ptr<NeuralEpisodicMemory>>(m, "NeuralEpisodicMemory", R"pbdoc(Episodic memory for experiences)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralEpisodicMemory::initialize, py::arg("brain"))
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode, py::arg("episode"))
        .def("retrieveSimilar", &NeuralEpisodicMemory::retrieveSimilar, py::arg("sensoryPattern"), py::arg("maxResults") = 5)
        .def("retrieveTemporal", &NeuralEpisodicMemory::retrieveTemporal, py::arg("startTime"), py::arg("endTime"), py::arg("maxResults") = 10)
        .def("retrieveByLocation", &NeuralEpisodicMemory::retrieveByLocation, py::arg("x"), py::arg("y"), py::arg("radius"), py::arg("maxResults") = 5)
        .def("retrieveAfterAction", &NeuralEpisodicMemory::retrieveAfterAction, py::arg("action"), py::arg("maxResults") = 5)
        .def("replayEpisode", &NeuralEpisodicMemory::replayEpisode, py::arg("episode"))
        .def("updateRelevance", &NeuralEpisodicMemory::updateRelevance)
        .def("getEpisode", &NeuralEpisodicMemory::getEpisode, py::arg("index"))
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount)
        .def("getRecentEpisodes", &NeuralEpisodicMemory::getRecentEpisodes, py::arg("count"))
        .def("getAverageReward", &NeuralEpisodicMemory::getAverageReward)
        .def("consolidate", &NeuralEpisodicMemory::consolidate, py::arg("relevanceThreshold"))
        .def("clear", &NeuralEpisodicMemory::clear)
        .def("enableReplay", &NeuralEpisodicMemory::enableReplay, py::arg("enable"))
        .def("isReplayEnabled", &NeuralEpisodicMemory::isReplayEnabled)
        .def("getEpisodesForReplay", &NeuralEpisodicMemory::getEpisodesForReplay, py::arg("count"))
        .def("replaySequence", &NeuralEpisodicMemory::replaySequence, py::arg("episodeIds"));

    py::class_<NeuralAssociativeMemory, std::shared_ptr<NeuralAssociativeMemory>>(m, "NeuralAssociativeMemory", R"pbdoc(Associative memory for relationships)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralAssociativeMemory::initialize, py::arg("brain"))
        .def("associate", &NeuralAssociativeMemory::associate, py::arg("patternA"), py::arg("patternB"), py::arg("strength") = 1.0f)
        .def("associateFromExperience", &NeuralAssociativeMemory::associateFromExperience, py::arg("episode"))
        .def("retrieve", &NeuralAssociativeMemory::retrieve, py::arg("queryPattern"), py::arg("maxResults") = 5)
        .def("getAssociationStrength", &NeuralAssociativeMemory::getAssociationStrength, py::arg("patternA"), py::arg("patternB"))
        .def("updateAssociation", &NeuralAssociativeMemory::updateAssociation, py::arg("patternA"), py::arg("patternB"), py::arg("delta"))
        .def("spreadActivation", &NeuralAssociativeMemory::spreadActivation, py::arg("cuePattern"), py::arg("steps") = 2)
        .def("clear", &NeuralAssociativeMemory::clear)
        .def("getAssociationCount", &NeuralAssociativeMemory::getAssociationCount);

    // ========== NEUROMODULATION SYSTEM BINDINGS ==========

    py::class_<Neuromodulator, std::shared_ptr<Neuromodulator>>(m, "Neuromodulator", R"pbdoc(Abstract neuromodulator)pbdoc")
        .def("getName", &Neuromodulator::getName)
        .def("getLevel", &Neuromodulator::getLevel)
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor)
        .def("update", &Neuromodulator::update, py::arg("dt"));

    py::class_<Dopamine, std::shared_ptr<Dopamine>>(m, "Dopamine", R"pbdoc(Reward prediction error signaling)pbdoc")
        .def(py::init<>())
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"))
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"));

    py::class_<Novelty, std::shared_ptr<Novelty>>(m, "Novelty", R"pbdoc(Stimulus novelty detection)pbdoc")
        .def(py::init<>())
        .def("getLevel", &Novelty::getLevel)
        .def("getSurprise", &Novelty::getSurprise)
        .def("update", &Novelty::update, py::arg("pattern"), py::arg("dt"))
        .def("getActivationThreshold", &Novelty::getActivationThreshold)
        .def("setActivationThreshold", &Novelty::setActivationThreshold, py::arg("threshold"));

    py::class_<Curiosity, std::shared_ptr<Curiosity>>(m, "Curiosity", R"pbdoc(Exploration motivation)pbdoc")
        .def(py::init<>())
        .def("initialize", &Curiosity::initialize, py::arg("brain"))
        .def("getLevel", &Curiosity::getLevel)
        .def("getExplorationDrive", &Curiosity::getExplorationDrive)
        .def("update", &Curiosity::update, py::arg("novelty"), py::arg("predictionError"), py::arg("dt"))
        .def("setNoveltyWeight", &Curiosity::setNoveltyWeight, py::arg("weight"))
        .def("setPredictionErrorWeight", &Curiosity::setPredictionErrorWeight, py::arg("weight"))
        .def("reset", &Curiosity::reset);

    py::class_<PredictionError, std::shared_ptr<PredictionError>>(m, "PredictionError", R"pbdoc(Prediction error signaling)pbdoc")
        .def(py::init<>())
        .def("getLevel", &PredictionError::getLevel)
        .def("getError", &PredictionError::getError)
        .def("update", &PredictionError::update, py::arg("predicted"), py::arg("actual"), py::arg("dt"))
        .def("getConfidence", &PredictionError::getConfidence)
        .def("reset", &PredictionError::reset);

    // ========== DEVELOPMENT SYSTEM BINDINGS ==========

    py::class_<DevelopmentSystem, std::shared_ptr<DevelopmentSystem>>(m, "DevelopmentSystem", R"pbdoc(Neural development orchestration)pbdoc")
        .def(py::init<>())
        .def("getStage", &DevelopmentSystem::getStage)
        .def("setStage", &DevelopmentSystem::setStage, py::arg("stage"))
        .def("advanceStage", &DevelopmentSystem::advanceStage)
        .def("getStageName", &DevelopmentSystem::getStageName)
        .def("update", [](DevelopmentSystem& self, Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
            self.update(brain, rng, dt);
        }, py::arg("brain"), py::arg("rng"), py::arg("dt"))
        .def("update", [](DevelopmentSystem& self, Brain* brain, SimulationStep currentStep) {
            self.update(brain, currentStep);
        }, py::arg("brain"), py::arg("currentStep"))
        .def("getPlasticityModifier", &DevelopmentSystem::getPlasticityModifier)
        .def("isCriticalPeriod", &DevelopmentSystem::isCriticalPeriod)
        .def("getCriticalPeriodProgress", &DevelopmentSystem::getCriticalPeriodProgress)
        .def("getDevelopmentalAge", &DevelopmentSystem::getDevelopmentalAge)
        .def("setDevelopmentalAge", &DevelopmentSystem::setDevelopmentalAge, py::arg("age"));

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

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

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

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

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

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
             "Log brain status");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
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
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // ========== ADVANCED FUNCTIONALITY ==========

    // SelfModel: Internal self-representation
    py::class_<SelfModel, std::shared_ptr<SelfModel>>(m, "SelfModel", R"pbdoc(Internal self-representation)pbdoc")
        .def(py::init<>())
        .def("initialize", &SelfModel::initialize, py::arg("brain"))
        .def("recordSelfAction", &SelfModel::recordSelfAction, py::arg("action"), py::arg("beforeState"), py::arg("afterState"))
        .def("predictActionConsequence", &SelfModel::predictActionConsequence, py::arg("action"), py::arg("currentState"))
        .def("getSelfModelConfidence", &SelfModel::getSelfModelConfidence, py::arg("action"))
        .def("computeSelfGeneratedLikeness", [](SelfModel& self, const std::vector<float>& beforeState, const std::vector<float>& afterState, ActionType action) {
            return self.computeSelfGeneratedLikeness(beforeState, afterState, action);
        }, py::arg("beforeState"), py::arg("afterState"), py::arg("action"))
        .def("getPreferredAction", &SelfModel::getPreferredAction, py::arg("state"))
        .def("updateSelfModel", &SelfModel::updateSelfModel, py::arg("predicted"), py::arg("actual"), py::arg("action"))
        .def("getCapabilityLevel", &SelfModel::getCapabilityLevel)
        .def("getBodyAwareness", &SelfModel::getBodyAwareness)
        .def("clear", &SelfModel::clear)
        .def("hasSelfModel", &SelfModel::hasSelfModel);

    // SocialLearning: Learning from others
    py::class_<SocialLearning, std::shared_ptr<SocialLearning>>(m, "SocialLearning", R"pbdoc(Social learning from other agents)pbdoc")
        .def(py::init<>())
        .def("initialize", &SocialLearning::initialize, py::arg("brain"))
        .def("observeAgentAction", &SocialLearning::observeAgentAction, py::arg("observedAction"), py::arg("observerState"), py::arg("resultingState"))
        .def("canImitate", &SocialLearning::canImitate, py::arg("observedAction"))
        .def("getImitationAction", &SocialLearning::getImitationAction, py::arg("currentState"))
        .def("learnCommunicationSignal", &SocialLearning::learnCommunicationSignal, py::arg("signalPattern"), py::arg("signalReward"))
        .def("detectSignal", &SocialLearning::detectSignal, py::arg("neuralPattern"))
        .def("getSignalPattern", &SocialLearning::getSignalPattern)
        .def("getSignalMeaning", &SocialLearning::getSignalMeaning)
        .def("updateSocialKnowledge", &SocialLearning::updateSocialKnowledge, py::arg("interactionReward"))
        .def("clear", &SocialLearning::clear)
        .def("hasSocialKnowledge", &SocialLearning::hasSocialKnowledge)
        .def("getObservationCount", &SocialLearning::getObservationCount);

    // ========== HIGH-LEVEL CONVENIENCE FUNCTIONS ==========

    // Brain creation and initialization
    m.def("createBrainWithDevelopment", [](bool enableDevelopment, bool enableCuriosity, bool enableStructuralPlasticity, bool enableRewardModulation) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("brain.development.enabled", enableDevelopment);
        config->set("neuromod.dopamine.enable", enableRewardModulation);
        config->set("neuromod.curiosity.enable", enableCuriosity);
        config->set("plasticity.structural.enable", enableStructuralPlasticity);
        return std::make_shared<Brain>(config);
    }, py::arg("enableDevelopment") = true, py::arg("enableCuriosity") = true,
        py::arg("enableStructuralPlasticity") = true, py::arg("enableRewardModulation") = true,
        "Create a brain with development and advanced features enabled");

    m.def("createExplanatoryAgent", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, bool enableLearning = true) -> std::shared_ptr<AgentBrain> {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        if (enableLearning) {
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
            agent->enableDevelopment(true);
            agent->enableStructuralPlasticity(true);
        }
        return agent;
    }, py::arg("brain"), py::arg("world"), py::arg("enableLearning") = true,
        "Create an agent brain with all learning systems enabled");

    // World configuration helpers
    m.def("createWorld", [](int width, int height, int visionWidth, int visionHeight) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
        "Create and configure a simple world");

    m.def("createWorldWithObjects", [](int width, int height, int visionWidth, int visionHeight) -> std::shared_ptr<SimpleWorld> {
        auto world = createWorld(width, height, visionWidth, visionHeight);
        // Add some default objects
        world->addObject(WorldObject(2.0f, 2.0f, WorldObjectType::Resource, 1.0f));
        world->addObject(WorldObject(5.0f, 5.0f, WorldObjectType::Hazard, -1.0f));
        world->setAgentStart(width/2, height/2);
        return world;
    }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
        "Create a world with objects and agent start position");

    // Simulation helpers
    m.def("runEpisode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent, int steps, bool enableLearning = true) -> std::unordered_map<std::string, float> {
        if (enableLearning) {
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
            agent->enableDevelopment(true);
        }

        auto agentBody = world->getAgentBody();
        world->reset();
        agent->initialize(*world);

        std::vector<float> visionHistory;
        float totalReward = 0.0f;

        for (int step = 0; step < steps; ++step) {
            world->update(0.1f);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());

            auto result = world->getActionResult();
            if (result) {
                totalReward += result->reward;
            }
        }

        std::unordered_map<std::string, float> metrics;
        metrics["total_reward"] = totalReward;
        metrics["steps"] = static_cast<float>(steps);
        metrics["avg_reward"] = totalReward / steps;
        metrics["curiosity_level"] = agent->getCuriosityLevel();
        metrics["novelty_level"] = agent->getNoveltyLevel();
        metrics["prediction_error"] = agent->getPredictionError();
        metrics["development_stage"] = static_cast<float>(brain->getDevelopmentalStage());

        return metrics;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"), py::arg("enableLearning") = true,
        "Run a complete episode with automatic learning");

    // Memory management helpers
    m.def("enableAllMemorySystems", [](std::shared_ptr<Brain> brain) {
        if (brain) {
            // Note: Memory systems are attached internally, this is just for convenience
            // The actual memory systems are automatically created and initialized
            // when the brain is initialized with the right configuration
        }
    }, py::arg("brain"),
        "Enable all memory systems (working, episodic, associative)");

    // Visualization helpers
    m.def("exportBrainState", [](std::shared_ptr<Brain> brain, const std::string& filename) {
        brain->save(filename);
    }, py::arg("brain"), py::arg("filename"),
        "Save brain state to file");

    m.def("importBrainState", [](const std::string& filename) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->load(filename);
        return brain;
    }, py::arg("filename"),
        "Load brain state from file");

    // Configuration helpers
    m.def("createResearchConfig", [](bool enableDevelopment, bool enableMemory, bool enablePrediction, bool enableCognition) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("brain.development.enabled", enableDevelopment);
        config->set("brain.memory.enable", enableMemory);
        config->set("brain.prediction.enable", enablePrediction);
        config->set("brain.cognition.enable", enableCognition);
        return config;
    }, py::arg("enableDevelopment") = true, py::arg("enableMemory") = true, py::arg("enablePrediction") = true, py::arg("enableCognition") = true,
        "Create a research-oriented configuration");

    // Utility functions
    m.def("format_brain_stats", [](std::shared_ptr<Brain> brain) {
        std::stringstream ss;
        ss << "Brain Statistics:" << std::endl;
        ss << "  Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
        ss << "  Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        ss << "  Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        ss << "  Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        ss << "  Average Firing Rate: " << brain->getAverageFiringRate() << std::endl;
        ss << "  Developmental Stage: " << brain->getDevelopmentalStage() << std::endl;
        return ss.str();
    }, py::arg("brain"),
        "Format brain statistics as string");

    m.def("get_neuron_type_name", [](NeuronType type) {
        switch (type) {
            case NeuronType::Excitatory: return "Excitatory";
            case NeuronType::Inhibitory: return "Inhibitory";
            case NeuronType::Modulatory: return "Modulatory";
            case NeuronType::Sensory: return "Sensory";
            case NeuronType::Motor: return "Motor";
            case NeuronType::Internal: return "Internal";
            default: return "Unknown";
        }
    }, py::arg("type"),
        "Get string name for neuron type");

    m.def("set_random_seed", [](unsigned int seed) {
        nlm::Random::setGlobalSeed(seed);
    }, py::arg("seed"),
        "Set global random seed for reproducibility");

} // namespace nlm
