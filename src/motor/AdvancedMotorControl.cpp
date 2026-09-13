#include "AdvancedMotorControl.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct AdvancedMotorControl::Impl {
    class Brain* brain_;
    std::vector<float> currentTarget_;
    std::vector<float> currentPosition_;
    std::vector<float> currentVelocity_;
    std::vector<float> currentAcceleration_;
    float movementSpeed_;
    float accelerationLimit_;
    float decelerationDistance_;
    float movementThreshold_;
    TimestepDuration movementStartTime_;
    TimestepDuration currentMovementTime_;
    
    Impl() : brain_(nullptr), movementSpeed_(1.0f), accelerationLimit_(1.0f),
             decelerationDistance_(0.1f), movementThreshold_(0.01f),
             movementStartTime_(0.0), currentMovementTime_(0.0) {
        currentPosition_.resize(5, 0.0f);
        currentVelocity_.resize(5, 0.0f);
        currentAcceleration_.resize(5, 0.0f);
    }
};

AdvancedMotorControl::AdvancedMotorControl() : pImpl(new Impl) {}

AdvancedMotorControl::~AdvancedMotorControl() = default;

void AdvancedMotorControl::initialize(Brain* brain) {
    pImpl->brain_ = brain;
    motorPlanner_.initialize(brain);
    NLM_LOG_INFO("AdvancedMotorControl initialized");
}

void AdvancedMotorControl::executeMovement(const std::vector<float>& target,
                                          const std::vector<float>& currentState,
                                          TimestepDuration dt) {
    pImpl->currentTarget_ = target;
    pImpl->currentPosition_ = currentState;
    pImpl->movementStartTime_ = pImpl->currentMovementTime_;
    
    // Plan smooth movement
    std::vector<ActionType> preferredActions = {ActionType::MoveForward, ActionType::TurnRight};
    std::vector<MotorPrimitive*> plan = motorPlanner_.planSequence(
        currentState, target, preferredActions);
    
    motorPlanner_.setCurrentPlan(plan);
    motorPlanner_.executePlan(dt);
}

std::vector<float> AdvancedMotorControl::getMotorCommand() const {
    return motorPlanner_.getCombinedMotorCommand();
}

void AdvancedMotorControl::update(TimestepDuration dt) {
    pImpl->currentMovementTime_ += dt;
    motorPlanner_.executePlan(dt);
}

std::vector<float> AdvancedMotorControl::getConstrainedMotorCommand(
    const std::vector<float>& currentVelocities,
    const std::vector<float>& targetVelocities,
    TimestepDuration dt) const {
    std::vector<float> constrainedCommand;
    
    for (size_t i = 0; i < currentVelocities.size(); ++i) {
        float desiredAccel = (targetVelocities[i] - currentVelocities[i]) / static_cast<float>(dt);
        float clampedAccel = std::max(-pImpl->accelerationLimit_, 
                                     std::min(pImpl->accelerationLimit_, desiredAccel));
        
        float newVelocity = currentVelocities[i] + clampedAccel * static_cast<float>(dt);
        
        // Apply velocity limits
        float maxVel = pImpl->movementSpeed_;
        if (std::abs(newVelocity) > maxVel) {
            newVelocity = maxVel * (newVelocity >= 0 ? 1 : -1);
        }
        
        // Update acceleration with smoothing
        float smoothedAccel = 0.9f * clampedAccel + 0.1f * pImpl->currentAcceleration_[i];
        pImpl->currentAcceleration_[i] = smoothedAccel;
        
        constrainedCommand.push_back(newVelocity);
    }
    
    return constrainedCommand;
}

std::vector<float> AdvancedMotorControl::interpolateTrajectory(
    const std::vector<float>& start,
    const std::vector<float>& goal,
    float timeProgress) const {
    return MotorPrimitive().generateSmoothTrajectory(start, goal, timeProgress);
}

bool AdvancedMotorControl::isAtTarget(const std::vector<float>& current,
                                      const std::vector<float>& target,
                                      float tolerance) const {
    if (current.size() != target.size()) return false;
    
    for (size_t i = 0; i < current.size(); ++i) {
        if (std::abs(current[i] - target[i]) > tolerance) {
            return false;
        }
    }
    return true;
}

void AdvancedMotorControl::reset() {
    pImpl->currentTarget_.clear();
    pImpl->currentPosition_.assign(pImpl->currentPosition_.size(), 0.0f);
    pImpl->currentVelocity_.assign(pImpl->currentVelocity_.size(), 0.0f);
    pImpl->currentAcceleration_.assign(pImpl->currentAcceleration_.size(), 0.0f);
    motorPlanner_.clearPlan();
    NLM_LOG_INFO("AdvancedMotorControl reset");
}

} // namespace nlm
