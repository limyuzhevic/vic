#include "MotorPrimitive.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct MotorPrimitive::Impl {
    class Brain* brain_;
    ActionType action_;
    std::vector<float> startPosition_;
    std::vector<float> targetPosition_;
    float strength_;
    TimestepDuration duration_;
    float smoothness_;
    bool isActive_;
    float currentTime_;
    float elapsedTime_;
    TimestepDuration minDuration_;
    TimestepDuration maxDuration_;
    float maxStrength_;
    float minStrength_;
    
    Impl() : brain_(nullptr), action_(ActionType::Wait), strength_(1.0f), 
             duration_(1.0), smoothness_(0.5f), isActive_(false),
             currentTime_(0.0f), elapsedTime_(0.0f),
             minDuration_(0.1), maxDuration_(10.0), maxStrength_(1.0f), minStrength_(0.1f) {}
};

MotorPrimitive::MotorPrimitive() : pImpl(new Impl) {
    // Initialize targetPosition_ as a copy of Impl's member
    targetPosition_ = pImpl->targetPosition_;
}

MotorPrimitive::~MotorPrimitive() = default;

void MotorPrimitive::initialize(Brain* brain) {
    pImpl->brain_ = brain;
    reset();
    NLM_LOG_INFO("MotorPrimitive initialized");
}

void MotorPrimitive::setAction(ActionType action) {
    pImpl->action_ = action;
}

void MotorPrimitive::setTargetPosition(const std::vector<float>& position) {
    pImpl->targetPosition_ = position;
}

void MotorPrimitive::setStrength(float strength) {
    pImpl->strength_ = std::max(pImpl->minStrength_, std::min(pImpl->maxStrength_, strength));
}

void MotorPrimitive::setDuration(TimestepDuration duration) {
    pImpl->duration_ = std::max(pImpl->minDuration_, std::min(pImpl->maxDuration_, duration));
}

void MotorPrimitive::setSmoothness(float smoothness) {
    pImpl->smoothness_ = std::max(0.0f, std::min(1.0f, smoothness));
}

void MotorPrimitive::update(TimestepDuration dt) {
    if (!pImpl->isActive_) return;
    
    pImpl->currentTime_ += dt;
    pImpl->elapsedTime_ += dt;
    
    // Clamp to duration
    if (pImpl->elapsedTime_ > pImpl->duration_) {
        pImpl->isActive_ = false;
        pImpl->elapsedTime_ = pImpl->duration_;
    }
}

std::vector<float> MotorPrimitive::getMotorCommand(float timeProgress) const {
    // Clamp progress to [0, 1]
    timeProgress = std::max(0.0f, std::min(1.0f, timeProgress));
    
    // Apply smooth easing function
    float smoothProgress = 1.0f - std::pow(1.0f - timeProgress, 2.0f / pImpl->smoothness_);
    
    std::vector<float> command;
    for (size_t i = 0; i < pImpl->targetPosition_.size(); ++i) {
        float value = pImpl->strength_ * pImpl->targetPosition_[i] * smoothProgress;
        command.push_back(value);
    }
    
    return command;
}

std::vector<float> MotorPrimitive::generateSmoothTrajectory(const std::vector<float>& start,
                                                           const std::vector<float>& end,
                                                           float progress) const {
    // Cubic Bezier curve for smooth trajectory
    progress = std::max(0.0f, std::min(1.0f, progress));
    
    std::vector<float> trajectory;
    for (size_t i = 0; i < start.size() && i < end.size(); ++i) {
        // Cubic Bezier interpolation
        float t = progress;
        float value = std::pow(1 - t, 3) * start[i] +
                    3 * std::pow(1 - t, 2) * t * (start[i] * 0.3f + end[i] * 0.05f) +
                    3 * (1 - t) * std::pow(t, 2) * (start[i] * 0.05f + end[i] * 0.7f) +
                    std::pow(t, 3) * end[i];
        
        trajectory.push_back(value);
    }
    
    return trajectory;
}

std::vector<float> MotorPrimitive::getSmoothCommand(float timeProgress) const {
    // Use start and target positions for smooth command
    std::vector<float> start(5, 0.0f); // Default 5D position
    return generateSmoothTrajectory(start, pImpl->targetPosition_, timeProgress);
}

void MotorPrimitive::reset() {
    pImpl->isActive_ = false;
    pImpl->currentTime_ = 0.0f;
    pImpl->elapsedTime_ = 0.0f;
    pImpl->startPosition_.clear();
}

} // namespace nlm
