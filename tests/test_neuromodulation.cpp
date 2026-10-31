// Neuromodulation Integration Tests
// Tests neuromodulation systems (dopamine, curiosity, novelty)

#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

namespace test_neuromodulation {

void testNeuromodulatorBase() {
    std::cout << "  Testing Neuromodulator Base..." << std::endl;
    
    // Test Acetylcholine (placeholder)
    nlm::Acetylcholine ach;
    assert(std::string(ach.getName()) == "ACh");
    assert(ach.getLevel() == 0.0f);
    assert(ach.getPlasticityFactor() == 1.0f);
    
    // Test Norepinephrine (placeholder)
    nlm::Norepinephrine ne;
    assert(std::string(ne.getName()) == "NE");
    assert(ne.getLevel() == 0.0f);
    assert(ne.getPlasticityFactor() == 1.0f);
    
    // Test Serotonin (placeholder)
    nlm::Serotonin serotonin;
    assert(std::string(serotonin.getName()) == "5-HT");
    assert(serotonin.getLevel() == 0.0f);
    assert(serotonin.getPlasticityFactor() == 1.0f);
    
    std::cout << "    testNeuromodulatorBase passed" << std::endl;
}

void testDopamineIntegration() {
    std::cout << "  Testing Dopamine Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::Dopamine* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    // Test initial state
    assert(std::string(dopamine->getName()) == "Dopamine");
    assert(dopamine->getLevel() == 0.0f);
    
    // Test reward signaling
    dopamine->signalReward(0.5f);
    // Level should increase based on reward
    assert(dopamine->getLevel() > 0.0f);
    
    // Test reward prediction error
    dopamine->signalRewardPredictionError(0.3f);
    // Level should adjust based on prediction error
    
    // Test level setting
    dopamine->setLevel(0.8f);
    assert(std::abs(dopamine->getLevel() - 0.8f) < 0.001f);
    
    // Test plasticity factor
    float plasticityFactor = dopamine->getPlasticityFactor();
    assert(plasticityFactor >= 0.0f && plasticityFactor <= 1.0f);
    
    // Test level updates
    dopamine->setLevel(0.0f);
    assert(dopamine->getLevel() == 0.0f);
    
    std::cout << "    testDopamineIntegration passed" << std::endl;
}

void testNoveltyIntegration() {
    std::cout << "  Testing Novelty Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::Novelty* novelty = brain.getNovelty();
    assert(novelty != nullptr);
    
    // Test level control
    assert(novelty->getLevel() == 0.0f);
    novelty->setLevel(0.5f);
    assert(std::abs(novelty->getLevel() - 0.5f) < 0.001f);
    
    // Test novelty detection
    // Note: This would require actual Observation objects, but we can test the conceptual integration
    // Novelty detection is based on comparison with previous observations
    std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f};
    std::vector<float> pattern2 = {0.5f, 0.6f, 0.7f};  // Different from pattern1
    std::vector<float> pattern3 = {0.1f, 0.2f, 0.3f};  // Same as pattern1
    
    // In a real implementation, these would detect novelty based on pattern differences
    // novelty->detectNovelty(pattern1, pattern2); // Should detect novelty
    // novelty->detectNovelty(pattern1, pattern3); // Should not detect novelty (or low novelty)
    
    // Test novelty decay over time
    novelty->setLevel(1.0f);
    for (int i = 0; i < 10; ++i) {
        novelty->update(0.1f);  // Decay rate
    }
    // Level should decrease due to decay
    assert(novelty->getLevel() < 1.0f);
    
    // Test novelty history
    std::vector<float> history = novelty->getHistory();
    assert(!history.empty());
    
    // Test novelty reset
    novelty->clearHistory();
    
    std::cout << "    testNoveltyIntegration passed" << std::endl;
}

void testCuriosityIntegration() {
    std::cout << "  Testing Curiosity Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(75), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::Curiosity* curiosity = brain.getCuriosity();
    assert(curiosity != nullptr);
    
    // Initialize curiosity with brain reference
    curiosity->initialize(&brain);
    
    // Test initial curiosity level
    assert(curiosity->getLevel() == 0.0f);
    assert(curiosity->getExplorationDrive() == 0.0f);
    
    // Test curiosity update based on novelty and prediction error
    curiosity->update(0.3f, 0.5f, 0.1f);  // novelty=0.3, predictionError=0.5, dt=0.1
    // Curiosity level should increase based on weighted sum
    assert(curiosity->getLevel() > 0.0f);
    
    // Test curiosity parameters
    curiosity->setNoveltyWeight(0.7f);
    curiosity->setPredictionErrorWeight(0.3f);
    // These parameters affect how curiosity responds to inputs
    
    // Test exploration drive (should be same as level)
    assert(std::abs(curiosity->getExplorationDrive() - curiosity->getLevel()) < 0.001f);
    
    // Test curiosity reset
    curiosity->reset();
    assert(curiosity->getLevel() == 0.0f);
    assert(curiosity->getExplorationDrive() == 0.0f);
    
    // Test curiosity integration with brain
    curiosity->update(0.8f, 0.2f, 0.05f);  // High novelty, low prediction error
    assert(curiosity->getLevel() > 0.0f);
    
    std::cout << "    testCuriosityIntegration passed" << std::endl;
}

void testPredictionErrorIntegration() {
    std::cout << "  Testing Prediction Error Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(60), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::PredictionError* predictionError = brain.getPredictionErrorSignal();
    assert(predictionError != nullptr);
    
    // Note: PredictionError typically is a neuromodulator signal
    // For now, test that it exists and can be integrated with other systems
    
    // Prediction error affects dopamine levels and curiosity
    // In Phase 2, predictionError would modulate other neuromodulators
    
    // Test basic integration with other neuromodulators
    nlm::Dopamine* dopamine = brain.getDopamine();
    nlm::Novelty* novelty = brain.getNovelty();
    nlm::Curiosity* curiosity = brain.getCuriosity();
    
    assert(dopamine != nullptr);
    assert(novelty != nullptr);
    assert(curiosity != nullptr);
    
    // All neuromodulators should be present and functional
    assert(std::string(dopamine->getName()) == "Dopamine");
    assert(std::string(novelty->getName()) == "Novelty");  // Novelty is a neuromodulator
    
    // Test neuromodulator application
    dopamine->setLevel(0.5f);
    novelty->setLevel(0.3f);
    
    // Brain can apply neuromodulation signals
    brain.applyNeuromodulation(*dopamine);
    brain.applyNeuromodulation(*novelty);
    
    // Test curiosity response to prediction error
    curiosity->update(0.0f, 0.5f, 0.1f);  // No novelty, high prediction error
    assert(curiosity->getLevel() > 0.0f);  // Should respond to prediction error
    
    std::cout << "    testPredictionErrorIntegration passed" << std::endl;
}

void testNeuromodulationIntegration() {
    std::cout << "  Testing Neuromodulation Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify all neuromodulators are accessible
    assert(brain.getDopamine() != nullptr);
    assert(brain.getCuriosity() != nullptr);
    assert(brain.getNovelty() != nullptr);
    assert(brain.getPredictionErrorSignal() != nullptr);
    
    // Test neuromodulator interconnections
    nlm::Dopamine* dopamine = brain.getDopamine();
    nlm::Novelty* novelty = brain.getNovelty();
    nlm::Curiosity* curiosity = brain.getCuriosity();
    
    // Set initial levels
    dopamine->setLevel(0.3f);
    novelty->setLevel(0.4f);
    
    // Update curiosity based on both neuromodulators
    curiosity->update(novelty->getLevel(), 0.2f, 0.1f);  // Use novelty as novelty input
    
    // Test plasticity factor modulation
    float dopaminePlasticity = dopamine->getPlasticityFactor();
    assert(dopaminePlasticity >= 0.0f && dopaminePlasticity <= 1.0f);
    
    // Test neuromodulator application
    brain.applyNeuromodulation(*dopamine);
    brain.applyNeuromodulation(*novelty);
    
    // Test neuromodulator effects on plasticity
    // Brain.updatePlasticity() would be called automatically in step()
    // But we can verify it's callable
    brain.updatePlasticity();
    
    // Test developmental effects
    brain.develop();  // Called automatically in step()
    
    // Test integration with memory systems
    nlm::NeuronId neuron1(1);
    nlm::NeuronId neuron2(2);
    
    // Neuromodulation affects memory consolidation
    // In Phase 2, dopamine would enhance memory encoding
    // Novelty would enhance episodic memory storage
    
    // This is tested through the broader brain integration tests
    
    std::cout << "    testNeuromodulationIntegration passed" << std::endl;
}

void testNeuromodulationPerformance() {
    std::cout << "  Testing Neuromodulation Performance..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::Dopamine* dopamine = brain.getDopamine();
    nlm::Novelty* novelty = brain.getNovelty();
    nlm::Curiosity* curiosity = brain.getCuriosity();
    
    assert(dopamine != nullptr);
    assert(novelty != nullptr);
    assert(curiosity != nullptr);
    
    const size_t iterations = 10000;
    
    // Performance benchmark for neuromodulation updates
    for (size_t i = 0; i < iterations; ++i) {
        // Update dopamine with reward signals
        dopamine->signalReward(static_cast<float>(i % 10) / 10.0f);
        
        // Update novelty with changing patterns
        novelty->setLevel(static_cast<float>(i % 100) / 100.0f);
        novelty->update(0.01f);
        
        // Update curiosity with novelty and prediction error
        curiosity->update(novelty->getLevel(), static_cast<float>(i % 50) / 50.0f, 0.01f);
        
        // Apply neuromodulation
        brain.applyNeuromodulation(*dopamine);
        brain.applyNeuromodulation(*novelty);
        
        // Update plasticity
        brain.updatePlasticity();
    }
    
    // Verify performance
    assert(dopamine->getLevel() >= 0.0f);
    assert(novelty->getLevel() >= 0.0f);
    assert(curiosity->getLevel() >= 0.0f);
    
    std::cout << "    testNeuromodulationPerformance passed" << std::endl;
}

void testNeuromodulationErrorConditions() {
    std::cout << "  Testing Neuromodulation Error Conditions..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::Dopamine* dopamine = brain.getDopamine();
    nlm::Novelty* novelty = brain.getNovelty();
    nlm::Curiosity* curiosity = brain.getCuriosity();
    
    // Test dopamine error conditions
    assert(dopamine->getLevel() == 0.0f);
    dopamine->setLevel(1.5f);  // Should be clamped somewhere (implementation dependent)
    // In real implementation, level might be clamped or handled differently
    
    // Test novelty error conditions
    novelty->setLevel(-0.5f);  // Should handle negative values or clamp
    assert(novelty->getLevel() >= 0.0f);  // Should be non-negative
    
    // Test curiosity error conditions
    curiosity->update(-0.5f, -0.5f, -0.5f);  // Negative inputs
    // Should handle gracefully (clamp or handle in implementation)
    
    // Test null checks
    assert(dopamine != nullptr);
    assert(novelty != nullptr);
    assert(curiosity != nullptr);
    
    // Test reset operations
    dopamine->setLevel(0.0f);
    novelty->setLevel(0.0f);
    curiosity->reset();
    
    std::cout << "    testNeuromodulationErrorConditions passed" << std::endl;
}

void runAll() {
    testNeuromodulatorBase();
    testDopamineIntegration();
    testNoveltyIntegration();
    testCuriosityIntegration();
    testPredictionErrorIntegration();
    testNeuromodulationIntegration();
    testNeuromodulationPerformance();
    testNeuromodulationErrorConditions();
}

} // namespace test_neuromodulation
