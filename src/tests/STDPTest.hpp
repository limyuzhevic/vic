// tests/STDPTest.hpp
#pragma once

#include "core/TestFramework.hpp"
#include "brain/Brain.hpp"

namespace nlm {

class STDPTest : public Test {
public:
    const std::string& getName() const override { static const std::string name = "STDPTest"; return name; }
    const std::string& getDescription() const override { 
        static const std::string desc = "Tests Spike-Timing-Dependent Plasticity by creating correlated pre->post activity and measuring potentiation"; 
        return desc; 
    }
    
    TestResult run(std::shared_ptr<Brain> brain, const TestConfig& config) override {
        TestResult result;
        result.testName = getName();
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region || region->getSynapses().size() < 5) {
            result.errorMessage = "Not enough synapses for STDP test";
            result.passed = false;
            return result;
        }
        
        // Get first few synapses and record initial weights
        auto& synapses = region->getSynapses();
        std::vector<float> beforeWeights;
        for (size_t i = 0; i < 5 && i < synapses.size(); ++i) {
            synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
            beforeWeights.push_back(synapses[i]->getWeight());
            NLM_LOG_INFO("  Synapse " + std::to_string(i) + " before: " + std::to_string(beforeWeights[i]));
        }
        
        // Create correlated activity: fire pre then post to trigger LTP
        NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
        
        auto neurons = region->getAllNeurons();
        Neuron* preNeuron = nullptr;
        Neuron* postNeuron = nullptr;
        
        if (neurons.size() >= 2) {
            preNeuron = neurons[0];
            postNeuron = neurons[1];
        }
        
        int trials = static_cast<int>(config.steps);
        for (int trial = 0; trial < trials; ++trial) {
            if (preNeuron && postNeuron) {
                // Pre fires first
                preNeuron->injectCurrent(config.currentLevel);
                brain->step(trial * 2, trial * 2 * 0.001);
                
                // Then post fires (short delay)
                postNeuron->injectCurrent(config.currentLevel);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
        }
        
        // Record after weights
        NLM_LOG_INFO("  After correlated activity:");
        float totalDelta = 0.0f;
        for (size_t i = 0; i < beforeWeights.size() && i < 5 && i < synapses.size(); ++i) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            totalDelta += delta;
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                         " after: " + std::to_string(synapses[i]->getWeight()) +
                         " (Δ=" + std::to_string(delta) + ")");
        }
        
        // Check if weights increased (LTP)
        if (totalDelta > 0.001f) {
            result.passed = true;
            NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
            result.metrics["total_potentiation"] = totalDelta;
        } else if (totalDelta < -0.001f) {
            result.passed = false;
            NLM_LOG_INFO("  ! STDP reversed: Check parameters");
            result.errorMessage = "STDP produced depression instead of potentiation";
        } else {
            result.passed = false;
            NLM_LOG_INFO("  ! No change: STDP may not be triggering");
            result.errorMessage = "STDP produced no significant change";
        }
        
        result.metrics["total_delta"] = totalDelta;
        return result;
    }
};

} // namespace nlm