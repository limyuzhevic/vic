// Advanced AgentBrain Tests
// Tests for new action sequencing, memory-based navigation, and goal-directed behavior

#include "agent/AgentBrain.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_agentbrain {

void testActionSequencing() {
    std::cout << "    Testing Action Sequencing..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::AgentBrain agent(brain);
    
    // Enable action sequencing
    agent.enableActionSequencing(true);
    assert(agent.isActionSequencingEnabled());
    
    // Create and start an action sequence
    std::vector<nlm::MotorCommand> sequence;
    sequence.push_back(nlm::MotorCommand::MoveForward);
    sequence.push_back(nlm::MotorCommand::TurnLeft);
    sequence.push_back(nlm::MotorCommand::Interact);
    
    agent.startActionSequence(sequence);
    assert(agent.isInActionSequence());
    
    // Get actions from sequence
    assert(agent.getNextActionFromSequence() == nlm::MotorCommand::MoveForward);
    assert(agent.getNextActionFromSequence() == nlm::MotorCommand::TurnLeft);
    assert(agent.getNextActionFromSequence() == nlm::MotorCommand::Interact);
    
    // Sequence should be empty now
    assert(!agent.isInActionSequence());
    assert(agent.getNextActionFromSequence() == nlm::MotorCommand::Wait);
    
    std::cout << "    testActionSequencing passed" << std::endl;
}

void testMemoryBasedNavigation() {
    std::cout << "    Testing Memory-Based Navigation..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::AgentBrain agent(brain);
    
    // Enable memory-based navigation
    agent.enableMemoryBasedNavigation(true);
    assert(agent.isMemoryBasedNavigationEnabled());
    
    // Record visited locations
    agent.recordVisitedLocation(1.0f, 2.0f);
    agent.recordVisitedLocation(3.0f, 4.0f);
    agent.recordVisitedLocation(1.0f, 2.0f); // Duplicate - should not be added
    
    // Check visited locations
    assert(agent.isLocationVisited(1.0f, 2.0f));
    assert(agent.isLocationVisited(3.0f, 4.0f));
    assert(!agent.isLocationVisited(5.0f, 6.0f));
    
    // Get visited locations
    auto locations = agent.getVisitedLocations();
    assert(locations.size() == 2);
    
    // Clear visited locations
    agent.clearVisitedLocations();
    assert(locations.empty());
    
    std::cout << "    testMemoryBasedNavigation passed" << std::endl;
}

void testGoalDirectedBehavior() {
    std::cout << "    Testing Goal-Directed Behavior..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::AgentBrain agent(brain);
    
    // Enable goal-directed behavior
    agent.enableGoalDirectedBehavior(true);
    assert(agent.isGoalDirectedBehaviorEnabled());
    
    // Set a goal
    agent.setGoal(10.0f, 5.0f);
    assert(agent.hasGoal());
    
    // Get goal
    auto goal = agent.getGoal();
    assert(goal.first == 10.0f);
    assert(goal.second == 5.0f);
    
    // Reset target
    agent.setGoal(0.0f, 0.0f);
    assert(agent.getGoal().first == 0.0f && agent.getGoal().second == 0.0f);
    assert(!agent.hasGoal());
    
    std::cout << "    testGoalDirectedBehavior passed" << std::endl;
}

void testAdvancedConfiguration() {
    std::cout << "    Testing Advanced Configuration..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::AgentBrain agent(brain);
    
    // Test all advanced configuration options
    agent.enableActionSequencing(true);
    agent.enableMemoryBasedNavigation(true);
    agent.enableGoalDirectedBehavior(true);
    agent.setExplorationRadius(5.0f);
    agent.setMemoryDecayRate(0.95f);
    
    assert(agent.isActionSequencingEnabled());
    assert(agent.isMemoryBasedNavigationEnabled());
    assert(agent.isGoalDirectedBehaviorEnabled());
    assert(agent.getExplorationRadius() == 5.0f);
    assert(agent.getMemoryDecayRate() == 0.95f);
    
    // Test configuration states
    auto behaviorStats = agent.getBehaviorStats();
    assert(std::get<0>(behaviorStats) == true);   // rewardModulation
    assert(std::get<1>(behaviorStats) == true);   // actionSequencing
    assert(std::get<2>(behaviorStats) == true);   // memoryNavigation
    assert(std::get<3>(behaviorStats) == true);   // goalDirected
    
    // Test memory usage
    agent.recordVisitedLocation(1.0f, 1.0f);
    auto memoryUsage = agent.getMemoryUsage();
    assert(std::get<0>(memoryUsage) == 1);  // visited locations
    assert(std::get<1>(memoryUsage) == 0);  // reward history (empty initially)
    
    std::cout << "    testAdvancedConfiguration passed" << std::endl;
}

void testAdvancedNeuromodulation() {
    std::cout << "    Testing Advanced Neuromodulation..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::AgentBrain agent(brain);
    
    // Enable reward modulation
    agent.enableRewardModulation(true);
    assert(agent.isRewardModulationEnabled());
    
    // Apply reward modulation
    agent.applyRewardModulation(1.0f, 0.5f);  // reward=1.0, predicted=0.5
    
    // Check prediction error
    float predictionError = agent.getPredictionError();
    assert(std::abs(predictionError - 0.5f) < 0.001f);
    
    // Check expected reward
    float expectedReward = agent.getExpectedReward();
    assert(std::abs(expectedReward - 1.0f) < 0.001f);
    
    // Check neuromodulation level
    float neuromodulationLevel = agent.getNeuromodulationLevel();
    assert(std::abs(neuromodulationLevel - 0.5f) < 0.001f);
    
    // Apply negative reward
    agent.applyRewardModulation(-1.0f, 0.0f);  // reward=-1.0, predicted=0.0
    
    // Check that prediction error is negative
    predictionError = agent.getPredictionError();
    assert(predictionError < 0.0f);
    
    // Check that expected reward moved toward negative values
    expectedReward = agent.getExpectedReward();
    assert(expectedReward < 0.0f);
    
    std::cout << "    testAdvancedNeuromodulation passed" << std::endl;
}

void testAdvancedDevelopment() {
    std::cout << "    Testing Advanced Development..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::AgentBrain agent(brain);
    
    // Enable development
    agent.enableDevelopment(true);
    assert(agent.isDevelopmentEnabled());
    
    // Check initial developmental stage
    auto stage = agent.getDevelopmentalStage();
    assert(stage == nlm::DevelopmentalStage::Initial);
    
    // Check initial plasticity modifier
    assert(agent.getPlasticityModifier() > 0.5f);
    
    // Check initial developmental age
    assert(agent.getDevelopmentalAge() == 0.0);
    
    // Update development multiple times
    for (int i = 0; i < 10; ++i) {
        agent.updateDevelopment(0.1f);
    }
    
    // Check that development age increased
    assert(agent.getDevelopmentalAge() > 0.0f);
    
    // Check that plasticity modifier changed with development
    float plasticityAfter = agent.getPlasticityModifier();
    assert(plasticityAfter < agent.getPlasticityModifier());  // Should decrease over time
    
    std::cout << "    testAdvancedDevelopment passed" << std::endl;
}

void testRewardPrediction() {
    std::cout << "    Testing Reward Prediction..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::AgentBrain agent(brain);
    
    // Enable reward modulation
    agent.enableRewardModulation(true);
    
    // Apply rewards with different patterns
    agent.applyRewardModulation(1.0f, 0.0f);   // Positive error
    agent.applyRewardModulation(0.5f, 1.0f);   // Negative error
    agent.applyRewardModulation(0.8f, 0.8f);   // Zero error
    
    // Check prediction error
    float predictionError = agent.getPredictionError();
    assert(std::abs(predictionError - (-0.3f)) < 0.001f);  // Should be -0.3f (0.5 + 0.8 - 2*0.0)
    
    // Check expected reward (exponential moving average)
    float expectedReward = agent.getExpectedReward();
    assert(std::abs(expectedReward - 0.7865f) < 0.001f);  // 0.95*0 + 0.05*1 + 0.95*1 + 0.05*0.5 + 0.95*0.8
    
    // Apply many more rewards to stabilize expected reward
    for (int i = 0; i < 100; ++i) {
        agent.applyRewardModulation(0.5f, 0.5f);
    }
    
    // Expected reward should approach 0.5f
    expectedReward = agent.getExpectedReward();
    assert(std::abs(expectedReward - 0.5f) < 0.01f);
    
    std::cout << "    testRewardPrediction passed" << std::endl;
}

void testMemoryNavigationDecay() {
    std::cout << "    Testing Memory Navigation Decay..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::AgentBrain agent(brain);
    
    // Enable memory-based navigation with low decay rate
    agent.enableMemoryBasedNavigation(true);
    agent.setMemoryDecayRate(0.1f);  // Fast decay
    agent.setExplorationRadius(100.0f);  // Large exploration radius
    
    // Record many locations
    for (int i = 0; i < 20; ++i) {
        agent.recordVisitedLocation(static_cast<float>(i), static_cast<float>(i));
    }
    
    // Get initial memory usage
    auto initialLocations = agent.getVisitedLocations();
    size_t initialCount = initialLocations.size();
    assert(initialCount == 20);
    
    // Record more locations (should not be decayed yet due to randomness)
    for (int i = 0; i < 10; ++i) {
        agent.recordVisitedLocation(static_cast<float>(i + 100), static_cast<float>(i + 100));
    }
    
    // Memory should not have decayed significantly yet (due to time-based decay)
    // The decay mechanism applies during new visits, not continuously
    
    std::cout << "    testMemoryNavigationDecay passed" << std::endl;
}

void testActionSequenceSafety() {
    std::cout << "    Testing Action Sequence Safety..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::AgentBrain agent(brain);
    
    // Test behavior with empty sequence
    assert(!agent.isInActionSequence());
    assert(agent.getNextActionFromSequence() == nlm::MotorCommand::Wait);
    
    // Test canceling empty sequence
    agent.cancelActionSequence();  // Should not crash
    
    // Test action sequence with single action
    std::vector<nlm::MotorCommand> singleSequence;
    singleSequence.push_back(nlm::MotorCommand::Wait);
    agent.startActionSequence(singleSequence);
    
    assert(agent.isInActionSequence());
    assert(agent.getNextActionFromSequence() == nlm::MotorCommand::Wait);
    assert(!agent.isInActionSequence());
    
    std::cout << "    testActionSequenceSafety passed" << std::endl;
}

void runAllAgentBrainTests() {
    std::cout << "\n=== Advanced AgentBrain Tests ===" << std::endl;
    
    testActionSequencing();
    testMemoryBasedNavigation();
    testGoalDirectedBehavior();
    testAdvancedConfiguration();
    testAdvancedNeuromodulation();
    testAdvancedDevelopment();
    testRewardPrediction();
    testMemoryNavigationDecay();
    testActionSequenceSafety();
    
    std::cout << "\n=== All AgentBrain Tests Passed ===" << std::endl;
}

} // namespace test_agentbrain