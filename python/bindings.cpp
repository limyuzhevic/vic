#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <deque>

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
#include "../src/memory/Memory.hpp"
#include "src/memory/NeuralWorkingMemory.hpp"
#include "src/memory/NeuralEpisodicMemory.hpp"
#include "src/cognition/NeuralPlanner.hpp"
#include "src/cognition/ConceptFormation.hpp"
#include "src/prediction/NeuralPrediction.hpp"
#include "src/prediction/PredictionSystem.hpp"
#include "src/neuromodulation/Curiosity.hpp"
#include "src/neuromodulation/Reward.hpp"
#include "src/neuromodulation/PredictionError.hpp"
#include "src/neuromodulation/Neuromodulator.hpp"

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

    // Memory Systems
    py::class_<WorkingMemory>(m, "WorkingMemory", R"pbdoc(Working memory: temporary active storage of information)pbdoc")
        .def(py::init<>())
        .def("store", &WorkingMemory::store,
             py::arg("neuron"), py::arg("value"),
             "Store item in working memory")
        .def("retrieve", &WorkingMemory::retrieve,
             py::arg("neuron"),
             "Retrieve item from working memory")
        .def("contains", &WorkingMemory::contains,
             py::arg("neuron"),
             "Check if item exists in working memory")
        .def("clear", &WorkingMemory::clear,
             "Clear all working memory")
        .def("getCapacity", &WorkingMemory::getCapacity,
             "Get working memory capacity")
        .def("getCurrentSize", &WorkingMemory::getCurrentSize,
             "Get current working memory size")
        .def("decay", &WorkingMemory::decay,
             py::arg("decayRate"),
             "Decay all items in working memory");

    py::class_<EpisodicMemory>(m, "EpisodicMemory", R"pbdoc(Episodic memory: storage of events and experiences)pbdoc")
        .def(py::init<>())
        .def("storeEpisode", &EpisodicMemory::storeEpisode,
             py::arg("episode"),
             "Store episode in episodic memory")
        .def("retrieveEpisode", &EpisodicMemory::retrieveEpisode,
             py::arg("index"),
             "Retrieve episode from episodic memory")
        .def("getEpisodeCount", &EpisodicMemory::getEpisodeCount,
             "Get number of episodes in episodic memory")
        .def("getRecentEpisodes", &EpisodicMemory::getRecentEpisodes,
             py::arg("count"),
             "Get recent episodes from episodic memory")
        .def("clear", &EpisodicMemory::clear,
             "Clear all episodic memory")
        .def("consolidate", &EpisodicMemory::consolidate,
             py::arg("relevanceThreshold"),
             "Consolidate episodes to long-term storage");

    py::class_<SemanticMemory>(m, "SemanticMemory", R"pbdoc(Semantic memory: gradually acquired knowledge)pbdoc")
        .def(py::init<>())
        .def("storeFact", &SemanticMemory::storeFact,
             py::arg("key"), py::arg("value"),
             "Store fact in semantic memory")
        .def("retrieveFact", &SemanticMemory::retrieveFact,
             py::arg("key"),
             "Retrieve fact from semantic memory")
        .def("hasFact", &SemanticMemory::hasFact,
             py::arg("key"),
             "Check if fact exists in semantic memory")
        .def("getAllFacts", &SemanticMemory::getAllFacts,
             "Get all facts in semantic memory")
        .def("clear", &SemanticMemory::clear,
             "Clear all semantic memory");

    py::class_<Skill>(m, "Skill", R"pbdoc(Learned skill in procedural memory)pbdoc")
        .def(py::init<>())
        .def_readwrite("name", &Skill::name,
                       "Skill name")
        .def_readwrite("neuralPattern", &Skill::neuralPattern,
                       "Neural pattern representing the skill")
        .def_readwrite("proficiency", &Skill::proficiency,
                       "Skill proficiency level (0-1)");

    py::class_<ProceduralMemory>(m, "ProceduralMemory", R"pbdoc(Procedural memory: learned skills and habits)pbdoc")
        .def(py::init<>())
        .def("learnSkill", &ProceduralMemory::learnSkill,
             py::arg("name"), py::arg("pattern"),
             "Learn new skill")
        .def("getSkill", &ProceduralMemory::getSkill,
             py::arg("name"),
             "Get skill from procedural memory")
        .def("getAllSkills", &ProceduralMemory::getAllSkills,
             "Get all learned skills")
        .def("updateProficiency", &ProceduralMemory::updateProficiency,
             py::arg("name"), py::arg("delta"),
             "Update skill proficiency")
        .def("clear", &ProceduralMemory::clear,
             "Clear all skills from procedural memory");

    py::class_<AssociativeMemory>(m, "AssociativeMemory", R"pbdoc(Associative memory: relationships between representations)pbdoc")
        .def(py::init<>())
        .def("associate", &AssociativeMemory::associate,
             py::arg("a"), py::arg("b"), py::arg("strength"),
             "Create association between two neurons")
        .def("getAssociations", &AssociativeMemory::getAssociations,
             py::arg("neuron"),
             "Get associated neurons")
        .def("getAssociationStrength", &AssociativeMemory::getAssociationStrength,
             py::arg("a"), py::arg("b"),
             "Get association strength between neurons")
        .def("updateAssociation", &AssociativeMemory::updateAssociation,
             py::arg("a"), py::arg("b"), py::arg("delta"),
             "Update association strength")
        .def("clear", &AssociativeMemory::clear,
             "Clear all associations");

    py::class_<EpisodicMemoryItem>(m, "EpisodicMemoryItem", R"pbdoc(Episodic memory item)pbdoc")
        .def(py::init<>())
        .def_readwrite("timestamp", &EpisodicMemoryItem::timestamp,
                       "Timestamp of the episode")
        .def_readwrite("neurons", &EpisodicMemoryItem::neurons,
                       "Neurons involved in the episode")
        .def_readwrite("values", &EpisodicMemoryItem::values,
                       "Values associated with neurons")
        .def_readwrite("metadata", &EpisodicMemoryItem::metadata,
                       "Additional metadata for the episode");

    // Neural memory system (more advanced)
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Neural working memory using persistent activity)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize,
             py::arg("brain"),
             "Initialize with brain reference")
        .def("store", &NeuralWorkingMemory::store,
             py::arg("pattern"), py::arg("strength") = 1.0f,
             "Store information in neural working memory")
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron,
             py::arg("neuron"), py::arg("activation"),
             "Store activation to specific neuron")
        .def("retrieve", &NeuralWorkingMemory::retrieve,
             "Retrieve current working memory content")
        .def("contains", &NeuralWorkingMemory::contains,
             py::arg("neuron"),
             "Check if neuron is in working memory")
        .def("getNeuronActivation", &NeuralWorkingMemory::getNeuronActivation,
             py::arg("neuron"),
             "Get activation level of neuron")
        .def("update", &NeuralWorkingMemory::update,
             py::arg("dt"),
             "Update working memory dynamics")
        .def("clear", &NeuralWorkingMemory::clear,
             "Clear working memory")
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces,
             "Get number of active memory traces")
        .def("getCapacity", &NeuralWorkingMemory::getCapacity,
             "Get working memory capacity")
        .def("setCapacity", &NeuralWorkingMemory::setCapacity,
             py::arg("cap"),
             "Set working memory capacity")
        .def("getDecayRate", &NeuralWorkingMemory::getDecayRate,
             "Get decay rate")
        .def("setDecayRate", &NeuralWorkingMemory::setDecayRate,
             py::arg("rate"),
             "Set decay rate")
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons,
             "Get neurons currently in working memory")
        .def("strengthenMemory", &NeuralWorkingMemory::strengthenMemory,
             py::arg("factor"),
             "Strengthen working memory representation")
        .def("runCompetition", &NeuralWorkingMemory::runCompetition,
             "Run competition between memory traces")
        .def("isWinning", &NeuralWorkingMemory::isWinning,
             py::arg("neuron"),
             "Check if neuron is winning")
        .def("getMemoryActivity", &NeuralWorkingMemory::getMemoryActivity,
             "Get overall memory activity level");

    py::class_<AttentionalSelection>(m, "AttentionalSelection", R"pbdoc(Neural attention through competition)pbdoc")
        .def(py::init<>())
        .def("initialize", &AttentionalSelection::initialize,
             py::arg("brain"),
             "Initialize with brain reference")
        .def("processCompetition", &AttentionalSelection::processCompetition,
             py::arg("competitors"), py::arg("globalInhibition") = 0.5f,
             "Process competing sensory inputs and select winners")
        .def("focusOnRegion", &AttentionalSelection::focusOnRegion,
             py::arg("region"),
             "Apply attention to a specific region")
        .def("releaseAttention", &AttentionalSelection::releaseAttention,
             "Release attention (allow all regions equal processing)")
        .def("getAttendedRegions", &AttentionalSelection::getAttendedRegions,
             "Get current attention focus")
        .def("setInhibitionStrength", &AttentionalSelection::setInhibitionStrength,
             py::arg("strength"),
             "Set inhibition strength")
        .def("setExcitationStrength", &AttentionalSelection::setExcitationStrength,
             py::arg("strength"),
             "Set excitation strength")
        .def("setCompetitionThreshold", &AttentionalSelection::setCompetitionThreshold,
             py::arg("threshold"),
             "Set competition threshold")
        .def("getInhibitionFor", &AttentionalSelection::getInhibitionFor,
             py::arg("neuron"),
             "Get inhibition level for a neuron")
        .def("getExcitationFor", &AttentionalSelection::getExcitationFor,
             py::arg("neuron"),
             "Get excitation level for a neuron")
        .def("update", &AttentionalSelection::update,
             py::arg("dt"),
             "Update attention dynamics")
        .def("isAttended", &AttentionalSelection::isAttended,
             py::arg("neuron"),
             "Check if neuron is currently attended")
        .def("getWinners", &AttentionalSelection::getWinners,
             "Get attention winners")
        .def("applyTopDownBias", &AttentionalSelection::applyTopDownBias,
             py::arg("neuron"), py::arg("biasStrength"),
             "Apply top-down attention bias")
        .def("applyBottomUpSalience", &AttentionalSelection::applyBottomUpSalience,
             py::arg("neuron"), py::arg("salienceStrength"),
             "Apply bottom-up attention salience")
        .def("reset", &AttentionalSelection::reset,
             "Reset attention");

    // Prediction Systems
    py::class_<NeuralPrediction>(m, "NeuralPrediction", R"pbdoc(Neural prediction using spiking substrate)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPrediction::initialize,
             py::arg("brain"),
             "Initialize with brain reference")
        .def("recordSensoryState", &NeuralPrediction::recordSensoryState,
             py::arg("sensoryState"), py::arg("currentStep"),
             "Record current sensory state for prediction")
        .def("generatePrediction", &NeuralPrediction::generatePrediction,
             py::arg("currentStep"),
             "Generate prediction for next timestep")
        .def("updateWithObservation", &NeuralPrediction::updateWithObservation,
             py::arg("actualState"), py::arg("currentStep"),
             "Update predictions with actual observation")
        .def("predictActionConsequence", &NeuralPrediction::predictActionConsequence,
             py::arg("action"), py::arg("currentState"),
             "Predict consequences of an action")
        .def("getPredictionError", &NeuralPrediction::getPredictionError,
             "Get prediction error (0 = perfect, 1 = total error)")
        .def("getPredictionConfidence", &NeuralPrediction::getPredictionConfidence,
             "Get prediction confidence")
        .def("predictMultipleSteps", &NeuralPrediction::predictMultipleSteps,
             py::arg("currentStep"), py::arg("numSteps"),
             "Multi-step prediction")
        .def("recordAction", &NeuralPrediction::recordAction,
             py::arg("action"), py::arg("step"),
             "Record action for learning")
        .def("getErrorHistory", &NeuralPrediction::getErrorHistory,
             "Get prediction error history")
        .def("clearHistory", &NeuralPrediction::clearHistory,
             "Clear prediction history")
        .def("enableTemporalPrediction", &NeuralPrediction::enableTemporalPrediction,
             py::arg("enable"),
             "Enable/disable temporal prediction")
        .def("enableActionConsequencePrediction", &NeuralPrediction::enableActionConsequencePrediction,
             py::arg("enable"),
             "Enable/disable action consequence prediction")
        .def("setSequenceMemorySize", &NeuralPrediction::setSequenceMemorySize,
             py::arg("size"),
             "Set sequence memory size")
        .def("setPredictionHorizon", &NeuralPrediction::setPredictionHorizon,
             py::arg("steps"),
             "Set prediction horizon")
        .def("getPredictionNeurons", &NeuralPrediction::getPredictionNeurons,
             "Get neurons involved in prediction")
        .def("getSequenceNeurons", &NeuralPrediction::getSequenceNeurons,
             "Get neurons involved in sequences");

    py::class_<ActionConsequencePredictor>(m, "ActionConsequencePredictor", R"pbdoc(Learns action -> consequence mappings)pbdoc")
        .def(py::init<>())
        .def("initialize", &ActionConsequencePredictor::initialize,
             py::arg("brain"),
             "Initialize with brain reference")
        .def("recordExperience", &ActionConsequencePredictor::recordExperience,
             py::arg("action"), py::arg("beforeState"), py::arg("afterState"), py::arg("reward"),
             "Record action consequence experience")
        .def("predictConsequence", &ActionConsequencePredictor::predictConsequence,
             py::arg("action"), py::arg("currentState"),
             "Predict consequence of an action")
        .def("getConsequenceConfidence", &ActionConsequencePredictor::getConsequenceConfidence,
             py::arg("action"), py::arg("currentState"),
             "Get prediction confidence")
        .def("updatePrediction", &ActionConsequencePredictor::updatePrediction,
             py::arg("action"), py::arg("predicted"), py::arg("actual"), py::arg("reward"),
             "Update based on actual outcome")
        .def("getActionQuality", &ActionConsequencePredictor::getActionQuality,
             py::arg("action"),
             "Get learned action quality")
        .def("clear", &ActionConsequencePredictor::clear,
             "Clear learned associations");

    py::class_<PredictionErrorSignal>(m, "PredictionErrorSignal", R"pbdoc(Computes and broadcasts prediction error)pbdoc")
        .def(py::init<>())
        .def("computeError", &PredictionErrorSignal::computeError,
             py::arg("predicted"), py::arg("actual"),
             "Compute error between predicted and actual")
        .def("getErrorComponents", &PredictionErrorSignal::getErrorComponents,
             "Get error components (intensity, spatial, temporal)")
        .def("getModulationSignal", &PredictionErrorSignal::getModulationSignal,
             "Get neuromodulation signal strength (0-1)")
        .def("recordError", &PredictionErrorSignal::recordError,
             py::arg("error"), py::arg("step"),
             "Record error for history")
        .def("getErrorHistory", &PredictionErrorSignal::getErrorHistory,
             "Get error history")
        .def("isSurprising", &PredictionErrorSignal::isSurprising,
             "Check if event was surprising")
        .def("setSurpriseThreshold", &PredictionErrorSignal::setSurpriseThreshold,
             py::arg("t"),
             "Set surprise threshold")
        .def("getSurpriseThreshold", &PredictionErrorSignal::getSurpriseThreshold,
             "Get surprise threshold");

    // Cognitive Systems
    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planner using predictive dynamics)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPlanner::initialize,
             py::arg("brain"),
             "Initialize with brain reference")
        .def("planAction", &NeuralPlanner::planAction,
             py::arg("currentState"), py::arg("targetReward") = 0.5f,
             "Plan next action given current state and goal")
        .def("evaluateSequence", &NeuralPlanner::evaluateSequence,
             py::arg("actions"), py::arg("startState"),
             "Evaluate a potential action sequence")
        .def("getPlanningDepth", &NeuralPlanner::getPlanningDepth,
             "Get number of planning steps")
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth,
             py::arg("depth"),
             "Set planning depth")
        .def("getPlanningConfidence", &NeuralPlanner::getPlanningConfidence,
             "Get planning confidence")
        .def("updatePlanQuality", &NeuralPlanner::updatePlanQuality,
             py::arg("plannedActions"), py::arg("actualActions"), py::arg("actualReward"),
             "Update plans based on actual outcome")
        .def("clearCache", &NeuralPlanner::clearCache,
             "Clear planning cache")
        .def("setActionQuality", &NeuralPlanner::setActionQuality,
             py::arg("action"), py::arg("quality"),
             "Set action quality from experience")
        .def("getCurrentGoal", &NeuralPlanner::getCurrentGoal,
             "Get current planning goal")
        .def("setCurrentGoal", &NeuralPlanner::setCurrentGoal,
             py::arg("goal"),
             "Set current planning goal")
        .def("wasRecentPlanSuccessful", &NeuralPlanner::wasRecentPlanSuccessful,
             "Check if recent plan was successful");

    py::class_<SelfModel>(m, "SelfModel", R"pbdoc(Self-model: internal representation of agent)pbdoc")
        .def(py::init<>())
        .def("initialize", &SelfModel::initialize,
             py::arg("brain"),
             "Initialize with brain reference")
        .def("recordSelfAction", &SelfModel::recordSelfAction,
             py::arg("action"), py::arg("beforeState"), py::arg("afterState"),
             "Record that taking an action caused a specific change")
        .def("predictActionConsequence", &SelfModel::predictActionConsequence,
             py::arg("action"), py::arg("currentState"),
             "Predict sensory consequence of an action")
        .def("getSelfModelConfidence", &SelfModel::getSelfModelConfidence,
             py::arg("action"),
             "Get confidence in self-model for action")
        .def("computeSelfGeneratedLikelihood", &SelfModel::computeSelfGeneratedLikeness,
             py::arg("beforeState"), py::arg("afterState"), py::arg("action"),
             "Check if change likely caused by self")
        .def("getPreferredAction", &SelfModel::getPreferredAction,
             py::arg("state"),
             "Get preferred action for state")
        .def("updateSelfModel", &SelfModel::updateSelfModel,
             py::arg("predicted"), py::arg("actual"), py::arg("action"),
             "Update self-model based on prediction error")
        .def("getCapabilityLevel", &SelfModel::getCapabilityLevel,
             "Get current capability level (0-1)")
        .def("getBodyAwareness", &SelfModel::getBodyAwareness,
             "Get body awareness (how accurate predictions are)")
        .def("clear", &SelfModel::clear,
             "Clear self-model")
        .def("hasSelfModel", &SelfModel::hasSelfModel,
             "Check if self-model has been formed");

    py::class_<SocialLearning>(m, "SocialLearning", R"pbdoc(Social learning from observing other agents)pbdoc")
        .def(py::init<>())
        .def("initialize", &SocialLearning::initialize,
             py::arg("brain"),
             "Initialize with brain reference")
        .def("observeAgentAction", &SocialLearning::observeAgentAction,
             py::arg("observedAction"), py::arg("observerState"), py::arg("resultingState"),
             "Record observation of another agent's action")
        .def("canImitate", &SocialLearning::canImitate,
             py::arg("observedAction"),
             "Can imitate observed action?")
        .def("getImitationAction", &SocialLearning::getImitationAction,
             py::arg("currentState"),
             "Get best action to imitate")
        .def("learnCommunicationSignal", &SocialLearning::learnCommunicationSignal,
             py::arg("signalPattern"), py::arg("signalReward"),
             "Learn communication signal from another agent")
        .def("detectSignal", &SocialLearning::detectSignal,
             py::arg("neuralPattern"),
             "Detect if another agent is signaling")
        .def("getSignalPattern", &SocialLearning::getSignalPattern,
             "Get learned signal pattern")
        .def("getSignalMeaning", &SocialLearning::getSignalMeaning,
             "Get signal meaning (associated reward)")
        .def("updateSocialKnowledge", &SocialLearning::updateSocialKnowledge,
             py::arg("interactionReward"),
             "Update social knowledge based on interactions")
        .def("clear", &SocialLearning::clear,
             "Clear social learning")
        .def("hasSocialKnowledge", &SocialLearning::hasSocialKnowledge,
             "Check if has learned from others")
        .def("getObservationCount", &SocialLearning::getObservationCount,
             "Get observation count");

    // Cognition Infrastructure
    py::class_<SpatialRepresentation>(m, "SpatialRepresentation", R"pbdoc(Spatial relationship encoding in neural patterns)pbdoc")
        .def(py::init<>())
        .def("initialize", &SpatialRepresentation::initialize,
             py::arg("brain"),
             "Initialize with brain reference")
        .def("recordPosition", &SpatialRepresentation::recordPosition,
             py::arg("x"), py::arg("y"), py::arg("sensoryCues"),
             "Record current position experience")
        .def("getPredictedPosition", &SpatialRepresentation::getPredictedPosition,
             "Get predicted position based on path integration")
        .def("integrateMovement", &SpatialRepresentation::integrateMovement,
             py::arg("dx"), py::arg("dy"),
             "Update position estimate based on movement")
        .def("getPositionActivation", &SpatialRepresentation::getPositionActivation,
             py::arg("x"), py::arg("y"),
             "Get position neuron activation")
        .def("getPlaceNeuronsNear", &SpatialRepresentation::getPlaceNeuronsNear,
             py::arg("x"), py::arg("y"), py::arg("radius"),
             "Find place neurons near a position")
        .def("getGridSpacing", &SpatialRepresentation::getGridSpacing,
             "Get grid spacing (if grid cells formed)")
        .def("clear", &SpatialRepresentation::clear,
             "Clear spatial representations")
        .def("hasPlaceCells", &SpatialRepresentation::hasPlaceCells,
             "Check if place cells have formed");

    py::class_<TemporalRelation>(m, "TemporalRelation", R"pbdoc(Temporal relationship encoding)pbdoc")
        .def(py::init<>())
        .def("initialize", &TemporalRelation::initialize,
             py::arg("brain"),
             "Initialize with brain reference")
        .def("recordSequence", &TemporalRelation::recordSequence,
             py::arg("eventA"), py::arg("eventB"), py::arg("reward"),
             "Record that event A was followed by event B")
        .def("predictNext", &TemporalRelation::predictNext,
             py::arg("eventA"),
             "Predict what comes next after event A")
        .def("predictPrevious", &TemporalRelation::predictPrevious,
             py::arg("eventB"),
             "Predict what came before event B")
        .def("getPredictionConfidence", &TemporalRelation::getPredictionConfidence,
             py::arg("event"),
             "Get confidence in temporal prediction")
        .def("getSequenceLength", &TemporalRelation::getSequenceLength,
             "Get temporal sequence length")
        .def("clear", &TemporalRelation::clear,
             "Clear temporal relationships");

    // Cognition concepts
    py::class_<PlanningCandidate>(m, "PlanningCandidate", R"pbdoc(Possible action sequence during planning)pbdoc")
        .def(py::init<>())
        .def_readwrite("actions", &PlanningCandidate::actions,
                       "Sequence of actions")
        .def_readwrite("predictedStates", &PlanningCandidate::predictedStates,
                       "Predicted sensory states")
        .def_readwrite("expectedReward", &PlanningCandidate::expectedReward,
                       "Expected total reward")
        .def_readwrite("confidence", &PlanningCandidate::confidence,
                       "Planning confidence")
        .def_readwrite("depth", &PlanningCandidate::depth,
                       "Planning depth");

    py::class_<DiscoveredConcept>(m, "DiscoveredConcept", R"pbdoc(Concept discovered from experience)pbdoc")
        .def(py::init<>())
        .def_readwrite("id", &DiscoveredConcept::id,
                       "Concept ID")
        .def_readwrite("prototype", &DiscoveredConcept::prototype,
                       "Central prototype pattern")
        .def_readwrite("instances", &DiscoveredConcept::instances,
                       "Instances belonging to this concept")
        .def_readwrite("categoryHint", &DiscoveredConcept::categoryHint,
                       "Category label derived from properties")
        .def_readwrite("associatedConceptIds", &DiscoveredConcept::associatedConceptIds,
                       "Associated concept IDs")
        .def_readwrite("totalObservations", &DiscoveredConcept::totalObservations,
                       "Total observations of this concept")
        .def_readwrite("avgStability", &DiscoveredConcept::avgStability,
                       "Average stability of instances");

    py::class_<ConceptInstance>(m, "ConceptInstance", R"pbdoc(Single instance of a concept)pbdoc")
        .def(py::init<>())
        .def_readwrite("pattern", &ConceptInstance::pattern,
                       "Neural activity pattern")
        .def_readwrite("features", &ConceptInstance::features,
                       "Associated sensory features")
        .def_readwrite("observationCount", &ConceptInstance::observationCount,
                       "Times observed")
        .def_readwrite("lastObserved", &ConceptInstance::lastObserved,
                       "Last observation time")
        .def_readwrite("avgReward", &ConceptInstance::avgReward,
                       "Average reward when observed")
        .def_readwrite("stability", &ConceptInstance::stability,
                       "Pattern stability (0-1)");

    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Discovers recurring patterns from experience)pbdoc")
        .def(py::init<>())
        .def("initialize", &ConceptFormation::initialize,
             py::arg("brain"),
             "Initialize with brain reference")
        .def("presentExperience", &ConceptFormation::presentExperience,
             py::arg("pattern"), py::arg("features"), py::arg("reward"), py::arg("currentTime"),
             "Present new experience to concept formation")
        .def("getMatchingConcept", &ConceptFormation::getMatchingConcept,
             py::arg("pattern"), py::arg("similarityThreshold") = 0.7f,
             "Get concept that best matches pattern")
        .def("getConcepts", &ConceptFormation::getConcepts,
             "Get all discovered concepts")
        .def("getConcept", &ConceptFormation::getConcept,
             py::arg("conceptId"),
             "Get specific concept")
        .def("getConceptPrototype", &ConceptFormation::getConceptPrototype,
             py::arg("conceptId"),
             "Get concept prototype")
        .def("getConceptInstances", &ConceptFormation::getConceptInstances,
             py::arg("conceptId"),
             "Get concept instances")
        .def("getConceptStability", &ConceptFormation::getConceptStability,
             py::arg("conceptId"),
             "Get concept stability (0-1)")
        .def("getConceptCount", &ConceptFormation::getConceptCount,
             "Get number of discovered concepts")
        .def("mergeConcepts", &ConceptFormation::mergeConcepts,
             py::arg("conceptA"), py::arg("conceptB"),
             "Merge two concepts when they're actually the same")
        .def("updateConcept", &ConceptFormation::updateConcept,
             py::arg("conceptId"), py::arg("newPattern"), py::arg("features"), py::arg("reward"),
             "Update concept with new instance")
        .def("computeSimilarity", &ConceptFormation::computeSimilarity,
             py::arg("a"), py::arg("b"),
             "Compute similarity between two patterns")
        .def("isNovel", &ConceptFormation::isNovel,
             py::arg("pattern"), py::arg("similarityThreshold") = 0.7f,
             "Check if pattern is novel")
        .def("findConceptForPattern", &ConceptFormation::findConceptForPattern,
             py::arg("pattern"),
             "Get concept for a given pattern")
        .def("clear", &ConceptFormation::clear,
             "Clear all concepts")
        .def("getGeneralizationAbility", &ConceptFormation::getGeneralizationAbility,
             py::arg("conceptId"),
             "Get concept generalization ability")
        .def("setFormationThreshold", &ConceptFormation::setFormationThreshold,
             py::arg("threshold"),
             "Set concept formation threshold")
        .def("setStabilityWindow", &ConceptFormation::setStabilityWindow,
             py::arg("window"),
             "Set stability observation window");

    // Neuromodulation
    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity drive: exploration motivation)pbdoc")
        .def(py::init<>())
        .def("initialize", &Curiosity::initialize,
             py::arg("brain"),
             "Initialize with brain reference")
        .def("getLevel", &Curiosity::getLevel,
             "Get curiosity level")
        .def("update", &Curiosity::update,
             py::arg("novelty"), py::arg("predictionError"), py::arg("dt"),
             "Update curiosity based on novelty and prediction error")
        .def("getExplorationDrive", &Curiosity::getExplorationDrive,
             "Get exploration drive (same as level)")
        .def("setNoveltyWeight", &Curiosity::setNoveltyWeight,
             py::arg("weight"),
             "Set novelty weight")
        .def("setPredictionErrorWeight", &Curiosity::setPredictionErrorWeight,
             py::arg("weight"),
             "Set prediction error weight")
        .def("reset", &Curiosity::reset,
             "Reset curiosity");

    py::class_<Reward>(m, "Reward", R"pbdoc(Reward signal for reinforcement learning)pbdoc")
        .def(py::init<>())
        .def("getValue", &Reward::getValue,
             "Get current reward value")
        .def("setValue", &Reward::setValue,
             py::arg("value"),
             "Set reward value")
        .def("add", &Reward::add,
             py::arg("delta"),
             "Add to current reward")
        .def("reset", &Reward::reset,
             "Reset accumulated reward")
        .def("computeReward", &Reward::computeReward,
             py::arg("observation"),
             "Compute reward from observation")
        .def("getHistory", &Reward::getHistory,
             "Get reward history")
        .def("clearHistory", &Reward::clearHistory,
             "Clear reward history");

    py::class_<PredictionErrorSignal::ErrorComponents>(m, "ErrorComponents", R"pbdoc(Error components: intensity, spatial, temporal)pbdoc")
        .def(py::init<>())
        .def_readwrite("intensityError", &PredictionErrorSignal::ErrorComponents::intensityError,
                       "How bright/loud things were")
        .def_readwrite("spatialError", &PredictionErrorSignal::ErrorComponents::spatialError,
                       "Where things were")
        .def_readwrite("temporalError", &PredictionErrorSignal::ErrorComponents::temporalError,
                       "When things happened")
        .def_readwrite("totalError", &PredictionErrorSignal::ErrorComponents::totalError,
                       "Total error magnitude");

    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Computes and broadcasts prediction error)pbdoc")
        .def(py::init<>())
        .def("computeError", &PredictionError::computeError,
             py::arg("predicted"), py::arg("actual"),
             "Compute error between predicted and actual")
        .def("getErrorComponents", &PredictionError::getErrorComponents,
             "Get error components")
        .def("getModulationSignal", &PredictionError::getModulationSignal,
             "Get neuromodulation signal strength (0-1)")
        .def("recordError", &PredictionError::recordError,
             py::arg("error"), py::arg("step"),
             "Record error for history")
        .def("getErrorHistory", &PredictionError::getErrorHistory,
             "Get error history")
        .def("isSurprising", &PredictionError::isSurprising,
             "Check if event was surprising")
        .def("setSurpriseThreshold", &PredictionError::setSurpriseThreshold,
             py::arg("threshold"),
             "Set surprise threshold")
        .def("getSurpriseThreshold", &PredictionError::getSurpriseThreshold,
             "Get surprise threshold");

    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(Base neuromodulator class)pbdoc")
        .def(py::init<>())
        .def("initialize", &Neuromodulator::initialize,
             py::arg("brain"),
             "Initialize with brain reference")
        .def("update", &Neuromodulator::update,
             py::arg("dt"),
             "Update neuromodulator state")
        .def("getLevel", &Neuromodulator::getLevel,
             "Get current neuromodulation level")
        .def("reset", &Neuromodulator::reset,
             "Reset neuromodulator");

    // Prediction System
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Prediction system: predicts future sensory states)pbdoc")
        .def(py::init<>())
        .def("predictNextState", &PredictionSystem::predictNextState,
             py::arg("currentState"),
             "Make prediction for next timestep")
        .def("updatePredictions", &PredictionSystem::updatePredictions,
             py::arg("predicted"), py::arg("actual"),
             "Update predictions based on observation")
        .def("getPredictionError", &PredictionSystem::getPredictionError,
             "Get prediction error")
        .def("getConfidence", &PredictionSystem::getConfidence,
             "Get prediction confidence")
        .def("getErrorHistory", &PredictionSystem::getErrorHistory,
             "Get prediction error history")
        .def("clearHistory", &PredictionSystem::clearHistory,
             "Clear prediction history")
        .def("train", &PredictionSystem::train,
             py::arg("observation"),
             "Train prediction model");

    // Remaining original bindings
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

    // Memory Systems
    py::class_<WorkingMemory>(m, "WorkingMemory", R"pbdoc(Working memory: temporary active storage of information)pbdoc")
        .def(py::init<>())
        .def("store", &WorkingMemory::store,
             py::arg("neuron"), py::arg("value"),
             "Store item in working memory")
        .def("retrieve", &WorkingMemory::retrieve,
             py::arg("neuron"),
             "Retrieve item from working memory")
        .def("contains", &WorkingMemory::contains,
             py::arg("neuron"),
             "Check if item exists in working memory")
        .def("clear", &WorkingMemory::clear,
             "Clear all working memory")
        .def("getCapacity", &WorkingMemory::getCapacity,
             "Get working memory capacity")
        .def("getCurrentSize", &WorkingMemory::getCurrentSize,
             "Get current working memory size")
        .def("decay", &WorkingMemory::decay,
             py::arg("decayRate"),
             "Decay all items in working memory");

    py::class_<EpisodicMemoryItem>(m, "EpisodicMemoryItem", R"pbdoc(Episodic memory item)pbdoc")
        .def(py::init<>())
        .def_readwrite("timestamp", &EpisodicMemoryItem::timestamp,
                       "Timestamp of the episode")
        .def_readwrite("neurons", &EpisodicMemoryItem::neurons,
                       "Neurons involved in the episode")
        .def_readwrite("values", &EpisodicMemoryItem::values,
                       "Values associated with neurons")
        .def_readwrite("metadata", &EpisodicMemoryItem::metadata,
                       "Additional metadata for the episode");

    py::class_<EpisodicMemory>(m, "EpisodicMemory", R"pbdoc(Episodic memory: storage of events and experiences)pbdoc")
        .def(py::init<>())
        .def("storeEpisode", &EpisodicMemory::storeEpisode,
             py::arg("episode"),
             "Store episode in episodic memory")
        .def("retrieveEpisode", &EpisodicMemory::retrieveEpisode,
             py::arg("index"),
             "Retrieve episode from episodic memory")
        .def("getEpisodeCount", &EpisodicMemory::getEpisodeCount,
             "Get number of episodes in episodic memory")
        .def("getRecentEpisodes", &EpisodicMemory::getRecentEpisodes,
             py::arg("count"),
             "Get recent episodes from episodic memory")
        .def("clear", &EpisodicMemory::clear,
             "Clear all episodic memory")
        .def("consolidate", &EpisodicMemory::consolidate,
             py::arg("relevanceThreshold"),
             "Consolidate episodes to long-term storage");

    py::class_<SemanticMemory>(m, "SemanticMemory", R"pbdoc(Semantic memory: gradually acquired knowledge)pbdoc")
        .def(py::init<>())
        .def("storeFact", &SemanticMemory::storeFact,
             py::arg("key"), py::arg("value"),
             "Store fact in semantic memory")
        .def("retrieveFact", &SemanticMemory::retrieveFact,
             py::arg("key"),
             "Retrieve fact from semantic memory")
        .def("hasFact", &SemanticMemory::hasFact,
             py::arg("key"),
             "Check if fact exists in semantic memory")
        .def("getAllFacts", &SemanticMemory::getAllFacts,
             "Get all facts in semantic memory")
        .def("clear", &SemanticMemory::clear,
             "Clear all semantic memory");

    py::class_<Skill>(m, "Skill", R"pbdoc(Learned skill in procedural memory)pbdoc")
        .def(py::init<>())
        .def_readwrite("name", &Skill::name,
                       "Skill name")
        .def_readwrite("neuralPattern", &Skill::neuralPattern,
                       "Neural pattern representing the skill")
        .def_readwrite("proficiency", &Skill::proficiency,
                       "Skill proficiency level (0-1)");

    py::class_<ProceduralMemory>(m, "ProceduralMemory", R"pbdoc(Procedural memory: learned skills and habits)pbdoc")
        .def(py::init<>())
        .def("learnSkill", &ProceduralMemory::learnSkill,
             py::arg("name"), py::arg("pattern"),
             "Learn new skill")
        .def("getSkill", &ProceduralMemory::getSkill,
             py::arg("name"),
             "Get skill from procedural memory")
        .def("getAllSkills", &ProceduralMemory::getAllSkills,
             "Get all learned skills")
        .def("updateProficiency", &ProceduralMemory::updateProficiency,
             py::arg("name"), py::arg("delta"),
             "Update skill proficiency")
        .def("clear", &ProceduralMemory::clear,
             "Clear all skills from procedural memory");

    py::class_<AssociativeMemory>(m, "AssociativeMemory", R"pbdoc(Associative memory: relationships between representations)pbdoc")
        .def(py::init<>())
        .def("associate", &AssociativeMemory::associate,
             py::arg("a"), py::arg("b"), py::arg("strength"),
             "Create association between two neurons")
        .def("getAssociations", &AssociativeMemory::getAssociations,
             py::arg("neuron"),
             "Get associated neurons")
        .def("getAssociationStrength", &AssociativeMemory::getAssociationStrength,
             py::arg("a"), py::arg("b"),
             "Get association strength between neurons")
        .def("updateAssociation", &AssociativeMemory::updateAssociation,
             py::arg("a"), py::arg("b"), py::arg("delta"),
             "Update association strength")
        .def("clear", &AssociativeMemory::clear,
             "Clear all associations");

    // Neural memory system (more advanced)
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Neural working memory using persistent activity)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize,
             py::arg("brain"),
             "Initialize with brain reference")
        .def("store", &NeuralWorkingMemory::store,
             py::arg("pattern"), py::arg("strength") = 1.0f,
             "Store information in neural working memory")
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron,
             py::arg("neuron"), py::arg("activation"),
             "Store activation to specific neuron")
        .def("retrieve", &NeuralWorkingMemory::retrieve,
             "Retrieve current working memory content")
        .def("contains", &NeuralWorkingMemory::contains,
             py::arg("neuron"),
             "Check if neuron is in working memory")
        .def("getNeuronActivation", &NeuralWorkingMemory::getNeuronActivation,
             py::arg("neuron"),
             "Get activation level of neuron")
        .def("update", &NeuralWorkingMemory::update,
             py::arg("dt"),
             "Update working memory dynamics")
        .def("clear", &NeuralWorkingMemory::clear,
             "Clear working memory")
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces,
             "Get number of active memory traces")
        .def("getCapacity", &NeuralWorkingMemory::getCapacity,
             "Get working memory capacity")
        .def("setCapacity", &NeuralWorkingMemory::setCapacity,
             py::arg("cap"),
             "Set working memory capacity")
        .def("getDecayRate", &NeuralWorkingMemory::getDecayRate,
             "Get decay rate")
        .def("setDecayRate", &NeuralWorkingMemory::setDecayRate,
             py::arg("rate"),
             "Set decay rate")
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons,
             "Get neurons currently in working memory")
        .def("strengthenMemory", &NeuralWorkingMemory::strengthenMemory,
             py::arg("factor"),
             "Strengthen working memory representation")
        .def("runCompetition", &NeuralWorkingMemory::runCompetition,
             "Run competition between memory traces")
        .def("isWinning", &NeuralWorkingMemory::isWinning,
             py::arg("neuron"),
             "Check if neuron is winning")
        .def("getMemoryActivity", &NeuralWorkingMemory::getMemoryActivity,
             "Get overall memory activity level");

    py::class_<AttentionalSelection>(m, "AttentionalSelection", R"pbdoc(Neural attention through competition)pbdoc")
        .def(py::init<>())
        .def("initialize", &AttentionalSelection::initialize,
             py::arg("brain"),
             "Initialize with brain reference")
        .def("processCompetition", &AttentionalSelection::processCompetition,
             py::arg("competitors"), py::arg("globalInhibition") = 0.5f,
             "Process competing sensory inputs and select winners")
        .def("focusOnRegion", &AttentionalSelection::focusOnRegion,
             py::arg("region"),
             "Apply attention to a specific region")
        .def("releaseAttention", &AttentionalSelection::releaseAttention,
             "Release attention (allow all regions equal processing)")
        .def("getAttendedRegions", &AttentionalSelection::getAttendedRegions,
             "Get current attention focus")
        .def("setInhibitionStrength", &AttentionalSelection::setInhibitionStrength,
             py::arg("strength"),
             "Set inhibition strength")
        .def("setExcitationStrength", &AttentionalSelection::setExcitationStrength,
             py::arg("strength"),
             "Set excitation strength")
        .def("setCompetitionThreshold", &AttentionalSelection::setCompetitionThreshold,
             py::arg("threshold"),
             "Set competition threshold")
        .def("getInhibitionFor", &AttentionalSelection::getInhibitionFor,
             py::arg("neuron"),
             "Get inhibition level for a neuron")
        .def("getExcitationFor", &AttentionalSelection::getExcitationFor,
             py::arg("neuron"),
             "Get excitation level for a neuron")
        .def("update", &AttentionalSelection::update,
             py::arg("dt"),
             "Update attention dynamics")
        .def("isAttended", &AttentionalSelection::isAttended,
             py::arg("neuron"),
             "Check if neuron is currently attended")
        .def("getWinners", &AttentionalSelection::getWinners,
             "Get attention winners")
        .def("applyTopDownBias", &AttentionalSelection::applyTopDownBias,
             py::arg("neuron"), py::arg("biasStrength"),
             "Apply top-down attention bias")
        .def("applyBottomUpSalience", &AttentionalSelection::applyBottomUpSalience,
             py::arg("neuron"), py::arg("salienceStrength"),
             "Apply bottom-up attention salience")
        .def("reset", &AttentionalSelection::reset,
             "Reset attention");

    // Prediction Systems
    py::class_<NeuralPrediction>(m, "NeuralPrediction", R"pbdoc(Neural prediction using spiking substrate)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPrediction::initialize,
             py::arg("brain"),
             "Initialize with brain reference")
        .def("recordSensoryState", &NeuralPrediction::recordSensoryState,
             py::arg("sensoryState"), py::arg("currentStep"),
             "Record current sensory state for prediction")
        .def("generatePrediction", &NeuralPrediction::generatePrediction,
             py::arg("currentStep"),
             "Generate prediction for next timestep")
        .def("updateWithObservation", &NeuralPrediction::updateWithObservation,
             py::arg("actualState"), py::arg("currentStep"),
             "Update predictions with actual observation")
        .def("predictActionConsequence", &NeuralPrediction::predictActionConsequence,
             py::arg("action"), py::arg("currentState"),
             "Predict consequences of an action")
        .def("getPredictionError", &NeuralPrediction::getPredictionError,
             "Get prediction error (0 = perfect, 1 = total error)")
        .def("getPredictionConfidence", &NeuralPrediction::getPredictionConfidence,
             "Get prediction confidence")
        .def("predictMultipleSteps", &NeuralPrediction::predictMultipleSteps,
             py::arg("currentStep"), py::arg("numSteps"),
             "Multi-step prediction")
        .def("recordAction", &NeuralPrediction::recordAction,
             py::arg("action"), py::arg("step"),
             "Record action for learning")
        .def("getErrorHistory", &NeuralPrediction::getErrorHistory,
             "Get prediction error history")
        .def("clearHistory", &NeuralPrediction::clearHistory,
             "Clear prediction history")
        .def("enableTemporalPrediction", &NeuralPrediction::enableTemporalPrediction,
             py::arg("enable"),
             "Enable/disable temporal prediction")
        .def("enableActionConsequencePrediction", &NeuralPrediction::enableActionConsequencePrediction,
             py::arg("enable"),
             "Enable/disable action consequence prediction")
        .def("setSequenceMemorySize", &NeuralPrediction::setSequenceMemorySize,
             py::arg("size"),
             "Set sequence memory size")
        .def("setPredictionHorizon", &NeuralPrediction::setPredictionHorizon,
             py::arg("steps"),
             "Set prediction horizon")
        .def("getPredictionNeurons", &NeuralPrediction::getPredictionNeurons,
             "Get neurons involved in prediction")
        .def("getSequenceNeurons", &NeuralPrediction::getSequenceNeurons,
             "Get neurons involved in sequences");

    py::class_<ActionConsequencePredictor>(m, "ActionConsequencePredictor", R"pbdoc(Learns action -> consequence mappings)pbdoc")
        .def(py::init<>())
        .def("initialize", &ActionConsequencePredictor::initialize,
             py::arg("brain"),
             "Initialize with brain reference")
        .def("recordExperience", &ActionConsequencePredictor::recordExperience,
             py::arg("action"), py::arg("beforeState"), py::arg("afterState"), py::arg("reward"),
             "Record action consequence experience")
        .def("predictConsequence", &ActionConsequencePredictor::predictConsequence,
             py::arg("action"), py::arg("currentState"),
             "Predict consequence of an action")
        .def("getConsequenceConfidence", &ActionConsequencePredictor::getConsequenceConfidence,
             py::arg("action"), py::arg("currentState"),
             "Get prediction confidence")
        .def("updatePrediction", &ActionConsequencePredictor::updatePrediction,
             py::arg("action"), py::arg("predicted"), py::arg("actual"), py::arg("reward"),
             "Update based on actual outcome")
        .def("getActionQuality", &ActionConsequencePredictor::getActionQuality,
             py::arg("action"),
             "Get learned action quality")
        .def("clear", &ActionConsequencePredictor::clear,
             "Clear learned associations");

    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Computes and broadcasts prediction error)pbdoc")
        .def(py::init<>())
        .def("computeError", &PredictionError::computeError,
             py::arg("predicted"), py::arg("actual"),
             "Compute error between predicted and actual")
        .def("getErrorComponents", &PredictionError::getErrorComponents,
             "Get error components (intensity, spatial, temporal)")
        .def("getModulationSignal", &PredictionError::getModulationSignal,
             "Get neuromodulation signal strength (0-1)")
        .def("recordError", &PredictionError::recordError,
             py::arg("error"), py::arg("step"),
             "Record error for history")
        .def("getErrorHistory", &PredictionError::getErrorHistory,
             "Get error history")
        .def("isSurprising", &PredictionError::isSurprising,
             "Check if event was surprising")
        .def("setSurpriseThreshold", &PredictionError::setSurpriseThreshold,
             py::arg("threshold"),
             "Set surprise threshold")
        .def("getSurpriseThreshold", &PredictionError::getSurpriseThreshold,
             "Get surprise threshold");

    // Cognitive Systems
    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planner using predictive dynamics)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPlanner::initialize,
             py::arg("brain"),
             "Initialize with brain reference")
        .def("planAction", &NeuralPlanner::planAction,
             py::arg("currentState"), py::arg("targetReward") = 0.5f,
             "Plan next action given current state and goal")
        .def("evaluateSequence", &NeuralPlanner::evaluateSequence,
             py::arg("actions"), py::arg("startState"),
             "Evaluate a potential action sequence")
        .def("getPlanningDepth", &NeuralPlanner::getPlanningDepth,
             "Get number of planning steps")
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth,
             py::arg("depth"),
             "Set planning depth")
        .def("getPlanningConfidence", &NeuralPlanner::getPlanningConfidence,
             "Get planning confidence")
        .def("updatePlanQuality", &NeuralPlanner::updatePlanQuality,
             py::arg("plannedActions"), py::arg("actualActions"), py::arg("actualReward"),
             "Update plans based on actual outcome")
        .def("clearCache", &NeuralPlanner::clearCache,
             "Clear planning cache")
        .def("setActionQuality", &NeuralPlanner::setActionQuality,
             py::arg("action"), py::arg("quality"),
             "Set action quality from experience")
        .def("getCurrentGoal", &NeuralPlanner::getCurrentGoal,
             "Get current planning goal")
        .def("setCurrentGoal", &NeuralPlanner::setCurrentGoal,
             py::arg("goal"),
             "Set current planning goal")
        .def("wasRecentPlanSuccessful", &NeuralPlanner::wasRecentPlanSuccessful,
             "Check if recent plan was successful");

    py::class_<SelfModel>(m, "SelfModel", R"pbdoc(Self-model: internal representation of agent)pbdoc")
        .def(py::init<>())
        .def("initialize", &SelfModel::initialize,
             py::arg("brain"),
             "Initialize with brain reference")
        .def("recordSelfAction", &SelfModel::recordSelfAction,
             py::arg("action"), py::arg("beforeState"), py::arg("afterState"),
             "Record that taking an action caused a specific change")
        .def("predictActionConsequence", &SelfModel::predictActionConsequence,
             py::arg("action"), py::arg("currentState"),
             "Predict sensory consequence of an action")
        .def("getSelfModelConfidence", &SelfModel::getSelfModelConfidence,
             py::arg("action"),
             "Get confidence in self-model for action")
        .def("computeSelfGeneratedLikelihood", &SelfModel::computeSelfGeneratedLikeness,
             py::arg("beforeState"), py::arg("afterState"), py::arg("action"),
             "Check if change likely caused by self")
        .def("getPreferredAction", &SelfModel::getPreferredAction,
             py::arg("state"),
             "Get preferred action for state")
        .def("updateSelfModel", &SelfModel::updateSelfModel,
             py::arg("predicted"), py::arg("actual"), py::arg("action"),
             "Update self-model based on prediction error")
        .def("getCapabilityLevel", &SelfModel::getCapabilityLevel,
             "Get current capability level (0-1)")
        .def("getBodyAwareness", &SelfModel::getBodyAwareness,
             "Get body awareness (how accurate predictions are)")
        .def("clear", &SelfModel::clear,
             "Clear self-model")
        .def("hasSelfModel", &SelfModel::hasSelfModel,
             "Check if self-model has been formed");

    py::class_<SocialLearning>(m, "SocialLearning", R"pbdoc(Social learning from observing other agents)pbdoc")
        .def(py::init<>())
        .def("initialize", &SocialLearning::initialize,
             py::arg("brain"),
             "Initialize with brain reference")
        .def("observeAgentAction", &SocialLearning::observeAgentAction,
             py::arg("observedAction"), py::arg("observerState"), py::arg("resultingState"),
             "Record observation of another agent's action")
        .def("canImitate", &SocialLearning::canImitate,
             py::arg("observedAction"),
             "Can imitate observed action?")
        .def("getImitationAction", &SocialLearning::getImitationAction,
             py::arg("currentState"),
             "Get best action to imitate")
        .def("learnCommunicationSignal", &SocialLearning::learnCommunicationSignal,
             py::arg("signalPattern"), py::arg("signalReward"),
             "Learn communication signal from another agent")
        .def("detectSignal", &SocialLearning::detectSignal,
             py::arg("neuralPattern"),
             "Detect if another agent is signaling")
        .def("getSignalPattern", &SocialLearning::getSignalPattern,
             "Get learned signal pattern")
        .def("getSignalMeaning", &SocialLearning::getSignalMeaning,
             "Get signal meaning (associated reward)")
        .def("updateSocialKnowledge", &SocialLearning::updateSocialKnowledge,
             py::arg("interactionReward"),
             "Update social knowledge based on interactions")
        .def("clear", &SocialLearning::clear,
             "Clear social learning")
        .def("hasSocialKnowledge", &SocialLearning::hasSocialKnowledge,
             "Check if has learned from others")
        .def("getObservationCount", &SocialLearning::getObservationCount,
             "Get observation count");

    // Cognition Infrastructure
    py::class_<SpatialRepresentation>(m, "SpatialRepresentation", R"pbdoc(Spatial relationship encoding in neural patterns)pbdoc")
        .def(py::init<>())
        .def("initialize", &SpatialRepresentation::initialize,
             py::arg("brain"),
             "Initialize with brain reference")
        .def("recordPosition", &SpatialRepresentation::recordPosition,
             py::arg("x"), py::arg("y"), py::arg("sensoryCues"),
             "Record current position experience")
        .def("getPredictedPosition", &SpatialRepresentation::getPredictedPosition,
             "Get predicted position based on path integration")
        .def("integrateMovement", &SpatialRepresentation::integrateMovement,
             py::arg("dx"), py::arg("dy"),
             "Update position estimate based on movement")
        .def("getPositionActivation", &SpatialRepresentation::getPositionActivation,
             py::arg("x"), py::arg("y"),
             "Get position neuron activation")
        .def("getPlaceNeuronsNear", &SpatialRepresentation::getPlaceNeuronsNear,
             py::arg("x"), py::arg("y"), py::arg("radius"),
             "Find place neurons near a position")
        .def("getGridSpacing", &SpatialRepresentation::getGridSpacing,
             "Get grid spacing (if grid cells formed)")
        .def("clear", &SpatialRepresentation::clear,
             "Clear spatial representations")
        .def("hasPlaceCells", &SpatialRepresentation::hasPlaceCells,
             "Check if place cells have formed");

    py::class_<TemporalRelation>(m, "TemporalRelation", R"pbdoc(Temporal relationship encoding)pbdoc")
        .def(py::init<>())
        .def("initialize", &TemporalRelation::initialize,
             py::arg("brain"),
             "Initialize with brain reference")
        .def("recordSequence", &TemporalRelation::recordSequence,
             py::arg("eventA"), py::arg("eventB"), py::arg("reward"),
             "Record that event A was followed by event B")
        .def("predictNext", &TemporalRelation::predictNext,
             py::arg("eventA"),
             "Predict what comes next after event A")
        .def("predictPrevious", &TemporalRelation::predictPrevious,
             py::arg("eventB"),
             "Predict what came before event B")
        .def("getPredictionConfidence", &TemporalRelation::getPredictionConfidence,
             py::arg("event"),
             "Get confidence in temporal prediction")
        .def("getSequenceLength", &TemporalRelation::getSequenceLength,
             "Get temporal sequence length")
        .def("clear", &TemporalRelation::clear,
             "Clear temporal relationships");

    // Cognition concepts
    py::class_<PlanningCandidate>(m, "PlanningCandidate", R"pbdoc(Possible action sequence during planning)pbdoc")
        .def(py::init<>())
        .def_readwrite("actions", &PlanningCandidate::actions,
                       "Sequence of actions")
        .def_readwrite("predictedStates", &PlanningCandidate::predictedStates,
                       "Predicted sensory states")
        .def_readwrite("expectedReward", &PlanningCandidate::expectedReward,
                       "Expected total reward")
        .def_readwrite("confidence", &PlanningCandidate::confidence,
                       "Planning confidence")
        .def_readwrite("depth", &PlanningCandidate::depth,
                       "Planning depth");

    py::class_<DiscoveredConcept>(m, "DiscoveredConcept", R"pbdoc(Concept discovered from experience)pbdoc")
        .def(py::init<>())
        .def_readwrite("id", &DiscoveredConcept::id,
                       "Concept ID")
        .def_readwrite("prototype", &DiscoveredConcept::prototype,
                       "Central prototype pattern")
        .def_readwrite("instances", &DiscoveredConcept::instances,
                       "Instances belonging to this concept")
        .def_readwrite("categoryHint", &DiscoveredConcept::categoryHint,
                       "Category label derived from properties")
        .def_readwrite("associatedConceptIds", &DiscoveredConcept::associatedConceptIds,
                       "Associated concept IDs")
        .def_readwrite("totalObservations", &DiscoveredConcept::totalObservations,
                       "Total observations of this concept")
        .def_readwrite("avgStability", &DiscoveredConcept::avgStability,
                       "Average stability of instances");

    py::class_<ConceptInstance>(m, "ConceptInstance", R"pbdoc(Single instance of a concept)pbdoc")
        .def(py::init<>())
        .def_readwrite("pattern", &ConceptInstance::pattern,
                       "Neural activity pattern")
        .def_readwrite("features", &ConceptInstance::features,
                       "Associated sensory features")
        .def_readwrite("observationCount", &ConceptInstance::observationCount,
                       "Times observed")
        .def_readwrite("lastObserved", &ConceptInstance::lastObserved,
                       "Last observation time")
        .def_readwrite("avgReward", &ConceptInstance::avgReward,
                       "Average reward when observed")
        .def_readwrite("stability", &ConceptInstance::stability,
                       "Pattern stability (0-1)");

    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Discovers recurring patterns from experience)pbdoc")
        .def(py::init<>())
        .def("initialize", &ConceptFormation::initialize,
             py::arg("brain"),
             "Initialize with brain reference")
        .def("presentExperience", &ConceptFormation::presentExperience,
             py::arg("pattern"), py::arg("features"), py::arg("reward"), py::arg("currentTime"),
             "Present new experience to concept formation")
        .def("getMatchingConcept", &ConceptFormation::getMatchingConcept,
             py::arg("pattern"), py::arg("similarityThreshold") = 0.7f,
             "Get concept that best matches pattern")
        .def("getConcepts", &ConceptFormation::getConcepts,
             "Get all discovered concepts")
        .def("getConcept", &ConceptFormation::getConcept,
             py::arg("conceptId"),
             "Get specific concept")
        .def("getConceptPrototype", &ConceptFormation::getConceptPrototype,
             py::arg("conceptId"),
             "Get concept prototype")
        .def("getConceptInstances", &ConceptFormation::getConceptInstances,
             py::arg("conceptId"),
             "Get concept instances")
        .def("getConceptStability", &ConceptFormation::getConceptStability,
             py::arg("conceptId"),
             "Get concept stability (0-1)")
        .def("getConceptCount", &ConceptFormation::getConceptCount,
             "Get number of discovered concepts")
        .def("mergeConcepts", &ConceptFormation::mergeConcepts,
             py::arg("conceptA"), py::arg("conceptB"),
             "Merge two concepts when they're actually the same")
        .def("updateConcept", &ConceptFormation::updateConcept,
             py::arg("conceptId"), py::arg("newPattern"), py::arg("features"), py::arg("reward"),
             "Update concept with new instance")
        .def("computeSimilarity", &ConceptFormation::computeSimilarity,
             py::arg("a"), py::arg("b"),
             "Compute similarity between two patterns")
        .def("isNovel", &ConceptFormation::isNovel,
             py::arg("pattern"), py::arg("similarityThreshold") = 0.7f,
             "Check if pattern is novel")
        .def("findConceptForPattern", &ConceptFormation::findConceptForPattern,
             py::arg("pattern"),
             "Get concept for a given pattern")
        .def("clear", &ConceptFormation::clear,
             "Clear all concepts")
        .def("getGeneralizationAbility", &ConceptFormation::getGeneralizationAbility,
             py::arg("conceptId"),
             "Get concept generalization ability")
        .def("setFormationThreshold", &ConceptFormation::setFormationThreshold,
             py::arg("threshold"),
             "Set concept formation threshold")
        .def("setStabilityWindow", &ConceptFormation::setStabilityWindow,
             py::arg("window"),
             "Set stability observation window");

    // Neuromodulation
    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity drive: exploration motivation)pbdoc")
        .def(py::init<>())
        .def("initialize", &Curiosity::initialize,
             py::arg("brain"),
             "Initialize with brain reference")
        .def("getLevel", &Curiosity::getLevel,
             "Get curiosity level")
        .def("update", &Curiosity::update,
             py::arg("novelty"), py::arg("predictionError"), py::arg("dt"),
             "Update curiosity based on novelty and prediction error")
        .def("getExplorationDrive", &Curiosity::getExplorationDrive,
             "Get exploration drive (same as level)")
        .def("setNoveltyWeight", &Curiosity::setNoveltyWeight,
             py::arg("weight"),
             "Set novelty weight")
        .def("setPredictionErrorWeight", &Curiosity::setPredictionErrorWeight,
             py::arg("weight"),
             "Set prediction error weight")
        .def("reset", &Curiosity::reset,
             "Reset curiosity");

    py::class_<Reward>(m, "Reward", R"pbdoc(Reward signal for reinforcement learning)pbdoc")
        .def(py::init<>())
        .def("getValue", &Reward::getValue,
             "Get current reward value")
        .def("setValue", &Reward::setValue,
             py::arg("value"),
             "Set reward value")
        .def("add", &Reward::add,
             py::arg("delta"),
             "Add to current reward")
        .def("reset", &Reward::reset,
             "Reset accumulated reward")
        .def("computeReward", &Reward::computeReward,
             py::arg("observation"),
             "Compute reward from observation")
        .def("getHistory", &Reward::getHistory,
             "Get reward history")
        .def("clearHistory", &Reward::clearHistory,
             "Clear reward history");

    py::class_<PredictionError::ErrorComponents>(m, "ErrorComponents", R"pbdoc(Error components: intensity, spatial, temporal)pbdoc")
        .def(py::init<>())
        .def_readwrite("intensityError", &PredictionError::ErrorComponents::intensityError,
                       "How bright/loud things were")
        .def_readwrite("spatialError", &PredictionError::ErrorComponents::spatialError,
                       "Where things were")
        .def_readwrite("temporalError", &PredictionError::ErrorComponents::temporalError,
                       "When things happened")
        .def_readwrite("totalError", &PredictionError::ErrorComponents::totalError,
                       "Total error magnitude");

    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Computes and broadcasts prediction error)pbdoc")
        .def(py::init<>())
        .def("computeError", &PredictionError::computeError,
             py::arg("predicted"), py::arg("actual"),
             "Compute error between predicted and actual")
        .def("getErrorComponents", &PredictionError::getErrorComponents,
             "Get error components")
        .def("getModulationSignal", &PredictionError::getModulationSignal,
             "Get neuromodulation signal strength (0-1)")
        .def("recordError", &PredictionError::recordError,
             py::arg("error"), py::arg("step"),
             "Record error for history")
        .def("getErrorHistory", &PredictionError::getErrorHistory,
             "Get error history")
        .def("isSurprising", &PredictionError::isSurprising,
             "Check if event was surprising")
        .def("setSurpriseThreshold", &PredictionError::setSurpriseThreshold,
             py::arg("threshold"),
             "Set surprise threshold")
        .def("getSurpriseThreshold", &PredictionError::getSurpriseThreshold,
             "Get surprise threshold");

    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(Base neuromodulator class)pbdoc")
        .def(py::init<>())
        .def("initialize", &Neuromodulator::initialize,
             py::arg("brain"),
             "Initialize with brain reference")
        .def("update", &Neuromodulator::update,
             py::arg("dt"),
             "Update neuromodulator state")
        .def("getLevel", &Neuromodulator::getLevel,
             "Get current neuromodulation level")
        .def("reset", &Neuromodulator::reset,
             "Reset neuromodulator");

    // Prediction System
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Prediction system: predicts future sensory states)pbdoc")
        .def(py::init<>())
        .def("predictNextState", &PredictionSystem::predictNextState,
             py::arg("currentState"),
             "Make prediction for next timestep")
        .def("updatePredictions", &PredictionSystem::updatePredictions,
             py::arg("predicted"), py::arg("actual"),
             "Update predictions based on observation")
        .def("getPredictionError", &PredictionSystem::getPredictionError,
             "Get prediction error")
        .def("getConfidence", &PredictionSystem::getConfidence,
             "Get prediction confidence")
        .def("getErrorHistory", &PredictionSystem::getErrorHistory,
             "Get prediction error history")
        .def("clearHistory", &PredictionSystem::clearHistory,
             "Clear prediction history")
        .def("train", &PredictionSystem::train,
             py::arg("observation"),
             "Train prediction model");

    // Remaining original bindings
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

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
