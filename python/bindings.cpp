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

// Memory systems
#include "../src/memory/WorkingMemory.hpp"
#include "../src/memory/EpisodicMemory.hpp"
#include "../src/memory/Memory.hpp"
#include "../src/memory/SemanticMemory.hpp"
#include "../src/memory/ProceduralMemory.hpp"
#include "../src/memory/AssociativeMemory.hpp"

// Neuromodulation systems
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/Curiosity.hpp"

// Development systems
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/development/Synaptogenesis.hpp"
#include "../src/development/Pruning.hpp"
#include "../src/development/Maturation.hpp"

// Cognition systems
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/Attention.hpp"

// Prediction systems
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/prediction/NeuralPrediction.hpp"

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

    // Memory systems bindings
    py::class_<WorkingMemory>(m, "WorkingMemory", R"pbdoc(Working memory system for temporary information storage)pbdoc")
        .def(py::init<>())
        .def("store", &WorkingMemory::store, py::arg("neuron"), py::arg("value"),
             "Store value in working memory associated with a neuron")
        .def("retrieve", &WorkingMemory::retrieve, py::arg("neuron"),
             "Retrieve value from working memory for a neuron")
        .def("contains", &WorkingMemory::contains, py::arg("neuron"),
             "Check if working memory contains an item for the neuron")
        .def("clear", &WorkingMemory::clear, "Clear all working memory items")
        .def("get_capacity", &WorkingMemory::getCapacity,
             "Get maximum capacity of working memory")
        .def("get_current_size", &WorkingMemory::getCurrentSize,
             "Get current number of items in working memory")
        .def("decay", &WorkingMemory::decay, py::arg("decay_rate"),
             "Apply decay to all memory items");

    py::class_<EpisodicMemoryItem>(m, "EpisodicMemoryItem", R"pbdoc(Individual episodic memory item)pbdoc")
        .def(py::init<>())
        .def_readwrite("timestamp", &EpisodicMemoryItem::timestamp)
        .def_readwrite("neurons", &EpisodicMemoryItem::neurons)
        .def_readwrite("values", &EpisodicMemoryItem::values)
        .def_readwrite("metadata", &EpisodicMemoryItem::metadata);

    py::class_<EpisodicMemory>(m, "EpisodicMemory", R"pbdoc(Episodic memory system for storing experiences)pbdoc")
        .def(py::init<>())
        .def("store_episode", &EpisodicMemory::storeEpisode, py::arg("episode"),
             "Store an episodic memory item")
        .def("retrieve_episode", &EpisodicMemory::retrieveEpisode, py::arg("index"),
             "Retrieve an episodic memory item by index")
        .def("get_episode_count", &EpisodicMemory::getEpisodeCount,
             "Get number of stored episodes")
        .def("get_recent_episodes", &EpisodicMemory::getRecentEpisodes, py::arg("count"),
             "Get recent episodes (count most recent)")
        .def("clear", &EpisodicMemory::clear, "Clear all episodic memories")
        .def("consolidate", &EpisodicMemory::consolidate, py::arg("relevance_threshold"),
             "Consolidate memories to long-term storage based on relevance");

    py::class_<SemanticMemory>(m, "SemanticMemory", R"pbdoc(Semantic memory system for facts and knowledge)pbdoc")
        .def(py::init<>())
        .def("store_fact", &SemanticMemory::storeFact, py::arg("key"), py::arg("value"),
             "Store a factual knowledge item")
        .def("retrieve_fact", &SemanticMemory::retrieveFact, py::arg("key"),
             "Retrieve a factual knowledge item by key")
        .def("has_fact", &SemanticMemory::hasFact, py::arg("key"),
             "Check if semantic memory contains a fact")
        .def("get_all_facts", &SemanticMemory::getAllFacts,
             "Get all stored facts as key-value pairs")
        .def("clear", &SemanticMemory::clear, "Clear all semantic memories");

    py::class_<Skill>(m, "Skill", R"pbdoc(Skill in procedural memory)pbdoc")
        .def(py::init<>())
        .def_readwrite("name", &Skill::name)
        .def_readwrite("neural_pattern", &Skill::neuralPattern)
        .def_readwrite("proficiency", &Skill::proficiency);

    py::class_<ProceduralMemory>(m, "ProceduralMemory", R"pbdoc(Procedural memory system for learned skills and habits)pbdoc")
        .def(py::init<>())
        .def("learn_skill", &ProceduralMemory::learnSkill, py::arg("name"), py::arg("pattern"),
             "Learn a new skill")
        .def("get_skill", &ProceduralMemory::getSkill, py::arg("name"),
             "Get a skill by name")
        .def("get_all_skills", &ProceduralMemory::getAllSkills,
             "Get all learned skills")
        .def("update_proficiency", &ProceduralMemory::updateProficiency, py::arg("name"), py::arg("delta"),
             "Update proficiency of a skill")
        .def("clear", &ProceduralMemory::clear, "Clear all skills");

    py::class_<AssociativeMemory>(m, "AssociativeMemory", R"pbdoc(Associative memory system for relationships between representations)pbdoc")
        .def(py::init<>())
        .def("associate", &AssociativeMemory::associate, py::arg("a"), py::arg("b"), py::arg("strength"),
             "Create association between two neurons")
        .def("get_associations", &AssociativeMemory::getAssociations, py::arg("neuron"),
             "Get all neurons associated with the given neuron")
        .def("get_association_strength", &AssociativeMemory::getAssociationStrength,
             py::arg("a"), py::arg("b"), "Get strength of association between two neurons")
        .def("update_association", &AssociativeMemory::updateAssociation,
             py::arg("a"), py::arg("b"), py::arg("delta"), "Update association strength")
        .def("clear", &AssociativeMemory::clear, "Clear all associations");

    // Neuromodulation systems bindings
    py::class_<Dopamine>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator for reward and reinforcement)pbdoc")
        .def(py::init<>())
        .def("signal_reward", &Dopamine::signalReward, py::arg("reward"),
             "Signal a reward event")
        .def("signal_reward_prediction_error", &Dopamine::signalRewardPredictionError, py::arg("error"),
             "Signal a reward prediction error")
        .def("get_name", &Dopamine::getName)
        .def("get_level", &Dopamine::getLevel)
        .def("set_level", &Dopamine::setLevel, py::arg("level"))
        .def("get_plasticity_factor", &Dopamine::getPlasticityFactor)
        .def("update", &Dopamine::update, py::arg("dt"));

    py::class_<Acetylcholine>(m, "Acetylcholine", R"pbdoc(Acetylcholine neuromodulator for attention and memory)pbdoc")
        .def(py::init<>())
        .def("get_name", &Acetylcholine::getName)
        .def("get_level", &Acetylcholine::getLevel)
        .def("set_level", &Acetylcholine::setLevel, py::arg("level"))
        .def("get_plasticity_factor", &Acetylcholine::getPlasticityFactor)
        .def("update", &Acetylcholine::update, py::arg("dt"));

    py::class_<Norepinephrine>(m, "Norepinephrine", R"pbdoc(Norepinephrine neuromodulator for arousal and vigilance)pbdoc")
        .def(py::init<>())
        .def("get_name", &Norepinephrine::getName)
        .def("get_level", &Norepinephrine::getLevel)
        .def("set_level", &Norepinephrine::setLevel, py::arg("level"))
        .def("get_plasticity_factor", &Norepinephrine::getPlasticityFactor)
        .def("update", &Norepinephrine::update, py::arg("dt"));

    py::class_<Serotonin>(m, "Serotonin", R"pbdoc(Serotonin neuromodulator for mood and social behavior)pbdoc")
        .def(py::init<>())
        .def("get_name", &Serotonin::getName)
        .def("get_level", &Serotonin::getLevel)
        .def("set_level", &Serotonin::setLevel, py::arg("level"))
        .def("get_plasticity_factor", &Serotonin::getPlasticityFactor)
        .def("update", &Serotonin::update, py::arg("dt"));

    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(Abstract base class for neuromodulators)pbdoc")
        .def("get_name", &Neuromodulator::getName)
        .def("get_level", &Neuromodulator::getLevel)
        .def("set_level", &Neuromodulator::setLevel, py::arg("level"))
        .def("get_plasticity_factor", &Neuromodulator::getPlasticityFactor)
        .def("update", &Neuromodulator::update, py::arg("dt"));

    // Development systems bindings
    py::class_<Synaptogenesis>(m, "Synaptogenesis", R"pbdoc(Developmental system for synapse formation)pbdoc")
        .def(py::init<>());

    py::class_<Pruning>(m, "Pruning", R"pbdoc(Developmental system for synaptic pruning)pbdoc")
        .def(py::init<>());

    py::class_<Maturation>(m, "Maturation", R"pbdoc(Developmental system for neural maturation)pbdoc")
        .def(py::init<>());

    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Development system for orchestrating neural development)pbdoc")
        .def(py::init<>())
        .def("get_stage", &DevelopmentSystem::getStage)
        .def("set_stage", &DevelopmentSystem::setStage, py::arg("stage"))
        .def("advance_stage", &DevelopmentSystem::advanceStage)
        .def("get_stage_name", &DevelopmentSystem::getStageName)
        .def("update", static_cast<void(DevelopmentSystem::*)(Brain*, SimulationStep)>
                   (&DevelopmentSystem::update), py::arg("brain"), py::arg("current_step"))
        .def("update_full", static_cast<void(DevelopmentSystem::*)(Brain*, RandomGenerator&, TimestepDuration)>
                   (&DevelopmentSystem::update), py::arg("brain"), py::arg("rng"), py::arg("dt"))
        .def("get_plasticity_modifier", &DevelopmentSystem::getPlasticityModifier)
        .def("is_critical_period", &DevelopmentSystem::isCriticalPeriod)
        .def("get_critical_period_progress", &DevelopmentSystem::getCriticalPeriodProgress)
        .def("get_developmental_age", &DevelopmentSystem::getDevelopmentalAge)
        .def("set_developmental_age", &DevelopmentSystem::setDevelopmentalAge, py::arg("age"));

    // Cognition systems bindings
    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation system)pbdoc")
        .def(py::init<>());

    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planning system)pbdoc")
        .def(py::init<>());

    // Attention system bindings (using AttentionalSelection from neural working memory)
    py::class_<AttentionalSelection>(m, "AttentionalSelection", R"pbdoc(Neural attention through competition)pbdoc")
        .def(py::init<>());

    // Prediction systems bindings
    py::class_<NeuralPrediction>(m, "NeuralPrediction", R"pbdoc(Neural prediction system)pbdoc")
        .def(py::init<>());

    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Prediction system for predictive coding)pbdoc")
        .def(py::init<>())
        .def("predict_next_state", &PredictionSystem::predictNextState, py::arg("current_state"),
             "Predict the next sensory state")
        .def("update_predictions", &PredictionSystem::updatePredictions,
             py::arg("predicted"), py::arg("actual"), "Update predictions based on actual observation")
        .def("get_prediction_error", &PredictionSystem::getPredictionError,
             "Get current prediction error")
        .def("get_confidence", &PredictionSystem::getConfidence,
             "Get prediction confidence")
        .def("get_error_history", &PredictionSystem::getErrorHistory,
             "Get prediction error history")
        .def("clear_history", &PredictionSystem::clearHistory,
             "Clear prediction error history")
        .def("train", &PredictionSystem::train, py::arg("observation"), "Train prediction model");

    // Add memory and neuromodulation to Brain class
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
