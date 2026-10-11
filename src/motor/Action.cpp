// Action.cpp - Implementation (Action.hpp was missing)
#include "Action.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

Action::Action() : timestamp(0.0), strength(0.0f), duration(0.0) {}

Action::Action(ActionType type, float value) : type(type), timestamp(0.0), strength(value), duration(0.0) {}

bool Action::isComplete() const {
    return duration > 0.0 && elapsedTime >= duration;
}

float Action::getProgress() const {
    if (duration <= 0.0) return 1.0f;
    return std::min(1.0f, elapsedTime / duration);
}

void Action::update(float dt) {
    elapsedTime += dt;
}

bool Action::canBeInterrupted() const {
    return interruptible;
}

} // namespace nlm