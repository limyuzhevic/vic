#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <memory>
#include <string>
#include <vector>

#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/SelfModel.hpp"
#include "../src/cognition/SocialLearning.hpp"
#include "../src/cognition/SpatialRepresentation.hpp"
#include "../src/cognition/TemporalRelation.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"

namespace py = pybind11;

void bind_neural_planner(py::module& m) {
    py::class_<nlm::NeuralPlanner>(m, "NeuralPlanner")
        .def(py::init<>())
        .def("initialize", &nlm::NeuralPlanner::initialize, py::arg("brain"),
             "Initialize NeuralPlanner with brain reference")
        .def("planAction", &nlm::NeuralPlanner::planAction,
             py::arg("currentState"), py::arg("targetReward") = 0.5f,
             "Plan next action given current state and target reward")
        .def("evaluateSequence", &nlm::NeuralPlanner::evaluateSequence,
             py::arg("actions"), py::arg("startState"),
             "Evaluate a potential action sequence")
        .def("getPlanningDepth", &nlm::NeuralPlanner::getPlanningDepth,
             "Get number of steps to look ahead")
        .def("setPlanningDepth", &nlm::NeuralPlanner::setPlanningDepth,
             py::arg("depth"), "Set number of steps to look ahead")
        .def("getPlanningConfidence", &nlm::NeuralPlanner::getPlanningConfidence,
             "Get planning confidence")
        .def("updatePlanQuality", &nlm::NeuralPlanner::updatePlanQuality,
             py::arg("plannedActions"), py::arg("actualActions"),
             py::arg("actualReward"),
             "Update plans based on actual outcome")
        .def("clearCache", &nlm::NeuralPlanner::clearCache,
             "Clear planning cache")
        .def("setActionQuality", &nlm::NeuralPlanner::setActionQuality,
             py::arg("action"), py::arg("quality"),
             "Set action quality function (from experience)")
        .def("setCurrentGoal", &nlm::NeuralPlanner::setCurrentGoal,
             py::arg("goal"), "Set current goal")
        .def("getCurrentGoal", &nlm::NeuralPlanner::getCurrentGoal,
             "Get current goal")
        .def("wasRecentPlanSuccessful", &nlm::NeuralPlanner::wasRecentPlanSuccessful,
             "Has recent planning been successful?")
        .def("__repr__", [](const nlm::NeuralPlanner& planner) {
            return "<NeuralPlanner: depth=" + std::to_string(planner.getPlanningDepth()) +
                   ", confidence=" + std::to_string(planner.getPlanningConfidence()) + ">";
        });
}

void bind_concept_formation(py::module& m) {
    py::class_<nlm::ConceptFormation>(m, "ConceptFormation")
        .def(py::init<>())
        .def("initialize", &nlm::ConceptFormation::initialize, py::arg("brain"),
             "Initialize ConceptFormation with brain reference")
        .def("presentExperience", &nlm::ConceptFormation::presentExperience,
             py::arg("pattern"), py::arg("features"), py::arg("reward"),
             py::arg("currentTime"),
             "Present a new experience to the concept formation system")
        .def("getMatchingConcept", &nlm::ConceptFormation::getMatchingConcept,
             py::arg("pattern"), py::arg("similarityThreshold") = 0.7f,
             "Get the concept that best matches a pattern")
        .def("getConceptCount", &nlm::ConceptFormation::getConceptCount,
             "Get number of concepts discovered")
        .def("getConcept", &nlm::ConceptFormation::getConcept,
             py::arg("conceptId"), "Get a specific concept")
        .def("getConceptPrototype", &nlm::ConceptFormation::getConceptPrototype,
             py::arg("conceptId"), "Get concept prototype")
        .def("getConceptInstances", &nlm::ConceptFormation::getConceptInstances,
             py::arg("conceptId"), "Get concept instances")
        .def("getConceptStability", &nlm::ConceptFormation::getConceptStability,
             py::arg("conceptId"), "Measure concept stability (0-1)")
        .def("getGeneralizationAbility", &nlm::ConceptFormation::getGeneralizationAbility,
             py::arg("conceptId"), "Get concept generalization ability")
        .def("isNovel", &nlm::ConceptFormation::isNovel,
             py::arg("pattern"), py::arg("similarityThreshold") = 0.7f,
             "Check if pattern is novel")
        .def("mergeConcepts", &nlm::ConceptFormation::mergeConcepts,
             py::arg("conceptA"), py::arg("conceptB"),
             "Merge two concepts (when discovered they are actually the same)")
        .def("clear", &nlm::ConceptFormation::clear,
             "Clear all concepts")
        .def("setFormationThreshold", &nlm::ConceptFormation::setFormationThreshold,
             py::arg("t"), "Set formation threshold")
        .def("setStabilityWindow", &nlm::ConceptFormation::setStabilityWindow,
             py::arg("w"), "Set stability window");
}

void bind_attentional_selection(py::module& m) {
    py::class_<nlm::AttentionalSelection>(m, "AttentionalSelection")
        .def(py::init<>())
        .def("initialize", &nlm::AttentionalSelection::initialize,
             py::arg("brain"), "Initialize AttentionalSelection with brain reference")
        .def("processCompetition", &nlm::AttentionalSelection::processCompetition,
             py::arg("competitors"), py::arg("globalInhibition") = 0.5f,
             "Process competition among neural populations")
        .def("focusOnRegion", &nlm::AttentionalSelection::focusOnRegion,
             py::arg("region"), "Apply attention to a specific region")
        .def("releaseAttention", &nlm::AttentionalSelection::releaseAttention,
             "Release attention (allow all regions equal processing)")
        .def("getAttendedRegions", &nlm::AttentionalSelection::getAttendedRegions,
             "Get current attention focus")
        .def("setInhibitionStrength", &nlm::AttentionalSelection::setInhibitionStrength,
             py::arg("strength"), "Set inhibition strength")
        .def("setExcitationStrength", &nlm::AttentionalSelection::setExcitationStrength,
             py::arg("strength"), "Set excitation strength")
        .def("setCompetitionThreshold", &nlm::AttentionalSelection::setCompetitionThreshold,
             py::arg("threshold"), "Set competition threshold")
        .def("getInhibitionFor", &nlm::AttentionalSelection::getInhibitionFor,
             py::arg("neuron"), "Get inhibition level for a neuron")
        .def("getExcitationFor", &nlm::AttentionalSelection::getExcitationFor,
             py::arg("neuron"), "Get excitation level for a neuron")
        .def("applyTopDownBias", &nlm::AttentionalSelection::applyTopDownBias,
             py::arg("neuron"), py::arg("biasStrength"),
             "Apply top-down attention bias")
        .def("applyBottomUpSalience", &nlm::AttentionalSelection::applyBottomUpSalience,
             py::arg("neuron"), py::arg("salienceStrength"),
             "Apply bottom-up attention salience")
        .def("reset", &nlm::AttentionalSelection::reset,
             "Reset attentional system to initial state");
}

void bind_neural_working_memory(py::module& m) {
    py::class_<nlm::NeuralWorkingMemory>(m, "NeuralWorkingMemory")
        .def(py::init<>())
        .def("initialize", &nlm::NeuralWorkingMemory::initialize,
             py::arg("brain"), "Initialize working memory system")
        .def("store", &nlm::NeuralWorkingMemory::store,
             py::arg("pattern"), py::arg("strength") = 1.0f,
             "Store information pattern in working memory")
        .def("storeToNeuron", &nlm::NeuralWorkingMemory::storeToNeuron,
             py::arg("neuron"), py::arg("activation"),
             "Store activation to specific neuron")
        .def("retrieve", &nlm::NeuralWorkingMemory::retrieve,
             "Retrieve current working memory content")
        .def("contains", &nlm::NeuralWorkingMemory::contains,
             py::arg("neuron"), "Check if specific neuron is in working memory")
        .def("getNeuronActivation", &nlm::NeuralWorkingMemory::getNeuronActivation,
             py::arg("neuron"), "Get activation level of specific neuron")
        .def("update", &nlm::NeuralWorkingMemory::update,
             py::arg("dt"), "Update working memory state")
        .def("clear", &nlm::NeuralWorkingMemory::clear,
             "Clear working memory")
        .def("strengthenMemory", &nlm::NeuralWorkingMemory::strengthenMemory,
             py::arg("factor"), "Strengthen working memory representation")
        .def("runCompetition", &nlm::NeuralWorkingMemory::runCompetition,
             "Run competition between memory traces")
        .def("isWinning", &nlm::NeuralWorkingMemory::isWinning,
             py::arg("neuron"), "Check if neuron is part of winning population")
        .def("getMemoryActivity", &nlm::NeuralWorkingMemory::getMemoryActivity,
             "Get overall activity level of working memory")
        .def("getCapacity", &nlm::NeuralWorkingMemory::getCapacity,
             "Get working memory capacity")
        .def("setCapacity", &nlm::NeuralWorkingMemory::setCapacity,
             py::arg("cap"), "Set working memory capacity")
        .def("getDecayRate", &nlm::NeuralWorkingMemory::getDecayRate,
             "Get current decay rate")
        .def("setDecayRate", &nlm::NeuralWorkingMemory::setDecayRate,
             py::arg("rate"), "Set decay rate");
}

void bind_neuromodulators(py::module& m) {
    // Reward neuromodulator (reward prediction error)
    py::class_<nlm::Reward>(m, "Reward")
        .def(py::init<>())
        .def("processReward", &nlm::Reward::processReward,
             py::arg("currentState"), py::arg("nextState"),
             py::arg("action"), py::arg("reward"),
             "Process reward signal")
        .def("getCurrentReward", &nlm::Reward::getCurrentReward,
             "Get current reward value")
        .def("getPredictionError", &nlm::Reward::getPredictionError,
             "Get reward prediction error")
        .def("reset", &nlm::Reward::reset,
             "Reset reward system");

    // Curiosity neuromodulator (exploration motivation)
    py::class_<nlm::Curiosity>(m, "Curiosity")
        .def(py::init<>())
        .def("processSensoryInput", &nlm::Curiosity::processSensoryInput,
             py::arg("input"), "Process sensory input for curiosity")
        .def("getCuriosityLevel", &nlm::Curiosity::getCuriosityLevel,
             "Get curiosity level")
        .def("update", &nlm::Curiosity::update,
             py::arg("dt"), "Update curiosity dynamics")
        .def("reset", &nlm::Curiosity::reset,
             "Reset curiosity system");

    // PredictionError neuromodulator (prediction error)
    py::class_<nlm::PredictionError>(m, "PredictionError")
        .def(py::init<>())
        .def("processPrediction", &nlm::PredictionError::processPrediction,
             py::arg("predicted"), py::arg("actual"),
             "Process prediction error")
        .def("getPredictionError", &nlm::PredictionError::getPredictionError,
             "Get prediction error")
        .def("getConfidence", &nlm::PredictionError::getConfidence,
             "Get prediction confidence")
        .def("reset", &nlm::PredictionError::reset,
             "Reset prediction error system");

    // Novelty neuromodulator (novelty detection)
    py::class_<nlm::Novelty>(m, "Novelty")
        .def(py::init<>())
        .def("processInput", &nlm::Novelty::processInput,
             py::arg("input"), "Process input for novelty detection")
        .def("getNoveltyLevel", &nlm::Novelty::getNoveltyLevel,
             "Get novelty level")
        .def("getConfidence", &nlm::Novelty::getConfidence,
             "Get novelty confidence")
        .def("reset", &nlm::Novelty::reset,
             "Reset novelty system");
}

void bind_prediction_system(py::module& m) {
    py::class_<nlm::PredictionSystem>(m, "PredictionSystem")
        .def(py::init<>())
        .def("predictNextState", &nlm::PredictionSystem::predictNextState,
             py::arg("currentState"),
             "Generate prediction for next state")
        .def("updatePredictions", &nlm::PredictionSystem::updatePredictions,
             py::arg("predicted"), py::arg("actual"),
             "Update prediction model based on actual observation")
        .def("getPredictionError", &nlm::PredictionSystem::getPredictionError,
             "Get current prediction error")
        .def("getConfidence", &nlm::PredictionSystem::getConfidence,
             "Get prediction confidence")
        .def("getErrorHistory", &nlm::PredictionSystem::getErrorHistory,
             "Get history of prediction errors")
        .def("clearHistory", &nlm::PredictionSystem::clearHistory,
             "Clear prediction history")
        .def("train", &nlm::PredictionSystem::train,
             py::arg("observation"), "Train prediction model")
        .def("reset", &nlm::PredictionSystem::reset,
             "Reset prediction system");
}

void bind_self_model(py::module& m) {
    py::class_<nlm::SelfModel>(m, "SelfModel")
        .def(py::init<>())
        .def("initialize", &nlm::SelfModel::initialize, py::arg("brain"),
             "Initialize self model with brain reference")
        .def("recordSelfAction", &nlm::SelfModel::recordSelfAction,
             py::arg("action"), py::arg("beforeState"),
             py::arg("afterState"),
             "Record that taking an action caused a specific sensory change")
        .def("predictActionConsequence", &nlm::SelfModel::predictActionConsequence,
             py::arg("action"), py::arg("currentState"),
             "Predict sensory consequence of an action")
        .def("getSelfModelConfidence", &nlm::SelfModel::getSelfModelConfidence,
             py::arg("action"), "Get confidence in self-model")
        .def("getPreferredAction", &nlm::SelfModel::getPreferredAction,
             py::arg("state"), "Get the body schema (preferred actions)")
        .def("updateSelfModel", &nlm::SelfModel::updateSelfModel,
             py::arg("predicted"), py::arg("actual"), py::arg("action"),
             "Update self-model based on prediction error")
        .def("getCapabilityLevel", &nlm::SelfModel::getCapabilityLevel,
             "Get current capability level (0-1)")
        .def("getBodyAwareness", &nlm::SelfModel::getBodyAwareness,
             "Get body awareness (how accurately can predict consequences)")
        .def("clear", &nlm::SelfModel::clear,
             "Clear self-model");
}

void bind_social_learning(py::module& m) {
    py::class_<nlm::SocialLearning>(m, "SocialLearning")
        .def(py::init<>())
        .def("initialize", &nlm::SocialLearning::initialize,
             py::arg("brain"), "Initialize with brain reference")
        .def("observeAgentAction", &nlm::SocialLearning::observeAgentAction,
             py::arg("observedAction"), py::arg("observerState"),
             py::arg("resultingState"),
             "Record observation of another agent's action and effect")
        .def("canImitate", &nlm::SocialLearning::canImitate,
             py::arg("observedAction"), "Can imitate this observed action?")
        .def("getImitationAction", &nlm::SocialLearning::getImitationAction,
             py::arg("currentState"), "Get best action to imitate")
        .def("learnCommunicationSignal", &nlm::SocialLearning::learnCommunicationSignal,
             py::arg("signalPattern"), py::arg("signalReward"),
             "Learn simple communication signal from another agent")
        .def("detectSignal", &nlm::SocialLearning::detectSignal,
             py::arg("neuralPattern"), "Detect if another agent is signaling")
        .def("clear", &nlm::SocialLearning::clear,
             "Clear social learning");
}

void init_phase4_bindings(py::module& m) {
    // Neural cognition systems
    bind_neural_planner(m);
    bind_concept_formation(m);
    bind_attentional_selection(m);
    bind_self_model(m);
    bind_social_learning(m);

    // Memory systems
    bind_neural_working_memory(m);

    // Neuromodulation systems
    bind_neuromodulators(m);

    // Prediction system
    bind_prediction_system(m);

    // Utility classes for cognitive systems
    py::class_<nlm::EpisodicMemoryItem>(m, "EpisodicMemoryItem")
        .def_readonly("timestamp", &nlm::EpisodicMemoryItem::timestamp)
        .def_readonly("sensoryState", &nlm::EpisodicMemoryItem::sensoryState)
        .def_readonly("positionX", &nlm::EpisodicMemoryItem::positionX)
        .def_readonly("positionY", &nlm::EpisodicMemoryItem::positionY)
        .def_readonly("orientation", &nlm::EpisodicMemoryItem::orientation)
        .def_readonly("action", &nlm::EpisodicMemoryItem::action)
        .def_readonly("reward", &nlm::EpisodicMemoryItem::reward)
        .def_readonly("energy", &nlm::EpisodicMemoryItem::energy)
        .def_readonly("novelty", &nlm::EpisodicMemoryItem::novelty);

    // Simple visualization utility
    py::class_<nlm::PlanningCandidate>(m, "PlanningCandidate")
        .def_readonly("actions", &nlm::PlanningCandidate::actions)
        .def_readonly("expectedReward", &nlm::PlanningCandidate::expectedReward)
        .def_readonly("confidence", &nlm::PlanningCandidate::confidence)
        .def_readonly("depth", &nlm::PlanningCandidate::depth);
}
