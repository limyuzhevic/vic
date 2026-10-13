// Basic Neural Connectivity Test
// Tests that spikes propagate correctly through the neural network

#pragma once

#include "framework/TestFramework.hpp"
#include <cmath>

namespace nlm {
namespace tests {

class BasicConnectivityTest : public TestFramework::Test {
public:
    std::string getName() const override {
        return "BasicNeuralConnectivity";
    }
    
    bool run(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) override {
        bool success = true;
        
        logInfo("Testing basic neural connectivity...");
        
        // Get neurons from first region
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            logFailure("No region found in brain");
            return false;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            logFailure("No neurons found in region");
            return false;
        }
        
        logInfo("Found " + std::to_string(neurons.size()) + " neurons");
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        logInfo("Initial spike count: " + std::to_string(initialSpikes));
        
        // Inject strong current into first 10 neurons
        size_t testNeurons = std::min(size_t(10), neurons.size());
        logInfo("Injecting current into " + std::to_string(testNeurons) + " neurons...");
        
        for (size_t i = 0; i < testNeurons; ++i) {
            neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
        }
        
        // Run simulation steps
        size_t steps = 50;
        logInfo("Running " + std::to_string(steps) + " simulation steps...");
        
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        logInfo("Generated " + std::to_string(spikes) + " spikes");
        
        // Check if spikes propagated
        if (spikes > 0) {
            logSuccess("Spikes propagate through network");
            
            // Additional verification: check neuron states
            size_t activeNeurons = 0;
            size_t refractoryNeurons = 0;
            
            for (size_t i = 0; i < std::min(size_t(5), neurons.size()); ++i) {
                if (neurons[i]->isFiring() || neurons[i]->isRefractory()) {
                    activeNeurons++;
                    if (neurons[i]->isRefractory()) {
                        refractoryNeurons++;
                    }
                }
            }
            
            logInfo("Active neurons: " + std::to_string(activeNeurons));
            logInfo("Refractory neurons: " + std::to_string(refractoryNeurons));
            
        } else {
            logWarning("No spikes generated - checking neuron parameters...");
            success = false;
            
            for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
                std::string status = "Neuron " + std::to_string(i) + 
                    " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                    " thresh=" + std::to_string(neurons[i]->getThreshold());
                logInfo("  " + status);
            }
        }
        
        // Test parameter validation
        float threshold = neurons[0]->getThreshold();
        if (threshold < -20.0f || threshold > -40.0f) {
            logWarning("Unusual threshold value: " + std::to_string(threshold) + "mV");
        }
        
        return success;
    }
};

} // namespace tests
} // namespace nlm
