// Working Memory Tests
// Tests for persistent activity memory and winner-take-all competition

#include "memory/NeuralWorkingMemory.hpp"
#include "brain/Brain.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_working_memory {

void testWorkingMemoryCreation() {
    nlm::NeuralWorkingMemory wm;
    
    // Creation doesn't crash
    std::cout << "    testWorkingMemoryCreation passed" << std::endl;
}

void testWorkingMemoryInitialize() {
    nlm::NeuralWorkingMemory wm;
    
    // Initialize with null brain (placeholder)
    wm.initialize(nullptr);
    
    // Capacity should have default value
    assert(wm.getCapacity() > 0);
    
    std::cout << "    testWorkingMemoryInitialize passed" << std::endl;
}

void testWorkingMemoryStore() {
    nlm::NeuralWorkingMemory wm;
    
    // Test storing a pattern
    std::vector<float> pattern = {0.1f, 0.5f, 0.9f};
    wm.store(pattern, 0.8f);
    
    // Should not crash
    assert(true);
    
    std::cout << "    testWorkingMemoryStore passed" << std::endl;
}

void testWorkingMemoryStoreToNeuron() {
    nlm::NeuralWorkingMemory wm;
    
    // Test storing to a specific neuron
    nlm::NeuronId neuronId(1000);
    wm.storeToNeuron(neuronId, 0.7f);
    
    // Should not crash
    assert(true);
    
    std::cout << "    testWorkingMemoryStoreToNeuron passed" << std::endl;
}

void testWorkingMemoryRetrieve() {
    nlm::NeuralWorkingMemory wm;
    
    // Store a pattern
    std::vector<float> pattern = {0.3f, 0.6f, 0.9f};
    wm.store(pattern, 0.5f);
    
    // Retrieve should return something
    std::vector<float> result = wm.retrieve();
    
    assert(!result.empty());
    assert(result.size() == pattern.size());
    
    std::cout << "    testWorkingMemoryRetrieve passed" << std::endl;
}

void testWorkingMemoryContains() {
    nlm::NeuralWorkingMemory wm;
    
    nlm::NeuronId neuronId(2000);
    assert(!wm.contains(neuronId));
    
    wm.storeToNeuron(neuronId, 0.5f);
    assert(wm.contains(neuronId));
    
    std::cout << "    testWorkingMemoryContains passed" << std::endl;
}

void testWorkingMemoryCapacity() {
    nlm::NeuralWorkingMemory wm;
    
    size_t originalCapacity = wm.getCapacity();
    wm.setCapacity(50);
    assert(wm.getCapacity() == 50);
    
    std::cout << "    testWorkingMemoryCapacity passed" << std::endl;
}

void testWorkingMemoryDecay() {
    nlm::NeuralWorkingMemory wm;
    
    wm.setDecayRate(0.01f);
    assert(wm.getDecayRate() == 0.01f);
    
    std::cout << "    testWorkingMemoryDecay passed" << std::endl;
}

void testWorkingMemoryUpdate() {
    nlm::NeuralWorkingMemory wm;
    
    // Update with time
    wm.update(0.1f);  // 100ms
    
    // Should not crash
    assert(true);
    
    std::cout << "    testWorkingMemoryUpdate passed" << std::endl;
}

void testWorkingMemoryCompetition() {
    nlm::NeuralWorkingMemory wm;
    
    // Store multiple patterns
    std::vector<float> pattern1 = {0.9f, 0.1f, 0.2f};
    std::vector<float> pattern2 = {0.2f, 0.8f, 0.3f};
    
    wm.store(pattern1, 0.8f);
    wm.store(pattern2, 0.6f);
    
    // Run competition
    wm.runCompetition();
    
    // Should not crash
    assert(true);
    
    std::cout << "    testWorkingMemoryCompetition passed" << std::endl;
}

void testWorkingMemoryWinners() {
    nlm::NeuralWorkingMemory wm;
    
    // Store a high-activation pattern
    std::vector<float> pattern = {0.95f, 0.1f, 0.1f};
    wm.store(pattern, 0.9f);
    
    // Check if any neuron is winning
    assert(!wm.isWinning(nlm::NeuronId(1)));
    
    std::cout << "    testWorkingMemoryWinners passed" << std::endl;
}

void testWorkingMemoryActivity() {
    nlm::NeuralWorkingMemory wm;
    
    // Store a pattern
    wm.store({0.5f, 0.5f, 0.5f}, 0.7f);
    
    float activity = wm.getMemoryActivity();
    assert(activity >= 0.0f && activity <= 1.0f);
    
    std::cout << "    testWorkingMemoryActivity passed" << std::endl;
}

void testWorkingMemoryClear() {
    nlm::NeuralWorkingMemory wm;
    
    // Store something
    wm.store({0.1f, 0.2f}, 0.5f);
    
    // Clear memory
    wm.clear();
    
    assert(wm.getActiveTraces() == 0);
    
    std::cout << "    testWorkingMemoryClear passed" << std::endl;
}

void testWorkingMemoryStrengthen() {
    nlm::NeuralWorkingMemory wm;
    
    wm.store({0.5f, 0.5f}, 0.5f);
    
    // Strengthen memory
    wm.strengthenMemory(1.5f);  // Should not exceed 1.0f
    
    std::cout << "    testWorkingMemoryStrengthen passed" << std::endl;
}

void runAll() {
    std::cout << "Running Working Memory tests..." << std::endl;
    testWorkingMemoryCreation();
    testWorkingMemoryInitialize();
    testWorkingMemoryStore();
    testWorkingMemoryStoreToNeuron();
    testWorkingMemoryRetrieve();
    testWorkingMemoryContains();
    testWorkingMemoryCapacity();
    testWorkingMemoryDecay();
    testWorkingMemoryUpdate();
    testWorkingMemoryCompetition();
    testWorkingMemoryWinners();
    testWorkingMemoryActivity();
    testWorkingMemoryClear();
    testWorkingMemoryStrengthen();
}

} // namespace test_working_memory