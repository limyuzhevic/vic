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
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
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
        
        // Advanced Brain System Access
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory system")
        .def("getSemanticMemory", &Brain::getSemanticMemory,
             py::return_value_policy::reference_internal,
             "Get semantic memory system")
        .def("getProceduralMemory", &Brain::getProceduralMemory,
             py::return_value_policy::reference_internal,
             "Get procedural memory system")

        // Working Memory - Direct Access Methods
        .def("wmStore", &Brain::getWorkingMemory().store,
             py::arg("pattern"), py::arg("strength") = 1.0f,
             "Store pattern in working memory")
        .def("wmRetrieve", &Brain::getWorkingMemory().retrieve,
             "Retrieve working memory content")
        .def("wmContains", &Brain::getWorkingMemory().contains,
             py::arg("neuron"), "Check if neuron is in working memory")
        .def("wmClear", &Brain::getWorkingMemory().clear,
             "Clear working memory")
        .def("wmDecay", &Brain::getWorkingMemory().decay,
             py::arg("decayRate"), "Decay working memory traces")
        .def("wmUpdate", &Brain::getWorkingMemory().update,
             py::arg("dt"), "Update working memory dynamics")
        .def("wmStrengthen", &Brain::getWorkingMemory().strengthenMemory,
             py::arg("factor"), "Strengthen memory traces")
        .def("wmRunCompetition", &Brain::getWorkingMemory().runCompetition,
             "Run competition between memory traces")
        .def("wmIsWinning", &Brain::getWorkingMemory().isWinning,
             py::arg("neuron"), "Check if neuron is a winner")
        .def("wmGetActivity", &Brain::getWorkingMemory().getMemoryActivity,
             "Get overall memory activity level")
        .def("wmGetCapacity", &Brain::getWorkingMemory().getCapacity,
             "Get working memory capacity")
        .def("wmSetCapacity", &Brain::getWorkingMemory().setCapacity,
             py::arg("capacity"), "Set working memory capacity")
        .def("wmGetDecayRate", &Brain::getWorkingMemory().getDecayRate,
             "Get working memory decay rate")
        .def("wmSetDecayRate", &Brain::getWorkingMemory().setDecayRate,
             py::arg("rate"), "Set working memory decay rate")
        .def("wmGetActiveTraces", &Brain::getWorkingMemory().getActiveTraces,
             "Get number of active memory traces")
        .def("wmGetMemoryNeurons", &Brain::getWorkingMemory().getMemoryNeurons,
             "Get neurons in working memory")

        // Episodic Memory - Direct Access Methods
        .def("emStoreEpisode", &Brain::getEpisodicMemory().storeEpisode,
             py::arg("episode"), "Store an episodic memory")
        .def("emRetrieveSimilar", &Brain::getEpisodicMemory().retrieveSimilar,
             py::arg("pattern"), py::arg("maxResults") = 5,
             "Retrieve similar episodes")
        .def("emRetrieveTemporal", &Brain::getEpisodicMemory().retrieveTemporal,
             py::arg("startTime"), py::arg("endTime"), py::arg("maxResults") = 10,
             "Retrieve episodes from time window")
        .def("emRetrieveByLocation", &Brain::getEpisodicMemory().retrieveByLocation,
             py::arg("x"), py::arg("y"), py::arg("radius"), py::arg("maxResults") = 5,
             "Retrieve episodes by location")
        .def("emRetrieveAfterAction", &Brain::getEpisodicMemory().retrieveAfterAction,
             py::arg("action"), py::arg("maxResults") = 5,
             "Retrieve episodes after specific action")
        .def("emReplayEpisode", &Brain::getEpisodicMemory().replayEpisode,
             py::arg("episode"), "Replay an episode")
        .def("emUpdateRelevance", &Brain::getEpisodicMemory().updateRelevance,
             py::arg("index"), py::arg("delta"), "Update episode relevance")
        .def("emGetEpisode", &Brain::getEpisodicMemory().getEpisode,
             py::arg("index"), "Get episode by index")
        .def("emGetRecentEpisodes", &Brain::getEpisodicMemory().getRecentEpisodes,
             py::arg("count"), "Get recent episodes")
        .def("emGetAverageReward", &Brain::getEpisodicMemory().getAverageReward,
             "Get average reward from episodes")
        .def("emConsolidate", &Brain::getEpisodicMemory().consolidate,
             py::arg("threshold"), "Consolidate episodes")
        .def("emClear", &Brain::getEpisodicMemory().clear,
             "Clear episodic memory")
        .def("emEnableReplay", &Brain::getEpisodicMemory().enableReplay,
             py::arg("enable"), "Enable/disable replay")
        .def("emIsReplayEnabled", &Brain::getEpisodicMemory().isReplayEnabled,
             "Check if replay is enabled")
        .def("emGetEpisodesForReplay", &Brain::getEpisodicMemory().getEpisodesForReplay,
             py::arg("count"), "Get episodes for replay")
        .def("emReplaySequence", &Brain::getEpisodicMemory().replaySequence,
             py::arg("episodeIds"), "Replay episode sequence")

        // Semantic Memory - Direct Access Methods
        .def("smStoreFact", &Brain::getSemanticMemory().storeFact,
             py::arg("key"), py::arg("value"), "Store a semantic fact")
        .def("smRetrieveFact", &Brain::getSemanticMemory().retrieveFact,
             py::arg("key"), "Retrieve semantic fact")
        .def("smHasFact", &Brain::getSemanticMemory().hasFact,
             py::arg("key"), "Check if semantic fact exists")
        .def("smGetAllFacts", &Brain::getSemanticMemory().getAllFacts,
             "Get all semantic facts")
        .def("smClear", &Brain::getSemanticMemory().clear,
             "Clear semantic memory")

        // Procedural Memory - Direct Access Methods
        .def("pmLearnSkill", &Brain::getProceduralMemory().learnSkill,
             py::arg("name"), py::arg("pattern"), "Learn a new skill")
        .def("pmGetSkill", &Brain::getProceduralMemory().getSkill,
             py::arg("name"), "Get procedural skill")
        .def("pmGetAllSkills", &Brain::getProceduralMemory().getAllSkills,
             "Get all procedural skills")
        .def("pmUpdateProficiency", &Brain::getProceduralMemory().updateProficiency,
             py::arg("name"), py::arg("delta"), "Update skill proficiency")
        .def("pmClear", &Brain::getProceduralMemory().clear,
             "Clear procedural memory")

        // Associative Memory - Direct Access Methods
        .def("amAssociate", &Brain::getAssociativeMemory().associate,
             py::arg("patternA"), py::arg("patternB"), py::arg("strength") = 1.0f,
             "Create association between patterns")
        .def("amAssociateFromExperience", &Brain::getAssociativeMemory().associateFromExperience,
             py::arg("episode"), "Create association from episode")
        .def("amRetrieve", &Brain::getAssociativeMemory().retrieve,
             py::arg("pattern"), py::arg("maxResults") = 5,
             "Retrieve associated patterns")
        .def("amGetAssociationStrength", &Brain::getAssociativeMemory().getAssociationStrength,
             py::arg("patternA"), py::arg("patternB"), "Get association strength")
        .def("amUpdateAssociation", &Brain::getAssociativeMemory().updateAssociation,
             py::arg("patternA"), py::arg("patternB"), py::arg("delta"),
             "Update association based on outcome")
        .def("amSpreadActivation", &Brain::getAssociativeMemory().spreadActivation,
             py::arg("cuePattern"), py::arg("steps") = 2,
             "Spread activation through associations")
        .def("amClear", &Brain::getAssociativeMemory().clear,
             "Clear associative memory")
        .def("amGetAssociationCount", &Brain::getAssociativeMemory().getAssociationCount,
             "Get number of associations")
        
        // Prediction System - Direct Access Methods
        .def("predictNextState", &Brain::getPredictionSystem().predictNextState,
             py::arg("currentState"), "Predict next state from current state")
        .def("updatePredictions", &Brain::getPredictionSystem().updatePredictions,
             py::arg("predicted"), py::arg("actual"), "Update predictions with actual observation")
        .def("getPredictionError", &Brain::getPredictionSystem().getPredictionError,
             "Get prediction error")
        .def("getPredictionConfidence", &Brain::getPredictionSystem().getConfidence,
             "Get prediction confidence")
        .def("getPredictionErrorHistory", &Brain::getPredictionSystem().getErrorHistory,
             "Get prediction error history")
        .def("clearPredictionHistory", &Brain::getPredictionSystem().clearHistory,
             "Clear prediction history")
        .def("trainPrediction", &Brain::getPredictionSystem().train,
             py::arg("observation"), "Train prediction model with observation")
        
        // Cognition Systems - Direct Access Methods
        // Neural Planner
        .def("planAction", &Brain::getPlanner().planAction,
             py::arg("currentState"), py::arg("targetReward") = 0.5f,
             "Plan next action given current state and goal")
        .def("evaluateSequence", &Brain::getPlanner().evaluateSequence,
             py::arg("actions"), py::arg("startState"),
             "Evaluate a potential action sequence")
        .def("setPlanningDepth", &Brain::getPlanner().setPlanningDepth,
             py::arg("depth"), "Set planning depth")
        .def("getPlanningDepth", &Brain::getPlanner().getPlanningDepth,
             "Get planning depth")
        .def("getPlanningConfidence", &Brain::getPlanner().getPlanningConfidence,
             "Get planning confidence")
        .def("updatePlanQuality", &Brain::getPlanner().updatePlanQuality,
             py::arg("plannedActions"), py::arg("actualActions"), py::arg("actualReward"),
             "Update plans based on actual outcome")
        .def("clearPlanningCache", &Brain::getPlanner().clearCache,
             "Clear planning cache")
        .def("setActionQuality", &Brain::getPlanner().setActionQuality,
             py::arg("action"), py::arg("quality"), "Set action quality from experience")
        .def("getCurrentGoal", &Brain::getPlanner().getCurrentGoal,
             "Get current goal")
        .def("setCurrentGoal", &Brain::getPlanner().setCurrentGoal,
             py::arg("goal"), "Set current goal")
        .def("wasRecentPlanSuccessful", &Brain::getPlanner().wasRecentPlanSuccessful,
             "Check if recent planning was successful")

        // Concept Formation
        .def("learnConcept", &Brain::getConceptFormation().presentExperience,
             py::arg("pattern"), py::arg("features"), py::arg("reward"), py::arg("currentTime"),
             "Present a new experience to concept formation system")
        .def("retrieveConcept", &Brain::getConceptFormation().getMatchingConcept,
             py::arg("pattern"), py::arg("similarityThreshold") = 0.7f,
             "Get the concept that best matches a pattern")
        .def("getConcepts", &Brain::getConceptFormation().getConcepts,
             "Get all concepts discovered")
        .def("getConcept", &Brain::getConceptFormation().getConcept,
             py::arg("conceptId"), "Get specific concept")
        .def("getConceptPrototype", &Brain::getConceptFormation().getConceptPrototype,
             py::arg("conceptId"), "Get concept prototype")
        .def("getConceptInstances", &Brain::getConceptFormation().getConceptInstances,
             py::arg("conceptId"), "Get concept instances")
        .def("getConceptStability", &Brain::getConceptFormation().getConceptStability,
             py::arg("conceptId"), "Get concept stability")
        .def("getConceptCount", &Brain::getConceptFormation().getConceptCount,
             "Get number of concepts discovered")
        .def("mergeConcepts", &Brain::getConceptFormation().mergeConcepts,
             py::arg("conceptA"), py::arg("conceptB"), "Merge two concepts")
        .def("updateConcept", &Brain::getConceptFormation().updateConcept,
             py::arg("conceptId"), py::arg("newPattern"), py::arg("features"), py::arg("reward"),
             "Update concept with new instance")
        .def("computeSimilarity", &Brain::getConceptFormation().computeSimilarity,
             py::arg("a"), py::arg("b"), "Compute similarity between patterns")
        .def("isNovel", &Brain::getConceptFormation().isNovel,
             py::arg("pattern"), py::arg("similarityThreshold") = 0.7f,
             "Check if pattern is novel")
        .def("findConceptForPattern", &Brain::getConceptFormation().findConceptForPattern,
             py::arg("pattern"), "Get concept for given pattern")
        .def("clearConcepts", &Brain::getConceptFormation().clear,
             "Clear all concepts")
        .def("getGeneralizationAbility", &Brain::getConceptFormation().getGeneralizationAbility,
             py::arg("conceptId"), "Get concept generalization ability")
        .def("setFormationThreshold", &Brain::getConceptFormation().setFormationThreshold,
             py::arg("threshold"), "Set concept formation threshold")
        .def("setStabilityWindow", &Brain::getConceptFormation().setStabilityWindow,
             py::arg("window"), "Set stability window")

        // Attention System
        .def("focusAttention", &Brain::getAttention().focusOnRegion,
             py::arg("region"), "Focus attention on specific region")
        .def("getAttentionalFocus", &Brain::getAttention().getAttendedRegions,
             "Get current attentional focus")
        .def("releaseAttention", &Brain::getAttention().releaseAttention,
             "Release attention (allow all regions equal processing)")
        .def("setInhibitionStrength", &Brain::getAttention().setInhibitionStrength,
             py::arg("strength"), "Set inhibition strength")
        .def("setExcitationStrength", &Brain::getAttention().setExcitationStrength,
             py::arg("strength"), "Set excitation strength")
        .def("setCompetitionThreshold", &Brain::getAttention().setCompetitionThreshold,
             py::arg("threshold"), "Set competition threshold")
        .def("getInhibitionFor", &Brain::getAttention().getInhibitionFor,
             py::arg("neuron"), "Get inhibition level for neuron")
        .def("getExcitationFor", &Brain::getAttention().getExcitationFor,
             py::arg("neuron"), "Get excitation level for neuron")
        .def("updateAttentionWeights", &Brain::getAttention().update,
             py::arg("dt"), "Update attention dynamics")
        .def("isAttended", &Brain::getAttention().isAttended,
             py::arg("neuron"), "Check if neuron is currently attended")
        .def("getAttentionWinners", &Brain::getAttention().getWinners,
             "Get attention winners")
        .def("applyTopDownBias", &Brain::getAttention().applyTopDownBias,
             py::arg("neuron"), py::arg("biasStrength"), "Apply top-down attention bias")
        .def("applyBottomUpSalience", &Brain::getAttention().applyBottomUpSalience,
             py::arg("neuron"), py::arg("salienceStrength"), "Apply bottom-up attention salience")
        .def("resetAttention", &Brain::getAttention().reset,
             "Reset attention")
        
        // Neuromodulation Systems - Direct Access Methods
        .def("dopamineSignalReward", &Brain::getDopamine().signalReward,
             py::arg("reward"), "Signal reward to dopamine system")
        .def("dopamineSignalPredictionError", &Brain::getDopamine().signalRewardPredictionError,
             py::arg("error"), "Signal prediction error to dopamine system")
        .def("dopamineGetLevel", &Brain::getDopamine().getLevel,
             "Get dopamine level")
        .def("dopamineSetLevel", &Brain::getDopamine().setLevel,
             py::arg("level"), "Set dopamine level")
        .def("dopamineGetPlasticityFactor", &Brain::getDopamine().getPlasticityFactor,
             "Get dopamine plasticity factor")
        .def("curiosityGetLevel", &Brain::getCuriosity().getLevel,
             "Get curiosity level")
        .def("curiositySetLevel", &Brain::getCuriosity().setLevel,
             py::arg("level"), "Set curiosity level")
        .def("curiosityGetPlasticityFactor", &Brain::getCuriosity().getPlasticityFactor,
             "Get curiosity plasticity factor")
        .def("noveltyGetLevel", &Brain::getNovelty().getLevel,
             "Get novelty level")
        .def("noveltySetLevel", &Brain::getNovelty().setLevel,
             py::arg("level"), "Set novelty level")
        .def("noveltyGetPlasticityFactor", &Brain::getNovelty().getPlasticityFactor,
             "Get novelty plasticity factor")
        .def("predictionErrorGetLevel", &Brain::getPredictionErrorSignal().getLevel,
             "Get prediction error signal level")
        .def("predictionErrorSetLevel", &Brain::getPredictionErrorSignal().setLevel,
             py::arg("level"), "Set prediction error signal level")
        .def("predictionErrorGetPlasticityFactor", &Brain::getPredictionErrorSignal().getPlasticityFactor,
             "Get prediction error signal plasticity factor")
        
        // Development System - Direct Access Methods
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("develop", &Brain::develop,
             "Apply developmental changes")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        .def("developmentGetStage", &Brain::getDevelopmentSystem().getStage,
             "Get current developmental stage via system")
        .def("developmentSetStage", &Brain::getDevelopmentSystem().setStage,
             py::arg("stage"), "Set developmental stage via system")
        .def("developmentAdvanceStage", &Brain::getDevelopmentSystem().advanceStage,
             "Advance developmental stage")
        .def("developmentGetStageName", &Brain::getDevelopmentSystem().getStageName,
             "Get current stage name")
        .def("developmentUpdate", &Brain::getDevelopmentSystem().update,
             py::arg("brain"), py::arg("currentStep"), "Update development (simple)")
        .def("developmentUpdateFull", &Brain::getDevelopmentSystem().update,
             py::arg("brain"), py::arg("rng"), py::arg("dt"), "Update development with full parameters")
        .def("developmentGetPlasticityModifier", &Brain::getDevelopmentSystem().getPlasticityModifier,
             "Get plasticity modifier for current stage")
        .def("developmentIsCriticalPeriod", &Brain::getDevelopmentSystem().isCriticalPeriod,
             "Check if in critical period")
        .def("developmentGetCriticalPeriodProgress", &Brain::getDevelopmentSystem().getCriticalPeriodProgress,
             "Get critical period progress")
        .def("developmentGetDevelopmentalAge", &Brain::getDevelopmentSystem().getDevelopmentalAge,
             "Get developmental age")
        .def("developmentSetDevelopmentalAge", &Brain::getDevelopmentSystem().setDevelopmentalAge,
             py::arg("age"), "Set developmental age")
        
        // Plasticity Systems
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get STDP plasticity system")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get Hebbian plasticity system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get structural plasticity system")
        
        // Spike System (advanced access)
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get spike system")
        
        // Convenience methods for common workflows
        .def("runMultipleBrainsParallel", [](const std::vector<std::shared_ptr<Brain>>& brains,
                                             const std::vector<size_t>& steps,
                                             double timestep = 0.001) {
            py::list results;
            for (size_t i = 0; i < brains.size(); ++i) {
                if (brains[i]) {
                    py::dict brain_stats;
                    brain_stats["brain_index"] = static_cast<int>(i);
                    brain_stats["steps"] = steps[i];
                    brain_stats["timestep"] = timestep;
                    results.append(brain_stats);
                    
                    for (nlm::SimulationStep step = 0; step < static_cast<nlm::SimulationStep>(steps[i]); ++step) {
                        brains[i]->step(step, step * timestep);
                    }
                }
            }
            return results;
        }, py::arg("brains"), py::arg("steps"), py::arg("timestep") = 0.001,
              "Run multiple brains in parallel")
        
        .def("monitorSimulationRealTime", [](std::shared_ptr<Brain> brain,
                                             const std::vector<size_t>& steps,
                                             double timestep = 0.001) {
            py::list monitoring_data;
            size_t max_steps = steps.empty() ? 0 : *std::max_element(steps.begin(), steps.end());
            
            for (nlm::SimulationStep step = 0; step < static_cast<nlm::SimulationStep>(max_steps); ++step) {
                if (!steps.empty() && step < steps.size()) {
                    brain->step(step, step * timestep);
                }
                
                py::dict snapshot;
                snapshot["step"] = static_cast<int>(step);
                snapshot["time"] = step * timestep;
                snapshot["total_neurons"] = brain->getTotalNeuronCount();
                snapshot["firing_neurons"] = brain->getFiringNeuronCount();
                snapshot["total_spikes"] = brain->getTotalSpikeCount();
                snapshot["avg_firing_rate"] = brain->getAverageFiringRate();
                snapshot["e_i_ratio"] = brain->getExcitationInhibitionRatio();
                snapshot["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
                
                monitoring_data.append(snapshot);
            }
            
            return monitoring_data;
        }, py::arg("brain"), py::arg("steps"), py::arg("timestep") = 0.001,
              "Real-time simulation monitoring with comprehensive metrics")
        
        .def("detectAnomalies", [](std::shared_ptr<Brain> brain,
                                  const std::vector<size_t>& steps,
                                  double timestep = 0.001) {
            py::dict anomalies;
            py::list spike_anomalies;
            py::list firing_rate_anomalies;
            py::list connectivity_anomalies;
            
            float expected_avg_rate = 0.1f;
            float spike_growth_threshold = 5.0f;
            float connection_threshold = 0.01f;
            
            size_t max_steps = steps.empty() ? 0 : *std::max_element(steps.begin(), steps.end());
            
            float baseline_spike_rate = 0.0f;
            size_t baseline_samples = 0;
            
            for (nlm::SimulationStep step = 0; step < static_cast<nlm::SimulationStep>(max_steps); ++step) {
                if (!steps.empty() && step < steps.size()) {
                    brain->step(step, step * timestep);
                }
                
                float firing_rate = brain->getAverageFiringRate();
                size_t total_connections = brain->getTotalSynapseCount();
                float e_i_ratio = brain->getExcitationInhibitionRatio();
                
                if (step < 10) {
                    baseline_spike_rate += firing_rate;
                    baseline_samples++;
                }
                
                if (baseline_samples > 0 && firing_rate > expected_avg_rate * 10.0f) {
                    py::dict anomaly;
                    anomaly["type"] = "high_firing_rate";
                    anomaly["step"] = static_cast<int>(step);
                    anomaly["firing_rate"] = firing_rate;
                    anomaly["expected"] = expected_avg_rate;
                    firing_rate_anomalies.append(anomaly);
                }
                
                if (step > 5 && !steps.empty() && step < steps.size()) {
                    float prev_rate = brain->getAverageFiringRate();
                    if (prev_rate > spike_growth_threshold) {
                        py::dict anomaly;
                        anomaly["type"] = "sudden_spike_increase";
                        anomaly["step"] = static_cast<int>(step);
                        anomaly["firing_rate"] = firing_rate;
                        anomaly["threshold"] = spike_growth_threshold;
                        spike_anomalies.append(anomaly);
                    }
                }
                
                if (total_connections == 0) {
                    py::dict anomaly;
                    anomaly["type"] = "no_connections";
                    anomaly["step"] = static_cast<int>(step);
                    anomaly["connection_count"] = total_connections;
                    connectivity_anomalies.append(anomaly);
                }
                
                if (e_i_ratio < 0.5f || e_i_ratio > 1.5f) {
                    py::dict anomaly;
                    anomaly["type"] = "unbalanced_e_i";
                    anomaly["step"] = static_cast<int>(step);
                    anomaly["e_i_ratio"] = e_i_ratio;
                    anomaly["status"] = (e_i_ratio < 0.5f) ? "inhibitory_dominant" : "excitatory_dominant";
                    connectivity_anomalies.append(anomaly);
                }
            }
            
            anomalies["spike_anomalies"] = spike_anomalies;
            anomalies["firing_rate_anomalies"] = firing_rate_anomalies;
            anomalies["connectivity_anomalies"] = connectivity_anomalies;
            
            return anomalies;
        }, py::arg("brain"), py::arg("steps"), py::arg("timestep") = 0.001,
              "Detect various simulation anomalies and potential issues")
        
        .def("analyzeConnectivity", [](std::shared_ptr<Brain> brain) {
            py::dict analysis;
            analysis["total_neurons"] = brain->getTotalNeuronCount();
            analysis["total_synapses"] = brain->getTotalSynapseCount();
            analysis["regions"] = brain->getRegionCount();
            
            float ei_ratio = brain->getExcitationInhibitionRatio();
            analysis["e_i_ratio"] = ei_ratio;
            analysis["e_i_balance"] = (ei_ratio > 1.0f) ? "Excitatory Dominant" : 
                                    (ei_ratio < 0.9f) ? "Inhibitory Dominant" : "Balanced";
            
            analysis["avg_firing_rate"] = brain->getAverageFiringRate();
            analysis["spike_density"] = (brain->getTotalNeuronCount() > 0) ? 
                static_cast<float>(brain->getTotalSpikeCount()) / brain->getTotalNeuronCount() : 0.0f;
            analysis["network_efficiency"] = (ei_ratio > 0.5f && ei_ratio < 1.5f) ? "Good" : "Poor";
            analysis["stability_indication"] = (analysis["spike_density"] < 0.5f) ? "Stable" : "Unstable";
            
            return analysis;
        }, py::arg("brain"), "Analyze brain connectivity and provide diagnostic information")
        
        .def("getSystemHealth", [](const std::vector<std::shared_ptr<Brain>>& brains,
                                  const std::vector<std::string>& names,
                                  bool include_detailed_analysis = true) {
            py::list health_reports;
            
            for (size_t i = 0; i < brains.size(); ++i) {
                py::dict health;
                if (i < names.size()) {
                    health["name"] = names[i];
                } else {
                    health["name"] = "brain_" + std::to_string(i);
                }
                
                if (brains[i]) {
                    health["status"] = "active";
                    health["neuron_count"] = brains[i]->getTotalNeuronCount();
                    health["synapse_count"] = brains[i]->getTotalSynapseCount();
                    health["spike_count"] = brains[i]->getTotalSpikeCount();
                    health["firing_neurons"] = brains[i]->getFiringNeuronCount();
                    health["avg_firing_rate"] = brains[i]->getAverageFiringRate();
                    health["developmental_stage"] = static_cast<int>(brains[i]->getDevelopmentalStage());
                    
                    float ei_ratio = brains[i]->getExcitationInhibitionRatio();
                    health["e_i_ratio"] = ei_ratio;
                    health["e_i_status"] = (ei_ratio > 1.5f) ? "Unbalanced_Excitatory" :
                                          (ei_ratio < 0.5f) ? "Unbalanced_Inhibitory" : "Balanced";
                    
                    // Check for potential issues
                    std::vector<std::string> issues;
                    if (brains[i]->getTotalNeuronCount() == 0) issues.push_back("No_neurons");
                    if (brains[i]->getTotalSynapseCount() == 0) issues.push_back("No_connections");
                    if (brains[i]->getFiringNeuronCount() == 0 && brains[i]->getTotalSpikeCount() > 0) issues.push_back("No_firing_neurons");
                    if (ei_ratio < 0.5f || ei_ratio > 1.5f) issues.push_back("Unbalanced_EI");
                    if (brains[i]->getAverageFiringRate() < 0.01f) issues.push_back("Low_activity");
                    
                    if (!issues.empty()) {
                        health["issues"] = issues;
                        health["status"] = "warning";
                    }
                    
                    if (include_detailed_analysis) {
                        // Add detailed analysis
                        health["detailed_analysis"] = nlm::analyzeConnectivity(brains[i]);
                    }
                } else {
                    health["status"] = "error";
                    health["error"] = "Brain_pointer_is_null";
                }
                
                health_reports.append(health);
            }
            
            return health_reports;
        }, py::arg("brains"), py::arg("names"), py::arg("include_detailed_analysis") = true,
              "Comprehensive system health monitoring with detailed analysis")
        
        .def("profilePerformance", [](const std::vector<std::shared_ptr<Brain>>& brains,
                                     const std::vector<size_t>& steps,
                                     double timestep = 0.001) {
            py::dict performance_report;
            
            auto start_time = std::chrono::high_resolution_clock::now();
            
            for (size_t i = 0; i < brains.size(); ++i) {
                if (brains[i]) {
                    size_t max_steps = steps.empty() ? 0 : *std::max_element(steps.begin(), steps.end());
                    
                    for (nlm::SimulationStep step = 0; step < static_cast<nlm::SimulationStep>(max_steps); ++step) {
                        if (!steps.empty() && step < steps.size()) {
                            brains[i]->step(step, step * timestep);
                        }
                    }
                }
            }
            
            auto end_time = std::chrono::high_resolution_clock::now();
            auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
            
            performance_report["total_execution_time_ms"] = total_duration.count();
            performance_report["brains_processed"] = static_cast<int>(brains.size());
            performance_report["total_steps"] = steps.empty() ? 0 : *std::max_element(steps.begin(), steps.end());
            performance_report["timestep"] = timestep;
            
            if (total_duration.count() > 0 && !steps.empty()) {
                double steps_per_second = (steps.size() * timestep) > 0 ? 
                    static_cast<double>(steps.size()) / ((steps.size() * timestep) / 1000.0) : 0.0;
                performance_report["steps_per_second"] = steps_per_second;
            }
            
            performance_report["avg_neuron_count"] = 0;
            performance_report["avg_synapse_count"] = 0;
            performance_report["avg_spike_count"] = 0;
            
            // Collect performance metrics from each brain
            py::list brain_performance;
            for (size_t i = 0; i < brains.size(); ++i) {
                py::dict brain_perf;
                brain_perf["brain_index"] = static_cast<int>(i);
                
                if (brains[i]) {
                    brain_perf["neuron_count"] = brains[i]->getTotalNeuronCount();
                    brain_perf["synapse_count"] = brains[i]->getTotalSynapseCount();
                    brain_perf["final_spike_count"] = brains[i]->getTotalSpikeCount();
                    brain_perf["final_firing_neurons"] = brains[i]->getFiringNeuronCount();
                    brain_perf["avg_firing_rate"] = brains[i]->getAverageFiringRate();
                    brain_perf["memory_usage_kb"] = 0;  // Would need actual memory measurement
                    brain_perf["e_i_ratio"] = brains[i]->getExcitationInhibitionRatio();
                    brain_perf["developmental_stage"] = static_cast<int>(brains[i]->getDevelopmentalStage());
                }
                
                brain_performance.append(brain_perf);
            }
            
            performance_report["brain_performance"] = brain_performance;
            
            return performance_report;
        }, py::arg("brains"), py::arg("steps"), py::arg("timestep") = 0.001,
              "Comprehensive performance monitoring and profiling")
        .def("simulateEpisode", [](Brain& self, size_t steps, double timestep, 
                                  std::shared_ptr<SensoryInput> input = nullptr) {
            for (nlm::SimulationStep step = 0; step < static_cast<nlm::SimulationStep>(steps); ++step) {
                if (input) {
                    self.receiveSensoryInput(*input);
                }
                self.step(step, step * timestep);
            }
        }, py::arg("steps"), py::arg("timestep"), py::arg("input") = nullptr,
             "Simulate an episode with optional sensory input at each step");

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

    // Advanced Factory Functions
    m.def("createBasicBrain", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.05f, ConfigSource::Default);
        return std::make_shared<Brain>(config);
    }, "Create a basic configured brain (500 neurons, 1 region)");

    m.def("createAdvancedBrain", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("plasticity.structural.enable", true, ConfigSource::Default);
        return std::make_shared<Brain>(config);
    }, "Create an advanced configured brain with plasticity (2000 neurons, 3 regions)");

    m.def("createQuickTestBrain", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        return std::make_shared<Brain>(config);
    }, "Create a quick test brain (100 neurons) with fixed seed for reproducibility");

    // World Factory Functions
    m.def("createStandardWorld", []() {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->setRandomSeed(42);
        return world;
    }, "Create a standard 20x20 world with 8x8 vision");

    m.def("createLargeWorld", []() {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(50, 50, 12, 12);
        world->setRandomSeed(123);
        return world;
    }, "Create a large 50x50 world with 12x12 vision");

    // Agent Factory Functions
    m.def("createSimpleAgentBrain", [](std::shared_ptr<Brain> brain) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        return agent;
    }, py::arg("brain"), "Create a simple agent brain with default subsystems enabled");

    m.def("createAdvancedAgentBrain", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        agent->enableNovelty(true);
        agent->initialize(*world);
        return agent;
    }, py::arg("brain"), py::arg("world"), "Create an advanced agent brain with all subsystems enabled");

    // Example world configurations
    m.def("createArenaWorld", []() {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(30, 30, 10, 10);
        world->setAgentStart(15.0, 15.0);
        world->setMaxEnergy(100.0);
        return world;
    }, "Create an arena-style world with agent start position");

    m.def("createMazeWorld", []() {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(25, 25, 8, 8);
        world->setAgentStart(5.0, 5.0);
        world->setMaxEnergy(50.0);
        return world;
    }, "Create a maze-style world with limited energy");

    // Batch and utility functions
    m.def("runSimulationBatch", [](const std::vector<std::shared_ptr<Brain>>& brains, 
                                  const std::vector<size_t>& steps,
                                  double timestep = 0.001) {
        py::list results;
        for (size_t i = 0; i < brains.size(); ++i) {
            if (brains[i]) {
                py::dict brain_stats;
                brain_stats["brain_index"] = static_cast<int>(i);
                brain_stats["steps"] = steps[i];
                brain_stats["timestep"] = timestep;
                results.append(brain_stats);
                
                for (nlm::SimulationStep step = 0; step < static_cast<nlm::SimulationStep>(steps[i]); ++step) {
                    brains[i]->step(step, step * timestep);
                }
            }
        }
        return results;
    }, py::arg("brains"), py::arg("steps"), py::arg("timestep") = 0.001,
             "Run simulation batch for multiple brains");

    m.def("compareBrains", [](const std::vector<std::shared_ptr<Brain>>& brains,
                             const std::vector<size_t>& steps,
                             double timestep = 0.001) {
        py::list comparison;
        for (size_t i = 0; i < brains.size(); ++i) {
            if (brains[i]) {
                py::dict stats;
                stats["brain_index"] = static_cast<int>(i);
                stats["final_spike_count"] = brains[i]->getTotalSpikeCount();
                stats["firing_neurons"] = brains[i]->getFiringNeuronCount();
                stats["avg_firing_rate"] = brains[i]->getAverageFiringRate();
                stats["steps_run"] = steps[i];
                comparison.append(stats);
            }
        }
        return comparison;
    }, py::arg("brains"), py::arg("steps"), py::arg("timestep") = 0.001,
             "Compare performance of multiple brains after running simulation");

    // Brain network utilities
    m.def("createBrainNetwork", [](const std::vector<std::shared_ptr<Config>>& configs) {
        std::vector<std::shared_ptr<Brain>> brains;
        for (const auto& config : configs) {
            if (config) {
                brains.push_back(std::make_shared<Brain>(*config));
            }
        }
        return brains;
    }, py::arg("configs"), "Create a network of brains from multiple configurations");

    m.def("resetBrainNetwork", [](const std::vector<std::shared_ptr<Brain>>& brains) {
        for (const auto& brain : brains) {
            if (brain) {
                brain->reset();
                brain->initialize(brain->getConfig());
            }
        }
    }, py::arg("brains"), "Reset a network of brains");

    // Performance and debugging utilities
    m.def("profileBrainPerformance", [](std::shared_ptr<Brain> brain, 
                                       const std::vector<size_t>& steps,
                                       double timestep = 0.001) {
        py::dict profile;
        profile["brain_index"] = 0;  // Will be set in implementation
        profile["steps_total"] = steps.size();
        profile["timestep"] = timestep;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < steps.size(); ++i) {
            for (nlm::SimulationStep step = 0; step < static_cast<nlm::SimulationStep>(steps[i]); ++step) {
                brain->step(step, step * timestep);
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        profile["execution_time_ms"] = duration.count();
        profile["steps_per_second"] = (steps.size() * timestep) > 0 ? 
            static_cast<double>(steps.size()) / ((steps.size() * timestep) / 1000.0) : 0.0;
        
        profile["final_spike_count"] = brain->getTotalSpikeCount();
        profile["final_firing_neurons"] = brain->getFiringNeuronCount();
        profile["final_avg_firing_rate"] = brain->getAverageFiringRate();
        
        return profile;
    }, py::arg("brain"), py::arg("steps"), py::arg("timestep") = 0.001,
             "Profile brain performance and return execution statistics");

    // Serialization utilities
    m.def("saveBrainNetwork", [](const std::vector<std::shared_ptr<Brain>>& brains,
                                const std::string& basePath) {
        py::list results;
        for (size_t i = 0; i < brains.size(); ++i) {
            if (brains[i]) {
                std::string filepath = basePath + "_brain_" + std::to_string(i) + ".bin";
                bool success = brains[i]->save(filepath);
                py::dict result;
                result["brain_index"] = static_cast<int>(i);
                result["filepath"] = filepath;
                result["success"] = success;
                results.append(result);
            }
        }
        return results;
    }, py::arg("brains"), py::arg("basePath"), "Save multiple brains to files");

    m.def("loadBrainNetwork", [](const std::vector<std::string>& filepaths,
                                const std::vector<std::shared_ptr<Config>>& configs) {
        std::vector<std::shared_ptr<Brain>> brains;
        for (size_t i = 0; i < filepaths.size(); ++i) {
            if (configs.size() > i && configs[i]) {
                auto brain = std::make_shared<Brain>(configs[i]);
                brain->load(filepaths[i]);
                brains.push_back(brain);
            }
        }
        return brains;
    }, py::arg("filepaths"), py::arg("configs"), "Load multiple brains from files");

    // Convenience presets for common simulation scenarios
    m.def("createExplorationScenario", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(800), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.08f, ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(123), ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("neuromod.dopamine.scale", 1.0f, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        auto agent = std::make_shared<AgentBrain>(brain);
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->setAgentStart(10.0, 10.0);
        agent->initialize(*world);
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        
        py::dict scenario;
        scenario["brain"] = brain;
        scenario["agent"] = agent;
        scenario["world"] = world;
        return scenario;
    }, "Create an exploration scenario with curiosity and reward modulation");

    m.def("createLearningScenario", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(1200), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
        config->set("connection_probability", 0.12f, ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(456), ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("plasticity.structural.enable", true, ConfigSource::Default);
        config->set("neuromod.dopamine.scale", 2.0f, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        auto agent = std::make_shared<AgentBrain>(brain);
        auto world = std::make_shared<SimpleWorld>();
        world->configure(30, 30, 10, 10);
        world->setAgentStart(15.0, 15.0);
        agent->initialize(*world);
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        py::dict scenario;
        scenario["brain"] = brain;
        scenario["agent"] = agent;
        scenario["world"] = world;
        return scenario;
    }, "Create a learning scenario with structural plasticity and reward-based learning");

    m.def("createDevelopmentScenario", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(789), ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("development.age_rate", 1.0f, ConfigSource::Default);
        config->set("development.plasticity_modifier", 0.8f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        auto agent = std::make_shared<AgentBrain>(brain);
        auto world = std::make_shared<SimpleWorld>();
        world->configure(25, 25, 8, 8);
        world->setAgentStart(12.5, 12.5);
        agent->initialize(*world);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        py::dict scenario;
        scenario["brain"] = brain;
        scenario["agent"] = agent;
        scenario["world"] = world;
        return scenario;
    }, "Create a development scenario with age-dependent plasticity");

    // Simulation helper functions
    m.def("runEpisode", [](std::shared_ptr<Brain> brain, 
                         std::shared_ptr<AgentBrain> agent,
                         std::shared_ptr<SimpleWorld> world,
                         size_t steps,
                         double timestep = 0.1) {
        py::dict results;
        results["brain"] = brain;
        results["agent"] = agent;
        results["world"] = world;
        
        for (size_t step = 0; step < steps; ++step) {
            // Update world
            world->update(timestep);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Run brain step
            brain->step(step);
            
            // Get action
            auto action = agent->decodeMotorCommand();
            
            // Apply action
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply reward modulation
            auto internalSignals = percept.getInternal();
            float reward = internalSignals.empty() ? 0.0f : internalSignals[0];
            agent->applyRewardModulation(reward, 0.0f);
            
            // Update development
            agent->updateDevelopment(timestep);
        }
        
        results["final_spike_count"] = brain->getTotalSpikeCount();
        results["final_firing_neurons"] = brain->getFiringNeuronCount();
        results["curiosity_level"] = agent->getCuriosityLevel();
        results["novelty_level"] = agent->getNoveltyLevel();
        results["developmental_stage"] = static_cast<int>(agent->getDevelopmentalStage());
        
        return results;
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), 
             py::arg("steps"), py::arg("timestep") = 0.1,
             "Run a complete episode with agent and world interaction");

    m.def("runBatchEpisodes", [](const std::vector<py::dict>& episodes,
                                size_t stepsPerEpisode,
                                double timestep = 0.1) {
        py::list results;
        for (size_t i = 0; i < episodes.size(); ++i) {
            auto episode = episodes[i];
            auto brain = episode["brain"].cast<std::shared_ptr<Brain>>();
            auto agent = episode["agent"].cast<std::shared_ptr<AgentBrain>>();
            auto world = episode["world"].cast<std::shared_ptr<SimpleWorld>>();
            
            if (brain && agent && world) {
                py::dict episode_results = nlm::runEpisode(brain, agent, world, stepsPerEpisode, timestep);
                episode_results["episode_index"] = static_cast<int>(i);
                results.append(episode_results);
                
                // Reset for next episode
                brain->reset();
                brain->initialize(brain->getConfig());
                agent->reset();
            }
        }
        return results;
    }, py::arg("episodes"), py::arg("stepsPerEpisode"), py::arg("timestep") = 0.1,
             "Run multiple episodes in batch");

    // Diagnostic and debugging utilities
    m.def("analyzeBrainConnectivity", [](std::shared_ptr<Brain> brain) {
        py::dict analysis;
        analysis["total_neurons"] = brain->getTotalNeuronCount();
        analysis["total_synapses"] = brain->getTotalSynapseCount();
        analysis["regions"] = brain->getRegionCount();
        
        float ei_ratio = brain->getExcitationInhibitionRatio();
        analysis["e_i_ratio"] = ei_ratio;
        analysis["e_i_balance"] = (ei_ratio > 1.0f) ? "Excitatory Dominant" : 
                                (ei_ratio < 0.9f) ? "Inhibitory Dominant" : "Balanced";
        
        analysis["avg_firing_rate"] = brain->getAverageFiringRate();
        analysis["spike_density"] = (brain->getTotalNeuronCount() > 0) ? 
            static_cast<float>(brain->getTotalSpikeCount()) / brain->getTotalNeuronCount() : 0.0f;
        
        return analysis;
    }, py::arg("brain"), "Analyze brain connectivity and provide diagnostic information");

    m.def("monitorSimulation", [](std::shared_ptr<Brain> brain,
                                  const std::vector<size_t>& steps,
                                  double timestep = 0.001) {
        py::list monitoring_data;
        size_t max_steps = 0;
        if (!steps.empty()) {
            max_steps = *std::max_element(steps.begin(), steps.end());
        }
        
        for (nlm::SimulationStep step = 0; step < static_cast<nlm::SimulationStep>(max_steps); ++step) {
            if (step < steps.size()) {
                brain->step(step, step * timestep);
            }
            
            py::dict snapshot;
            snapshot["step"] = static_cast<int>(step);
            snapshot["time"] = step * timestep;
            snapshot["spike_count"] = brain->getTotalSpikeCount();
            snapshot["firing_neurons"] = brain->getFiringNeuronCount();
            snapshot["avg_firing_rate"] = brain->getAverageFiringRate();
            snapshot["e_i_ratio"] = brain->getExcitationInhibitionRatio();
            
            monitoring_data.append(snapshot);
        }
        
        return monitoring_data;
    }, py::arg("brain"), py::arg("steps"), py::arg("timestep") = 0.001,
             "Monitor simulation progress and return time-series data");

    m.def("detectAnomalies", [](std::shared_ptr<Brain> brain,
                               const std::vector<size_t>& steps,
                               double timestep = 0.001) {
        py::dict anomalies;
        py::list spike_anomalies;
        py::list firing_rate_anomalies;
        
        float expected_avg_rate = 0.1f;  // Normal firing rate threshold
        float spike_growth_threshold = 5.0f;  // Sudden spike increase threshold
        
        size_t max_steps = (!steps.empty()) ? *std::max_element(steps.begin(), steps.end()) : 0;
        
        float baseline_spike_rate = 0.0f;
        size_t baseline_samples = 0;
        
        for (nlm::SimulationStep step = 0; step < static_cast<nlm::SimulationStep>(max_steps); ++step) {
            if (step < steps.size()) {
                brain->step(step, step * timestep);
            }
            
            float firing_rate = brain->getAverageFiringRate();
            
            if (step < 10) {
                baseline_spike_rate += firing_rate;
                baseline_samples++;
            }
            
            // Detect firing rate anomalies
            if (baseline_samples > 0 && firing_rate > expected_avg_rate * 10.0f) {
                py::dict anomaly;
                anomaly["type"] = "high_firing_rate";
                anomaly["step"] = static_cast<int>(step);
                anomaly["firing_rate"] = firing_rate;
                anomaly["expected"] = expected_avg_rate;
                firing_rate_anomalies.append(anomaly);
            }
            
            // Detect sudden spike increases
            if (step > 5 && step < steps.size()) {
                float prev_rate = brain->getAverageFiringRate();
                if (prev_rate > spike_growth_threshold) {
                    py::dict anomaly;
                    anomaly["type"] = "sudden_spike_increase";
                    anomaly["step"] = static_cast<int>(step);
                    anomaly["firing_rate"] = firing_rate;
                    anomaly["threshold"] = spike_growth_threshold;
                    spike_anomalies.append(anomaly);
                }
            }
        }
        
        anomalies["spike_anomalies"] = spike_anomalies;
        anomalies["firing_rate_anomalies"] = firing_rate_anomalies;
        
        return anomalies;
    }, py::arg("brain"), py::arg("steps"), py::arg("timestep") = 0.001,
             "Detect simulation anomalies and potential issues");

    // System status and health monitoring
    m.def("getSystemHealth", [](const std::vector<std::shared_ptr<Brain>>& brains,
                              const std::vector<std::string>& names) {
        py::list health_reports;
        
        for (size_t i = 0; i < brains.size(); ++i) {
            py::dict health;
            if (i < names.size()) {
                health["name"] = names[i];
            } else {
                health["name"] = "brain_" + std::to_string(i);
            }
            
            if (brains[i]) {
                health["status"] = "active";
                health["neuron_count"] = brains[i]->getTotalNeuronCount();
                health["synapse_count"] = brains[i]->getTotalSynapseCount();
                health["spike_count"] = brains[i]->getTotalSpikeCount();
                health["firing_neurons"] = brains[i]->getFiringNeuronCount();
                health["avg_firing_rate"] = brains[i]->getAverageFiringRate();
                health["developmental_stage"] = static_cast<int>(brains[i]->getDevelopmentalStage());
                
                float ei_ratio = brains[i]->getExcitationInhibitionRatio();
                health["e_i_ratio"] = ei_ratio;
                health["e_i_status"] = (ei_ratio > 1.5f) ? "Unbalanced_Excitatory" :
                                      (ei_ratio < 0.5f) ? "Unbalanced_Inhibitory" : "Balanced";
                
                // Check for potential issues
                std::vector<std::string> issues;
                if (brains[i]->getTotalNeuronCount() == 0) issues.push_back("No_neurons");
                if (brains[i]->getTotalSynapseCount() == 0) issues.push_back("No_connections");
                if (brains[i]->getFiringNeuronCount() == 0 && brains[i]->getTotalSpikeCount() > 0) issues.push_back("No_firing_neurons");
                if (ei_ratio < 0.5f || ei_ratio > 1.5f) issues.push_back("Unbalanced_EI");
                
                if (!issues.empty()) {
                    health["issues"] = issues;
                    health["status"] = "warning";
                }
            } else {
                health["status"] = "error";
                health["error"] = "Brain_pointer_is_null";
            }
            
            health_reports.append(health);
        }
        
        return health_reports;
    }, py::arg("brains"), py::arg("names"), "Get health status of multiple brains");

    m.def("monitorPerformance", [](const std::vector<std::shared_ptr<Brain>>& brains,
                                   const std::vector<size_t>& steps,
                                   double timestep = 0.001) {
        py::dict performance_report;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < brains.size(); ++i) {
            if (brains[i]) {
                size_t max_steps = (!steps.empty()) ? *std::max_element(steps.begin(), steps.end()) : 0;
                
                for (nlm::SimulationStep step = 0; step < static_cast<nlm::SimulationStep>(max_steps); ++step) {
                    if (step < steps.size()) {
                        brains[i]->step(step, step * timestep);
                    }
                }
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        performance_report["total_execution_time_ms"] = total_duration.count();
        performance_report["brains_processed"] = static_cast<int>(brains.size());
        performance_report["total_steps"] = steps.empty() ? 0 : *std::max_element(steps.begin(), steps.end());
        performance_report["timestep"] = timestep;
        
        if (total_duration.count() > 0 && !steps.empty()) {
            double steps_per_second = (steps.size() * timestep) > 0 ? 
                static_cast<double>(steps.size()) / ((steps.size() * timestep) / 1000.0) : 0.0;
            performance_report["steps_per_second"] = steps_per_second;
        }
        
        // Collect performance metrics from each brain
        py::list brain_performance;
        for (size_t i = 0; i < brains.size(); ++i) {
            py::dict brain_perf;
            brain_perf["brain_index"] = static_cast<int>(i);
            
            if (brains[i]) {
                brain_perf["neuron_count"] = brains[i]->getTotalNeuronCount();
                brain_perf["synapse_count"] = brains[i]->getTotalSynapseCount();
                brain_perf["final_spike_count"] = brains[i]->getTotalSpikeCount();
                brain_perf["final_firing_neurons"] = brains[i]->getFiringNeuronCount();
                brain_perf["avg_firing_rate"] = brains[i]->getAverageFiringRate();
                brain_perf["memory_usage_kb"] = 0;  // Would need actual memory measurement
            }
            
            brain_performance.append(brain_perf);
        }
        
        performance_report["brain_performance"] = brain_performance;
        
        return performance_report;
    }, py::arg("brains"), py::arg("steps"), py::arg("timestep") = 0.001,
             "Monitor and report performance metrics for brain simulations");

    // Add version and metadata
    m.attr("__version__") = "2.0.0";
    m.attr("__description__") = "NLM Python Bindings - Advanced Neural Simulation Framework";
    m.attr("__author__") = "NLM Research Team";
    m.attr("__license__") = "MIT";
    
    m.def("getInfo", []() {
        py::dict info;
        info["version"] = "2.0.0";
        info["description"] = "NLM Python Bindings - Advanced Neural Simulation Framework";
        info["author"] = "NLM Research Team";
        info["license"] = "MIT";
        info["python_version"] = PY_VERSION;
        info["built_with"] = "pybind11 " PYBIND11_VERSION;
        return info;
    }, "Get library information and version details");

} // namespace nlm
