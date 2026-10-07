// Cognitive system bindings

#include "bindings.h"

namespace nlm {

void bind_cognition_module(py::module_& m) {
    // NeuralPlanner class
    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(
        Neural planner for action sequence evaluation and selection.
        
        The neural planner uses:
        - Predictive neural dynamics for action evaluation
        - Multi-step planning through neural simulation
        - Reward-based action selection
        - Adaptive planning based on confidence
        
        Key features:
        - Plans multiple steps ahead
        - Evaluates action sequences
        - Selects best immediate actions
        - Adapts plans based on prediction confidence
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPlanner::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("planAction", &NeuralPlanner::planAction, py::arg("currentState"), py::arg("targetReward") = 0.5f,
             "Plan next action given current state and goal")
        .def("evaluateSequence", &NeuralPlanner::evaluateSequence, py::arg("actions"), py::arg("startState"),
             "Evaluate a potential action sequence")
        .def("getPlanningDepth", &NeuralPlanner::getPlanningDepth, "Get number of steps to look ahead")
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth, py::arg("depth"), "Set planning depth")
        .def("getPlanningConfidence", &NeuralPlanner::getPlanningConfidence, "Get planning confidence")
        .def("updatePlanQuality", &NeuralPlanner::updatePlanQuality,
             py::arg("plannedActions"), py::arg("actualActions"), py::arg("actualReward"),
             "Update plans based on actual outcome")
        .def("clearCache", &NeuralPlanner::clearCache, "Clear planning cache")
        .def("setActionQuality", &NeuralPlanner::setActionQuality, py::arg("action"), py::arg("quality"),
             "Set action quality function (from experience)")
        .def("getCurrentGoal", &NeuralPlanner::getCurrentGoal, "Get current goal")
        .def("setCurrentGoal", &NeuralPlanner::setCurrentGoal, py::arg("goal"), "Set current goal")
        .def("wasRecentPlanSuccessful", &NeuralPlanner::wasRecentPlanSuccessful,
             "Has recent planning been successful?")
        .def("generateActionSequences", &NeuralPlanner::generateActionSequences, py::arg("depth"),
             "Generate possible action sequences (advanced usage)")
        .def("evaluateAction", &NeuralPlanner::evaluateAction, py::arg("action"), py::arg("state"),
             "Evaluate single action from state (advanced usage)");

    // SelfModel class
    py::class_<SelfModel>(m, "SelfModel", R"pbdoc(
        Self-model representing agent's internal model of itself.
        
        The self-model provides:
        - Body schema learning (how actions affect sensory state)
        - Forward models for predicting consequences
        - Self-other differentiation
        - Capability assessment
        
        Key features:
        - Learns action-effect associations
        - Predicts sensory consequences
        - Distinguishes self-generated from external changes
        - Represents own capabilities and limitations
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &SelfModel::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("recordSelfAction", &SelfModel::recordSelfAction,
             py::arg("action"), py::arg("beforeState"), py::arg("afterState"),
             "Record that taking an action caused a specific sensory change")
        .def("predictActionConsequence", &SelfModel::predictActionConsequence,
             py::arg("action"), py::arg("currentState"), "Predict sensory consequence of an action")
        .def("getSelfModelConfidence", &SelfModel::getSelfModelConfidence, py::arg("action"),
             "Get confidence in self-model for a given action")
        .def("computeSelfGeneratedLikeness", &SelfModel::computeSelfGeneratedLikeness,
             py::arg("beforeState"), py::arg("afterState"), py::arg("action"),
             "Is this change likely caused by self (action) vs external?")
        .def("getPreferredAction", &SelfModel::getPreferredAction, py::arg("state"),
             "Get the body schema (preferred actions in different states)")
        .def("updateSelfModel", &SelfModel::updateSelfModel,
             py::arg("predicted"), py::arg("actual"), py::arg("action"),
             "Update self-model based on prediction error")
        .def("getCapabilityLevel", &SelfModel::getCapabilityLevel, "Get current capability level (0-1)")
        .def("getBodyAwareness", &SelfModel::getBodyAwareness, "Get body awareness (how accurately can predict consequences)")
        .def("clear", &SelfModel::clear, "Clear self-model")
        .def("hasSelfModel", &SelfModel::hasSelfModel, "Has self-model formed?")
        .def("findMatchingEffect", &SelfModel::findMatchingEffect, py::arg("action"), py::arg("beforeState"),
             "Find best matching previous experience (advanced usage)");

    // SocialLearning class
    py::class_<SocialLearning>(m, "SocialLearning", R"pbdoc(
        Social learning system for learning from observing other agents.
        
        Social learning provides:
        - Observation of other agents' actions
        - Learning that other agents cause predictable changes
        - Imitation of observed actions
        - Simple communication signals
        
        Key features:
        - Record observations of other agents
        - Can imitate observed actions
        - Learn communication signals
        - Detect signals from other agents
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &SocialLearning::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("observeAgentAction", &SocialLearning::observeAgentAction,
             py::arg("observedAction"), py::arg("observerState"), py::arg("resultingState"),
             "Record observation of another agent's action and effect")
        .def("canImitate", &SocialLearning::canImitate, py::arg("observedAction"),
             "Can I imitate this observed action?")
        .def("getImitationAction", &SocialLearning::getImitationAction, py::arg("currentState"),
             "Get the best action to imitate given current state")
        .def("learnCommunicationSignal", &SocialLearning::learnCommunicationSignal,
             py::arg("signalPattern"), py::arg("signalReward"),
             "Learn simple communication signal from another agent")
        .def("detectSignal", &SocialLearning::detectSignal, py::arg("neuralPattern"),
             "Detect if another agent is signaling")
        .def("getSignalPattern", &SocialLearning::getSignalPattern, "Get learned signal pattern")
        .def("getSignalMeaning", &SocialLearning::getSignalMeaning, "Get signal meaning (associated reward)")
        .def("updateSocialKnowledge", &SocialLearning::updateSocialKnowledge, py::arg("interactionReward"),
             "Update social knowledge based on interactions")
        .def("clear", &SocialLearning::clear, "Clear social learning")
        .def("hasSocialKnowledge", &SocialLearning::hasSocialKnowledge, "Has learned from others?")
        .def("getObservationCount", &SocialLearning::getObservationCount, "Get observation count");

    // ConceptFormation class
    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(
        Concept formation system for discovering abstract patterns.
        
        Concept formation provides:
        - Pattern discovery from experience
        - Abstract concept representation
        - Categorization and abstraction
        - Integration with episodic memory
        
        Key features:
        - Extract common patterns from experiences
        - Form abstract concepts
        - Categorize similar experiences
        - Build hierarchical knowledge
    )pbdoc")
        .def(py::init<>())
        .def("discoverConcept", &ConceptFormation::discoverConcept, py::arg("pattern"), py::arg("context"),
             "Discover a new concept from pattern")
        .def("getConcept", &ConceptFormation::getConcept, py::arg("conceptId"),
             py::return_value_policy::reference_internal, "Get concept by ID")
        .def("getConcepts", &ConceptFormation::getConcepts, "Get all concepts")
        .def("getConceptsByCategory", &ConceptFormation::getConceptsByCategory, py::arg("category"),
             "Get concepts in a category")
        .def("clear", &ConceptFormation::clear, "Clear all concepts");

    // AttentionalSelection class
    py::class_<AttentionalSelection>(m, "AttentionalSelection", R"pbdoc(
        Attentional selection through neural competition.
        
        Attentional selection provides:
        - Competitive selection between neural populations
        - Salience-based focus
        - Recurrent amplification of winners
        - Inhibitory surround suppression
        
        Key features:
        - Process competing sensory inputs
        - Select winning neural populations
        - Apply attention to specific regions
        - Modulate inhibition and excitation
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &AttentionalSelection::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("processCompetition", &AttentionalSelection::processCompetition,
             py::arg("competitors"), py::arg("globalInhibition") = 0.5f,
             "Process competing sensory inputs and select winners")
        .def("focusOnRegion", &AttentionalSelection::focusOnRegion, py::arg("region"),
             "Apply attention to a specific region (focus processing there)")
        .def("releaseAttention", &AttentionalSelection::releaseAttention,
             "Release attention (allow all regions equal processing)")
        .def("getAttendedRegions", &AttentionalSelection::getAttendedRegions, "Get current attention focus")
        .def("setInhibitionStrength", &AttentionalSelection::setInhibitionStrength, py::arg("strength"),
             "Set inhibition strength")
        .def("setExcitationStrength", &AttentionalSelection::setExcitationStrength, py::arg("strength"),
             "Set excitation strength")
        .def("setCompetitionThreshold", &AttentionalSelection::setCompetitionThreshold, py::arg("threshold"),
             "Set competition threshold")
        .def("getInhibitionFor", &AttentionalSelection::getInhibitionFor, py::arg("neuron"),
             "Get inhibition level for a neuron")
        .def("getExcitationFor", &AttentionalSelection::getExcitationFor, py::arg("neuron"),
             "Get excitation level for a neuron")
        .def("update", &AttentionalSelection::update, py::arg("dt"), "Update attention dynamics")
        .def("isAttended", &AttentionalSelection::isAttended, py::arg("neuron"), "Is this neuron currently attended?")
        .def("getWinners", &AttentionalSelection::getWinners, py::return_value_policy::reference_internal, "Get attention winners")
        .def("applyTopDownBias", &AttentionalSelection::applyTopDownBias, py::arg("neuron"), py::arg("biasStrength"),
             "Apply top-down attention (goal-directed)")
        .def("applyBottomUpSalience", &AttentionalSelection::applyBottomUpSalience, py::arg("neuron"), py::arg("salienceStrength"),
             "Apply bottom-up attention (stimulus-driven)")
        .def("reset", &AttentionalSelection::reset, "Reset attention");
}

} // namespace nlm