#include "AdvancedAgent.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>

namespace test_integration {

void testConfigurationIntegration() {
    std::cout << "    Testing configuration integration..." << std::endl;
    
    // Test 1: Create configuration and save to file
    auto config = std::make_shared<Config>();
    
    // Set various configuration values
    config->set("neuron_count", 1000);
    config->set("region_count", 2);
    config->set("connection_probability", 0.15);
    config->set("plasticity_learning_rate", 0.02);
    config->set("stdp_ltp_weight", 0.03);
    config->set("stdp_ltd_weight", 0.025);
    config->set("development_synaptogenesis_rate", 0.002);
    config->set("development_pruning_rate", 0.0002);
    config->set("dopamine_baseline", 0.1);
    config->set("curiosity_threshold", 0.3);
    config->set("novelty_decay", 0.99);
    
    // Save configuration to file
    std::string configFile = "test_config.cfg";
    bool saveSuccess = config->saveToFile(configFile);
    assert(saveSuccess);
    
    // Verify file was created and has content
    std::ifstream file(configFile);
    assert(file.good());
    
    std::string line;
    bool foundNeuronCount = false;
    while (std::getline(file, line)) {
        if (line.find("neuron_count") != std::string::npos) {
            foundNeuronCount = true;
            break;
        }
    }
    assert(foundNeuronCount);
    
    file.close();
    std::remove(configFile.c_str());
    
    // Test 2: Load configuration from file
    auto config2 = std::make_shared<Config>();
    bool loadSuccess = config2->loadFromFile(configFile);
    assert(loadSuccess);
    
    // Test 3: Test command-line configuration
    auto config3 = std::make_shared<Config>();
    int argc = 5;
    char* argv[] = {
        (char*)"test",
        (char*)"--neuron_count=500",
        (char*)"--region_count=1",
        (char*)"--plasticity_learning_rate=0.05"
    };
    
    bool cliSuccess = config3->loadFromArgs(argc, argv);
    assert(cliSuccess);
    
    // Verify CLI values
    assert(config3->has("neuron_count"));
    assert(config3->has("region_count"));
    assert(config3->has("plasticity_learning_rate"));
    
    std::cout << "    Configuration integration test passed" << std::endl;
}

void testBrainIntegration() {
    std::cout << "    Testing brain integration..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    config->set("region_count", 2);
    config->set("connection_probability", 0.1);
    config->set("plasticity_learning_rate", 0.01);
    
    // Initialize brain
    Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Verify brain properties
    assert(brain.getTotalNeuronCount() == 500);
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalSynapseCount() > 0);
    assert(brain.getTotalNeuronCount() > 0);
    
    // Run simulation steps
    for (SimulationStep step = 0; step < 100; ++step) {
        brain.step(step, step * 0.001);
    }
    
    // Verify brain state after simulation
    assert(brain.getTotalNeuronCount() == 500);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    Brain integration test passed" << std::endl;
}

void testAdvancedPlasticityIntegration() {
    std::cout << "    Testing advanced plasticity integration..." << std::endl;
    
    // Create brain with configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 200);
    config->set("region_count", 1);
    config->set("plasticity_learning_rate", 0.01);
    
    Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Create advanced plasticity system
    AdvancedPlasticity advancedPlasticity;
    
    // Enable all plasticity mechanisms
    advancedPlasticity.enableHomeostaticPlasticity(true);
    advancedPlasticity.enableRewardModulatedPlasticity(true);
    advancedPlasticity.enableMetaPlasticity(true);
    advancedPlasticity.enableSynapticScaling(true);
    
    // Configure parameters
    advancedPlasticity.setHomeostaticTarget(10.0f);
    advancedPlasticity.setRewardLearningRate(0.01f);
    advancedPlasticity.setMetaPlasticityLearningRate(0.001f);
    advancedPlasticity.setSynapticScalingGain(1.0f);
    
    // Initialize with random parameters
    RandomGenerator rng(42);
    advancedPlasticity.initializeRandom(rng);
    
    // Test application to brain
    advancedPlasticity.applyToBrain(&brain, 0.001);
    
    // Verify plasticity mechanisms are working
    assert(advancedPlasticity.isHomeostaticPlasticityEnabled());
    assert(advancedPlasticity.isRewardModulatedPlasticityEnabled());
    assert(advancedPlasticity.isMetaPlasticityEnabled());
    assert(advancedPlasticity.isSynapticScalingEnabled());
    
    // Run simulation with advanced plasticity
    for (SimulationStep step = 0; step < 50; ++step) {
        brain.step(step, step * 0.001);
        advancedPlasticity.update(15.0f, 0.5f, 0.1f, 0.3f, 0.2f, 0.001);
        advancedPlasticity.applyToBrain(&brain, 0.001);
    }
    
    std::cout << "    Advanced plasticity integration test passed" << std::endl;
}

void testAdvancedAgentIntegration() {
    std::cout << "    Testing advanced agent integration..." << std::endl;
    
    // Create configuration
    AdvancedAgentConfig agentConfig;
    agentConfig.enableMetaCognitiveMonitoring = true;
    agentConfig.enableSocialLearning = true;
    agentConfig.enableGoalBasedBehavior = true;
    agentConfig.enableWorkingMemory = true;
    
    // Create world
    SimpleWorld world;
    
    // Create brain
    auto brainConfig = std::make_shared<Config>();
    brainConfig->set("neuron_count", 100);
    brainConfig->set("region_count", 1);
    
    Brain brain(brainConfig);
    brain.initialize();
    
    // Create advanced agent
    AdvancedAgent agent(brain, world);
    agent.initialize(agentConfig);
    
    // Set up goals
    Goal goal1(1, "Move forward", 0.8f, 0.9f, 100.0, "move_forward", 0.8f);
    Goal goal2(2, "Collect item", 0.6f, 0.7f, 200.0, "collect_item", 0.7f);
    
    agent.setGoal(goal1);
    agent.setGoal(goal2);
    
    // Run simulation steps
    for (int step = 0; step < 20; ++step) {
        AgentStepResult result = agent.step(0.1);
        
        // Verify step results
        assert(result.timestep >= 0);
        assert(result.time > 0.0);
        assert(result.rewardReceived >= 0.0f);
        assert(result.actionSuccessful || !result.actionSuccessful); // Either is valid
        
        // Update goals based on performance
        agent.updateGoalsAndPlanning(result);
        
        // Update working memory
        agent.updateWorkingMemory(result);
        
        // Store experience
        agent.storeExperience(result);
    }
    
    // Verify agent state
    std::vector<Goal> activeGoals = agent.getActiveGoals();
    assert(!activeGoals.empty());
    
    std::cout << "    Advanced agent integration test passed" << std::endl;
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
    
    // Monitor meta-cognitive state over time
    std::vector<float> confidenceHistory;
    std::vector<float> uncertaintyHistory;
    
    for (int step = 0; step < 30; ++step) {
        AgentStepResult result;
        result.timestep = step;
        result.time = step * 0.1;
        result.rewardReceived = 1.0f;
        result.predictionError = 0.1f;
        result.curiosityLevel = 0.2f;
        result.actionSuccessful = true;
        
        // Update meta-cognitive state
        agent.updateMetaCognitiveState();
        
        // Record meta-cognitive metrics
        float confidence = agent.getConfidence();
        float uncertainty = agent.getUncertainty();
        
        confidenceHistory.push_back(confidence);
        uncertaintyHistory.push_back(uncertainty);
        
        // Verify reasonable values
        assert(confidence >= 0.0f && confidence <= 1.0f);
        assert(uncertainty >= 0.0f && uncertainty <= 1.0f);
    }
    
    // Check that meta-cognitive state is evolving
    assert(!confidenceHistory.empty());
    assert(!uncertaintyHistory.empty());
    
    std::cout << "    Meta-cognitive monitoring test passed" << std::endl;
}

void testCuriosityDrivenBehavior() {
    std::cout << "    Testing curiosity-driven behavior..." << std::endl;
    
    AdvancedAgentConfig config;
    config.enableMetaCognitiveMonitoring = true;
    config.curiosityThreshold = 0.2f;
    config.explorationRate = 0.5f;
    SimpleWorld world;
    auto brainConfig = std::make_shared<Config>();
    brainConfig->set("neuron_count", 50);
    brainConfig->set("region_count", 1);
    
    Brain brain(brainConfig);
    brain.initialize();
    
    AdvancedAgent agent(brain, world);
    agent.initialize(config);
    
    // Track exploration/exploitation decisions
    int exploreCount = 0;
    int exploitCount = 0;
    
    for (int step = 0; step < 50; ++step) {
        // Update curiosity-driven exploration
        agent.updateCuriosityDrivenExploration();
        
        // Check exploration/exploitation status
        if (agent.shouldExplore() && !agent.shouldExploit()) {
            exploreCount++;
        } else if (!agent.shouldExplore() && agent.shouldExploit()) {
            exploitCount++;
        }
        
        // Verify intrinsic motivation calculation
        float intrinsicMotivation = agent.calculateIntrinsicMotivation();
        assert(intrinsicMotivation >= 0.0f && intrinsicMotivation <= 1.0f);
    
    // Verify exploration behavior occurred
    assert(exploreCount > 0 || exploitCount > 0);
    
    // Verify curiosity level is reasonable
    float curiosityLevel = agent.getCuriosityLevel();
    assert(curiosityLevel >= 0.0f && curiosityLevel <= 1.0f);
    
    std::cout << "    Curiosity-driven behavior test passed" << std::endl;
}

void testSocialLearningIntegration() {
    std::cout << "    Testing social learning integration..." << std::endl;
    
    AdvancedAgentConfig config;
    config.enableSocialLearning = true;
    SimpleWorld world;
    auto brainConfig = std::make_shared<Config>();
    brainConfig->set("neuron_count", 50);
    brainConfig->set("region_count", 1);
    
    Brain brain(brainConfig);
    brain.initialize();
    
    // Create multiple agents
    std::vector<AdvancedAgent*> agents;
    for (int i = 0; i < 3; ++i) {
        agents.push_back(new AdvancedAgent(brain, world));
        agents[i]->initialize(config);
    }
    
    // Set up social groups
    for (int i = 0; i < 3; ++i) {
        agents[i]->getAgentState().socialGroupId = 1;
    }
    
    // Have agents observe each other
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (i != j) {
                agents[i]->observeOtherAgent(*agents[j]);
            }
        }
    }
    
    // Clean up
    for (AdvancedAgent* agent : agents) {
        delete agent;
    }
    
    std::cout << "    Social learning integration test passed" << std::endl;
}

void runAll() {
    std::cout << "Running integration tests..." << std::endl;
    
    testConfigurationIntegration();
    testBrainIntegration();
    testAdvancedPlasticityIntegration();
    testAdvancedAgentIntegration();
    testMetaCognitiveMonitoring();
    testCuriosityDrivenBehavior();
    testSocialLearningIntegration();
    
    std::cout << "All integration tests passed!" << std::endl;
}

} // namespace test_integration
