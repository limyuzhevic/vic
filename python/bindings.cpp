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
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/AttentionalSelection.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/NeuralAssociativeMemory.hpp"
#include "../src/dynamics/SpikeSystem.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/dynamics/NeuralDynamics.hpp"

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

    // Neural Working Memory for temporary storage
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Neural working memory for temporary information storage)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"),
             "Initialize working memory with brain reference")
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron, py::arg("neuronId"), py::arg("activation"),
             "Store neural activation in working memory")
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces,
             "Get number of active memory traces")
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons,
             "Get neurons currently in memory")
        .def("getMemoryCapacity", &NeuralWorkingMemory::getMemoryCapacity,
             "Get working memory capacity")
        .def("setCapacity", &NeuralWorkingMemory::setCapacity, py::arg("capacity"),
             "Set working memory capacity")
        .def("getMemoryStrength", &NeuralWorkingMemory::getMemoryStrength, py::arg("neuronId"),
             "Get memory strength for a neuron")
        .def("update", &NeuralWorkingMemory::update, py::arg("dt"), "Update working memory")
        .def("getMemoryAge", &NeuralWorkingMemory::getMemoryAge, py::arg("neuronId"),
             "Get age of memory trace")
        .def("reset", &NeuralWorkingMemory::reset, "Reset working memory");

    // Neural Episodic Memory for experience storage
    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Neural episodic memory for experience storage)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralEpisodicMemory::initialize, py::arg("brain"),
             "Initialize episodic memory with brain reference")
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode, py::arg("episode"),
             "Store an episodic memory")
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount,
             "Get number of stored episodes")
        .def("getMaxEpisodes", &NeuralEpisodicMemory::getMaxEpisodes,
             "Get maximum number of episodes")
        .def("setMaxEpisodes", &NeuralEpisodicMemory::setMaxEpisodes, py::arg("maxEpisodes"),
             "Set maximum number of episodes")
        .def("getEpisodesForReplay", &NeuralEpisodicMemory::getEpisodesForReplay, py::arg("count"),
             "Get episodes for replay")
        .def("replayEpisode", &NeuralEpisodicMemory::replayEpisode, py::arg("episode"),
             "Replay an episodic memory")
        .def("consolidate", &NeuralEpisodicMemory::consolidate, py::arg("threshold"),
             "Consolidate important memories")
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount,
             "Get number of stored episodes")
        .def("reset", &NeuralEpisodicMemory::reset, "Reset episodic memory");

    // Neural Associative Memory for pattern associations
    py::class_<NeuralAssociativeMemory>(m, "NeuralAssociativeMemory", R"pbdoc(Neural associative memory for pattern associations)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralAssociativeMemory::initialize, py::arg("brain"),
             "Initialize associative memory with brain reference")
        .def("storeAssociation", &NeuralAssociativeMemory::storeAssociation, py::arg("pattern"), py::arg("associatedPattern"),
             "Store an associative memory between two patterns")
        .def("retrieve", &NeuralAssociativeMemory::retrieve, py::arg("pattern"),
             "Retrieve associated pattern for given pattern")
        .def("getAssociationCount", &NeuralAssociativeMemory::getAssociationCount,
             "Get number of stored associations")
        .def("clearAssociations", &NeuralAssociativeMemory::clearAssociations,
             "Clear all associations")
        .def("getActivationStrength", &NeuralAssociativeMemory::getActivationStrength, py::arg("pattern1"), py::arg("pattern2"),
             "Get activation strength between patterns")
        .def("reset", &NeuralAssociativeMemory::reset, "Reset associative memory");

    // Neural systems
    py::class_<PlanningCandidate>(m, "PlanningCandidate", R"pbdoc(A possible action sequence considered during planning)pbdoc")
        .def(py::init<>())
        .def_readwrite("actions", &PlanningCandidate::actions)
        .def_readwrite("predictedStates", &PlanningCandidate::predictedStates)
        .def_readwrite("expectedReward", &PlanningCandidate::expectedReward)
        .def_readwrite("confidence", &PlanningCandidate::confidence)
        .def_readwrite("depth", &PlanningCandidate::depth);

    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planner: uses predictions to plan multi-step actions)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPlanner::initialize, py::arg("brain"), R"pbdoc(Initialize with brain reference)pbdoc")
        .def("planAction", &NeuralPlanner::planAction, py::arg("currentState"), py::arg("targetReward") = 0.5f, R"pbdoc(Plan next action)pbdoc")
        .def("evaluateSequence", &NeuralPlanner::evaluateSequence, py::arg("actions"), py::arg("startState"), R"pbdoc(Evaluate action sequence)pbdoc")
        .def("getPlanningDepth", &NeuralPlanner::getPlanningDepth, R"pbdoc(Get planning depth)pbdoc")
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth, py::arg("depth"), R"pbdoc(Set planning depth)pbdoc")
        .def("getPlanningConfidence", &NeuralPlanner::getPlanningConfidence, R"pbdoc(Get planning confidence)pbdoc")
        .def("updatePlanQuality", &NeuralPlanner::updatePlanQuality, py::arg("plannedActions"), py::arg("actualActions"), py::arg("actualReward"), R"pbdoc(Update plan quality)pbdoc")
        .def("clearCache", &NeuralPlanner::clearCache, R"pbdoc(Clear planning cache)pbdoc")
        .def("setActionQuality", &NeuralPlanner::setActionQuality, py::arg("action"), py::arg("quality"), R"pbdoc(Set action quality)pbdoc")
        .def("getCurrentGoal", &NeuralPlanner::getCurrentGoal, R"pbdoc(Get current goal)pbdoc")
        .def("setCurrentGoal", &NeuralPlanner::setCurrentGoal, py::arg("goal"), R"pbdoc(Set current goal)pbdoc")
        .def("wasRecentPlanSuccessful", &NeuralPlanner::wasRecentPlanSuccessful, R"pbdoc(Check if recent plan was successful)pbdoc")
        .def("generateActionSequences", &NeuralPlanner::generateActionSequences, py::arg("depth"), R"pbdoc(Generate action sequences)pbdoc")
        .def("evaluateAction", &NeuralPlanner::evaluateAction, py::arg("action"), py::arg("state"), R"pbdoc(Evaluate single action)pbdoc");

    // Self model
    py::class_<SelfModel>(m, "SelfModel", R"pbdoc(Self model: agent's internal model of itself)pbdoc")
        .def(py::init<>())
        .def("initialize", &SelfModel::initialize, py::arg("brain"), R"pbdoc(Initialize with brain reference)pbdoc")
        .def("recordSelfAction", &SelfModel::recordSelfAction, py::arg("action"), py::arg("beforeState"), py::arg("afterState"), R"pbdoc(Record self action)pbdoc")
        .def("predictActionConsequence", &SelfModel::predictActionConsequence, py::arg("action"), py::arg("currentState"), R"pbdoc(Predict action consequence)pbdoc")
        .def("getSelfModelConfidence", &SelfModel::getSelfModelConfidence, py::arg("action"), R"pbdoc(Get self-model confidence)pbdoc")
        .def("computeSelfGeneratedLikeness", &SelfModel::computeSelfGenerated Likeness, py::arg("beforeState"), py::arg("afterState"), py::arg("action"), R"pbdoc(Compute self-generated likeness)pbdoc")
        .def("getPreferredAction", &SelfModel::getPreferredAction, py::arg("state"), R"pbdoc(Get preferred action)pbdoc")
        .def("updateSelfModel", &SelfModel::updateSelfModel, py::arg("predicted"), py::arg("actual"), py::arg("action"), R"pbdoc(Update self-model)pbdoc")
        .def("getCapabilityLevel", &SelfModel::getCapabilityLevel, R"pbdoc(Get capability level)pbdoc")
        .def("getBodyAwareness", &SelfModel::getBodyAwareness, R"pbdoc(Get body awareness)pbdoc")
        .def("clear", &SelfModel::clear, R"pbdoc(Clear self-model)pbdoc")
        .def("hasSelfModel", &SelfModel::hasSelfModel, R"pbdoc(Check if self-model formed)pbdoc")
        .def("findMatchingEffect", &SelfModel::findMatchingEffect, py::arg("action"), py::arg("beforeState"), R"pbdoc(Find matching effect)pbdoc");

    // Social learning
    py::class_<SocialLearning>(m, "SocialLearning", R"pbdoc(Social learning: learning from other agents)pbdoc")
        .def(py::init<>())
        .def("initialize", &SocialLearning::initialize, py::arg("brain"), R"pbdoc(Initialize with brain reference)pbdoc")
        .def("observeAgentAction", &SocialLearning::observeAgentAction, py::arg("observedAction"), py::arg("observerState"), py::arg("resultingState"), R"pbdoc(Observe agent action)pbdoc")
        .def("canImitate", &SocialLearning::canImitate, py::arg("observedAction"), R"pbdoc(Check if can imitate)pbdoc")
        .def("getImitationAction", &SocialLearning::getImitationAction, py::arg("currentState"), R"pbdoc(Get imitation action)pbdoc")
        .def("learnCommunicationSignal", &SocialLearning::learnCommunicationSignal, py::arg("signalPattern"), py::arg("signalReward"), R"pbdoc(Learn communication signal)pbdoc")
        .def("detectSignal", &SocialLearning::detectSignal, py::arg("neuralPattern"), R"pbdoc(Detect signal)pbdoc")
        .def("getSignalPattern", &SocialLearning::getSignalPattern, R"pbdoc(Get signal pattern)pbdoc")
        .def("getSignalMeaning", &SocialLearning::getSignalMeaning, R"pbdoc(Get signal meaning)pbdoc")
        .def("updateSocialKnowledge", &SocialLearning::updateSocialKnowledge, py::arg("interactionReward"), R"pbdoc(Update social knowledge)pbdoc")
        .def("clear", &SocialLearning::clear, R"pbdoc(Clear social learning)pbdoc")
        .def("hasSocialKnowledge", &SocialLearning::hasSocialKnowledge, R"pbdoc(Check if has social knowledge)pbdoc")
        .def("getObservationCount", &SocialLearning::getObservationCount, R"pbdoc(Get observation count)pbdoc");

    // Concept formation
    py::class_<ConceptInstance>(m, "ConceptInstance", R"pbdoc(A single instance of a concept)pbdoc")
        .def(py::init<>())
        .def_readwrite("pattern", &ConceptInstance::pattern)
        .def_readwrite("features", &ConceptInstance::features)
        .def_readwrite("observationCount", &ConceptInstance::observationCount)
        .def_readwrite("lastObserved", &ConceptInstance::lastObserved)
        .def_readwrite("avgReward", &ConceptInstance::avgReward)
        .def_readwrite("stability", &ConceptInstance::stability);

    py::class_<DiscoveredConcept>(m, "DiscoveredConcept", R"pbdoc(A concept discovered from experience)pbdoc")
        .def(py::init<>())
        .def_readwrite("id", &DiscoveredConcept::id)
        .def_readwrite("prototype", &DiscoveredConcept::prototype)
        .def("getInstances", &DiscoveredConcept::instances, R"pbdoc(Get concept instances)pbdoc")
        .def("getTotalObservations", &DiscoveredConcept::totalObservations, R"pbdoc(Get total observations)pbdoc")
        .def("getAverageStability", &DiscoveredConcept::avgStability, R"pbdoc(Get average stability)pbdoc")
        .def_readwrite("categoryHint", &DiscoveredConcept::categoryHint)
        .def("getAssociatedConceptIds", &DiscoveredConcept::associatedConceptIds, R"pbdoc(Get associated concept IDs)pbdoc")
        .def("update", &DiscoveredConcept::update, R"pbdoc(Update concept)pbdoc");

    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation: discovers recurring patterns)pbdoc")
        .def(py::init<>())
        .def("initialize", &ConceptFormation::initialize, py::arg("brain"), R"pbdoc(Initialize concept formation)pbdoc")
        .def("presentExperience", &ConceptFormation::presentExperience, py::arg("pattern"), py::arg("features"), py::arg("reward"), py::arg("currentTime"), R"pbdoc(Present experience)pbdoc")
        .def("getMatchingConcept", &ConceptFormation::getMatchingConcept, py::arg("pattern"), py::arg("similarityThreshold") = 0.7f, R"pbdoc(Get matching concept)pbdoc")
        .def("getConcepts", &ConceptFormation::getConcepts, R"pbdoc(Get all concepts)pbdoc")
        .def("getConcept", &ConceptFormation::getConcept, py::arg("conceptId"), R"pbdoc(Get specific concept)pbdoc")
        .def("getConceptPrototype", &ConceptFormation::getConceptPrototype, py::arg("conceptId"), R"pbdoc(Get concept prototype)pbdoc")
        .def("getConceptInstances", &ConceptFormation::getConceptInstances, py::arg("conceptId"), R"pbdoc(Get concept instances)pbdoc")
        .def("getConceptStability", &ConceptFormation::getConceptStability, py::arg("conceptId"), R"pbdoc(Get concept stability)pbdoc")
        .def("mergeConcepts", &ConceptFormation::mergeConcepts, py::arg("conceptA"), py::arg("conceptB"), R"pbdoc(Merge two concepts)pbdoc")
        .def("updateConcept", &ConceptFormation::updateConcept, py::arg("conceptId"), py::arg("newPattern"), py::arg("features"), py::arg("reward"), R"pbdoc(Update concept)pbdoc")
        .def("computeSimilarity", &ConceptFormation::computeSimilarity, py::arg("a"), py::arg("b"), R"pbdoc(Compute similarity between patterns)pbdoc")
        .def("isNovel", &ConceptFormation::isNovel, py::arg("pattern"), py::arg("similarityThreshold") = 0.7f, R"pbdoc(Check if pattern is novel)pbdoc")
        .def("findConceptForPattern", &ConceptFormation::findConceptForPattern, py::arg("pattern"), R"pbdoc(Find concept for pattern)pbdoc")
        .def("clear", &ConceptFormation::clear, R"pbdoc(Clear all concepts)pbdoc")
        .def("getGeneralizationAbility", &ConceptFormation::getGeneralizationAbility, py::arg("conceptId"), R"pbdoc(Get generalization ability)pbdoc")
        .def("setFormationThreshold", &ConceptFormation::setFormationThreshold, py::arg("threshold"), R"pbdoc(Set formation threshold)pbdoc")
        .def("setStabilityWindow", &ConceptFormation::setStabilityWindow, py::arg("window"), R"pbdoc(Set stability window)pbdoc");

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
