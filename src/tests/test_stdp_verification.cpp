// Test 3: STDP Verification Test
// Improved version with focused testing and better error handling

#include "tests/test_stdp_verification.hpp"
#include <iostream>

using namespace nlm;

void runStdpVerification(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    // Validate input
    if (!brain) {
        NLM_LOG_ERROR("Invalid brain pointer for STDP verification");
        return;
    }
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_ERROR("No region 1 available for STDP test");
        return;
    }
    
    auto& synapses = region->getSynapses();
    if (synapses.empty()) {
        NLM_LOG_INFO("  No synapses for STDP test");
        return;
    }
    
    // Use one real synapse and its own pre/post neurons (improved approach)
    Synapse* syn = synapses[0].get();
    syn->enablePlasticity(false, true, false);  // STDP only
    float beforeWeight = syn->getWeight();
    NLM_LOG_INFO("  Testing STDP on 1 synapse:");
    NLM_LOG_INFO("    Synapse before: " + std::to_string(beforeWeight) +
                 " (pre=" + std::to_string(syn->getSourceNeuron().index()) +
                 ", post=" + std::to_string(syn->getDestinationNeuron().index()) + ")");
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
    
    // Run STDP potentiation test with better control
    float totalDelta = 0.0f;
    for (int trial = 0; trial < 50; ++trial) {
        // Pre-synaptic spike
        brain->injectCurrent(syn->getSourceNeuron(), 60.0f);
        brain->step(trial * 2, trial * 2 * 0.001);
        
        // Post-synaptic spike immediately after (correlated timing)
        brain->injectCurrent(syn->getDestinationNeuron(), 60.0f);
        brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        
        totalDelta = syn->getWeight() - beforeWeight;
        
        // Log progress every 10 trials
        if (trial % 10 == 0) {
            NLM_LOG_INFO("  Trial " + std::to_string(trial + 1) + 
                        ": weight = " + std::to_string(syn->getWeight()) +
                        " (Δ = " + std::to_string(totalDelta) + ")");
        }
    }
    
    // Final weight measurement
    float finalWeight = syn->getWeight();
    float delta = finalWeight - beforeWeight;
    
    NLM_LOG_INFO("  After correlated activity:");
    NLM_LOG_INFO("    Synapse after: " + std::to_string(finalWeight) +
                 " (Δ = " + std::to_string(delta) + ")");
    
    NLM_LOG_INFO("");
    if (delta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (delta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
    }
}
