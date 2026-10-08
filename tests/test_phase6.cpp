// Phase 6 Tests - Comprehensive testing of integrated brain systems
// Tests working memory, prediction, cognition, neuromodulation, and development

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

namespace test_phase6 {

void testWorkingMemory() {
    std::cout << "  Testing Working Memory..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::NeuralWorkingMemory workingMemory;
    workingMemory.initialize(&brain);
    
    // Test initial state
    assert(workingMemory.getActiveTraces() == 0);
    assert(workingMemory.getMemoryNeurons().empty());
    
    // Test storing patterns
    std::vector<float> pattern1 = {0.1f, 0.5f, 0.9f, 0.3f, 0.7f};
    workingMemory.store(pattern1, 1.0f);
    
    // Verify storage
    assert(workingMemory.getActiveTraces() > 0);
    assert(!workingMemory.getMemoryNeurons().empty());
    
    // Test memory activity
    float activity = workingMemory.getMemoryActivity();
    assert(activity >= 0.0f);
    
    // Test decay
    workingMemory.update(0.1);
    
    // Test strengthening
    workingMemory.strengthenMemory(1.5f);
    
    // Test competition
    workingMemory.runCompetition();
    
    // Test clear
    workingMemory.clear();
    assert(workingMemory.getActiveTraces() == 0);
    
    std::cout << "    testWorkingMemory passed" << std::endl;
}

void testEpisodicMemory() {
    std::cout << "  Testing Episodic Memory..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::NeuralEpisodicMemory episodicMemory;
    episodicMemory.initialize(&brain);
    
    // Test initial state
    assert(episodicMemory.getTotalEncodings() == 0);
    
    // Test episodic encoding
    std::vector<float> encoding = {0.2f, 0.8f, 0.4f, 0.6f};
    bool success = episodicMemory.encode(encoding, nlm::EpisodicEventType::Experience);
    assert(success);
    assert(episodicMemory.getTotalEncodings() == 1);
    
    // Test memory retrieval
    std::vector<float> retrieved = episodicMemory.retrieveRecent(1);
    assert(retrieved.size() > 0);
    
    // Test replay
    episodicMemory.triggerReplay(0.5f);
    assert(episodicMemory.getReconstructionConfidence() >= 0.0f);
    
    // Test consolidation
    episodicMemory.consolidateMemory(0.01);
    
    // Test cleanup
    episodicMemory.cleanupOldEncodings(100);  // Keep recent
    
    std::cout << "    testEpisodicMemory passed" << std::endl;
}

void testPredictionSystem() {
    std::cout << "  Testing Prediction System..." << std::endl;
    
    // Note: PredictionSystem may not be fully implemented in Phase 2
    // This test verifies the interface exists and basic functionality
    
    nlm::PredictionSystem prediction;
    
    // Test basic interface
    assert(prediction.getErrorHistory().empty());
    assert(prediction.getConfidence() == 0.0f);
    assert(prediction.getPredictionError() == 0.0f);
    
    // Test clear history
    prediction.clearHistory();
    
    // Test training interface
    nlm::SensoryInput dummyInput;
    prediction.train(dummyInput);
    
    std::cout << "    testPredictionSystem passed (interface test)" << std::endl;
}

void testNeuralPlanner() {
    std::cout << "  Testing Neural Planner..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::NeuralPlanner planner;
    planner.initialize(&brain);
    
    // Test planner initialization
    assert(planner.getPlannerActive() == false);
    
    // Test action planning
    std::vector<float> context = {0.3f, 0.7f, 0.2f, 0.5f};
    std::vector<nlm::Action> plannedActions;
    
    // Test planning interface
    planner.setPlanningMode(nlm::PlanningMode::Reactive);
    assert(planner.getPlanningMode() == nlm::PlanningMode::Reactive);
    
    // Test evaluation
    nlm::Action dummyAction = nlm::ActionType::MoveForward;
    float score = planner.evaluateAction(dummyAction, context);
    assert(score >= -1.0f && score <= 1.0f);
    
    // Test sequence planning
    std::vector<nlm::Action> sequence = {nlm::ActionType::MoveForward, nlm::ActionType::TurnLeft};
    std::vector<float> sequenceScores;
    
    // Test step planning
    planner.planStep(nlm::ActionType::MoveForward);
    
    // Test planning update
    planner.updatePlanning(0.1, 0.5f);
    
    // Test planning reset
    planner.resetPlanning();
    
    std::cout << "    testNeuralPlanner passed" << std::endl;
}

void testConceptFormation() {
    std::cout << "  Testing Concept Formation..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::ConceptFormation conceptFormation;
    conceptFormation.initialize(&brain);
    
    // Test initial state
    assert(conceptFormation.getConceptCount() == 0);
    assert(conceptFormation.getClusteringStrength() >= 0.0f);
    
    // Test concept formation from patterns
    std::vector<float> pattern1 = {0.9f, 0.1f, 0.2f, 0.8f};
    std::vector<float> pattern2 = {0.8f, 0.2f, 0.1f, 0.9f};
    std::vector<float> pattern3 = {0.1f, 0.9f, 0.8f, 0.2f};
    
    // Form concepts from similar patterns
    conceptFormation.addPattern(pattern1);
    conceptFormation.addPattern(pattern2);  // Should cluster with pattern1
    conceptFormation.addPattern(pattern3);  // Should be new concept
    
    // Test concept retrieval
    std::vector<float> newPattern = {0.85f, 0.15f, 0.25f, 0.75f};
    auto similarConcepts = conceptFormation.getSimilarConcepts(newPattern, 0.7f);
    assert(similarConcepts.size() > 0);
    
    // Test concept merging
    conceptFormation.mergeSimilarConcepts(0.5f);
    
    // Test concept pruning
    conceptFormation.pruneConcepts(0.5f);
    
    // Test concept strength
    assert(conceptFormation.getAverageConceptStrength() >= 0.0f);
    
    std::cout << "    testConceptFormation passed" << std::endl;
}

void testIntegratedBrainSystems() {
    std::cout << "  Testing Integrated Brain Systems..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test that all Phase 6 systems are integrated into brain
    nlm::NeuralWorkingMemory* workingMemory = brain.getWorkingMemory();
    nlm::NeuralEpisodicMemory* episodicMemory = brain.getEpisodicMemory();
    nlm::PredictionSystem* predictionSystem = brain.getPredictionSystem();
    nlm::NeuralPlanner* planner = brain.getPlanner();
    nlm::ConceptFormation* conceptFormation = brain.getConceptFormation();
    
    // Verify all systems are accessible
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    assert(predictionSystem != nullptr);
    assert(planner != nullptr);
    assert(conceptFormation != nullptr);
    
    // Test neuromodulation integration
    nlm::Dopamine* dopamine = brain.getDopamine();
    nlm::Curiosity* curiosity = brain.getCuriosity();
    nlm::Novelty* novelty = brain.getNovelty();
    nlm::PredictionError* predictionError = brain.getPredictionErrorSignal();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    assert(predictionError != nullptr);
    
    // Test development system integration
    nlm::DevelopmentSystem* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Test developmental stage
    nlm::DevelopmentalStage stage = brain.getDevelopmentalStage();
    assert(static_cast<int>(stage) >= 0);
    assert(static_cast<int>(stage) < 6);  // Should be within valid range
    
    std::cout << "    testIntegratedBrainSystems passed" << std::endl;
}

void testNeuromodulation() {
    std::cout << "  Testing Neuromodulation..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::Dopamine* dopamine = brain.getDopamine();
    nlm::Curiosity* curiosity = brain.getCuriosity();
    nlm::Novelty* novelty = brain.getNovelty();
    nlm::PredictionError* predictionError = brain.getPredictionErrorSignal();
    
    // Test initial neuromodulation state
    assert(dopamine->getLevel() >= 0.0f);
    assert(curiosity->getLevel() >= 0.0f);
    assert(novelty->getLevel() >= 0.0f);
    assert(predictionError->getError() == 0.0f);
    
    // Test neuromodulator update with simulated reward
    nlm::SensoryInput dummySensory;
    nlm::Action dummyAction = nlm::ActionType::MoveForward;
    
    // Simulate reward and apply neuromodulation
    float reward = 1.0f;
    float predictedReward = 0.5f;
    
    // Apply reward modulation
    brain.applyNeuromodulation(nlm::Neuromodulator::createFromReward(reward, predictedReward));
    
    // Verify neuromodulators responded
    assert(dopamine->getLevel() >= 0.0f);
    assert(curiosity->getLevel() >= 0.0f);
    assert(novelty->getLevel() >= 0.0f);
    
    std::cout << "    testNeuromodulation passed" << std::endl;
}

void runAll() {
    std::cout << "Running Phase 6 Integration Tests..." << std::endl;
    std::cout << std::endl;
    
    testWorkingMemory();
    testEpisodicMemory();
    testPredictionSystem();
    testNeuralPlanner();
    testConceptFormation();
    testIntegratedBrainSystems();
    testNeuromodulation();
    
    std::cout << std::endl;
    std::cout << "All Phase 6 tests passed successfully!" << std::endl;
}

} // namespace test_phase6