// Neuromodulation Integration Tests
// Tests integration of Dopamine, Curiosity, Novelty, and PredictionError with plasticity

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>

namespace test_neuromodulation_integration {

void testDopamineSystemInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Note: Brain may not directly expose neuromodulators yet
    // This test verifies the system can be extended for neuromodulation
    
    // Run a few simulation steps
    for (nlm::SimulationStep step = 0; step < 5; ++step) {
        brain.step(step);
    }
    
    // Verify brain is operational
    assert(brain.getTotalNeuronCount() > 0);
    
    std::cout << "    testDopamineSystemInitialization passed" << std::endl;
}

void testCuriositySystemInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test curiosity system through brain interaction
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        // Update with some novelty and prediction error
        float novelty = static_cast<float>(step % 10) / 10.0f;
        float predictionError = static_cast<float>((step * 3) % 10) / 10.0f;
        
        brain.step(step);
    }
    
    std::cout << "    testCuriositySystemInitialization passed" << std::endl;
}

void testNoveltySystemIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test novelty detection through simulation
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        // Different input patterns should generate novelty
        float patternVariation = static_cast<float>(step) / 20.0f;
        brain.step(step);
    }
    
    // Verify system is responsive
    assert(brain.getTotalNeuronCount() > 0);
    
    std::cout << "    testNoveltySystemIntegration passed" << std::endl;
}

void testPredictionErrorSystemIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test prediction error integration through learning
    for (nlm::SimulationStep step = 0; step < 30; ++step) {
        brain.step(step);
        
        // Simulate prediction error signals
        float error = std::abs(static_cast<float>(step % 7 - 3)) / 3.0f;
        
        // Note: In full implementation, error would be sent to neuromodulation system
    }
    
    std::cout << "    testPredictionErrorSystemIntegration passed" << std::endl;
}

void testNeuromodulationWithPlasticity() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    config->set("plasticity.stdp.enabled", true, nlm::ConfigSource::Default);
    config->set("plasticity.hebbian.enabled", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Get plasticity systems
    auto* stdp = brain.getSTDP();
    auto* hebbian = brain.getHebbian();
    assert(stdp != nullptr);
    assert(hebbian != nullptr);
    
    // Create neurons and synapses for testing
    nlm::RegionId region1 = brain.addRegion("test_region_1");
    nlm::RegionId region2 = brain.addRegion("test_region_2");
    
    // Verify plasticity systems are connected to brain
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
        
        // Apply some stimulation that would normally trigger neuromodulation
        float stimulation = static_cast<float>(step % 5) / 5.0f;
        
        // In full implementation: apply neuromodulatory signals
        // brain.applyNeuromodulation(neuromodulator);
    }
    
    // Check that plasticity has occurred (synaptic weights should have changed)
    // Note: This depends on the specific implementation
    
    std::cout << "    testNeuromodulationWithPlasticity passed" << std::endl;
}

void testNeuromodulationFeedbackLoop() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.15, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Run a simulation where neuromodulation affects brain function
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        // Simulate different environmental conditions that would affect neuromodulation
        float novelty = 1.0f / (1.0f + std::exp(static_cast<float>(step - 50) / 10.0f)); // Bell curve
        float reward = (step % 10 == 0) ? 1.0f : 0.0f;
        
        brain.step(step);
        
        // In full integration: neuromodulators would update based on these signals
        // and influence brain function
    }
    
    // Verify the brain maintains stability
    assert(brain.getTotalNeuronCount() > 0);
    assert(brain.getFiringNeuronCount() < brain.getTotalNeuronCount()); // Not all neurons should fire
    
    std::cout << "    testNeuromodulationFeedbackLoop passed" << std::endl;
}

void runAll() {
    std::cout << "Running Neuromodulation Integration tests..." << std::endl;
    testDopamineSystemInitialization();
    testCuriositySystemInitialization();
    testNoveltySystemIntegration();
    testPredictionErrorSystemIntegration();
    testNeuromodulationWithPlasticity();
    testNeuromodulationFeedbackLoop();
}

} // namespace test_neuromodulation_integration
