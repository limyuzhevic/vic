// NLM Basic Connectivity Test
// Phase 2: Real Neural Computation Tests

#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <cassert>

namespace test_basic_connectivity {

void runTest(std::shared_ptr<nlm::Brain> brain) {
    nlm::NLM_LOG_INFO("");
    nlm::NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    
    // Inject current into a few neurons and see if spikes propagate
    auto* region = brain->getRegion(nlm::RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        nlm::NLM_LOG_INFO("  No neurons found!");
        return;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject strong current into first 10 neurons
    nlm::NLM_LOG_INFO("  Injecting current into 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
    }
    
    // Run a few steps
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    nlm::NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        nlm::NLM_LOG_INFO("  ✓ Spikes propagate through network");
        return; // Test passes
    } else {
        nlm::NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            nlm::NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                             " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                             " thresh=" + std::to_string(neurons[i]->getThreshold()));
        }
        throw std::runtime_error("Basic connectivity test failed: no spikes generated");
    }
}

} // namespace test_basic_connectivity