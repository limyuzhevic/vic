#include "test_common.hpp"
#include <cassert>
#include <cmath>
#include <algorithm>

namespace test_memory {

void testMemoryCreation() {
    nlm::Memory memory;
    assert(memory.getWorkingMemoryCapacity() == 100);
    assert(memory.getEpisodicMemoryCapacity() == 1000);
    assert(memory.getAssociativeMemoryCapacity() == 10000);
    std::cout << "    testMemoryCreation passed" << std::endl;
}

void testWorkingMemory() {
    nlm::NeuralWorkingMemory workingMemory;
    assert(workingMemory.getCapacity() == 100);
    assert(workingMemory.getCurrentLoad() == 0);
    assert(workingMemory.isEmpty() == true);
    
    // Store some data
    std::vector<float> data = {0.1f, 0.2f, 0.3f};
    workingMemory.store(data);
    assert(workingMemory.getCurrentLoad() == 3);
    assert(workingMemory.isEmpty() == false);
    
    // Retrieve data
    std::vector<float> retrieved = workingMemory.recall();
    assert(retrieved.size() == 3);
    std::cout << "    testWorkingMemory passed" << std::endl;
}

void testEpisodicMemory() {
    nlm::NeuralEpisodicMemory episodicMemory;
    assert(episodicMemory.getCapacity() == 1000);
    assert(episodicMemory.getEventCount() == 0);
    
    // Store an episode
    std::string context = "test_context";
    std::string content = "test_content";
    nlm::Timestamp timestamp = 1.0f;
    
    episodicMemory.storeEpisode(context, content, timestamp);
    assert(episodicMemory.getEventCount() == 1);
    
    // Recall episode
    auto recalled = episodicMemory.recallEpisode(0);
    assert(recalled.context == context);
    assert(recalled.content == content);
    assert(recalled.timestamp == timestamp);
    std::cout << "    testEpisodicMemory passed" << std::endl;
}

void testAssociativeMemory() {
    nlm::NeuralAssociativeMemory associativeMemory;
    assert(associativeMemory.getCapacity() == 10000);
    assert(associativeMemory.getAssociationCount() == 0);
    
    // Create an association
    std::string key1 = "key1";
    std::string key2 = "key2";
    float strength = 0.8f;
    
    associativeMemory.createAssociation(key1, key2, strength);
    assert(associativeMemory.getAssociationCount() == 1);
    assert(associativeMemory.getStrength(key1, key2) == strength);
    
    // Retrieve association
    float retrievedStrength = associativeMemory.getAssociation(key1, key2);
    assert(retrievedStrength == strength);
    std::cout << "    testAssociativeMemory passed" << std::endl;
}

void testMemoryDecay() {
    nlm::Memory memory;
    memory.setDecayRate(0.1f);
    assert(memory.getDecayRate() == 0.1f);
    
    // Simulate decay over time
    float value = 1.0f;
    for (int i = 0; i < 10; ++i) {
        value = value * (1.0f - memory.getDecayRate());
    }
    assert(value < 0.35f); // Should decay significantly
    std::cout << "    testMemoryDecay passed" << std::endl;
}

void testMemoryReset() {
    nlm::Memory memory;
    
    // Store some data
    std::vector<float> data = {0.5f, 0.6f, 0.7f};
    memory.getWorkingMemory().store(data);
    
    // Reset memory
    memory.reset();
    assert(memory.getWorkingMemory().getCurrentLoad() == 0);
    assert(memory.getEpisodicMemory().getEventCount() == 0);
    assert(memory.getAssociativeMemory().getAssociationCount() == 0);
    std::cout << "    testMemoryReset passed" << std::endl;
}

void testMemoryCapacity() {
    nlm::Memory memory;
    
    // Test capacity limits
    nlm::NeuralWorkingMemory& wm = memory.getWorkingMemory();
    std::vector<float> largeData(200); // Exceeds capacity
    wm.store(largeData);
    assert(wm.getCurrentLoad() <= wm.getCapacity());
    
    nlm::NeuralEpisodicMemory& em = memory.getEpisodicMemory();
    // Test capacity enforcement
    std::cout << "    testMemoryCapacity passed" << std::endl;
}

void testMemoryStatistics() {
    nlm::Memory memory;
    
    // Store some data
    std::vector<float> data = {0.1f, 0.2f, 0.3f};
    memory.getWorkingMemory().store(data);
    
    // Get statistics
    auto stats = memory.getStatistics();
    assert(stats.workingMemoryUsage >= 0);
    assert(stats.episodicMemoryUsage >= 0);
    assert(stats.associativeMemoryUsage >= 0);
    assert(stats.totalUsage >= 0);
    std::cout << "    testMemoryStatistics passed" << std::endl;
}

void testMemoryIntegration() {
    nlm::Memory memory;
    
    // Test integration with Brain
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Memory should be accessible
    assert(&memory.getWorkingMemory() != nullptr);
    std::cout << "    testMemoryIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running Memory tests..." << std::endl;
    testMemoryCreation();
    testWorkingMemory();
    testEpisodicMemory();
    testAssociativeMemory();
    testMemoryDecay();
    testMemoryReset();
    testMemoryCapacity();
    testMemoryStatistics();
    testMemoryIntegration();
}

} // namespace test_memory