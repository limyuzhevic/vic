// Working Memory Integration Tests
// Tests neural working memory integration with the brain processing system

#include "memory/NeuralWorkingMemory.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <vector>

namespace test_working_memory_integration {

void testWorkingMemoryStoreRetrieve() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::NeuralWorkingMemory memory;
    memory.initialize(&brain);
    memory.setCapacity(50);
    memory.setDecayRate(0.01f);
    
    // Test storing pattern
    std::vector<float> pattern{0.5f, 0.8f, 0.3f, 0.9f, 0.2f};
    memory.store(pattern, 1.0f);
    
    // Test retrieving pattern
    auto retrieved = memory.retrieve();
    assert(retrieved.size() == pattern.size());
    for (size_t i = 0; i < pattern.size(); ++i) {
        assert(std::abs(retrieved[i] - pattern[i]) < 0.01f);
    }
    
    std::cout << "    testWorkingMemoryStoreRetrieve passed" << std::endl;
}

void testPersistentActivity() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::NeuralWorkingMemory memory;
    memory.initialize(&brain);
    memory.setCapacity(100);
    memory.setDecayRate(0.005f);
    
    // Store multiple patterns
    for (int i = 0; i < 10; ++i) {
        std::vector<float> pattern;
        for (int j = 0; j < 5; ++j) {
            pattern.push_back(static_cast<float>((i + j) % 10) / 10.0f);
        }
        memory.store(pattern, 0.8f);
    }
    
    // Verify patterns maintain over multiple updates
    auto initialState = memory.retrieve();
    for (int step = 0; step < 50; ++step) {
        memory.update(0.1f);  // Update with decay
        auto currentState = memory.retrieve();
        assert(currentState.size() == initialState.size());
    }
    
    std::cout << "    testPersistentActivity passed" << std::endl;
}

void testNeuralDynamics() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::NeuralWorkingMemory memory;
    memory.initialize(&brain);
    memory.setCapacity(75);
    memory.setDecayRate(0.02f);
    
    // Test store to specific neurons
    brain.initializeRegions(); // Ensure regions are initialized
    size_t totalNeurons = brain.getTotalNeuronCount();
    if (totalNeurons > 0) {
        NeuronId testNeuron{0};
        memory.storeToNeuron(testNeuron, 0.7f);
        assert(memory.getNeuronActivation(testNeuron) > 0.6f);
    }
    
    // Test competition mechanism
    memory.clear();
    std::vector<float> strongPattern{0.9f, 0.8f, 0.7f};
    std::vector<float> weakPattern{0.3f, 0.2f, 0.1f};
    
    memory.store(strongPattern, 1.0f);
    memory.store(weakPattern, 0.5f);
    
    memory.runCompetition();
    
    // Strong pattern should be winning
    bool hasWinners = !memory.getMemoryNeurons().empty();
    assert(hasWinners);
    
    std::cout << "    testNeuralDynamics passed" << std::endl;
}

void testMemoryIntegrationWithBrain() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::NeuralWorkingMemory memory;
    memory.initialize(&brain);
    
    // Store information and verify brain integration
    std::vector<float> pattern{0.6f, 0.7f, 0.8f, 0.5f, 0.9f};
    memory.store(pattern, 1.0f);
    
    // Verify memory affects brain state
    float memoryActivity = memory.getMemoryActivity();
    assert(memoryActivity > 0.0f);
    
    // Test working memory capacity limits
    memory.setCapacity(10);
    assert(memory.getCapacity() == 10);
    
    std::cout << "    testMemoryIntegrationWithBrain passed" << std::endl;
}

void runAll() {
    testWorkingMemoryStoreRetrieve();
    testPersistentActivity();
    testNeuralDynamics();
    testMemoryIntegrationWithBrain();
}

} // namespace test_working_memory_integration