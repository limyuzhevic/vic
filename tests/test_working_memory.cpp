// Working Memory Tests
// Tests for WorkingMemory functionality including storage, retrieval, decay, and capacity management

#include "memory/Memory.hpp"
#include "core/Config/Config.hpp"
#include "TestInfrastructure.hpp"
#include "TestUtilities.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_working_memory {

// Helper function to create test neuron IDs
std::vector<nlm::NeuronId> createTestNeuronIds(size_t count) {
    std::vector<nlm::NeuronId> ids;
    for (size_t i = 0; i < count; ++i) {
        ids.push_back(nlm::NeuronId(i));
    }
    return ids;
}

void testWorkingMemoryCreation() {
    nlm::WorkingMemory memory;
    
    // Test basic properties
    assert(memory.getCapacity() > 0);
    assert(memory.getCurrentSize() == 0);
    
    std::cout << "    testWorkingMemoryCreation passed" << std::endl;
}

void testWorkingMemoryStoreAndRetrieve() {
    nlm::WorkingMemory memory;
    
    // Create test neurons
    nlm::NeuronId neuron1(1);
    nlm::NeuronId neuron2(2);
    nlm::NeuronId neuron3(3);
    
    // Store values
    memory.store(neuron1, 10.5f);
    memory.store(neuron2, 20.3f);
    memory.store(neuron3, 30.7f);
    
    // Retrieve values
    assert(nlm_test::utils::approxEqual(memory.retrieve(neuron1), 10.5f));
    assert(nlm_test::utils::approxEqual(memory.retrieve(neuron2), 20.3f));
    assert(nlm_test::utils::approxEqual(memory.retrieve(neuron3), 30.7f));
    
    // Test retrieval of non-existent neuron returns 0.0
    nlm::NeuronId neuron999(999);
    assert(nlm_test::utils::approxEqual(memory.retrieve(neuron999), 0.0f));
    
    std::cout << "    testWorkingMemoryStoreAndRetrieve passed" << std::endl;
}

void testWorkingMemoryContains() {
    nlm::WorkingMemory memory;
    
    nlm::NeuronId neuron1(1);
    nlm::NeuronId neuron2(2);
    
    // Initially contains nothing
    assert(!memory.contains(neuron1));
    assert(!memory.contains(neuron2));
    
    // Store and check contains
    memory.store(neuron1, 15.0f);
    assert(memory.contains(neuron1));
    assert(!memory.contains(neuron2));
    
    // Update existing neuron
    memory.store(neuron1, 25.0f);
    assert(memory.contains(neuron1));
    
    std::cout << "    testWorkingMemoryContains passed" << std::endl;
}

void testWorkingMemoryUpdateExisting() {
    nlm::WorkingMemory memory;
    
    nlm::NeuronId neuron1(1);
    nlm::NeuronId neuron2(2);
    
    // Store initial value
    memory.store(neuron1, 10.0f);
    assert(nlm_test::utils::approxEqual(memory.retrieve(neuron1), 10.0f));
    
    // Update with new value
    memory.store(neuron1, 50.0f);
    assert(nlm_test::utils::approxEqual(memory.retrieve(neuron1), 50.0f));
    
    // Different neuron should not be affected
    memory.store(neuron2, 20.0f);
    assert(nlm_test::utils::approxEqual(memory.retrieve(neuron2), 20.0f));
    
    std::cout << "    testWorkingMemoryUpdateExisting passed" << std::endl;
}

void testWorkingMemoryDecay() {
    nlm::WorkingMemory memory;
    
    nlm::NeuronId neuron1(1);
    nlm::NeuronId neuron2(2);
    
    // Store values
    memory.store(neuron1, 100.0f);
    memory.store(neuron2, 200.0f);
    
    // Apply decay
    memory.decay(0.1f); // 10% decay
    
    // Values should be decayed
    assert(nlm_test::utils::approxEqual(memory.retrieve(neuron1), 90.0f, 0.001f));
    assert(nlm_test::utils::approxEqual(memory.retrieve(neuron2), 180.0f, 0.001f));
    
    // Multiple decay calls
    memory.decay(0.2f); // Additional 20% decay
    assert(nlm_test::utils::approxEqual(memory.retrieve(neuron1), 72.0f, 0.001f)); // 100 * 0.9 * 0.8 = 72
    
    std::cout << "    testWorkingMemoryDecay passed" << std::endl;
}

void testWorkingMemoryCapacity() {
    nlm::WorkingMemory memory;
    
    size_t initialCapacity = memory.getCapacity();
    assert(initialCapacity > 0);
    
    // Store more items than capacity (should be limited)
    std::vector<nlm::NeuronId> neurons;
    for (size_t i = 0; i < initialCapacity * 2; ++i) {
        neurons.push_back(nlm::NeuronId(i));
        memory.store(neurons[i], static_cast<float>(i * 10));
    }
    
    // Current size should not exceed capacity
    assert(memory.getCurrentSize() <= initialCapacity);
    
    // At least some items should be stored
    assert(memory.getCurrentSize() > 0);
    
    std::cout << "    testWorkingMemoryCapacity passed" << std::endl;
}

void testWorkingMemoryClear() {
    nlm::WorkingMemory memory;
    
    // Store some values
    for (int i = 0; i < 10; ++i) {
        memory.store(nlm::NeuronId(i), static_cast<float>(i * 5));
    }
    
    assert(memory.getCurrentSize() > 0);
    
    // Clear memory
    memory.clear();
    assert(memory.getCurrentSize() == 0);
    
    // All retrievals should return 0.0
    for (int i = 0; i < 10; ++i) {
        assert(nlm_test::utils::approxEqual(memory.retrieve(nlm::NeuronId(i)), 0.0f));
    }
    
    std::cout << "    testWorkingMemoryClear passed" << std::endl;
}

void testWorkingMemoryIntegrationWithBrain() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::NeuronId testNeuron(42);
    float testValue = 77.7f;
    
    // Test that working memory is accessible through brain
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Store and retrieve through working memory
    workingMemory->store(testNeuron, testValue);
    assert(nlm_test::utils::approxEqual(workingMemory->retrieve(testNeuron), testValue));
    
    // Verify contains works
    assert(workingMemory->contains(testNeuron));
    
    // Test decay through brain interface
    workingMemory->decay(0.1f);
    assert(nlm_test::utils::approxEqual(workingMemory->retrieve(testNeuron), testValue * 0.9f, 0.001f));
    
    std::cout << "    testWorkingMemoryIntegrationWithBrain passed" << std::endl;
}

void testWorkingMemoryStress() {
    nlm::WorkingMemory memory;
    
    // Rapid store/retrieve operations
    for (size_t i = 0; i < 1000; ++i) {
        nlm::NeuronId neuron(i % 100); // Reuse 100 different neurons
        float value = static_cast<float>(i) * 0.1f;
        memory.store(neuron, value);
        assert(nlm_test::utils::approxEqual(memory.retrieve(neuron), value));
    }
    
    // Verify capacity constraints are maintained
    assert(memory.getCurrentSize() <= memory.getCapacity());
    
    std::cout << "    testWorkingMemoryStress passed" << std::endl;
}

void testWorkingMemoryPersistence() {
    nlm::WorkingMemory memory1;
    
    // Store some values
    for (int i = 0; i < 50; ++i) {
        memory1.store(nlm::NeuronId(i), static_cast<float>(i * 2));
    }
    
    // Create second memory
    nlm::WorkingMemory memory2;
    
    // Values should be independent
    assert(memory1.getCurrentSize() == 0); // memory2 is empty
    assert(memory2.getCurrentSize() > 0);   // memory1 has data
    
    // Store different values in memory2
    for (int i = 0; i < 30; ++i) {
        memory2.store(nlm::NeuronId(i), static_cast<float>(i * 3));
    }
    
    // Verify independence
    for (int i = 0; i < 30; ++i) {
        assert(nlm_test::utils::approxEqual(memory1.retrieve(nlm::NeuronId(i)), 0.0f));
        assert(nlm_test::utils::approxEqual(memory2.retrieve(nlm::NeuronId(i)), static_cast<float>(i * 3)));
    }
    
    std::cout << "    testWorkingMemoryPersistence passed" << std::endl;
}

void runAll() {
    nlm_test::TestRunner runner(true);
    
    runner.runTest("Working Memory Creation", testWorkingMemoryCreation);
    runner.runTest("Working Memory Store and Retrieve", testWorkingMemoryStoreAndRetrieve);
    runner.runTest("Working Memory Contains", testWorkingMemoryContains);
    runner.runTest("Working Memory Update Existing", testWorkingMemoryUpdateExisting);
    runner.runTest("Working Memory Decay", testWorkingMemoryDecay);
    runner.runTest("Working Memory Capacity", testWorkingMemoryCapacity);
    runner.runTest("Working Memory Clear", testWorkingMemoryClear);
    runner.runTest("Working Memory Integration With Brain", testWorkingMemoryIntegrationWithBrain);
    runner.runTest("Working Memory Stress Test", testWorkingMemoryStress);
    runner.runTest("Working Memory Persistence", testWorkingMemoryPersistence);
    
    runner.runAll();
}

} // namespace test_working_memory
