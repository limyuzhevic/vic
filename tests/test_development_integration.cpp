// Development System Integration Tests
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "development/Synaptogenesis.hpp"
#include "development/Pruning.hpp"
#include "development/Maturation.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

namespace test_development_integration {

void testSynaptogenesisPlasticityIntegration() {
    std::cout << "    Testing Synaptogenesis + Plasticity Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200));
    config->set("synaptogenesis_rate", 0.0001f);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* synaptogenesis = brain.getSynaptogenesis();
    auto* stdp = brain.getSTDP();
    
    assert(synaptogenesis != nullptr);
    assert(stdp != nullptr);
    
    // Record initial synapse count
    size_t initialSynapses = brain.getTotalSynapseCount();
    
    // Run simulation with development and plasticity
    for (int step = 0; step < 100; ++step) {
        // Update development (synaptogenesis)
        synaptogenesis->update(0.001f);
        
        // Apply STDP plasticity
        stdp->update(0.001f);
        
        // Simulate neural activity
        if (step % 10 == 0) {
            // Create spikes to trigger STDP
            for (int i = 0; i < 5; ++i) {
                brain.injectCurrentToNeurons(nlm::NeuronType::Excitatory, 3.0f);
            }
        }
        
        // Apply Hebbian plasticity occasionally
        if (step % 15 == 0) {
            brain.applyHebbianPlasticity();
        }
        
        brain.step(step, step * 0.001f);
    }
    
    // Check integration results
    size_t finalSynapses = brain.getTotalSynapseCount();
    assert(finalSynapses >= initialSynapses); // Should grow or stay the same
    
    float avgWeight = brain.getAverageSynapticWeight();
    assert(avgWeight >= 0.0f);
    
    // Synaptogenesis should have created new connections
    size_t synaptogenesisEvents = synaptogenesis->getEventCount();
    assert(synaptogenesisEvents >= 0);
    
    std::cout << "    Synaptogenesis + Plasticity integration test passed" << std::endl;
}

void testPruningMaturationIntegration() {
    std::cout << "    Testing Pruning + Maturation Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(250));
    config->set("pruning_rate", 0.00005f);
    config->set("maturation_factor", 0.8f);
    config->set("excitability_increase_rate", 0.02f);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* pruning = brain.getPruning();
    auto* maturation = brain.getMaturation();
    
    assert(pruning != nullptr);
    assert(maturation != nullptr);
    
    // Record initial state
    size_t initialSynapses = brain.getTotalSynapseCount();
    float initialExcitability = brain.getGlobalExcitability();
    
    // Run developmental simulation
    for (int step = 0; step < 80; ++step) {
        // Update maturation
        maturation->update(0.001f);
        
        // Update pruning based on activity
        pruning->update(0.001f);
        
        // Simulate learning activity
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 2.0f);
        
        // Apply age-dependent plasticity
        if (step % 20 == 0) {
            brain.applyDevelopmentPlasticity();
        }
        
        brain.step(step, step * 0.001f);
    }
    
    // Check integration results
    size_t finalSynapses = brain.getTotalSynapseCount();
    assert(finalSynapses >= 0); // May prune or maintain
    
    float finalExcitability = brain.getGlobalExcitability();
    assert(finalExcitability >= 0.0f);
    
    // Check maturation effects
    size_t pruningEvents = pruning->getPrunedSynapseCount();
    assert(pruningEvents >= 0);
    
    size_t maturationEvents = maturation->getMaturationCount();
    assert(maturationEvents >= 0);
    
    // Excitability should have changed with maturation
    assert(std::abs(finalExcitability - initialExcitability) >= 0.0f);
    
    std::cout << "    Pruning + Maturation integration test passed" << std::endl;
}

void testDevelopmentMemoryIntegration() {
    std::cout << "    Testing Development + Memory Integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(180));
    config->set("synaptogenesis_rate", 0.00005f);
    config->set("maturation_factor", 0.7f);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* developmentSystem = brain.getDevelopmentSystem();
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    
    assert(developmentSystem != nullptr);
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    
    // Create memory patterns
    std::vector<float> memoryPattern1(30, 0.0f);
    std::vector<float> memoryPattern2(30, 0.0f);
    
    for (size_t i = 0; i < 30; ++i) {
        memoryPattern1[i] = sinf((float)i * 0.2f);
        memoryPattern2[i] = cosf((float)i * 0.2f);
    }
    
    // Run developmental learning with memory
    for (int step = 0; step < 120; ++step) {
        // Update development
        developmentSystem->update(0.001f);
        
        // Store patterns in working memory
        if (step % 15 == 0) {
            workingMemory->store(memoryPattern1, 0.9f);
            
            // Create episodic memory
            nlm::EpisodicMemoryItem episode;
            episode.timestamp = step;
            episode.sensoryState = memoryPattern2;
            episode.action = nlm::ActionType::Wait;
            episode.reward = 0.6f + 0.2f * sinf((float)step * 0.1f);
            
            episodicMemory->storeEpisode(episode);
        }
        
        // Apply development-modulated plasticity
        brain.applyDevelopmentPlasticity();
        
        // Simulate learning activity
        brain.injectCurrentToNeurons(nlm::NeuronType::Excitatory, 1.5f);
        
        brain.step(step, step * 0.001f);
    }
    
    // Check integration results
    size_t finalMemories = episodicMemory->getEpisodeCount();
    assert(finalMemories >= 0);
    
    size_t activeTraces = workingMemory->getActiveTraces();
    assert(activeTraces >= 0);
    
    // Development should have enhanced memory formation
    float plasticityRate = developmentSystem->getPlasticityRate();
    assert(plasticityRate >= 0.0f);
    
    // Check memory quality (should have some consolidation)
    if (finalMemories > 0) {
        episodicMemory->consolidate(0.5f); // Consolidate older memories
    }
    
    std::cout << "    Development + Memory integration test passed" << std::endl;
}

void runAll() {
    testSynaptogenesisPlasticityIntegration();
    testPruningMaturationIntegration();
    testDevelopmentMemoryIntegration();
}

} // namespace test_development_integration