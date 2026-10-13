#include "SleepState.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct SleepState::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

SleepState::SleepState()
    : pImpl(new Impl)
    , brain_(nullptr)
    , sleepPhaseDuration_(2000)
    , wakePhaseDuration_(3000)
    , dutyCycle_(0.4f)
    , enabled_(true)
    , isSleeping_(false)
    , lastSleepStart_(0)
    , lastWakeStart_(0)
    , totalSleepTime_(0)
    , totalWakeTime_(0)
{
}

SleepState::~SleepState() = default;

void SleepState::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("SleepState initialized");
}

void SleepState::update(SimulationStep currentStep) {
    if (!enabled_ || !brain_) return;
    
    updateSleepWakeTransitions(currentStep);
}

void SleepState::updateSleepWakeTransitions(SimulationStep currentStep) {
    bool shouldSleep = shouldTransitionToSleep(currentStep);
    bool shouldWake = shouldTransitionToWake(currentStep);
    
    if (shouldSleep && !isSleeping_) {
        // Transition to sleep
        isSleeping_ = true;
        lastSleepStart_ = currentStep;
        NLM_LOG_INFO("SleepState: Entering sleep phase at step " + std::to_string(currentStep));
    } else if (shouldWake && isSleeping_) {
        // Transition to wake
        isSleeping_ = false;
        lastWakeStart_ = currentStep;
        totalSleepTime_ += (currentStep - lastSleepStart_);
        NLM_LOG_INFO("SleepState: Waking up at step " + std::to_string(currentStep));
    }
}

bool SleepState::isSleeping(SimulationStep currentStep) const {
    if (!enabled_) return false;
    
    // Check if we should transition to sleep
    if (shouldTransitionToSleep(currentStep)) {
        return true;
    }
    
    // Check if we should transition to wake
    if (shouldTransitionToWake(currentStep)) {
        return false;
    }
    
    return isSleeping_;
}

bool SleepState::shouldTransitionToSleep(SimulationStep currentStep) const {
    // Simple implementation: transition to sleep at regular intervals
    // In more complex implementations, this could depend on neuromodulatory signals
    
    SimulationStep cycleLength = sleepPhaseDuration_ + wakePhaseDuration_;
    if (cycleLength <= 0) return false;
    
    SimulationStep cyclePosition = currentStep % cycleLength;
    return (cyclePosition >= sleepPhaseDuration_ && cyclePosition < cycleLength);
}

bool SleepState::shouldTransitionToWake(SimulationStep currentStep) const {
    // Simple implementation: transition to wake after sleep phase
    SimulationStep cycleLength = sleepPhaseDuration_ + wakePhaseDuration_;
    if (cycleLength <= 0) return false;
    
    SimulationStep cyclePosition = currentStep % cycleLength;
    return (cyclePosition >= sleepPhaseDuration_);
}

void SleepState::forceSleep(bool sleep) {
    if (sleep != isSleeping_) {
        isSleeping_ = sleep;
        if (sleep) {
            lastSleepStart_ = 0;  // Unknown start
            NLM_LOG_INFO("SleepState: Forced sleep");
        } else {
            lastWakeStart_ = 0;  // Unknown start
            totalSleepTime_ += (0 - lastSleepStart_);  // Add unknown sleep time
            NLM_LOG_INFO("SleepState: Forced wake");
        }
    }
}

void SleepState::reset() {
    isSleeping_ = false;
    lastSleepStart_ = 0;
    lastWakeStart_ = 0;
    totalSleepTime_ = 0;
    totalWakeTime_ = 0;
    NLM_LOG_INFO("SleepState: Reset complete");
}

SimulationStep SleepState::calculateSleepStart(SimulationStep currentStep) const {
    // Calculate when sleep will start
    SimulationStep cycleLength = sleepPhaseDuration_ + wakePhaseDuration_;
    if (cycleLength <= 0) return currentStep;
    
    SimulationStep cyclePosition = currentStep % cycleLength;
    if (cyclePosition < sleepPhaseDuration_) {
        // We're currently in sleep or about to sleep
        return currentStep - cyclePosition + sleepPhaseDuration_;
    } else {
        // We're in wake phase
        return currentStep - cyclePosition + cycleLength + sleepPhaseDuration_;
    }
}

SimulationStep SleepState::calculateWakeStart(SimulationStep currentStep) const {
    // Calculate when wake will start
    SimulationStep cycleLength = sleepPhaseDuration_ + wakePhaseDuration_;
    if (cycleLength <= 0) return currentStep;
    
    SimulationStep cyclePosition = currentStep % cycleLength;
    return currentStep - cyclePosition + sleepPhaseDuration_;
}

SimulationStep SleepState::getSleepStart(SimulationStep currentStep) const {
    if (isSleeping_) {
        return lastSleepStart_;
    }
    return calculateSleepStart(currentStep);
}

SimulationStep SleepState::getSleepEnd(SimulationStep currentStep) const {
    if (!isSleeping_) {
        return calculateSleepStart(currentStep) + sleepPhaseDuration_;
    }
    return lastSleepStart_ + sleepPhaseDuration_;
}

SimulationStep SleepState::getWakeStart(SimulationStep currentStep) const {
    if (!isSleeping_) {
        return lastWakeStart_;
    }
    return calculateWakeStart(currentStep);
}

size_t SleepState::getSleepDuration(SimulationStep currentStep) const {
    if (isSleeping_) {
        return currentStep - lastSleepStart_;
    } else if (lastSleepStart_ > 0) {
        return sleepPhaseDuration_;
    }
    return 0;
}

size_t SleepState::getWakeDuration(SimulationStep currentStep) const {
    if (!isSleeping_) {
        return currentStep - lastWakeStart_;
    } else if (lastWakeStart_ > 0) {
        return wakePhaseDuration_;
    }
    return 0;
}

float SleepState::getSleepPercentage(SimulationStep currentStep) const {
    SimulationStep cycleLength = sleepPhaseDuration_ + wakePhaseDuration_;
    if (cycleLength <= 0) return 0.0f;
    
    return static_cast<float>(sleepPhaseDuration_) / static_cast<float>(cycleLength);
}

} // namespace nlm