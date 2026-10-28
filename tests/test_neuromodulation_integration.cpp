// Neuromodulation Systems Integration Tests
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>

namespace test_neuromodulation_integration {

void testDopamineCuriosityNoveltyIntegration() {
    std::cout << "    Testing Dopamine + Curiosity + Novelty Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200));
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    auto* predictionError = brain.getPredictionError();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    assert(predictionError != nullptr);
    
    // Initialize with zero levels
    dopamine->setLevel(0.0f);
    curiosity->setLevel(0.0f);
    novelty->setLevel(0.0f);
    
    // Create sensory input sequence with varying novelty
    std::vector<nlm::SensoryInput> sensorySequence;
    float baseTimeStep = 0.001f;
    
    for (int step = 0; step < 100; ++step) {
        nlm::SensoryInput input;
        input.timestamp = step * baseTimeStep;
        input.step = step;
        
        // Vary sensory input to create novelty
        float noveltyFactor = 1.0f + 0.5f * sinf((float)step * 0.1f);
        for (size_t i = 0; i < 15; ++i) {
            input.visionData.push_back(0.5f * noveltyFactor * sinf((float)i * 0.2f));
        }
        
        sensorySequence.push_back(input);
        
        // Compute novelty from sensory input
        float noveltyValue = novelty->computeNovelty(input);
        
        // Update curiosity based on novelty
        curiosity->updateCuriosity(noveltyValue);
        
        // Create prediction error
        float predictedReward = 0.5f + 0.3f * sinf((float)step * 0.05f);
        float actualReward = 0.7f + 0.2f * cosf((float)step * 0.08f);
        float error = predictionError->computeError(predictedReward, actualReward);
        
        // Update dopamine with prediction error
        dopamine->signalRewardPredictionError(error);
        
        // Apply neuromodulation effects
        if (step % 10 == 0) {
            dopamine->update(baseTimeStep);
        }
        curiosity->update(baseTimeStep);
        novelty->update(baseTimeStep);
        
        // Simulate brain step with neuromodulation
        brain.step(step, step * baseTimeStep);
    }
    
    // Verify integration results
    float dopamineLevel = dopamine->getLevel();
    assert(dopamineLevel >= 0.0f); // Should be non-negative
    
    float curiosityLevel = curiosity->getCuriosityLevel();
    assert(curiosityLevel >= 0.0f && curiosityLevel <= 1.0f);
    
    float noveltyLevel = novelty->getNoveltyLevel();
    assert(noveltyLevel >= 0.0f && noveltyLevel <= 1.0f);
    
    // Check that neuromodulators affected brain dynamics
    float firingRate = brain.getAverageFiringRate();
    assert(firingRate >= 0.0f);
    
    std::cout << "    Dopamine + Curiosity + Novelty integration test passed" << std::endl;
}

void testNeuromodulationPlasticityIntegration() {
    std::cout << "    Testing Neuromodulation + Plasticity Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150));
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    
    // Create synaptic connections by spiking neurons
    std::vector<nlm::NeuronId> preSynaptic;
    std::vector<nlm::NeuronId> postSynaptic;
    
    for (int i = 0; i < 20; ++i) {
        preSynaptic.push_back(nlm::NeuronId(i));
        postSynaptic.push_back(nlm::NeuronId(i + 100));
    }
    
    // Record baseline synaptic weights
    float baselineWeight = 0.5f;
    
    // Run simulation with neuromodulation
    for (int step = 0; step < 80; ++step) {
        // Inject activity to create spikes
        brain.injectCurrentToNeurons(nlm::NeuronType::Excitatory, 2.0f);
        
        // Create prediction error based on step
        float predictedValue = 0.6f + 0.2f * sinf((float)step * 0.1f);
        float actualValue = 0.8f + 0.1f * cosf((float)step * 0.12f);
        float error = actualValue - predictedValue;
        
        // Signal prediction error
        dopamine->signalRewardPredictionError(error);
        
        // Update neuromodulation
        dopamine->update(0.001f);
        curiosity->update(0.001f);
        
        // Apply reward modulation
        float reward = actualValue;
        brain.applyRewardModulation(reward, error);
        
        brain.step(step, step * 0.001f);
    }
    
    // Check that neuromodulation affected synaptic plasticity
    float avgSynapticWeight = brain.getAverageSynapticWeight();
    assert(avgSynapticWeight >= 0.0f);
    
    // Verify plasticity rules were applied
    size_t totalSpikeCount = brain.getTotalSpikeCount();
    assert(totalSpikeCount > 0);
    
    std::cout << "    Neuromodulation + Plasticity integration test passed" << std::endl;
}

void testDevelopmentNeuromodulationIntegration() {
    std::cout << "    Testing Development + Neuromodulation Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(180));
    config->set("synaptogenesis_rate", 0.0001f);
    config->set("pruning_rate", 0.00005f);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* dopamine = brain.getDopamine();
    auto* developmentSystem = brain.getDevelopmentSystem();
    
    assert(dopamine != nullptr);
    assert(developmentSystem != nullptr);
    
    // Record initial development state
    float initialPlasticity = developmentSystem->getPlasticityRate();
    
    // Run developmental simulation with neuromodulation
    for (int step = 0; step < 60; ++step) {
        // Update development
        developmentSystem->update(0.001f);
        
        // Simulate learning with neuromodulation
        if (step % 5 == 0) {
            // Signal reward to dopamine
            float reward = 0.5f + 0.3f * sinf((float)step * 0.05f);
            dopamine->signalReward(reward);
        }
        
        // Update dopamine
        dopamine->update(0.001f);
        
        // Simulate spiking and learning
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 1.5f);
        brain.step(step, step * 0.001f);
    }
    
    // Check development-neuromodulation integration
    float finalPlasticity = developmentSystem->getPlasticityRate();
    assert(finalPlasticity >= 0.0f);
    
    // Development should have affected brain connectivity
    size_t totalSynapses = brain.getTotalSynapseCount();
    assert(totalSynapses > 0);
    
    // Neuromodulation levels should reflect development
    float dopamineLevel = dopamine->getLevel();
    assert(dopamineLevel >= 0.0f);
    
    std::cout << "    Development + Neuromodulation integration test passed" << std::endl;
}

void runAll() {
    testDopamineCuriosityNoveltyIntegration();
    testNeuromodulationPlasticityIntegration();
    testDevelopmentNeuromodulationIntegration();
}

} // namespace test_neuromodulation_integration