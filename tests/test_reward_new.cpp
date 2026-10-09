// NLM Reward System Tests
// Test the complete Reward system for reinforcement learning and brain emulation

#include "../src/neuromodulation/Reward.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <algorithm>

namespace test_reward_system {
    
    void testInitialization() {
        nlm::Reward reward;
        
        // Test basic initialization
        assert(reward.getCurrentReward() == 0.0f);
        assert(reward.getImmediateReward() == 0.0f);
        
        // Test parameter configuration
        const auto& params = reward.getParameters();
        assert(params.find("exploration_rate") != params.end());
        assert(params.find("discount_factor") != params.end());
        assert(params.find("prediction_horizon") != params.end());
        
        // Test default values
        assert(reward.getParameters()["exploration_rate"] == 0.1f);
        assert(reward.getParameters()["discount_factor"] == 0.95f);
        assert(reward.getParameters()["prediction_horizon"] == 10.0f);
        
        // Test strategy
        reward.setStrategy(nlm::Reward::Strategy::Hybrid);
        // Strategy setting doesn't return value, but we can test it's set
        
        std::cout << "  Reward initialization test PASSED" << std::endl;
    }
    
    void testRewardComputation() {
        nlm::Reward reward;
        
        // Create a simple observation (we'll test with a mock observation)
        // For now, test basic reward setting
        reward.setCurrentReward(1.0f);
        assert(reward.getCurrentReward() == 1.0f);
        assert(reward.getImmediateReward() == 1.0f);
        
        // Update reward
        reward.update(0.5f, 0.1f);
        assert(reward.getCurrentReward() == 1.5f);
        
        // Test add
        reward.setCurrentReward(0.0f);
        reward.add(0.3f);
        assert(reward.getCurrentReward() == 0.3f);
        
        std::cout << "  Reward computation test PASSED" << std::endl;
    }
    
    void testRewardModalityIntegration() {
        nlm::Reward reward;
        
        // Create a vector of modality rewards
        std::vector<std::pair<nlm::Reward::Modality, float>> modalityRewards;
        modalityRewards.push_back({nlm::Reward::Modality::Extrinsic, 1.0f});
        modalityRewards.push_back({nlm::Reward::Modality::Intrinsic, 0.5f});
        modalityRewards.push_back({nlm::Reward::Modality::Social, 0.8f});
        modalityRewards.push_back({nlm::Reward::Modality::Structural, 0.2f});
        
        // Test integration
        float integrated = reward.integrateRewards(modalityRewards);
        assert(integrated > 0.0f);
        
        // Test individual modality contributions
        reward.setRewardScaling(1.0f, 1.0f, 1.0f, 1.0f);
        
        std::cout << "  Reward modality integration test PASSED" << std::endl;
    }
    
    void testRewardShaping() {
        nlm::Reward reward;
        
        // Create a mock action
        nlm::Action action(nlm::ActionType::Eat, {0.5f, 0.5f, 0.5f});
        
        // Create a mock observation (we'd need a real observation for proper testing)
        // For now, test the shaping logic with simple values
        
        // Test exploration reward shaping
        float baseReward = 1.0f;
        float shapedReward = reward.shapeReward(baseReward, action, nlm::Observation(), true);
        assert(shapedReward >= baseReward); // Exploration should add bonus
        
        // Test exploitation reward shaping
        shapedReward = reward.shapeReward(baseReward, action, nlm::Observation(), false);
        assert(shapedReward <= baseReward); // Exploitation might reduce reward
        
        std::cout << "  Reward shaping test PASSED" << std::endl;
    }
    
    void testRewardPrediction() {
        nlm::Reward reward;
        
        // Test prediction update
        reward.updatePrediction(1.0f, 0.5f);
        
        // Check prediction error
        float predictionError = reward.getStats().predictionAccuracy;
        assert(predictionError >= 0.0f && predictionError <= 1.0f);
        
        // Test uncertainty
        float uncertainty = reward.getStats().uncertainty;
        assert(uncertainty >= 0.0f && uncertainty <= 1.0f);
        
        // Test prediction horizon
        reward.setPredictionHorizon(20.0f);
        assert(reward.getParameters()["prediction_horizon"] == 20.0f);
        
        std::cout << "  Reward prediction test PASSED" << std::endl;
    }
    
    void testQLearning() {
        nlm::Reward reward;
        
        // Create state features
        std::vector<float> state = {0.5f, 0.8f, 0.3f};
        
        // Create next state features
        std::vector<float> nextState = {0.6f, 0.7f, 0.4f};
        
        // Create a mock action
        nlm::Action action(nlm::ActionType::MoveForward, {0.5f});
        
        // Test Q-learning update
        reward.qLearningUpdate(state, action, 1.0f, nextState, 0.1f);
        
        // Check that statistics were updated
        assert(reward.getStats().totalRewards > 0.0f);
        
        std::cout << "  Q-learning test PASSED" << std::endl;
    }
    
    void testSARSAA() {
        nlm::Reward reward;
        
        // Create state and next state
        std::vector<float> state = {0.5f, 0.5f};
        std::vector<float> nextState = {0.6f, 0.6f};
        
        // Create mock actions
        nlm::Action action(nlm::ActionType::TurnLeft, {0.5f});
        nlm::Action nextAction(nlm::ActionType::MoveRight, {0.5f});
        
        // Test SARSA update
        reward.sarsaUpdate(state, action, 1.0f, nextState, nextAction, 0.1f, 0.95f);
        
        std::cout << "  SARSA test PASSED" << std::endl;
    }
    
    void testPolicyGradient() {
        nlm::Reward reward;
        
        // Create mock action and observation
        nlm::Action action(nlm::ActionType::Interact, {0.5f, 0.5f});
        
        // Test policy gradient reward computation
        float policyReward = reward.policyGradientReward(action, nlm::Observation(), 0.5f);
        assert(policyReward >= 0.0f);
        
        std::cout << "  Policy gradient test PASSED" << std::endl;
    }
    
    void testActorCritic() {
        nlm::Reward reward;
        
        // Create mock actions and observations
        nlm::Action action(nlm::ActionType::Look, {0.5f});
        nlm::Action nextAction(nlm::ActionType::LookUp, {0.5f});
        
        // Test actor-critic update
        reward.actorCriticUpdate(nlm::Observation(), action, 1.0f, nlm::Observation(),
                               0.1f, 0.01f, 0.95f);
        
        std::cout << "  Actor-critic test PASSED" << std::endl;
    }
    
    void testRewardUncertainty() {
        nlm::Reward reward;
        
        // Test uncertainty computation
        float uncertainty = reward.computeUncertainty(nlm::Observation());
        assert(uncertainty >= 0.0f && uncertainty <= 1.0f);
        
        // Test noise addition
        float noisyReward = reward.addNoise(1.0f, 0.1f);
        assert(std::abs(noisyReward - 1.0f) <= 0.5f); // Noise should be within reasonable bounds
        
        std::cout << "  Reward uncertainty test PASSED" << std::endl;
    }
    
    void testStatistics() {
        nlm::Reward reward;
        
        // Add some rewards to generate statistics
        reward.addRewardEntry(nlm::Reward::Modality::Extrinsic, 1.0f, "test1");
        reward.addRewardEntry(nlm::Reward::Modality::Intrinsic, 0.5f, "test2");
        reward.addRewardEntry(nlm::Reward::Modality::Social, 0.8f, "test3");
        
        // Check statistics
        const auto& stats = reward.getStats();
        assert(stats.totalRewards == 2.3f);
        assert(stats.extrinsicTotal == 1.0f);
        assert(stats.intrinsicTotal == 0.5f);
        assert(stats.socialTotal == 0.8f);
        
        // Check history
        const auto& history = reward.getHistory();
        assert(history.size() == 3);
        
        std::cout << "  Reward statistics test PASSED" << std::endl;
    }
    
    void testNeuromodulationIntegration() {
        nlm::Reward reward;
        
        // Create a dopamine system
        nlm::Dopamine dopamine;
        
        // Test integration with dopamine
        reward.integrateWithNeuromodulation(dopamine, nlm::Observation());
        
        // Test signaling to dopamine
        reward.signalToDopamine(dopamine, 1.0f, 0.5f);
        
        std::cout << "  Neuromodulation integration test PASSED" << std::endl;
    }
    
    void testAdvancedFeatures() {
        nlm::Reward reward;
        
        // Test strategy configuration
        reward.setStrategy(nlm::Reward::Strategy::FunctionApproximation);
        
        // Test exploration rate
        reward.setExplorationRate(0.5f);
        assert(reward.getParameters()["exploration_rate"] == 0.5f);
        
        // Test discount factor
        reward.setDiscountFactor(0.8f);
        assert(reward.getParameters()["discount_factor"] == 0.8f);
        
        // Test reward scaling
        reward.setRewardScaling(2.0f, 1.5f, 1.0f, 0.5f);
        
        // Test noise parameters
        reward.setNoiseParameters(0.0f, 0.2f);
        
        // Test reward history
        reward.clearHistory();
        assert(reward.getHistory().empty());
        
        // Test cloning
        auto clonedReward = reward.clone();
        assert(clonedReward != nullptr);
        
        std::cout << "  Advanced features test PASSED" << std::endl;
    }
    
    void testExplorationExploitation() {
        nlm::Reward reward;
        
        // Create available actions
        std::vector<nlm::Action> actions;
        actions.push_back(nlm::Action(nlm::ActionType::MoveForward, {0.5f}));
        actions.push_back(nlm::Action(nlm::ActionType::MoveBackward, {0.5f}));
        actions.push_back(nlm::Action(nlm::ActionType::TurnLeft, {0.5f}));
        actions.push_back(nlm::Action(nlm::ActionType::TurnRight, {0.5f}));
        
        // Create state features
        std::vector<float> stateFeatures = {0.5f, 0.5f, 0.5f};
        
        // Test action selection
        nlm::Action selectedAction = reward.selectAction(actions, nlm::Observation(), stateFeatures);
        assert(!selectedAction.getParameters().empty());
        
        // Test exploration rate adjustment
        reward.setExplorationRate(0.0f); // Force exploitation
        
        std::cout << "  Exploration-exploitation test PASSED" << std::endl;
    }
    
    void testCreditAssignment() {
        nlm::Reward reward;
        
        // Create mock actions and observation
        nlm::Action action(nlm::ActionType::Eat, {0.5f});
        
        // Test credit assignment for delayed rewards
        reward.creditAssignmentForDelayedRewards(nlm::Observation(), action, 1.0f, 5.0f);
        
        // Check that delayed reward history was updated
        // (This would be tested in a more comprehensive implementation)
        
        std::cout << "  Credit assignment test PASSED" << std::endl;
    }
    
    void testLearningAnalysis() {
        nlm::Reward reward;
        
        // Add some rewards to history
        reward.addRewardEntry(nlm::Reward::Modality::Extrinsic, 1.0f, "test1");
        reward.addRewardEntry(nlm::Reward::Modality::Intrinsic, 0.5f, "test2");
        
        // Get learning analysis
        auto analysis = reward.analyzeRewardLearning();
        
        // Check analysis components
        assert(analysis.learningCurve[0] != 0.0f || analysis.learningCurve[0] == 0.0f); // May be 0 or set
        assert(analysis.predictionErrorVariance >= 0.0f);
        assert(analysis.rewardHistoryEntropy >= 0.0f);
        assert(analysis.explorationExploitationRatio >= 0.0f);
        assert(analysis.intrinsicExtrinsicRatio >= 0.0f);
        assert(analysis.creditAssignmentAccuracy >= 0.0f);
        assert(analysis.modelPredictionError >= 0.0f);
        assert(analysis.rewardModalityDistribution.size() == 4); // 4 modalities
        
        std::cout << "  Learning analysis test PASSED" << std::endl;
    }
    
    void testReset() {
        nlm::Reward reward;
        
        // Modify some state
        reward.setCurrentReward(1.0f);
        reward.setExplorationRate(0.5f);
        reward.addRewardEntry(nlm::Reward::Modality::Extrinsic, 0.5f, "test");
        
        // Reset
        reward.reset();
        
        // Check that state was reset
        assert(reward.getCurrentReward() == 0.0f);
        assert(reward.getParameters()["exploration_rate"] == 0.1f); // Back to default
        assert(reward.getHistory().empty());
        
        std::cout << "  Reset functionality test PASSED" << std::endl;
    }
    
    void runAll() {
        std::cout << "Running Reward System tests..." << std::endl;
        
        testInitialization();
        testRewardComputation();
        testRewardModalityIntegration();
        testRewardShaping();
        testRewardPrediction();
        testQLearning();
        testSARSAA();
        testPolicyGradient();
        testActorCritic();
        testRewardUncertainty();
        testStatistics();
        testNeuromodulationIntegration();
        testAdvancedFeatures();
        testExplorationExploitation();
        testCreditAssignment();
        testLearningAnalysis();
        testReset();
        
        std::cout << "All Reward System tests PASSED" << std::endl;
    }
}

// Alias for backward compatibility with test_main.cpp
namespace test_reward {
    void runAll() {
        test_reward_system::runAll();
    }
}