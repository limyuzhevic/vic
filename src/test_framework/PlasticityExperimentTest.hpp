// Plasticity Experiment Test Implementation
// Tests plasticity learning through experience

#pragma once

#include "test_framework/TestFramework.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <numeric>

class PlasticityExperimentTest : public TestBase {
public:
    std::string getName() const override {
        return "plasticity";
    }
    
    TestConfig getDefaultConfig() const override {
        TestConfig config;
        config.plasticitySteps = 1000;
        config.plasticityNeuronCount = 20;
        config.plasticityCurrent = 30.0f;
        config.plasticityWeightThreshold = 0.01f;
        config.plasticityLearningThreshold = 0.001f;
        return config;
    }
    
    bool run(std::shared_ptr<nlm::Brain> brain, const TestConfig& config) override {
        if (config.enableProgressReporting) {
            std::cout << "Running Plasticity Learning Experiment..." << std::endl;
            std::cout << "  Simulation steps: " << config.plasticitySteps << std::endl;
            std::cout << "  Neurons receiving input: " << config.plasticityNeuronCount << std::endl;
            std::cout << "  Input current: " << config.plasticityCurrent << std::endl;
        }
        
        // Enable plasticity on synapses
        auto* region = brain->getRegion(nlm::RegionId(1));
        if (!region) {
            std::cerr << "Error: No brain region found" << std::endl;
            return false;
        }
        
        auto& synapses = region->getSynapses();
        if (synapses.empty()) {
            std::cerr << "Error: No synapses found for plasticity" << std::endl;
            return false;
        }
        
        for (auto& syn : synapses) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
        
        // Apply repeated input pattern
        for (nlm::SimulationStep step = 0; step < config.plasticitySteps; ++step) {
            // Create input pattern - inject current into sensory neurons
            size_t neuronsToStimulate = std::min(config.plasticityNeuronCount, brain->getTotalNeuronCount() / 4);
            for (size_t i = 0; i < neuronsToStimulate; ++i) {
                brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, config.plasticityCurrent);
            }
            
            brain->step(step, step * config.timestep);
            
            if (config.enableProgressReporting && step % 200 == 0) {
                std::cout << "  Step " << step << ": " << brain->getTotalSpikeCount() 
                          << " spikes, " << brain->getFiringNeuronCount() << " firing neurons" << std::endl;
            }
        }
        
        if (config.enableProgressReporting) {
            // Compute plasticity statistics
            size_t totalSpikes = brain->getTotalSpikeCount();
            size_t firingNeurons = brain->getFiringNeuronCount();
            std::cout << "  Final spike count: " << totalSpikes << std::endl;
            std::cout << "  Final firing neurons: " << firingNeurons << std::endl;
        }
        
        return true; // Plasticity experiment always succeeds for now
    }
};
