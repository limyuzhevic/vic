// Advanced Agent Behaviors Tests - Phase 4 Extensions
// Tests for all 8 advanced agent behaviors

#include "../tests/test_advanced_behaviors.cpp"
#include "behaviors/Behaviors.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <memory>
#include <algorithm>

namespace nlm {

// Mock classes for testing
class MockRandomGenerator {
public:
    float uniformReal(float min, float max) { return 0.5f; }
    int uniformInt(int min, int max) { return min; }
    float bernoulli(float p) { return p > 0.5f ? 1.0f : 0.0f; }
};

class MockBrain {
public:
    std::shared_ptr<MockRandomGenerator> getRandomGenerator() {
        return std::make_shared<MockRandomGenerator>();
    }
};

class MockAgentBrain {
public:
    MockAgentBrain() {
        testState = std::vector<float>(100, 0.5f);
    }
    
    std::vector<float> getSensoryInputSize() const { 
        return testState; 
    }
    
    float getNeuromodulationLevel() const { return 0.5f; }
    
    MockBrain mockBrain;
    std::vector<float> testState;
    Brain* getBrain() { return brain_.get(); }
    std::shared_ptr<Brain> brain_ = std::make_shared<Brain>();
};

class MockMemory {
public:
    MockMemory() = default;
    
    void storeEpisode(const EpisodicMemoryItem& episode) {
        episodes.push_back(episode);
    }
    
    std::vector<const EpisodicMemoryItem*> retrieveSimilar(
        const std::vector<float>& pattern, 
        size_t maxResults) const {
        return {};
    }
    
    size_t getEpisodeCount() const { return episodes.size(); }
    
    std::vector<const EpisodicMemoryItem*> getRecentEpisodes(size_t count) const {
        return {};
    }
    
    std::vector<EpisodicMemoryItem> episodes;
};

class MockPlanner {
public:
    MockPlanner() = default;
    
    MotorCommand planAction(const std::vector<float>& currentState, 
                          float targetReward = 0.5f) {
        return MotorCommand::Wait;
    }
    
    ActionType getCurrentPlan() const { return ActionType::Wait; }
};

// Test 1: Memory-guided decision making
void testMemoryGuidedDecisionMaking() {
    std::cout << "Testing Memory-guided decision making..." << std::endl;
    
    MockAgentBrain mockBrain;
    MockMemory mockMemory;
    MockPlanner mockPlanner;
    
    // Test memory-guided behavior
    MemoryGuidedBehavior behavior;
    behavior.initialize(&mockBrain, &mockMemory, &mockPlanner, mockBrain.getBrain().get());
    behavior.setEnabled(true);
    
    // Record some experiences
    std::vector<float> state1(10, 0.3f);
    std::vector<float> state2(10, 0.7f);
    behavior.recordExperience(ActionType::MoveForward, state1, state2, 0.8f);
    
    // Test action recommendation based on memory
    MotorCommand cmd = behavior.recommendAction(state1, ActionType::MoveForward, 0.5f);
    
    // Verify behavior functions
    assert(behavior.getName() == "MemoryGuided");
    assert(mockMemory.getEpisodeCount() == 1);
    
    std::cout << "✓ Memory-guided decision making tests passed" << std::endl;
}

// Test 2: Social learning
void testSocialLearning() {
    std::cout << "Testing Social learning..." << std::endl;
    
    MockAgentBrain mockBrain;
    MockMemory mockMemory;
    MockPlanner mockPlanner;
    
    SocialLearningBehavior behavior;
    behavior.initialize(&mockBrain, &mockMemory, &mockPlanner, mockBrain.getBrain().get());
    behavior.setEnabled(true);
    
    // Record observations
    std::vector<float> observerState(10, 0.4f);
    std::vector<float> resultingState(10, 0.8f);
    behavior.recordObservation(ActionType::TurnLeft, observerState, resultingState, 0.7f);
    
    // Test imitation capability
    assert(behavior.canImitate(ActionType::TurnLeft) == true);
    
    // Test action selection
    ActionType action = behavior.getImitationAction(observerState);
    assert(action == ActionType::TurnLeft || action == ActionType::Wait);
    
    std::cout << "✓ Social learning tests passed" << std::endl;
}

// Test 3: Meta-cognition
void testMetaCognition() {
    std::cout << "Testing Meta-cognition..." << std::endl;
    
    MockAgentBrain mockBrain;
    MockMemory mockMemory;
    MockPlanner mockPlanner;
    
    MetaCognitionBehavior behavior;
    behavior.initialize(&mockBrain, &mockMemory, &mockPlanner, mockBrain.getBrain().get());
    behavior.setEnabled(true);
    
    // Test prediction recording
    std::vector<float> state(10, 0.5f);
    behavior.recordPredictionOutcome(
        ActionType::MoveForward,
        ActionType::MoveForward,
        0.5f,
        0.6f,
        state
    );
    
    // Test confidence retrieval
    float confidence = behavior.getActionConfidence(ActionType::MoveForward);
    assert(confidence >= 0.0f);
    
    // Test metacognitive confidence
    float metaConf = behavior.getMetaCognitiveConfidence();
    assert(metaConf >= 0.0f);
    
    std::cout << "✓ Meta-cognition tests passed" << std::endl;
}

// Test 4: Curiosity-driven exploration
void testCuriosityDrivenExploration() {
    std::cout << "Testing Curiosity-driven exploration..." << std::endl;
    
    MockAgentBrain mockBrain;
    MockMemory mockMemory;
    MockPlanner mockPlanner;
    
    CuriosityDrivenBehavior behavior;
    behavior.initialize(&mockBrain, &mockMemory, &mockPlanner, mockBrain.getBrain().get());
    behavior.setEnabled(true);
    
    // Test curiosity update
    behavior.updateCuriosity(1.0);
    
    // Test curiosity level
    float curiosity = behavior.getCuriosityLevel();
    assert(curiosity >= 0.0f && curiosity <= 1.0f);
    
    // Test curiosity parameters
    behavior.setCuriosityParameters(0.9f, 5.0f);
    
    std::cout << "✓ Curiosity-driven exploration tests passed" << std::endl;
}

// Test 5: Reward shaping
void testAdvancedRewardShaping() {
    std::cout << "Testing Advanced Reward Shaping..." << std::endl;
    
    RewardShapingBehavior behavior;
    assert(behavior.getName() == "RewardShaping");
    
    MockAgentBrain mockBrain;
    MockMemory mockMemory;
    MockPlanner mockPlanner;
    
    behavior.initialize(&mockBrain, &mockMemory, &mockPlanner, mockBrain.getBrain().get());
    behavior.setEnabled(true);
    
    // Test complex reward shaping components
    float (*complexComponent)(const std::vector<float>&, const ActionType&) = 
        [](const std::vector<float>& state, const ActionType& action) {
            float baseValue = 0.5f;
            // Add complexity based on state
            for (float val : state) {
                baseValue += std::abs(val - 0.5f) * 0.1f;
            }
            return std::min(1.0f, baseValue);
        };
    
    behavior.addShapingComponent(complexComponent, 1.5f);
    behavior.setBaseRewardFunction(
        [](const std::vector<float>&, const ActionType&) { return 0.3f; }
    );
    
    // Test update with shaped reward
    MotorCommand cmd = behavior.update(1.0);
    
    std::cout << "✓ Advanced Reward Shaping tests passed" << std::endl;
}

// Test 6: Transfer learning
void testAdvancedTransferLearning() {
    std::cout << "Testing Advanced Transfer Learning..." << std::endl;
    
    TransferLearningBehavior behavior;
    assert(behavior.getName() == "TransferLearning");
    
    MockAgentBrain mockBrain;
    MockMemory mockMemory;
    MockPlanner mockPlanner;
    
    behavior.initialize(&mockBrain, &mockMemory, &mockPlanner, mockBrain.getBrain().get());
    behavior.setEnabled(true);
    
    // Register multiple skills with different patterns
    std::vector<float> pattern1(10, 0.3f);
    std::vector<float> pattern2(10, 0.7f);
    std::vector<float> pattern3(10, 0.5f);
    
    behavior.registerSkill("NavigationSkill", pattern1, ActionType::MoveForward);
    behavior.registerSkill("ExplorationSkill", pattern2, ActionType::LookLeft);
    behavior.registerSkill("SocialSkill", pattern3, ActionType::Interact);
    
    // Test skill retrieval with similarity matching
    float confidence;
    MotorCommand action = behavior.findTransferSkill(pattern1, confidence);
    assert(action == MotorCommand::Wait || action == MotorCommand::MoveForward);
    
    // Test confidence calculation
    assert(confidence >= 0.0f);
    
    std::cout << "✓ Advanced Transfer Learning tests passed" << std::endl;
}

// Test 7: Hierarchical planning
void testAdvancedHierarchicalPlanning() {
    std::cout << "Testing Advanced Hierarchical Planning..." << std::endl;
    
    HierarchicalPlanningBehavior behavior;
    assert(behavior.getName() == "HierarchicalPlanning");
    
    MockAgentBrain mockBrain;
    MockMemory mockMemory;
    MockPlanner mockPlanner;
    
    behavior.initialize(&mockBrain, &mockMemory, &mockPlanner, mockBrain.getBrain().get());
    behavior.setEnabled(true);
    
    // Add multiple high-level goals
    std::vector<float> goal1(5, 0.8f);
    std::vector<float> goal2(5, 0.4f);
    std::vector<float> goal3(5, 0.9f);
    
    behavior.addHighLevelGoal(goal1, 2.0f);
    behavior.addHighLevelGoal(goal2, 0.5f);
    behavior.addHighLevelGoal(goal3, 1.5f);
    
    // Test planning confidence
    float confidence = behavior.getPlanningConfidence();
    assert(confidence >= 0.0f);
    
    // Test plan decomposition
    std::vector<ActionType> plan = behavior.decomposeGoal(goal1);
    assert(plan.size() > 0);
    
    // Test current plan management
    std::vector<ActionType> currentPlan = behavior.getCurrentPlan();
    
    std::cout << "✓ Advanced Hierarchical Planning tests passed" << std::endl;
}

// Test 8: Meta-reasoning
void testMetaReasoning() {
    std::cout << "Testing Meta-reasoning..." << std::endl;
    
    // Meta-reasoning is tested through integration with other behaviors
    MockAgentBrain mockBrain;
    MockMemory mockMemory;
    MockPlanner mockPlanner;
    
    // Test that meta-cognition behavior provides meta-reasoning capabilities
    MetaCognitionBehavior metaBehavior;
    metaBehavior.initialize(&mockBrain, &mockMemory, &mockPlanner, mockBrain.getBrain().get());
    metaBehavior.setEnabled(true);
    
    std::vector<float> state(10, 0.5f);
    metaBehavior.recordPredictionOutcome(
        ActionType::MoveForward,
        ActionType::Wait,
        0.5f,
        0.8f,
        state
    );
    
    // Meta-reasoning confidence should reflect prediction accuracy
    float confidence = metaBehavior.getActionConfidence(ActionType::MoveForward);
    float metaConfidence = metaBehavior.getMetaCognitiveConfidence();
    
    assert(confidence >= 0.0f);
    assert(metaConfidence >= 0.0f);
    
    std::cout << "✓ Meta-reasoning tests passed" << std::endl;
}

void runAllAdvancedAgentBehaviorTests() {
    std::cout << "Running Advanced Agent Behavior Tests..." << std::endl << std::endl;
    
    try {
        testMemoryGuidedDecisionMaking();
        testSocialLearning();
        testMetaCognition();
        testCuriosityDrivenExploration();
        testAdvancedRewardShaping();
        testAdvancedTransferLearning();
        testAdvancedHierarchicalPlanning();
        testMetaReasoning();
        
        std::cout << std::endl;
        std::cout << "All Advanced Agent Behavior tests passed! ✓" << std::endl;
        std::cout << "8 advanced behaviors tested successfully." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        throw;
    }
}

} // namespace nlm
