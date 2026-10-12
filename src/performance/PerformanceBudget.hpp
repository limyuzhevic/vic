// PerformanceBudget.hpp - New performance budget manager
#pragma once

#include "Performance.hpp"
#include <chrono>

namespace nlm {

class PerformanceBudget {
public:
    PerformanceBudget(double timeBudgetPerStep = 1.0, 
                     double memoryBudgetMB = 1024.0,
                     size_t eventBudgetPerStep = 10000);
    
    ~PerformanceBudget() = default;
    
    // Disable copying
    PerformanceBudget(const PerformanceBudget&) = delete;
    PerformanceBudget& operator=(const PerformanceBudget&) = delete;
    
    // Check if current step is within budget
    bool isStepWithinBudget(double stepTimeMs, size_t eventsProcessed, size_t memoryBytes) const;
    
    // Update budgets based on actual usage
    void updateBudget(double stepTimeMs, size_t eventsProcessed, size_t memoryBytes);
    
    // Get current budget status
    double getRemainingTimeBudget() const;
    size_t getRemainingEventBudget() const;
    size_t getRemainingMemoryBudget() const;
    
    // Adjust budgets dynamically
    void setTimeBudgetPerStep(double budget);
    void setMemoryBudgetMB(double budget);
    void setEventBudgetPerStep(size_t budget);
    
    // Get configuration
    double getTimeBudgetPerStep() const { return timeBudgetPerStep_; }
    double getMemoryBudgetMB() const { return memoryBudgetMB_; }
    size_t getEventBudgetPerStep() const { return eventBudgetPerStep_; }
    
    // Budget status
    bool isTimeBudgetExceeded() const { return timeBudgetExceeded_; }
    bool isMemoryBudgetExceeded() const { return memoryBudgetExceeded_; }
    bool isEventBudgetExceeded() const { return eventBudgetExceeded_; }
    
private:
    double timeBudgetPerStep_;        // Time budget per simulation step
    double memoryBudgetMB_;           // Memory budget in MB
    size_t eventBudgetPerStep_;       // Event budget per simulation step
    
    // Current usage tracking
    double stepTimeUsed_;             // Time used in current step
    size_t eventsUsed_;               // Events used in current step
    size_t memoryUsed_;               // Memory used (bytes)
    
    // Budget status
    bool timeBudgetExceeded_;         // Time budget exceeded flag
    bool memoryBudgetExceeded_;       // Memory budget exceeded flag
    bool eventBudgetExceeded_;        // Event budget exceeded flag
    
    std::chrono::steady_clock::time_point stepStartTime_;
};

} // namespace nlm