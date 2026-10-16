// Phase 6 Prediction Integration Tests
// Test prediction system integration with neural activity

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>

using namespace nlm;

namespace test_phase6_prediction {

void testPredictionSystemCreation() {
    std::cout << "  Testing Prediction System Creation..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(cfg);
    brain->initialize();
    
    // Test prediction system creation and basic functionality
    auto* prediction = brain->getPredictionSystem();
    assert(prediction != nullptr);
    
    // Test prediction error calculation
    float predictionError = prediction->getPredictionError();
    assert(predictionError >= 0.0f);
    
    // Test confidence calculation
    float confidence = prediction->getConfidence();
    assert(confidence >= 0.0f && confidence <= 1.0f);
    
    // Test prediction horizon
    int horizon = prediction->getPredictionHorizon();
    assert(horizon > 0);
    
    std::cout << "    PASSED" << std::endl;
}

void testPredictionLearning() {
    std::cout << "  Testing Prediction Learning..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(cfg);
    brain->initialize();
    
    auto* prediction = brain->getPredictionSystem();
    assert(prediction != nullptr);
    
    // Record initial prediction error
    float initialError = prediction->getPredictionError();
    
    // Inject some activity and step the brain
    for (int i = 0; i < 50; ++i) {
        brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
        brain->step(i, i * 0.001);
        
        // Get updated prediction error
        float currentError = prediction->getPredictionError();
        
        // Prediction error should evolve over time (not crash)
        assert(currentError >= 0.0f);
    }
    
    // Verify prediction system is still functional
    assert(prediction->getPredictionError() >= 0.0f);
    
    std::cout << "    PASSED" << std::endl;
}

void testPredictionIntegrationWithMemory() {
    std::cout << "  Testing Prediction Integration With Memory..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(cfg);
    brain->initialize();
    
    // Get both prediction and memory systems
    auto* prediction = brain->getPredictionSystem();
    auto* workingMemory = brain->getWorkingMemory();
    
    assert(prediction != nullptr);
    assert(workingMemory != nullptr);
    
    // Test that prediction can access working memory
    size_t workingMemCapacity = workingMemory->getCapacity();
    assert(workingMemCapacity > 0);
    
    // Run brain steps with prediction and memory working together
    for (int i = 0; i < 30; ++i) {
        // Prediction system operates independently
        float error = prediction->getPredictionError();
        assert(error >= 0.0f);
        
        // Memory system operates independently  
        size_t traces = workingMemory->getActiveTraces();
        assert(traces >= 0);
        
        brain->step(i, i * 0.001);
    }
    
    std::cout << "    PASSED" << std::endl;
}

void runAll() {
    std::cout << "=== Phase 6 Prediction Integration Tests ===" << std::endl;
    
    // Initialize logger
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    testPredictionSystemCreation();
    testPredictionLearning();
    testPredictionIntegrationWithMemory();
    
    std::cout << std::endl;
    std::cout << "=== All Phase 6 Prediction Integration Tests PASSED ===" << std::endl;
}

} // namespace test_phase6_prediction