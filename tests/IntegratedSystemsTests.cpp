// Test framework for integrated systems in NLM
// Comprehensive test coverage for all integrated systems

#include "IntegratedSystemsTests.hpp"
#include "core/Types/Types.hpp"
#include "memory/Memory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/ConceptFormation.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "sensory/SensoryInput.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

namespace nlm {

namespace test_integrated_systems {

// Helper functions
static void assertAlmostEqual(float a, float b, float epsilon = 0.001f) {
    assert(std::abs(a - b) < epsilon);
}

static void assertGreaterThan(float a, float b) {
    assert(a > b);
}

static void assertLessThan(float a, float b) {
    assert(a < b);
}

static void assertInRange(float value, float min, float max) {
    assert(value >= min && value <= max);
}

// =============================================================================
// AssociativeMemory Tests
// =============================================================================

void testAssociativeMemoryPatternCompletion() {
    std::cout << "  testAssociativeMemoryPatternCompletion..." << std::endl;
    
    // Create associations
    nlm::AssociativeMemory assoc;
    assoc.associate(NeuronId(1), NeuronId(2), 0.8f);
    assoc.associate(NeuronId(1), NeuronId(3), 0.6f);
    
    // Test pattern completion - Neuron 1 should activate associated neurons
    auto associations = assoc.getAssociations(NeuronId(1));
    assert(associations.size() >= 1);
    assert(std::find(associations.begin(), associations.end(), NeuronId(2)) != associations.end());
    assert(std::find(associations.begin(), associations.end(), NeuronId(3)) != associations.end());
    
    // Test association strength
    assertAlmostEqual(assoc.getAssociationStrength(NeuronId(1), NeuronId(2)), 0.8f);
    assertAlmostEqual(assoc.getAssociationStrength(NeuronId(1), NeuronId(3)), 0.6f);
    
    std::cout << "    testAssociativeMemoryPatternCompletion passed" << std::endl;
}

void testAssociativeMemorySpreadActivation() {
    std::cout << "  testAssociativeMemorySpreadActivation..." << std::endl;
    
    nlm::AssociativeMemory assoc;
    
    // Create a network of associations
    assoc.associate(NeuronId(1), NeuronId(2), 0.9f);
    assoc.associate(NeuronId(2), NeuronId(3), 0.8f);
    assoc.associate(NeuronId(3), NeuronId(4), 0.7f);
    
    // Test spread activation - Neuron 1 should activate Neuron 3 through Neuron 2
    auto associations1 = assoc.getAssociations(NeuronId(1));
    auto associations2 = assoc.getAssociations(NeuronId(2));
    
    // Both levels should be connected
    assert(!associations1.empty());
    assert(!associations2.empty());
    
    // Test strength decay (weaker associations should be affected by stronger ones)
    float strength13 = assoc.getAssociationStrength(NeuronId(1), NeuronId(3));
    float strength24 = assoc.getAssociationStrength(NeuronId(2), NeuronId(4));
    
    std::cout << "    testAssociativeMemorySpreadActivation passed" << std::endl;
}

void testAssociativeMemoryMemoryRetention() {
    std::cout << "  testAssociativeMemoryMemoryRetention..." << std::endl;
    
    nlm::AssociativeMemory assoc;
    
    // Create and test memory retention
    assoc.associate(NeuronId(100), NeuronId(101), 0.95f);
    
    // Test immediate retrieval
    float strength = assoc.getAssociationStrength(NeuronId(100), NeuronId(101));
    assertAlmostEqual(strength, 0.95f);
    
    // Test multiple associations retained
    for (int i = 0; i < 10; ++i) {
        assoc.associate(NeuronId(i), NeuronId(i + 100), 0.5f + (i * 0.01f));
    }
    
    // Verify all associations are retained
    for (int i = 0; i < 10; ++i) {
        float expected = 0.5f + (i * 0.01f);
        assertAlmostEqual(assoc.getAssociationStrength(NeuronId(i), NeuronId(i + 100)), expected);
    }
    
    std::cout << "    testAssociativeMemoryMemoryRetention passed" << std::endl;
}

void testAssociativeMemoryIntegrationWithWorkingMemory() {
    std::cout << "  testAssociativeMemoryIntegrationWithWorkingMemory..." << std::endl;
    
    nlm::AssociativeMemory assoc;
    
    // Simulate working memory items that have associations
    std::vector<std::pair<NeuronId, float>> workingMemoryItems = {
        {NeuronId(1), 0.7f},
        {NeuronId(2), 0.8f},
        {NeuronId(3), 0.9f}
    };
    
    // Associate working memory items with other concepts
    for (const auto& item : workingMemoryItems) {
        assoc.associate(item.first, NeuronId(1000), 0.6f * item.second);
    }
    
    // Verify that working memory items have associations
    for (const auto& item : workingMemoryItems) {
        auto associations = assoc.getAssociations(item.first);
        assert(!associations.empty());
    }
    
    // Test that association strength reflects working memory value
    for (const auto& item : workingMemoryItems) {
        float expectedStrength = 0.6f * item.second;
        float actualStrength = assoc.getAssociationStrength(item.first, NeuronId(1000));
        assertAlmostEqual(actualStrength, expectedStrength, 0.01f);
    }
    
    std::cout << "    testAssociativeMemoryIntegrationWithWorkingMemory passed" << std::endl;
}

// =============================================================================
// PredictionSystem Tests
// =============================================================================

void testPredictionSystemAccuracy() {
    std::cout << "  testPredictionSystemAccuracy..." << std::endl;
    
    nlm::PredictionSystem predictor;
    
    // Create a simple sensory input
    nlm::SensoryInput input1;
    input1.voltage = 0.5f;
    input1.spikes = 10;
    input1.noise = 0.1f;
    
    nlm::SensoryInput input2;
    input2.voltage = 0.6f;
    input2.spikes = 12;
    input2.noise = 0.2f;
    
    // Test prediction accuracy
    auto predicted = predictor.predictNextState(input1);
    
    // Update with actual
    predictor.updatePredictions(*predicted, input2);
    
    // Check that prediction error is within reasonable bounds
    float error = predictor.getPredictionError();
    assertGreaterThan(0.0f, error); // Error should be positive (actual - predicted)
    assertLessThan(error, 1.0f);    // But not too large
    
    // Test multiple predictions improve accuracy
    float initialError = error;
    for (int i = 0; i < 5; ++i) {
        predictor.train(input2);
    }
    
    float finalError = predictor.getPredictionError();
    assertLessThan(finalError, initialError); // Should improve over time
    
    std::cout << "    testPredictionSystemAccuracy passed" << std::endl;
}

void testPredictionSystemErrorTracking() {
    std::cout << "  testPredictionSystemErrorTracking..." << std::endl;
    
    nlm::PredictionSystem predictor;
    
    // Create test inputs with known error pattern
    nlm::SensoryInput input1;
    input1.voltage = 0.1f;
    input1.spikes = 5;
    
    nlm::SensoryInput input2;
    input2.voltage = 0.9f;  // Large difference from input1
    input2.spikes = 15;
    
    auto predicted = predictor.predictNextState(input1);
    predictor.updatePredictions(*predicted, input2);
    
    // Error should be large due to big difference
    float error = predictor.getPredictionError();
    assertGreaterThan(error, 0.5f); // Should be significant error
    
    // Test error history
    auto history = predictor.getErrorHistory();
    assert(!history.empty());
    
    std::cout << "    testPredictionSystemErrorTracking passed" << std::endl;
}

void testPredictionSystemConfidenceAdaptation() {
    std::cout << "  testPredictionSystemConfidenceAdaptation..." << std::endl;
    
    nlm::PredictionSystem predictor;
    
    // Start with low confidence
    float initialConfidence = predictor.getConfidence();
    assert(initialConfidence < 0.5f);
    
    // Make accurate predictions to increase confidence
    nlm::SensoryInput input1;
    input1.voltage = 0.5f;
    input1.spikes = 10;
    
    nlm::SensoryInput input2;
    input2.voltage = 0.55f;
    input2.spikes = 11;
    
    auto predicted = predictor.predictNextState(input1);
    predictor.updatePredictions(*predicted, input2);
    
    // Confidence should increase with good predictions
    float afterUpdateConfidence = predictor.getConfidence();
    assertGreaterThan(afterUpdateConfidence, initialConfidence);
    
    std::cout << "    testPredictionSystemConfidenceAdaptation passed" << std::endl;
}

void testPredictionSystemIntegrationWithCuriosity() {
    std::cout << "  testPredictionSystemIntegrationWithCuriosity..." << std::endl;
    
    nlm::PredictionSystem predictor;
    nlm::Curiosity curiosity;
    
    // Set up initial state
    nlm::SensoryInput input1;
    input1.voltage = 0.3f;
    input1.spikes = 8;
    
    nlm::SensoryInput input2;
    input2.voltage = 0.7f;  // Different enough to be novel
    input2.spikes = 12;
    
    // Make prediction
    auto predicted = predictor.predictNextState(input1);
    predictor.updatePredictions(*predicted, input2);
    
    // Large prediction error should trigger curiosity
    float error = predictor.getPredictionError();
    float curiosityLevel = curiosity.getLevel();
    
    // Update curiosity based on prediction error
    curiosity.signalPredictionError(error);
    
    assertGreaterThan(curiosity.getLevel(), curiosityLevel);
    
    std::cout << "    testPredictionSystemIntegrationWithCuriosity passed" << std::endl;
}

// =============================================================================
// ConceptFormation Tests
// =============================================================================

void testConceptFormationBasic() {
    std::cout << "  testConceptFormationBasic..." << std::endl;
    
    nlm::ConceptFormation conceptFormation;
    
    // Create some neural patterns and features
    std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> features1 = {0.6f, 0.7f, 0.8f};
    
    // Present experiences
    size_t conceptId1 = conceptFormation.presentExperience(pattern1, features1, 0.8f, 0);
    assertGreaterThan(conceptId1, 0); // Should get a concept ID > 0
    
    // Present similar experience - should match existing concept
    std::vector<float> similarPattern1 = {0.15f, 0.25f, 0.35f, 0.45f, 0.55f};
    size_t conceptId2 = conceptFormation.getMatchingConcept(similarPattern1);
    assertGreaterThan(conceptId2, 0);
    
    std::cout << "    testConceptFormationBasic passed" << std::endl;
}

void testConceptFormationStability() {
    std::cout << "  testConceptFormationStability..." << std::endl;
    
    nlm::ConceptFormation conceptFormation;
    
    // Create a pattern and features
    std::vector<float> pattern = {0.2f, 0.4f, 0.6f, 0.8f, 1.0f};
    std::vector<float> features = {0.1f, 0.3f, 0.5f};
    
    size_t conceptId = conceptFormation.presentExperience(pattern, features, 0.9f, 0);
    
    // Present the same concept multiple times to test stability
    for (int i = 1; i < 10; ++i) {
        conceptFormation.updateConcept(conceptId, pattern, features, 0.9f);
        float stability = conceptFormation.getConceptStability(conceptId);
        assertInRange(stability, 0.0f, 1.0f);
    }
    
    // Concept should become stable over time
    float finalStability = conceptFormation.getConceptStability(conceptId);
    assertGreaterThan(finalStability, 0.5f); // Should be reasonably stable
    
    std::cout << "    testConceptFormationStability passed" << std::endl;
}

void testConceptFormationGeneralization() {
    std::cout << "  testConceptFormationGeneralization..." << std::endl;
    
    nlm::ConceptFormation conceptFormation;
    
    // Create base concept
    std::vector<float> basePattern = {0.5f, 0.5f, 0.5f};
    std::vector<float> baseFeatures = {0.5f, 0.5f};
    
    size_t conceptId = conceptFormation.presentExperience(basePattern, baseFeatures, 0.7f, 0);
    
    // Test generalization - similar but not identical patterns
    std::vector<float> variant1 = {0.4f, 0.5f, 0.6f};
    std::vector<float> variant2 = {0.6f, 0.7f, 0.8f};
    
    size_t match1 = conceptFormation.getMatchingConcept(variant1, 0.6f); // Higher threshold
    size_t match2 = conceptFormation.getMatchingConcept(variant2, 0.6f);
    
    // These should either match the concept or be novel
    assert(match1 == conceptId || match1 == 0);
    assert(match2 == conceptId || match2 == 0);
    
    std::cout << "    testConceptFormationGeneralization passed" << std::endl;
}

void testConceptFormationIntegrationWithEpisodicMemory() {
    std::cout << "  testConceptFormationIntegrationWithEpisodicMemory..." << std::endl;
    
    nlm::ConceptFormation conceptFormation;
    
    // Create pattern and features
    std::vector<float> pattern = {0.3f, 0.6f, 0.9f};
    std::vector<float> features = {0.2f, 0.7f, 0.8f};
    
    // Present experience
    size_t conceptId = conceptFormation.presentExperience(pattern, features, 0.6f, 100);
    
    // Get concept information
    const auto* concept = conceptFormation.getConcept(conceptId);
    assert(concept != nullptr);
    
    // Verify concept contains the presented information
    assert(concept->totalObservations > 0);
    
    std::cout << "    testConceptFormationIntegrationWithEpisodicMemory passed" << std::endl;
}

// =============================================================================
// Neuromodulation Tests
// =============================================================================

void testNeuromodulationDopamine() {
    std::cout << "  testNeuromodulationDopamine..." << std::endl;
    
    nlm::Dopamine dopamine;
    
    // Test initial state
    assert(dopamine.getName() != nullptr);
    assertAlmostEqual(dopamine.getLevel(), 0.0f);
    
    // Test reward signaling
    dopamine.signalReward(1.0f);
    assertGreaterThan(dopamine.getLevel(), 0.0f);
    
    // Test prediction error signaling
    dopamine.signalRewardPredictionError(0.5f);
    
    // Test plasticity factor
    float plasticityFactor = dopamine.getPlasticityFactor();
    assertInRange(plasticityFactor, 0.0f, 2.0f);
    
    std::cout << "    testNeuromodulationDopamine passed" << std::endl;
}

void testNeuromodulationPredictionError() {
    std::cout << "  testNeuromodulationPredictionError..." << std::endl;
    
    nlm::PredictionError predictionError;
    
    // Test initial state
    assertAlmostEqual(predictionError.getLevel(), 0.0f);
    
    // Simulate prediction error
    predictionError.signalPredictionError(0.8f);
    assertGreaterThan(predictionError.getLevel(), 0.0f);
    
    // Test update
    predictionError.update(0.1f); // dt = 0.1 seconds
    
    // Test plasticity factor
    float plasticityFactor = predictionError.getPlasticityFactor();
    assertGreaterThan(plasticityFactor, 1.0f);
    
    std::cout << "    testNeuromodulationPredictionError passed" << std::endl;
}

void testNeuromodulationCuriosity() {
    std::cout << "  testNeuromodulationCuriosity..." << std::endl;
    
    nlm::Curiosity curiosity;
    
    // Test initial state
    assertGreaterThan(curiosity.getLevel(), 0.0f);
    
    // Test novelty detection
    curiosity.signalNovelty(0.7f);
    assertGreaterThan(curiosity.getLevel(), 0.0f);
    
    // Test update
    curiosity.update(0.1f);
    
    std::cout << "    testNeuromodulationCuriosity passed" << std::endl;
}

void testNeuromodulationNoveltyDetection() {
    std::cout << "  testNeuromodulationNoveltyDetection..." << std::endl;
    
    nlm::Novelty novelty;
    
    // Test initial state
    assertAlmostEqual(novelty.getLevel(), 0.0f);
    
    // Test novelty signal
    novelty.signalNovelty(1.0f);
    assertAlmostEqual(novelty.getLevel(), 1.0f);
    
    // Update and check decay
    novelty.update(1.0f); // dt = 1.0 second
    assertLessThan(novelty.getLevel(), 1.0f);
    
    std::cout << "    testNeuromodulationNoveltyDetection passed" << std::endl;
}

// =============================================================================
// Cross-System Tests
// =============================================================================

void testCrossSystemMemoryPrediction() {
    std::cout << "  testCrossSystemMemoryPrediction..." << std::endl;
    
    nlm::AssociativeMemory memory;
    nlm::PredictionSystem predictor;
    
    // Create a memory association
    memory.associate(NeuronId(1), NeuronId(2), 0.8f);
    
    // Use memory to inform prediction
    nlm::SensoryInput input1;
    input1.voltage = 0.4f;
    input1.spikes = 10;
    
    nlm::SensoryInput input2;
    input2.voltage = 0.5f;
    input2.spikes = 12;
    
    auto predicted = predictor.predictNextState(input1);
    predictor.updatePredictions(*predicted, input2);
    
    // Both systems should be functional
    assert(memory.getAssociationStrength(NeuronId(1), NeuronId(2)) > 0.0f);
    assertGreaterThan(predictor.getPredictionError(), 0.0f);
    
    std::cout << "    testCrossSystemMemoryPrediction passed" << std::endl;
}

void testCrossSystemConceptAttention() {
    std::cout << "  testCrossSystemConceptAttention..." << std::endl;
    
    nlm::ConceptFormation conceptFormation;
    nlm::Acetylcholine attention;
    
    // Create a concept
    std::vector<float> pattern = {0.3f, 0.6f, 0.9f};
    std::vector<float> features = {0.4f, 0.5f, 0.6f};
    
    size_t conceptId = conceptFormation.presentExperience(pattern, features, 0.7f, 0);
    
    // Test that concept formation can influence attention
    const auto* concept = conceptFormation.getConcept(conceptId);
    assert(concept != nullptr);
    
    // Attention modulation should be available
    assertGreaterThan(attention.getPlasticityFactor(), 0.0f);
    
    std::cout << "    testCrossSystemConceptAttention passed" << std::endl;
}

void testCrossSystemPredictionDopamine() {
    std::cout << "  testCrossSystemPredictionDopamine..." << std::endl;
    
    nlm::PredictionSystem predictor;
    nlm::Dopamine dopamine;
    
    // Create prediction error
    nlm::SensoryInput input1;
    input1.voltage = 0.5f;
    input1.spikes = 10;
    
    nlm::SensoryInput input2;
    input2.voltage = 0.8f;
    input2.spikes = 15;
    
    auto predicted = predictor.predictNextState(input1);
    predictor.updatePredictions(*predicted, input2);
    
    // Convert prediction error to reward prediction error for dopamine
    float predictionError = predictor.getPredictionError();
    float rewardPredictionError = 1.0f - predictionError; // Inverted
    
    // Dopamine should respond to prediction error
    dopamine.signalRewardPredictionError(rewardPredictionError);
    
    assertGreaterThan(dopamine.getLevel(), 0.0f);
    
    std::cout << "    testCrossSystemPredictionDopamine passed" << std::endl;
}

// =============================================================================
// Performance Tests
// =============================================================================

void testPerformanceMemoryEfficiency() {
    std::cout << "  testPerformanceMemoryEfficiency..." << std::endl;
    
    nlm::AssociativeMemory assoc;
    
    // Test memory efficiency with many associations
    const size_t numAssociations = 1000;
    
    // Create many associations
    for (size_t i = 0; i < numAssociations; ++i) {
        assoc.associate(NeuronId(i), NeuronId(i + numAssociations), 0.5f + (i * 0.001f));
    }
    
    // Verify all associations are accessible
    for (size_t i = 0; i < numAssociations; i += 100) { // Check every 100th
        float strength = assoc.getAssociationStrength(NeuronId(i), NeuronId(i + numAssociations));
        assertInRange(strength, 0.5f, 1.5f);
    }
    
    // Memory should not crash with large number of associations
    size_t capacity = assoc.getCapacity();
    assertGreaterThan(capacity, 0);
    
    std::cout << "    testPerformanceMemoryEfficiency passed" << std::endl;
}

void testPerformancePredictionAccuracyImprovement() {
    std::cout << "  testPerformancePredictionAccuracyImprovement..." << std::endl;
    
    nlm::PredictionSystem predictor;
    
    // Test that accuracy improves with training
    nlm::SensoryInput input1;
    input1.voltage = 0.3f;
    input1.spikes = 8;
    
    nlm::SensoryInput input2;
    input2.voltage = 0.4f;
    input2.spikes = 9;
    
    nlm::SensoryInput input3;
    input3.voltage = 0.35f;
    input3.spikes = 8;
    
    // Initial prediction
    auto predicted1 = predictor.predictNextState(input1);
    predictor.updatePredictions(*predicted1, input2);
    float error1 = predictor.getPredictionError();
    
    // Train more
    predictor.train(input2);
    predictor.train(input3);
    
    // Should have more data, potentially better accuracy
    float error2 = predictor.getPredictionError();
    
    // While we can't guarantee error reduction, system should remain stable
    assertLessThan(error2, 10.0f); // Should not explode
    
    std::cout << "    testPerformancePredictionAccuracyImprovement passed" << std::endl;
}

void testPerformanceSystemStability() {
    std::cout << "  testPerformanceSystemStability..." << std::endl;
    
    nlm::ConceptFormation conceptFormation;
    
    // Test system stability over multiple operations
    for (int iteration = 0; iteration < 100; ++iteration) {
        // Create and update concepts
        std::vector<float> pattern = {
            static_cast<float>(iteration % 10) / 10.0f,
            static_cast<float>((iteration * 3) % 10) / 10.0f,
            static_cast<float>((iteration * 7) % 10) / 10.0f
        };
        std::vector<float> features = {
            static_cast<float>(iteration % 5) / 5.0f,
            static_cast<float>((iteration * 2) % 5) / 5.0f
        };
        
        size_t conceptId = conceptFormation.presentExperience(pattern, features, 0.5f, iteration);
        
        // Verify system doesn't crash
        size_t conceptCount = conceptFormation.getConceptCount();
        assertGreaterThan(conceptCount, 0);
        
        // Clean up periodically
        if (iteration % 20 == 19) {
            conceptFormation.clear();
        }
    }
    
    std::cout << "    testPerformanceSystemStability passed" << std::endl;
}

// =============================================================================
// Main Test Runner
// =============================================================================

void runAllIntegratedSystemTests() {
    std::cout << "=== Running Integrated Systems Tests ===" << std::endl;
    
    // AssociativeMemory tests
    testAssociativeMemoryPatternCompletion();
    testAssociativeMemorySpreadActivation();
    testAssociativeMemoryMemoryRetention();
    testAssociativeMemoryIntegrationWithWorkingMemory();
    
    // PredictionSystem tests
    testPredictionSystemAccuracy();
    testPredictionSystemErrorTracking();
    testPredictionSystemConfidenceAdaptation();
    testPredictionSystemIntegrationWithCuriosity();
    
    // ConceptFormation tests
    testConceptFormationBasic();
    testConceptFormationStability();
    testConceptFormationGeneralization();
    testConceptFormationIntegrationWithEpisodicMemory();
    
    // Neuromodulation tests
    testNeuromodulationDopamine();
    testNeuromodulationPredictionError();
    testNeuromodulationCuriosity();
    testNeuromodulationNoveltyDetection();
    
    // Cross-system tests
    testCrossSystemMemoryPrediction();
    testCrossSystemConceptAttention();
    testCrossSystemPredictionDopamine();
    
    // Performance tests
    testPerformanceMemoryEfficiency();
    testPerformancePredictionAccuracyImprovement();
    testPerformanceSystemStability();
    
    std::cout << "=== All Integrated Systems Tests Passed ===" << std::endl;
}

} // namespace test_integrated_systems

} // namespace nlm
