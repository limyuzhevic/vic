// Phase 6 Demo - Integration Test
 *
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>

using namespace nlm;

// Basic connectivity test function for Phase 6 Demo
void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    std::cout << "" << std::endl;
    std::cout << "=== Test 1: Basic Neural Connectivity ===" << std::endl;
    
    // Inject current into a few neurons and see if spikes propagate
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        std::cout << "  No neurons found!" << std::endl;
        return;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject strong current into first 10 neurons
    std::cout << "  Injecting current into 10 neurons..." << std::endl;
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
    }
    
    // Run a few steps
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    std::cout << "  Spikes generated: " << spikes << std::endl;
    
    if (spikes > 0) {
        std::cout << "  ✓ Spikes propagate through network" << std::endl;
    } else {
        std::cout << "  ! No spikes - checking neuron parameters..." << std::endl;
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            std::cout << "    Neuron " << i 
                      << " V=" << neurons[i]->getMembranePotential()
                      << " thresh=" << neurons[i]->getThreshold() << std::endl;
        }
    }
    
    // Additional connectivity metrics
    float avgFiring = brain->getAverageFiringRate();
    size_t activeNeurons = brain->getActiveNeuronCount();
    float excitationInhibitionRatio = brain->getExcitationInhibitionRatio();
    
    std::cout << "  Network connectivity metrics:" << std::endl;
    std::cout << "    Average firing rate: " << avgFiring << std::endl;
    std::cout << "    Active neurons: " << activeNeurons << std::endl;
    std::cout << "    Excitation/Inhibition ratio: " << excitationInhibitionRatio << std::endl;
}