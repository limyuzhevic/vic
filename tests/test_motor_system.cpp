#include <gtest/gtest.h>
#include "../motor/MotorSystem.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Neuron.hpp"

class MotorSystemTest : public ::testing::Test {
protected:
    nlm::MotorSystem motorSystem;
    std::vector<nlm::Neuron*> neurons;
    nlm::NeuralRegion motorRegion;
};

TEST_F(MotorSystemTest, GetAvailableActions) {
    const auto& actions = motorSystem.getAvailableActions();
    EXPECT_GT(actions.size(), 0U);
    
    // Check for some expected action types
    bool foundMoveForward = false;
    bool foundWait = false;
    
    for (const auto& action : actions) {
        if (action == nlm::ActionType::MoveForward) foundMoveForward = true;
        if (action == nlm::ActionType::Wait) foundWait = true;
    }
    
    EXPECT_TRUE(foundMoveForward);
    EXPECT_TRUE(foundWait);
}

TEST_F(MotorSystemTest, SetActionPreference) {
    motorSystem.setActionPreference(nlm::ActionType::MoveForward, 1.0f);
    
    // The preference is stored internally, so we can't directly test it
    // But we can test that the function doesn't crash
    motorSystem.setActionPreference(nlm::ActionType::TurnLeft, 0.0f);
    motorSystem.setActionPreference(nlm::ActionType::Wait, 0.5f);
}

TEST_F(MotorSystemTest, SelectActionWithEmptyRegion) {
    // Test with empty neural region
    auto action = motorSystem.selectAction(motorRegion);
    EXPECT_NE(action, nullptr);
    EXPECT_EQ(action->getType(), nlm::ActionType::Wait); // Fallback action
}

TEST_F(MotorSystemTest, ActionHistory) {
    auto action1 = std::make_unique<nlm::Action>(nlm::ActionType::MoveForward);
    
    // Manually add to history (since we can't easily call selectAction with a real region)
    // In real implementation, this would be done by the brain
    motorSystem.clearHistory();
    
    EXPECT_EQ(motorSystem.getActionHistory().size(), 0U);
}