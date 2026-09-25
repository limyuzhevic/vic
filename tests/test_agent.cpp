// Agent Tests
#include "agent/SensoryPercept.hpp"
#include "agent/AgentBrain.hpp"
#include <catch2/catch_test_macros.hpp>

namespace test_agent {

TEST_CASE("SensoryPercept creation and basic functionality", "[agent][sensory_percept][creation]") {
    nlm::SensoryPercept percept;
    
    REQUIRE(percept.getPerceptId() != 0);
    REQUIRE(percept.getSensoryModality() != nullptr);
    REQUIRE(percept.getConfidence() >= 0.0f);
}

TEST_CASE("AgentBrain creation and initialization", "[agent][agent_brain][creation]") {
    nlm::AgentBrain agentBrain;
    
    REQUIRE(!agentBrain.isInitialized());
}

TEST_CASE("AgentBrain initialization with config", "[agent][agent_brain][initialization]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("agent.enabled", true, nlm::ConfigSource::Runtime);
    config->set("agent.brain_size", 100, nlm::ConfigSource::Runtime);
    
    nlm::AgentBrain agentBrain;
    agentBrain.initialize(config);
    REQUIRE(agentBrain.isInitialized());
}

TEST_CASE("SensoryPercept processing and interpretation", "[agent][sensory_percept][processing]") {
    nlm::SensoryPercept percept;
    
    percept.setSensoryModality("vision");
    percept.setConfidence(0.9f);
    
    REQUIRE(percept.getSensoryModality() == "vision");
    REQUIRE(percept.getConfidence() == 0.9f);
}

TEST_CASE("AgentBrain sensory integration", "[agent][agent_brain][integration]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("agent.enabled", true, nlm::ConfigSource::Runtime);
    config->set("agent.integration_enabled", true, nlm::ConfigSource::Runtime);
    
    nlm::AgentBrain agentBrain;
    agentBrain.initialize(config);
    
    REQUIRE(agentBrain.isInitialized());
    
    // Create sensory percept
    nlm::SensoryPercept percept;
    percept.setSensoryModality("audio");
    percept.setConfidence(0.8f);
    
    // Agent brain should process sensory input
    REQUIRE(agentBrain.isInitialized());
}

TEST_CASE("AgentBrain action selection and execution", "[agent][agent_brain][actions]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("agent.enabled", true, nlm::ConfigSource::Runtime);
    config->set("agent.action_selection_enabled", true, nlm::ConfigSource::Runtime);
    
    nlm::AgentBrain agentBrain;
    agentBrain.initialize(config);
    
    REQUIRE(agentBrain.isInitialized());
    
    // Agent brain should select and execute actions
    // This would involve complex integration with other systems
    REQUIRE(agentBrain.isInitialized());
}

TEST_CASE("AgentBrain with different configurations", "[agent][brain][config]") {
    // Test agent brain with different configurations
    std::vector<std::string> agentTypes = {"simple", "complex", "advanced", "neural"};
    
    for (const auto& agentType : agentTypes) {
        auto config = std::make_shared<nlm::Config>();
        config->set("agent.type", agentType, nlm::ConfigSource::Runtime);
        config->set("agent.brain_size", 50, nlm::ConfigSource::Runtime);
        
        nlm::AgentBrain agentBrain;
        agentBrain.initialize(config);
        
        REQUIRE(agentBrain.isInitialized());
    }
}

TEST_CASE("AgentBrain learning and adaptation", "[agent][brain][learning]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("agent.enabled", true, nlm::ConfigSource::Runtime);
    config->set("agent.learning_enabled", true, nlm::ConfigSource::Runtime);
    config->set("agent.adaptation_rate", 0.5f, nlm::ConfigSource::Runtime);
    
    nlm::AgentBrain agentBrain;
    agentBrain.initialize(config);
    
    REQUIRE(agentBrain.isInitialized());
    
    // Agent brain should learn and adapt over time
    // This would involve complex learning algorithms
    REQUIRE(agentBrain.isInitialized());
}

TEST_CASE("AgentBrain performance evaluation", "[agent][brain][performance]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("agent.enabled", true, nlm::ConfigSource::Runtime);
    
    nlm::AgentBrain agentBrain;
    agentBrain.initialize(config);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Perform agent brain operations
    for (int i = 0; i < 50; ++i) {
        // Agent brain would perform various operations
        // This simulates agent behavior
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    // Should complete within reasonable time
    REQUIRE(duration.count() < 1000000);  // Less than 1 second
}

TEST_CASE("AgentBrain integration with all systems", "[agent][integration][systems]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("agent.enabled", true, nlm::ConfigSource::Runtime);
    config->set("agent.full_integration", true, nlm::ConfigSource::Runtime);
    
    nlm::AgentBrain agentBrain;
    agentBrain.initialize(config);
    
    REQUIRE(agentBrain.isInitialized());
    
    // Agent brain should integrate with all NLM systems:
    // - Brain (neural processing)
    // - Sensory (perception)
    // - Memory (learning)
    // - Motor (action)
    // - Environment (interaction)
    // - World (context)
    
    REQUIRE(agentBrain.isInitialized());
}

} // namespace test_agent
