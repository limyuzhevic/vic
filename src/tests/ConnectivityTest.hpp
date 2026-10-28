// tests/ConnectivityTest.hpp
#pragma once

#include "core/TestFramework.hpp"
#include "brain/Brain.hpp"

namespace nlm {

class ConnectivityTest : public Test {
public:
    const std::string& getName() const override { static const std::string name = "ConnectivityTest"; return name; }
    const std::string& getDescription() const override { 
        static const std::string desc = "Tests basic neural connectivity by injecting current and checking for spike propagation"; 
        return desc; 
    }
    
    TestResult run(std::shared_ptr<Brain> brain, const TestConfig& config) override {
        TestResult result;
        result.testName = getName();
        
        // Inject current into neurons
        size_t neuronsToStimulate = std::min(static_cast<size_t>(10), brain->getTotalNeuronCount() / 4);
        for (size_t i = 0; i < neuronsToStimulate; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, config.currentLevel);
        }
        
        // Run simulation steps
        size_t initialSpikes = brain->getTotalSpikeCount();
        for (SimulationStep step = 0; step < config.steps; ++step) {
            brain->step(step, step * 0.001);
        }
        
        // Check results
        size_t spikesGenerated = brain->getTotalSpikeCount() - initialSpikes;
        result.spikesGenerated = spikesGenerated;
        result.passed = (spikesGenerated > 0);
        
        if (result.passed) {
            result.metrics["spikes_per_step"] = static_cast<double>(spikesGenerated) / config.steps;
        } else {
            result.errorMessage = "No spikes generated despite current injection";
        }
        
        return result;
    }
};

} // namespace nlm