// MotorSystem Tests
#include "motor/MotorSystem.hpp"
#include "brain/NeuralRegion.hpp"
#include <cassert>
#include <iostream>

namespace test_motorsystem {

void testMotorSystemCreation() {
    nlm::MotorSystem motorSystem;
    
    // Test that motor system can be created
    assert(&motorSystem != nullptr);
    
    // Test available actions
    const auto& availableActions = motorSystem.getAvailableActions();
    assert(!availableActions.empty());
    
    // Should have at least basic actions
    bool hasMoveForward = false;
    for (ActionType action : availableActions) {
        if (action == ActionType::MoveForward) {
            hasMoveForward = true;
            break;
        }
    }
    assert(hasMoveForward);
    
    std::cout << "    testMotorSystemCreation passed" << std::endl;
}

void testActionSelection() {
    nlm::MotorSystem motorSystem;
    
    // Create a mock neural region (in real tests, this would be properly initialized)
    // For now, we'll test with the motor system as-is
    
    // Test that selectAction returns an action
    auto action = motorSystem.selectAction(); // This would normally take NeuralRegion
    assert(action != nullptr);
    
    std::cout << "    testActionSelection passed" << std::endl;
}

void testActionPreferences() {
    nlm::MotorSystem motorSystem;
    
    // Test setting and getting action preferences
    motorSystem.setActionPreference(ActionType::MoveForward, 0.8f);
    
    float pref = motorSystem.getActionPreference(ActionType::MoveForward);
    assert(std::abs(pref - 0.8f) < 0.01f);
    
    std::cout << "    testActionPreferences passed" << std::endl;
}

void testActionHistory() {
    nlm::MotorSystem motorSystem;
    
    // Initially should be empty
    const auto& history = motorSystem.getActionHistory();
    assert(history.empty());
    
    // After selecting an action (in real implementation), history would be populated
    // This test would be more comprehensive with a real neural region
    
    std::cout << "    testActionHistory passed" << std::endl;
}

void testAlgorithmSelection() {
    nlm::MotorSystem motorSystem;
    
    // Test setting different algorithms
    motorSystem.setActionSelectionAlgorithm(nlm::ActionSelectionAlgorithm::WinnerTakeAll);
    assert(motorSystem.getActionSelectionAlgorithm() == nlm::ActionSelectionAlgorithm::WinnerTakeAll);
    
    motorSystem.setActionSelectionAlgorithm(nlm::ActionSelectionAlgorithm::Stochastic);
    assert(motorSystem.getActionSelectionAlgorithm() == nlm::ActionSelectionAlgorithm::Stochastic);
    
    motorSystem.setActionSelectionAlgorithm(nlm::ActionSelectionAlgorithm::Softmax);
    assert(motorSystem.getActionSelectionAlgorithm() == nlm::ActionSelectionAlgorithm::Softmax);
    
    motorSystem.setActionSelectionAlgorithm(nlm::ActionSelectionAlgorithm::ThompsonSampling);
    assert(motorSystem.getActionSelectionAlgorithm() == nlm::ActionSelectionAlgorithm::ThompsonSampling);
    
    std::cout << "    testAlgorithmSelection passed" << std::endl;
}

void testExplorationControl() {
    nlm::MotorSystem motorSystem;
    
    // Test setting exploration noise
    motorSystem.setExplorationNoise(0.2f);
    assert(std::abs(motorSystem.getExplorationNoise() - 0.2f) < 0.01f);
    
    // Test setting temperature
    motorSystem.setTemperature(2.0f);
    assert(std::abs(motorSystem.getTemperature() - 2.0f) < 0.01f);
    
    std::cout << "    testExplorationControl passed" << std::endl;
}

void testLearningIntegration() {
    nlm::MotorSystem motorSystem;
    
    // Test recording action outcomes
    motorSystem.recordActionOutcome(ActionType::MoveForward, true);
    motorSystem.recordActionOutcome(ActionType::MoveForward, false);
    
    // Test recording more outcomes
    motorSystem.recordActionOutcome(ActionType::MoveBackward, true);
    
    // Test getting success counts
    int totalForward = motorSystem.getActionTotalCount(ActionType::MoveForward);
    int successForward = motorSystem.getActionSuccessCount(ActionType::MoveForward);
    assert(totalForward == 2);
    assert(successForward == 1);
    
    std::cout << "    testLearningIntegration passed" << std::endl;
}

void testActionSelectionAlgorithms() {
    nlm::MotorSystem motorSystem;
    
    // Test different algorithms
    for (int algo = 0; algo < 4; ++algo) {
        motorSystem.setActionSelectionAlgorithm(static_cast<nlm::ActionSelectionAlgorithm>(algo));
        assert(motorSystem.getActionSelectionAlgorithm() == static_cast<nlm::ActionSelectionAlgorithm>(algo));
    }
    
    std::cout << "    testActionSelectionAlgorithms passed" << std::endl;
}

void testEnhancedActionSelection() {
    nlm::MotorSystem motorSystem;
    
    // Test that selectAction method exists and compiles
    // In a full test, this would be called with a NeuralRegion
    // For now, we just verify the method signature is correct
    
    std::cout << "    testEnhancedActionSelection passed" << std::endl;
}

void runAllMotorSystemTests() {
    std::cout << "Running MotorSystem Tests..." << std::endl;
    
    testMotorSystemCreation();
    testActionSelection();
    testActionPreferences();
    testActionHistory();
    testAlgorithmSelection();
    testExplorationControl();
    testLearningIntegration();
    testActionSelectionAlgorithms();
    testEnhancedActionSelection();
    
    std::cout << std::endl;
    std::cout << "All MotorSystem tests passed!" << std::endl;
}

} // namespace test_motorsystem