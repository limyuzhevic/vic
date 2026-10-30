// Memory System Tests
// Comprehensive tests for memory systems (Working, Episodic, Associative)

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_memory {

void testWorkingMemory() {
    std::cout << "    Testing Working Memory..." << std::endl;
    
    nlm::Config config;
    config.set("neuron_count", 1000);
    config.set("region_count", 1);
    
    auto brain = std::make_shared<nlm::Brain>(std::make_shared<nlm::Config>(config));
    
    if (!brain->initialize()) {
        throw std::runtime_error("Failed to initialize brain");
    }
    
    auto working_memory = brain->getWorkingMemory();
    assert(working_memory != nullptr);
    
    // Test working memory functionality
    // Note: This is a basic test - real tests would be more comprehensive
    float memory_utilization = 0.0f;
    size_t capacity = 100; // Example capacity
    
    // Simulate memory operations
    for (int i = 0; i < 50; ++i) {
        working_memory->store(nlm::WorkingMemoryItem(i, std::vector<float>(10, 1.0f)));
    }
    
    memory_utilization = working_memory->getUtilization();
    assert(memory_utilization > 0.0f);
    
    std::cout << "      PASSED" << std::endl;
}

void testEpisodicMemory() {
    std::cout << "    Testing Episodic Memory..." << std::endl;
    
    nlm::Config config;
    config.set("neuron_count", 1000);
    
    auto brain = std::make_shared<nlm::Brain>(std::make_shared<nlm::Config>(config));
    
    if (!brain->initialize()) {
        throw std::runtime_error("Failed to initialize brain");
    }
    
    auto episodic_memory = brain->getEpisodicMemory();
    assert(episodic_memory != nullptr);
    
    // Test episodic memory functionality
    // Create test episode
    nlm::Episode episode;
    episode.id = 1;
    episode.timestamp = 0.0;
    episode.action = nlm::Action();
    episode.reward = 1.0f;
    episode.observation = nlm::Observation();
    
    episodic_memory->storeEpisode(episode);
    
    // Test retrieval
    auto retrieved = episodic_memory->retrieveEpisode(1);
    assert(retrieved.has_value());
    assert(retrieved.value().id == 1);
    
    std::cout << "      PASSED" << std::endl;
}

void testAssociativeMemory() {
    std::cout << "    Testing Associative Memory..." << std::endl;
    
    nlm::Config config;
    config.set("neuron_count", 1000);
    
    auto brain = std::make_shared<nlm::Brain>(std::make_shared<nlm::Config>(config));
    
    if (!brain->initialize()) {
        throw std::runtime_error("Failed to initialize brain");
    }
    
    auto associative_memory = brain->getAssociativeMemory();
    assert(associative_memory != nullptr);
    
    // Test associative memory functionality
    std::vector<float> pattern1 = {1.0f, 0.5f, 0.0f, -0.5f, -1.0f};
    std::vector<float> pattern2 = {0.0f, 0.3f, 0.8f, 0.6f, 0.2f};
    
    associative_memory->associate(pattern1, pattern2);
    
    // Test recall
    auto recall = associative_memory->recall(pattern1);
    assert(recall.has_value());
    
    std::cout << "      PASSED" << std::endl;
}

void runAll() {
    testWorkingMemory();
    testEpisodicMemory();
    testAssociativeMemory();
}

} // namespace test_memory
