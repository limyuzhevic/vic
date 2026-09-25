// Cognition Integration Tests
// Tests NeuralPlanner, SelfModel, and SocialLearning integration

#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/AttentionalSelection.hpp"
#include "../src/cognition/SelfModel.hpp"
#include "../src/cognition/SocialLearning.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>

namespace test_cognition_integration {

void testNeuralPlannerBasic() {
    std::cout << "  Testing NeuralPlanner basic functionality..." << std::endl;
    
    NeuralPlanner planner;
    
    // Test initialization
    auto config = std::make_shared<Config>();
    Brain brain(config);
    brain.initialize();
    
    // Create test state
    std::vector<float> testState = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    
    // Test planAction
    ActionType action = planner.planAction(testState, 0.5f);
    // Should return a valid action type
    assert(action >= ActionType::MoveForward && action <= ActionType::Wait);
    
    // Test setCurrentGoal
    std::vector<float> goal = {0.5f, 0.5f, 0.5f, 0.5f, 0.5f};
    planner.setCurrentGoal(goal);
    
    // Test getCurrentGoal
    std::vector<float> retrievedGoal = planner.getCurrentGoal();
    assert(retrievedGoal.size() == goal.size());
    
    // Test getPlanningDepth
    size_t depth = planner.getPlanningDepth();
    assert(depth > 0);
    
    // Test getPlanningConfidence
    float confidence = planner.getPlanningConfidence();
    assert(confidence >= 0.0f && confidence <= 1.0f);
    
    std::cout << "    PASSED" << std::endl;
}

void testSelfModelBasic() {
    std::cout << "  Testing SelfModel basic functionality..." << std::endl;
    
    SelfModel selfModel;
    
    auto config = std::make_shared<Config>();
    Brain brain(config);
    brain.initialize();
    
    // Test initialization
    
    // Create test states
    std::vector<float> beforeState = {0.1f, 0.2f, 0.3f};
    std::vector<float> afterState = {0.15f, 0.25f, 0.35f};
    
    ActionType action = ActionType::MoveForward;
    
    // Test recordSelfAction
    selfModel.recordSelfAction(action, beforeState, afterState);
    
    // Test predictActionConsequence
    std::vector<float> predicted = selfModel.predictActionConsequence(action, beforeState);
    // Should return something (either matching or original state)
    assert(predicted.size() == beforeState.size());
    
    // Test getSelfModelConfidence
    float confidence = selfModel.getSelfModelConfidence(action);
    assert(confidence >= 0.0f && confidence <= 1.0f);
    
    // Test getCapabilityLevel
    float capability = selfModel.getCapabilityLevel();
    assert(capability >= 0.0f && capability <= 1.0f);
    
    // Test getBodyAwareness
    float awareness = selfModel.getBodyAwareness();
    assert(awareness >= 0.0f && awareness <= 1.0f);
    
    std::cout << "    PASSED" << std::endl;
}

void testSocialLearningBasic() {
    std::cout << "  Testing SocialLearning basic functionality..." << std::endl;
    
    SocialLearning socialLearning;
    
    auto config = std::make_shared<Config>();
    Brain brain(config);
    brain.initialize();
    
    // Test initialization
    
    // Create test states
    std::vector<float> observerState = {0.1f, 0.2f, 0.3f};
    std::vector<float> resultingState = {0.15f, 0.25f, 0.35f};
    
    ActionType observedAction = ActionType::MoveForward;
    
    // Test observeAgentAction
    socialLearning.observeAgentAction(observedAction, observerState, resultingState);
    
    // Test canImitate
    bool canImitate = socialLearning.canImitate(observedAction);
    assert(canImitate);  // Should be true since we just recorded it
    
    // Test getImitationAction
    ActionType imitation = socialLearning.getImitationAction(observerState);
    assert(imitation >= ActionType::MoveForward && imitation <= ActionType::Wait);
    
    // Test getObservationCount
    size_t count = socialLearning.getObservationCount();
    assert(count > 0);
    
    // Test getSignalMeaning
    float meaning = socialLearning.getSignalMeaning();
    assert(meaning >= 0.0f);
    
    std::cout << "    PASSED" << std::endl;
}

void testCognitionIntegrationWithBrain() {
    std::cout << "  Testing cognition integration with Brain..." << std::endl;
    
    auto config = std::make_shared<Config>();
    Brain brain(config);
    brain.initialize();
    
    // Get cognition systems
    auto* planner = brain.getPlanner();
    auto* selfModel = brain.getSelfModel();
    auto* socialLearning = brain.getSocialLearning();
    auto* attention = brain.getAttention();
    
    // Test that cognition systems are accessible
    if (planner) {
        std::vector<float> testState = {0.1f, 0.2f, 0.3f};
        ActionType action = planner->planAction(testState, 0.5f);
        assert(action >= ActionType::MoveForward && action <= ActionType::Wait);
    }
    
    if (selfModel) {
        std::vector<float> beforeState = {0.1f, 0.2f};
        std::vector<float> afterState = {0.15f, 0.25f};
        selfModel->recordSelfAction(ActionType::MoveForward, beforeState, afterState);
    }
    
    if (socialLearning) {
        socialLearning->observeAgentAction(ActionType::TurnLeft, {0.1f}, {0.2f});
    }
    
    if (attention) {
        std::vector<NeuronId> competitors;
        competitors.push_back(NeuronId(1000));
        competitors.push_back(NeuronId(2000));
        attention->processCompetition(competitors);
    }
    
    std::cout << "    PASSED" << std::endl;
}

void runAll() {
    std::cout << "=== Cognition Integration Tests ===" << std::endl;
    
    testNeuralPlannerBasic();
    testSelfModelBasic();
    testSocialLearningBasic();
    testCognitionIntegrationWithBrain();
    
    std::cout << "=== Cognition Integration Tests Complete ===" << std::endl;
}

} // namespace test_cognition_integration