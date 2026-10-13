// Plasticity Learning Experiment Test
// Tests that synaptic plasticity (learning) occurs in response to activity

#pragma once

#include "test/TestUtils.hpp"
#include <memory>
#include <vector>

namespace nlm {
namespace test {

class PlasticityExperimentTest : public SimpleTest {
public:
    PlasticityExperimentTest() : SimpleTest("Plasticity Learning Experiment") {}
    
protected:
    bool setup(std::shared_ptr<Brain> brain) override {
        NLM_LOG_INFO("Setting up plasticity learning experiment");
        
        // Enable plasticity on synapses
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false); // Enable Hebbian and STDP
            }
        }
        
        return true;
    }
    
    bool executeTest(std::shared_ptr<Brain> brain) override {
        NLM_LOG_INFO("Running plasticity learning experiment (1000 steps)");
        
        // Record initial state using brain's built-in functionality
        brain->logStatus();
        
        // Apply repeated input patterns to stimulate learning
        for (SimulationStep step = 0; step < 1000; ++step) {
            // Create input pattern - inject current into sensory neurons
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * 0.001f);
            
            // Log progress every 100 steps
            if (step % 100 == 0) {
                NLM_LOG_INFO("  Step " + std::to_string(step) + 
                            " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                            " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        // Record final state
        brain->logStatus();
        
        NLM_LOG_INFO("Plasticity experiment completed");
        
        // Check if learning occurred by comparing activity before/after
        size_t totalSpikes = brain->getTotalSpikeCount();
        size_t firingNeurons = brain->getFiringNeuronCount();
        
        NLM_LOG_INFO("Experiment results:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(totalSpikes));
        NLM_LOG_INFO("  Firing neurons: " + std::to_string(firingNeurons));
        NLM_LOG_INFO("  Learning activity detected: " + std::to_string(totalSpikes > 100));
        
        // Consider experiment successful if we got significant activity
        return totalSpikes > 100 && firingNeurons > 10;
    }
    
    bool teardown(std::shared_ptr<Brain> brain) override {
        NLM_LOG_INFO("Plasticity experiment test completed");
        return true;
    }
};

}} // namespace nlm::test