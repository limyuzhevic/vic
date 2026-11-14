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

    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            int argc = static_cast<int>(args.size());
            
            // Use stack-allocated buffer for small arrays (common case)
            if (argc <= 64) {
                char* argv_stack[64];
                for (size_t i = 0; i < args.size(); ++i) {
                    argv_stack[i] = const_cast<char*>(args[i].c_str());
                }
                return self.loadFromArgs(argc, argv_stack);
            }
            
            // Fallback to heap allocation for larger arrays
            std::vector<char*> argv_heap;
            argv_heap.reserve(args.size());
            for (const auto& a : args) argv_heap.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(argc, argv_heap.data());
        }, py::arg("args"),
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

    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Neural working memory system)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("store", static_cast<void (NeuralWorkingMemory::*)(const std::vector<float>&, float)>(&NeuralWorkingMemory::store),
             py::arg("pattern"), py::arg("strength") = 1.0f,
             "Store information in working memory")
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron,
             py::arg("neuron"), py::arg("activation"),
             "Store specific neuron activation")
        .def("retrieve", &NeuralWorkingMemory::retrieve,
             "Retrieve current working memory content as activity levels")
        .def("contains", &NeuralWorkingMemory::contains,
             py::arg("neuron"),
             "Check if specific neuron is part of working memory")
        .def("getNeuronActivation", &NeuralWorkingMemory::getNeuronActivation,
             py::arg("neuron"),
             "Get activation level of a specific neuron in working memory")
        .def("update", &NeuralWorkingMemory::update,
             py::arg("dt"),
             "Update working memory (maintenance and decay)")
        .def("clear", &NeuralWorkingMemory::clear,
             "Clear working memory")
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces,
             "Get number of active memory traces")
        .def("getMemoryNeuronCount", &NeuralWorkingMemory::getMemoryNeuronCount,
             "Get number of neurons in working memory")
        .def("getMemoryActivations", &NeuralWorkingMemory::getMemoryActivations,
             "Get memory activations")
        .def("getMemoryTimestamps", &NeuralWorkingMemory::getMemoryTimestamps,
             "Get memory timestamps")
        .def("restoreSnapshot", &NeuralWorkingMemory::restoreSnapshot,
             py::arg("neurons"), py::arg("activations"), py::arg("timestamps"),
             "Restore snapshot of working memory")
        .def("getCapacity", &NeuralWorkingMemory::getCapacity,
             "Get capacity")
        .def("setCapacity", &NeuralWorkingMemory::setCapacity,
             py::arg("cap"),
             "Set capacity")
        .def("getDecayRate", &NeuralWorkingMemory::getDecayRate,
             "Get decay rate")
        .def("setDecayRate", &NeuralWorkingMemory::setDecayRate,
             py::arg("rate"),
             "Set decay rate")
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons,
             "Get neurons in working memory")
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

    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Neural episodic memory system)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralEpisodicMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode,
             py::arg("episode"),
             "Store a new experience")
        .def("retrieveSimilar", &NeuralEpisodicMemory::retrieveSimilar,
             py::arg("sensoryPattern"), py::arg("maxResults") = 5,
             "Retrieve episodes similar to a query pattern")
        .def("retrieveTemporal", &NeuralEpisodicMemory::retrieveTemporal,
             py::arg("startTime"), py::arg("endTime"), py::arg("maxResults") = 10,
             "Retrieve episodes from a specific time window")
        .def("retrieveByLocation", &NeuralEpisodicMemory::retrieveByLocation,
             py::arg("x"), py::arg("y"), py::arg("radius"), py::arg("maxResults") = 5,
             "Retrieve episodes by location")
        .def("retrieveAfterAction", &NeuralEpisodicMemory::retrieveAfterAction,
             py::arg("action"), py::arg("maxResults") = 5,
             "Retrieve episodes following a specific action")
        .def("replayEpisode", &NeuralEpisodicMemory::replayEpisode,
             py::arg("episode"),
             "Replay an episode")
        .def("updateRelevance", &NeuralEpisodicMemory::updateRelevance,
             py::arg("episodeId"), py::arg("relevanceDelta"),
             "Update episode relevance")
        .def("getEpisode", &NeuralEpisodicMemory::getEpisode,
             py::arg("index"),
             "Get episode by index")
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount,
             "Get total number of episodes")
        .def("getEpisodes", &NeuralEpisodicMemory::getEpisodes,
             "Get all episodes")
        .def("restoreEpisodes", &NeuralEpisodicMemory::restoreEpisodes,
             py::arg("episodes"),
             "Restore episodes from snapshot")
        .def("setMaxEpisodes", &NeuralEpisodicMemory::setMaxEpisodes,
             py::arg("n"),
             "Set maximum number of episodes")
        .def("getRecentEpisodes", &NeuralEpisodicMemory::getRecentEpisodes,
             py::arg("count"),
             "Get recent episodes")
        .def("getAverageReward", &NeuralEpisodicMemory::getAverageReward,
             "Get average reward from episodes")
        .def("consolidate", &NeuralEpisodicMemory::consolidate,
             py::arg("relevanceThreshold"),
             "Consolidate episodes")
        .def("clear", &NeuralEpisodicMemory::clear,
             "Clear all episodes")
        .def("enableReplay", &NeuralEpisodicMemory::enableReplay,
             py::arg("enable"),
             "Enable/disable replay")
        .def("isReplayEnabled", &NeuralEpisodicMemory::isReplayEnabled,
             "Check if replay is enabled")
        .def("getEpisodesForReplay", &NeuralEpisodicMemory::getEpisodesForReplay,
             py::arg("count"),
             "Get episodes for replay")
        .def("replaySequence", &NeuralEpisodicMemory::replaySequence,
             py::arg("episodeIds"),
             "Replay multiple episodes");

    py::class_<NeuralAssociativeMemory>(m, "NeuralAssociativeMemory", R"pbdoc(Neural associative memory system)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralAssociativeMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("associate", static_cast<void (NeuralAssociativeMemory::*)(const std::vector<float>&, const std::vector<float>&, float)>(&NeuralAssociativeMemory::associate),
             py::arg("patternA"), py::arg("patternB"), py::arg("strength") = 1.0f,
             "Create association between two neural patterns")
        .def("associateFromExperience", &NeuralAssociativeMemory::associateFromExperience,
             py::arg("episode"),
             "Create association based on experience")
        .def("retrieve", &NeuralAssociativeMemory::retrieve,
             py::arg("queryPattern"), py::arg("maxResults") = 5,
             "Retrieve patterns associated with a query pattern")
        .def("getAssociationStrength", &NeuralAssociativeMemory::getAssociationStrength,
             py::arg("patternA"), py::arg("patternB"),
             "Get association strength between two patterns")
        .def("updateAssociation", &NeuralAssociativeMemory::updateAssociation,
             py::arg("patternA"), py::arg("patternB"), py::arg("delta"),
             "Update association based on outcome")
        .def("spreadActivation", &NeuralAssociativeMemory::spreadActivation,
             py::arg("cuePattern"), py::arg("steps") = 2,
             "Spread activation from cue pattern")
        .def("clear", &NeuralAssociativeMemory::clear,
             "Clear associations")
        .def("getAssociationCount", &NeuralAssociativeMemory::getAssociationCount,
             "Get number of associations");

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

    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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

    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
        // getRegion is const/non-const overloaded; bind the mutable one.
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"),
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
        
        // ========== DEVELOPMENT SYSTEM ==========
        // Add neuromodulation system getters to Brain class
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get the dopamine system")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get the curiosity system")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get the novelty detection system")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get the prediction error signal");

    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(Base neuromodulator class)pbdoc")
        .def("getName", &Neuromodulator::getName,
             "Get modulator name")
        .def("getLevel", &Neuromodulator::getLevel,
             "Get current concentration/level")
        .def("setLevel", &Neuromodulator::setLevel,
             py::arg("level"),
             "Set current concentration/level")
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor,
             "Get plasticity factor")
        .def("update", &Neuromodulator::update,
             py::arg("dt"),
             "Update neuromodulator state");

    py::class_<Dopamine>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator)pbdoc")
        .def(py::init<>())
        .def("getName", &Dopamine::getName,
             "Get modulator name")
        .def("getLevel", &Dopamine::getLevel,
             "Get current level")
        .def("setLevel", &Dopamine::setLevel,
             py::arg("level"),
             "Set current level")
        .def("getPlasticityFactor", &Dopamine::getPlasticityFactor,
             "Get plasticity factor")
        .def("update", &Dopamine::update,
             py::arg("dt"),
             "Update neuromodulator state")
        .def("signalReward", &Dopamine::signalReward,
             py::arg("reward"),
             "Signal reward")
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError,
             py::arg("error"),
             "Signal reward prediction error");

    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity neuromodulator)pbdoc")
        .def(py::init<>())
        .def("initialize", &Curiosity::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("getLevel", &Curiosity::getLevel,
             "Get curiosity level")
        .def("update", &Curiosity::update,
             py::arg("novelty"), py::arg("predictionError"), py::arg("dt"),
             "Update curiosity based on novelty and prediction error")
        .def("getExplorationDrive", &Curiosity::getExplorationDrive,
             "Get exploration drive")
        .def("setNoveltyWeight", &Curiosity::setNoveltyWeight,
             py::arg("weight"),
             "Set novelty weight")
        .def("setPredictionErrorWeight", &Curiosity::setPredictionErrorWeight,
             py::arg("weight"),
             "Set prediction error weight")
        .def("reset", &Curiosity::reset,
             "Reset curiosity");

    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty detection)pbdoc")
        .def(py::init<>())
        .def("initialize", &Novelty::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("getLevel", &Novelty::getLevel,
             "Get novelty level")
        .def("setLevel", &Novelty::setLevel,
             py::arg("level"),
             "Set novelty level")
        .def("detectNovelty", static_cast<void (Novelty::*)(const std::vector<float>&, const std::vector<float>&)>(&Novelty::detectNovelty),
             py::arg("currentPattern"), py::arg("previousPattern"),
             "Detect novelty from sensory patterns")
        .def("update", &Novelty::update,
             py::arg("dt"),
             "Decay novelty over time")
        .def("getHistory", &Novelty::getHistory,
             "Get novelty history")
        .def("clearHistory", &Novelty::clearHistory,
             "Clear novelty history");

    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error signal)pbdoc")
        .def(py::init<>())
        .def("initialize", &PredictionError::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("getError", &PredictionError::getError,
             "Get error value")
        .def("computeError", &PredictionError::computeError,
             py::arg("predicted"), py::arg("actual"),
             "Compute prediction error")
        .def("updatePrediction", &PredictionError::updatePrediction,
             py::arg("newPrediction"),
             "Update prediction")
        .def("getHistory", &PredictionError::getHistory,
             "Get error history")
        .def("clearHistory", &PredictionError::clearHistory,
             "Clear error history")
        .def("getMagnitude", &PredictionError::getMagnitude,
             "Get error magnitude");

    py::class_<Reward>(m, "Reward", R"pbdoc(Reward signal)pbdoc")
        .def(py::init<>())
        .def("getValue", &Reward::getValue,
             "Get current reward value")
        .def("setValue", &Reward::setValue,
             py::arg("value"),
             "Set current reward value")
        .def("add", &Reward::add,
             py::arg("delta"),
             "Accumulate reward")
        .def("reset", &Reward::reset,
             "Reset accumulated reward")
        .def("getHistory", &Reward::getHistory,
             "Get reward history")
        .def("clearHistory", &Reward::clearHistory,
             "Clear reward history");

    // Add acetylcholine, norepinephrine, and serotonin as simple stubs
    // (they're implemented as simple placeholder classes in the code)
    // 
    // Note: These classes are minimal implementations for Phase 6
    // and provide basic stub implementations for integration completeness.
        .def(py::init<>())
        .def("getStage", &DevelopmentSystem::getStage,
             "Get current developmental stage")
        .def("setStage", &DevelopmentSystem::setStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("advanceStage", &DevelopmentSystem::advanceStage,
             "Advance developmental stage")
        .def("getStageName", &DevelopmentSystem::getStageName,
             "Get stage name")
        .def("update", static_cast<void (DevelopmentSystem::*)(Brain*, SimulationStep)>(&DevelopmentSystem::update),
             py::arg("brain"), py::arg("currentStep"),
             "Update development (simple version)")
        .def("update", static_cast<void (DevelopmentSystem::*)(Brain*, RandomGenerator&, TimestepDuration)>(&DevelopmentSystem::update),
             py::arg("brain"), py::arg("rng"), py::arg("dt"),
             "Update development with full parameters")
        .def("getPlasticityModifier", &DevelopmentSystem::getPlasticityModifier,
             "Get plasticity modifier for current stage")
        .def("isCriticalPeriod", &DevelopmentSystem::isCriticalPeriod,
             "Check if in critical period")
        .def("getCriticalPeriodProgress", &DevelopmentSystem::getCriticalPeriodProgress,
             "Get critical period progress")
        .def("getDevelopmentalAge", &DevelopmentSystem::getDevelopmentalAge,
             "Get developmental age")
        .def("setDevelopmentalAge", &DevelopmentSystem::setDevelopmentalAge,
             py::arg("age"),
             "Set developmental age");

    py::class_<Synaptogenesis>(m, "Synaptogenesis", R"pbdoc(Synaptogenesis - synapse formation)pbdoc")
        .def(py::init<>())
        .def("getFormationRate", &Synaptogenesis::getFormationRate,
             "Get formation rate")
        .def("setFormationRate", &Synaptogenesis::setFormationRate,
             py::arg("rate"),
             "Set formation rate")
        .def("getTargetDensity", &Synaptogenesis::getTargetDensity,
             "Get target synapse density")
        .def("setTargetDensity", &Synaptogenesis::setTargetDensity,
             py::arg("density"),
             "Set target density")
        .def("update", &Synaptogenesis::update,
             py::arg("brain"), py::arg("rng"),
             "Update synapse formation");

    py::class_<Pruning>(m, "Pruning", R"pbdoc(Synapse pruning)pbdoc")
        .def(py::init<>())
        .def("getThreshold", &Pruning::getThreshold,
             "Get pruning threshold")
        .def("setThreshold", &Pruning::setThreshold,
             py::arg("threshold"),
             "Set pruning threshold")
        .def("getPruningRate", &Pruning::getPruningRate,
             "Get pruning rate")
        .def("setPruningRate", &Pruning::setPruningRate,
             py::arg("rate"),
             "Set pruning rate")
        .def("update", &Pruning::update,
             py::arg("brain"), py::arg("rng"),
             "Update pruning")
        .def("pruneSynapse", &Pruning::pruneSynapse,
             py::arg("synapse"),
             "Force pruning of specific synapse");

    py::class_<Maturation>(m, "Maturation", R"pbdoc(Neural maturation)pbdoc")
        .def(py::init<>())
        .def("getProgress", &Maturation::getProgress,
             "Get maturation progress")
        .def("setProgress", &Maturation::setProgress,
             py::arg("progress"),
             "Set maturation progress")
        .def("update", &Maturation::update,
             py::arg("brain"), py::arg("currentStep"),
             "Update maturation")
        .def("getMatureThreshold", &Maturation::getMatureThreshold,
             "Get mature threshold")
        .def("getMatureRestingPotential", &Maturation::getMatureRestingPotential,
             "Get mature resting potential")
        .def("getMatureTimeConstant", &Maturation::getMatureTimeConstant,
             "Get mature time constant");

    // ========== PREDICTION SYSTEM ==========
    .def("getPredictionSystem", &Brain::getPredictionSystem,
         py::return_value_policy::reference_internal,
         "Get the prediction system");

    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Prediction system)pbdoc")
        .def(py::init<>())
        .def("predictNextState", &PredictionSystem::predictNextState,
             py::arg("currentState"),
             "Make prediction for next timestep")
        .def("updatePredictions", &PredictionSystem::updatePredictions,
             py::arg("predicted"), py::arg("actual"),
             "Update predictions based on actual observation")
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

    py::class_<NeuralPrediction>(m, "NeuralPrediction", R"pbdoc(Neural prediction system)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPrediction::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("recordSensoryState", &NeuralPrediction::recordSensoryState,
             py::arg("sensoryState"), py::arg("currentStep"),
             "Record current sensory state")
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
             "Get current prediction error")
        .def("getPredictionConfidence", &NeuralPrediction::getPredictionConfidence,
             "Get prediction confidence")
        .def("predictMultipleSteps", &NeuralPrediction::predictMultipleSteps,
             py::arg("currentStep"), py::arg("numSteps"),
             "Predict multiple steps into the future")
        .def("recordAction", &NeuralPrediction::recordAction,
             py::arg("action"), py::arg("step"),
             "Record action for learning")
        .def("getErrorHistory", &NeuralPrediction::getErrorHistory,
             "Get error history")
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

    py::class_<ActionConsequencePredictor>(m, "ActionConsequencePredictor", R"pbdoc(Action consequence prediction)pbdoc")
        .def(py::init<>())
        .def("initialize", &ActionConsequencePredictor::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("recordExperience", &ActionConsequencePredictor::recordExperience,
             py::arg("action"), py::arg("beforeState"), py::arg("afterState"), py::arg("reward"),
             "Record action experience")
        .def("predictConsequence", &ActionConsequencePredictor::predictConsequence,
             py::arg("action"), py::arg("currentState"),
             "Predict consequence of an action")
        .def("getConsequenceConfidence", &ActionConsequencePredictor::getConsequenceConfidence,
             py::arg("action"), py::arg("currentState"),
             "Get prediction confidence")
        .def("updatePrediction", &ActionConsequencePredictor::updatePrediction,
             py::arg("action"), py::arg("predicted"), py::arg("actual"), py::arg("reward"),
             "Update prediction")
        .def("getActionQuality", &ActionConsequencePredictor::getActionQuality,
             py::arg("action"),
             "Get learned action quality")
        .def("clear", &ActionConsequencePredictor::clear,
             "Clear learned associations");

    py::class_<PredictionErrorSignal>(m, "PredictionErrorSignal", R"pbdoc(Prediction error signal)pbdoc")
        .def(py::init<>())
        .def("computeError", &PredictionErrorSignal::computeError,
             py::arg("predicted"), py::arg("actual"),
             "Compute prediction error")
        .def("getErrorComponents", &PredictionErrorSignal::getErrorComponents,
             "Get error components")
        .def("getModulationSignal", &PredictionErrorSignal::getModulationSignal,
             "Get neuromodulation signal strength")
        .def("recordError", &PredictionErrorSignal::recordError,
             py::arg("error"), py::arg("step"),
             "Record error in history")
        .def("getErrorHistory", &PredictionErrorSignal::getErrorHistory,
             "Get error history")
        .def("isSurprising", &PredictionErrorSignal::isSurprising,
             "Check if event is surprising")
        .def("setSurpriseThreshold", &PredictionErrorSignal::setSurpriseThreshold,
             py::arg("threshold"),
             "Set surprise threshold")
.def("getSurpriseThreshold", &PredictionErrorSignal::getSurpriseThreshold,
              "Get surprise threshold");

    // ===== PERFORMANCE SYSTEMS =====
    
    // Add performance system getters to Brain class
    .def("getMemoryPool", &Brain::getMemoryPool,
         py::return_value_policy::reference_internal,
         "Get the memory pool system")
    .def("getEventQueue", &Brain::getEventQueue,
         py::return_value_policy::reference_internal,
         "Get the event queue system")
    .def("getCheckpointSystem", &Brain::getCheckpointSystem,
         py::return_value_policy::reference_internal,
         "Get the checkpoint system");

    py::class_<SpikeRingBuffer>(m, "SpikeRingBuffer", R"pbdoc(Spike event ring buffer)pbdoc")
        .def(py::init<size_t>(), py::arg("capacity") = 16384)
        .def("push", &SpikeRingBuffer::push, py::arg("event"))
        .def("pop", &SpikeRingBuffer::pop)
        .def("empty", &SpikeRingBuffer::empty)
        .def("full", &SpikeRingBuffer::full)
        .def("size", &SpikeRingBuffer::size)
        .def("capacity", &SpikeRingBuffer::capacity)
        .def("clear", &SpikeRingBuffer::clear);

    py::class_<DelayedSpikeQueue>(m, "DelayedSpikeQueue", R"pbdoc(Delayed spike queue)pbdoc")
        .def(py::init<size_t, size_t>(), py::arg("numBuckets") = 1024, py::arg("maxDelaySteps") = 100)
        .def("schedule", &DelayedSpikeQueue::schedule,
             py::arg("sourceNeuron"), py::arg("destNeuron"), py::arg("synapseId"),
             py::arg("weight"), py::arg("isExcitatory"), py::arg("deliveryStep"))
        .def("getDueSpikes", &DelayedSpikeQueue::getDueSpikes, py::arg("currentStep"))
        .def("pendingCount", &DelayedSpikeQueue::pendingCount)
        .def("bucketSize", &DelayedSpikeQueue::bucketSize, py::arg("step"))
        .def("clear", &DelayedSpikeQueue::clear)
        .def("advanceTime", &DelayedSpikeQueue::advanceTime, py::arg("currentStep"));

    py::class_<PriorityEventQueue>(m, "PriorityEventQueue", R"pbdoc(Priority event queue)pbdoc")
        .def(py::init<>())
        .def("push", &PriorityEventQueue::push,
             py::arg("timestamp"), py::arg("priority"), py::arg("type"), py::arg("data") = nullptr)
        .def("pop", &PriorityEventQueue::pop)
        .def("getDueEvents", &PriorityEventQueue::getDueEvents, py::arg("currentTime"))
        .def("empty", &PriorityEventQueue::empty)
        .def("clear", &PriorityEventQueue::clear)
        .def("size", &PriorityEventQueue::size);

    py::class_<CheckpointManager>(m, "CheckpointManager", R"pbdoc(Checkpoint manager)pbdoc")
        .def(py::init<>())
        .def("configure", &CheckpointManager::configure,
             py::arg("checkpointDir"), py::arg("saveIntervalSteps") = 10000,
             py::arg("maxCheckpoints") = 10, py::arg("compress") = true)
        .def("setNeuronProvider", &CheckpointManager::setNeuronProvider,
             py::arg("provider"))
        .def("setSynapseProvider", &CheckpointManager::setSynapseProvider,
             py::arg("provider"))
        .def("update", &CheckpointManager::update, py::arg("currentStep"), py::arg("currentTime"))
        .def("saveImmediately", &CheckpointManager::saveImmediately, py::arg("name") = "")
        .def("load", &CheckpointManager::load, py::arg("name"))
        .def("listCheckpoints", &CheckpointManager::listCheckpoints)
        .def("pruneOldCheckpoints", &CheckpointManager::pruneOldCheckpoints, py::arg("keepCount"))
        .def("getCheckpointDir", &CheckpointManager::getCheckpointDir)
        .def("getLastCheckpointPath", &CheckpointManager::getLastCheckpointPath)
        .def("shouldSave", &CheckpointManager::shouldSave, py::arg("currentStep"));

    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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
.def("getPredictionError", &AgentBrain::getPredictionError,
              "Get prediction error")
        .def("getThoughtTrace", &AgentBrain::getThoughtTrace,
             "Get thought trace (human-readable)")
        .def("getLearnedPreference", &AgentBrain::getLearnedPreference,
             "Get learned preference trait")
        .def("encodeText", &AgentBrain::encodeText,
             py::arg("text"),
             "Encode text into neuron patterns")
        .def("getMotorGroupActivities", &AgentBrain::getMotorGroupActivities,
             "Get motor group activities with detailed statistics")
        .def("getInternalFiringRates", &AgentBrain::getInternalFiringRates,
             "Get internal neuron firing rates")
        .def("getReflexBonusCount", &AgentBrain::getReflexBonusCount,
             "Get vision-guided reflex bonus count")
        .def("getReflexSuppressCount", &AgentBrain::getReflexSuppressCount,
             "Get hazard suppression count")
        .def("getWtaPulseCount", &AgentBrain::getWtaPulseCount,
             "Get winner-take-all pulse count")
        .def("getPlannerOverrideCount", &AgentBrain::getPlannerOverrideCount,
             "Get planner override count")
        .def("getPlannerAgreementCount", &AgentBrain::getPlannerAgreementCount,
             "Get planner agreement count")
        .def("getPlannerDecisionCount", &AgentBrain::getPlannerDecisionCount,
             "Get planner decision count")
        .def("getConceptBiasCount", &AgentBrain::getConceptBiasCount,
             "Get concept bias count")
        .def("getSelfBiasCount", &AgentBrain::getSelfBiasCount,
             "Get self-bias count")
        .def("getSocialBiasCount", &AgentBrain::getSocialBiasCount,
             "Get social bias count")
        .def("getConceptCount", &AgentBrain::getConceptCount,
             "Get concept count")
        .def("getSelfModelConfidence", &AgentBrain::getSelfModelConfidence,
             "Get self-model confidence")
        .def("getSocialObservationCount", &AgentBrain::getSocialObservationCount,
             "Get social observation count")
        .def("observeOtherAgent", &AgentBrain::observeOtherAgent,
             py::arg("observed"), py::arg("beforeVision"), py::arg("afterVision"),
             "Observe another agent for social learning")
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

    // Configuration factory methods for easier Python use
    m.def("loadConfig", [](const std::string& filepath) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            throw std::runtime_error("Failed to load configuration from file: " + filepath);
        }
        return config;
    }, py::arg("filepath"), "Load configuration from a file and return Config object");

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Set optimized defaults for Phase 6 simulation
        config->set("neuron_count", static_cast<int64_t>(1000));
        config->set("region_count", static_cast<int64_t>(1));
        config->set("connection_probability", 0.1f);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("stdp_tau", 20.0f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        config->set("replay_interval", 100);
        config->set("consolidation_interval", 1000);
        config->set("simulation_timestep", 0.001);
        config->set("random_seed", static_cast<int64_t>(42));
        return config;
    }, "Create a default configuration optimized for Phase 6 simulation");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(16, 16, 16, 16);
        return world;
    }, "Create a configured simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    m.def("run_brain_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                   std::shared_ptr<AgentBrain> agent, double timestep, int steps) {
        // Run complete brain simulation
        world->reset();
        auto config = brain->getConfig();
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(*world);
        
        for (int i = 0; i < steps; ++i) {
            // Get observation
            world->update(timestep);
            SensoryPercept percept = world->getSensoryPercept();
            
            // Process sensory input
            agentBrain->processSensoryInput(percept);
            
            // Brain step
            brain->step(i, i * timestep);
            
            // Get motor command
            MotorCommand cmd = agentBrain->decodeMotorCommand();
            
            // Apply action to world
            ActionResult actionResult = world->applyMotorCommand(cmd, i * timestep);
            
            // Apply reward modulation
            agentBrain->applyRewardModulation(actionResult.reward, 0.0f);
            
            // Update development
            agentBrain->updateDevelopment(timestep);
        }
        
        return brain->getTotalSpikeCount();
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
       py::arg("timestep") = 0.1, py::arg("steps") = 100,
       "Run complete brain simulation and return spike count");

// Type conversion helpers with improved error handling
    m.def("_convert_py_to_config_value", [](py::object obj) -> ConfigValue {
        try {
            if (py::isinstance<py::int_>(obj)) {
                return obj.cast<int>();
            } else if (py::isinstance<py::float_>(obj)) {
                return obj.cast<double>();
            } else if (py::isinstance<py::bool_>(obj)) {
                return obj.cast<bool>();
            } else if (py::isinstance<py::str>(obj)) {
                return obj.cast<std::string>();
            } else if (py::isinstance<py::list>(obj)) {
                // Handle both std::vector<double> and std::vector<int>
                auto lst = obj.cast<std::vector<double>>();
                return lst;
            } else if (py::isinstance<py::tuple>(obj)) {
                auto tup = obj.cast<std::tuple<std::vector<float>>>();
                return std::get<0>(tup);
            } else {
                throw std::runtime_error("Unsupported Python type for Config value: " + 
                                       py::str(obj.getattr("__class__").attr("__name__")).cast<std::string>());
            }
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Error converting Python object to ConfigValue: ") + e.what());
        }
    });

    m.def("_config_value_to_py", [](const ConfigValue& value) -> py::object {
        try {
            return std::visit([](auto&& arg) -> py::object {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    return py::int_(arg);
                } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                    return py::float_(arg);
                } else if constexpr (std::is_same_v<T, bool>) {
                    return py::bool_(arg);
                } else if constexpr (std::is_same_v<T, std::string>) {
                    return py::str(arg);
                } else if constexpr (std::is_same_v<T, std::vector<int>> || 
                                     std::is_same_v<T, std::vector<double>> || 
                                     std::is_same_v<T, std::vector<std::string>>) {
                    return py::cast(arg);
                } else {
                    throw std::runtime_error("Unsupported ConfigValue type");
                }
            }, value);
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Error converting ConfigValue to Python object: ") + e.what());
        }
    });

    m.def("_make_sensory_input", [](const std::vector<float>& data, size_t width, size_t height) -> std::shared_ptr<SensoryInput> {
        try {
            if (data.empty()) {
                throw std::runtime_error("Cannot create vision data with empty vector");
            }
            auto vision = std::make_shared<Vision>(width, height);
            vision->setData(data);
            return vision;
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Error creating sensory input: ") + e.what());
        }
    });

    m.def("_make_action", [](ActionType type, std::vector<float> parameters) -> std::shared_ptr<Action> {
        try {
            return std::make_shared<Action>(type, parameters);
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Error creating action: ") + e.what());
        }
    });

    m.def("_make_world_object", [](float x, float y, WorldObjectType type, float value, float radius) -> std::shared_ptr<WorldObject> {
        try {
            if (radius <= 0.0f) {
                throw std::runtime_error("World object radius must be positive");
            }
            return std::make_shared<WorldObject>(x, y, type, value, radius);
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Error creating world object: ") + e.what());
        }
    });

    // Memory system helper for snapshot/restore with validation
    m.def("_working_memory_snapshot", [](std::shared_ptr<NeuralWorkingMemory> wm) -> py::tuple {
        try {
            if (!wm) {
                throw std::runtime_error("WorkingMemory is null");
            }
            return py::make_tuple(
                wm->getMemoryNeurons(),
                wm->getMemoryActivations(),
                wm->getMemoryTimestamps()
            );
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Error getting working memory snapshot: ") + e.what());
        }
    });

    m.def("_working_memory_restore", [](std::shared_ptr<NeuralWorkingMemory> wm,
                                         const std::vector<NeuronId>& neurons,
                                         const std::vector<float>& activations,
                                         const std::vector<SimulationStep>& timestamps) {
        try {
            if (!wm) {
                throw std::runtime_error("WorkingMemory is null");
            }
            if (neurons.size() != activations.size() || neurons.size() != timestamps.size()) {
                throw std::runtime_error("Neurons, activations, and timestamps vectors must have same size");
            }
            wm->restoreSnapshot(neurons, activations, timestamps);
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Error restoring working memory: ") + e.what());
        }
    });

    // Prediction system helper with validation
    m.def("_get_prediction_error_components", [](std::shared_ptr<PredictionErrorSignal> predError) {
        try {
            if (!predError) {
                throw std::runtime_error("PredictionErrorSignal is null");
            }
            auto components = predError->getErrorComponents();
            return py::make_tuple(
                components.intensityError,
                components.spatialError,
                components.temporalError,
                components.totalError
            );
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Error getting prediction error components: ") + e.what());
        }
    });

    // Development system helper
    m.def("_get_development_stages", []() {
        return std::vector<std::string>{
            "Initial", "CriticalPeriod", "Maturation", "Adult", "Aging"
        };
    });

    // Neuromodulation helper
    m.def("_get_neuromodulator_names", []() {
        return std::vector<std::string>{
            "Dopamine", "Curiosity", "Novelty", "PredictionError"
        };
    });

    // Simulation utilities helper
    m.def("_create_simulation_config", [](const std::string& name,
                                           size_t neuronCount,
                                           size_t regionCount,
                                           float connectionProb) -> std::shared_ptr<Config> {
        try {
            if (neuronCount == 0) {
                throw std::runtime_error("Neuron count must be positive");
            }
            if (regionCount == 0) {
                throw std::runtime_error("Region count must be positive");
            }
            if (connectionProb < 0.0f || connectionProb > 1.0f) {
                throw std::runtime_error("Connection probability must be between 0.0 and 1.0");
            }
            
            auto config = std::make_shared<Config>();
            config->set("neuron_count", static_cast<int64_t>(neuronCount));
            config->set("region_count", static_cast<int64_t>(regionCount));
            config->set("connection_probability", connectionProb);
            config->set("name", name);
            return config;
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Error creating simulation config: ") + e.what());
        }
    });

    // Configuration loading with error handling
    m.def("loadConfig", [](const std::string& filepath) -> std::shared_ptr<Config> {
        try {
            auto config = std::make_shared<Config>();
            if (!config->loadFromFile(filepath)) {
                throw std::runtime_error("Failed to load configuration from file: " + filepath);
            }
            return config;
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Error loading configuration: ") + e.what());
        }
    }, py::arg("filepath"), "Load configuration from a file and return Config object");

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        try {
            auto config = std::make_shared<Config>();
            // Set optimized defaults for Phase 6 simulation
            config->set("neuron_count", static_cast<int64_t>(1000));
            config->set("region_count", static_cast<int64_t>(1));
            config->set("connection_probability", 0.1f);
            config->set("stdp_ltp_weight", 0.01f);
            config->set("stdp_ltd_weight", 0.012f);
            config->set("stdp_tau", 20.0f);
            config->set("synaptogenesis_rate", 0.0001f);
            config->set("pruning_rate", 0.00001f);
            config->set("replay_interval", 100);
            config->set("consolidation_interval", 1000);
            config->set("simulation_timestep", 0.001);
            config->set("random_seed", static_cast<int64_t>(42));
            return config;
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Error creating default configuration: ") + e.what());
        }
    }, "Create a default configuration optimized for Phase 6 simulation");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        try {
            if (!config) {
                throw std::runtime_error("Config cannot be null");
            }
            return std::make_shared<Brain>(config);
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Error creating brain: ") + e.what());
        }
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        try {
            auto world = std::make_shared<SimpleWorld>();
            world->configure(16, 16, 16, 16);
            return world;
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Error creating world: ") + e.what());
        }
    }, "Create a configured simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        try {
            if (!brain) {
                throw std::runtime_error("Brain cannot be null");
            }
            return std::make_shared<AgentBrain>(brain);
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Error creating agent brain: ") + e.what());
        }
    }, py::arg("brain"), "Create a new agent brain interface");

    m.def("run_brain_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                   std::shared_ptr<AgentBrain> agent, double timestep, int steps) {
        try {
            if (!brain || !world || !agent) {
                throw std::runtime_error("Brain, world, or agent is null");
            }
            if (timestep <= 0.0) {
                throw std::runtime_error("Timestep must be positive");
            }
            if (steps <= 0) {
                throw std::runtime_error("Steps must be positive");
            }
            
            // Run complete brain simulation
            world->reset();
            auto config = brain->getConfig();
            auto agentBrain = std::make_shared<AgentBrain>(brain);
            agentBrain->initialize(*world);
            
            for (int i = 0; i < steps; ++i) {
                // Get observation
                world->update(timestep);
                SensoryPercept percept = world->getSensoryPercept();
                
                // Process sensory input
                agentBrain->processSensoryInput(percept);
                
                // Brain step
                brain->step(i, i * timestep);
                
                // Get motor command
                MotorCommand cmd = agentBrain->decodeMotorCommand();
                
                // Apply action to world
                ActionResult actionResult = world->applyMotorCommand(cmd, i * timestep);
                
                // Apply reward modulation
                agentBrain->applyRewardModulation(actionResult.reward, 0.0f);
                
                // Update development
                agentBrain->updateDevelopment(timestep);
            }
            
            return brain->getTotalSpikeCount();
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Error running brain simulation: ") + e.what());
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
       py::arg("timestep") = 0.1, py::arg("steps") = 100,
       "Run complete brain simulation and return spike count");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
} // namespace nlm
