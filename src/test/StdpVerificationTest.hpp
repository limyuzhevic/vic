// STDP (Spike-Timing-Dependent Plasticity) Verification Test
// Tests that pre-before-post spike activity strengthens synapses (LTP)

#pragma once

#include "test/TestUtils.hpp"
#include <memory>
#include <vector>

namespace nlm {
namespace test {

class StdpVerificationTest : public SimpleTest {
public:
    StdpVerificationTest() : SimpleTest("STDP Verification") {}
    
protected:
    bool setup(std::shared_ptr<Brain> brain) override {
        NLM_LOG_INFO("Setting up STDP verification test");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region || region->getSynapses().size() < 5) {
            NLM_LOG_ERROR("Not enough synapses for STDP test");
            return false;
        }
        
        // Enable only STDP for this test
        auto& synapses = region->getSynapses();
        for (size_t i = 0; i < 5 && i < synapses.size(); ++i) {
            synapses[i]->enablePlasticity(false, true, false);
        }
        
        return true;
    }
    
    bool executeTest(std::shared_ptr<Brain> brain) override {
        NLM_LOG_INFO("Testing STDP with pre-before-post spike activity");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No brain region found");
            return false;
        }
        
        auto& synapses = region->getSynapses();
        
        // Record initial weights
        std::vector<float> initialWeights;
        for (size_t i = 0; i < 5 && i < synapses.size(); ++i) {
            initialWeights.push_back(synapses[i]->getWeight());
            NLM_LOG_INFO("  Synapse " + std::to_string(i) + 
                        " before: " + std::to_string(initialWeights[i]));
        }
        
        // Create correlated activity: fire pre then post to trigger LTP
        NLM_LOG_INFO("Creating correlated pre->post activity (potentiation)...");
        
        auto neurons = region->getAllNeurons();
        if (neurons.size() < 2) {
            NLM_LOG_ERROR("Not enough neurons for STDP test");
            return false;
        }
        
        for (int trial = 0; trial < 50; ++trial) {
            Neuron* preNeuron = neurons[0];
            Neuron* postNeuron = neurons[1];
            
            if (!preNeuron || !postNeuron) {
                continue;
            }
            
            // Pre fires first
            preNeuron->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001f);
            
            // Then post fires
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001f);
        }
        
        // Record after weights
        NLM_LOG_INFO("After correlated activity:");
        float totalDelta = 0.0f;
        for (size_t i = 0; i < initialWeights.size(); ++i) {
            float delta = synapses[i]->getWeight() - initialWeights[i];
            totalDelta += delta;
            NLM_LOG_INFO("  Synapse " + std::to_string(i) + 
                        " after: " + std::to_string(synapses[i]->getWeight()) +
                        " (Δ=" + std::to_string(delta) + ")");
        }
        
        NLM_LOG_INFO("");
        if (totalDelta > 0.001f) {
            NLM_LOG_INFO("✓ STDP WORKING: Pre-before-post produced potentiation");
            return true;
        } else if (totalDelta < -0.001f) {
            NLM_LOG_INFO("! STDP reversed: Check parameters");
            return false;
        } else {
            NLM_LOG_INFO("! No change: STDP may not be triggering");
            return false;
        }
    }
    
    bool teardown(std::shared_ptr<Brain> brain) override {
        NLM_LOG_INFO("STDP verification test completed");
        return true;
    }
};

}} // namespace nlm::test