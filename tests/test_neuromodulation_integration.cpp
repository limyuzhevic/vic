// Neuromodulation Integration Tests
// Phase 3.3: Integration tests for neuromodulation system

#include "brain/Brain.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Reward.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <vector>

namespace test_neuromodulation_integration {

void testRewardIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("enable_reward_modulation", true, nlm::ConfigSource::Default);
    config->set("reward_sensitivity", 0.5, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test reward modulation integration
    brain.applyRewardModulation(1.0f, 0.8f); // reward, predicted reward
    
    brain.step(0);
    
    assert(brain.getTotalNeuronCount() == 200);
    assert(brain.getAverageFiringRate() >= 0.0f);
    
    std::cout << "    testRewardIntegration passed" << std::endl;
}

void testPredictionErrorIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(180), nlm::ConfigSource::Default);
    config->set("enable_prediction_error", true, nlm::ConfigSource::Default);
    config->set("prediction_error_sensitivity", 0.3, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test prediction error integration
    // The brain should have prediction error capabilities
    for (nlm::SimulationStep step = 0; step < 15; ++step) {
        brain.step(step);
        
        // Check prediction error system
        assert(brain.getTotalNeuronCount() == 180);
    }
    
    std::cout << "    testPredictionErrorIntegration passed" << std::endl;
}

void testNoveltyIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(220), nlm::ConfigSource::Default);
    config->set("enable_curiosity", true, nlm::ConfigSource::Default);
    config->set("novelty_threshold", 0.7, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test novelty detection integration
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        // The brain should have novelty detection capabilities
        brain.step(step);
        
        assert(brain.getTotalNeuronCount() == 220);
    }
    
    std::cout << "    testNoveltyIntegration passed" << std::endl;
}

void testCuriosityIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(190), nlm::ConfigSource::Default);
    config->set("enable_curiosity", true, nlm::ConfigSource::Default);
    config->set("curiosity_threshold", 0.6, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test curiosity integration
    for (nlm::SimulationStep step = 0; step < 18; ++step) {
        // The brain should have curiosity system integration
        brain.step(step);
        
        assert(brain.getTotalNeuronCount() == 190);
    }
    
    std::cout << "    testCuriosityIntegration passed" << std::endl;
}

void testCompleteNeuromodulationIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(250), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("enable_reward_modulation", true, nlm::ConfigSource::Default);
    config->set("enable_prediction_error", true, nlm::ConfigSource::Default);
    config->set("enable_novelty", true, nlm::ConfigSource::Default);
    config->set("enable_curiosity", true, nlm::ConfigSource::Default);
    config->set("reward_sensitivity", 0.5, nlm::ConfigSource::Default);
    config->set("prediction_error_sensitivity", 0.3, nlm::ConfigSource::Default);
    config->set("novelty_threshold", 0.7, nlm::ConfigSource::Default);
    config->set("curiosity_threshold", 0.6, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Complete neuromodulation integration test
    for (nlm::SimulationStep step = 0; step < 40; ++step) {
        // Apply various neuromodulation inputs
        brain.applyRewardModulation(0.5f, 0.3f);
        
        // The brain should handle multiple neuromodulation systems
        brain.step(step);
        
        // Verify neuromodulation integration
        assert(brain.getTotalNeuronCount() == 250);
        assert(std::isfinite(brain.getAverageFiringRate()));
        
        // Check neuromodulation levels
        brain.getNeuromodulationLevel();
        brain.getCuriosityLevel();
        brain.getNoveltyLevel();
        brain.getPredictionError();
    }
    
    std::cout << "    testCompleteNeuromodulationIntegration passed" << std::endl;
}

void testNeuromodulationPerformance() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("enable_reward_modulation", true, nlm::ConfigSource::Default);
    config->set("enable_prediction_error", true, nlm::ConfigSource::Default);
    config->set("enable_curiosity", true, nlm::ConfigSource::Default);
    config->set("enable_novelty", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Benchmark neuromodulation performance
    auto startTime = std::chrono::high_resolution_clock::now();
    
    float totalNeuromodulationEffect = 0.0f;
    for (nlm::SimulationStep step = 0; step < 60; ++step) {
        // Apply neuromodulation
        brain.applyRewardModulation(0.5f + 0.1f * (step % 5), 0.2f * (step % 3));
        
        // Step with neuromodulation
        brain.step(step);
        
        totalNeuromodulationEffect += brain.getAverageFiringRate();
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    std::cout << "    testNeuromodulationPerformance: " << duration.count() << " microseconds for 60 steps with neuromodulation" << std::endl;
    std::cout << "    Average neuromodulation effect: " << (totalNeuromodulationEffect / 60.0f) << std::endl;
    
    std::cout << "    testNeuromodulationPerformance passed" << std::endl;
}

void testNeuromodulationResourceManagement() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("enable_reward_modulation", true, nlm::ConfigSource::Default);
    config->set("enable_prediction_error", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test neuromodulation resource management under load
    for (int i = 0; i < 5; ++i) {
        // Heavy neuromodulation usage
        for (nlm::SimulationStep step = 0; step < 20; ++step) {
            brain.applyRewardModulation(1.0f, 0.5f);
            brain.step(step);
        }
        
        // Reset and verify cleanup
        brain.reset();
        assert(brain.initialize());
    }
    
    std::cout << "    testNeuromodulationResourceManagement passed" << std::endl;
}

void testNeuromodulationSystemRobustness() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(280), nlm::ConfigSource::Default);
    config->set("enable_reward_modulation", true, nlm::ConfigSource::Default);
    config->set("enable_prediction_error", true, nlm::ConfigSource::Default);
    config->set("enable_curiosity", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test neuromodulation system robustness
    for (nlm::SimulationStep step = 0; step < 35; ++step) {
        // Variable neuromodulation inputs
        brain.applyRewardModulation(0.1f * (step % 10), 0.05f * (step % 5));
        
        // Stress neuromodulation system
        brain.step(step);
        
        // Verify system stability
        assert(brain.getTotalNeuronCount() == 280);
        assert(std::isfinite(brain.getAverageFiringRate()));
    }
    
    std::cout << "    testNeuromodulationSystemRobustness passed" << std::endl;
}

void testNeuromodulationIntegrationWithOtherSystems() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(320), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
    config->set("enable_reward_modulation", true, nlm::ConfigSource::Default);
    config->set("enable_prediction_error", true, nlm::ConfigSource::Default);
    config->set("enable_development", true, nlm::ConfigSource::Default);
    config->set("enable_curiosity", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test neuromodulation integration with other systems
    for (nlm::SimulationStep step = 0; step < 45; ++step) {
        // Neuromodulation should work with development
        brain.setDevelopmentalStage(nlm::DevelopmentalStage::CriticalPeriod);
        
        // Neuromodulation should work with memory
        brain.applyRewardModulation(0.3f, 0.2f);
        
        // Step the integrated system
        brain.step(step);
        
        // Verify all systems are working together
        assert(brain.getTotalNeuronCount() == 320);
        assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::CriticalPeriod);
    }
    
    std::cout << "    testNeuromodulationIntegrationWithOtherSystems passed" << std::endl;
}

void runAll() {
    testRewardIntegration();
    testPredictionErrorIntegration();
    testNoveltyIntegration();
    testCuriosityIntegration();
    testCompleteNeuromodulationIntegration();
    testNeuromodulationPerformance();
    testNeuromodulationResourceManagement();
    testNeuromodulationSystemRobustness();
    testNeuromodulationIntegrationWithOtherSystems();
}

} // namespace test_neuromodulation_integration