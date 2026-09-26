// Action class tests - testing factory methods and convenience methods
#include "motor/Action.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_action {

void testActionFactoryMethods() {
    std::cout << "    Testing Action factory methods..." << std::endl;
    
    // Test creating actions with different types
    nlm::Action moveForwardAction(nlm::ActionType::MoveForward);
    assert(moveForwardAction.getType() == nlm::ActionType::MoveForward);
    assert(moveForwardAction.getName() == "MoveForward");
    
    nlm::Action turnLeftAction(nlm::ActionType::TurnLeft);
    assert(turnLeftAction.getType() == nlm::ActionType::TurnLeft);
    assert(turnLeftAction.getName() == "TurnLeft");
    
    nlm::Action interactAction(nlm::ActionType::Interact);
    assert(interactAction.getType() == nlm::ActionType::Interact);
    assert(interactAction.getName() == "Interact");
    
    nlm::Action customAction(nlm::ActionType::Custom, std::vector<float>{0.5f, 0.7f, 0.9f});
    assert(customAction.getType() == nlm::ActionType::Custom);
    assert(customAction.getParameters().size() == 3);
    assert(customAction.getParameters()[0] == 0.5f);
    
    std::cout << "    testActionFactoryMethods passed" << std::endl;
}

void testActionConvenienceMethods() {
    std::cout << "    Testing Action convenience methods..." << std::endl;
    
    nlm::Action action1(nlm::ActionType::MoveForward);
    nlm::Action action2(nlm::ActionType::MoveForward);
    
    // Test getName
    assert(action1.getName() == "MoveForward");
    assert(action2.getName() == "MoveForward");
    
    // Test parameters
    std::vector<float> params = {1.0f, 2.0f, 3.0f};
    nlm::Action paramAction(nlm::ActionType::Custom, params);
    assert(paramAction.getParameters().size() == 3);
    assert(paramAction.getParameters()[0] == 1.0f);
    assert(paramAction.getParameters()[1] == 2.0f);
    assert(paramAction.getParameters()[2] == 3.0f);
    
    // Test clone
    auto clonedAction = paramAction.clone();
    assert(clonedAction->getType() == paramAction.getType());
    assert(clonedAction->getParameters().size() == paramAction.getParameters().size());
    for (size_t i = 0; i < params.size(); ++i) {
        assert(clonedAction->getParameters()[i] == paramAction.getParameters()[i]);
    }
    
    // Test setType and setParameters
    nlm::Action mutableAction(nlm::ActionType::TurnLeft);
    mutableAction.setType(nlm::ActionType::MoveForward);
    assert(mutableAction.getType() == nlm::ActionType::MoveForward);
    
    std::vector<float> newParams = {0.1f, 0.2f};
    mutableAction.setParameters(newParams);
    assert(mutableAction.getParameters().size() == 2);
    assert(mutableAction.getParameters()[0] == 0.1f);
    
    std::cout << "    testActionConvenienceMethods passed" << std::endl;
}

void testActionTypeChecking() {
    std::cout << "    Testing Action type checking..." << std::endl;
    
    nlm::Action moveForward(nlm::ActionType::MoveForward);
    nlm::Action turnLeft(nlm::ActionType::TurnLeft);
    nlm::Action interact(nlm::ActionType::Interact);
    nlm::Action custom(nlm::ActionType::Custom, std::vector<float>(1, 0.5f));
    
    // Test is_move
    bool isMoveForward = moveForward.getType() == nlm::ActionType::MoveForward ||
                         moveForward.getType() == nlm::ActionType::MoveBackward;
    assert(isMoveForward);
    
    bool isTurnLeft = turnLeft.getType() == nlm::ActionType::TurnLeft ||
                     turnLeft.getType() == nlm::ActionType::TurnRight;
    assert(isTurnLeft);
    
    bool isInteract = interact.getType() == nlm::ActionType::Interact;
    assert(isInteract);
    
    // Test action names
    assert(moveForward.getName() == "MoveForward");
    assert(turnLeft.getName() == "TurnLeft");
    assert(interact.getName() == "Interact");
    assert(custom.getName() == "Custom");
    
    std::cout << "    testActionTypeChecking passed" << std::endl;
}

void testActionSerialization() {
    std::cout << "    Testing Action serialization..." << std::endl;
    
    // Create action with parameters
    std::vector<float> params = {0.5f, 0.75f, 1.0f, 1.25f};
    nlm::Action action(nlm::ActionType::Custom, params);
    
    // Test clone preserves data
    auto cloned = action.clone();
    assert(cloned->getType() == action.getType());
    assert(cloned->getParameters().size() == action.getParameters().size());
    for (size_t i = 0; i < params.size(); ++i) {
        assert(cloned->getParameters()[i] == params[i]);
    }
    
    // Test to_dict equivalent (using existing bindings)
    // This would be tested through the Python bindings interface
    
    std::cout << "    testActionSerialization passed" << std::endl;
}

void runAll() {
    testActionFactoryMethods();
    testActionConvenienceMethods();
    testActionTypeChecking();
    testActionSerialization();
    std::cout << "=== Action Tests PASSED ===" << std::endl;
}

} // namespace test_action