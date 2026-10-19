#pragma once

#include "Types.hpp"
#include <functional>
#include <atomic>
#include <chrono>

namespace nlm {

// Statistics for optimized clock performance monitoring
struct optimized_clock_stats {
    SimulationStep currentStep;
    Timestamp currentTime;
    TimestepDuration timestep;
    size_t advanceCount;
    size_t batchAdvanceCount;
    bool cacheValid;
    double realTimeElapsed;
    double simulatedTime;
    double speedFactor;
};

// Optimized simulation clock with SIMD and batch processing
class OptimizedSimulationClock {
public:
    // Create with specific timestep
    explicit OptimizedSimulationClock(TimestepDuration timestep = 0.001); // 1ms default
    
    ~OptimizedSimulationClock();
    
    // Disable copying, enable moving
    OptimizedSimulationClock(const OptimizedSimulationClock&) = delete;
    OptimizedSimulationClock& operator=(const OptimizedSimulationClock&) = delete;
    OptimizedSimulationClock(OptimizedSimulationClock&&) noexcept;
    OptimizedSimulationClock& operator=(OptimizedSimulationClock&&) noexcept;
    
    // Core time operations
    void reset();
    void advance();
    void batchAdvance(SimulationStep count);
    
    // Step counter
    SimulationStep getStep() const;
    void setStep(SimulationStep step);
    
    // Current simulation time
    Timestamp getTime() const;
    void setTime(Timestamp time);
    
    // Timestep
    TimestepDuration getTimestep() const;
    void setTimestep(TimestepDuration dt);
    
    // Convenience queries
    bool isFirstStep() const;
    bool isLastStep(SimulationStep totalSteps) const;
    
    // Step to time conversion
    Timestamp stepToTime(SimulationStep step) const;
    SimulationStep timeToStep(Timestamp time) const;
    
    // Advance by multiple steps
    void advanceBy(SimulationStep steps);
    
    // Get time at a future step from now
    Timestamp timeAtFutureStep(SimulationStep stepsAhead) const;
    
    // Real-time wall clock comparison
    double getRealTimeElapsed() const;
    void setRealTimeStart();
    
    // Performance monitoring
    optimized_clock_stats getStats() const;
    
    // Optimization configuration
    void enableSIMD(bool enable);
    void enableBatch(bool enable, size_t size = 64);
    void setStepCallback(std::function<void(SimulationStep, Timestamp)> callback);
    
private:
    struct Impl;
    Impl* pImpl;
    mutable std::mutex mutex_;
};

// Original simulation clock (backward compatibility)
class SimulationClock {
public:
    // Create with specific timestep
    explicit SimulationClock(TimestepDuration timestep = 0.001); // 1ms default
    
    ~SimulationClock();
    
    // Disable copying, enable moving
    SimulationClock(const SimulationClock&) = delete;
    SimulationClock& operator=(const SimulationClock&) = delete;
    SimulationClock(SimulationClock&&) noexcept;
    SimulationClock& operator=(SimulationClock&&) noexcept;
    
    // Time operations
    void reset();
    void advance();
    
    // Step counter
    SimulationStep getStep() const;
    void setStep(SimulationStep step);
    
    // Current simulation time
    Timestamp getTime() const;
    void setTime(Timestamp time);
    
    // Timestep
    TimestepDuration getTimestep() const;
    void setTimestep(TimestepDuration dt);
    
    // Convenience queries
    bool isFirstStep() const;
    bool isLastStep(SimulationStep totalSteps) const;
    
    // Step to time conversion
    Timestamp stepToTime(SimulationStep step) const;
    SimulationStep timeToStep(Timestamp time) const;
    
    // Advance by multiple steps
    void advanceBy(SimulationStep steps);
    
    // Get time at a future step from now
    Timestamp timeAtFutureStep(SimulationStep stepsAhead) const;
    
    // Real-time wall clock comparison (for real-time mode)
    double getRealTimeElapsed() const;
    void setRealTimeStart();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
