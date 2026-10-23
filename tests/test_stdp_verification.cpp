// NLM STDP Verification Test
// Phase 2: Real Neural Computation Tests

#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <cassert>

namespace test_stdp_verification {

void runTest(std::shared_ptr<nlm::Brain> brain) {
    nlm::NLM_LOG_INFO("");
    nlm::NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    auto* region = brain->getRegion(nlm::RegionId(1));
    if (!region) return;
    
    // Get first few synapses
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        nlm::NLM_LOG_INFO("  Not enough synapses for STDP test");
        return;
    }
    
    nlm::NLM_LOG_INFO("  Testing STDP on 5 synapses:");
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
        nlm::NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                         " before: " + std::to_string(beforeWeights[i]));
    }
    
    // Create correlated activity: fire pre then post to trigger LTP
    nlm::NLM_LOG_INFO("");
    nlm::NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
    
    for (int trial = 0; trial < 50; ++trial) {
        // Fire pre-synaptic neuron
        nlm::Neuron* preNeuron = nullptr;
        nlm::Neuron* postNeuron = nullptr;
        
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            preNeuron = neurons[0];
            postNeuron = neurons[1];
        }
        
        if (preNeuron && postNeuron) {
            // Pre fires first
            preNeuron->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            // Then post fires
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }
    
    // Record after weights
    nlm::NLM_LOG_INFO("  After correlated activity:");
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        nlm::NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                         " after: " + std::to_string(synapses[i]->getWeight()) +
                         " (Δ=" + std::to_string(delta) + ")");
    }
    
    // Check if weights increased (LTP)
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        totalDelta += synapses[i]->getWeight() - beforeWeights[i];
    }
    
    nlm::NLM_LOG_INFO("");
    if (totalDelta > 0.001f) {
        nlm::NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        nlm::NLM_LOG_INFO("  ! STDP reversed: Check parameters");
    } else {
        nlm::NLM_LOG_INFO("  ! No change: STDP may not be triggering");
    }
}

} // namespace test_stdp_verification