#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

// Core NLM headers
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

// Memory systems
#include "../src/memory/Memory.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/NeuralAssociativeMemory.hpp"

// Development system
#include "../src/development/DevelopmentSystem.hpp"

// Prediction and cognition systems
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/prediction/NeuralPrediction.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"

// Neuromodulation systems
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides comprehensive classes for brain simulation, memory systems,
        neuromodulation, development, prediction, and cognition.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ===== BASIC TYPES =====
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

    // ===== ENUMS =====
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
        });

    // ===== SENSORY SYSTEMS =====
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

    // ===== ACTION AND MOTOR SYSTEMS =====
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

    // ===== WORLD SYSTEM =====
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

    // ===== BRAIN CORE =====
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

    // ===== AGENT BRAIN =====
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

    // ===== MEMORY SYSTEMS ==========

    // Basic memory systems (placeholder implementations)
    py::class_<WorkingMemory>(m, "WorkingMemory", R"pbdoc(Working memory: temporary active storage of information)pbdoc")
        .def(py::init<>())
        .def("store", &WorkingMemory::store, py::arg("neuron"), py::arg("value"),
             "Store item in working memory")
        .def("retrieve", &WorkingMemory::retrieve, py::arg("neuron"),
             "Retrieve item from working memory")
        .def("contains", &WorkingMemory::contains, py::arg("neuron"),
             "Check if item exists in working memory")
        .def("clear", &WorkingMemory::clear, "Clear all working memory")
        .def("getCapacity", &WorkingMemory::getCapacity, "Get working memory capacity")
        .def("getCurrentSize", &WorkingMemory::getCurrentSize, "Get current size of working memory")
        .def("decay", &WorkingMemory::decay, py::arg("decayRate"),
             "Decay all items with given decay rate");

    py::class_<EpisodicMemoryItem>(m, "EpisodicMemoryItem", R"pbdoc(Item in episodic memory)pbdoc")
        .def(py::init<>())
        .def_readwrite("timestamp", &EpisodicMemoryItem::timestamp)
        .def("addNeuron", [](EpisodicMemoryItem& item, NeuronId neuron, float value) {
            item.neurons.push_back(neuron);
            item.values.push_back(value);
        }, py::arg("neuron"), py::arg("value"), "Add neuron activation to episode")
        .def("getNeuronValue", [](const EpisodicMemoryItem& item, size_t index) {
            if (index < item.values.size()) return item.values[index];
            return 0.0f;
        }, py::arg("index"), "Get value of neuron at index")
        .def("getNeuronCount", [](const EpisodicMemoryItem& item) {
            return item.neurons.size();
        }, "Get number of neurons in episode");

    py::class_<EpisodicMemory>(m, "EpisodicMemory", R"pbdoc(Episodic memory: storage of events and experiences)pbdoc")
        .def(py::init<>())
        .def("storeEpisode", &EpisodicMemory::storeEpisode, py::arg("episode"),
             "Store an episode in memory")
        .def("getEpisodeCount", &EpisodicMemory::getEpisodeCount,
             "Get number of episodes stored")
        .def("getRecentEpisodes", &EpisodicMemory::getRecentEpisodes, py::arg("count"),
             "Get recent episodes")
        .def("clear", &EpisodicMemory::clear, "Clear all episodes")
        .def("consolidate", &EpisodicMemory::consolidate, py::arg("relevanceThreshold"),
             "Consolidate episodes based on relevance");

    py::class_<SemanticMemory>(m, "SemanticMemory", R"pbdoc(Semantic memory: gradually acquired knowledge)pbdoc")
        .def(py::init<>())
        .def("storeFact", &SemanticMemory::storeFact, py::arg("key"), py::arg("value"),
             "Store a fact in semantic memory")
        .def("retrieveFact", &SemanticMemory::retrieveFact, py::arg("key"),
             "Retrieve a fact from semantic memory")
        .def("hasFact", &SemanticMemory::hasFact, py::arg("key"),
             "Check if fact exists in semantic memory")
        .def("getAllFacts", &SemanticMemory::getAllFacts,
             "Get all facts stored in semantic memory")
        .def("clear", &SemanticMemory::clear, "Clear all semantic memory");

    py::class_<Skill>(m, "Skill", R"pbdoc(Learned skill or capability)pbdoc")
        .def(py::init<>())
        .def_readwrite("name", &Skill::name)
        .def_readwrite("proficiency", &Skill::proficiency)
        .def("addNeuralPattern", [](Skill& skill, NeuronId neuron) {
            skill.neuralPattern.push_back(neuron);
        }, py::arg("neuron"), "Add neuron to skill's neural pattern");

    py::class_<ProceduralMemory>(m, "ProceduralMemory", R"pbdoc(Procedural memory: learned skills and habits)pbdoc")
        .def(py::init<>())
        .def("learnSkill", &ProceduralMemory::learnSkill,
             py::arg("name"), py::arg("pattern"), "Learn a new skill")
        .def("getSkill", &ProceduralMemory::getSkill, py::arg("name"),
             "Get a skill by name")
        .def("getAllSkills", &ProceduralMemory::getAllSkills,
             "Get all learned skills")
        .def("updateProficiency", &ProceduralMemory::updateProficiency,
             py::arg("name"), py::arg("delta"), "Update skill proficiency")
        .def("clear", &ProceduralMemory::clear, "Clear all skills");

    py::class_<AssociativeMemory>(m, "AssociativeMemory", R"pbdoc(Associative memory: relationships between representations)pbdoc")
        .def(py::init<>())
        .def("associate", &AssociativeMemory::associate,
             py::arg("a"), py::arg("b"), py::arg("strength"), "Create association between two neurons")
        .def("getAssociations", &AssociativeMemory::getAssociations, py::arg("neuron"),
             "Get all associated neurons")
        .def("getAssociationStrength", &AssociativeMemory::getAssociationStrength,
             py::arg("a"), py::arg("b"), "Get association strength")
        .def("updateAssociation", &AssociativeMemory::updateAssociation,
             py::arg("a"), py::arg("b"), py::arg("delta"), "Update association strength")
        .def("clear", &AssociativeMemory::clear, "Clear all associations");

    // Neural memory systems
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Neural working memory: working memory using neural dynamics)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("store", &NeuralWorkingMemory::store, py::arg("pattern"), py::arg("strength") = 1.0f,
             "Store pattern in working memory")
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron,
             py::arg("neuron"), py::arg("activation"), "Store activation to specific neuron")
        .def("retrieve", &NeuralWorkingMemory::retrieve,
             "Retrieve current working memory content")
        .def("contains", &NeuralWorkingMemory::contains, py::arg("neuron"),
             "Check if neuron is in working memory")
        .def("getNeuronActivation", &NeuralWorkingMemory::getNeuronActivation,
             py::arg("neuron"), "Get neuron activation in working memory")
        .def("update", &NeuralWorkingMemory::update, py::arg("dt"), "Update working memory")
        .def("clear", &NeuralWorkingMemory::clear, "Clear working memory")
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces,
             "Get number of active memory traces")
        .def("getCapacity", &NeuralWorkingMemory::getCapacity,
             "Get working memory capacity")
        .def("setCapacity", &NeuralWorkingMemory::setCapacity, py::arg("cap"),
             "Set working memory capacity")
        .def("getDecayRate", &NeuralWorkingMemory::getDecayRate,
             "Get decay rate")
        .def("setDecayRate", &NeuralWorkingMemory::setDecayRate, py::arg("rate"),
             "Set decay rate")
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons,
             "Get neurons in working memory")
        .def("strengthenMemory", &NeuralWorkingMemory::strengthenMemory,
             py::arg("factor"), "Strengthen memory representation")
        .def("runCompetition", &NeuralWorkingMemory::runCompetition,
             "Run competition between memory traces")
        .def("isWinning", &NeuralWorkingMemory::isWinning, py::arg("neuron"),
             "Check if neuron is winning")
        .def("getMemoryActivity", &NeuralWorkingMemory::getMemoryActivity,
             "Get overall memory activity level");

    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Neural episodic memory: episodic memory using neural patterns)pbdoc")
        .def(py::init<>())
        .def("storeEpisode", [](NeuralEpisodicMemory& self, const std::vector<float>& pattern,
                               const std::string& metadata = "") {
            self.storeEpisode(pattern, metadata);
        }, py::arg("pattern"), py::arg("metadata") = "",
             "Store episode as neural pattern")
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount,
             "Get number of episodes stored")
        .def("getRecentEpisodes", &NeuralEpisodicMemory::getRecentEpisodes, py::arg("count"),
             "Get recent episodes")
        .def("getEpisodePattern", &NeuralEpisodicMemory::getEpisodePattern, py::arg("index"),
             "Get neural pattern of episode")
        .def("getEpisodeMetadata", &NeuralEpisodicMemory::getEpisodeMetadata, py::arg("index"),
             "Get metadata of episode")
        .def("clear", &NeuralEpisodicMemory::clear, "Clear all episodes")
        .def("consolidate", &NeuralEpisodicMemory::consolidate, py::arg("relevanceThreshold"),
             "Consolidate episodes based on relevance")
        .def("replayEpisode", &NeuralEpisodicMemory::replayEpisode, py::arg("index"),
             "Replay episode to reinforce memory");

    py::class_<NeuralAssociativeMemory>(m, "NeuralAssociativeMemory", R"pbdoc(Neural associative memory: associative memory using neural connections)pbdoc")
        .def(py::init<>())
        .def("associate", &NeuralAssociativeMemory::associate,
             py::arg("a"), py::arg("b"), py::arg("strength"), "Create association between neurons")
        .def("getAssociations", &NeuralAssociativeMemory::getAssociations, py::arg("neuron"),
             "Get all associated neurons")
        .def("getAssociationStrength", &NeuralAssociativeMemory::getAssociationStrength,
             py::arg("a"), py::arg("b"), "Get association strength")
        .def("updateAssociation", &NeuralAssociativeMemory::updateAssociation,
             py::arg("a"), py::arg("b"), py::arg("delta"), "Update association strength")
        .def("clear", &NeuralAssociativeMemory::clear, "Clear all associations")
        .def("getHebbianConnections", &NeuralAssociativeMemory::getHebbianConnections,
             "Get all Hebbian connections")
        .def("getSTDPConnections", &NeuralAssociativeMemory::getSTDPConnections,
             "Get all STDP connections")
        .def("getStructuralConnections", &NeuralAssociativeMemory::getStructuralConnections,
             "Get all structural connections");

    // ===== DEVELOPMENT SYSTEM =====

    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Development system: orchestrates neural development)pbdoc")
        .def(py::init<>())
        .def("getStage", &DevelopmentSystem::getStage, "Get current developmental stage")
        .def("setStage", &DevelopmentSystem::setStage, py::arg("stage"), "Set developmental stage")
        .def("advanceStage", &DevelopmentSystem::advanceStage, "Advance developmental stage")
        .def("getStageName", &DevelopmentSystem::getStageName, "Get stage name")
        .def("update", static_cast<void (DevelopmentSystem::*)(Brain* brain, SimulationStep currentStep)>(&DevelopmentSystem::update),
             py::arg("brain"), py::arg("currentStep"), "Update development (simple version)")
        .def("update", static_cast<void (DevelopmentSystem::*)(Brain* brain, RandomGenerator& rng, TimestepDuration dt)>(&DevelopmentSystem::update),
             py::arg("brain"), py::arg("rng"), py::arg("dt"), "Update development with full parameters")
        .def("getPlasticityModifier", &DevelopmentSystem::getPlasticityModifier, "Get plasticity modifier for current stage")
        .def("isCriticalPeriod", &DevelopmentSystem::isCriticalPeriod, "Get critical period info")
        .def("getCriticalPeriodProgress", &DevelopmentSystem::getCriticalPeriodProgress, "Get critical period progress")
        .def("getDevelopmentalAge", &DevelopmentSystem::getDevelopmentalAge, "Get developmental age (in simulation time)")
        .def("setDevelopmentalAge", &DevelopmentSystem::setDevelopmentalAge, py::arg("age"), "Set developmental age");

    // ===== PREDICTION AND COGNITION SYSTEMS =====

    // Neural prediction system
    py::class_<NeuralPrediction>(m, "NeuralPrediction", R"pbdoc(Neural prediction for sensory prediction and error computation)pbdoc")
        .def(py::init<>())
        .def("predict", &NeuralPrediction::predict, py::arg("input"), "Generate prediction from input")
        .def("getPredictionError", &NeuralPrediction::getPredictionError, "Get prediction error")
        .def("getConfidence", &NeuralPrediction::getConfidence, "Get prediction confidence")
        .def("update", &NeuralPrediction::update, py::arg("dt"), "Update prediction system")
        .def("reset", &NeuralPrediction::reset, "Reset prediction system");

    // Neural planner for action planning
    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planner for action planning)pbdoc")
        .def(py::init<>())
        .def("planAction", &NeuralPlanner::planAction, py::arg("currentState"), "Plan action based on current state")
        .def("evaluateAction", &NeuralPlanner::evaluateAction, py::arg("action"), py::arg("context"), "Evaluate action in given context")
        .def("getActionSequence", &NeuralPlanner::getActionSequence, "Get planned action sequence")
        .def("update", &NeuralPlanner::update, py::arg("dt"), "Update planner state")
        .def("reset", &NeuralPlanner::reset, "Reset planner");

    // Concept formation for pattern discovery
    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation for pattern discovery)pbdoc")
        .def(py::init<>())
        .def("learnConcept", &ConceptFormation::learnConcept, py::arg("pattern"), "Learn concept from pattern")
        .def("getConcepts", &ConceptFormation::getConcepts, "Get learned concepts")
        .def("recognizePattern", &ConceptFormation::recognizePattern, py::arg("pattern"), "Recognize pattern using learned concepts")
        .def("update", &ConceptFormation::update, py::arg("dt"), "Update concept formation")
        .def("reset", &ConceptFormation::reset, "Reset concept formation");

    // Prediction system
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Prediction system for sensory prediction and error computation)pbdoc")
        .def(py::init<>())
        .def("predict", &PredictionSystem::predict, py::arg("sensoryInput"), "Generate prediction from sensory input")
        .def("getPredictionError", &PredictionSystem::getPredictionError, "Get prediction error")
        .def("getPredictionConfidence", &PredictionSystem::getPredictionConfidence, "Get prediction confidence")
        .def("update", &PredictionSystem::update, py::arg("dt"), "Update prediction system")
        .def("reset", &PredictionSystem::reset, "Reset prediction system")
        .def("getNeuralPrediction", &PredictionSystem::getNeuralPrediction, "Get neural prediction component")
        .def("getPredictionErrorSignal", &PredictionSystem::getPredictionErrorSignal, "Get prediction error signal");
    py::class_<Neuromodulator>(m, "Neuromodulator", py::module_local(),
        R"pbdoc(Abstract base class for neuromodulatory signals)pbdoc")
        .def("getName", &Neuromodulator::getName, "Get modulator name")
        .def("getLevel", &Neuromodulator::getLevel, "Get current concentration/level")
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"), "Set current level")
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor,
             "Get plasticity factor for this neuromodulator")
        .def("update", &Neuromodulator::update, py::arg("dt"), "Update neuromodulator state");

    // Dopamine - reward and reinforcement learning signal
    py::class_<Dopamine, Neuromodulator>(m, "Dopamine", R"pbdoc(Dopamine: reward and reinforcement learning signal)pbdoc")
        .def(py::init<>())
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"), "Signal reward")
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError,
             py::arg("error"), "Signal reward prediction error")
        .def("getLearningRate", &Dopamine::getLearningRate, "Get current learning rate")
        .def("getPredictionError", &Dopamine::getPredictionError, "Get prediction error signal");

    // Acetylcholine - attention and memory consolidation
    py::class_<Acetylcholine, Neuromodulator>(m, "Acetylcholine", R"pbdoc(Acetylcholine: attention and memory consolidation)pbdoc")
        .def(py::init<>())
        .def("getAttentionLevel", &Acetylcholine::getAttentionLevel, "Get attention level")
        .def("enhanceMemory", &Acetylcholine::enhanceMemory, "Enhance memory consolidation")
        .def("getWorkingMemoryBoost", &Acetylcholine::getWorkingMemoryBoost,
             "Get working memory boost factor");

    // Norepinephrine - arousal and vigilance
    py::class_<Norepinephrine, Neuromodulator>(m, "Norepinephrine", R"pbdoc(Norepinephrine: arousal and vigilance)pbdoc")
        .def(py::init<>())
        .def("getAlertness", &Norepinephrine::getAlertness, "Get alertness level")
        .def("getStressLevel", &Norepinephrine::getStressLevel, "Get stress level")
        .def("enhanceSensoryProcessing", &Norepinephrine::enhanceSensoryProcessing,
             "Enhance sensory processing");

    // Serotonin - mood, impulsivity, and social behavior
    py::class_<Serotonin, Neuromodulator>(m, "Serotonin", R"pbdoc(Serotonin: mood, impulsivity, and social behavior)pbdoc")
        .def(py::init<>())
        .def("getMood", &Serotonin::getMood, "Get mood level")
        .def("getImpulseControl", &Serotonin::getImpulseControl, "Get impulse control level")
        .def("enhanceSocialBehavior", &Serotonin::enhanceSocialBehavior,
             "Enhance social behavior processing");

    // Novelty detection system
    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty: novelty detection and surprise signaling)pbdoc")
        .def(py::init<>())
        .def("detectNovelty", &Novelty::detectNovelty, py::arg("input"), "Detect novelty in input")
        .def("getSurpriseValue", &Novelty::getSurpriseValue, "Get surprise value")
        .def("getCuriosityLevel", &Novelty::getCuriosityLevel, "Get curiosity level")
        .def("reset", &Novelty::reset, "Reset novelty detection");

    // Prediction error signal
    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error signal for learning)pbdoc")
        .def(py::init<>())
        .def("computeError", &PredictionError::computeError, py::arg("prediction"), py::arg("actual"),
             "Compute prediction error")
        .def("getErrorMagnitude", &PredictionError::getErrorMagnitude, "Get error magnitude")
        .def("getErrorSign", &PredictionError::getErrorSign, "Get error sign (positive/negative)")
        .def("getLearningBonus", &PredictionError::getLearningBonus, "Get learning bonus factor");

    // Curiosity system
    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity: exploration motivation)pbdoc")
        .def(py::init<>())
        .def("getExplorationDrive", &Curiosity::getExplorationDrive, "Get exploration drive level")
        .def("getNoveltySeeker", &Curiosity::getNoveltySeeker, "Get novelty seeking level")
        .def("getUncertainty", &Curiosity::getUncertainty, "Get uncertainty level")
        .def("updateExploration", &Curiosity::updateExploration, py::arg("dt"),
             "Update exploration behavior")
        .def("increaseExploration", &Curiosity::increaseExploration, "Increase exploration drive")
        .def("decreaseExploration", &Curiosity::decreaseExploration, "Decrease exploration drive");

    // ===== BATCH OPERATIONS AND PYTHONIC CONVENIENCE =====

    // Brain batch operations
    m.def("batchInjectCurrent", [](Brain& brain, const std::vector<std::pair<NeuronId, MembranePotential>>& injections) {
        for (const auto& injection : injections) {
            brain.injectCurrent(injection.first, injection.second);
        }
    }, py::arg("injections"), "Batch inject current into multiple neurons");

    m.def("batchInjectByType", [](Brain& brain, NeuronType type, MembranePotential current) {
        brain.injectCurrentToNeurons(type, current);
    }, py::arg("type"), py::arg("current"), "Batch inject current to all neurons of a specific type");

    // Statistics and analysis
    m.def("computeBrainStatistics", [](const Brain& brain) {
        std::unordered_map<std::string, double> stats;
        stats["total_neurons"] = brain.getTotalNeuronCount();
        stats["total_synapses"] = brain.getTotalSynapseCount();
        stats["active_neurons"] = brain.getActiveNeuronCount();
        stats["firing_neurons"] = brain.getFiringNeuronCount();
        stats["average_firing_rate"] = brain.getAverageFiringRate();
        stats["excitation_inhibition_ratio"] = brain.getExcitationInhibitionRatio();
        stats["total_spikes"] = brain.getTotalSpikeCount();
        stats["developmental_stage"] = static_cast<int>(brain.getDevelopmentalStage());
        return stats;
    }, "Compute comprehensive brain statistics");

    // Memory system batch operations
    m.def("batchStoreMemories", [](WorkingMemory& memory, const std::vector<std::pair<NeuronId, float>>& items) {
        for (const auto& item : items) {
            memory.store(item.first, item.second);
        }
    }, py::arg("items"), "Batch store multiple items in working memory");

    m.def("batchRetrieveMemories", [](const WorkingMemory& memory, const std::vector<NeuronId>& neurons) {
        std::vector<float> values;
        for (const auto& neuron : neurons) {
            values.push_back(memory.retrieve(neuron));
        }
        return values;
    }, py::arg("neurons"), "Batch retrieve multiple items from working memory");

    // World batch operations
    m.def("batchUpdateWorld", [](SimpleWorld& world, const std::vector<std::pair<ActionType, Timestamp>>& actions) {
        for (const auto& action : actions) {
            // This is a simplified example - in practice, you'd need more sophisticated action handling
            // For now, just demonstrate batch processing capability
            world.update(1.0f);
        }
    }, py::arg("actions"), "Batch process multiple world updates");

    // ===== HIGH-LEVEL HELPER FUNCTIONS =====

    // Convenience factory functions
    m.def("createAdvancedConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 1000);
        config->set("brain.simulation_speed", 1.0f);
        config->set("memory.working_memory_capacity", 100);
        config->set("memory.episodic_memory_capacity", 1000);
        config->set("neuromodulation.dopamine_level", 0.5f);
        config->set("neuromodulation.curiosity_level", 0.3f);
        return config;
    }, "Create an advanced configuration with sensible defaults");

    m.def("createCompleteSystem", [](std::shared_ptr<Config> config) -> std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>> {
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        world->configure(800, 600, 80, 60);
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(world);
        return std::make_tuple(brain, world, agentBrain);
    }, py::arg("config"), "Create a complete NLM system with brain, world, and agent brain");

    // ===== ERROR HANDLING AND VALIDATION =====

    // Error handling utilities
    m.def("safeBrainStep", [](Brain& brain, SimulationStep currentStep, Timestamp currentTime) {
        try {
            brain.step(currentStep, currentTime);
            return true;
        } catch (const std::exception& e) {
            py::print("Error during brain step: {}", e.what());
            return false;
        }
    }, py::arg("brain"), py::arg("currentStep"), py::arg("currentTime"),
       "Safely perform a brain step with error handling");

    m.def("validateBrainState", [](const Brain& brain) {
        std::vector<std::string> errors;
        if (brain.getTotalNeuronCount() == 0) {
            errors.push_back("Brain has no neurons");
        }
        if (brain.getTotalSynapseCount() == 0) {
            errors.push_back("Brain has no synapses");
        }
        if (brain.getActiveNeuronCount() > brain.getTotalNeuronCount()) {
            errors.push_back("Active neuron count exceeds total neuron count");
        }
        return errors;
    }, "Validate brain state and return list of errors");

    m.def("validateMemoryState", [](const WorkingMemory& memory) {
        std::vector<std::string> errors;
        if (memory.getCurrentSize() > memory.getCapacity()) {
            errors.push_back("Memory usage exceeds capacity");
        }
        return errors;
    }, "Validate memory state and return list of errors");

    // ===== ADVANCED FUNCTIONALITY =====

    // Neural network analysis
    m.def("analyzeNeuralConnectivity", [](const Brain& brain) {
        std::unordered_map<std::string, double> connectivity;
        connectivity["total_synapses"] = brain.getTotalSynapseCount();
        connectivity["excitatory_synapses"] = 0.0f; // Would need actual calculation
        connectivity["inhibitory_synapses"] = 0.0f; // Would need actual calculation
        connectivity["average_connectivity"] = 0.0f; // Would need actual calculation
        return connectivity;
    }, "Analyze neural network connectivity");

    // Learning progress tracking
    m.def("trackLearningProgress", [](Brain& brain, const std::string& episodeName) {
        auto stats = m.def("computeBrainStatistics", brain);
        stats["episode"] = episodeName;
        stats["timestamp"] = brain.getConfig()->getOr("simulation.time", 0.0);
        return stats;
    }, py::arg("brain"), py::arg("episodeName"), "Track learning progress for an episode");

    // ===== DEBUGGING AND MONITORING =====

    // Debug output
    m.def("debugBrainState", [](Brain& brain) {
        py::print("Brain Debug State:");
        py::print("  Total neurons: {}", brain.getTotalNeuronCount());
        py::print("  Total synapses: {}", brain.getTotalSynapseCount());
        py::print("  Active neurons: {}", brain.getActiveNeuronCount());
        py::print("  Firing neurons: {}", brain.getFiringNeuronCount());
        py::print("  Average firing rate: {}", brain.getAverageFiringRate());
        py::print("  Developmental stage: {}", static_cast<int>(brain.getDevelopmentalStage()));
    }, "Debug brain state with detailed output");

    // Performance monitoring
    m.def("getPerformanceMetrics", []() {
        std::unordered_map<std::string, double> metrics;
        metrics["version"] = 1.0;
        metrics["build_info"] = "NLM Python Bindings v1.0";
        metrics["features"] = 50.0; // Approximate feature count
        return metrics;
    }, "Get performance metrics and build information");

    // ===== EXPERIMENT UTILITIES =====

    // Experiment control
    m.def("createExperimentSetup", [](std::shared_ptr<Config> config, const std::string& experimentName) {
        auto experimentConfig = std::make_shared<Config>(*config);
        experimentConfig->set("experiment.name", experimentName);
        experimentConfig->set("experiment.start_time", 0.0);
        experimentConfig->set("experiment.max_steps", 10000);
        return experimentConfig;
    }, py::arg("config"), py::arg("experimentName"), "Create experiment configuration");

    m.def("runExperimentStep", [](Brain& brain, SimpleWorld& world, WorkingMemory& memory, float timestep) {
        try {
            // Reset memory if needed
            memory.clear();
            
            // Update world
            world.update(timestep);
            
            // Process brain
            brain.step(static_cast<SimulationStep>(world.getSimulationTime()));
            
            // Get sensory input and process
            auto percept = world.getSensoryPercept();
            if (percept) {
                brain.receiveSensoryInput(*percept);
            }
            
            return true;
        } catch (const std::exception& e) {
            py::print("Error during experiment step: {}", e.what());
            return false;
        }
    }, py::arg("brain"), py::arg("world"), py::arg("memory"), py::arg("timestep"),
       "Run a single experiment step with error handling");

    // ===== CONSTANTS =====
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm