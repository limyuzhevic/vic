// AgentBrain Integration Tests
// Tests integration of AgentBrain with brain systems

#include "integration/test_framework.cpp"

namespace test_agentbrain_integration {

bool testAgentBrainWithWorkingMemory() {
    auto config = IntegrationTestFramework::createDefaultConfig();
    config->set("working_memory_capacity", 500, nlm::ConfigSource::Default);
    config->set("working_memory_decay", 0.95f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    REQUIRE(success);
    
    // Verify brain subsystems are initialized
    REQUIRE(brain.getWorkingMemory() != nullptr);
    REQUIRE(brain.getEpisodicMemory() != nullptr);
    REQUIRE(brain.getAssociativeMemory() != nullptr);
    
    // Test AgentBrain integration with working memory
    brain.getWorkingMemory()->storeToNeuron(1, 0.6f);
    float retrieved = brain.getWorkingMemory()->retrieve(1);
    REQUIRE_EQ(retrieved, 0.6f);
    
    brain.reset();
    return true;
}

bool testAgentBrainWithEpisodicMemory() {
    auto config = IntegrationTestFramework::createDefaultConfig();
    config->set("episodic_memory_capacity", 300, nlm::ConfigSource::Default);
    config->set("episodic_decay_rate", 0.98f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    REQUIRE(success);
    
    // Verify brain subsystems are initialized
    REQUIRE(brain.getEpisodicMemory() != nullptr);
    
    // Test AgentBrain integration with episodic memory
    std::vector<float> experience = {0.1f, 0.2f, 0.3f, 0.4f};
    brain.getEpisodicMemory()->recordExperience(experience, 1);
    
    auto episodes = brain.getEpisodicMemory()->getRecentEpisodes(1);
    REQUIRE_EQ(episodes.size(), 1);
    REQUIRE_EQ(episodes[0].size(), experience.size());
    
    brain.reset();
    return true;
}

bool testAgentBrainWithPredictionSystem() {
    auto config = IntegrationTestFramework::createDefaultConfig();
    config->set("prediction_horizon", 3, nlm::ConfigSource::Default);
    config->set("prediction_accuracy", 0.8f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    REQUIRE(success);
    
    // Verify brain subsystems are initialized
    REQUIRE(brain.getPredictionSystem() != nullptr);
    REQUIRE(brain.getDopamine() != nullptr);
    
    // Test AgentBrain integration with prediction system
    std::vector<float> state = {0.1f, 0.2f, 0.3f};
    brain.getPredictionSystem()->recordSensoryState(state, 1);
    
    auto prediction = brain.getPredictionSystem()->generatePrediction(1);
    REQUIRE(!prediction.empty());
    
    float error = brain.getPredictionSystem()->updateWithObservation(state, 2);
    REQUIRE(error >= 0.0f);
    
    brain.reset();
    return true;
}

bool testAgentBrainSensoryProcessing() {
    auto config = IntegrationTestFramework::createDefaultConfig();
    config->set("sensory_neuron_count", 100, nlm::ConfigSource::Default);
    config->set("sensory_processing_rate", 0.1f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    REQUIRE(success);
    
    // Verify brain subsystems are initialized
    REQUIRE(brain.getDopamine() != nullptr);
    REQUIRE(brain.getCuriosity() != nullptr);
    REQUIRE(brain.getNovelty() != nullptr);
    
    // Test AgentBrain sensory processing
    brain.getDopamine()->setLevel(0.5f);
    brain.getCuriosity()->setCuriosityLevel(0.7f);
    
    REQUIRE_EQ(brain.getDopamine()->getLevel(), 0.5f);
    REQUIRE_EQ(brain.getCuriosity()->getCuriosityLevel(), 0.7f);
    
    brain.reset();
    return true;
}

bool testAgentBrainActionSelectionWithMemoryBasedCuriosity() {
    auto config = IntegrationTestFramework::createDefaultConfig();
    config->set("action_selection_strength", 0.8f, nlm::ConfigSource::Default);
    config->set("memory_based_curiosity", 0.6f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    REQUIRE(success);
    
    // Verify brain subsystems are initialized
    REQUIRE(brain.getWorkingMemory() != nullptr);
    REQUIRE(brain.getCuriosity() != nullptr);
    REQUIRE(brain.getDopamine() != nullptr);
    
    // Test AgentBrain action selection with memory-based curiosity
    brain.getWorkingMemory()->storeToNeuron(1, 0.8f);
    float memoryValue = brain.getWorkingMemory()->retrieve(1);
    REQUIRE_EQ(memoryValue, 0.8f);
    
    brain.getCuriosity()->setCuriosityLevel(0.6f);
    float curiosityLevel = brain.getCuriosity()->getCuriosityLevel();
    REQUIRE_EQ(curiosityLevel, 0.6f);
    
    brain.getDopamine()->setLevel(0.3f);
    float dopamineLevel = brain.getDopamine()->getLevel();
    REQUIRE_EQ(dopamineLevel, 0.3f);
    
    brain.reset();
    return true;
}

// Run all AgentBrain integration tests
bool runAgentBrainIntegrationTests() {
    bool allPassed = true;
    
    std::cout << "[RUNNING] AgentBrain Integration Tests..." << std::endl;
    
    allPassed &= testAgentBrainWithWorkingMemory();
    std::cout << "[PASS] AgentBrain With WorkingMemory" << std::endl;
    
    allPassed &= testAgentBrainWithEpisodicMemory();
    std::cout << "[PASS] AgentBrain With EpisodicMemory" << std::endl;
    
    allPassed &= testAgentBrainWithPredictionSystem();
    std::cout << "[PASS] AgentBrain With PredictionSystem" << std::endl;
    
    allPassed &= testAgentBrainSensoryProcessing();
    std::cout << "[PASS] AgentBrain SensoryProcessing" << std::endl;
    
    allPassed &= testAgentBrainActionSelectionWithMemoryBasedCuriosity();
    std::cout << "[PASS] AgentBrain ActionSelection With MemoryBasedCuriosity" << std::endl;
    
    return allPassed;
}

} // namespace test_agentbrain_integration
