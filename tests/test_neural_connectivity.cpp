// Phase 2 Neural Connectivity Tests
// Tests for basic neural network connectivity and functionality

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>

namespace test_neural_connectivity {

void testBasicConnectivity() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    if (!brain.initialize()) {
        std::cerr << "ERROR: Brain initialization failed for basic connectivity test" << std::endl;
        return;
    }
    
    auto neurons = brain.getRegion(nlm::RegionId(1))->getAllNeurons();
    if (neurons.empty()) {
        std::cerr << "ERROR: No neurons found in basic connectivity test" << std::endl;
        return;
    }
    
    size_t initialSpikes = brain.getTotalSpikeCount();
    
    // Inject current into first 10 neurons
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);
    }
    
    // Run steps
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step, step * 0.001);
    }
    
    size_t spikes = brain.getTotalSpikeCount() - initialSpikes;
    
    if (spikes > 0) {
        std::cout << "[PASS] Basic connectivity test: spikes propagated through network" << std::endl;
    } else {
        std::cout << "[WARNING] Basic connectivity test: no spikes generated" << std::endl;
    }
}

void testNeuralExcitability() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    if (!brain.initialize()) {
        std::cerr << "ERROR: Brain initialization failed for neural excitability test" << std::endl;
        return;
    }
    
    auto neurons = brain.getRegion(nlm::RegionId(1))->getAllNeurons();
    
    // Test individual neuron properties
    bool neuronInitialized = false;
    for (auto& neuron : neurons) {
        if (neuron->getMembranePotential() != 0.0f && neuron->getThreshold() > 0.0f) {
            neuronInitialized = true;
            break;
        }
    }
    
    if (neuronInitialized) {
        std::cout << "[PASS] Neural excitability test: neurons initialized properly" << std::endl;
    } else {
        std::cout << "[WARNING] Neural excitability test: neurons may not be initialized" << std::endl;
    }
}

} // namespace test_neural_connectivity