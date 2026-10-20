// Prediction System and Concept Formation Tests
#include "brain/Brain.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/ConceptFormation.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>

namespace test_prediction_and_concept {

void testPredictionSystemInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // Get prediction system
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Test prediction generation
    auto& sensoryInput = *new nlm::Vision(16, 16, 3);
    auto prediction = predictionSystem->predictNextState(sensoryInput);
    assert(prediction != nullptr);
    
    // Test prediction error computation
    predictionSystem->updatePredictions(*prediction, sensoryInput);
    float error = predictionSystem->getPredictionError();
    float confidence = predictionSystem->getConfidence();
    
    assert(error >= 0.0f);
    assert(confidence >= 0.1f && confidence <= 1.0f);
    
    std::cout << "    testPredictionSystemInitialization passed" << std::endl;
}

void testConceptFormationInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // Create concept formation system
    nlm::ConceptFormation conceptFormation;
    conceptFormation.initialize(&brain);
    
    // Test concept creation
    std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> features1 = {0.15f, 0.25f, 0.35f};
    
    size_t conceptId1 = conceptFormation.presentExperience(pattern1, features1, 0.5f, 0);
    assert(conceptId1 > 0);  // Should create new concept
    
    // Test concept update with similar pattern
    std::vector<float> pattern2 = {0.11f, 0.19f, 0.32f, 0.41f, 0.49f};  // Similar to pattern1
    std::vector<float> features2 = {0.14f, 0.26f, 0.36f};
    
    size_t conceptId2 = conceptFormation.presentExperience(pattern2, features2, 0.6f, 1);
    assert(conceptId2 == conceptId1);  // Should belong to same concept
    
    // Test concept similarity computation
    float similarity = conceptFormation.computeSimilarity(pattern1, pattern2);
    assert(similarity >= 0.0f && similarity <= 1.0f);
    
    // Test concept stability
    float stability = conceptFormation.getConceptStability(conceptId1);
    assert(stability >= 0.0f && stability <= 1.0f);
    
    std::cout << "    testConceptFormationInitialization passed" << std::endl;
}

void testNeuralWorkingMemoryFix() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // Get working memory
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Test storeToNeuron method (this is where the bug was)
    nlm::NeuronId testNeuronId(1000);
    
    // This should not crash (the bug fix)
    workingMemory->storeToNeuron(testNeuronId, 0.5f);
    
    // Test contains method
    bool contains = workingMemory->contains(testNeuronId);
    assert(contains == true);
    
    // Test update method
    workingMemory->update(0.01f);  // Should not crash
    
    std::cout << "    testNeuralWorkingMemoryFix passed" << std::endl;
}

void testIntegratedPredictionConceptFlow() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // Test the integrated flow: Working Memory -> Prediction System -> Concept Formation
    auto* workingMemory = brain.getWorkingMemory();
    auto* predictionSystem = brain.getPredictionSystem();
    assert(workingMemory != nullptr && predictionSystem != nullptr);
    
    // Simulate some neural activity
    for (nlm::SimulationStep step = 0; step < 5; ++step) {
        brain.step(step);
        
        // Test prediction system integration (STEP 8)
        if (predictionSystem) {
            float error = predictionSystem->getPredictionError();
            float confidence = predictionSystem->getConfidence();
            assert(error >= 0.0f);
            assert(confidence >= 0.1f);
        }
    }
    
    std::cout << "    testIntegratedPredictionConceptFlow passed" << std::endl;
}

void runAllTests() {
    std::cout << "=== Prediction System and Concept Formation Tests ===" << std::endl;
    
    testPredictionSystemInitialization();
    testConceptFormationInitialization();
    testNeuralWorkingMemoryFix();
    testIntegratedPredictionConceptFlow();
    
    std::cout << "=== All Tests Passed ===" << std::endl;
}

} // namespace test_prediction_and_concept

// Integration with existing test framework
void test_additional_improvements() {
    test_prediction_and_concept::runAllTests();
}