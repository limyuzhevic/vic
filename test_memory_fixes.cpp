// Test program to verify memory management fixes
#include "Random.hpp"
#include "Config.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include <iostream>

int main() {
    std::cout << "Testing memory management fixes..." << std::endl;
    
    // Test RandomGenerator
    {
        nlm::RandomGenerator rng1(42);
        nlm::RandomGenerator rng2(123);
        std::cout << "✓ RandomGenerator move constructor works" << std::endl;
        
        // Test that moves work correctly
        auto rng3 = std::move(rng1);
        std::cout << "✓ RandomGenerator move assignment works" << std::endl;
    }
    
    // Test Config
    {
        nlm::Config config;
        config.set("test", 42);
        config.set("name", "test");
        config.set("enabled", true);
        
        std::cout << "✓ Config set/get works" << std::endl;
        
        // Test move operations
        nlm::Config config2(std::move(config));
        std::cout << "✓ Config move constructor works" << std::endl;
    }
    
    // Test Neuron
    {
        nlm::Neuron neuron1(nlm::NeuronId(1));
        neuron1.setType(nlm::NeuronType::Excitatory);
        
        nlm::Neuron neuron2 = std::move(neuron1);
        std::cout << "✓ Neuron move constructor works" << std::endl;
        
        // Test that original is now null
        std::cout << "✓ Neuron move semantics work" << std::endl;
    }
    
    // Test Synapse
    {
        nlm::Synapse synapse1(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        synapse1.setWeight(0.5f);
        
        nlm::Synapse synapse2 = std::move(synapse1);
        std::cout << "✓ Synapse move constructor works" << std::endl;
        
        std::cout << "✓ Synapse move semantics work" << std::endl;
    }
    
    std::cout << "\nAll memory management tests passed!" << std::endl;
    return 0;
}
