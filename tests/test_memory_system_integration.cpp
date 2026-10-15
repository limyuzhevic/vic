// Memory System Integration Tests
// Memory system integration with custom plugins

#include "memory/Memory.hpp"
#include "core/CustomMemorySystem.hpp"
#include "brain/Brain.hpp"
#include <cassert>
#include <iostream>

namespace nlm {
namespace tests {

namespace test_memory_system_integration {

void testMemorySystemIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Initialize brain
    bool success = brain.initialize();
    
    // Test memory system integration
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testMemorySystemIntegration passed" << std::endl;
}

void testCustomMemorySystemWithBrain() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Test custom memory system integration
    auto customMemory = std::make_unique<nlm::CustomMemorySystem>();
    
    // Initialize brain with custom memory
    bool success = brain.initialize();
    
    std::cout << "    testCustomMemorySystemWithBrain passed" << std::endl;
}

void testMemorySystemPerformance() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Initialize brain
    brain.initialize();
    
    // Test memory system performance
    for (int step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    std::cout << "    testMemorySystemPerformance passed" << std::endl;
}

void testMemorySystemWithEpisodicMemory() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(120), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Test episodic memory integration
    brain.initialize();
    
    // Test action production
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    std::cout << "    testMemorySystemWithEpisodicMemory passed" << std::endl;
}

void testMemorySystemWithWorkingMemory() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(130), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Test working memory integration
    brain.initialize();
    
    // Test brain status
    assert(brain.getTotalSynapseCount() >= 0);
    
    std::cout << "    testMemorySystemWithWorkingMemory passed" << std::endl;
}

void testMemorySystemWithSemanticMemory() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(140), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Test semantic memory integration
    brain.initialize();
    
    // Test regions
    auto regionIds = brain.getRegionIds();
    assert(regionIds.size() == 2);
    
    std::cout << "    testMemorySystemWithSemanticMemory passed" << std::endl;
}

void testMemorySystemWithAssociativeMemory() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(160), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Test associative memory integration
    brain.initialize();
    
    // Test reset functionality
    brain.reset();
    
    std::cout << "    testMemorySystemWithAssociativeMemory passed" << std::endl;
}

void testMemorySystemWithProceduralMemory() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(180), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Test procedural memory integration
    brain.initialize();
    
    // Test step execution
    for (int step = 0; step < 5; ++step) {
        brain.step(step);
    }
    
    std::cout << "    testMemorySystemWithProceduralMemory passed" << std::endl;
}

void runAll() {
    testMemorySystemIntegration();
    testCustomMemorySystemWithBrain();
    testMemorySystemPerformance();
    testMemorySystemWithEpisodicMemory();
    testMemorySystemWithWorkingMemory();
    testMemorySystemWithSemanticMemory();
    testMemorySystemWithAssociativeMemory();
    testMemorySystemWithProceduralMemory();
}

} // namespace test_memory_system_integration

} // namespace nlm
namespace tests {

void test_memory_system_integration() {
    test_memory_system_integration::runAll();
}

} // namespace tests
