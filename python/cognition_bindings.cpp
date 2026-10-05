#pragma once

#include "cognition_bindings.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <typeindex>
#include <unordered_map>

namespace py = pybind11;

namespace nlm {
    
    class CognitionModule : public BindingModule {
    public:
        std::string getName() const override { return "cognition"; }
        std::string getVersion() const override { return "1.0.0"; }
        std::string getDescription() const override { 
            return "Cognition system bindings for NLM (NeuralPlanner, ConceptFormation, etc.)"; 
        }
        
        void registerBindings(pybind11::module_& m) override {
            // Cognition class bindings using ModuleBuilder pattern
            auto builder = ModuleBuilder(m);
            
            // NeuralPlanner class
            builder.class_<NeuralPlanner>("NeuralPlanner", R"pbdoc(Neural planning system)pbdoc")
                .def(py::init<>())
                .def("generatePlan", &NeuralPlanner::generatePlan, py::arg("goal"), py::arg("constraints"),
                     "Generate a plan to achieve a goal")
                .def("getPlanSteps", &NeuralPlanner::getPlanSteps,
                     py::return_value_policy::reference_internal,
                     "Get the current plan steps")
                .def("setGoal", &NeuralPlanner::setGoal, py::arg("goal"),
                     "Set the current goal")
                .def("getGoal", &NeuralPlanner::getGoal,
                     "Get the current goal")
                .def("updatePlan", &NeuralPlanner::updatePlan, py::arg("timestep"),
                     "Update the plan based on current state")
                .def("evaluatePlan", &NeuralPlanner::evaluatePlan, py::arg("plan"),
                     "Evaluate the quality of a plan");
            
            // ConceptFormation class
            builder.class_<ConceptFormation>("ConceptFormation", R"pbdoc(Concept formation system)pbdoc")
                .def(py::init<>())
                .def("learnConcept", &ConceptFormation::learnConcept, py::arg("data"), py::arg("context"),
                     "Learn a new concept from data and context")
                .def("getConcepts", &ConceptFormation::getConcepts,
                     py::return_value_policy::reference_internal,
                     "Get all learned concepts")
                .def("classifyData", &ConceptFormation::classifyData, py::arg("data"),
                     "Classify data into existing concepts")
                .def("forgetConcept", &ConceptFormation::forgetConcept, py::arg("conceptId"),
                     "Forget a concept")
                .def("getConceptById", &ConceptFormation::getConceptById, py::arg("id"),
                     py::return_value_policy::reference_internal,
                     "Get concept by ID")
                .def("updateConcept", &ConceptFormation::updateConcept, py::arg("id"), py::arg("data"),
                     "Update a concept with new data");
            
            // SelfModel class
            builder.class_<SelfModel>("SelfModel", R"pbdoc(Self-modeling system)pbdoc")
                .def(py::init<>())
                .def("updateSelfModel", &SelfModel::updateSelfModel, py::arg("percept"), py::arg("action"),
                     "Update self-model based on percept and action")
                .def("predictOutcome", &SelfModel::predictOutcome, py::arg("action"),
                     "Predict outcome of an action")
                .def("getStateRepresentation", &SelfModel::getStateRepresentation,
                     py::return_value_policy::reference_internal,
                     "Get current state representation")
                .def("getActionPrediction", &SelfModel::getActionPrediction, py::arg("action"),
                     "Get predicted outcome for an action")
                .def("reset", &SelfModel::reset, "Reset self-model");
            
            // SocialLearning class
            builder.class_<SocialLearning>("SocialLearning", R"pbdoc(Social learning system)pbdoc")
                .def(py::init<>())
                .def("learnFromObservation", &SocialLearning::learnFromObservation, py::arg("observer"), py::arg("observed"),
                     "Learn from observing another agent's behavior")
                .def("getSocialKnowledge", &SocialLearning::getSocialKnowledge,
                     py::return_value_policy::reference_internal,
                     "Get social knowledge base")
                .def("updateSocialModel", &SocialLearning::updateSocialModel, py::arg("newInfo"),
                     "Update social model with new information")
                .def("getSocialNorm", &SocialLearning::getSocialNorm, py::arg("context"),
                     "Get social norm for a context")
                .def("setSocialNorm", &SocialLearning::setSocialNorm, py::arg("context"), py::arg("norm"),
                     "Set social norm for a context");
            
            // SpatialRepresentation class
            builder.class_<SpatialRepresentation>("SpatialRepresentation", R"pbdoc(Spatial representation system)pbdoc")
                .def(py::init<>())
                .def("addLandmark", &SpatialRepresentation::addLandmark, py::arg("id"), py::arg("position"), py::arg("type"),
                     "Add a landmark to the spatial map")
                .def("getLandmark", &SpatialRepresentation::getLandmark, py::arg("id"),
                     py::return_value_policy::reference_internal,
                     "Get landmark by ID")
                .def("getPosition", &SpatialRepresentation::getPosition, py::arg("landmarkId"),
                     "Get position of landmark")
                .def("calculateDistance", &SpatialRepresentation::calculateDistance, py::arg("id1"), py::arg("id2"),
                     "Calculate distance between landmarks")
                .def("getPath", &SpatialRepresentation::getPath, py::arg("startId"), py::arg("endId"),
                     py::return_value_policy::reference_internal,
                     "Get path between landmarks");
            
            // TemporalRelation class
            builder.class_<TemporalRelation>("TemporalRelation", R"pbdoc(Temporal relation system)pbdoc")
                .def(py::init<>())
                .def("recordTemporalEvent", &TemporalRelation::recordTemporalEvent, py::arg("eventId"), py::arg("timestamp"),
                     "Record a temporal event")
                .def("getEventDuration", &TemporalRelation::getEventDuration, py::arg("eventId1"), py::arg("eventId2"),
                     "Get duration between two events")
                .def("getEventOrder", &TemporalRelation::getEventOrder, py::arg("eventId1"), py::arg("eventId2"),
                     "Get temporal order of two events")
                .def("getTemporalSequence", &TemporalRelation::getTemporalSequence,
                     py::return_value_policy::reference_internal,
                     "Get temporal sequence of events")
                .def("predictFutureEvent", &TemporalRelation::predictFutureEvent, py::arg("eventId"), py::arg("timeOffset"),
                     "Predict future event based on temporal patterns");
            
            // Context manager support for cognitive operations
            builder.class_<NeuralPlanner>("NeuralPlannerContext", R"pbdoc(Context manager for NeuralPlanner operations)pbdoc")
                .def(py::init<NeuralPlanner>())
                .def("enter", [](NeuralPlanner& self) { return self; })
                .def("exit", [](NeuralPlanner&, const py::args& args) { return true; });
            
            builder.class_<ConceptFormation>("ConceptFormationContext", R"pbdoc(Context manager for ConceptFormation operations)pbdoc")
                .def(py::init<ConceptFormation>())
                .def("enter", [](ConceptFormation& self) { return self; })
                .def("exit", [](ConceptFormation&, const py::args& args) { return true; });
            
            // Cognition factory functions
            m.def("create_neural_planner", []() {
                return std::make_shared<NeuralPlanner>();
            }, "Create a new neural planner");
            
            m.def("create_concept_formation", []() {
                return std::make_shared<ConceptFormation>();
            }, "Create a new concept formation system");
            
            m.def("create_self_model", []() {
                return std::make_shared<SelfModel>();
            }, "Create a new self-model");
            
            m.def("create_social_learning", []() {
                return std::make_shared<SocialLearning>();
            }, "Create a new social learning system");
            
            m.def("create_spatial_representation", []() {
                return std::make_shared<SpatialRepresentation>();
            }, "Create a new spatial representation system");
            
            m.def("create_temporal_relation", []() {
                return std::make_shared<TemporalRelation>();
            }, "Create a new temporal relation system");
            
            // Cognition utilities
            m.def("get_cognitive_system_names", []() {
                return std::vector<std::string>{
                    "neural_planner", "concept_formation", "self_model",
                    "social_learning", "spatial_representation", "temporal_relation"
                };
            }, "Get list of available cognitive systems");
            
            m.def("cognitive_system_info", [](const std::string& systemName) {
                // Implementation would return system information
                return std::map<std::string, std::string>{{ "name", systemName }, { "description", "" }};
            }, py::arg("systemName"), "Get information about a cognitive system");
            
            // Cognitive planning utilities
            m.def("evaluate_plan_quality", [](const std::vector<std::string>& plan, double goalImportance) {
                // Implementation would evaluate plan quality
                return 0.5;
            }, py::arg("plan"), py::arg("goalImportance"), "Evaluate quality of a plan");
            
            m.def("extract_concepts_from_data", [](const py::object& data) {
                // Implementation would extract concepts from data
                return std::vector<std::string>{};
            }, py::arg("data"), "Extract concepts from data");
            
            // Cognitive validation
            m.def("validate_cognitive_state", [](const NeuralPlanner& planner) {
                // Implementation would validate planner state
                return true;
            }, py::arg("planner"), "Validate cognitive planner state");
            
            m.def("validate_concept_formation_state", [](const ConceptFormation& concepts) {
                // Implementation would validate concept formation state
                return true;
            }, py::arg("concepts"), "Validate concept formation state");
            
            // Cognitive simulation utilities
            m.def("simulate_cognitive_process", [](NeuralPlanner& planner, ConceptFormation& concepts, double timestep) {
                // Implementation would simulate cognitive process
                planner.updatePlan(timestep);
            }, py::arg("planner"), py::arg("concepts"), py::arg("timestep"), "Simulate cognitive process");
            
            m.def("learn_from_cognitive_experience", [](ConceptFormation& concepts, const py::object& experience) {
                // Implementation would learn from cognitive experience
            }, py::arg("concepts"), py::arg("experience"), "Learn from cognitive experience");
        }
        
        void initialize() override {
            // Initialize cognition module
        }
        
        void shutdown() override {
            // Cleanup cognition module resources
        }
        
        bool hasFeature(const std::string& feature) const override {
            return feature == "cognitive_factory" || feature == "cognitive_planning" || 
                   feature == "concept_extraction" || feature == "cognitive_simulation" ||
                   feature == "cognitive_context" || feature == "cognitive_validation";
        }
        
        py::object getFeature(const std::string& feature) override {
            if (feature == "cognitive_factory") {
                return py::module_::import("nlm.cognition").attr("cognitive_factory");
            } else if (feature == "cognitive_planning") {
                return py::module_::import("nlm.cognition").attr("cognitive_planning");
            }
            throw std::runtime_error("Feature not available: " + feature);
        }
    };
    
    // Register the cognition module
    static CognitionModule register_cognition_module() {
        auto module = std::make_shared<CognitionModule>();
        BindingRegistry::getInstance().registerModule(module);
        return *module;
    }
    
    // Module registration function
    PYBIND11_MODULE(cognition_bindings, m) {
        static CognitionModule module;
        module.registerBindings(m);
        
        m.doc() = R"pbdoc(
            NLM Cognition Python Bindings
            -----------------------------
            
            This module provides Python bindings for the NLM cognition system,
            including NeuralPlanner, ConceptFormation, SelfModel, SocialLearning,
            SpatialRepresentation, and TemporalRelation classes.
            
            It provides cognitive interfaces for planning, concept formation,
            self-modeling, social learning, spatial reasoning, and temporal
            relationships, with context managers and cognitive utilities.
            
            .. currentmodule:: cognition_bindings
            
            Cognition API
            =============""";
        
        // Register with the central registry
        BindingRegistry::getInstance().registerModule(std::make_shared<CognitionModule>(module));
    }
}