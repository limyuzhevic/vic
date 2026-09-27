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
        .def("initialize", &nlm::ConceptFormation::initialize,
             py::arg("brain"), "Initialize with brain reference")
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
             py::arg("w"), "Set stability window")
        .def("findConceptForPattern", &nlm::ConceptFormation::findConceptForPattern,
             py::arg("pattern"), "Get concept for a given pattern");
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

void bind_spatial_representation(py::module& m) {
    py::class_<nlm::SpatialRepresentation>(m, "SpatialRepresentation")
        .def(py::init<>())
        .def("initialize", &nlm::SpatialRepresentation::initialize,
             py::arg("brain"), "Initialize with brain reference")
        .def("recordPosition", &nlm::SpatialRepresentation::recordPosition,
             py::arg("x"), py::arg("y"), py::arg("sensoryCues"),
             "Record current position experience")
        .def("getPredictedPosition", &nlm::SpatialRepresentation::getPredictedPosition,
             "Get predicted position based on path integration")
        .def("integrateMovement", &nlm::SpatialRepresentation::integrateMovement,
             py::arg("dx"), py::arg("dy"), "Update position estimate")
        .def("getPositionActivation", &nlm::SpatialRepresentation::getPositionActivation,
             py::arg("x"), py::arg("y"), "Get position neuron activation")
        .def("getPlaceNeuronsNear", &nlm::SpatialRepresentation::getPlaceNeuronsNear,
             py::arg("x"), py::arg("y"), py::arg("radius"),
             "Find place neurons near a position")
        .def("clear", &nlm::SpatialRepresentation::clear,
             "Clear spatial representations");
}

void bind_temporal_relation(py::module& m) {
    py::class_<nlm::TemporalRelation>(m, "TemporalRelation")
        .def(py::init<>())
        .def("initialize", &nlm::TemporalRelation::initialize,
             py::arg("brain"), "Initialize with brain reference")
        .def("recordSequence", &nlm::TemporalRelation::recordSequence,
             py::arg("eventA"), py::arg("eventB"), py::arg("reward"),
             "Record that event A was followed by event B")
        .def("predictNext", &nlm::TemporalRelation::predictNext,
             py::arg("eventA"), "Given event A, predict what comes next")
        .def("predictPrevious", &nlm::TemporalRelation::predictPrevious,
             py::arg("eventB"), "Given event B, predict what came before")
        .def("getPredictionConfidence", &nlm::TemporalRelation::getPredictionConfidence,
             py::arg("event"), "Get confidence in prediction")
        .def("clear", &nlm::TemporalRelation::clear,
             "Clear temporal relations");
}

void init_cognition_bindings(py::module& m) {
    bind_neural_planner(m);
    bind_concept_formation(m);
    bind_self_model(m);
    bind_social_learning(m);
    bind_spatial_representation(m);
    bind_temporal_relation(m);
}
