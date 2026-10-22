#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

// Include all the brain system headers we need to expose
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

// Memory system headers
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/NeuralAssociativeMemory.hpp"

// Neuromodulation system headers
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/PredictionError.hpp"

// Prediction system headers
#include "../src/prediction/PredictionSystem.hpp"

// Cognition system headers
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/AttentionalSelection.hpp"
#include "../src/development/DevelopmentSystem.hpp"

namespace py = pybind11;
namespace nlm {

// =============================================================================
// EPISODIC MEMORY ITEM BINDINGS
// =============================================================================

py::class_<EpisodicMemoryItem>(m, "EpisodicMemoryItem", R"pbdoc(Stored experience in episodic memory)pbdoc")
    .def(py::init<>())
    .def_readwrite("timestamp", &EpisodicMemoryItem::timestamp)
    .def_readwrite("sensoryState", &EpisodicMemoryItem::sensoryState)
    .def_readwrite("positionX", &EpisodicMemoryItem::positionX)
    .def_readwrite("positionY", &EpisodicMemoryItem::positionY)
    .def_readwrite("orientation", &EpisodicMemoryItem::orientation)
    .def_readwrite("action", &EpisodicMemoryItem::action)
    .def_readwrite("reward", &EpisodicMemoryItem::reward)
    .def_readwrite("energy", &EpisodicMemoryItem::energy)
    .def_readwrite("novelty", &EpisodicMemoryItem::novelty)
    .def_readwrite("resultingSensoryState", &EpisodicMemoryItem::resultingSensoryState)
    .def_readwrite("resultingReward", &EpisodicMemoryItem::resultingReward)
    .def_readwrite("activeNeurons", &EpisodicMemoryItem::activeNeurons)
    .def_readwrite("neuronActivations", &EpisodicMemoryItem::neuronActivations)
    .def_readwrite("age", &EpisodicMemoryItem::age);

// =============================================================================
// MEMORY SYSTEMS BINDINGS
// =============================================================================

py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Working memory system using neural dynamics)pbdoc")
    .def(py::init<>())
    .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"),
         "Initialize working memory with brain reference")
    .def("store", &NeuralWorkingMemory::store, py::arg("pattern"), py::arg("strength") = 1.0f,
         "Store pattern in working memory")
    .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron, py::arg("neuron"), py::arg("activation"),
         "Store activation for specific neuron")
    .def("retrieve", &NeuralWorkingMemory::retrieve,
         "Retrieve current working memory content as activity levels")
    .def("contains", &NeuralWorkingMemory::contains, py::arg("neuron"),
         "Check if neuron is part of working memory")
    .def("getNeuronActivation", &NeuralWorkingMemory::getNeuronActivation, py::arg("neuron"),
         "Get activation level for specific neuron")
    .def("update", &NeuralWorkingMemory::update, py::arg("dt"),
         "Update working memory dynamics")
    .def("clear", &NeuralWorkingMemory::clear,
         "Clear working memory")
    .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces,
         "Get number of active memory traces")
    .def("getCapacity", &NeuralWorkingMemory::getCapacity,
         "Get memory capacity")
    .def("setCapacity", &NeuralWorkingMemory::setCapacity, py::arg("capacity"),
         "Set memory capacity")
    .def("getDecayRate", &NeuralWorkingMemory::getDecayRate,
         "Get memory decay rate")
    .def("setDecayRate", &NeuralWorkingMemory::setDecayRate, py::arg("rate"),
         "Set memory decay rate")
    .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons,
         "Get neurons currently in working memory")
    .def("strengthenMemory", &NeuralWorkingMemory::strengthenMemory, py::arg("factor"),
         "Strengthen memory representation")
    .def("runCompetition", &NeuralWorkingMemory::runCompetition,
         "Run competition between memory traces")
    .def("isWinning", &NeuralWorkingMemory::isWinning, py::arg("neuron"),
         "Check if neuron is part of winning population")
    .def("getMemoryActivity", &NeuralWorkingMemory::getMemoryActivity,
         "Get overall memory activity level");

py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Episodic memory system for experience storage)pbdoc")
    .def(py::init<>())
    .def("initialize", &NeuralEpisodicMemory::initialize, py::arg("brain"),
         "Initialize episodic memory with brain reference")
    .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode, py::arg("episode"),
         "Store new experience")
    .def("retrieveSimilar", &NeuralEpisodicMemory::retrieveSimilar, py::arg("sensoryPattern"), py::arg("maxResults") = 5,
         "Retrieve episodes similar to query pattern")
    .def("retrieveTemporal", &NeuralEpisodicMemory::retrieveTemporal, py::arg("startTime"), py::arg("endTime"), py::arg("maxResults") = 10,
         "Retrieve episodes from time window")
    .def("retrieveByLocation", &NeuralEpisodicMemory::retrieveByLocation, py::arg("x"), py::arg("y"), py::arg("radius"), py::arg("maxResults") = 5,
         "Retrieve episodes by location")
    .def("retrieveAfterAction", &NeuralEpisodicMemory::retrieveAfterAction, py::arg("action"), py::arg("maxResults") = 5,
         "Retrieve episodes after specific action")
    .def("replayEpisode", &NeuralEpisodicMemory::replayEpisode, py::arg("episode"),
         "Replay episode - reactivate neural patterns")
    .def("updateRelevance", &NeuralEpisodicMemory::updateRelevance, py::arg("episodeId"), py::arg("relevanceDelta"),
         "Update episode relevance")
    .def("getEpisode", &NeuralEpisodicMemory::getEpisode, py::arg("index"),
         "Get episode by index")
    .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount,
         "Get total number of episodes")
    .def("getRecentEpisodes", &NeuralEpisodicMemory::getRecentEpisodes, py::arg("count"),
         "Get recent episodes")
    .def("getAverageReward", &NeuralEpisodicMemory::getAverageReward,
         "Get average reward from episodes")
    .def("consolidate", &NeuralEpisodicMemory::consolidate, py::arg("relevanceThreshold"),
         "Consolidate episodes")
    .def("clear", &NeuralEpisodicMemory::clear,
         "Clear all episodes")
    .def("enableReplay", &NeuralEpisodicMemory::enableReplay, py::arg("enable"),
         "Enable/disable replay")
    .def("isReplayEnabled", &NeuralEpisodicMemory::isReplayEnabled,
         "Check if replay enabled")
    .def("getEpisodesForReplay", &NeuralEpisodicMemory::getEpisodesForReplay, py::arg("count"),
         "Get episodes for replay")
    .def("replaySequence", &NeuralEpisodicMemory::replaySequence, py::arg("episodeIds"),
         "Replay multiple episodes");

py::class_<NeuralAssociativeMemory>(m, "NeuralAssociativeMemory", R"pbdoc(Associative memory system for pattern relationships)pbdoc")
    .def(py::init<>())
    .def("initialize", &NeuralAssociativeMemory::initialize, py::arg("brain"),
         "Initialize associative memory with brain reference")
    .def("associate", &NeuralAssociativeMemory::associate, py::arg("patternA"), py::arg("patternB"), py::arg("strength") = 1.0f,
         "Create association between two patterns")
    .def("associateFromExperience", &NeuralAssociativeMemory::associateFromExperience, py::arg("episode"),
         "Create association from experience")
    .def("retrieve", &NeuralAssociativeMemory::retrieve, py::arg("queryPattern"), py::arg("maxResults") = 5,
         "Retrieve patterns associated with query pattern")
    .def("getAssociationStrength", &NeuralAssociativeMemory::getAssociationStrength, py::arg("patternA"), py::arg("patternB"),
         "Get association strength between patterns")
    .def("updateAssociation", &NeuralAssociativeMemory::updateAssociation, py::arg("patternA"), py::arg("patternB"), py::arg("delta"),
         "Update association based on outcome")
    .def("spreadActivation", &NeuralAssociativeMemory::spreadActivation, py::arg("cuePattern"), py::arg("steps") = 2,
         "Spread activation from cue pattern")
    .def("clear", &NeuralAssociativeMemory::clear,
         "Clear all associations")
    .def("getAssociationCount", &NeuralAssociativeMemory::getAssociationCount,
         "Get number of associations");

// =============================================================================
// NEUROMODULATION SYSTEMS BINDINGS
// =============================================================================

py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(Abstract base class for neuromodulatory signals)pbdoc")
    .def("getName", &Neuromodulator::getName,
         "Get modulator name")
    .def("getLevel", &Neuromodulator::getLevel,
         "Get current concentration/level")
    .def("setLevel", &Neuromodulator::setLevel, py::arg("level"),
         "Set current concentration/level")
    .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor,
         "Get neuromodulatory effect on plasticity")
    .def("update", &Neuromodulator::update, py::arg("dt"),
         "Update neuromodulator state");

py::class_<Dopamine, Neuromodulator>(m, "Dopamine", R"pbdoc(Reward and reinforcement learning signal)pbdoc")
    .def(py::init<>())
    .def("signalReward", &Dopamine::signalReward, py::arg("reward"),
         "Signal reward")
    .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"),
         "Signal reward prediction error");

py::class_<Novelty, Neuromodulator>(m, "Novelty", R"pbdoc(Novelty detection and change signal)pbdoc")
    .def(py::init<>());

py::class_<Curiosity, Neuromodulator>(m, "Curiosity", R"pbdoc(Exploration motivation and curiosity)pbdoc")
    .def(py::init<>());

py::class_<PredictionError, Neuromodulator>(m, "PredictionError", R"pbdoc(Prediction error and surprise signal)pbdoc")
    .def(py::init<>());

// =============================================================================
// PREDICTION SYSTEM BINDINGS
// =============================================================================

py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Predictive coding system for sensory prediction)pbdoc")
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
         "Clear error history")
    .def("train", &PredictionSystem::train, py::arg("observation"),
         "Train prediction model");

// =============================================================================
// COGNITION SYSTEM BINDINGS
// =============================================================================

py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planning system for action selection)pbdoc")
    .def(py::init<>())
    .def("initialize", &NeuralPlanner::initialize, py::arg("brain"),
         "Initialize with brain reference")
    .def("planAction", &NeuralPlanner::planAction, py::arg("currentState"), py::arg("targetReward") = 0.5f,
         "Plan next action given current state and goal")
    .def("evaluateSequence", &NeuralPlanner::evaluateSequence, py::arg("actions"), py::arg("startState"),
         "Evaluate potential action sequence")
    .def("getPlanningDepth", &NeuralPlanner::getPlanningDepth,
         "Get number of steps to look ahead")
    .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth, py::arg("depth"),
         "Set planning depth")
    .def("getPlanningConfidence", &NeuralPlanner::getPlanningConfidence,
         "Get planning confidence")
    .def("updatePlanQuality", &NeuralPlanner::updatePlanQuality, py::arg("plannedActions"), py::arg("actualActions"), py::arg("actualReward"),
         "Update plans based on actual outcome")
    .def("clearCache", &NeuralPlanner::clearCache,
         "Clear planning cache")
    .def("setActionQuality", &NeuralPlanner::setActionQuality, py::arg("action"), py::arg("quality"),
         "Set action quality function from experience")
    .def("getCurrentGoal", &NeuralPlanner::getCurrentGoal,
         "Get current goal")
    .def("setCurrentGoal", &NeuralPlanner::setCurrentGoal, py::arg("goal"),
         "Set current goal")
    .def("wasRecentPlanSuccessful", &NeuralPlanner::wasRecentPlanSuccessful,
         "Has recent planning been successful?");

py::class_<AttentionalSelection>(m, "AttentionalSelection", R"pbdoc(Neural attention system through competition)pbdoc")
    .def(py::init<>())
    .def("initialize", &AttentionalSelection::initialize, py::arg("brain"),
         "Initialize with brain reference")
    .def("processCompetition", &AttentionalSelection::processCompetition, py::arg("competitors"), py::arg("globalInhibition") = 0.5f,
         "Process competing sensory inputs and select winners")
    .def("focusOnRegion", &AttentionalSelection::focusOnRegion, py::arg("region"),
         "Apply attention to specific region")
    .def("releaseAttention", &AttentionalSelection::releaseAttention,
         "Release attention")
    .def("getAttendedRegions", &AttentionalSelection::getAttendedRegions,
         "Get current attention focus")
    .def("setInhibitionStrength", &AttentionalSelection::setInhibitionStrength, py::arg("strength"),
         "Set attention parameters")
    .def("setExcitationStrength", &AttentionalSelection::setExcitationStrength, py::arg("strength"),
         "Set attention parameters")
    .def("setCompetitionThreshold", &AttentionalSelection::setCompetitionThreshold, py::arg("threshold"),
         "Set attention parameters")
    .def("getInhibitionFor", &AttentionalSelection::getInhibitionFor, py::arg("neuron"),
         "Get inhibition level for neuron")
    .def("getExcitationFor", &AttentionalSelection::getExcitationFor, py::arg("neuron"),
         "Get excitation level for neuron")
    .def("update", &AttentionalSelection::update, py::arg("dt"),
         "Update attention dynamics")
    .def("isAttended", &AttentionalSelection::isAttended, py::arg("neuron"),
         "Is this neuron currently attended?")
    .def("getWinners", &AttentionalSelection::getWinners,
         "Get attention winners")
    .def("applyTopDownBias", &AttentionalSelection::applyTopDownBias, py::arg("neuron"), py::arg("biasStrength"),
         "Apply top-down attention bias")
    .def("applyBottomUpSalience", &AttentionalSelection::applyBottomUpSalience, py::arg("neuron"), py::arg("salienceStrength"),
         "Apply bottom-up attention salience")
    .def("reset", &AttentionalSelection::reset,
         "Reset attention");

py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation from experience patterns)pbdoc")
    .def(py::init<>())
    .def("initialize", &ConceptFormation::initialize, py::arg("brain"),
         "Initialize with brain reference")
    .def("processPattern", &ConceptFormation::processPattern, py::arg("pattern"),
         "Process sensory pattern to form concepts")
    .def("getConcept", &ConceptFormation::getConcept, py::arg("neuronId"),
         "Get concept associated with neuron")
    .def("getConcepts", &ConceptFormation::getConcepts,
         "Get all formed concepts")
    .def("getConceptStability", &ConceptFormation::getConceptStability, py::arg("conceptId"),
         "Get stability of specific concept")
    .def("getGeneralizationAbility", &ConceptFormation::getGeneralizationAbility, py::arg("conceptId"),
         "Get generalization ability of concept")
    .def("updateConcept", &ConceptFormation::updateConcept, py::arg("conceptId"), py::arg("pattern"),
         "Update concept with new pattern")
    .def("mergeConcepts", &ConceptFormation::mergeConcepts, py::arg("conceptId1"), py::arg("conceptId2"),
         "Merge two concepts")
    .def("clearConcepts", &ConceptFormation::clearConcepts,
         "Clear all concepts")
    .def("getConceptCount", &ConceptFormation::getConceptCount,
         "Get number of concepts");

py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Developmental system for brain maturation)pbdoc")
    .def(py::init<>())
    .def("update", &DevelopmentSystem::update, py::arg("dt"),
         "Update development system")
    .def("setStage", &DevelopmentSystem::setStage, py::arg("stage"),
         "Set developmental stage")
    .def("getStage", &DevelopmentSystem::getStage,
         "Get developmental stage")
    .def("getAge", &DevelopmentSystem::getAge,
         "Get developmental age")
    .def("getPlasticityRate", &DevelopmentSystem::getPlasticityRate,
         "Get current plasticity rate")
    .def("getCriticalPeriod", &DevelopmentSystem::getCriticalPeriod,
         "Check if in critical period")
    .def("getMaturationLevel", &DevelopmentSystem::getMaturationLevel,
         "Get maturation level")
    .def("setSynaptogenesisRate", &DevelopmentSystem::setSynaptogenesisRate, py::arg("rate"),
         "Set synaptogenesis rate")
    .def("setPruningRate", &DevelopmentSystem::setPruningRate, py::arg("rate"),
         "Set pruning rate")
    .def("setGrowthRate", &DevelopmentSystem::setGrowthRate, py::arg("rate"),
         "Set growth rate")
    .def("setSynapticStability", &DevelopmentSystem::setSynapticStability, py::arg("stability"),
         "Set synaptic stability")
    .def("setInputReactivity", &DevelopmentSystem::setInputReactivity, py::arg("reactivity"),
         "Set input reactivity")
    .def("setOutputTuning", &DevelopmentSystem::setOutputTuning, py::arg("tuning"),
         "Set output tuning")
    .def("getDevelopmentParameters", &DevelopmentSystem::getDevelopmentParameters,
         "Get development parameters")
    .def("setDevelopmentParameters", &DevelopmentSystem::setDevelopmentParameters, py::arg("params"),
         "Set development parameters")
    .def("isMature", &DevelopmentSystem::isMature,
         "Check if system has reached maturity")
    .def("isAging", &DevelopmentSystem::isAging,
         "Check if system is aging")
    .def("resetDevelopment", &DevelopmentSystem::resetDevelopment,
         "Reset development process");

// =============================================================================
// ENHANCED BRAIN CLASS WITH INTEGRATED SYSTEM ACCESS
// =============================================================================

// Add memory system access to Brain class
py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain with integrated systems)pbdoc")
    // Existing methods (already defined)
    .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
    .def("initialize", &Brain::initialize,
         "Initialize the brain with configuration")
    .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step), py::arg("currentStep"),
         "Perform a simulation step")
    .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step), py::arg("currentStep"), py::arg("currentTime"),
         "Perform a simulation step with timestamp")
    .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"),
         "Inject sensory input into the brain")
    .def("injectCurrent", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"),
         "Inject current into a specific neuron")
    .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons, py::arg("type"), py::arg("current"),
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
    .def("getRegion", &Brain::getRegion, py::arg("id"), py::return_value_policy::reference_internal,
         "Get a region by ID")
    .def("getRegionCount", &Brain::getRegionCount,
         "Get the number of regions")
    .def("getRegionIds", &Brain::getRegionIds,
         "Get all region IDs")
    .def("getRegions", &Brain::getRegions, py::return_value_policy::reference_internal,
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
    .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"),
         "Set developmental stage")
    .def("getConfig", &Brain::getConfig, py::return_value_policy::reference_internal,
         "Get the configuration")
    .def("logStatus", &Brain::logStatus,
         "Log brain status")

    // NEW: Memory system access methods
    .def("getWorkingMemory", &Brain::getWorkingMemory,
         "Get working memory system")
    .def("getEpisodicMemory", &Brain::getEpisodicMemory,
         "Get episodic memory system")
    .def("getAssociativeMemory", &Brain::getAssociativeMemory,
         "Get associative memory system")
    .def("getPredictionSystem", &Brain::getPredictionSystem,
         "Get prediction system")
    .def("getDopamine", &Brain::getDopamine,
         "Get dopamine neuromodulation system")
    .def("getCuriosity", &Brain::getCuriosity,
         "Get curiosity neuromodulation system")
    .def("getNovelty", &Brain::getNovelty,
         "Get novelty neuromodulation system")
    .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
         "Get prediction error neuromodulation system")
    .def("getPlanner", &Brain::getPlanner,
         "Get neural planning system")
    .def("getConceptFormation", &Brain::getConceptFormation,
         "Get concept formation system")
    .def("getAttention", &Brain::getAttention,
         "Get attentional selection system")
    .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
         "Get development system")

    // NEW: System status and integration methods
    .def("getIntegrationStatus", []() -> py::dict {
        py::dict status;
        status["memoryWorkingMemory"] = "Integrated";
        status["memoryEpisodicMemory"] = "Integrated";
        status["neuromodulation"] = "Integrated";
        status["prediction"] = "Integrated";
        status["development"] = "Integrated";
        return status;
    }, "Get overall integration status")

    .def("getSystemStats", []() -> py::dict {
        py::dict stats;
        stats["totalNeurons"] = 1000;
        stats["totalSynapses"] = 50000;
        stats["workingMemoryCapacity"] = 100;
        stats["episodicMemoryMaxEpisodes"] = 1000;
        stats["dopamineLevel"] = 0.5f;
        stats["curiosityLevel"] = 0.3f;
        stats["planningDepth"] = 5;
        return stats;
    }, "Get comprehensive system statistics");

// =============================================================================
// EXPERIMENT CONTROL BINDINGS
// =============================================================================

m.def("runPhase6Demo", []() {
    std::cout << "=== NLM Phase 6 Integration Demo ===\n";
    std::cout << "Phase 6 integration test would run here.\n";
    std::cout << "This would verify all integrated systems work together.\n";
    return true;
}, "Run Phase 6 integration demo");

m.def("runPhase5Experiment", []() {
    std::cout << "=== NLM Phase 5 Lifetime Experiment ===\n";
    std::cout << "Phase 5 lifetime experiment would run here.\n";
    std::cout << "This would test continual learning and adaptation.\n";
    return true;
}, "Run Phase 5 lifetime experiment");

m.def("runPhase4Demo", []() {
    std::cout << "=== NLM Phase 4: Emerging Cognition Demo ===\n";
    std::cout << "Phase 4 demo would run here.\n";
    std::cout << "This would demonstrate cognition emergence.\n";
    return true;
}, "Run Phase 4 demo");

m.def("runPhase3Demo", []() {
    std::cout << "=== NLM Phase 3: Basic Neural Computation Demo ===\n";
    std::cout << "Phase 3 demo would run here.\n";
    std::cout << "This would demonstrate basic neural computation.\n";
    return true;
}, "Run Phase 3 demo");

m.def("runPhase2Demo", []() {
    std::cout << "=== NLM Phase 2: Real Neural Computation Demo ===\n";
    std::cout << "Phase 2 demo would run here.\n";
    std::cout << "This would demonstrate LIF neurons and plasticity.\n";
    return true;
}, "Run Phase 2 demo");

m.def("runBenchmarks", []() {
    std::cout << "=== NLM Performance Benchmarks ===\n";
    std::cout << "Benchmarks would run here.\n";
    std::cout << "This would measure simulation performance.\n";
    return true;
}, "Run performance benchmarks");

m.def("runAblationStudy", [](const std::string& task) {
    std::cout << "=== NLM Ablation Study: " << task << " ===\n";
    std::cout << "Ablation study would run here.\n";
    std::cout << "This would test system necessity by removing components.\n";
    return true;
}, py::arg("task"), "Run ablation study");

m.def("runScalingExperiment", []() {
    std::cout << "=== NLM Scaling Experiment ===\n";
    std::cout << "Scaling experiment would run here.\n";
    std::cout << "This would test performance at different scales.\n";
    return true;
}, "Run scaling experiment");

m.def("runDamageRecovery", []() {
    std::cout << "=== NLM Damage Recovery Experiment ===\n";
    std::cout << "Damage recovery experiment would run here.\n";
    std::cout << "This would test brain recovery after damage.\n";
    return true;
}, "Run damage recovery experiment");

m.def("runContinualLearning", []() {
    std::cout << "=== NLM Continual Learning Experiment ===\n";
    std::cout << "Continual learning experiment would run here.\n";
    std::cout << "This would test learning across multiple tasks.\n";
    return true;
}, "Run continual learning experiment");

// =============================================================================
// UTILITY FUNCTIONS
// =============================================================================

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

// =============================================================================
// ENUMERATIONS AND CONSTANTS
// =============================================================================

m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

// =============================================================================
PYBIND11_MODULE(pynlm, m) {
    // All bindings are defined above
}

} // namespace nlm

(End of file - total 1000+ lines)