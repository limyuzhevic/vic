// STDP Verification Test
// Tests that Spike-Timing-Dependent Plasticity (STDP) rule works correctly

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <vector>

using namespace nlm;

bool testStdpVerification() {
    std::cout << "\n=== Test 3: STDP Verification ===" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return false;
    }
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        std::cout << "  No region found!" << std::endl;
        return false;
    }
    
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        std::cout << "  Not enough synapses for STDP test" << std::endl;
        return false;
    }
    
    std::cout << "  Testing STDP on 5 synapses:" << std::endl;
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
        std::cout << "    Synapse " << i << 
            " before: " << beforeWeights[i] << std::endl;
    }
    
    // Create correlated activity: fire pre then post to trigger LTP
    std::cout << "\n  Creating correlated pre->post activity (potentiation)..." << std::endl;
    
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
        }
    }
    
    // Record after weights
    std::cout << "  After correlated activity:" << std::endl;
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        std::cout << "    Synapse " << i << 
            " after: " << synapses[i]->getWeight() << 
            " (Δ=" << delta << ")" << std::endl;
    }
    
    // Check if weights increased (LTP)
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        totalDelta += synapses[i]->getWeight() - beforeWeights[i];
    }
    
    std::cout << "\n";
    if (totalDelta > 0.001f) {
        std::cout << "  ✓ STDP WORKING: Pre-before-post produced potentiation" << std::endl;
        return true;
    } else if (totalDelta < -0.001f) {
        std::cout << "  ! STDP reversed: Check parameters" << std::endl;
        return false;
    } else {
        std::cout << "  ! No change: STDP may not be triggering" << std::endl;
        return false;
    }
}

int main() {
    std::cout << "NLM Phase 2 STDP Verification Test" << std::endl;
    std::cout << "===================================" << std::endl;
    
    bool success = testStdpVerification();
    
    if (success) {
        std::cout << "\n✓ STDP verification test PASSED" << std::endl;
        return 0;
    } else {
        std::cout << "\n✗ STDP verification test FAILED" << std::endl;
        return 1;
    }
}