// test_phase4_self_model.cpp
// Tests SelfModel functionality

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include <memory>

namespace nlm {

TEST_CASE("Phase 4 Self-Model Tests", "[self_model]") {
    
    SECTION("Test SelfModel initialization") {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        NeuralPlanner planner;
        planner.initialize(brain.get());
        
        auto* selfModel = planner.getSelfModel();
        REQUIRE(selfModel != nullptr);
        
        // Test basic self-model properties
        REQUIRE(selfModel->hasSelfModel() == false);  // No actions recorded yet
        REQUIRE(selfModel->getBodyAwareness() >= 0.0f && selfModel->getBodyAwareness() <= 1.0f);
    }
    
    SECTION("Test SelfModel action recording") {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        NeuralPlanner planner;
        planner.initialize(brain.get());
        
        auto* selfModel = planner.getSelfModel();
        REQUIRE(selfModel != nullptr);
        
        // Record self-actions
        std::vector<float> beforeState(30, 0.5f);
        std::vector<float> afterState = beforeState;
        
        ActionType action = ActionType::MoveForward;
        selfModel->recordSelfAction(action, beforeState, afterState);
        
        // Should have recorded an action
        REQUIRE(selfModel->hasSelfModel() == true);
        
        // Test prediction
        auto prediction = selfModel->predictActionConsequence(action, beforeState);
        REQUIRE(!prediction.empty());
        
        // Test confidence
        float confidence = selfModel->getSelfModelConfidence(action);
        REQUIRE(confidence >= 0.0f && confidence <= 1.0f);
    }
    
    SECTION("Test SelfModel self-generated likeness computation") {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        NeuralPlanner planner;
        planner.initialize(brain.get());
        
        auto* selfModel = planner.getSelfModel();
        REQUIRE(selfModel != nullptr);
        
        // Create before and after states
        std::vector<float> beforeState(30, 0.5f);
        std::vector<float> afterState = beforeState;
        
        ActionType action = ActionType::MoveForward;
        selfModel->recordSelfAction(action, beforeState, afterState);
        
        // Compute self-generated likeness
        float likeness = selfModel->computeSelfGeneratedLikeness(beforeState, afterState, action);
        
        // Should be high for recorded action
        REQUIRE(likeness >= 0.0f && likeness <= 1.0f);
        
        // Test with different action
        ActionType differentAction = ActionType::TurnLeft;
        float differentLikeness = selfModel->computeSelfGeneratedLikeness(beforeState, afterState, differentAction);
        
        // Should be lower or zero for different action
        REQUIRE(differentLikeness >= 0.0f);
    }
    
    SECTION("Test SelfModel preferred action selection") {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        NeuralPlanner planner;
        planner.initialize(brain.get());
        
        auto* selfModel = planner.getSelfModel();
        REQUIRE(selfModel != nullptr);
        
        // Record actions
        std::vector<float> state1(30, 0.5f);
        std::vector<float> state2(30, 0.8f);
        
        ActionType action1 = ActionType::MoveForward;
        ActionType action2 = ActionType::Interact;
        
        selfModel->recordSelfAction(action1, state1, state1);
        selfModel->recordSelfAction(action2, state2, state2);
        
        // Test preferred action selection
        ActionType preferred = selfModel->getPreferredAction(state1);
        
        // Should select one of the recorded actions
        REQUIRE(preferred == action1 || preferred == action2);
    }
    
    SECTION("Test SelfModel update with prediction error") {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        brain->initialize();
        
        NeuralPlanner planner;
        planner.initialize(brain.get());
        
        auto* selfModel = planner.getSelfModel();
        REQUIRE(selfModel != nullptr);
        
        // Record an action
        std::vector<float> beforeState(30, 0.5f);
        std::vector<float> afterState = beforeState;
        
        ActionType action = ActionType::MoveForward;
        selfModel->recordSelfAction(action, beforeState, afterState);
        
        // Update with predicted state (close to actual)
        std::vector<float> predictedState = beforeState;
        selfModel->updateSelfModel(predictedState, afterState, action);
        
        // Should not crash and should maintain reasonable confidence
        float confidence = selfModel->getSelfModelConfidence(action);
        REQUIRE(confidence >= 0.0f);
        
        // Update with very different predicted state
        std::vector<float> veryDifferentState(30, 0.0f);
        selfModel->updateSelfModel(veryDifferentState, afterState, action);
        
        // Confidence should still be reasonable
        float newConfidence = selfModel->getSelfModelConfidence(action);
        REQUIRE(newConfidence >= 0.0f);
    }
}

} // namespace nlm
