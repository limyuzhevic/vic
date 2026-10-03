// Phase 2 STDP Test Implementation
#pragma once

#include "core/Phase2TestFramework.hpp"
#include "core/Phase2TestUtils.hpp"
#include <memory>

namespace nlm {
namespace phase2 {

/**
 * Phase 2 STDP (Spike-Timing-Dependent Plasticity) test
 */
class StdpTest : public Phase2Test {
public:
    StdpTest() = default;
    ~StdpTest() override = default;
    
    TestResult run(std::shared_ptr<Config> config) override;
    std::string getName() const override { return "STDP Test"; }
    std::string getDescription() const override { 
        return "Tests Spike-Timing-Dependent Plasticity with pre-before-post correlation"; 
    }
    bool shouldRun(const Phase2TestConfig& config) const override { 
        return config.runStdpTest; 
    }
};

TestResult StdpTest::run(std::shared_ptr<Config> config) {
    TestResult result(getName());
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        // Create brain
        auto brain = TestUtils::createBrain(config);
        if (!brain->initialize()) {
            result.passed = false;
            result.message = "Failed to initialize brain";
            return result;
        }
        
        // Get first few synapses
        auto* region = brain->getRegion(RegionId(1));
        if (!region || region->getSynapses().size() < 5) {
            result.passed = false;
            result.message = "Not enough synapses for STDP test";
            return result;
        }
        
        auto& synapses = region->getSynapses();
        
        // Record initial weights
        std::vector<float> beforeWeights;
        for (size_t i = 0; i < std::min(size_t(5), synapses.size()); ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false); // Enable only STDP
        }
        
        // Create correlated activity: pre-synaptic then post-synaptic
        auto neurons = region->getAllNeurons();
        if (neurons.size() < 2) {
            result.passed = false;
            result.message = "Not enough neurons for STDP test";
            return result;
        }
        
        // Create pre->post activity (potentiation)
        for (int trial = 0; trial < PHASE2_STDP_TEST_TRIALS; ++trial) {
            Neuron* preNeuron = neurons[0];
            Neuron* postNeuron = neurons[1];
            
            // Pre fires first
            preNeuron->injectCurrent(PHASE2_LTP_CURRENT);
            brain->step(trial * 2, (trial * 2) * PHASE2_SIMULATION_TIMESTEP);
            
            // Then post fires
            postNeuron->injectCurrent(PHASE2_LTP_CURRENT);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * PHASE2_SIMULATION_TIMESTEP);
        }
        
        // Record after weights
        float totalDelta = 0.0f;
        for (size_t i = 0; i < std::min(size_t(5), synapses.size()); ++i) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            totalDelta += delta;
        }
        
        // Check if STDP is working (should see potentiation)
        result.passed = (totalDelta > PHASE2_STDP_THRESHOLD);
        result.message = "STDP test " + std::string(result.passed ? "passed" : "failed");
        result.details.push_back("Total weight delta: " + std::to_string(totalDelta));
        result.details.push_back("STDP threshold: " + std::to_string(PHASE2_STDP_THRESHOLD));
        
        if (totalDelta > 0.001f) {
            result.details.push_back("✓ STDP WORKING: Pre-before-post produced potentiation");
        } else if (totalDelta < -0.001f) {
            result.details.push_back("! STDP reversed: Check parameters");
        } else {
            result.details.push_back("! No change: STDP may not be triggering");
        }
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.message = "Error running STDP test: " + std::string(e.what());
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
    
    return result;
}

} // namespace phase2
} // namespace nlm