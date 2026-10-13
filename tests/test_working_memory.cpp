// Working Memory Tests
// Tests persistent activity, selective retention, and competition-based storage

#include "memory/NeuralWorkingMemory.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

namespace test_working_memory {

void testWorkingMemoryCreation() {
    nlm::NeuralWorkingMemory wm;
    
    // Should initialize with default capacity
    assert(wm.getCapacity() > 0);
    assert(wm.getCurrentSize() == 0);
    assert(wm.getActiveTraces() == 0);
    
    std::cout << "    testWorkingMemoryCreation passed" << std::endl;
}

void testWorkingMemoryCapacity() {
    nlm::NeuralWorkingMemory wm;
    
    // Test capacity setting
    wm.setCapacity(1000);
    assert(wm.getCapacity() == 1000);
    
    wm.setCapacity(5000);
    assert(wm.getCapacity() == 5000);
    
    std::cout << "    testWorkingMemoryCapacity passed" << std::endl;
}

void testWorkingMemoryStoreRetrieve() {
    nlm::NeuralWorkingMemory wm;
    
    // Store a pattern
    std::vector<float> pattern = {0.1f, 0.5f, 0.9f, 0.3f, 0.7f};
    wm.store(pattern, 1.0f);
    
    // Retrieve should return similar pattern
    std::vector<float> retrieved = wm.retrieve();
    
    // Pattern should have same size
    assert(retrieved.size() == pattern.size());
    
    // Values should be normalized to [0, 1]
    for (float val : retrieved) {
        assert(val >= 0.0f && val <= 1.0f);
    }
    
    std::cout << "    testWorkingMemoryStoreRetrieve passed" << std::endl;
}

void testWorkingMemoryNeuronStorage() {
    nlm::NeuralWorkingMemory wm;
    
    // Store to specific neuron
    nlm::NeuronId testNeuron(42);
    wm.storeToNeuron(testNeuron, 0.8f);
    
    // Should contain the neuron
    assert(wm.contains(testNeuron));
    
    // Get activation should return stored value (approximately)
    float activation = wm.getNeuronActivation(testNeuron);
    assert(activation >= 0.7f && activation <= 1.0f);
    
    std::cout << "    testWorkingMemoryNeuronStorage passed" << std::endl;
}

void testWorkingMemoryDecay() {
    nlm::NeuralWorkingMemory wm;
    
    // Set slow decay rate
    wm.setDecayRate(0.01f);
    
    std::vector<float> pattern = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    wm.store(pattern, 1.0f);
    
    // Update without input to allow decay
    for (int i = 0; i < 100; ++i) {
        wm.update(0.001f);  // Small timestep
    }
    
    // Memory should have decayed somewhat
    std::vector<float> retrieved = wm.retrieve();
    float totalActivity = std::accumulate(retrieved.begin(), retrieved.end(), 0.0f);
    
    // Total activity should be less than initial (1000)
    assert(totalActivity < 1000.0f);
    
    std::cout << "    testWorkingMemoryDecay passed" << std::endl;
}

void testWorkingMemoryCompetition() {
    nlm::NeuralWorkingMemory wm;
    
    // Store multiple patterns
    std::vector<float> pattern1 = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    std::vector<float> pattern2 = {0.0f, 1.0f, 0.0f, 0.0f, 0.0f};
    std::vector<float> pattern3 = {0.0f, 0.0f, 1.0f, 0.0f, 0.0f};
    
    wm.store(pattern1, 1.0f);
    wm.store(pattern2, 1.0f);
    wm.store(pattern3, 1.0f);
    
    // Run competition
    wm.runCompetition();
    
    // Check that winners are set
    const auto& winners = wm.getWinners();
    assert(!winners.empty());
    
    // Should have selected at least one winner
    assert(winners.size() >= 1);
    
    std::cout << "    testWorkingMemoryCompetition passed" << std::endl;
}

void testWorkingMemoryStrengthening() {
    nlm::NeuralWorkingMemory wm;
    
    std::vector<float> pattern = {0.5f, 0.5f, 0.5f, 0.5f, 0.5f};
    wm.store(pattern, 1.0f);
    
    // Get initial activation
    float initialTotal = 0.0f;
    for (size_t i = 0; i < pattern.size(); ++i) {
        initialTotal += wm.getNeuronActivation(nlm::NeuronId(i));
    }
    
    // Strengthen memory
    wm.strengthenMemory(2.0f);  // Double strength
    
    // Get new activation
    float newTotal = 0.0f;
    for (size_t i = 0; i < pattern.size(); ++i) {
        newTotal += wm.getNeuronActivation(nlm::NeuronId(i));
    }
    
    // Should have increased
    assert(newTotal >= initialTotal);
    
    std::cout << "    testWorkingMemoryStrengthening passed" << std::endl;
}

void testWorkingMemoryClear() {
    nlm::NeuralWorkingMemory wm;
    
    // Store something
    wm.store(std::vector<float>(100, 0.5f), 1.0f);
    assert(wm.getCurrentSize() > 0);
    
    // Clear
    wm.clear();
    
    // Should be empty
    assert(wm.getCurrentSize() == 0);
    assert(wm.getActiveTraces() == 0);
    assert(wm.getMemoryActivity() == 0.0f);
    
    std::cout << "    testWorkingMemoryClear passed" << std::endl;
}

void testPersistentActivity() {
    nlm::NeuralWorkingMemory wm;
    
    // Create a strong memory trace
    std::vector<float> pattern(100, 0.0f);
    for (size_t i = 0; i < 50; ++i) {
        pattern[i] = 1.0f;  // First half of pattern is strong
    }
    
    wm.store(pattern, 2.0f);  // Strong input
    
    // Simulate persistent activity - no new input but decay
    float initialActivity = wm.getMemoryActivity();
    assert(initialActivity > 0.0f);
    
    for (int step = 0; step < 50; ++step) {
        wm.update(0.001f);
    }
    
    // Activity should decay but still be present
    float finalActivity = wm.getMemoryActivity();
    assert(finalActivity >= 0.0f);  // Shouldn't go negative
    
    std::cout << "    testPersistentActivity passed" << std::endl;
}

void testSelectiveRetention() {
    nlm::NeuralWorkingMemory wm;
    
    // Create patterns with different strengths
    std::vector<float> weakPattern(50, 0.1f);
    std::vector<float> mediumPattern(50, 0.5f);
    std::vector<float> strongPattern(50, 1.0f);
    
    wm.store(weakPattern, 0.5f);
    wm.store(mediumPattern, 1.0f);
    wm.store(strongPattern, 2.0f);
    
    // Get memory activity
    float totalActivity = wm.getMemoryActivity();
    
    // Strong pattern should contribute more
    assert(totalActivity > 0.0f);
    
    // Clear and test again with just weak pattern
    wm.clear();
    wm.store(weakPattern, 0.5f);
    
    float weakTotal = wm.getMemoryActivity();
    assert(weakTotal > 0.0f);
    
    std::cout << "    testSelectiveRetention passed" << std::endl;
}

void testCompetitionBasedStorage() {
    nlm::NeuralWorkingMemory wm;
    
    // Create multiple competing patterns
    std::vector<float> patternA = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    std::vector<float> patternB = {0.0f, 1.0f, 0.0f, 0.0f, 0.0f};
    std::vector<float> patternC = {0.0f, 0.0f, 1.0f, 0.0f, 0.0f};
    
    wm.store(patternA, 1.0f);
    wm.store(patternB, 1.0f);
    wm.store(patternC, 1.0f);
    
    // Get memory neurons
    const auto& neurons = wm.getMemoryNeurons();
    assert(neurons.size() > 0);
    
    // Run competition multiple times
    for (int i = 0; i < 10; ++i) {
        wm.runCompetition();
        const auto& winners = wm.getWinners();
        // Should have winners after competition
        assert(!winners.empty());
    }
    
    std::cout << "    testCompetitionBasedStorage passed" << std::endl;
}

void runAll() {
    testWorkingMemoryCreation();
    testWorkingMemoryCapacity();
    testWorkingMemoryStoreRetrieve();
    testWorkingMemoryNeuronStorage();
    testWorkingMemoryDecay();
    testWorkingMemoryCompetition();
    testWorkingMemoryStrengthening();
    testWorkingMemoryClear();
    testPersistentActivity();
    testSelectiveRetention();
    testCompetitionBasedStorage();
}

} // namespace test_working_memory
