#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

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
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/performance/MemoryPool.hpp"
#include "../src/performance/CheckpointSystem.hpp"

namespace py = pybind11;
namespace nlm {

// Custom exception for NLM errors
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        An enhanced Python binding for the NLM C++ neural simulation framework.
        
        This version includes all major systems: memory, neuromodulation, prediction,
        cognition, development, and performance systems.
    )pbdoc";

    // Register custom exception
    py::register_exception<NLMError>(m, "NLMError");

    // ====================================================
    // CORE TYPES AND IDENTIFIERS
    // ====================================================

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

    // ====================================================
    // ENUMS
    // ====================================================

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

    // ====================================================
    // CONFIG SYSTEM
    // ====================================================

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

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    // ====================================================
    // SENSORY INPUT SYSTEMS
    // ====================================================

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

    // ====================================================
    // ACTION SYSTEMS
    // ====================================================

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

    // ====================================================
    // AGENT SYSTEMS
    // ====================================================

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

    // ====================================================
    // WORLD SYSTEM
    // ====================================================

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
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"))
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    // ====================================================
    // BRAIN SYSTEM - DIRECT ACCESS TO CORE SYSTEMS
    // ====================================================

    // Memory Systems
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Working memory with persistent activity and competition)pbdoc")
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
        .def("setCapacity", &NeuralWorkingMemory::setCapacity, py::arg("cap"))
        .def("getDecayRate", &NeuralWorkingMemory::getDecayRate)
        .def("setDecayRate", &NeuralWorkingMemory::setDecayRate, py::arg("rate"))
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons)
        .def("strengthenMemory", &NeuralWorkingMemory::strengthenMemory, py::arg("factor"))
        .def("runCompetition", &NeuralWorkingMemory::runCompetition)
        .def("isWinning", &NeuralWorkingMemory::isWinning, py::arg("neuron"))
        .def("getMemoryActivity", &NeuralWorkingMemory::getMemoryActivity);

    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Episodic memory with experience encoding and replay)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralEpisodicMemory::initialize, py::arg("brain"))
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode, py::arg("episode"))
        .def("retrieveSimilar", py::overload_cast<const std::vector<float>&, size_t>(&NeuralEpisodicMemory::retrieveSimilar),
             py::arg("sensoryPattern"), py::arg("maxResults") = 5)
        .def("retrieveTemporal", py::overload_cast<SimulationStep, SimulationStep, size_t>(&NeuralEpisodicMemory::retrieveTemporal),
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
        .def("consolidate", &NeuralEpisodicMemory::consolidate, py::arg("relevanceThreshold"))
        .def("clear", &NeuralEpisodicMemory::clear)
        .def("enableReplay", &NeuralEpisodicMemory::enableReplay, py::arg("enable"))
        .def("isReplayEnabled", &NeuralEpisodicMemory::isReplayEnabled)
        .def("getEpisodesForReplay", &NeuralEpisodicMemory::getEpisodesForReplay, py::arg("count"))
        .def("replaySequence", &NeuralEpisodicMemory::replaySequence, py::arg("episodeIds"));

    py::class_<NeuralAssociativeMemory>(m, "NeuralAssociativeMemory", R"pbdoc(Associative memory with Hebbian pattern associations)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralAssociativeMemory::initialize, py::arg("brain"))
        .def("associate", &NeuralAssociativeMemory::associate,
             py::arg("patternA"), py::arg("patternB"), py::arg("strength") = 1.0f)
        .def("associateFromExperience", &NeuralAssociativeMemory::associateFromExperience,
             py::arg("episode"))
        .def("retrieve", py::overload_cast<const std::vector<float>&, size_t>(&NeuralAssociativeMemory::retrieve),
             py::arg("queryPattern"), py::arg("maxResults") = 5)
        .def("getAssociationStrength", &NeuralAssociativeMemory::getAssociationStrength,
             py::arg("patternA"), py::arg("patternB"))
        .def("updateAssociation", &NeuralAssociativeMemory::updateAssociation,
             py::arg("patternA"), py::arg("patternB"), py::arg("delta"))
        .def("spreadActivation", py::overload_cast<const std::vector<float>&, size_t>(&NeuralAssociativeMemory::spreadActivation),
             py::arg("cuePattern"), py::arg("steps") = 2)
        .def("clear", &NeuralAssociativeMemory::clear)
        .def("getAssociationCount", &NeuralAssociativeMemory::getAssociationCount);

    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation from experience patterns)pbdoc")
        .def(py::init<>())
        .def("initialize", &ConceptFormation::initialize, py::arg("brain"))
        .def("presentExperience", &ConceptFormation::presentExperience,
             py::arg("experience"), py::arg("reward"), py::arg("novelty"))
        .def("getMatchingConcept", &ConceptFormation::getMatchingConcept,
             py::arg("pattern"), py::arg("similarityThreshold") = 0.7f)
        .def("getConceptPrototype", &ConceptFormation::getConceptPrototype,
             py::arg("conceptId"))
        .def("getConceptStability", &ConceptFormation::getConceptStability,
             py::arg("conceptId"))
        .def("isNovel", &ConceptFormation::isNovel, py::arg("pattern"))
        .def("getConceptCount", &ConceptFormation::getConceptCount)
        .def("clear", &ConceptFormation::clear);

    // Neuromodulation Systems
    py::class_<Dopamine>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator for reward and reinforcement)pbdoc")
        .def(py::init<>())
        .def("getName", &Dopamine::getName)
        .def("getLevel", &Dopamine::getLevel)
        .def("setLevel", &Dopamine::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Dopamine::getPlasticityFactor)
        .def("update", &Dopamine::update, py::arg("dt"))
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"))
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError,
             py::arg("error"));

    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty detection neuromodulator)pbdoc")
        .def(py::init<>())
        .def("getName", &Novelty::getName)
        .def("getLevel", &Novelty::getLevel)
        .def("setLevel", &Novelty::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Novelty::getPlasticityFactor)
        .def("update", &Novelty::update, py::arg("dt"))
        .def("detectNovelty", &Novelty::detectNovelty, py::arg("currentState"),
             py::arg("expectedState"))
        .def("resetNovelty", &Novelty::resetNovelty);

    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity neuromodulator for exploration)pbdoc")
        .def(py::init<>())
        .def("getName", &Curiosity::getName)
        .def("getLevel", &Curiosity::getLevel)
        .def("setLevel", &Curiosity::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Curiosity::getPlasticityFactor)
        .def("update", &Curiosity::update, py::arg("dt"))
        .def("getCuriosityDrive", &Curiosity::getCuriosityDrive,
             py::arg("stateEntropy"), py::arg("actionDiversity"))
        .def("applyExplorationBonus", &Curiosity::applyExplorationBonus,
             py::arg("action"), py::arg("reward"));

    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error signal for learning)pbdoc")
        .def(py::init<>())
        .def("getName", &PredictionError::getName)
        .def("getLevel", &PredictionError::getLevel)
        .def("setLevel", &PredictionError::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &PredictionError::getPlasticityFactor)
        .def("update", &PredictionError::update, py::arg("dt"))
        .def("computeError", &PredictionError::computeError,
             py::arg("predicted"), py::arg("actual"))
        .def("resetError", &PredictionError::resetError);

    // ====================================================
    // PREDICTION SYSTEM
    // ====================================================

    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Prediction system for sensory prediction and error computation)pbdoc")
        .def(py::init<>())
        .def("predictNextState", &PredictionSystem::predictNextState, py::arg("currentState"))
        .def("updatePredictions", &PredictionSystem::updatePredictions,
             py::arg("predicted"), py::arg("actual"))
        .def("getPredictionError", &PredictionSystem::getPredictionError)
        .def("getConfidence", &PredictionSystem::getConfidence)
        .def("getErrorHistory", &PredictionSystem::getErrorHistory)
        .def("clearHistory", &PredictionSystem::clearHistory)
        .def("train", &PredictionSystem::train, py::arg("observation"));

    // ====================================================
    // COGNITION SYSTEMS
    // ====================================================

    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planner with action sequence evaluation)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPlanner::initialize, py::arg("brain"))
        .def("planAction", &NeuralPlanner::planAction,
             py::arg("currentState"), py::arg("targetReward") = 0.5f)
        .def("evaluateSequence", &NeuralPlanner::evaluateSequence,
             py::arg("actions"), py::arg("startState"))
        .def("getPlanningDepth", &NeuralPlanner::getPlanningDepth)
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth, py::arg("depth"))
        .def("getPlanningConfidence", &NeuralPlanner::getPlanningConfidence)
        .def("updatePlanQuality", &NeuralPlanner::updatePlanQuality,
             py::arg("plannedActions"), py::arg("actualActions"), py::arg("actualReward"))
        .def("clearCache", &NeuralPlanner::clearCache)
        .def("setActionQuality", &NeuralPlanner::setActionQuality,
             py::arg("action"), py::arg("quality"))
        .def("setCurrentGoal", &NeuralPlanner::setCurrentGoal, py::arg("goal"))
        .def("getCurrentGoal", &NeuralPlanner::getCurrentGoal)
        .def("wasRecentPlanSuccessful", &NeuralPlanner::wasRecentPlanSuccessful);

    py::class_<SelfModel>(m, "SelfModel", R"pbdoc(Sensorimotor self-model and forward prediction)pbdoc")
        .def(py::init<>())
        .def("initialize", &SelfModel::initialize, py::arg("brain"))
        .def("recordSelfAction", &SelfModel::recordSelfAction,
             py::arg("action"), py::arg("beforeState"), py::arg("afterState"))
        .def("predictActionConsequence", &SelfModel::predictActionConsequence,
             py::arg("action"), py::arg("currentState"))
        .def("getSelfModelConfidence", &SelfModel::getSelfModelConfidence, py::arg("action"))
        .def("computeSelfGeneratedLikeness", &SelfModel::computeSelfGeneratedLikeness,
             py::arg("beforeState"), py::arg("afterState"), py::arg("action"))
        .def("getPreferredAction", &SelfModel::getPreferredAction, py::arg("state"))
        .def("updateSelfModel", &SelfModel::updateSelfModel,
             py::arg("predicted"), py::arg("actual"), py::arg("action"))
        .def("getCapabilityLevel", &SelfModel::getCapabilityLevel)
        .def("getBodyAwareness", &SelfModel::getBodyAwareness)
        .def("clear", &SelfModel::clear)
        .def("hasSelfModel", &SelfModel::hasSelfModel);

    py::class_<SocialLearning>(m, "SocialLearning", R"pbdoc(Social learning from observation and imitation)pbdoc")
        .def(py::init<>())
        .def("initialize", &SocialLearning::initialize, py::arg("brain"))
        .def("observeAgentAction", &SocialLearning::observeAgentAction,
             py::arg("observedAction"), py::arg("observerState"), py::arg("resultingState"))
        .def("canImitate", &SocialLearning::canImitate, py::arg("observedAction"))
        .def("getImitationAction", &SocialLearning::getImitationAction, py::arg("currentState"))
        .def("learnCommunicationSignal", &SocialLearning::learnCommunicationSignal,
             py::arg("signalPattern"), py::arg("signalReward"))
        .def("detectSignal", &SocialLearning::detectSignal, py::arg("neuralPattern"))
        .def("getSignalPattern", &SocialLearning::getSignalPattern)
        .def("getSignalMeaning", &SocialLearning::getSignalMeaning)
        .def("updateSocialKnowledge", &SocialLearning::updateSocialKnowledge,
             py::arg("interactionReward"))
        .def("clear", &SocialLearning::clear)
        .def("hasSocialKnowledge", &SocialLearning::hasSocialKnowledge)
        .def("getObservationCount", &SocialLearning::getObservationCount);

    py::class_<AttentionalSelection>(m, "AttentionalSelection", R"pbdoc(Neural attention with competitive selection)pbdoc")
        .def(py::init<>())
        .def("initialize", &AttentionalSelection::initialize, py::arg("brain"))
        .def("processCompetition", &AttentionalSelection::processCompetition,
             py::arg("competitors"), py::arg("globalInhibition") = 0.5f)
        .def("focusOnRegion", &AttentionalSelection::focusOnRegion, py::arg("region"))
        .def("releaseAttention", &AttentionalSelection::releaseAttention)
        .def("getAttendedRegions", &AttentionalSelection::getAttendedRegions)
        .def("setInhibitionStrength", &AttentionalSelection::setInhibitionStrength,
             py::arg("strength"))
        .def("setExcitationStrength", &AttentionalSelection::setExcitationStrength,
             py::arg("strength"))
        .def("setCompetitionThreshold", &AttentionalSelection::setCompetitionThreshold,
             py::arg("threshold"))
        .def("getInhibitionFor", &AttentionalSelection::getInhibitionFor, py::arg("neuron"))
        .def("getExcitationFor", &AttentionalSelection::getExcitationFor, py::arg("neuron"))
        .def("update", &AttentionalSelection::update, py::arg("dt"))
        .def("isAttended", &AttentionalSelection::isAttended, py::arg("neuron"))
        .def("getWinners", &AttentionalSelection::getWinners)
        .def("applyTopDownBias", &AttentionalSelection::applyTopDownBias,
             py::arg("neuron"), py::arg("biasStrength"))
        .def("applyBottomUpSalience", &AttentionalSelection::applyBottomUpSalience,
             py::arg("neuron"), py::arg("salienceStrength"))
        .def("reset", &AttentionalSelection::reset);

    // ====================================================
    // DEVELOPMENT SYSTEM
    // ====================================================

    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Development system with age-dependent plasticity)pbdoc")
        .def(py::init<>())
        .def("initialize", &DevelopmentSystem::initialize, py::arg("brain"))
        .def("update", &DevelopmentSystem::update, py::arg("dt"))
        .def("getStage", &DevelopmentSystem::getStage)
        .def("setStage", &DevelopmentSystem::setStage, py::arg("stage"))
        .def("getAge", &DevelopmentSystem::getAge)
        .def("getPlasticityRate", &DevelopmentSystem::getPlasticityRate)
        .def("setPlasticityRate", &DevelopmentSystem::setPlasticityRate, py::arg("rate"))
        .def("isCriticalPeriod", &DevelopmentSystem::isCriticalPeriod)
        .def("getMaturationProgress", &DevelopmentSystem::getMaturationProgress,
             py::arg("targetProperty"));

    // ====================================================
    // PERFORMANCE/MONITORING SYSTEMS
    // ====================================================

    py::class_<MemoryPool>(m, "MemoryPool", R"pbdoc(Optimized memory pool for performance)pbdoc")
        .def(py::init<>())
        .def("allocate", [](MemoryPool& self, size_t size) {
            return self.allocate(size);
        }, py::arg("size"))
        .def("deallocate", [](MemoryPool& self, void* ptr, size_t size) {
            self.deallocate(ptr, size);
        }, py::arg("ptr"), py::arg("size"))
        .def("allocateNeuron", []() -> NeuronId {
            return NeuronId();
        })
        .def("allocateSynapse", []() -> SynapseId {
            return SynapseId();
        })
        .def("getAllocationCount", &MemoryPool::getAllocationCount)
        .def("getDeallocationCount", &MemoryPool::getDeallocationCount)
        .def("getAvailableMemory", &MemoryPool::getAvailableMemory)
        .def("resetStats", &MemoryPool::resetStats);

    py::class_<CheckpointSystem>(m, "CheckpointSystem", R"pbdoc(Checkpoint system for state persistence)pbdoc")
        .def(py::init<>())
        .def("saveState", &CheckpointSystem::saveState, py::arg("filepath"),
             py::arg("metadata") = "")
        .def("loadState", &CheckpointSystem::loadState, py::arg("filepath"))
        .def("getCheckpointCount", &CheckpointSystem::getCheckpointCount)
        .def("getCheckpointInfo", &CheckpointSystem::getCheckpointInfo, py::arg("index"))
        .def("listCheckpoints", &CheckpointSystem::listCheckpoints)
        .def("removeCheckpoint", &CheckpointSystem::removeCheckpoint, py::arg("index"))
        .def("clearOldCheckpoints", &CheckpointSystem::clearOldCheckpoints,
             py::arg("maxCheckpoints") = 10);

    // ====================================================
    // AGENT BRAIN INTERFACE (Enhanced)
    // ====================================================

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"))
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize)
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize)
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"))
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand)
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"))
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"))
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage)
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel)
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel)
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel)
        .def("getPredictionError", &AgentBrain::getPredictionError)
        .def("reset", &AgentBrain::reset)
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal)
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        .def("getWorkingMemory", [](const AgentBrain& agent) -> std::shared_ptr<NeuralWorkingMemory> {
            if (agent.getBrain()) {
                return agent.getBrain()->getWorkingMemory();
            }
            return nullptr;
        })
        .def("getEpisodicMemory", [](const AgentBrain& agent) -> std::shared_ptr<NeuralEpisodicMemory> {
            if (agent.getBrain()) {
                return agent.getBrain()->getEpisodicMemory();
            }
            return nullptr;
        })
        .def("getAssociativeMemory", [](const AgentBrain& agent) -> std::shared_ptr<NeuralAssociativeMemory> {
            if (agent.getBrain()) {
                return agent.getBrain()->getAssociativeMemory();
            }
            return nullptr;
        })
        .def("getPlanner", [](const AgentBrain& agent) -> std::shared_ptr<NeuralPlanner> {
            if (agent.getBrain()) {
                return agent.getBrain()->getPlanner();
            }
            return nullptr;
        })
        .def("getAttention", [](const AgentBrain& agent) -> std::shared_ptr<AttentionalSelection> {
            if (agent.getBrain()) {
                return agent.getBrain()->getAttention();
            }
            return nullptr;
        })
        .def("getPredictionSystem", [](const AgentBrain& agent) -> std::shared_ptr<PredictionSystem> {
            if (agent.getBrain()) {
                return agent.getBrain()->getPredictionSystem();
            }
            return nullptr;
        })
        .def("getDevelopmentSystem", [](const AgentBrain& agent) -> std::shared_ptr<DevelopmentSystem> {
            if (agent.getBrain()) {
                return agent.getBrain()->getDevelopmentSystem();
            }
            return nullptr;
        });

    // ====================================================
    // BRAIN CLASS (ENHANCED)
    // ====================================================

    py::class_<Brain>(m, "Brain", R"pbdoc(Enhanced neural simulation brain class with direct system access)pbdoc")
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
             "Get pending spike event count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             "Get random generator")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        // Direct system access methods
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
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection system")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine neuromodulator")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty neuromodulator")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity neuromodulator")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error signal")
        // High-level convenience methods
        .def("trainEpisode", [](Brain& brain, AgentBrain& agent, SimpleWorld& world,
                               double timestep, int steps) {
            for (int i = 0; i < steps; ++i) {
                world.update(timestep);
                auto percept = world.getSensoryPercept();
                agent.processSensoryInput(percept);
                brain.step(i);
                auto action = agent.decodeMotorCommand();
                world.applyMotorCommand(action, world.getSimulationTime());
            }
        }, py::arg("agent"), py::arg("world"), py::arg("timestep"), py::arg("steps"),
           "Run a training episode")
        .def("evaluateEpisode", [](Brain& brain, AgentBrain& agent, SimpleWorld& world,
                                  double timestep, int steps) {
            float totalReward = 0;
            for (int i = 0; i < steps; ++i) {
                world.update(timestep);
                auto percept = world.getSensoryPercept();
                agent.processSensoryInput(percept);
                brain.step(i);
                auto action = agent.decodeMotorCommand();
                auto result = world.applyMotorCommand(action, world.getSimulationTime());
                totalReward += result.reward;
            }
            return totalReward;
        }, py::arg("agent"), py::arg("world"), py::arg("timestep"), py::arg("steps"),
           "Run an evaluation episode and return total reward")
        .def("resetBrain", [](Brain& brain, AgentBrain& agent, SimpleWorld& world) {
            brain.reset();
            agent.reset();
            world.reset();
        }, py::arg("agent"), py::arg("world"),
           "Reset all brain, agent, and world state");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // ====================================================
    // ERROR HANDLING AND UTILITIES
    // ====================================================

    // Add utility functions for error handling and validation
    m.def("validateBrainState", [](const Brain& brain) {
        if (!brain.getTotalNeuronCount()) {
            throw NLMError("Brain has no neurons - must initialize first");
        }
        return true;
    }, py::arg("brain"), "Validate brain state");

    m.def("safeBrainStep", [](Brain& brain, int step, double time = 0.0) {
        try {
            if (time > 0.0) {
                brain.step(step, time);
            } else {
                brain.step(step);
            }
            return true;
        } catch (const std::exception& e) {
            throw NLMError(std::string("Brain step failed: ") + e.what());
        }
    }, py::arg("brain"), py::arg("step"), py::arg("time") = 0.0,
       "Safely perform a brain step with error handling");

    // ====================================================
    // FACTORY FUNCTIONS AND HIGH-LEVEL APIs
    // ====================================================

    // Default configuration creation
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Set reasonable defaults for common use cases
        return config;
    }, "Create a default configuration with sensible defaults");

    // High-level training function
    m.def("trainAgent", [](AgentBrain& agent, SimpleWorld& world, int episodes,
                         int stepsPerEpisode, double timestep) {
        float totalReward = 0;
        for (int episode = 0; episode < episodes; ++episode) {
            for (int step = 0; step < stepsPerEpisode; ++step) {
                world.update(timestep);
                auto percept = world.getSensoryPercept();
                agent.processSensoryInput(percept);
                agent.getBrain()->step(step);
                auto action = agent.decodeMotorCommand();
                auto result = world.applyMotorCommand(action, world.getSimulationTime());
                totalReward += result.reward;
                
                // Apply reward modulation
                agent.applyRewardModulation(result.reward, agent.getPredictionError());
            }
            
            // Update development after each episode
            agent.updateDevelopment(timestep * stepsPerEpisode);
        }
        return totalReward / (episodes * stepsPerEpisode);
    }, py::arg("agent"), py::arg("world"), py::arg("episodes"),
       py::arg("stepsPerEpisode"), py::arg("timestep"),
       "Train an agent in a world for multiple episodes");

    // Performance benchmarking
    m.def("benchmarkBrain", [](Brain& brain, int steps, double timestep) {
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < steps; ++i) {
            brain.step(i, timestep * i);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        return std::make_tuple(
            brain.getTotalSpikeCount(),
            brain.getActiveNeuronCount(),
            brain.getAverageFiringRate(),
            duration.count()
        );
    }, py::arg("brain"), py::arg("steps"), py::arg("timestep"),
       "Benchmark brain performance and return statistics");

    // ====================================================
    // CONSTANTS AND HELPERS
    // ====================================================

    // Add invalid ID constants
    m.attr("INVALID_NEURON_ID") = INVALID_NEURON_ID;
    m.attr("INVALID_SYNAPSE_ID") = INVALID_SYNAPSE_ID;
    m.attr("INVALID_REGION_ID") = INVALID_REGION_ID;
    m.attr("INVALID_POPULATION_ID") = INVALID_POPULATION_ID;

    // Add convenience constants
    m.attr("DEFAULT_BRAIN_CAPACITY") = 1000;
    m.attr("DEFAULT_MEMORY_DECAY_RATE") = 0.01f;
    m.attr("DEFAULT_PLANNING_DEPTH") = 3;
    m.attr("DEFAULT_DEVELOPMENT_PLATONICITY") = 0.1f;

    // Add version info
    m.attr("NLM_VERSION") = "2.0.0-enhanced";
    m.attr("PYTHON_BINDINGS_VERSION") = "2.0.0-enhanced";

    // Add examples and documentation
    m.def("examples", []() {
        std::cout << "See documentation and examples for usage patterns.\n";
        std::cout << "Example basic usage:\n";
        std::cout << "  import pynlm\n";
        std::cout << "  brain = pynlm.createBrain(pynlm.createDefaultConfig())\n";
        std::cout << "  brain.initialize()\n";
        std::cout << "  for i in range(100):\n";
        std::cout << "      brain.step(i)\n";
        std::cout << "  print('Brain thinking!')\n";
    });

} // namespace nlm
