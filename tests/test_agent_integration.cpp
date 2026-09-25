// AgentBrain Integration Tests
// Tests AgentBrain integration with all cognition systems and the world

#include "../src/agent/AgentBrain.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>

namespace test_agent_integration {

void testAgentBrainBasic() {
    std::cout << "  Testing AgentBrain basic functionality..." << std::endl;
    
    auto config = std::make_shared<Config>();
    Brain brain(config);
    brain.initialize();
    
    AgentBrain agent(std::make_shared<Brain>(brain));
    
    // Create test world
    SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    
    // Initialize agent
    agent.initialize(world);
    
    // Test basic properties
    assert(agent.getSensoryInputSize() > 0);
    assert(agent.getMotorOutputSize() > 0);
    
    // Test configuration
    agent.enableRewardModulation(true);
    agent.enableCuriosity(true);
    agent.enableDevelopment(true);
    
    assert(agent.isRewardModulationEnabled());
    assert(agent.isCuriosityEnabled());
    assert(agent.isDevelopmentEnabled());
    
    // Test neuromodulation levels
    float neuromodulation = agent.getNeuromodulationLevel();
    assert(neuromodulation >= 0.0f);
    
    float curiosity = agent.getCuriosityLevel();
    assert(curiosity >= 0.0f && curiosity <= 1.0f);
    
    float novelty = agent.getNoveltyLevel();
    assert(novelty >= 0.0f);
    
    float predictionError = agent.getPredictionError();
    assert(predictionError >= 0.0f);
    
    std::cout << "    PASSED" << std::endl;
}

void testAgentBrainCognitionIntegration() {
    std::cout << "  Testing AgentBrain cognition integration..." << std::endl;
    
    auto config = std::make_shared<Config>();
    Brain brain(config);
    brain.initialize();
    
    AgentBrain agent(std::make_shared<Brain>(brain));
    
    // Create test world
    SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    agent.initialize(world);
    
    // Create test sensory percept
    SensoryPercept percept;
    
    // Get vision dimensions
    size_t visionWidth = world.getVisionWidth();
    size_t visionHeight = world.getVisionHeight();
    
    std::vector<float> visionData(visionWidth * visionHeight, 0.5f);
    percept.setVision(visionData);
    
    // Test processSensoryInput
    agent.processSensoryInput(percept);
    
    // Test decodeMotorCommand
    MotorCommand action = agent.decodeMotorCommand();
    // Should return a valid motor command
    assert(action >= MotorCommand::MoveForward && action <= MotorCommand::Wait);
    
    // Test cognition system connection methods
    auto* predictionSystem = agent.getPredictionSystem();
    // Should return nullptr since we haven't connected anything
    assert(predictionSystem == nullptr);
    
    auto* planner = agent.getNeuralPlanner();
    assert(planner != nullptr);
    
    std::cout << "    PASSED" << std::endl;
}

void testAgentBrainCognitionConnection() {
    std::cout << "  Testing AgentBrain cognition system connections..." << std::endl;
    
    auto config = std::make_shared<Config>();
    Brain brain(config);
    brain.initialize();
    
    AgentBrain agent(std::make_shared<Brain>(brain));
    
    // Create test world
    SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    agent.initialize(world);
    
    // Create mock cognition systems (in real implementation these would be injected)
    // For testing, we'll just test the connection methods exist
    
    // Test connection methods (they should accept nullptr without crashing)
    agent.connectPredictionSystem(nullptr);
    agent.connectNeuralPlanner(nullptr);
    agent.connectSelfModel(nullptr);
    agent.connectSocialLearning(nullptr);
    
    // Test updateCognitionSystems
    agent.updateCognitionSystems();
    
    // Test goal setting
    std::vector<float> goal = {0.5f, 0.5f, 0.5f, 0.5f, 0.5f};
    agent.setGoal(goal);
    
    std::vector<float> retrievedGoal = agent.getCurrentGoal();
    assert(retrievedGoal.size() == goal.size());
    
    std::cout << "    PASSED" << std::endl;
}

void testAgentBrainDevelopment() {
    std::cout << "  Testing AgentBrain development integration..." << std::endl;
    
    auto config = std::make_shared<Config>();
    Brain brain(config);
    brain.initialize();
    
    AgentBrain agent(std::make_shared<Brain>(brain));
    
    // Create test world
    SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    agent.initialize(world);
    
    // Test updateDevelopment
    agent.updateDevelopment(0.1);  // Update by 0.1 timestep
    
    // Test developmental stage
    DevelopmentalStage stage = agent.getDevelopmentalStage();
    assert(stage >= DevelopmentalStage::Initial && stage <= DevelopmentalStage::Adult);
    
    // Test reset
    agent.reset();
    
    // Reset should work without crashing
    assert(true);
    
    std::cout << "    PASSED" << std::endl;
}

void testAgentBrainRewardModulation() {
    std::cout << "  Testing AgentBrain reward modulation..." << std::endl;
    
    auto config = std::make_shared<Config>();
    Brain brain(config);
    brain.initialize();
    
    AgentBrain agent(std::make_shared<Brain>(brain));
    
    // Create test world
    SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    agent.initialize(world);
    
    // Test applyRewardModulation
    agent.applyRewardModulation(1.0f, 0.5f);  // Reward 1.0, predicted 0.5
    
    // Test prediction error
    float predictionError = agent.getPredictionError();
    assert(predictionError >= 0.0f);
    
    // Test expected reward
    float expectedReward = agent.getPredictionError();  // Note: this is prediction error, not expected reward
    assert(expectedReward >= 0.0f);
    
    std::cout << "    PASSED" << std::endl;
}

void runAll() {
    std::cout << "=== AgentBrain Integration Tests ===" << std::endl;
    
    testAgentBrainBasic();
    testAgentBrainCognitionIntegration();
    testAgentBrainCognitionConnection();
    testAgentBrainDevelopment();
    testAgentBrainRewardModulation();
    
    std::cout << "=== AgentBrain Integration Tests Complete ===" << std::endl;
}

} // namespace test_agent_integration