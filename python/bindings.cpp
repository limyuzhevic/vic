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

    py::class_<WorkingMemory>(m, "WorkingMemory", R"pbdoc(Neural working memory system)pbdoc")
        .def(py::init<>())
        .def("store", &WorkingMemory::store, py::arg("neuron"), py::arg("value"),
             "Store information in working memory")
        .def("retrieve", &WorkingMemory::retrieve, py::arg("neuron"),
             "Retrieve information from working memory")
        .def("contains", &WorkingMemory::contains, py::arg("neuron"),
             "Check if working memory contains information for neuron")
        .def("clear", &WorkingMemory::clear, "Clear all working memory")
        .def("getCapacity", &WorkingMemory::getCapacity,
             "Get working memory capacity")
        .def("getCurrentSize", &WorkingMemory::getCurrentSize,
             "Get current working memory size")
        .def("getMemoryNeurons", &WorkingMemory::getMemoryNeurons,
             "Get neurons currently in working memory")
        .def("isActive", &WorkingMemory::isActive, py::arg("neuron"),
             "Check if neuron is currently active in working memory")
        .def("update", &WorkingMemory::update, py::arg("dt"),
             "Update working memory with decay and competition");

    py::class_<EpisodicMemory>(m, "EpisodicMemory", R"pbdoc(Neural episodic memory system)pbdoc")
        .def(py::init<>())
        .def("storeEpisode", &EpisodicMemory::storeEpisode, py::arg("episode"),
             "Store an episode in episodic memory")
        .def("retrieveEpisode", &EpisodicMemory::retrieveEpisode, py::arg("index"),
             "Retrieve episode by index")
        .def("getEpisodeCount", &EpisodicMemory::getEpisodeCount,
             "Get number of stored episodes")
        .def("getRecentEpisodes", &EpisodicMemory::getRecentEpisodes, py::arg("count"),
             "Get recent episodes")
        .def("clear", &EpisodicMemory::clear, "Clear all episodic memory")
        .def("consolidate", &EpisodicMemory::consolidate, py::arg("threshold"),
             "Consolidate important memories")
        .def("replayEpisode", &EpisodicMemory::replayEpisode, py::arg("episode"),
             "Replay episode for memory strengthening");

    py::class_<SemanticMemory>(m, "SemanticMemory", R"pbdoc(Neural semantic memory system)pbdoc")
        .def(py::init<>())
        .def("storeFact", &SemanticMemory::storeFact, py::arg("key"), py::arg("value"),
             "Store a fact in semantic memory")
        .def("retrieveFact", &SemanticMemory::retrieveFact, py::arg("key"),
             "Retrieve fact from semantic memory")
        .def("hasFact", &SemanticMemory::hasFact, py::arg("key"),
             "Check if semantic memory contains fact")
        .def("getAllFacts", &SemanticMemory::getAllFacts,
             "Get all facts in semantic memory")
        .def("getConceptSimilarity", &SemanticMemory::getConceptSimilarity,
             py::arg("concept1"), py::arg("concept2"), "Get similarity between concepts")
        .def("getRelatedConcepts", &SemanticMemory::getRelatedConcepts, py::arg("concept"),
             "Get concepts related to given concept");

    py::class_<ProceduralMemory>(m, "ProceduralMemory", R"pbdoc(Neural procedural memory system)pbdoc")
        .def(py::init<>())
        .def("learnSkill", &ProceduralMemory::learnSkill, py::arg("name"), py::arg("pattern"),
             "Learn a new skill from neural pattern")
        .def("executeSkill", &ProceduralMemory::executeSkill, py::arg("name"),
             "Execute a learned skill")
        .def("getSkill", &ProceduralMemory::getSkill, py::arg("name"),
             "Get skill by name")
        .def("updateProficiency", &ProceduralMemory::updateProficiency, py::arg("name"), py::arg("delta"), py::arg("successRate"),
             "Update skill proficiency")
        .def("getSkillEffectiveness", &ProceduralMemory::getSkillEffectiveness, py::arg("name"),
             "Get skill effectiveness")
        .def("getTopSkills", &ProceduralMemory::getTopSkills, py::arg("count"),
             "Get top skills by proficiency");

    py::class_<AssociativeMemory>(m, "AssociativeMemory", R"pbdoc(Neural associative memory system)pbdoc")
        .def(py::init<>())
        .def("associate", py::overload_cast<const std::string&, const std::string&, float>(&AssociativeMemory::associate), py::arg("key1"), py::arg("key2"), py::arg("strength"),
             "Create association between two concepts")
        .def("associate", py::overload_cast<NeuronId, NeuronId, float>(&AssociativeMemory::associate), py::arg("neuron1"), py::arg("neuron2"), py::arg("strength"),
             "Create association between two neurons")
        .def("associate", py::overload_cast<const std::string&, NeuronId, float>(&AssociativeMemory::associate), py::arg("concept"), py::arg("neuron"), py::arg("strength"),
             "Create association between concept and neuron")
        .def("getAssociations", py::overload_cast<const std::string&>(&AssociativeMemory::getAssociations), py::arg("key"),
             "Get concepts associated with given key")
        .def("getAssociations", py::overload_cast<NeuronId>(&AssociativeMemory::getAssociations), py::arg("neuron"),
             "Get neurons associated with given neuron")
        .def("getStrength", &AssociativeMemory::getAssociationStrength, py::arg("key1"), py::arg("key2"),
             "Get strength of association between two items")
        .def("propagateActivation", &AssociativeMemory::propagateActivation, py::arg("key"), py::arg("factor"),
             "Propagate activation through associations");

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

    // Add more commands to Python module for advanced users
    m.def("createAdvancedConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Set advanced configuration for complex experiments
        config->set("simulation_timestep", 0.0001, ConfigSource::Default);  // Higher precision
        config->set("neuron_count", 5000, ConfigSource::Default);         // Larger network
        config->set("region_count", 4, ConfigSource::Default);           // Multiple regions
        config->set("connection_probability", 0.2, ConfigSource::Default); // More connections
        
        // Memory system configuration
        config->set("working_memory_capacity", 500, ConfigSource::Default);
        config->set("episodic_memory_max_episodes", 10000, ConfigSource::Default);
        config->set("semantic_memory_max_concepts", 5000, ConfigSource::Default);
        config->set("procedural_memory_max_skills", 200, ConfigSource::Default);
        
        // Neuromodulation configuration
        config->set("dopamine_baseline", 0.1, ConfigSource::Default);
        config->set("curiosity_sensitivity", 0.5, ConfigSource::Default);
        config->set("novelty_threshold", 0.05, ConfigSource::Default);
        
        // Plasticity configuration
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.025f, ConfigSource::Default);
        config->set("stdp_time_constant", 25.0f, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.0002f, ConfigSource::Default);
        config->set("pruning_rate", 0.00002f, ConfigSource::Default);
        
        // Development configuration
        config->set("development_period_duration", 600.0, ConfigSource::Default);
        config->set("critical_period_start", 120.0, ConfigSource::Default);
        config->set("critical_period_end", 300.0, ConfigSource::Default);
        
        // Advanced options
        config->set("enable_structural_plasticity", true, ConfigSource::Default);
        config->set("enable_reward_modulation", true, ConfigSource::Default);
        config->set("enable_curiosity", true, ConfigSource::Default);
        config->set("enable_development", true, ConfigSource::Default);
        config->set("enable_predictive_coding", true, ConfigSource::Default);
        config->set("enable_social_learning", false, ConfigSource::Default);
        
        return config;
    }, "Create an advanced configuration optimized for complex experiments");
    
    m.def("createMinimalConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Minimal configuration for basic operation
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", 200, ConfigSource::Default);  // Smaller network
        config->set("region_count", 1, ConfigSource::Default);   // Single region
        config->set("connection_probability", 0.1, ConfigSource::Default);
        
        return config;
    }, "Create a minimal configuration for basic operation");
    
    m.def("createResearchConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Research-oriented configuration
        config->set("simulation_timestep", 0.0005, ConfigSource::Default);  // High precision
        config->set("neuron_count", 10000, ConfigSource::Default);          // Large network
        config->set("region_count", 8, ConfigSource::Default);            // Many regions
        config->set("connection_probability", 0.15, ConfigSource::Default);
        
        // Research focus areas
        config->set("research_focus", "memory_integration", ConfigSource::Default);
        config->set("research_tracking_level", "detailed", ConfigSource::Default);
        config->set("enable_detailed_metrics", true, ConfigSource::Default);
        config->set("enable_checkpointing", true, ConfigSource::Default);
        config->set("checkpoint_interval", 1000, ConfigSource::Default);
        
        // Neuromodulation focus
        config->set("curiosity_enabled", true, ConfigSource::Default);
        config->set("novelty_detection_sensitivity", 0.03, ConfigSource::Default);
        config->set("reward_prediction_error_weight", 1.5, ConfigSource::Default);
        
        return config;
    }, "Create a research-oriented configuration");
    
    m.def("createDevelopmentConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Development-focused configuration
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", 2000, ConfigSource::Default);
        config->set("region_count", 2, ConfigSource::Default);
        config->set("connection_probability", 0.12, ConfigSource::Default);
        
        // Development stages
        config->set("development_mode", "accelerated", ConfigSource::Default);
        config->set("plasticity_decay_rate", 0.8, ConfigSource::Default);
        config->set("critical_period_multiplier", 2.0, ConfigSource::Default);
        config->set("synaptogenesis_boost", 5.0, ConfigSource::Default);
        
        // Learning objectives
        config->set("developmental_stage", "critical_period", ConfigSource::Default);
        config->set("learning_objective", "skill_acquisition", ConfigSource::Default);
        config->set("development_tracking_enabled", true, ConfigSource::Default);
        
        return config;
    }, "Create a development-focused configuration");
    
    m.def("createExplorationConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Exploration-focused configuration
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", 1500, ConfigSource::Default);
        config->set("region_count", 3, ConfigSource::Default);
        config->set("connection_probability", 0.18, ConfigSource::Default);
        
        // Exploration characteristics
        config->set("exploration_mode", "curiosity_driven", ConfigSource::Default);
        config->set("exploration_rate", 0.4, ConfigSource::Default);
        config->set("novelty_response_multiplier", 1.2, ConfigSource::Default);
        config->set("reward_variance", 0.3, ConfigSource::Default);
        
        // Exploration objectives
        config->set("exploration_target", "environmental Mastery", ConfigSource::Default);
        config->set("exploration_metrics", "behavioral Diversity", ConfigSource::Default);
        
        return config;
    }, "Create an exploration-focused configuration");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
