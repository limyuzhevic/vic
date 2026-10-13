// STDP Verification Test
// Tests spike timing dependent plasticity (STDP) mechanisms

#pragma once

#include "framework/TestFramework.hpp"
#include <cmath>

namespace nlm {
namespace tests {

class StdpVerificationTest : public TestFramework::Test {
public:
    std::string getName() const override {
        return "STDPVerification";
    }
    
    bool run(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) override {
        logInfo("Testing STDP (Spike Timing Dependent Plasticity)...");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            logFailure("No region found in brain");
            return false;
        }
        
        // Get first few synapses
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            logFailure("Not enough synapses for STDP test (need 5, have " + 
                      std::to_string(synapses.size()) + ")");
            return false;
        }
        
        logInfo("Testing STDP on " + std::to_string(std::min(size_t(5), synapses.size())) + " synapses");
        
        // Record initial weights
        std::vector<float> beforeWeights;
        for (size_t i = 0; i < 5; ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
            logInfo("  Synapse " + std::to_string(i) + 
                   " before: " + std::to_string(beforeWeights[i]));
        }
        
        // Create correlated activity: fire pre then post to trigger LTP
        logInfo("");
        logInfo("  Creating correlated pre->post activity (potentiation)...");
        
        float totalDelta = 0.0f;
        size_t successfulTrials = 0;
        
        for (int trial = 0; trial < 50; ++trial) {
            // Fire pre-synaptic neuron
            Neuron* preNeuron = nullptr;
            Neuron* postNeuron = nullptr;
            
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
                
                successfulTrials++;
            }
        }
        
        logInfo("  Completed " + std::to_string(successfulTrials) + "/50 trials");
        
        // Record after weights
        logInfo("  After correlated activity:");
        for (size_t i = 0; i < 5; ++i) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            totalDelta += delta;
            logInfo("    Synapse " + std::to_string(i) + 
                   " after: " + std::to_string(synapses[i]->getWeight()) +
                   " (Δ=" + std::to_string(delta) + ")");
        }
        
        logInfo("");
        
        // Check if weights increased (LTP)
        if (totalDelta > 0.001f) {
            logSuccess("STDP WORKING: Pre-before-post produced potentiation (+" + 
                      std::to_string(totalDelta) + ")");
            return true;
        } else if (totalDelta < -0.001f) {
            logWarning("STDP reversed: Check parameters (LTD = " + 
                      std::to_string(totalDelta) + ")");
            return false;
        } else {
            logWarning("No change: STDP may not be triggering (Δ = " + 
                      std::to_string(totalDelta) + ")");
            return false;
        }
    }
};

} // namespace tests
} // namespace nlm
