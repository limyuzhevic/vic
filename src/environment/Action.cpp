// Stub implementation for Action.cpp
// This is a placeholder to satisfy CMake build requirements
// Phase 2 implementations will replace this stub

#include "Action.hpp"
#include <iostream>

namespace nlm {

// Action implementation (stub)
Action::Action() : type(ActionType::Wait), success(false), reward(0.0f) {}

Action::Action(ActionType actionType) : type(actionType), success(false), reward(0.0f) {}

Action::~Action() = default;

void Action::execute() {
    // Stub implementation
    std::cout << "Action::execute called (stub)" << std::endl;
    success = true;
}

ActionType Action::getType() const {
    return type;
}

bool Action::isSuccess() const {
    return success;
}

float Action::getReward() const {
    return reward;
}

void Action::setReward(float r) {
    reward = r;
}

} // namespace nlm