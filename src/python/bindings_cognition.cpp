// Cognitive Systems Module for NLM Python Bindings
// This file contains Python bindings for cognitive-related systems

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/agent/AttentionalSelection.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"

namespace py = pybind11;
namespace nlm {

void bind_cognitive_systems(py::module_ &m) {
    // === NEURAL PLANNER ===
    py::class_<NeuralPlanner>(m, "NeuralPlanner",
        "Neural planner with action sequence evaluation")
        .def(py::init<>())
        .def("initialize", &NeuralPlanner::initialize,
            py::arg("brain"), "Initialize neural planner")
        .def("planAction", &NeuralPlanner::planAction,
            py::arg("currentState"), py::arg("goalState"),
            "Plan action sequence from current to goal state")
        .def("evaluateAction", &NeuralPlanner::evaluateAction,
            py::arg("action"), py::arg("currentState"),
            "Evaluate action quality")
        .def("getActionSequence", &NeuralPlanner::getActionSequence,
            "Get current action sequence")
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth,
            py::arg("depth"), "Set planning depth")
        .def("getPlanningDepth", &NeuralPlanner::getPlanningDepth,
            "Get current planning depth")
        .def("clearActionSequence", &NeuralPlanner::clearActionSequence,
            "Clear current action sequence")
        .def("getEvaluationMetrics", &NeuralPlanner::getEvaluationMetrics,
            "Get action evaluation metrics");
    
    // === CONCEPT FORMATION ===
    py::class_<ConceptFormation>(m, "ConceptFormation",
        "Concept formation from experience patterns")
        .def(py::init<>())
        .def("initialize", &ConceptFormation::initialize,
            py::arg("brain"), "Initialize concept formation")
        .def("processPattern", &ConceptFormation::processPattern,
            py::arg("pattern"), py::arg("strength"),
            "Process and learn from pattern")
        .def("getConcept", &ConceptFormation::getConcept,
            py::arg("conceptId"), "Get concept by ID")
        .def("getAllConcepts", &ConceptFormation::getAllConcepts,
            "Get all learned concepts")
        .def("findSimilarConcepts", &ConceptFormation::findSimilarConcepts,
            py::arg("pattern"), "Find similar concepts")
        .def("removeConcept", &ConceptFormation::removeConcept,
            py::arg("conceptId"), "Remove concept")
        .def("clearConcepts", &ConceptFormation::clearConcepts,
            "Clear all concepts")
        .def("getConceptCount", &ConceptFormation::getConceptCount,
            "Get number of learned concepts")
        .def("updateConceptStrength", &ConceptFormation::updateConceptStrength,
            py::arg("conceptId"), py::arg("newStrength"),
            "Update concept strength");
    
    // === ATTENTIONAL SELECTION ===
    py::class_<AttentionalSelection>(m, "AttentionalSelection",
        "Competitive selection dynamics for attention")
        .def(py::init<>())
        .def("initialize", &AttentionalSelection::initialize,
            py::arg("brain"), "Initialize attentional selection")
        .def("processCompetition", &AttentionalSelection::processCompetition,
            py::arg("competitors"), "Process competition among items")
        .def("getAttendedItem", &AttentionalSelection::getAttendedItem,
            "Get currently attended item")
        .def("setInhibitionStrength", &AttentionalSelection::setInhibitionStrength,
            py::arg("strength"), "Set inhibition strength")
        .def("setExcitationStrength", &AttentionalSelection::setExcitationStrength,
            py::arg("strength"), "Set excitation strength")
        .def("getAttentionWeights", &AttentionalSelection::getAttentionWeights,
            "Get attention weights for all items")
        .def("reset", &AttentionalSelection::reset,
            "Reset attentional selection");
    
    // === COGNITIVE STATE ===
    py::class_<CognitiveState>(m, "CognitiveState",
        "Represents cognitive state of the system")
        .def(py::init<>())
        .def_readwrite("activeConcepts", &CognitiveState::activeConcepts)
        .def_readwrite("attentionFocus", &CognitiveState::attentionFocus)
        .def_readwrite("workingMemoryContent", &CognitiveState::workingMemoryContent)
        .def_readwrite("planningGoal", &CognitiveState::planningGoal)
        .def_readwrite("timestamp", &CognitiveState::timestamp)
        .def("addActiveConcept", &CognitiveState::activeConcepts.emplace_back,
            py::arg("conceptId"), "Add active concept")
        .def("setAttentionFocus", &CognitiveState::setAttentionFocus,
            py::arg("itemId"), "Set attention focus");
    
    // === ACTION EVALUATION METRICS ===
    py::class_<ActionEvaluationMetrics>(m, "ActionEvaluationMetrics",
        "Metrics for action evaluation")
        .def(py::init<>())
        .def_readwrite("expectedReward", &ActionEvaluationMetrics::expectedReward)
        .def_readwrite("expectedRisk", &ActionEvaluationMetrics::expectedRisk)
        .def_readwrite("expectedTime", &ActionEvaluationMetrics::expectedTime)
        .def_readwrite("expectedCost", &ActionEvaluationMetrics::expectedCost)
        .def_readwrite("confidence", &ActionEvaluationMetrics::confidence)
        .def("calculateUtility", &ActionEvaluationMetrics::calculateUtility,
            "Calculate overall utility score");
}

// === FACTORY FUNCTIONS ===

void bind_cognitive_factories(py::module_ &m) {
    m.def("createNeuralPlanner", []() {
        return std::make_unique<NeuralPlanner>();
    }, "Create a neural planner instance");
    
    m.def("createConceptFormation", []() {
        return std::make_unique<ConceptFormation>();
    }, "Create a concept formation instance");
    
    m.def("createAttentionalSelection", []() {
        return std::make_unique<AttentionalSelection>();
    }, "Create an attentional selection instance");
    
    m.def("createDefaultCognitiveSystem", []() {
        auto planner = std::make_unique<NeuralPlanner>();
        auto conceptFormation = std::make_unique<ConceptFormation>();
        auto attention = std::make_unique<AttentionalSelection>();
        
        return std::tuple<std::unique_ptr<NeuralPlanner>,
                         std::unique_ptr<ConceptFormation>,
                         std::unique_ptr<AttentionalSelection>>(
            std::move(planner),
            std::move(conceptFormation),
            std::move(attention)
        );
    }, "Create a complete default cognitive system");
}

} // namespace nlm