// Phase 6 Enhanced Python Bindings Improvements
// 
// This file adds Phase 6 specific functionality to the Python bindings
// to better support the integrated brain systems and advanced features

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

// Include Phase 6 specific headers
#include "../src/experiments/Phase6IntegratedExperiment.hpp"
#include "../src/memory/WorkingMemory.hpp"
#include "../src/memory/EpisodicMemory.hpp"
#include "../src/memory/SemanticMemory.hpp"
#include "../src/memory/ProceduralMemory.hpp"
#include "../src/memory/AssociativeMemory.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/Attention.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/prediction/PredictionSystem.hpp"

namespace py = pybind11;
namespace nlm {

// Enhanced Python bindings for Phase 6 integration
void addPhase6Bindings(py::module_& m) {
    
    // Memory System Classes
    py::class_<WorkingMemory>(m, "WorkingMemory", R"pbdoc(Working memory system with persistent activity and competition)pbdoc")
        .def("getActiveTraces", &WorkingMemory::getActiveTraces,
             "Get number of active memory traces")
        .def("getMaxCapacity", &WorkingMemory::getMaxCapacity,
             "Get maximum working memory capacity")
        .def("addTrace", &WorkingMemory::addTrace,
             py::arg("trace"), "Add a memory trace")
        .def("removeTrace", &WorkingMemory::removeTrace,
             py::arg("trace"), "Remove a memory trace");
    
    py::class_<EpisodicMemory>(m, "EpisodicMemory", R"pbdoc(Episodic memory system for storing experience episodes)pbdoc")
        .def("getEpisodeCount", &EpisodicMemory::getEpisodeCount,
             "Get number of stored episodes")
        .def("getMaxEpisodes", &EpisodicMemory::getMaxEpisodes,
             "Get maximum number of episodes")
        .def("addEpisode", &EpisodicMemory::addEpisode,
             py::arg("episode"), "Add an episode to memory")
        .def("getEpisode", &EpisodicMemory::getEpisode,
             py::arg("index"), "Get episode at specified index")
        .def("replayEpisodes", &EpisodicMemory::replayEpisodes,
             py::arg("count"), "Replay specified number of episodes");
    
    py::class_<SemanticMemory>(m, "SemanticMemory", R"pbdoc(Semantic memory system for knowledge representation)pbdoc")
        .def("getKnowledgeBase", &SemanticMemory::getKnowledgeBase,
             "Get semantic knowledge base")
        .def("addKnowledge", &SemanticMemory::addKnowledge,
             py::arg("concept"), py::arg("data"), "Add semantic knowledge")
        .def("getConcept", &SemanticMemory::getConcept,
             py::arg("concept"), "Retrieve semantic concept");
    
    py::class_<ProceduralMemory>(m, "ProceduralMemory", R"pbdoc(Procedural memory system for skills and habits)pbdoc")
        .def("getSkillCount", &ProceduralMemory::getSkillCount,
             "Get number of learned skills")
        .def("addSkill", &ProceduralMemory::addSkill,
             py::arg("skill"), "Add a procedural skill")
        .def("executeSkill", &ProceduralMemory::executeSkill,
             py::arg("skill"), "Execute a procedural skill");
    
    py::class_<AssociativeMemory>(m, "AssociativeMemory", R"pbdoc(Associative memory system for pattern associations)pbdoc")
        .def("getAssociationCount", &AssociativeMemory::getAssociationCount,
             "Get number of associations")
        .def("createAssociation", &AssociativeMemory::createAssociation,
             py::arg("pattern1"), py::arg("pattern2"), py::arg("strength"), "Create pattern association")
        .def("getAssociationStrength", &AssociativeMemory::getAssociationStrength,
             py::arg("pattern1"), py::arg("pattern2"), "Get association strength");
    
    // Neuromodulation System Classes
    py::class_<Dopamine>(m, "Dopamine", R"pbdoc(Dopamine neuromodulation system for reward processing)pbdoc")
        .def("getLevel", &Dopamine::getLevel,
             "Get current dopamine level")
        .def("getBaseline", &Dopamine::getBaseline,
             "Get baseline dopamine level")
        .def("setLevel", &Dopamine::setLevel,
             py::arg("level"), "Set dopamine level")
        .def("getModulationEffect", &Dopamine::getModulationEffect,
             "Get current neuromodulation effect");
    
    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity neuromodulation system for exploration drive)pbdoc")
        .def("getLevel", &Curiosity::getLevel,
             "Get current curiosity level")
        .def("getExplorationRate", &Curiosity::getExplorationRate,
             "Get exploration rate");
    
    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty detection system for surprise computation)pbdoc")
        .def("getLevel", &Novelty::getLevel,
             "Get current novelty level")
        .def("detectNovelty", &Novelty::detectNovelty,
             py::arg("input"), "Detect novelty in input");
    
    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error system for learning and adaptation)pbdoc")
        .def("getError", &PredictionError::getError,
             "Get prediction error signal")
        .def("updatePrediction", &PredictionError::updatePrediction,
             py::arg("prediction"), py::arg("actual"), "Update prediction and compute error");
    
    // Cognition System Classes
    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation system for abstract representation)pbdoc")
        .def("getConceptCount", &ConceptFormation::getConceptCount,
             "Get number of formed concepts")
        .def("createConcept", &ConceptFormation::createConcept,
             py::arg("features"), "Create new concept from features")
        .def("getConcept", &ConceptFormation::getConcept,
             py::arg("id"), "Retrieve concept by ID")
        .def("matchConcept", &ConceptFormation::matchConcept,
             py::arg("input"), "Match input to existing concepts");
    
    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planning system for action sequence evaluation)pbdoc")
        .def("getPlanCount", &NeuralPlanner::getPlanCount,
             "Get number of planned actions")
        .def("createPlan", &NeuralPlanner::createPlan,
             py::arg("sequence"), "Create new action plan")
        .def("evaluatePlan", &NeuralPlanner::evaluatePlan,
             py::arg("plan"), "Evaluate plan quality")
        .def("getBestPlan", &NeuralPlanner::getBestPlan,
             "Get best available plan");
    
    py::class_<Attention>(m, "Attention", R"pbdoc(Attention system with competitive selection dynamics)pbdoc")
        .def("getFocusArea", &Attention::getFocusArea,
             "Get current focus area")
        .def("allocateAttention", &Attention::allocateAttention,
             py::arg("target"), py::arg("priority"), "Allocate attention to target")
        .def("getAttentionDistribution", &Attention::getAttentionDistribution,
             "Get current attention distribution");
    
    // Development System Classes
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Development system managing developmental stages and plasticity)pbdoc")
        .def("getCurrentStage", &DevelopmentSystem::getCurrentStage,
             "Get current developmental stage")
        .def("getStageDuration", &DevelopmentSystem::getStageDuration,
             py::arg("stage"), "Get duration of developmental stage")
        .def("getMaturityLevel", &DevelopmentSystem::getMaturityLevel,
             "Get current maturity level")
        .def("advanceDevelopment", &DevelopmentSystem::advanceDevelopment,
             py::arg("timeStep"), "Advance development by time step");
    
    // Prediction System Classes
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Prediction system integrated into brain loop)pbdoc")
        .def("getPrediction", &PredictionSystem::getPrediction,
             "Get current prediction")
        .def("setPrediction", &PredictionSystem::setPrediction,
             py::arg("prediction"), "Set prediction value")
        .def("getConfidence", &PredictionSystem::getConfidence,
             "Get prediction confidence")
        .def("updatePrediction", &PredictionSystem::updatePrediction,
             py::arg("input"), "Update prediction based on input");
    
    // Phase 6 Integration Test Class
    py::class_<Phase6IntegratedExperiment>(m, "Phase6IntegratedExperiment", R"pbdoc(Phase 6 integration test for verifying all brain systems work together)pbdoc")
        .def(py::init<>())
        .def("verifyIntegration", &Phase6IntegratedExperiment::verifyIntegration,
             "Run quick integration verification")
        .def("testMemoryIntegration", &Phase6IntegratedExperiment::testMemoryIntegration,
             "Test memory system integration")
        .def("testNeuromodulationIntegration", &Phase6IntegratedExperiment::testNeuromodulationIntegration,
             "Test neuromodulation system integration")
        .def("testCheckpointing", &Phase6IntegratedExperiment::testCheckpointing,
             "Test checkpoint save/load functionality")
        .def("testReplay", &Phase6IntegratedExperiment::testReplay,
             "Test replay system functionality")
        .def("run", &Phase6IntegratedExperiment::run,
             py::arg("config"), "Run full integration test");
    
    // Python-friendly wrapper functions for Phase 6
    m.def("createPhase6Experiment", []() {
        return std::make_shared<Phase6IntegratedExperiment>();
    }, "Create a Phase 6 integration experiment");
    
    m.def("runQuickPhase6Verification", []() {
        auto experiment = std::make_shared<Phase6IntegratedExperiment>();
        return experiment->verifyIntegration();
    }, "Run quick Phase 6 integration verification");
    
    // Enhanced configuration utilities for Phase 6
    m.def("getDefaultPhase6Config", []() {
        auto config = std::make_shared<Config>();
        
        // Set Phase 6 specific defaults
        config->set("neuron_count", 1000);
        config->set("working_memory.capacity", 100);
        config->set("episodic_memory.max_episodes", 1000);
        config->set("dopamine.scale", 1.0f);
        config->set("curiosity.adaptation_rate", 0.01f);
        config->set("prediction.horizon", 10);
        config->set("development.initial_plasticity", 1.0f);
        
        return config;
    }, "Create default Phase 6 configuration");
    
    // Phase 6 simulation utilities
    m.def("runPhase6Demo", []() {
        // This would run the Phase 6 demo similar to the C++ executable
        // For now, returns the experiment object
        return std::make_shared<Phase6IntegratedExperiment>();
    }, "Create Phase 6 demo simulation");
}

} // namespace nlm
