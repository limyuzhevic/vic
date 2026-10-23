// NLM Plasticity Experiment Test
// Phase 2: Real Neural Computation Tests

#include "brain/Brain.hpp"
#include "experiments/LearningExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <cassert>

namespace test_plasticity_experiment {

void runTest(std::shared_ptr<nlm::Brain> brain) {
    nlm::NLM_LOG_INFO("");
    nlm::NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    
    nlm::LearningExperiment experiment(brain, 42);
    
    // Record initial state
    experiment.recordInitialState();
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(nlm::RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input pattern to stimulate learning
    nlm::NLM_LOG_INFO("");
    nlm::NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
    
    for (nlm::SimulationStep step = 0; step < 1000; ++step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            nlm::NLM_LOG_INFO("  Step " + std::to_string(step) + 
                             " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                             " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute and display statistics
    experiment.computeStatistics();
}

} // namespace test_plasticity_experiment