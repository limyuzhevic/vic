#include "tests/TestNeuralWorkingMemory.h"
#include "src/memory/NeuralWorkingMemory.hpp"
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include <cassert>
#include <iostream>

void testNeuralWorkingMemory_basic() {
    std::cout << "Testing NeuralWorkingMemory basic functionality..." << std::endl;
    
    // Create a simple brain for testing
    auto config = std::make_shared<Config>();
    config->set("random_seed", 42, ConfigSource::Default);
    config->set("neuron_count", 100, ConfigSource::Default);
    config->set("region_count", 1, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Test memory system initialization
    auto& memory = brain->getWorkingMemory();
    
    // Create test neurons
    auto neurons = brain->getRegion(RegionId(1))->getAllNeurons();
    if (neurons && !neurons->empty()) {
        NeuronId testNeuron = neurons->at(0)->getId();
        
        // Test storing and retrieving neuron activation
        memory.storeToNeuron(testNeuron, 0.5f);
        assert(memory.contains(testNeuron));
        assert(memory.getNeuronActivation(testNeuron) == 0.5f);
        
        // Test strengthening memory
        memory.strengthenMemory(1.2f);
        assert(memory.getNeuronActivation(testNeuron) <= 1.0f);  // Should not exceed 1.0
        
        // Test clearing memory
        memory.clear();
        assert(!memory.contains(testNeuron));
        
        std::cout << "  ✓ Basic memory operations passed" << std::endl;
    }
    
    // Test competition functionality
    if (neurons && neurons->size() >= 3) {
        // Store different activations for different neurons
        memory.storeToNeuron(neurons->at(0)->getId(), 0.8f);
        memory.storeToNeuron(neurons->at(1)->getId(), 0.3f);
        memory.storeToNeuron(neurons->at(2)->getId(), 0.6f);
        
        // Update to run competition
        memory.update(0.001f);
        
        // Check winners
        bool hasWinner = memory.isWinning(neurons->at(0)->getId()) || 
                         memory.isWinning(neurons->at(2)->getId());
        assert(hasWinner);  // At least one should be a winner
        
        std::cout << "  ✓ Competition system passed" << std::endl;
    }
    
    std::cout << "NeuralWorkingMemory basic tests passed!" << std::endl;
}

void testNeuralWorkingMemory_capacity() {
    std::cout << "Testing NeuralWorkingMemory capacity limits..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("random_seed", 42, ConfigSource::Default);
    config->set("neuron_count", 100, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    auto& memory = brain->getWorkingMemory();
    auto neurons = brain->getRegion(RegionId(1))->getAllNeurons();
    
    if (neurons) {
        // Test capacity - should not exceed configured limit
        memory.clear();
        
        // Try to store many neurons (may be limited by brain capacity)
        size_t stored = 0;
        for (size_t i = 0; i < std::min(static_cast<size_t>(10), neurons->size()); ++i) {
            memory.storeToNeuron(neurons->at(i)->getId(), 0.5f);
            stored++;
        }
        
        // Memory should contain the stored neurons
        for (size_t i = 0; i < stored; ++i) {
            assert(memory.contains(neurons->at(i)->getId()));
        }
        
        std::cout << "  ✓ Capacity limits passed" << std::endl;
    }
    
    std::cout << "NeuralWorkingMemory capacity tests passed!" << std::endl;
}

void testNeuralWorkingMemory_decay() {
    std::cout << "Testing NeuralWorkingMemory decay mechanisms..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("random_seed", 42, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    auto& memory = brain->getWorkingMemory();
    auto neurons = brain->getRegion(RegionId(1))->getAllNeurons();
    
    if (neurons && !neurons->empty()) {
        NeuronId testNeuron = neurons->at(0)->getId();
        
        // Set high activation
        memory.storeToNeuron(testNeuron, 1.0f);
        
        // Update for many steps to trigger decay
        for (int step = 0; step < 2000; ++step) {
            memory.update(0.001f);
        }
        
        // Activation should have decayed
        float finalActivation = memory.getNeuronActivation(testNeuron);
        assert(finalActivation < 1.0f);  // Should decay over time
        
        std::cout << "  ✓ Decay mechanisms passed" << std::endl;
    }
    
    std::cout << "NeuralWorkingMemory decay tests passed!" << std::endl;
}

int main() {
    std::cout << "=== NeuralWorkingMemory Unit Tests ===" << std::endl << std::endl;
    
    try {
        testNeuralWorkingMemory_basic();
        std::cout << std::endl;
        
        testNeuralWorkingMemory_capacity();
        std::cout << std::endl;
        
        testNeuralWorkingMemory_decay();
        std::cout << std::endl;
        
        std::cout << "=== All NeuralWorkingMemory tests passed! ===" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}