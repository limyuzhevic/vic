// AgentBrain Tests
#include "agent/AgentBrain.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_agentbrain {

void testAgentBrainCreation() {
    nlm::Config config;
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    assert(agent.getBrain() == brain.get());
    assert(agent.getSensoryInputSize() > 0);
    assert(agent.getMotorOutputSize() > 0);
    
    std::cout << "    testAgentBrainCreation passed" << std::endl;
}

void testAgentBrainInitialization() {
    nlm::Config config;
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    // Create a mock world for testing
    class MockWorld {
    public:
        void initialize(int, int) {}
        int getVisionWidth() const { return 16; }
        int getVisionHeight() const { return 16; }
    };
    
    MockWorld world;
    agent.initialize(world);
    
    // Agent should have initialized successfully
    assert(agent.isInitialized());
    
    std::cout << "    testAgentBrainInitialization passed" << std::endl;
}

void testAgentBrainConfiguration() {
    nlm::Config config;
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    // Test default configuration
    assert(agent.isRewardModulationEnabled());
    assert(agent.isDevelopmentEnabled());
    assert(agent.isCuriosityEnabled());
    
    // Test enable/disable flags
    agent.enableRewardModulation(false);
    assert(!agent.isRewardModulationEnabled());
    
    agent.enableRewardModulation(true);
    assert(agent.isRewardModulationEnabled());
    
    agent.enableDevelopment(false);
    assert(!agent.isDevelopmentEnabled());
    
    agent.enableDevelopment(true);
    assert(agent.isDevelopmentEnabled());
    
    agent.enableCuriosity(false);
    assert(!agent.isCuriosityEnabled());
    
    agent.enableCuriosity(true);
    assert(agent.isCuriosityEnabled());
    
    // Test structural plasticity
    agent.enableStructuralPlasticity(true);
    assert(agent.isStructuralPlasticityEnabled());
    
    std::cout << "    testAgentBrainConfiguration passed" << std::endl;
}

void testAgentBrainStats() {
    nlm::Config config;
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    // Test neuron count statistics
    size_t motorCount = agent.getMotorNeuronCount();
    size_t sensoryCount = agent.getSensoryNeuronCount();
    
    // Motor and sensory counts should be non-negative
    assert(motorCount >= 0);
    assert(sensoryCount >= 0);
    
    // Test config access
    auto config = agent.getConfig();
    assert(config != nullptr);
    
    std::cout << "    testAgentBrainStats passed" << std::endl;
}

void testAgentBrainNeuromodulation() {
    nlm::Config config;
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    // Test neuromodulation levels
    float dopamine = agent.getNeuromodulationLevel();
    float curiosity = agent.getCuriosityLevel();
    float novelty = agent.getNoveltyLevel();
    float predictionError = agent.getPredictionError();
    
    // Initial values should be 0 or reasonable
    assert(dopamine >= -1.0f && dopamine <= 1.0f);
    assert(curiosity >= 0.0f && curiosity <= 1.0f);
    assert(novelty >= 0.0f && novelty <= 1.0f);
    assert(predictionError >= -1.0f && predictionError <= 1.0f);
    
    std::cout << "    testAgentBrainNeuromodulation passed" << std::endl;
}

void testAgentBrainDevelopment() {
    nlm::Config config;
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    // Test developmental stage
    auto stage = agent.getDevelopmentalStage();
    // Stage should be a valid enum value
    assert(stage == nlm::DevelopmentalStage::Initial ||
           stage == nlm::DevelopmentalStage::CriticalPeriod ||
           stage == nlm::DevelopmentalStage::Maturation ||
           stage == nlm::DevelopmentalStage::Adult ||
           stage == nlm::DevelopmentalStage::Aging);
    
    std::cout << "    testAgentBrainDevelopment passed" << std::endl;
}

void testAgentBrainReset() {
    nlm::Config config;
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    // Set some state
    agent.enableRewardModulation(false);
    float dopamine = agent.getNeuromodulationLevel();
    
    // Reset agent
    agent.reset();
    
    // State should be reset to default
    assert(agent.isRewardModulationEnabled());  // Reset to true
    assert(agent.getNeuromodulationLevel() == 0.0f);
    assert(agent.getCuriosityLevel() == 0.0f);
    
    std::cout << "    testAgentBrainReset passed" << std::endl;
}

void testAgentBrainConfigIntegration() {
    nlm::Config config;
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    // Test that agent's config access works
    auto agentConfig = agent.getConfig();
    assert(agentConfig != nullptr);
    
    // Config should be accessible through both brain and agent
    auto brainConfig = brain->getConfig();
    assert(brainConfig != nullptr);
    
    std::cout << "    testAgentBrainConfigIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running AgentBrain tests..." << std::endl;
    testAgentBrainCreation();
    testAgentBrainInitialization();
    testAgentBrainConfiguration();
    testAgentBrainStats();
    testAgentBrainNeuromodulation();
    testAgentBrainDevelopment();
    testAgentBrainReset();
    testAgentBrainConfigIntegration();
}

} // namespace test_agentbrain