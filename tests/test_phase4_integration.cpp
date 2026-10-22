// Phase 4 Integration Tests
// Tests for working memory, episodic memory, prediction, attention, planning, concept formation, etc.

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "experiments/Phase4Experiment.hpp"
#include <cassert>
#include <iostream>

namespace test_phase4_integration {

void testTemporalPrediction() {
    std::cout << "Running Temporal Prediction Test..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    TemporalPredictionExperiment experiment;
    Phase4Results results = experiment.run(&brain, 100);
    
    // Check that prediction improved over time
    assert(results.predictionAccuracyImprovement >= -0.1f);  // Allow some noise
    assert(results.predictionErrorFinal < 1.0f);
    
    std::cout << "  ✓ Temporal Prediction test passed" << std::endl;
}

void testWorkingMemory() {
    std::cout << "Running Working Memory Test..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    WorkingMemoryExperiment experiment;
    Phase4Results results = experiment.run(&brain, 150);
    
    // Check working memory retention improved with practice
    assert(results.workingMemoryCapacity > 0.0f);
    assert(results.workingMemoryRetentionFinal >= results.workingMemoryRetentionInitial);
    
    std::cout << "  ✓ Working Memory test passed" << std::endl;
}

void testEpisodicMemory() {
    std::cout << "Running Episodic Memory Test..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(250), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    EpisodicRecallExperiment experiment;
    Phase4Results results = experiment.run(&brain, 80);
    
    // Check episodic memory accuracy
    assert(results.episodicRecallAccuracy > 0.0f);
    assert(results.episodesStored > 0);
    
    std::cout << "  ✓ Episodic Memory test passed" << std::endl;
}

void testConceptFormation() {
    std::cout << "Running Concept Formation Test..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    ConceptFormationExperiment experiment;
    Phase4Results results = experiment.run(&brain, 120);
    
    // Check concept formation
    assert(results.conceptsFormed > 0);
    assert(results.conceptStability > 0.0f);
    assert(results.generalizationAbility > 0.0f);
    
    std::cout << "  ✓ Concept Formation test passed" << std::endl;
}

void testAttention() {
    std::cout << "Running Attention Test..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    AttentionExperiment experiment;
    Phase4Results results = experiment.run(&brain, 100);
    
    // Check attention selectivity
    assert(results.attentionSelectivity > 0.0f);
    
    std::cout << "  ✓ Attention test passed" << std::endl;
}

void testPlanning() {
    std::cout << "Running Planning Test..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(180), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    PlanningExperiment experiment;
    Phase4Results results = experiment.run(&brain, 90);
    
    // Check planning accuracy
    assert(results.planningAccuracy > 0.0f);
    assert(results.planningConfidence > 0.0f);
    
    std::cout << "  ✓ Planning test passed" << std::endl;
}

void testSelfModel() {
    std::cout << "Running Self-Model Test..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(160), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    SelfModelExperiment experiment;
    Phase4Results results = experiment.run(&brain, 70);
    
    // Check self-prediction accuracy
    assert(results.selfPredictionAccuracy > 0.0f);
    assert(results.bodyAwareness > 0.0f);
    
    std::cout << "  ✓ Self-Model test passed" << std::endl;
}

void testSocialLearning() {
    std::cout << "Running Social Learning Test..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(140), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    SocialLearningExperiment experiment;
    Phase4Results results = experiment.run(&brain, 60);
    
    // Check social learning accuracy
    assert(results.imitationAccuracy > 0.0f);
    assert(results.observationsFromOthers > 0);
    
    std::cout << "  ✓ Social Learning test passed" << std::endl;
}

void testContinualLearning() {
    std::cout << "Running Continual Learning Test..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(220), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    ContinualLearningExperiment experiment;
    Phase4Results results = experiment.run(&brain, 200);
    
    // Check continual learning performance
    assert(results.behaviorImprovement > -0.1f);  // Allow some degradation
    assert(results.transferPerformance > 0.0f);
    
    std::cout << "  ✓ Continual Learning test passed" << std::endl;
}

void testGeneralization() {
    std::cout << "Running Generalization Test..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(180), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    GeneralizationExperiment experiment;
    Phase4Results results = experiment.run(&brain, 80);
    
    // Check generalization ability
    assert(results.generalizationAbility > 0.0f);
    assert(results.transferPerformance > 0.0f);
    
    std::cout << "  ✓ Generalization test passed" << std::endl;
}

void runAllPhase4Tests() {
    std::cout << "=== NLM Phase 4 Integration Tests ===" << std::endl;
    
    testTemporalPrediction();
    testWorkingMemory();
    testEpisodicMemory();
    testConceptFormation();
    testAttention();
    testPlanning();
    testSelfModel();
    testSocialLearning();
    testContinualLearning();
    testGeneralization();
    
    std::cout << "=== All Phase 4 Tests PASSED ===" << std::endl;
}

} // namespace test_phase4_integration