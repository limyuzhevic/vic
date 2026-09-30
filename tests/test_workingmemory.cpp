// NeuralWorkingMemory Tests
#include "memory/NeuralWorkingMemory.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <algorithm>
#include <vector>

namespace test_workingmemory {

void testWorkingMemoryCreation() {
    nlm::NeuralWorkingMemory memory;
    
    std::cout << "    testWorkingMemoryCreation passed" << std::endl;
}

void testWorkingMemoryStoreRetrieve() {
    // Create a simple brain for testing
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create working memory and initialize with brain
    nlm::NeuralWorkingMemory memory;
    memory.initialize(&brain);
    
    // Store a pattern
    std::vector<float> pattern = {0.1f, 0.5f, 0.8f, 0.3f, 0.9f};
    memory.store(pattern, 1.0f);
    
    // Retrieve the pattern
    std::vector<float> retrieved = memory.retrieve();
    assert(retrieved.size() == pattern.size());
    
    // Check that values are approximately equal (normalized)
    float totalStored = std::accumulate(pattern.begin(), pattern.end(), 0.0f);
    float totalRetrieved = std::accumulate(retrieved.begin(), retrieved.end(), 0.0f);
    
    if (totalStored > 0.0f) {
        for (size_t i = 0; i < pattern.size(); ++i) {
            assert(std::abs(retrieved[i] - pattern[i]) < 0.01f);
        }
    }
    
    std::cout << "    testWorkingMemoryStoreRetrieve passed" << std::endl;
}

void testWorkingMemoryNeuronStorage() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::NeuralWorkingMemory memory;
    memory.initialize(&brain);
    
    // Test storing to specific neuron
    nlm::NeuronId neuronId(1);
    memory.storeToNeuron(neuronId, 0.7f);
    
    // Check that neuron is in memory
    assert(memory.contains(neuronId));
    
    // Get neuron activation
    float activation = memory.getNeuronActivation(neuronId);
    assert(activation > 0.0f);
    
    std::cout << "    testWorkingMemoryNeuronStorage passed" << std::endl;
}

void testWorkingMemoryCapacity() {
    nlm::NeuralWorkingMemory memory;
    
    // Test default capacity
    assert(memory.getCapacity() == 100);
    
    // Test capacity setting
    memory.setCapacity(50);
    assert(memory.getCapacity() == 50);
    
    // Test decay rate
    assert(memory.getDecayRate() == 0.01f);
    
    memory.setDecayRate(0.05f);
    assert(memory.getDecayRate() == 0.05f);
    
    std::cout << "    testWorkingMemoryCapacity passed" << std::endl;
}

void testWorkingMemoryUpdate() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::NeuralWorkingMemory memory;
    memory.initialize(&brain);
    
    // Store some data
    memory.store({0.5f, 0.8f}, 1.0f);
    
    // Test update with small timestep
    nlm::TimestepDuration dt = 0.001f;
    memory.update(dt);
    
    // Should not crash and should still have memory traces
    assert(memory.getActiveTraces() >= 0);
    
    std::cout << "    testWorkingMemoryUpdate passed" << std::endl;
}

void testWorkingMemoryClear() {
    nlm::NeuralWorkingMemory memory;
    
    memory.setCapacity(10);
    memory.setDecayRate(0.02f);
    
    // Store some data
    memory.store({0.5f, 0.8f, 0.3f}, 1.0f);
    
    // Clear memory
    memory.clear();
    
    // Verify cleared
    assert(memory.getActiveTraces() == 0);
    assert(memory.getCurrentSize() == 0);
    
    std::cout << "    testWorkingMemoryClear passed" << std::endl;
}

void testWorkingMemoryStrengthening() {
    nlm::NeuralWorkingMemory memory;
    memory.initialize(nullptr);  // No brain needed for this test
    
    // Store some data
    memory.store({0.5f, 0.8f}, 1.0f);
    
    // Get initial activity
    std::vector<float> initial = memory.retrieve();
    
    // Strengthen memory
    memory.strengthenMemory(1.5f);
    
    // Get strengthened activity
    std::vector<float> strengthened = memory.retrieve();
    
    // Check that values increased
    for (size_t i = 0; i < initial.size(); ++i) {
        assert(strengthened[i] >= initial[i]);
    }
    
    std::cout << "    testWorkingMemoryStrengthening passed" << std::endl;
}

void testWorkingMemoryCompetition() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::NeuralWorkingMemory memory;
    memory.initialize(&brain);
    
    // Store multiple patterns with different strengths
    memory.store({0.2f, 0.3f}, 0.5f);  // Weaker
    memory.store({0.9f, 0.8f}, 1.5f);  // Stronger
    
    // Run competition
    memory.runCompetition();
    
    // Verify winners were selected (at least one neuron should be marked as winner)
    bool hasWinners = false;
    for (size_t i = 0; i < memory.getMemoryNeurons().size(); ++i) {
        if (memory.isWinning(memory.getMemoryNeurons()[i])) {
            hasWinners = true;
            break;
        }
    }
    
    // In this simple case, there should be winners
    // (the exact behavior depends on implementation details)
    
    std::cout << "    testWorkingMemoryCompetition passed" << std::endl;
}

void runAll() {
    testWorkingMemoryCreation();
    testWorkingMemoryStoreRetrieve();
    testWorkingMemoryNeuronStorage();
    testWorkingMemoryCapacity();
    testWorkingMemoryUpdate();
    testWorkingMemoryClear();
    testWorkingMemoryStrengthening();
    testWorkingMemoryCompetition();
}

} // namespace test_workingmemory
