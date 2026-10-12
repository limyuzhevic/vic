#include "PerformanceBudget.hpp"
#include <iostream>
#include <cmath>

namespace nlm {

PerformanceBudget::PerformanceBudget(double timeBudgetPerStep, 
                                   double memoryBudgetMB,
                                   size_t eventBudgetPerStep)
    : timeBudgetPerStep_(timeBudgetPerStep)
    , memoryBudgetMB_(memoryBudgetMB)
    , eventBudgetPerStep_(eventBudgetPerStep)
    , stepTimeUsed_(0.0)
    , eventsUsed_(0)
    , memoryUsed_(0)
    , timeBudgetExceeded_(false)
    , memoryBudgetExceeded_(false)
    , eventBudgetExceeded_(false)
    , stepStartTime_(std::chrono::steady_clock::now()) {}

bool PerformanceBudget::isStepWithinBudget(double stepTimeMs, size_t eventsProcessed, size_t memoryBytes) const {
    // Convert stepTimeMs to seconds for comparison with timeBudgetPerStep (in seconds)
    double stepTimeSeconds = stepTimeMs / 1000.0;
    
    timeBudgetExceeded_ = (stepTimeSeconds > timeBudgetPerStep_);
    eventBudgetExceeded_ = (eventsProcessed > eventBudgetPerStep_);
    
    // Convert memory from bytes to MB for comparison with memoryBudgetMB_
    double memoryMB = memoryBytes / (1024.0 * 1024.0);
    memoryBudgetExceeded_ = (memoryMB > memoryBudgetMB_);
    
    return !timeBudgetExceeded_ && !eventBudgetExceeded_ && !memoryBudgetExceeded_;
}

void PerformanceBudget::updateBudget(double stepTimeMs, size_t eventsProcessed, size_t memoryBytes) {
    // Update current usage
    stepTimeUsed_ += stepTimeMs;
    eventsUsed_ += eventsProcessed;
    memoryUsed_ += memoryBytes;
    
    // Check budgets (same logic as isStepWithinBudget but updates flags)
    double stepTimeSeconds = stepTimeMs / 1000.0;
    timeBudgetExceeded_ = (stepTimeSeconds > timeBudgetPerStep_);
    eventBudgetExceeded_ = (eventsProcessed > eventBudgetPerStep_);
    
    double memoryMB = memoryBytes / (1024.0 * 1024.0);
    memoryBudgetExceeded_ = (memoryMB > memoryBudgetMB_);
    
    // Reset step usage if step exceeded budget (for next step)
    if (timeBudgetExceeded_ || eventBudgetExceeded_ || memoryBudgetExceeded_) {
        stepTimeUsed_ = 0.0;
        eventsUsed_ = 0;
        memoryUsed_ = 0;
    }
}

double PerformanceBudget::getRemainingTimeBudget() const {
    return std::max(0.0, timeBudgetPerStep_ - stepTimeUsed_ / 1000.0);
}

size_t PerformanceBudget::getRemainingEventBudget() const {
    return std::max(0UL, eventBudgetPerStep_ - eventsUsed_);
}

size_t PerformanceBudget::getRemainingMemoryBudget() const {
    double remainingMB = std::max(0.0, memoryBudgetMB_ - memoryUsed_ / (1024.0 * 1024.0));
    return static_cast<size_t>(remainingMB * 1024 * 1024); // Convert back to bytes
}

void PerformanceBudget::setTimeBudgetPerStep(double budget) {
    timeBudgetPerStep_ = budget;
    // Reset exceeded flag if budget increased
    if (budget > 0) {
        timeBudgetExceeded_ = false;
    }
}

void PerformanceBudget::setMemoryBudgetMB(double budget) {
    memoryBudgetMB_ = budget;
    // Reset exceeded flag if budget increased
    if (budget > 0) {
        memoryBudgetExceeded_ = false;
    }
}

void PerformanceBudget::setEventBudgetPerStep(size_t budget) {
    eventBudgetPerStep_ = budget;
    // Reset exceeded flag if budget increased
    if (budget > 0) {
        eventBudgetExceeded_ = false;
    }
}

} // namespace nlm