// Memory System Integration Tests
// Tests integration of memory systems with the brain

#include "integration/test_framework.cpp"

namespace test_memory_integration {

bool testWorkingMemoryIntegration() {
    auto config = IntegrationTestFramework::createDefaultConfig();
    config->set("memory_capacity", 1000, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    REQUIRE(success);
    
    // Verify working memory is initialized
    REQUIRE(brain.getWorkingMemory() != nullptr);
    
    // Test working memory functionality
    brain.getWorkingMemory()->storeToNeuron(1, 0.5f);
    float retrieved = brain.getWorkingMemory()->retrieve(1);
    REQUIRE_EQ(retrieved, 0.5f);
    
    // Test multiple neurons
    for (size_t i = 0; i < 50; ++i) {
        brain.getWorkingMemory()->storeToNeuron(i, static_cast<float>(i) * 0.01f);
    }
    
    // Verify retrieval
    for (size_t i = 0; i < 50; ++i) {
        float retrieved = brain.getWorkingMemory()->retrieve(i);
        REQUIRE_EQ(retrieved, static_cast<float>(i) * 0.01f);
    }
    
    brain.reset();
    return true;
}

bool testEpisodicMemoryIntegration() {
    auto config = IntegrationTestFramework::createDefaultConfig();
    config->set("episodic_capacity", 500, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    REQUIRE(success);
    
    // Verify episodic memory is initialized
    REQUIRE(brain.getEpisodicMemory() != nullptr);
    
    // Test episodic memory storage
    std::vector<float> testState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    brain.getEpisodicMemory()->recordExperience(testState, 1);
    
    auto episodes = brain.getEpisodicMemory()->getRecentEpisodes(1);
    REQUIRE_EQ(episodes.size(), 1);
    REQUIRE_EQ(episodes[0].size(), testState.size());
    
    for (size_t i = 0; i < testState.size(); ++i) {
        REQUIRE_EQ(episodes[0][i], testState[i]);
    }
    
    brain.reset();
    return true;
}

bool testAssociativeMemoryIntegration() {
    auto config = IntegrationTestFramework::createDefaultConfig();
    config->set("associative_capacity", 200, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    REQUIRE(success);
    
    // Verify associative memory is initialized
    REQUIRE(brain.getAssociativeMemory() != nullptr);
    
    // Test associative memory
    std::vector<float> key = {0.1f, 0.2f, 0.3f};
    std::vector<float> value = {0.9f, 0.8f, 0.7f};
    
    brain.getAssociativeMemory()->storeAssociation(key, value);
    
    auto retrievedValue = brain.getAssociativeMemory()->retrieveAssociation(key);
    REQUIRE_EQ(retrievedValue.size(), value.size());
    
    for (size_t i = 0; i < value.size(); ++i) {
        REQUIRE_EQ(retrievedValue[i], value[i]);
    }
    
    brain.reset();
    return true;
}

bool testMemorySystemInitialization() {
    auto config = IntegrationTestFramework::createDefaultConfig();
    config->set("working_memory_capacity", 500, nlm::ConfigSource::Default);
    config->set("episodic_memory_capacity", 300, nlm::ConfigSource::Default);
    config->set("associative_memory_capacity", 200, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    REQUIRE(success);
    
    // Verify all memory systems are properly initialized
    REQUIRE(brain.getWorkingMemory() != nullptr);
    REQUIRE(brain.getEpisodicMemory() != nullptr);
    REQUIRE(brain.getAssociativeMemory() != nullptr);
    
    // Test memory storage/retrieval functionality
    brain.getWorkingMemory()->storeToNeuron(42, 0.75f);
    float retrieved = brain.getWorkingMemory()->retrieve(42);
    REQUIRE_EQ(retrieved, 0.75f);
    
    std::vector<float> state = {0.1f, 0.2f, 0.3f};
    brain.getEpisodicMemory()->recordExperience(state, 5);
    
    auto episodes = brain.getEpisodicMemory()->getRecentEpisodes(1);
    REQUIRE_EQ(episodes.size(), 1);
    REQUIRE_EQ(episodes[0].size(), state.size());
    
    brain.reset();
    return true;
}

// Run all memory integration tests
bool runMemoryIntegrationTests() {
    bool allPassed = true;
    
    std::cout << "[RUNNING] Memory Integration Tests..." << std::endl;
    
    allPassed &= testWorkingMemoryIntegration();
    std::cout << "[PASS] WorkingMemory Integration" << std::endl;
    
    allPassed &= testEpisodicMemoryIntegration();
    std::cout << "[PASS] EpisodicMemory Integration" << std::endl;
    
    allPassed &= testAssociativeMemoryIntegration();
    std::cout << "[PASS] AssociativeMemory Integration" << std::endl;
    
    allPassed &= testMemorySystemInitialization();
    std::cout << "[PASS] MemorySystem Initialization" << std::endl;
    
    return allPassed;
}

} // namespace test_memory_integration
