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

    // Add advanced integration systems for power users
    py::class_<Brain::Impl>(m, "BrainImpl", R"pbdoc(Internal brain implementation structure)pbdoc")
        .def_readonly("workingMemory", &Brain::Impl::workingMemory)
        .def_readonly("episodicMemory", &Brain::Impl::episodicMemory)
        .def_readonly("associativeMemory", &Brain::Impl::associativeMemory)
        .def_readonly("predictionSystem", &Brain::Impl::predictionSystem)
        .def_readonly("planner", &Brain::Impl::planner)
        .def_readonly("conceptFormation", &Brain::Impl::conceptFormation)
        .def_readonly("attention", &Brain::Impl::attention)
        .def_readonly("developmentSystem", &Brain::Impl::developmentSystem)
        .def_readonly("dopamine", &Brain::Impl::dopamine)
        .def_readonly("curiosity", &Brain::Impl::curiosity)
        .def_readonly("novelty", &Brain::Impl::novelty)
        .def_readonly("predictionError", &Brain::Impl::predictionError);

    // Advanced memory systems bindings
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Neural working memory system)pbdoc")
        .def("store", &NeuralWorkingMemory::store, py::arg("pattern"), py::arg("strength") = 1.0f)
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron, py::arg("neuron"), py::arg("activation"))
        .def("retrieve", &NeuralWorkingMemory::retrieve)
        .def("contains", &NeuralWorkingMemory::contains, py::arg("neuron"))
        .def("getNeuronActivation", &NeuralWorkingMemory::getNeuronActivation, py::arg("neuron"))
        .def("update", &NeuralWorkingMemory::update, py::arg("dt"))
        .def("clear", &NeuralWorkingMemory::clear)
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces);

    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Neural episodic memory system)pbdoc")
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode, py::arg("episode"))
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount)
        .def("getEpisodesForReplay", &NeuralEpisodicMemory::getEpisodesForReplay, py::arg("maxEpisodes"))
        .def("replayEpisode", &NeuralEpisodicMemory::replayEpisode, py::arg("episode"))
        .def("consolidate", &NeuralEpisodicMemory::consolidate, py::arg("threshold"))
        .def("clear", &NeuralEpisodicMemory::clear)
        .def("getMaxEpisodes", &NeuralEpisodicMemory::getMaxEpisodes)
        .def("setMaxEpisodes", &NeuralEpisodicMemory::setMaxEpisodes, py::arg("max"));

    py::class_<NeuralAssociativeMemory>(m, "NeuralAssociativeMemory", R"pbdoc(Neural associative memory system)pbdoc")
        .def("storeAssociation", &NeuralAssociativeMemory::storeAssociation, 
             py::arg("pattern1"), py::arg("pattern2"), py::arg("strength") = 1.0f)
        .def("recallAssociation", &NeuralAssociativeMemory::recallAssociation, 
             py::arg("pattern"), py::arg("threshold") = 0.5f)
        .def("getAssociationStrength", &NeuralAssociativeMemory::getAssociationStrength,
             py::arg("pattern1"), py::arg("pattern2"))
        .def("update", &NeuralAssociativeMemory::update, py::arg("dt"))
        .def("clear", &NeuralAssociativeMemory::clear);

    // Advanced prediction system bindings
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(High-level prediction system interface)pbdoc")
        .def("predictNextState", &PredictionSystem::predictNextState, py::arg("currentState"))
        .def("updatePredictions", &PredictionSystem::updatePredictions, 
             py::arg("predicted"), py::arg("actual"))
        .def("getPredictionError", &PredictionSystem::getPredictionError)
        .def("getConfidence", &PredictionSystem::getConfidence)
        .def("getErrorHistory", &PredictionSystem::getErrorHistory)
        .def("clearHistory", &PredictionSystem::clearHistory)
        .def("train", &PredictionSystem::train, py::arg("observation"));

    py::class_<NeuralPrediction>(m, "NeuralPrediction", R"pbdoc(Neural prediction system using spiking dynamics)pbdoc")
        .def("initialize", &NeuralPrediction::initialize, py::arg("brain"))
        .def("recordSensoryState", &NeuralPrediction::recordSensoryState,
             py::arg("sensoryState"), py::arg("currentStep"))
        .def("generatePrediction", &NeuralPrediction::generatePrediction, py::arg("currentStep"))
        .def("updateWithObservation", &NeuralPrediction::updateWithObservation,
             py::arg("actualState"), py::arg("currentStep"))
        .def("predictActionConsequence", &NeuralPrediction::predictActionConsequence,
             py::arg("action"), py::arg("currentState"))
        .def("getPredictionError", &NeuralPrediction::getPredictionError)
        .def("getPredictionConfidence", &NeuralPrediction::getPredictionConfidence)
        .def("predictMultipleSteps", &NeuralPrediction::predictMultipleSteps,
             py::arg("currentStep"), py::arg("numSteps"))
        .def("recordAction", &NeuralPrediction::recordAction, py::arg("action"), py::arg("step"))
        .def("getErrorHistory", &NeuralPrediction::getErrorHistory)
        .def("clearHistory", &NeuralPrediction::clearHistory)
        .def("enableTemporalPrediction", &NeuralPrediction::enableTemporalPrediction, py::arg("enable"))
        .def("enableActionConsequencePrediction", &NeuralPrediction::enableActionConsequencePrediction, py::arg("enable"))
        .def("setSequenceMemorySize", &NeuralPrediction::setSequenceMemorySize, py::arg("size"))
        .def("setPredictionHorizon", &NeuralPrediction::setPredictionHorizon, py::arg("steps"))
        .def("getPredictionNeurons", &NeuralPrediction::getPredictionNeurons)
        .def("getSequenceNeurons", &NeuralPrediction::getSequenceNeurons);

    // Advanced cognition system bindings
    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planning system for action selection)pbdoc")
        .def("initialize", &NeuralPlanner::initialize, py::arg("brain"))
        .def("evaluateActionSequence", &NeuralPlanner::evaluateActionSequence,
             py::arg("sequence"), py::arg("currentState"))
        .def("planActions", &NeuralPlanner::planActions, py::arg("currentState"), py::arg("horizon"))
        .def("getActionPriority", &NeuralPlanner::getActionPriority, 
             py::arg("action"), py::arg("currentState"))
        .def("getPlanningConfidence", &NeuralPlanner::getPlanningConfidence)
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth, py::arg("depth"))
        .def("getActionSequence", &NeuralPlanner::getActionSequence);

    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation from sensory patterns)pbdoc")
        .def("initialize", &ConceptFormation::initialize, py::arg("brain"))
        .def("processPattern", &ConceptFormation::processPattern,
             py::arg("pattern"), py::arg("step"))
        .def("getConcept", &ConceptFormation::getConcept, py::arg("conceptId"))
        .def("getConceptCount", &ConceptFormation::getConceptCount)
        .def("getConceptSimilarity", &ConceptFormation::getConceptSimilarity,
             py::arg("conceptId1"), py::arg("conceptId2"))
        .def("getConcepts", &ConceptFormation::getConcepts);

    py::class_<AttentionalSelection>(m, "AttentionalSelection", R"pbdoc(Attentional selection system)pbdoc")
        .def("initialize", &AttentionalSelection::initialize, py::arg("brain"))
        .def("update", &AttentionalSelection::update, py::arg("dt"))
        .def("processCompetition", &AttentionalSelection::processCompetition,
             py::arg("competitors"))
        .def("getCurrentFocus", &AttentionalSelection::getCurrentFocus)
        .def("setInhibitionStrength", &AttentionalSelection::setInhibitionStrength, py::arg("strength"))
        .def("setExcitationStrength", &AttentionalSelection::setExcitationStrength, py::arg("strength"))
        .def("getAttentionLevel", &AttentionalSelection::getAttentionLevel);

    // Advanced development system bindings
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Development and maturation system)pbdoc")
        .def("initialize", &DevelopmentSystem::initialize, py::arg("brain"))
        .def("update", &DevelopmentSystem::update, py::arg("brain"), py::arg("rng"), py::arg("dt"))
        .def("getDevelopmentalStage", &DevelopmentSystem::getDevelopmentalStage)
        .def("setDevelopmentalStage", &DevelopmentSystem::setDevelopmentalStage, py::arg("stage"))
        .def("getPlasticityRate", &DevelopmentSystem::getPlasticityRate, py::arg("stage"))
        .def("getCriticalPeriodActive", &DevelopmentSystem::getCriticalPeriodActive)
        .def("getMaturationProgress", &DevelopmentSystem::getMaturationProgress);

    // Advanced neuromodulation system bindings
    py::class_<Dopamine>(m, "Dopamine", R"pbdoc(Dopamine neuromodulation system)pbdoc")
        .def("initialize", &Dopamine::initialize, py::arg("brain"))
        .def("update", &Dopamine::update, py::arg("dt"))
        .def("getLevel", &Dopamine::getLevel)
        .def("getPlasticityFactor", &Dopamine::getPlasticityFactor)
        .def("getExcitabilityFactor", &Dopamine::getExcitabilityFactor)
        .def("getCoherenceLevel", &Dopamine::getCoherenceLevel);

    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity neuromodulation system)pbdoc")
        .def("initialize", &Curiosity::initialize, py::arg("brain"))
        .def("update", &Curiosity::update, py::arg("dt"))
        .def("getLevel", &Curiosity::getLevel)
        .def("getExplorationBias", &Curiosity::getExplorationBias)
        .def("getNoveltyThreshold", &Curiosity::getNoveltyThreshold)
        .def("getSurpriseLevel", &Curiosity::getSurpriseLevel);

    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty detection system)pbdoc")
        .def("initialize", &Novelty::initialize, py::arg("brain"))
        .def("update", &Novelty::update, py::arg("dt"))
        .def("getLevel", &Novelty::getLevel)
        .def("getNoveltyPattern", &Novelty::getNoveltyPattern)
        .def("getSurpriseScore", &Novelty::getSurpriseScore);

    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error signal system)pbdoc")
        .def("initialize", &PredictionError::initialize, py::arg("brain"))
        .def("update", &PredictionError::update, py::arg("dt"))
        .def("computeError", &PredictionError::computeError,
             py::arg("predicted"), py::arg("actual"))
        .def("getModulationSignal", &PredictionError::getModulationSignal)
        .def("recordError", &PredictionError::recordError, py::arg("error"), py::arg("step"))
        .def("getErrorHistory", &PredictionError::getErrorHistory);

    // Advanced configuration for power users
    py::class_<Phase6Config>(m, "Phase6Config", R"pbdoc(Phase 6 integration configuration)pbdoc")
        .def(py::init<>())
        .def_readwrite("neuronCount", &Phase6Config::neuronCount)
        .def_readwrite("regionCount", &Phase6Config::regionCount)
        .def_readwrite("connectionProbability", &Phase6Config::connectionProbability)
        .def_readwrite("maxSteps", &Phase6Config::maxSteps)
        .def_readwrite("enableCheckpointing", &Phase6Config::enableCheckpointing)
        .def_readwrite("enableReplay", &Phase6Config::enableReplay)
        .def_readwrite("enableDevelopment", &Phase6Config::enableDevelopment)
        .def_readwrite("checkpointPath", &Phase6Config::checkpointPath);

    py::class_<Phase6IntegrationResult>(m, "Phase6IntegrationResult", R"pbdoc(Phase 6 integration test results)pbdoc")
        .def_readonly("totalReward", &Phase6IntegrationResult::totalReward)
        .def_readonly("avgFiringRate", &Phase6IntegrationResult::avgFiringRate)
        .def_readonly("memoryEpisodesStored", &Phase6IntegrationResult::memoryEpisodesStored)
        .def_readonly("noveltyLevel", &Phase6IntegrationResult::noveltyLevel)
        .def_readonly("curiosityLevel", &Phase6IntegrationResult::curiosityLevel)
        .def_readonly("dopamineLevel", &Phase6IntegrationResult::dopamineLevel)
        .def_readonly("memoryWorkingMemoryIntegrated", &Phase6IntegrationResult::memoryWorkingMemoryIntegrated)
        .def_readonly("memoryEpisodicMemoryIntegrated", &Phase6IntegrationResult::memoryEpisodicMemoryIntegrated)
        .def_readonly("neuromodulationIntegrated", &Phase6IntegrationResult::neuromodulationIntegrated)
        .def_readonly("predictionIntegrated", &Phase6IntegrationResult::predictionIntegrated)
        .def_readonly("developmentIntegrated", &Phase6IntegrationResult::developmentIntegrated)
        .def_readonly("checkpointingWorks", &Phase6IntegrationResult::checkpointingWorks)
        .def_readonly("totalWallClockTime", &Phase6IntegrationResult::totalWallClockTime);

    // Advanced utility functions for power users
    m.def("createPhase6Config", []() -> Phase6Config {
        return Phase6Config();
    }, "Create Phase 6 configuration with defaults");

    m.def("createBrainFromConfig", [](const Phase6Config& config) -> std::shared_ptr<Brain> {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", config.neuronCount);
        cfg->set("region_count", config.regionCount);
        cfg->set("connection_probability", config.connectionProbability);
        return std::make_shared<Brain>(cfg);
    }, py::arg("config"), "Create brain from Phase 6 configuration");

    // Advanced simulation control
    m.def("runPhase6Integration", [](std::shared_ptr<Brain> brain, const Phase6Config& config) {
        Phase6IntegratedExperiment experiment;
        return experiment.run(config);
    }, py::arg("brain"), py::arg("config"), "Run Phase 6 integration experiment");

    m.def("runQuickIntegrationTest", []() {
        Phase6IntegratedExperiment experiment;
        bool success = experiment.verifyIntegration();
        if (success) {
            experiment.testMemoryIntegration();
            experiment.testNeuromodulationIntegration();
            experiment.testCheckpointing();
            experiment.testReplay();
        }
        return success;
    }, "Run quick integration test");

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

    // Advanced utility functions for power users
    m.def("createAdvancedConfig", [](int neuronCount, int regionCount, float connectionProbability,
                                    bool enablePlasticity, bool enableDevelopment, bool enableCuriosity) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neuronCount);
        config->set("region_count", regionCount);
        config->set("connection_probability", connectionProbability);
        config->set("plasticity.stdp.enable", enablePlasticity);
        config->set("plasticity.hebbian.enable", enablePlasticity);
        config->set("plasticity.structural.enable", enablePlasticity);
        config->set("neuromod.dopamine.scale", 1.0f);
        config->set("neuromod.curiosity.enable", enableCuriosity);
        config->set("neuromod.novelty.enable", true);
        config->set("development.enabled", enableDevelopment);
        
        return config;
    }, py::arg("neuronCount") = 1000, py::arg("regionCount") = 1,
         py::arg("connectionProbability") = 0.1f, py::arg("enablePlasticity") = true,
         py::arg("enableDevelopment") = true, py::arg("enableCuriosity") = true,
         "Create advanced configuration with specified parameters");

    // Learning and adaptation utilities
    m.def("enableLearningMode", [](std::shared_ptr<AgentBrain> agent, bool enable) {
        agent->enableRewardModulation(enable);
        agent->enableStructuralPlasticity(enable);
        agent->enableDevelopment(enable);
        agent->enableCuriosity(enable);
        return true;
    }, py::arg("agent"), py::arg("enable"), "Enable or disable all learning subsystems");

    m.def("getSystemReadinessStatus", [](std::shared_ptr<Brain> brain,
                                        std::shared_ptr<AgentBrain> agent) {
        std::unordered_map<std::string, bool> status;
        
        // Check core systems
        status["brain_initialized"] = brain->getTotalNeuronCount() > 0;
        status["working_memory"] = brain->getWorkingMemory() != nullptr;
        status["episodic_memory"] = brain->getEpisodicMemory() != nullptr;
        status["prediction_system"] = brain->getPredictionSystem() != nullptr;
        status["planner"] = brain->getPlanner() != nullptr;
        status["concept_formation"] = brain->getConceptFormation() != nullptr;
        status["attention"] = brain->getAttention() != nullptr;
        status["development"] = brain->getDevelopmentSystem() != nullptr;
        
        // Check neuromodulation systems
        status["dopamine"] = brain->getDopamine() != nullptr;
        status["curiosity"] = brain->getCuriosity() != nullptr;
        status["novelty"] = brain->getNovelty() != nullptr;
        status["prediction_error"] = brain->getPredictionErrorSignal() != nullptr;
        
        // Check agent systems
        status["agent_brain"] = agent != nullptr;
        status["reward_modulation"] = agent ? agent->isRewardModulationEnabled() : false;
        status["structural_plasticity"] = agent ? agent->isStructuralPlasticityEnabled() : false;
        status["development_enabled"] = agent ? agent->isDevelopmentEnabled() : false;
        status["curiosity_enabled"] = agent ? agent->isCuriosityEnabled() : false;
        
        return status;
    }, py::arg("brain"), py::arg("agent"), "Get comprehensive system readiness status");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
