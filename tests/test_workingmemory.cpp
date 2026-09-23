// WorkingMemory Tests
#include "brain/NeuralWorkingMemory.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>

namespace test_workingmemory {

void testWorkingMemoryCreation() {
    nlm::NeuralWorkingMemory workingMem;
    
    size_t capacity = workingMem.getCapacity();
    assert(capacity > 0);
    
    float decayRate = workingMem.getDecayRate();
    assert(decayRate >= 0.0f && decayRate <= 1.0f);
    
    size_t activeTraces = workingMem.getActiveTraces();
    assert(activeTraces == 0);  // Should start empty
    
    std::cout << "    testWorkingMemoryCreation passed" << std::endl;
}

void testWorkingMemoryConfiguration() {
    nlm::NeuralWorkingMemory workingMem;
    
    // Set capacity
    workingMem.setCapacity(256);
    assert(workingMem.getCapacity() == 256);
    
    // Set decay rate
    workingMem.setDecayRate(0.01f);
    assert(workingMem.getDecayRate() == 0.01f);
    
    // Test boundary values
    workingMem.setCapacity(0);
    assert(workingMem.getCapacity() == 0);
    
    workingMem.setDecayRate(1.0f);
    assert(workingMem.getDecayRate() == 1.0f);
    
    std::cout << "    testWorkingMemoryConfiguration passed" << std::endl;
}

void testWorkingMemoryStoreRetrieve() {
    nlm::NeuralWorkingMemory workingMem;
    
    // Create a test pattern
    std::vector<float> pattern = {0.1f, 0.5f, 0.3f, 0.8f, 0.2f};
    
    // Store the pattern
    workingMem.store(pattern, 0.7f);
    
    // Verify memory contains the neurons
    bool contains = workingMem.contains(0);  // Should not contain 0 since pattern is small
    assert(!contains);  // This is expected since we don't know neuron mapping
    
    // Test store to specific neuron (if neuron 0 is valid)
    workingMem.storeToNeuron(0, 0.9f);
    
    // Get activation
    float activation = workingMem.getNeuronActivation(0);
    assert(activation >= 0.0f && activation <= 1.0f);
    
    std::cout << "    testWorkingMemoryStoreRetrieve passed" << std::endl;
}

void testWorkingMemoryUpdate() {
    nlm::NeuralWorkingMemory workingMem;
    
    // Store a pattern
    std::vector<float> pattern = {0.5f, 0.6f, 0.7f};
    workingMem.store(pattern, 0.8f);
    
    // Update with different time steps
    workingMem.update(0.001);
    workingMem.update(0.01);
    workingMem.update(0.1);
    
    // Should not crash
    std::cout << "    testWorkingMemoryUpdate passed" << std::endl;
}

void testWorkingMemoryClear() {
    nlm::NeuralWorkingMemory workingMem;
    
    // Store some data
    std::vector<float> pattern = {0.5f, 0.6f, 0.7f};
    workingMem.store(pattern, 0.8f);
    
    // Clear should work
    workingMem.clear();
    
    std::cout << "    testWorkingMemoryClear passed" << std::endl;
}

void testWorkingMemoryStrengthening() {
    nlm::NeuralWorkingMemory workingMem;
    
    // Store a pattern
    std::vector<float> pattern = {0.5f, 0.6f, 0.7f};
    workingMem.store(pattern, 0.8f);
    
    // Strengthen memory
    workingMem.strengthenMemory(1.5f);
    
    std::cout << "    testWorkingMemoryStrengthening passed" << std::endl;
}

void testWorkingMemoryCompetition() {
    nlm::NeuralWorkingMemory workingMem;
    
    // Store multiple patterns
    std::vector<float> pattern1 = {0.5f, 0.6f, 0.7f};
    std::vector<float> pattern2 = {0.8f, 0.9f, 0.1f};
    
    workingMem.store(pattern1, 0.8f);
    workingMem.store(pattern2, 0.9f);
    
    // Run competition
    workingMem.runCompetition();
    
    // Check if any neuron is winning
    bool isWinning = workingMem.isWinning(0);
    // Not asserting, just verifying it doesn't crash
    
    std::cout << "    testWorkingMemoryCompetition passed" << std::endl;
}

void runAll() {
    std::cout << "Running WorkingMemory tests..." << std::endl;
    testWorkingMemoryCreation();
    testWorkingMemoryConfiguration();
    testWorkingMemoryStoreRetrieve();
    testWorkingMemoryUpdate();
    testWorkingMemoryClear();
    testWorkingMemoryStrengthening();
    testWorkingMemoryCompetition();
}

} // namespace test_workingmemory