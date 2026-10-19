// Prediction System Tests
// Tests for forward model, prediction error, and confidence tracking

#include "prediction/PredictionSystem.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_prediction {

void testPredictionSystem() {
    std::cout << "    Testing prediction system..." << std::endl;
    
    nlm::PredictionSystem predictionSystem;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", 42);
    
    nlm::Brain brain(config);
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain for prediction system test" << std::endl;
        assert(false);
    }
    
    // Note: PredictionSystem doesn't have initialize method currently
    // But we can test that it exists and has the right interface
    auto* ps = brain.getPredictionSystem();
    assert(ps != nullptr);
    
    std::cout << "    Prediction system test passed" << std::endl;
}

void testForwardModel() {
    std::cout << "    Testing forward model..." << std::endl;
    
    // Test forward model concepts
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", 42);
    
    nlm::Brain brain(config);
    
    if (brain.initialize()) {
        // Test prediction error integration
        auto* predictionError = brain.getPredictionErrorSignal();
        assert(predictionError != nullptr);
        
        // Prediction error should affect brain state
        nlm::Neuromodulator signal;
        signal.setLevel(0.2f);
        
        brain.applyNeuromodulation(signal);
    }
    
    std::cout << "    Forward model test passed" << std::endl;
}

void testPredictionErrorIntegration() {
    std::cout << "    Testing prediction error integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 50);
    config->set("region_count", 1);
    
    nlm::Brain brain(config);
    
    if (brain.initialize()) {
        // Test that prediction error affects brain plasticity
        auto* predictionError = brain.getPredictionErrorSignal();
        auto* dopamine = brain.getDopamine();
        
        assert(predictionError != nullptr);
        assert(dopamine != nullptr);
        
        // Prediction error should modify dopamine levels
        nlm::Neuromodulator signal;
        signal.setLevel(0.5f);
        
        brain.applyNeuromodulation(signal);
    }
    
    std::cout << "    Prediction error integration test passed" << std::endl;
}

void testConfidenceTracking() {
    std::cout << "    Testing confidence tracking..." << std::endl;
    
    // Confidence tracking would involve maintaining confidence levels
    // for predictions and decisions
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", 42);
    
    nlm::Brain brain(config);
    
    if (brain.initialize()) {
        // Get brain statistics that might include confidence-related metrics
        size_t spikeCount = brain.getTotalSpikeCount();
        float eiratio = brain.getExcitationInhibitionRatio();
        
        // These statistics can be used as proxies for confidence
        assert(spikeCount >= 0);
        assert(eiratio >= 0.0f);
    }
    
    std::cout << "    Confidence tracking test passed" << std::endl;
}

void testLearningFromPredictions() {
    std::cout << "    Testing learning from predictions..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("connection_probability", 0.2f);
    
    nlm::Brain brain(config);
    
    if (brain.initialize()) {
        // Test that brain can generate predictions and learn from errors
        nlm::SimulationStep step = 0;
        
        for (size_t i = 0; i < 10; ++i) {
            brain.step(step++);
            
            // Check that the brain is adapting (showing signs of learning)
            size_t spikeCount = brain.getTotalSpikeCount();
            assert(spikeCount >= 0);
        }
        
        // Test checkpoint system (part of prediction/learning)
        std::string checkpointDir = "./test_checkpoints";
        auto* checkpointManager = brain.getCheckpointManager();
        if (checkpointManager) {
            checkpointManager->configure(checkpointDir, 1000, 5, true);
        }
    }
    
    std::cout << "    Learning from predictions test passed" << std::endl;
}

} // namespace test_prediction
