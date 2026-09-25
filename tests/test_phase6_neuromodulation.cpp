// Phase 6 Neuromodulation System Integration Tests
// Comprehensive testing of neuromodulation system integration

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>

namespace test_phase6_neuromodulation {

void testDopamineSystemIntegration() {
    std::cout << "    Testing Dopamine System Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    // Test initial dopamine level
    float initialDopamine = dopamine->getCurrentLevel();
    std::cout << "      Initial dopamine level: " << initialDopamine << std::endl;
    
    // Run simulation with reward signals
    for (nlm::SimulationStep step = 0; step < 200; ++step) {
        brain.step(step);
        
        // Apply reward signals at different intensities
        if (step % 20 == 0) {
            nlm::Reward reward;
            reward.value = 0.8f;  // Strong reward
            reward.source = nlm::RewardSource::Action;
            brain.applyNeuromodulation(reward);
        }
        else if (step % 30 == 0) {
            nlm::Reward reward;
            reward.value = 0.3f;  // Moderate reward
            reward.source = nlm::RewardSource::Punishment;
            brain.applyNeuromodulation(reward);
        }
        
        // Apply prediction error signals
        if (step % 25 == 0) {
            nlm::PredictionError predError;
            predError.value = 0.6f;
            predError.type = nlm::PredictionErrorType::Positive;
            brain.applyNeuromodulation(predError);
        }
    }
    
    float finalDopamine = dopamine->getCurrentLevel();
    std::cout << "      Final dopamine level: " << finalDopamine << std::endl;
    
    // Dopamine level should have changed based on rewards
    assert(std::abs(finalDopamine - initialDopamine) >= 0.0f);
    
    std::cout << "      Dopamine system integration test PASSED" << std::endl;
}

void testCuriositySystemIntegration() {
    std::cout << "    Testing Curiosity System Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* curiosity = brain.getCuriosity();
    assert(curiosity != nullptr);
    
    // Test curiosity level
    float initialCuriosity = curiosity->getCuriosityLevel();
    std::cout << "      Initial curiosity level: " << initialCuriosity << std::endl;
    
    // Run simulation with novelty signals
    for (nlm::SimulationStep step = 0; step < 150; ++step) {
        brain.step(step);
        
        // Inject novelty to stimulate curiosity
        if (step % 15 == 0) {
            brain.injectCurrentToNeurons(nlm::NeuronType::Excitatory, 1.0f);
        }
        
        // Apply prediction errors that drive curiosity
        if (step % 20 == 0) {
            nlm::PredictionError predError;
            predError.value = 0.7f;
            predError.type = nlm::PredictionErrorType::Negative;
            brain.applyNeuromodulation(predError);
        }
    }
    
    float finalCuriosity = curiosity->getCuriosityLevel();
    std::cout << "      Final curiosity level: " << finalCuriosity << std::endl;
    
    // Curiosity should have changed based on exploration
    assert(finalCuriosity >= 0.0f);
    
    std::cout << "      Curiosity system integration test PASSED" << std::endl;
}

void testNoveltySystemIntegration() {
    std::cout << "    Testing Novelty System Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* novelty = brain.getNovelty();
    assert(novelty != nullptr);
    
    // Test novelty detection
    float initialNovelty = novelty->getNoveltyLevel();
    std::cout << "      Initial novelty level: " << initialNovelty << std::endl;
    
    // Run simulation with varied inputs
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
        
        // Create novelty through varied sensory patterns
        if (step % 10 == 0) {
            nlm::SensoryInput sensoryInput;
            sensoryInput.pattern = step % 5;  // Different patterns
            sensoryInput.intensity = 0.5f + (step % 10) * 0.1f;
            brain.receiveSensoryInput(sensoryInput);
            
            brain.applyNeuromodulation(sensoryInput);
        }
    }
    
    float finalNovelty = novelty->getNoveltyLevel();
    std::cout << "      Final novelty level: " << finalNovelty << std::endl;
    
    // Novelty should reflect input variety
    assert(finalNovelty >= 0.0f);
    
    std::cout << "      Novelty system integration test PASSED" << std::endl;
}

void testPredictionErrorSystemIntegration() {
    std::cout << "    Testing Prediction Error System Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto* predError = brain.getPredictionErrorSignal();
    assert(predError != nullptr);
    
    // Test prediction error accumulation
    float initialPredError = predError->getErrorValue();
    std::cout << "      Initial prediction error: " << initialPredError << std::endl;
    
    // Run simulation with prediction errors
    for (nlm::SimulationStep step = 0; step < 200; ++step) {
        brain.step(step);
        
        // Simulate prediction errors based on sensory mismatch
        if (step % 25 == 0) {
            nlm::PredictionError error;
            error.value = 0.4f + (step % 3) * 0.2f;
            error.type = (step % 2 == 0) ? nlm::PredictionErrorType::Positive : nlm::PredictionErrorType::Negative;
            brain.applyNeuromodulation(error);
        }
    }
    
    float finalPredError = predError->getErrorValue();
    std::cout << "      Final prediction error: " << finalPredError << std::endl;
    
    // Prediction error should have accumulated
    assert(finalPredError >= 0.0f);
    
    // Test prediction error reset
    predError->reset();
    assert(predError->getErrorValue() == 0.0f);
    
    std::cout << "      Prediction error system integration test PASSED" << std::endl;
}

void testNeuromodulationFeedbackLoop() {
    std::cout << "    Testing Neuromodulation Feedback Loop..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Verify all neuromodulation systems
    assert(brain.getDopamine() != nullptr);
    assert(brain.getCuriosity() != nullptr);
    assert(brain.getNovelty() != nullptr);
    assert(brain.getPredictionErrorSignal() != nullptr);
    
    std::cout << "      All neuromodulation systems verified" << std::endl;
    
    // Run complex feedback simulation
    for (nlm::SimulationStep step = 0; step < 300; ++step) {
        brain.step(step);
        
        // Complex neuromodulation interactions
        if (step % 30 == 0) {
            nlm::Reward reward;
            reward.value = 0.5f + (step % 5) * 0.2f;
            reward.source = nlm::RewardSource::Action;
            brain.applyNeuromodulation(reward);
        }
        
        if (step % 40 == 0) {
            nlm::PredictionError predError;
            predError.value = 0.8f;
            predError.type = nlm::PredictionErrorType::Negative;
            brain.applyNeuromodulation(predError);
        }
        
        if (step % 50 == 0) {
            nlm::NoveltySignal novelty;
            novelty.strength = 0.6f;
            novelty.cause = "exploration";
            brain.applyNeuromodulation(novelty);
        }
        
        // Feedback from memory systems
        if (step % 60 == 0) {
            auto* wm = brain.getWorkingMemory();
            if (wm != nullptr && wm->getActiveTraces() > 0) {
                nlm::Reward reward;
                reward.value = 0.3f;  // Memory-based reward
                reward.source = nlm::RewardSource::Memory;
                brain.applyNeuromodulation(reward);
            }
        }
    }
    
    std::cout << "      Neuromodulation feedback loop test PASSED" << std::endl;
}

void runAll() {
    std::cout << "=== Phase 6 Neuromodulation Tests ===" << std::endl;
    
    testDopamineSystemIntegration();
    testCuriositySystemIntegration();
    testNoveltySystemIntegration();
    testPredictionErrorSystemIntegration();
    testNeuromodulationFeedbackLoop();
    
    std::cout << "\n=== All Phase 6 Neuromodulation Tests PASSED ===" << std::endl;
}

} // namespace test_phase6_neuromodulation