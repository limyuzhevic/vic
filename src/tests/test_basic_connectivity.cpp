// Test 1: Basic Neural Connectivity Test
// Improved version with better error handling and reporting

#include "tests/test_basic_connectivity.hpp"
#include <iostream>
#include <algorithm>

using namespace nlm;

void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    
    // Validate input
    if (!brain) {
        NLM_LOG_ERROR("Invalid brain pointer for connectivity test");
        return;
    }
    
    // Get the first region with error handling
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_ERROR("No region 1 available for connectivity test");
        return;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_INFO("  No neurons found!");
        return;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject strong current into first 10 neurons with validation
    NLM_LOG_INFO("  Injecting current into 10 neurons...");
    size_t neuronsToStimulate = std::min(size_t(10), neurons.size());
    
    for (size_t i = 0; i < neuronsToStimulate; ++i) {
        if (neurons[i]) {
            neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
        } else {
            NLM_LOG_WARNING("  Neuron at index " + std::to_string(i) + " is null");
        }
    }
    
    // Run a few steps with error checking
    NLM_LOG_INFO("  Running simulation for 50 steps...");
    for (SimulationStep step = 0; step < 50; ++step) {
        try {
            brain->step(step, step * 0.001);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("  Error during simulation step " + std::to_string(step) + ": " + std::string(e.what()));
            break;
        }
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        NLM_LOG_INFO("  ✓ Spikes propagate through network");
    } else {
        NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            if (neurons[i]) {
                NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                            " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                            " thresh=" + std::to_string(neurons[i]->getThreshold()));
            }
        }
    }
}
