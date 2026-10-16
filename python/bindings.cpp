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
#include "../src/memory/WorkingMemory.hpp"
#include "../src/memory/Memory.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/AttentionalSelection.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/Curiosity.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        Also includes memory systems, neuromodulation, prediction, and cognitive systems.
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
        .def("__ne__", &NeuronId::operator!=)
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
        .def("getHeight", &SimpleWorld::getHeight)\n        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
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
        
        // Memory systems access
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory system")
        
        // Prediction system access
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system")
        
        // Cognition systems access
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection system")
        
        // Development system access
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        
        // Neuromodulation systems access
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine neuromodulator")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity neuromodulator")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty neuromodulator")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error signal")
        
        // Spike system access
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get spike system")
        
        // Plasticity system access
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get STDP plasticity rule")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get Hebbian plasticity rule")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get structural plasticity rule")
        
        // Plasticity control
        .def("updatePlasticity", &Brain::updatePlasticity,
             "Update plasticity rules (called automatically in step)")
        
        // Development control
        .def("develop", &Brain::develop,
             "Apply developmental changes (called automatically in step)")
        
        // Neuromodulation control
        .def("applyNeuromodulation", &Brain::applyNeuromodulation,
             py::arg("signal"),
             "Apply neuromodulatory signals")
        
        // Random generator access
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get random generator")
        
        // Inter-region connection management
        .def("addInterRegionConnection", &Brain::addInterRegionConnection,
             py::arg("source"), py::arg("target"),
             py::arg("weight") = 0.0f, py::arg("delay") = Delay(1),
             "Add inter-region connection")
        .def("removeInterRegionConnection", &Brain::removeInterRegionConnection,
             py::arg("source"), py::arg("target"),
             "Remove inter-region connection");

    py::class_<WorkingMemory>(m, "WorkingMemory", R"pbdoc(Working memory for temporary information storage)pbdoc")
        .def(py::init<>())
        .def("store", &WorkingMemory::store, py::arg("neuron"), py::arg("value"),
             "Store item in working memory")
        .def("retrieve", &WorkingMemory::retrieve, py::arg("neuron"),
             "Retrieve item from working memory")
        .def("contains", &WorkingMemory::contains, py::arg("neuron"),
             "Check if item exists in working memory")
        .def("clear", &WorkingMemory::clear,
             "Clear all items from working memory")
        .def("getCapacity", &WorkingMemory::getCapacity,
             "Get working memory capacity")
        .def("getCurrentSize", &WorkingMemory::getCurrentSize,
             "Get current number of items in working memory")
        .def("decay", &WorkingMemory::decay, py::arg("decayRate"),
             "Decay all items with given decay rate");

    py::class_<EpisodicMemoryItem>(m, "EpisodicMemoryItem", R"pbdoc(Episodic memory item)pbdoc")
        .def(py::init<>())
        .def_readwrite("timestamp", &EpisodicMemoryItem::timestamp)
        .def_readwrite("neurons", &EpisodicMemoryItem::neurons)
        .def_readwrite("values", &EpisodicMemoryItem::values)
        .def_readwrite("metadata", &EpisodicMemoryItem::metadata);

    py::class_<EpisodicMemory>(m, "EpisodicMemory", R"pbdoc(Episodic memory for storing events and experiences)pbdoc")
        .def(py::init<>())
        .def("storeEpisode", &EpisodicMemory::storeEpisode, py::arg("episode"),
             "Store an episode in episodic memory")
        .def("retrieveEpisode", &EpisodicMemory::retrieveEpisode, py::arg("index"),
             "Retrieve an episode from episodic memory")
        .def("getEpisodeCount", &EpisodicMemory::getEpisodeCount,
             "Get number of episodes in episodic memory")
        .def("getRecentEpisodes", &EpisodicMemory::getRecentEpisodes, py::arg("count"),
             "Get recent episodes")
        .def("clear", &EpisodicMemory::clear,
             "Clear all episodes from episodic memory")
        .def("consolidate", &EpisodicMemory::consolidate, py::arg("relevanceThreshold"),
             "Consolidate episodes above relevance threshold");

    py::class_<SemanticMemory>(m, "SemanticMemory", R"pbdoc(Semantic memory for acquired knowledge and facts)pbdoc")
        .def(py::init<>())
        .def("storeFact", &SemanticMemory::storeFact, py::arg("key"), py::arg("value"),
             "Store a fact in semantic memory")
        .def("retrieveFact", &SemanticMemory::retrieveFact, py::arg("key"),
             "Retrieve a fact from semantic memory")
        .def("hasFact", &SemanticMemory::hasFact, py::arg("key"),
             "Check if a fact exists in semantic memory")
        .def("getAllFacts", &SemanticMemory::getAllFacts,
             "Get all facts from semantic memory")
        .def("clear", &SemanticMemory::clear,
             "Clear all facts from semantic memory");

    py::class_<Skill>(m, "Skill", R"pbdoc(Procedural skill)pbdoc")
        .def(py::init<>())
        .def_readwrite("name", &Skill::name)
        .def_readwrite("neuralPattern", &Skill::neuralPattern)
        .def_readwrite("proficiency", &Skill::proficiency);

    py::class_<ProceduralMemory>(m, "ProceduralMemory", R"pbdoc(Procedural memory for learned skills and habits)pbdoc")
        .def(py::init<>())
        .def("learnSkill", &ProceduralMemory::learnSkill, py::arg("name"), py::arg("pattern"),
             "Learn a new skill")
        .def("getSkill", &ProceduralMemory::getSkill, py::arg("name"),
             py::return_value_policy::reference_internal,
             "Get a skill by name")
        .def("getAllSkills", &ProceduralMemory::getAllSkills,
             "Get all skills")
        .def("updateProficiency", &ProceduralMemory::updateProficiency, py::arg("name"), py::arg("delta"),
             "Update skill proficiency")
        .def("clear", &ProceduralMemory::clear,
             "Clear all skills from procedural memory");

    py::class_<AssociativeMemory>(m, "AssociativeMemory", R"pbdoc(Associative memory for relationships between representations)pbdoc")
        .def(py::init<>())
        .def("associate", &AssociativeMemory::associate, py::arg("a"), py::arg("b"), py::arg("strength"),
             "Create association between two neurons")
        .def("getAssociations", &AssociativeMemory::getAssociations, py::arg("neuron"),
             "Get all associations for a neuron")
        .def("getAssociationStrength", &AssociativeMemory::getAssociationStrength, py::arg("a"), py::arg("b"),
             "Get association strength between two neurons")
        .def("updateAssociation", &AssociativeMemory::updateAssociation, py::arg("a"), py::arg("b"), py::arg("delta"),
             "Update association strength")
        .def("clear", &AssociativeMemory::clear,
             "Clear all associations");

    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Prediction system for sensory prediction and error computation)pbdoc")
        .def(py::init<>())
        .def("predictNextState", &PredictionSystem::predictNextState, py::arg("currentState"),
             "Make prediction for next timestep")
        .def("updatePredictions", &PredictionSystem::updatePredictions, py::arg("predicted"), py::arg("actual"),
             "Update predictions based on actual observation")
        .def("getPredictionError", &PredictionSystem::getPredictionError,
             "Get prediction error")
        .def("getConfidence", &PredictionSystem::getConfidence,
             "Get prediction confidence")
        .def("getErrorHistory", &PredictionSystem::getErrorHistory,
             "Get prediction error history")
        .def("clearHistory", &PredictionSystem::clearHistory,
             "Clear prediction error history")
        .def("train", &PredictionSystem::train, py::arg("observation"),
             "Train prediction model with observation");

    py::class_<PlanningCandidate>(m, "PlanningCandidate", R"pbdoc(Action planning candidate)pbdoc")
        .def(py::init<>())
        .def_readwrite("actions", &PlanningCandidate::actions)
        .def_readwrite("predictedStates", &PlanningCandidate::predictedStates)
        .def_readwrite("expectedReward", &PlanningCandidate::expectedReward)
        .def_readwrite("confidence", &PlanningCandidate::confidence)
        .def_readwrite("depth", &PlanningCandidate::depth);

    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planner for action planning)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPlanner::initialize, py::arg("brain"),
             "Initialize planner with brain reference")
        .def("planAction", &NeuralPlanner::planAction, py::arg("currentState"), py::arg("targetReward") = 0.5f,
             "Plan next action given current state and goal")
        .def("evaluateSequence", &NeuralPlanner::evaluateSequence, py::arg("actions"), py::arg("startState"),
             "Evaluate a potential action sequence")
        .def("getPlanningDepth", &NeuralPlanner::getPlanningDepth,
             "Get number of steps to look ahead")
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth, py::arg("depth"),
             "Set number of steps to look ahead")
        .def("getPlanningConfidence", &NeuralPlanner::getPlanningConfidence,
             "Get planning confidence")
        .def("updatePlanQuality", &NeuralPlanner::updatePlanQuality, py::arg("plannedActions"), py::arg("actualActions"), py::arg("actualReward"),
             "Update plans based on actual outcome")
        .def("clearCache", &NeuralPlanner::clearCache,
             "Clear planning cache")
        .def("setActionQuality", &NeuralPlanner::setActionQuality, py::arg("action"), py::arg("quality"),
             "Set action quality from experience")
        .def("getCurrentGoal", &NeuralPlanner::getCurrentGoal,
             "Get current goal")
        .def("setCurrentGoal", &NeuralPlanner::setCurrentGoal, py::arg("goal"),
             "Set current goal")
        .def("wasRecentPlanSuccessful", &NeuralPlanner::wasRecentPlanSuccessful,
             "Has recent planning been successful?");

    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation from experience patterns)pbdoc")
        .def(py::init<>())
        .def("processPattern", &ConceptFormation::processPattern, py::arg("pattern"),
             "Process a pattern for concept formation")
        .def("getConcepts", &ConceptFormation::getConcepts,
             "Get formed concepts")
        .def("getConceptSimilarity", &ConceptFormation::getConceptSimilarity, py::arg("concept1"), py::arg("concept2"),
             "Get similarity between two concepts")
        .def("updateWithExperience", &ConceptFormation::updateWithExperience, py::arg("experience"),
             "Update concepts with new experience");

    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Development system for age-dependent changes)pbdoc")
        .def(py::init<>())
        .def("update", &DevelopmentSystem::update, py::arg("dt"),
             "Update development system")
        .def("getCurrentStage", &DevelopmentSystem::getCurrentStage,
             "Get current developmental stage")
        .def("setCurrentStage", &DevelopmentSystem::setCurrentStage, py::arg("stage"),
             "Set current developmental stage")
        .def("getAge", &DevelopmentSystem::getAge,
             "Get current age")
        .def("isPlasticityEnabled", &DevelopmentSystem::isPlasticityEnabled,
             "Check if plasticity is enabled")
        .def("setPlasticityEnabled", &DevelopmentSystem::setPlasticityEnabled, py::arg("enabled"),
             "Set plasticity enabled state")
        .def("getPlasticityRate", &DevelopmentSystem::getPlasticityRate,
             "Get current plasticity rate")
        .def("setPlasticityRate", &DevelopmentSystem::setPlasticityRate, py::arg("rate"),
             "Set plasticity rate");

    py::class_<Dopamine>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator for reward and reinforcement)pbdoc")
        .def(py::init<>())
        .def("getName", &Dopamine::getName,
             "Get neuromodulator name")
        .def("getLevel", &Dopamine::getLevel,
             "Get current concentration/level")
        .def("setLevel", &Dopamine::setLevel, py::arg("level"),
             "Set current concentration/level")
        .def("getPlasticityFactor", &Dopamine::getPlasticityFactor,
             "Get plasticity modulation factor")
        .def("update", &Dopamine::update, py::arg("dt"),
             "Update neuromodulator state")
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"),
             "Signal reward for reinforcement learning")
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"),
             "Signal reward prediction error");

    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity neuromodulator for exploration motivation)pbdoc")
        .def(py::init<>())
        .def("getName", &Curiosity::getName,
             "Get neuromodulator name")
        .def("getLevel", &Curiosity::getLevel,
             "Get current level")
        .def("setLevel", &Curiosity::setLevel, py::arg("level"),
             "Set current level")
        .def("getPlasticityFactor", &Curiosity::getPlasticityFactor,
             "Get plasticity modulation factor")
        .def("update", &Curiosity::update, py::arg("dt"),
             "Update neuromodulator state")
        .def("getExplorationLevel", &Curiosity::getExplorationLevel,
             "Get exploration level")
        .def("setExplorationLevel", &Curiosity::setExplorationLevel, py::arg("level"),
             "Set exploration level");

    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty neuromodulator for novelty detection)pbdoc")
        .def(py::init<>())
        .def("getName", &Novelty::getName,
             "Get neuromodulator name")
        .def("getLevel", &Novelty::getLevel,
             "Get current level")
        .def("setLevel", &Novelty::setLevel, py::arg("level"),
             "Set current level")
        .def("getPlasticityFactor", &Novelty::getPlasticityFactor,
             "Get plasticity modulation factor")
        .def("update", &Novelty::update, py::arg("dt"),
             "Update neuromodulator state")
        .def("getNoveltyScore", &Novelty::getNoveltyScore, py::arg("input"),
             "Get novelty score for input")
        .def("setNoveltyThreshold", &Novelty::setNoveltyThreshold, py::arg("threshold"),
             "Set novelty detection threshold");

    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error neuromodulator)pbdoc")
        .def(py::init<>())
        .def("getName", &PredictionError::getName,
             "Get neuromodulator name")
        .def("getLevel", &PredictionError::getLevel,
             "Get current level")
        .def("setLevel", &PredictionError::setLevel, py::arg("level"),
             "Set current level")
        .def("getPlasticityFactor", &PredictionError::getPlasticityFactor,
             "Get plasticity modulation factor")
        .def("update", &PredictionError::update, py::arg("dt"),
             "Update neuromodulator state")
        .def("getError", &PredictionError::getError, py::arg("prediction"), py::arg("target"),
             "Get prediction error")
        .def("setLearningRate", &PredictionError::setLearningRate, py::arg("rate"),
             "Set learning rate for error correction");

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

    // Convenience functions for common operations
    
    m.def("reset_all_motors", [](std::shared_ptr<Brain> brain) {
        if (brain) {
            auto action = brain->produceAction();
            // Motor action produced - in real usage, this would be processed by world
            return true;
        }
        return false;
    }, py::arg("brain"), "Reset all motor outputs (convenience function)");
    
    m.def("get_current_state", [](std::shared_ptr<Brain> brain) {
        if (!brain) return std::vector<float>();
        
        std::vector<float> state;
        state.reserve(10); // Reserve space for common metrics
        
        // Add current neural activity metrics
        state.push_back(static_cast<float>(brain->getTotalSpikeCount()));
        state.push_back(static_cast<float>(brain->getActiveNeuronCount()));
        state.push_back(static_cast<float>(brain->getFiringNeuronCount()));
        state.push_back(static_cast<float>(brain->getAverageFiringRate()));
        state.push_back(static_cast<float>(brain->getExcitationInhibitionRatio()));
        
        // Add memory system metrics
        if (auto* wm = brain->getWorkingMemory()) {
            state.push_back(static_cast<float>(wm->getCurrentSize()));
        } else {
            state.push_back(0.0f);
        }
        
        if (auto* em = brain->getEpisodicMemory()) {
            state.push_back(static_cast<float>(em->getEpisodeCount()));
        } else {
            state.push_back(0.0f);
        }
        
        // Add neuromodulation metrics
        if (auto* dopamine = brain->getDopamine()) {
            state.push_back(dopamine->getLevel());
        } else {
            state.push_back(0.0f);
        }
        
        if (auto* curiosity = brain->getCuriosity()) {
            state.push_back(curiosity->getLevel());
        } else {
            state.push_back(0.0f);
        }
        
        return state;
    }, py::arg("brain"), "Get current brain state as vector (convenience function)");
    
    m.def("set_developmental_stage", [](std::shared_ptr<Brain> brain, DevelopmentalStage stage) {
        if (brain) {
            brain->setDevelopmentalStage(stage);
            if (auto* dev = brain->getDevelopmentSystem()) {
                dev->setCurrentStage(stage);
            }
            return true;
        }
        return false;
    }, py::arg("brain"), py::arg("stage"), "Set developmental stage for brain (convenience function)");
    
    m.def("enable_all_systems", [](std::shared_ptr<Brain> brain) {
        if (!brain) return false;
        
        // Enable all neuromodulation systems
        if (auto* agent_brain = dynamic_cast<AgentBrain*>(brain.get())) {
            agent_brain->enableRewardModulation(true);
            agent_brain->enableDevelopment(true);
            agent_brain->enableCuriosity(true);
        }
        
        return true;
    }, py::arg("brain"), "Enable all neuromodulation and development systems (convenience function)");
    
    m.def("get_system_status", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, std::string> status;
        
        if (!brain) return status;
        
        status["brain_initialized"] = brain->getRegionCount() > 0 ? "true" : "false";
        status["neuron_count"] = std::to_string(brain->getTotalNeuronCount());
        status["synapse_count"] = std::to_string(brain->getTotalSynapseCount());
        status["total_spikes"] = std::to_string(brain->getTotalSpikeCount());
        
        // Memory system status
        if (auto* wm = brain->getWorkingMemory()) {
            status["working_memory_capacity"] = std::to_string(wm->getCapacity());
            status["working_memory_size"] = std::to_string(wm->getCurrentSize());
        }
        
        // Neuromodulation status
        if (auto* dopamine = brain->getDopamine()) {
            status["dopamine_level"] = std::to_string(dopamine->getLevel());
        }
        
        if (auto* curiosity = brain->getCuriosity()) {
            status["curiosity_level"] = std::to_string(curiosity->getLevel());
        }
        
        return status;
    }, py::arg("brain"), "Get comprehensive system status as dictionary (convenience function)");
    
    m.def("create_complete_config", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Set commonly used default values for quick setup
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001);
        config->set("neuron_count", static_cast<int64_t>(500));
        config->set("region_count", static_cast<int64_t>(1));
        config->set("connection_probability", 0.15f);
        
        // STDP parameters
        config->set("stdp_ltp_weight", 0.02f);
        config->set("stdp_ltd_weight", 0.015f);
        config->set("stdp_tau", 20.0f);
        
        // Structural plasticity
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        
        return config;
    }, "Create a complete configuration with all commonly used settings (convenience function)");
    
    m.def("create_training_config", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Configuration optimized for learning and training
        config->set("random_seed", static_cast<int64_t>(123));
        config->set("simulation_timestep", 0.001);
        config->set("neuron_count", static_cast<int64_t>(1000));
        config->set("region_count", static_cast<int64_t>(3));
        config->set("connection_probability", 0.20f);
        
        // High plasticity settings for learning
        config->set("stdp_ltp_weight", 0.05f);
        config->set("stdp_ltd_weight", 0.03f);
        config->set("stdp_tau", 50.0f);
        
        // Strong structural plasticity
        config->set("synaptogenesis_rate", 0.0005f);
        config->set("pruning_rate", 0.00005f);
        
        return config;
    }, "Create a configuration optimized for learning and training (convenience function)");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm