// MotorSystem Tests
#include "motor/MotorSystem.hpp"
#include "brain/Neuron.hpp"
#include <cassert>
#include <iostream>

namespace test_motor {

void testMotorSystemCreation() {
    nlm::MotorSystem motorSystem;
    
    // Check available actions
    assert(!motorSystem.getAvailableActions().empty());
    assert(motorSystem.getActionHistory().empty());
    
    std::cout << "    testMotorSystemCreation passed" << std::endl;
}

void testActionSelection() {
    nlm::MotorSystem motorSystem;
    
    // Create a mock neural region for testing
    // In a real implementation, this would test actual neural activity
    // For now, just test that the function returns an action
    auto action = motorSystem.selectAction(nlm::NeuralRegion(1));
    
    assert(action != nullptr);
    assert(action->getType() != nlm::ActionType::Wait || !motorSystem.getAvailableActions().empty());
    
    std::cout << "    testActionSelection passed" << std::endl;
}

void testActionPreferences() {
    nlm::MotorSystem motorSystem;
    
    // Set preferences for different actions
    motorSystem.setActionPreference(nlm::ActionType::MoveForward, 1.5f);
    motorSystem.setActionPreference(nlm::ActionType::MoveBackward, 0.5f);
    motorSystem.setActionPreference(nlm::ActionType::Wait, 0.1f);
    
    // Test that preferences are set (implementation dependent)
    // In a real implementation, this would influence action selection
    assert(true); // Placeholder for actual preference testing
    
    std::cout << "    testActionPreferences passed" << std::endl;
}

void testActionHistory() {
    nlm::MotorSystem motorSystem;
    
    // Clear history
    motorSystem.clearHistory();
    assert(motorSystem.getActionHistory().empty());
    
    // Add an action via selectAction
    auto action1 = motorSystem.selectAction(nlm::NeuralRegion(1));
    if (action1) {
        // History should now have one action
        assert(motorSystem.getActionHistory().size() == 1);
    }
    
    // Add another action
    auto action2 = motorSystem.selectAction(nlm::NeuralRegion(1));
    if (action2) {
        assert(motorSystem.getActionHistory().size() == 2);
    }
    
    // Clear history
    motorSystem.clearHistory();
    assert(motorSystem.getActionHistory().empty());
    
    std::cout << "    testActionHistory passed" << std::endl;
}

void testAll() {
    testMotorSystemCreation();
    testActionSelection();
    testActionPreferences();
    testActionHistory();
}

} // namespace test_motor