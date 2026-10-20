// Motor System Tests
// Tests for motor system, actions, motor commands

#include "motor/MotorSystem.hpp"
#include "motor/Action.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_motor {

void testMotorSystem() {
    auto config = std::make_shared<nlm::Config>();
    
    nlm::MotorSystem motor(config);
    
    // Test basic operations
    assert(motor.getMotorCount() > 0);
    assert(motor.getMaxForce() > 0.0f);
    assert(motor.getMaxTorque() > 0.0f);
    
    // Test action generation
    std::vector<float> state{0.5f, 0.3f, 0.8f};
    nlm::Action action = motor.generateAction(state);
    
    assert(action.getType() != nlm::ActionType::Custom); // Not custom by default
    
    std::cout << "    testMotorSystem passed" << std::endl;
}

void testAction() {
    auto config = std::make_shared<nlm::Config>();
    
    nlm::Action action(nlm::ActionType::MoveForward);
    
    // Test basic operations
    assert(action.getType() == nlm::ActionType::MoveForward);
    assert(action.getParameters().size() == 0);
    
    // Test parameter setting
    action.setParameters(std::vector<float>{1.0f, 0.5f});
    assert(action.getParameters().size() == 2);
    assert(action.getParameters()[0] == 1.0f);
    
    // Test action cloning
    nlm::Action action_copy = action.clone();
    assert(action_copy.getType() == action.getType());
    assert(action_copy.getParameters() == action.getParameters());
    
    std::cout << "    testAction passed" << std::endl;
}

void runAll() {
    testMotorSystem();
    testAction();
}

} // namespace test_motor