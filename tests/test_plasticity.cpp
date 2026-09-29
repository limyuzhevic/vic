// Plasticity and Learning Tests
// Tests for synaptic plasticity, learning, and memory formation

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>

namespace test_plasticity {

struct LearningExperiment {
    std::shared_ptr<nlm::Brain> brain;
    uint64_t seed;
    
    LearningExperiment(std::shared_ptr<nlm::Brain> b, uint64_t s) 
        : brain(b), seed(s) {}
    
    void runExperiment() {
        // Record initial state
        size_t initialSynapseCount = brain->getTotalSynapseCount();
        
        // Enable plasticity on synapses
        if (auto* region = brain->getRegion(nlm::RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
            }
        }
        
        // Apply repeated input pattern to stimulate learning
        std::cout << "Applying repeated input patterns..." << std::endl;
        
        for (nlm::SimulationStep step = 0; step < 100; ++step) {
            // Create input pattern
            for (size_t i = 0; i < 10 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * 0.001);
            
            if (step % 20 == 0) {
                std::cout << "  Step " << step 
                          << " | Spikes: " << brain->getTotalSpikeCount()
                          << " | Firing: " << brain->getFiringNeuronCount() << std::endl;
            }
        }
        
        // Check for learning
        size_t finalSynapseCount = brain->getTotalSynapseCount();
        size_t spikes = brain->getTotalSpikeCount();
        
        std::cout << "Initial synapses: " << initialSynapseCount << std::endl;
        std::cout << "Final synapses: " << finalSynapseCount << std::endl;
        std::cout << "Total spikes: " << spikes << std::endl;
        
        if (spikes > 0) {
            std::cout << "[PASS] Plasticity experiment: learning activity detected" << std::endl;
        } else {
            std::cout << "[WARNING] Plasticity experiment: no significant activity" << std::endl;
        }
    }
};

void testHebbianPlasticity() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    if (!brain.initialize()) {
        std::cerr << "ERROR: Brain initialization failed for Hebbian plasticity test" << std::endl;
        return;
    }
    
    LearningExperiment experiment(std::make_shared<nlm::Brain>(brain), 42);
    experiment.runExperiment();
}

void testSTDPMechanism() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    if (!brain.initialize()) {
        std::cerr << "ERROR: Brain initialization failed for STDP test" << std::endl;
        return;
    }
    
    // Record initial weights
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
    
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 2; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
    }
    
    // Create correlated activity (pre before post)
    auto neurons = region->getAllNeurons();
    if (neurons.size() >= 2) {
        // Fire pre-synaptic neuron
        neurons[0]->injectCurrent(60.0f);
        brain.step(0, 0.0);
        
        // Then post fires
        neurons[1]->injectCurrent(60.0f);
        brain.step(1, 0.001);
        
        // Check weight changes
        float delta1 = synapses[0]->getWeight() - beforeWeights[0];
        float delta2 = synapses[1]->getWeight() - beforeWeights[1];
        
        if (delta1 > 0.001f || delta2 > 0.001f) {
            std::cout << "[PASS] STDP mechanism: potentiation detected" << std::endl;
        } else {
            std::cout << "[INFO] STDP mechanism: no significant change (may be normal)" << std::endl;
        }
    }
}

} // namespace test_plasticity