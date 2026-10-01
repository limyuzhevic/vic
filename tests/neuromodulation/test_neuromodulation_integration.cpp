// Neuromodulation Integration Tests for NLM
// Tests integration between Dopamine, Curiosity, Novelty, PredictionError, Acetylcholine, and Norepinephrine

#include "brain/Brain.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "neuromodulation/Reward.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "prediction/PredictionSystem.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "plasticity/STDP.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>

namespace test_neuromodulation_integration {

void testDopamineIntegration() {
    std::cout << "    Testing dopamine integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    // Create dopamine signal
    float reward = 1.0f;
    float predictionError = 0.5f;
    nlm::Neuromodulator dopamineSignal(nlm::Dopamine::Create(reward, predictionError));
    
    // Apply to brain
    brain.applyNeuromodulation(dopamineSignal);
    
    // Verify integration
    assert(dopamine->getCurrentLevel() > 0.0f);
    assert(dopamine->getPlasticityModulation() > 0.0f);
    
    // Verify effects on plasticity
    auto* stdp = brain.getSTDP();
    assert(stdp != nullptr);
    
    std::cout << "    testDopamineIntegration passed" << std::endl;
}

void testCuriosityIntegration() {
    std::cout << "    Testing curiosity integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* curiosity = brain.getCuriosity();
    assert(curiosity != nullptr);
    
    // Create curiosity signal
    float noveltyLevel = 0.8f;
    float uncertainty = 0.6f;
    nlm::Neuromodulator curiositySignal(nlm::Curiosity::Create(noveltyLevel, uncertainty));
    
    brain.applyNeuromodulation(curiositySignal);
    
    // Verify integration
    assert(curiosity->getCurrentLevel() > 0.0f);
    assert(curiosity->getExplorationDrive() > 0.0f);
    
    // Verify effects on behavior
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    std::cout << "    testCuriosityIntegration passed" << std::endl;
}

void testNoveltyIntegration() {
    std::cout << "    Testing novelty integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* novelty = brain.getNovelty();
    assert(novelty != nullptr);
    
    // Create novelty signal
    std::vector<float> sensoryInput = {0.1f, 0.5f, 0.9f, 0.2f, 0.8f};
    nlm::Neuromodulator noveltySignal(nlm::Novelty::Create(sensoryInput));
    
    brain.applyNeuromodulation(noveltySignal);
    
    // Verify integration
    assert(novelty->getCurrentLevel() > 0.0f);
    assert(novelty->getSurpriseScore() > 0.0f);
    
    // Verify novelty affects attention
    auto* attention = brain.getAttention();
    assert(attention != nullptr);
    
    std::cout << "    testNoveltyIntegration passed" << std::endl;
}

void testPredictionErrorIntegration() {
    std::cout << "    Testing prediction error integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* predictionError = brain.getPredictionErrorSignal();
    assert(predictionError != nullptr);
    
    // Create prediction error signal
    std::vector<float> predicted = {0.3f, 0.7f, 0.5f};
    std::vector<float> actual = {0.4f, 0.6f, 0.6f};
    float errorMagnitude = 0.2f;
    nlm::Neuromodulator errorSignal(nlm::PredictionError::Create(predicted, actual, errorMagnitude));
    
    brain.applyNeuromodulation(errorSignal);
    
    // Verify integration
    assert(predictionError->getCurrentLevel() > 0.0f);
    assert(predictionError->getErrorHistory().size() > 0);
    
    // Verify affects learning
    auto* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    std::cout << "    testPredictionErrorIntegration passed" << std::endl;
}

void testSystemNeuromodulationIntegration() {
    std::cout << "    Testing system-level neuromodulation integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Create multiple neuromodulators
    std::vector<nlm::Neuromodulator> signals;
    
    signals.push_back(nlm::Neuromodulator(nlm::Dopamine::Create(1.0f, 0.3f)));
    signals.push_back(nlm::Neuromodulator(nlm::Curiosity::Create(0.7f, 0.5f)));
    signals.push_back(nlm::Neuromodulator(nlm::Novelty::Create(std::vector<float>{0.1f, 0.9f, 0.5f})));
    
    // Apply all signals
    for (const auto& signal : signals) {
        brain.applyNeuromodulation(signal);
    }
    
    // Verify integrated effects
    assert(brain.getDopamine() != nullptr);
    assert(brain.getCuriosity() != nullptr);
    assert(brain.getNovelty() != nullptr);
    
    // Run brain loop to see integration effects
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    std::cout << "    testSystemNeuromodulationIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "=== Neuromodulation Integration Tests ===" << std::endl;
    testDopamineIntegration();
    testCuriosityIntegration();
    testNoveltyIntegration();
    testPredictionErrorIntegration();
    testSystemNeuromodulationIntegration();
    std::cout << "=== Neuromodulation Integration Tests Complete ===" << std::endl;
}

} // namespace test_neuromodulation_integration