// Basic Connectivity Test Implementation
// Tests basic neural connectivity and spike propagation

#pragma once

#include "test_framework/TestFramework.hpp"
#include "brain/Brain.hpp"
#include <iostream>

class BasicConnectivityTest : public TestBase {
public:
    std::string getName() const override {
        return "connectivity";
    }
    
    TestConfig getDefaultConfig() const override {
        TestConfig config;
        config.connectivityNeuronCount = 10;
        config.connectivityCurrent = 50.0f;
        config.connectivitySteps = 50;
        return config;
    }
    
    bool run(std::shared_ptr<nlm::Brain> brain, const TestConfig& config) override {
        if (config.enableProgressReporting) {
            std::cout << "Running Basic Connectivity Test..." << std::endl;
            std::cout << "  Neuron count: " << config.connectivityNeuronCount << std::endl;
            std::cout << "  Injected current: " << config.connectivityCurrent << std::endl;
            std::cout << "  Simulation steps: " << config.connectivitySteps << std::endl;
        }
        
        // Inject current into neurons
        auto* region = brain->getRegion(nlm::RegionId(1));
        if (!region) {
            std::cerr << "Error: No brain region found" << std::endl;
            return false;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            std::cerr << "Error: No neurons found" << std::endl;
            return false;
        }
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject current into specified number of neurons
        size_t neuronsToInject = std::min(config.connectivityNeuronCount, neurons.size());
        for (size_t i = 0; i < neuronsToInject; ++i) {
            neurons[i]->injectCurrent(config.connectivityCurrent);
        }
        
        // Run simulation steps
        for (nlm::SimulationStep step = 0; step < config.connectivitySteps; ++step) {
            brain->step(step, step * config.timestep);
            if (config.enableProgressReporting && step % 10 == 0) {
                std::cout << "  Step " << step << ": " << brain->getFiringNeuronCount() << " firing neurons" << std::endl;
            }
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        
        if (config.enableProgressReporting) {
            std::cout << "  Result: " << spikes << " spikes generated" << std::endl;
        }
        
        if (spikes == 0) {
            std::cout << "  Warning: No spikes generated - checking neuron parameters" << std::endl;
            for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
                std::cout << "    Neuron " << i << " V=" << neurons[i]->getMembranePotential()
                          << " thresh=" << neurons[i]->getThreshold() << std::endl;
            }
            return false; // Consider this a test failure
        }
        
        return true;
    }
};
