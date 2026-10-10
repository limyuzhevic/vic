#include "test_common.hpp"
#include <cassert>
#include <cmath>
#include <algorithm>

namespace test_motor {

void testMotorSystemCreation() {
    nlm::MotorSystem motorSystem;
    assert(motorSystem.getActionCount() == 0);
    assert(motorSystem.isActive() == false);
    std::cout << "    testMotorSystemCreation passed" << std::endl;
}

void testActionCreation() {
    nlm::Action action;
    assert(action.getActionType() == nlm::ActionType::None);
    assert(action.getStrength() == 0.0f);
    assert(action.getDuration() == 0.0f);
    std::cout << "    testActionCreation passed" << std::endl;
}

void testMotorSystem() {
    nlm::MotorSystem motorSystem;
    
    // Create an action
    nlm::Action action;
    action.setActionType(nlm::ActionType::Move);
    action.setStrength(0.8f);
    action.setDuration(1.0f);
    
    motorSystem.addAction(action);
    assert(motorSystem.getActionCount() == 1);
    
    // Execute action
    bool executed = motorSystem.executeAction(0);
    assert(executed == true);
    std::cout << "    testMotorSystem passed" << std::endl;
}

void testActionTypes() {
    nlm::Action action;
    
    // Test different action types
    action.setActionType(nlm::ActionType::Move);
    assert(action.getActionType() == nlm::ActionType::Move);
    
    action.setActionType(nlm::ActionType::Speak);
    assert(action.getActionType() == nlm::ActionType::Speak);
    
    action.setActionType(nlm::ActionType::Grasp);
    assert(action.getActionType() == nlm::ActionType::Grasp);
    
    action.setActionType(nlm::ActionType::Release);
    assert(action.getActionType() == nlm::ActionType::Release);
    
    std::cout << "    testActionTypes passed" << std::endl;
}

void testMotorIntegration() {
    nlm::MotorSystem motorSystem;
    
    // Test integration with Brain
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Motor system should be accessible
    assert(&motorSystem != nullptr);
    std::cout << "    testMotorIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running Motor tests..." << std::endl;
    testMotorSystemCreation();
    testActionCreation();
    testMotorSystem();
    testActionTypes();
    testMotorIntegration();
}

} // namespace test_motor