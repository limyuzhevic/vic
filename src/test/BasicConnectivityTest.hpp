// Basic Neural Connectivity Test
// Tests that neural signals can propagate through the network

#pragma once

#include "test/TestUtils.hpp"
#include <memory>

namespace nlm {
namespace test {

class BasicConnectivityTest : public SimpleTest {
public:
    BasicConnectivityTest() : SimpleTest("Basic Connectivity") {}
    
protected:
    bool setup(std::shared_ptr<Brain> brain) override {
        NLM_LOG_INFO("Setting up basic connectivity test");
        return true;
    }
    
    bool executeTest(std::shared_ptr<Brain> brain) override {
        NLM_LOG_INFO("Testing neural signal propagation");
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject current into sensory neurons
        NLM_LOG_INFO("Injecting current into sensory neurons");
        brain->injectCurrentToNeurons(NeuronType::Sensory, 50.0f);
        
        // Run simulation steps
        NLM_LOG_INFO("Running simulation for 50 steps");
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001f);
        }
        
        // Check for spikes
        size_t finalSpikes = brain->getTotalSpikeCount();
        size_t spikesGenerated = finalSpikes - initialSpikes;
        
        NLM_LOG_INFO("Connectivity test results:");
        NLM_LOG_INFO("  Initial spikes: " + std::to_string(initialSpikes));
        NLM_LOG_INFO("  Final spikes: " + std::to_string(finalSpikes));
        NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikesGenerated));
        
        return spikesGenerated > 0;
    }
    
    bool teardown(std::shared_ptr<Brain> brain) override {
        NLM_LOG_INFO("Basic connectivity test completed");
        return true;
    }
};

}} // namespace nlm::test