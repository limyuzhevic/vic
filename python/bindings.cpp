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

    // Neuromodulation
    py::enum_<NeuromodulatorType>(m, "NeuromodulatorType", R"pbdoc(Neuromodulator type enumeration)pbdoc")
        .value("Dopamine", NeuromodulatorType::Dopamine)
        .value("Acetylcholine", NeuromodulatorType::Acetylcholine)
        .value("Norepinephrine", NeuromodulatorType::Norepinephrine)
        .value("Serotonin", NeuromodulatorType::Serotonin)
        .export_values();

    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(Base neuromodulator)pbdoc")
        .def(py::init<>())
        .def("getType", &Neuromodulator::getType)
        .def("getLevel", &Neuromodulator::getLevel)
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor)
        .def("getInfluenceRange", &Neuromodulator::getInfluenceRange)
        .def("update", &Neuromodulator::update, py::arg("dt"));

    py::class_<Dopamine, Neuromodulator>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator)pbdoc")
        .def(py::init<>())
        .def("setLevel", &Dopamine::setLevel, py::arg("level"))
        .def("getPredictionError", &Dopamine::getPredictionError)
        .def("setPredictionError", &Dopamine::setPredictionError, py::arg("error"));

    py::class_<Curiosity, Neuromodulator>(m, "Curiosity", R"pbdoc(Curiosity neuromodulator)pbdoc")
        .def(py::init<>())
        .def("setCuriosityLevel", &Curiosity::setCuriosityLevel, py::arg("level"))
        .def("getExplorationBias", &Curiosity::getExplorationBias)
        .def("setExplorationBias", &Curiosity::setExplorationBias, py::arg("bias"));

    py::class_<Novelty, Neuromodulator>(m, "Novelty", R"pbdoc(Novelty neuromodulator)pbdoc")
        .def(py::init<>())
        .def("setNoveltyLevel", &Novelty::setNoveltyLevel, py::arg("level"))
        .def("getNoveltyThreshold", &Novelty::getNoveltyThreshold)
        .def("setNoveltyThreshold", &Novelty::setNoveltyThreshold, py::arg("threshold"));

    py::class_<PredictionError, Neuromodulator>(m, "PredictionError", R"pbdoc(Prediction error neuromodulator)pbdoc")
        .def(py::init<>())
        .def("setError", &PredictionError::setError, py::arg("error"))
        .def("getError", &PredictionError::getError)
        .def("setTemporalDifference", &PredictionError::setTemporalDifference, py::arg("td"));

    py::class_<Acetylcholine, Neuromodulator>(m, "Acetylcholine", R"pbdoc(Acetylcholine neuromodulator)pbdoc")
        .def(py::init<>())
        .def("setAttentionModulation", &Acetylcholine::setAttentionModulation, py::arg("mod"))
        .def("getMemoryConsolidation", &Acetylcholine::getMemoryConsolidation);

    py::class_<Norepinephrine, Neuromodulator>(m, "Norepinephrine", R"pbdoc(Norepinephrine neuromodulator)pbdoc")
        .def(py::init<>())
        .def("setArousal", &Norepinephrine::setArousal, py::arg("arousal"))
        .def("getStressLevel", &Norepinephrine::getStressLevel)
        .def("setStressLevel", &Norepinephrine::setStressLevel, py::arg("stress"));

    py::class_<Serotonin, Neuromodulator>(m, "Serotonin", R"pbdoc(Serotonin neuromodulator)pbdoc")
        .def(py::init<>())
        .def("setMood", &Serotonin::setMood, py::arg("mood"))
        .def("getRewardSensitivity", &Serotonin::getRewardSensitivity)
        .def("setRewardSensitivity", &Serotonin::setRewardSensitivity, py::arg("sensitivity"));

    // Memory systems
    py::class_<WorkingMemory>(m, "WorkingMemory", R"pbdoc(Working memory system)pbdoc")
        .def(py::init<>())
        .def("store", &WorkingMemory::store, py::arg("neuron"), py::arg("value"))
        .def("retrieve", &WorkingMemory::retrieve, py::arg("neuron"))
        .def("contains", &WorkingMemory::contains, py::arg("neuron"))
        .def("getCapacity", &WorkingMemory::getCapacity)
        .def("setCapacity", &WorkingMemory::setCapacity, py::arg("cap"))
        .def("getCurrentSize", &WorkingMemory::getCurrentSize)
        .def("clear", &WorkingMemory::clear)
        .def("decay", &WorkingMemory::decay, py::arg("decayRate"))
        .def("update", &WorkingMemory::update, py::arg("dt"));

    py::class_<EpisodicMemory>(m, "EpisodicMemory", R"pbdoc(Episodic memory system)pbdoc")
        .def(py::init<>())
        .def("storeEpisode", &EpisodicMemory::storeEpisode, py::arg("episode"))
        .def("getEpisodeCount", &EpisodicMemory::getEpisodeCount)
        .def("retrieveEpisode", &EpisodicMemory::retrieveEpisode, py::arg("index"))
        .def("getRecentEpisodes", &EpisodicMemory::getRecentEpisodes, py::arg("count"))
        .def("clear", &EpisodicMemory::clear)
        .def("consolidate", &EpisodicMemory::consolidate, py::arg("threshold"))
        .def("replayEpisode", &EpisodicMemory::replayEpisode, py::arg("episode"));

    py::class_<SemanticMemory>(m, "SemanticMemory", R"pbdoc(Semantic memory system)pbdoc")
        .def(py::init<>())
        .def("storeFact", &SemanticMemory::storeFact, py::arg("key"), py::arg("value"))
        .def("retrieveFact", &SemanticMemory::retrieveFact, py::arg("key"))
        .def("hasFact", &SemanticMemory::hasFact, py::arg("key"))
        .def("getAllFacts", &SemanticMemory::getAllFacts)
        .def("clear", &SemanticMemory::clear);

    py::class_<ProceduralMemory>(m, "ProceduralMemory", R"pbdoc(Procedural memory system)pbdoc")
        .def(py::init<>())
        .def("learnSkill", &ProceduralMemory::learnSkill, py::arg("name"), py::arg("pattern"))
        .def("getSkill", &ProceduralMemory::getSkill, py::arg("name"))
        .def("updateProficiency", &ProceduralMemory::updateProficiency, py::arg("name"), py::arg("delta"))
        .def("clear", &ProceduralMemory::clear);

    py::class_<AssociativeMemory>(m, "AssociativeMemory", R"pbdoc(Associative memory system)pbdoc")
        .def(py::init<>())
        .def("associate", &AssociativeMemory::associate, py::arg("a"), py::arg("b"), py::arg("strength"))
        .def("getAssociations", &AssociativeMemory::getAssociations, py::arg("neuron"))
        .def("getAssociationStrength", &AssociativeMemory::getAssociationStrength, py::arg("a"), py::arg("b"))
        .def("updateAssociation", &AssociativeMemory::updateAssociation, py::arg("a"), py::arg("b"), py::arg("delta"))
        .def("clear", &AssociativeMemory::clear);

    // Cognition systems
    py::class_<AttentionSystem>(m, "AttentionSystem", R"pbdoc(Attention system)pbdoc")
        .def(py::init<>())
        .def("processCompetition", &AttentionSystem::processCompetition, py::arg("competitors"), py::arg("globalInhibition") = 0.5f)
        .def("focusOnRegion", &AttentionSystem::focusOnRegion, py::arg("region"))
        .def("releaseAttention", &AttentionSystem::releaseAttention)
        .def("getWinners", &AttentionSystem::getWinners)
        .def("update", &AttentionSystem::update, py::arg("dt"))
        .def("reset", &AttentionSystem::reset);

    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planner)pbdoc")
        .def(py::init<>())
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth, py::arg("depth"))
        .def("planAction", &NeuralPlanner::planAction, py::arg("context"))
        .def("update", &NeuralPlanner::update, py::arg("dt"))
        .def("initialize", &NeuralPlanner::initialize, py::arg("brain"));

    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation)pbdoc")
        .def(py::init<>())
        .def("processPattern", &ConceptFormation::processPattern, py::arg("pattern"))
        .def("getConcepts", &ConceptFormation::getConcepts)
        .def("update", &ConceptFormation::update, py::arg("dt"))
        .def("initialize", &ConceptFormation::initialize, py::arg("brain"));

    // Prediction system
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Prediction system)pbdoc")
        .def(py::init<>())
        .def("predictNext", &PredictionSystem::predictNext, py::arg("input"))
        .def("computeError", &PredictionSystem::computeError, py::arg("prediction"), py::arg("actual"))
        .def("getConfidence", &PredictionSystem::getConfidence)
        .def("update", &PredictionSystem::update, py::arg("dt"))
        .def("initialize", &PredictionSystem::initialize, py::arg("brain"));

    py::class_<NeuralPrediction>(m, "NeuralPrediction", R"pbdoc(Neural prediction)pbdoc")
        .def(py::init<>())
        .def("train", &NeuralPrediction::train, py::arg("sequence"), py::arg("target"))
        .def("predict", &NeuralPrediction::predict, py::arg("input"))
        .def("getWeight", &NeuralPrediction::getWeight)
        .def("update", &NeuralPrediction::update, py::arg("dt"));

    // Development system
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Development system)pbdoc")
        .def(py::init<>())
        .def("update", &DevelopmentSystem::update, py::arg("brain"), py::arg("rng"), py::arg("dt"))
        .def("getStage", &DevelopmentSystem::getStage)
        .def("setStage", &DevelopmentSystem::setStage, py::arg("stage"))
        .def("initialize", &DevelopmentSystem::initialize, py::arg("brain"));

    // Performance systems
    py::class_<MemoryPool>(m, "MemoryPool", R"pbdoc(Memory pool for performance)pbdoc")
        .def(py::init<>())
        .def("allocate", &MemoryPool::allocate, py::arg("size"))
        .def("free", &MemoryPool::free)
        .def("optimize", &MemoryPool::optimize)
        .def("getStats", &MemoryPool::getStats);

    py::class_<EventQueue>(m, "EventQueue", R"pbdoc(Event queue for performance)pbdoc")
        .def(py::init<>())
        .def("push", &EventQueue::push, py::arg("event"))
        .def("pop", &EventQueue::pop)
        .def("size", &EventQueue::size)
        .def("clear", &EventQueue::clear)
        .def("update", &EventQueue::update, py::arg("dt"));

    py::class_<SparseConnectivity>(m, "SparseConnectivity", R"pbdoc(Sparse connectivity for performance)pbdoc")
        .def(py::init<>())
        .def("addConnection", &SparseConnectivity::addConnection, py::arg("from"), py::arg("to"), py::arg("weight"))
        .def("getConnections", &SparseConnectivity::getConnections, py::arg("from"))
        .def("computeSparseMatrix", &SparseConnectivity::computeSparseMatrix)
        .def("initialize", &SparseConnectivity::initialize, py::arg("brain"));

    // Experimental framework
    py::class_<Experiment>(m, "Experiment", R"pbdoc(Experiment framework)pbdoc")
        .def(py::init<>())
        .def("run", &Experiment::run, py::arg("brain"), py::arg("config"))
        .def("getResults", &Experiment::getResults)
        .def("saveResults", &Experiment::saveResults, py::arg("filepath"))
        .def("loadResults", &Experiment::loadResults, py::arg("filepath"));

    // Python module setup
    m.def("createWorkingMemory", []() -> std::shared_ptr<WorkingMemory> {
        return std::make_shared<WorkingMemory>();
    }, "Create a working memory system");

    m.def("createEpisodicMemory", []() -> std::shared_ptr<EpisodicMemory> {
        return std::make_shared<EpisodicMemory>();
    }, "Create an episodic memory system");

    m.def("createSemanticMemory", []() -> std::shared_ptr<SemanticMemory> {
        return std::make_shared<SemanticMemory>();
    }, "Create a semantic memory system");

    m.def("createProceduralMemory", []() -> std::shared_ptr<ProceduralMemory> {
        return std::make_shared<ProceduralMemory>();
    }, "Create a procedural memory system");

    m.def("createAssociativeMemory", []() -> std::shared_ptr<AssociativeMemory> {
        return std::make_shared<AssociativeMemory>();
    }, "Create an associative memory system");

    m.def("createAttentionSystem", []() -> std::shared_ptr<AttentionSystem> {
        return std::make_shared<AttentionSystem>();
    }, "Create an attention system");

    m.def("createNeuralPlanner", []() -> std::shared_ptr<NeuralPlanner> {
        return std::make_shared<NeuralPlanner>();
    }, "Create a neural planner");

    m.def("createConceptFormation", []() -> std::shared_ptr<ConceptFormation> {
        return std::make_shared<ConceptFormation>();
    }, "Create a concept formation system");

    m.def("createPredictionSystem", []() -> std::shared_ptr<PredictionSystem> {
        return std::make_shared<PredictionSystem>();
    }, "Create a prediction system");

    m.def("createNeuralPrediction", []() -> std::shared_ptr<NeuralPrediction> {
        return std::make_shared<NeuralPrediction>();
    }, "Create a neural prediction system");

    m.def("createDevelopmentSystem", []() -> std::shared_ptr<DevelopmentSystem> {
        return std::make_shared<DevelopmentSystem>();
    }, "Create a development system");

    m.def("createMemoryPool", []() -> std::shared_ptr<MemoryPool> {
        return std::make_shared<MemoryPool>();
    }, "Create a memory pool");

    m.def("createEventQueue", []() -> std::shared_ptr<EventQueue> {
        return std::make_shared<EventQueue>();
    }, "Create an event queue");

    m.def("createSparseConnectivity", []() -> std::shared_ptr<SparseConnectivity> {
        return std::make_shared<SparseConnectivity>();
    }, "Create sparse connectivity");

    m.def("createExperiment", []() -> std::shared_ptr<Experiment> {
        return std::make_shared<Experiment>();
    }, "Create an experiment");

    // Module attributes
    m.attr("VERSION") = "Phase 6.0 - Complete Integration";
    m.attr("AUTHOR") = "NLM Research Team";
    m.attr("EMAIL") = "research@nlm.org";
}

} // namespace nlm
