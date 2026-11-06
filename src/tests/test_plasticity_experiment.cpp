// Test 2: Plasticity Learning Experiment
// Improved version with better error handling and pattern-based learning

#include "tests/test_plasticity_experiment.hpp"
#include "LearningExperiment.hpp"
#include <iostream>
#include <algorithm>

using namespace nlm;

void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    
    // Validate input
    if (!brain) {
        NLM_LOG_ERROR("Invalid brain pointer for plasticity experiment");
        return;
    }
    
    // Create learning experiment with improved memory management
    LearningExperiment experiment(brain, 42);
    
    // Record initial state
    experiment.recordInitialState();
    
    // Enable plasticity on synapses with better error handling
    if (auto* region = brain->getRegion(RegionId(1))) {
        NLM_LOG_INFO("  Enabling plasticity on " + std::to_string(region->getSynapses().size()) + " synapses");
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    } else {
        NLM_LOG_ERROR("No region 1 available for plasticity experiment");
        return;
    }
    
    // Apply repeated input pattern to stimulate learning (improved selective stimulation)
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying selective input patterns (1000 steps)...");
    
    std::vector<NeuronId> patternNeurons;
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto* neuron : region->getAllNeurons()) {
            if (neuron && neuron->getType() == NeuronType::Sensory && patternNeurons.size() < 20) {
                patternNeurons.push_back(neuron->getId());
            }
        }
    }
    
    NLM_LOG_INFO("  Selected " + std::to_string(patternNeurons.size()) + " sensory neurons for stimulation");
    
    // Run the learning experiment with progress reporting
    for (SimulationStep step = 0; step < 1000; ++step) {
        // Apply input to selected neurons only (improved from previous version)
        for (NeuronId id : patternNeurons) {
            brain->injectCurrent(id, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress every 100 steps (reduced frequency for cleaner output)
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute and display statistics
    experiment.computeStatistics();
}
