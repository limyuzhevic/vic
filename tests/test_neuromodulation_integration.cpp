// Neuromodulation Integration Tests
// Tests neuromodulation system integration within the brain

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "neuromodulation/Novelty.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

namespace test_neuromodulation_integration {

// Test 1: Neuromodulator abstract base class interface
void testNeuromodulatorBase() {
    std::cout << "    Testing neuromodulator base class..." << std::endl;
    
    // Test concrete implementations
    nlm::Dopamine dopamine;
    assert(std::string(dopamine.getName()) == "Dopamine");
    assert(dopamine.getLevel() == 0.0f);
    assert(dopamine.getPlasticityFactor() >= 0.0f);
    
    // Test setting level
    dopamine.setLevel(0.7f);
    assert(dopamine.getLevel() == 0.7f);
    
    nlm::Curiosity curiosity;
    assert(std::string(curiosity.getName()) == "Curiosity");
    assert(curiosity.getLevel() == 0.0f);
    
    nlm::PredictionError predictionError;
    assert(std::string(predictionError.getName()) == "PredictionError");
    assert(predictionError.getLevel() == 0.0f);
    
    nlm::Novelty novelty;
    assert(std::string(novelty.getName()) == "Novelty");
    assert(novelty.getLevel() == 0.0f);
    
    std::cout << "      PASSED" << std::endl;
}

// Test 2: Dopamine system integration
void testDopamineSystem() {
    std::cout << "    Testing dopamine system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::Dopamine* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    // Test initial state
    assert(dopamine->getLevel() == 0.0f);
    
    // Test reward signaling
    dopamine->signalReward(1.0f);  // Should increase level
    assert(dopamine->getLevel() > 0.0f);
    
    // Test reward prediction error
    dopamine->signalRewardPredictionError(0.5f);  // Should affect learning
    assert(dopamine->getLevel() > 0.0f);
    
    // Test plasticity factor
    float plasticity = dopamine->getPlasticityFactor();
    assert(plasticity >= 0.0f);
    
    // Test update with time
    dopamine->update(0.1f);
    assert(dopamine->getLevel() >= 0.0f);
    
    // Test integration with brain applyNeuromodulation
    nlm::Neuromodulator neuromod;
    neuromod.setLevel(0.6f);
    brain.applyNeuromodulation(neurom);
    
    std::cout << "      PASSED" << std::endl;
}

// Test 3: Curiosity system integration
void testCuriositySystem() {
    std::cout << "    Testing curiosity system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::Curiosity* curiosity = brain.getCuriosity();
    assert(curiosity != nullptr);
    
    // Test curiosity signaling
    curiosity->signalCuriosity();
    assert(curiosity->getLevel() > 0.0f);
    
    // Test update
    curiosity->update(0.1f);
    assert(curiosity->getLevel() >= 0.0f);
    
    // Test integration with brain
    nlm::Neuromodulator neuromod;
    neuromod.setLevel(0.8f);
    brain.applyNeuromodulation(neurom);
    
    std::cout << "      PASSED" << std::endl;
}

// Test 4: Prediction error system integration
void testPredictionErrorSystem() {
    std::cout << "    Testing prediction error system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::PredictionError* predError = brain.getPredictionErrorSignal();
    assert(predError != nullptr);
    
    // Test prediction error signaling
    predError->signalPredictionError(0.5f);
    assert(predError->getLevel() > 0.0f);
    
    // Test integration with brain
    nlm::Neuromodulator neuromod;
    neuromod.setLevel(0.4f);
    brain.applyNeuromodulation(neurom);
    
    std::cout << "      PASSED" << std::endl;
}

// Test 5: Novelty system integration
void testNoveltySystem() {
    std::cout << "    Testing novelty system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::Novelty* novelty = brain.getNovelty();
    assert(novelty != nullptr);
    
    // Novelty system has no signal method, just base functionality
    assert(novelty->getLevel() == 0.0f);
    assert(novelty->getPlasticityFactor() >= 0.0f);
    
    // Test integration with brain
    nlm::Neuromodulator neuromod;
    neuromod.setLevel(0.3f);
    brain.applyNeuromodulation(neurom);
    
    std::cout << "      PASSED" << std::endl;
}

// Test 6: Neuromodulation effects on brain activity
void testNeuromodulationEffects() {
    std::cout << "    Testing neuromodulation effects on brain activity..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(60), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Create neuromodulatory signals
    nlm::Neuromodulator dopamineSignal;
    dopamineSignal.setLevel(0.7f);
    
    nlm::Neuromodulator curiositySignal;
    curiositySignal.setLevel(0.5f);
    
    nlm::Neuromodulator rewardSignal;
    rewardSignal.setLevel(0.8f);
    
    // Apply multiple neuromodulatory signals
    brain.applyNeuromodulation(dopamineSignal);
    brain.applyNeuromodulation(curiositySignal);
    brain.applyNeuromodulation(rewardSignal);
    
    // Run simulation steps with neuromodulation
    for (nlm::SimulationStep step = 0; step < 15; ++step) {
        // Create sensory input
        nlm::SensoryInput input;
        input.values = {0.3f, 0.6f, 0.9f};
        brain.receiveSensoryInput(input);
        
        // Step brain
        brain.step(step);
        
        // Update neuromodulators
        brain.getDopamine()->update(0.05f);
        brain.getCuriosity()->update(0.05f);
        brain.getPredictionErrorSignal()->update(0.05f);
        
        // Verify brain activity is modulated
        assert(brain.getTotalSpikeCount() > 0);
        assert(brain.getFiringNeuronCount() > 0);
    }
    
    // Verify neuromodulators have accumulated effects
    assert(brain.getDopamine()->getLevel() > 0.0f);
    assert(brain.getCuriosity()->getLevel() > 0.0f);
    
    std::cout << "      PASSED" << std::endl;
}

// Test 7: Neuromodulation plasticity modulation
void testNeuromodulationPlasticity() {
    std::cout << "    Testing neuromodulation plasticity modulation..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(55), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test different neuromodulators affecting plasticity
    brain.getDopamine()->signalReward(1.0f);
    brain.getCuriosity()->signalCuriosity();
    brain.getPredictionErrorSignal()->signalPredictionError(0.5f);
    
    // Run steps to observe plasticity effects
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        nlm::SensoryInput input;
        input.values = {0.5f, 0.5f, 0.5f};
        brain.receiveSensoryInput(input);
        brain.step(step);
    }
    
    // Plasticity factor should be affected by neuromodulators
    assert(brain.getDopamine()->getPlasticityFactor() >= 0.0f);
    
    // Test development system plasticity
    brain.getDevelopmentSystem()->advanceStage();
    float plasticityModifier = brain.getDevelopmentSystem()->getPlasticityModifier();
    assert(plasticityModifier >= 0.0f && plasticityModifier <= 1.0f);
    
    // Test neuromodulation effects on synaptic changes
    brain.updatePlasticity();
    
    std::cout << "      PASSED" << std::endl;
}

// Test 8: Neuromodulation reward learning integration
void testNeuromodulationRewardLearning() {
    std::cout << "    Testing neuromodulation reward learning integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::Dopamine* dopamine = brain.getDopamine();
    
    // Simulate reward learning
    for (int step = 0; step < 20; ++step) {
        // Present stimulus
        nlm::SensoryInput input;
        input.values = {0.2f + 0.1f * sin(step * 0.5f), 0.3f, 0.4f};
        brain.receiveSensoryInput(input);
        
        // Step brain
        brain.step(step);
        
        // Signal reward (positive or negative based on step)
        float reward = (step % 3 == 0) ? 1.0f : 0.0f;  // Every 3rd step is rewarding
        if (reward > 0.0f) {
            dopamine->signalReward(reward);
        }
    }
    
    // Verify dopamine system responded to rewards
    assert(dopamine->getLevel() > 0.0f);
    
    // Test prediction error in reward learning
    nlm::PredictionError* predError = brain.getPredictionErrorSignal();
    predError->signalPredictionError(0.2f);
    assert(predError->getLevel() > 0.0f);
    
    // Test curiosity in exploration
    nlm::Curiosity* curiosity = brain.getCuriosity();
    curiosity->signalCuriosity();
    assert(curiosity->getLevel() > 0.0f);
    
    std::cout << "      PASSED" << std::endl;
}

void runAll() {
    std::cout << "Neuromodulation Integration Tests" << std::endl;
    std::cout << "=" << std::string(55, '=') << std::endl;
    
    testNeuromodulatorBase();
    testDopamineSystem();
    testCuriositySystem();
    testPredictionErrorSystem();
    testNoveltySystem();
    testNeuromodulationEffects();
    testNeuromodulationPlasticity();
    testNeuromodulationRewardLearning();
    
    std::cout << "=" << std::string(55, '=') << std::endl;
    std::cout << "All neuromodulation integration tests completed successfully!" << std::endl;
}

} // namespace test_neuromodulation_integration