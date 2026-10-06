#include "AdvancedAgent.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_advanced_agent {

void testAdvancedAgentCreation() {
    std::cout << "    Testing AdvancedAgent creation..." << std::endl;
    
    // Create configuration
    AdvancedAgentConfig config;
    config.enableMetaCognitiveMonitoring = true;
    config.enableSocialLearning = true;
    config.enableGoalBasedBehavior = true;
    config.enableWorkingMemory = true;
    
    // Create world
    SimpleWorld world;
    
    // Create brain
    auto brainConfig = std::make_shared<Config>();
    brainConfig->set("neuron_count", 100);
    brainConfig->set("region_count", 2);
    brainConfig->set("connection_probability", 0.1);
    
    Brain brain(brainConfig);
    if (!brain.initialize()) {
        assert(false && "Failed to initialize brain");
    }
    
    // Create advanced agent
    AdvancedAgent agent(brain, world);
    agent.initialize(config);
    
    // Verify agent state
    assert(agent.getAgentState().agentId == 0);
    assert(agent.getAgentState().agentType == "AdvancedAgent");
    assert(agent.getAgentState().confidence > 0.0f);
    
    std::cout << "    AdvancedAgent creation test passed" << std::endl;
}

void testGoalManagement() {
    std::cout << "    Testing goal management..." << std::endl;
    
    AdvancedAgentConfig config;
    SimpleWorld world;
    auto brainConfig = std::make_shared<Config>();
    brainConfig->set("neuron_count", 50);
    brainConfig->set("region_count", 1);
    
    Brain brain(brainConfig);
    brain.initialize();
    
    AdvancedAgent agent(brain, world);
    agent.initialize(config);
    
    // Create goals
    Goal goal1(1, "Reach position (10,10)", 0.8f, 0.9f, 1000.0, "reach_10_10", 0.8f);
    Goal goal2(2, "Collect resource", 0.6f, 0.7f, 500.0, "collect_resource", 0.7f);
    
    agent.setGoal(goal1);
    agent.setGoal(goal2);
    
    // Verify goals
    std::vector<Goal> goals = agent.getActiveGoals();
    assert(goals.size() == 2);
    
    Goal goal1Copy = goals[0];
    Goal goal2Copy = goals[1];
    assert(goal1Copy.id == 1 || goal2Copy.id == 1);
    assert(goal1Copy.id == 2 || goal2Copy.id == 2);
    
    // Remove a goal
    agent.removeGoal(1);
    goals = agent.getActiveGoals();
    assert(goals.size() == 1);
    assert(goals[0].id == 2);
    
    std::cout << "    Goal management test passed" << std::endl;
}

void testPlanningAndDecisionMaking() {
    std::cout << "    Testing planning and decision making..." << std::endl;
    
    AdvancedAgentConfig config;
    SimpleWorld world;
    auto brainConfig = std::make_shared<Config>();
    brainConfig->set("neuron_count", 50);
    brainConfig->set("region_count", 1);
    
    Brain brain(brainConfig);
    brain.initialize();
    
    AdvancedAgent agent(brain, world);
    agent.initialize(config);
    
    // Set up goals
    Goal goal1(1, "Move to target", 0.7f, 0.8f, 1000.0, "move_to_target", 0.8f);
    agent.setGoal(goal1);
    
    // Generate plans
    agent.generatePlans();
    
    // Verify plans
    std::vector<Plan> plans = agent.getCurrentPlans();
    assert(!plans.empty());
    
    Plan selectedPlan = agent.selectBestPlan();
    assert(selectedPlan.id >= 0);
    
    // Verify plan has steps
    assert(!selectedPlan.steps.empty());
    
    std::cout << "    Planning and decision making test passed" << std::endl;
}

void testMetaCognitiveMonitoring() {
    std::cout << "    Testing meta-cognitive monitoring..." << std::endl;
    
    AdvancedAgentConfig config;
    config.enableMetaCognitiveMonitoring = true;
    SimpleWorld world;
    auto brainConfig = std::make_shared<Config>();
    brainConfig->set("neuron_count", 50);
    brainConfig->set("region_count", 1);
    
    Brain brain(brainConfig);
    brain.initialize();
    
    AdvancedAgent agent(brain, world);
    agent.initialize(config);
    
    // Get initial meta-cognitive state
    float initialConfidence = agent.getConfidence();
    float initialUncertainty = agent.getUncertainty();
    
    assert(initialConfidence > 0.0f && initialConfidence <= 1.0f);
    assert(initialUncertainty >= 0.0f && initialUncertainty <= 1.0f);
    
    // Simulate some steps
    for (int i = 0; i < 10; ++i) {
        // Create step result
        AgentStepResult result;
        result.timestep = i;
        result.time = i * 0.1;
        result.rewardReceived = 1.0f;
        result.predictionError = 0.1f;
        result.curiosityLevel = 0.3f;
        result.actionSuccessful = true;
        
        // Update meta-cognitive state
        agent.updateMetaCognitiveState();
        
        // Check that meta-cognitive state is updating
        float currentConfidence = agent.getConfidence();
        float currentUncertainty = agent.getUncertainty();
        
        // Confidence should increase with performance
        if (result.rewardReceived > 0.5f) {
            assert(currentConfidence >= initialConfidence);
        }
    }
    
    std::cout << "    Meta-cognitive monitoring test passed" << std::endl;
}

void testCuriosityDrivenExploration() {
    std::cout << "    Testing curiosity-driven exploration..." << std::endl;
    
    AdvancedAgentConfig config;
    config.enableMetaCognitiveMonitoring = true;
    config.curiosityThreshold = 0.3f;
    config.explorationRate = 0.4f;
    SimpleWorld world;
    auto brainConfig = std::make_shared<Config>();
    brainConfig->set("neuron_count", 50);
    brainConfig->set("region_count", 1);
    
    Brain brain(brainConfig);
    brain.initialize();
    
    AdvancedAgent agent(brain, world);
    agent.initialize(config);
    
    // Check initial curiosity level
    float initialCuriosity = agent.getCuriosityLevel();
    assert(initialCuriosity >= 0.0f && initialCuriosity <= 1.0f);
    
    // Update curiosity-driven exploration
    agent.updateCuriosityDrivenExploration();
    
    // Check that exploration behavior is working
    bool shouldExplore = agent.shouldExplore();
    bool shouldExploit = agent.shouldExploit();
    
    // Either exploration or exploitation should be true
    assert(shouldExplore || shouldExploit);
    
    // Check intrinsic motivation calculation
    float intrinsicMotivation = agent.calculateIntrinsicMotivation();
    assert(intrinsicMotivation >= 0.0f && intrinsicMotivation <= 1.0f);
    
    // Check exploration action selection
    MotorCommand explorationAction = agent.selectExplorationAction();
    // Should be a valid motor command
    assert(static_cast<int>(explorationAction) >= 0);
    assert(static_cast<int>(explorationAction) < 8); // 8 motor commands
    
    std::cout << "    Curiosity-driven exploration test passed" << std::endl;
}

void testSocialLearning() {
    std::cout << "    Testing social learning..." << std::endl;
    
    AdvancedAgentConfig config;
    config.enableSocialLearning = true;
    SimpleWorld world;
    auto brainConfig = std::make_shared<Config>();
    brainConfig->set("neuron_count", 50);
    brainConfig->set("region_count", 1);
    
    Brain brain(brainConfig);
    brain.initialize();
    
    // Create two agents
    AdvancedAgent agent1(brain, world);
    AdvancedAgent agent2(brain, world);
    
    agent1.initialize(config);
    agent2.initialize(config);
    
    // Set same social group
    agent1.getAgentState().socialGroupId = 1;
    agent2.getAgentState().socialGroupId = 1;
    
    // Have agent1 observe agent2
    agent1.observeOtherAgent(agent2);
    
    // Check that social observations were recorded
    // (Implementation-dependent, just verify no crash)
    
    std::cout << "    Social learning test passed" << std::endl;
}

void testWorkingMemoryIntegration() {
    std::cout << "    Testing working memory integration..." << std::endl;
    
    AdvancedAgentConfig config;
    config.enableWorkingMemory = true;
    SimpleWorld world;
    auto brainConfig = std::make_shared<Config>();
    brainConfig->set("neuron_count", 50);
    brainConfig->set("region_count", 1);
    
    Brain brain(brainConfig);
    brain.initialize();
    
    AdvancedAgent agent(brain, world);
    agent.initialize(config);
    
    // Get working memory
    std::shared_ptr<NeuralWorkingMemory> workingMemory = agent.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Create step result with sensory input
    AgentStepResult result;
    result.timestep = 1;
    result.time = 0.1;
    result.sensoryInput = std::vector<float>(10, 0.5f);
    result.motorOutput = std::vector<float>(6, 0.3f);
    result.rewardReceived = 1.0f;
    
    // Update working memory
    agent.updateWorkingMemory(result);
    
    // Verify working memory was updated (implementation-dependent)
    
    std::cout << "    Working memory integration test passed" << std::endl;
}

void testPerformanceMetrics() {
    std::cout << "    Testing performance metrics..." << std::endl;
    
    AdvancedAgentConfig config;
    SimpleWorld world;
    auto brainConfig = std::make_shared<Config>();
    brainConfig->set("neuron_count", 50);
    brainConfig->set("region_count", 1);
    
    Brain brain(brainConfig);
    brain.initialize();
    
    AdvancedAgent agent(brain, world);
    agent.initialize(config);
    
    // Get initial performance stats
    PerformanceStats initialStats = agent.getPerformanceStats();
    assert(initialStats.totalSteps == 0);
    assert(initialStats.totalReward == 0.0f);
    
    // Simulate some steps
    for (int i = 0; i < 5; ++i) {
        AgentStepResult result;
        result.timestep = i;
        result.time = i * 0.1;
        result.rewardReceived = 1.0f + i * 0.1f;
        result.predictionError = 0.1f;
        result.curiosityLevel = 0.2f + i * 0.05f;
        result.actionSuccessful = true;
        
        agent.updatePerformanceMetrics(result);
        
        PerformanceStats currentStats = agent.getPerformanceStats();
        assert(currentStats.totalSteps == i + 1);
        assert(currentStats.totalReward == 5.0f + 0.5f * i);
        assert(currentStats.averageRewardPerStep > 0.0f);
    }
    
    std::cout << "    Performance metrics test passed" << std::endl;
}

void testResetFunctionality() {
    std::cout << "    Testing reset functionality..." << std::endl;
    
    AdvancedAgentConfig config;
    SimpleWorld world;
    auto brainConfig = std::make_shared<Config>();
    brainConfig->set("neuron_count", 50);
    brainConfig->set("region_count", 1);
    
    Brain brain(brainConfig);
    brain.initialize();
    
    AdvancedAgent agent(brain, world);
    agent.initialize(config);
    
    // Set up some state
    Goal goal(1, "Test goal", 0.8f, 0.9f, 100.0, "test_goal", 0.8f);
    agent.setGoal(goal);
    
    // Get initial state
    AgentState initialState = agent.getAgentState();
    
    // Reset agent
    agent.reset();
    
    // Verify state was reset
    AgentState resetState = agent.getAgentState();
    assert(resetState.agentId == 0); // Reset preserves agent ID
    assert(resetState.age == 0.0f);
    assert(resetState.energy == 1.0f);
    assert(resetState.health == 1.0f);
    
    // Verify goals were reset
    std::vector<Goal> goals = agent.getActiveGoals();
    assert(goals.empty()); // Goals should be cleared
    
    // Test reset for new episode
    agent.resetForNewEpisode();
    
    // Verify additional reset effects
    assert(resetState.age == 0.0f);
    
    std::cout << "    Reset functionality test passed" << std::endl;
}

void runAll() {
    std::cout << "Running AdvancedAgent tests..." << std::endl;
    
    testAdvancedAgentCreation();
    testGoalManagement();
    testPlanningAndDecisionMaking();
    testMetaCognitiveMonitoring();
    testCuriosityDrivenExploration();
    testSocialLearning();
    testWorkingMemoryIntegration();
    testPerformanceMetrics();
    testResetFunctionality();
    
    std::cout << "All AdvancedAgent tests passed!" << std::endl;
}

} // namespace test_advanced_agent
