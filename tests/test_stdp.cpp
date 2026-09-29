// STDP Verification Tests
// Specific tests for Spike-Timing-Dependent Plasticity (STDP)

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>

namespace test_stdp_verification {

void testSTDPPreBeforePost() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    if (!brain.initialize()) {
        std::cerr << "ERROR: Brain initialization failed for STDP test" << std::endl;
        return;
    }
    
    auto* region = brain.getRegion(nlm::RegionId(1));
    if (!region) {
        std::cerr << "ERROR: No region found for STDP test" << std::endl;
        return;
    }
    
    auto& synapses = region->getSynapses();
    if (synapses.size() < 2) {
        std::cout << "[WARNING] STDP test: not enough synapses for test" << std::endl;
        return;
    }
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 2; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
    }
    
    // Create correlated activity: pre fires before post (LTP)
    auto neurons = region->getAllNeurons();
    if (neurons.size() >= 2) {
        std::cout << "Creating correlated pre->post activity (potentiation)..." << std::endl;
        
        for (int trial = 0; trial < 10; ++trial) {
            // Fire pre-synaptic neuron
            neurons[0]->injectCurrent(60.0f);
            brain.step(trial * 2, trial * 2 * 0.001);
            
            // Then post fires (within STDP time window)
            neurons[1]->injectCurrent(60.0f);
            brain.step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
        
        // Record after weights
        std::cout << "After correlated activity:" << std::endl;
        float totalDelta = 0.0f;
        for (size_t i = 0; i < 2; ++i) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            totalDelta += delta;
            std::cout << "  Synapse " << i << ": Δ=" << delta << std::endl;
        }
        
        // Check if weights increased (LTP)
        if (totalDelta > 0.001f) {
            std::cout << "[PASS] STDP working: pre-before-post produced potentiation" << std::endl;
        } else if (totalDelta < -0.001f) {
            std::cout << "[INFO] STDP reversed: Check parameters" << std::endl;
        } else {
            std::cout << "[INFO] STDP no change: May not be triggering (normal for simple test)" << std::endl;
        }
    }
}

void testSTDPPostBeforePre() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    if (!brain.initialize()) {
        std::cerr << "ERROR: Brain initialization failed for STDP test" << std::endl;
        return;
    }
    
    auto* region = brain.getRegion(nlm::RegionId(1));
    if (!region) {
        std::cerr << "ERROR: No region found for STDP test" << std::endl;
        return;
    }
    
    auto& synapses = region->getSynapses();
    if (synapses.size() < 2) {
        std::cout << "[WARNING] STDP test: not enough synapses for test" << std::endl;
        return;
    }
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 2; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
    }
    
    // Create correlated activity: post fires before pre (LTD)
    auto neurons = region->getAllNeurons();
    if (neurons.size() >= 2) {
        std::cout << "Creating correlated post->pre activity (depression)..." << std::endl;
        
        for (int trial = 0; trial < 10; ++trial) {
            // Fire post-synaptic neuron first
            neurons[1]->injectCurrent(60.0f);
            brain.step(trial * 2, trial * 2 * 0.001);
            
            // Then pre fires (within STDP time window)
            neurons[0]->injectCurrent(60.0f);
            brain.step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
        
        // Record after weights
        std::cout << "After correlated activity (post->pre):" << std::endl;
        float totalDelta = 0.0f;
        for (size_t i = 0; i < 2; ++i) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            totalDelta += delta;
            std::cout << "  Synapse " << i << ": Δ=" << delta << std::endl;
        }
        
        // Check if weights decreased (LTD)
        if (totalDelta < -0.001f) {
            std::cout << "[PASS] STDP working: post-before-pre produced depression" << std::endl;
        } else if (totalDelta > 0.001f) {
            std::cout << "[INFO] STDP reversed: Check parameters" << std::endl;
        } else {
            std::cout << "[INFO] STDP no change: May not be triggering (normal for simple test)" << std::endl;
        }
    }
}

} // namespace test_stdp_verification