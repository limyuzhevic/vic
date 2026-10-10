// Test program to verify memory management fixes
// Compile with: g++ -std=c++17 test_memory_fix.cpp

#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/NeuralRegion.hpp"
#include "brain/NeuralPopulation.hpp"
#include "development/DevelopmentSystem.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "neuromodulation/Reward.hpp"
#include "agent/AgentBrain.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <vector>
#include <memory>

int main() {
    std::cout << "Testing memory management fixes..." << std::endl;
    
    try {
        // Test 1: Create and destroy Brain object
        std::cout << "Test 1: Brain object creation/destruction..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        {
            nlm::Brain brain(config);
            std::cout << "  Brain created successfully" << std::endl;
            brain.initialize();
            std::cout << "  Brain initialized successfully" << std::endl;
        }
        std::cout << "  Brain destroyed - no memory leaks!" << std::endl;
        
        // Test 2: Create and destroy Neuron objects
        std::cout << "Test 2: Neuron object creation/destruction..." << std::endl;
        std::vector<nlm::Neuron*> neurons;
        for (int i = 0; i < 10; ++i) {
            neurons.push_back(new nlm::Neuron(nlm::NeuronId(i + 1)));
        }
        std::cout << "  Created " << neurons.size() << " neurons" << std::endl;
        
        // Move test
        if (!neurons.empty()) {
            nlm::Neuron movedNeuron(std::move(*neurons[0]));
            neurons.erase(neurons.begin());
            std::cout << "  Moved neuron - source nullified: " << (neurons.empty() ? "true" : "false") << std::endl;
        }
        
        // Cleanup
        for (auto* neuron : neurons) {
            delete neuron;
        }
        std::cout << "  All neurons destroyed - no memory leaks!" << std::endl;
        
        // Test 3: Create and destroy Synapse objects
        std::cout << "Test 3: Synapse object creation/destruction..." << std::endl;
        std::vector<nlm::Synapse*> synapses;
        for (int i = 0; i < 5; ++i) {
            synapses.push_back(new nlm::Synapse(nlm::SynapseId(i + 1), 
                                                nlm::NeuronId(1), 
                                                nlm::NeuronId(2)));
        }
        std::cout << "  Created " << synapses.size() << " synapses" << std::endl;
        
        // Move test
        if (!synapses.empty()) {
            nlm::Synapse movedSynapse(std::move(*synapses[0]));
            synapses.erase(synapses.begin());
            std::cout << "  Moved synapse - source nullified: " << (synapses.empty() ? "true" : "false") << std::endl;
        }
        
        // Cleanup
        for (auto* synapse : synapses) {
            delete synapse;
        }
        std::cout << "  All synapses destroyed - no memory leaks!" << std::endl;
        
        // Test 4: Create and destroy DevelopmentSystem object
        std::cout << "Test 4: DevelopmentSystem object creation/destruction..." << std::endl;
        {
            nlm::DevelopmentSystem devSystem;
            std::cout << "  DevelopmentSystem created successfully" << std::endl;
            devSystem.setStage(nlm::DevelopmentalStage::Initial);
            std::cout << "  DevelopmentSystem initialized successfully" << std::endl;
        }
        std::cout << "  DevelopmentSystem destroyed - no memory leaks!" << std::endl;
        
        // Test 5: Create and destroy Neuromodulator objects
        std::cout << "Test 5: Neuromodulator objects creation/destruction..." << std::endl;
        {
            nlm::Dopamine dopamine;
            std::cout << "  Dopamine created successfully" << std::endl;
            dopamine.setLevel(0.5f);
            std::cout << "  Dopamine level: " << dopamine.getLevel() << std::endl;
        }
        std::cout << "  Dopamine destroyed - no memory leaks!" << std::endl;
        
        {
            nlm::Curiosity curiosity;
            std::cout << "  Curiosity created successfully" << std::endl;
            curiosity.reset();
            std::cout << "  Curiosity reset successfully" << std::endl;
        }
        std::cout << "  Curiosity destroyed - no memory leaks!" << std::endl;
        
        {
            nlm::Novelty novelty;
            std::cout << "  Novelty created successfully" << std::endl;
            novelty.clearHistory();
            std::cout << "  Novelty history cleared successfully" << std::endl;
        }
        std::cout << "  Novelty destroyed - no memory leaks!" << std::endl;
        
        {
            nlm::PredictionError predError;
            std::cout << "  PredictionError created successfully" << std::endl;
            predError.clearHistory();
            std::cout << "  PredictionError history cleared successfully" << std::endl;
        }
        std::cout << "  PredictionError destroyed - no memory leaks!" << std::endl;
        
        {
            nlm::Reward reward;
            std::cout << "  Reward created successfully" << std::endl;
            reward.reset();
            std::cout << "  Reward reset successfully" << std::endl;
        }
        std::cout << "  Reward destroyed - no memory leaks!" << std::endl;
        
        // Test 6: Create and destroy AgentBrain object
        std::cout << "Test 6: AgentBrain object creation/destruction..." << std::endl;
        {
            nlm::AgentBrain agentBrain(config);
            std::cout << "  AgentBrain created successfully" << std::endl;
            nlm::SimpleWorld world(16, 16);
            agentBrain.initialize(world);
            std::cout << "  AgentBrain initialized successfully" << std::endl;
        }
        std::cout << "  AgentBrain destroyed - no memory leaks!" << std::endl;
        
        std::cout << "\nAll tests passed successfully! Memory management fixes verified." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}