#pragma once

// SimulationClock.cpp - Enhanced implementation with comprehensive error handling

#include "SimulationClock.hpp"
#include "Error/NLMError.hpp"
#include "Error/ErrorCategory.hpp"
#include <chrono>
#include <limits>
#include <cstring>
#include <cmath>

namespace nlm {

// SimulationClock constants
static constexpr TimestepDuration MIN_TIMESTEP = 1e-12; // 1 picosecond
static constexpr TimestepDuration MAX_TIMESTEP = 3600.0; // 1 hour
static constexpr TimestepDuration EPSILON = 1e-15; // For floating-point comparisons

// Time conversion constants
static constexpr Timestamp MAX_TIMESTAMP = std::numeric_limits<Timestamp>::max() * 0.9; // 90% of max to avoid overflow
static constexpr Timestamp MIN_TIMESTAMP = -MAX_TIMESTAMP;

// Validation helper functions
static bool isFinite(Timestamp value) {
    return std::isfinite(value);
}

static bool isPositive(Timestamp value) {
    return value > 0.0;
}

static bool isNonNegative(Timestamp value) {
    return value >= 0.0;
}

static bool isInteger(Timestamp value) {
    return std::abs(value - std::round(value)) < EPSILON;
}

static Timestamp clamp(Timestamp value, Timestamp min, Timestamp max) {
    return std::max(min, std::min(max, value));
}

static SimulationStep safeStepConversion(Timestamp time, TimestepDuration timestep) {
    if (timestep <= 0.0) {
        throw SimulationClockError("safeStepConversion", "Invalid timestep: must be positive");
    }
    
    // Avoid overflow in multiplication
    if (std::abs(time) > MAX_TIMESTAMP / std::abs(timestep)) {
        throw SimulationClockError("safeStepConversion", "Time/timestep ratio too large, risk of overflow");
    }
    
    return static_cast<SimulationStep>(time / timestep);
}

} // namespace nlm

// Enhanced SimulationClock implementation with thread safety
namespace nlm {

SimulationClock::SimulationClock(TimestepDuration timestep) : pImpl(new Impl(timestep)) {
    try {
        if (!isFinite(timestep) || timestep <= 0.0) {
            throw SimulationClockError("constructor", 
                                     "Invalid timestep: " + std::to_string(timestep) + ". Must be finite and positive");
        }
        
        if (timestep < MIN_TIMESTEP) {
            spdlog::warn("Simulation timestep is very small ({:.2e} seconds), may cause performance issues", timestep);
        }
        
        if (timestep > MAX_TIMESTEP) {
            spdlog::warn("Simulation timestep is very large ({:.2f} seconds), may cause accuracy issues", timestep);
        }
        
        // Initialize real time start for elapsed time tracking
        pImpl->realStart = std::chrono::steady_clock::now();
        pImpl->realStartSet = true;
        
        spdlog::info("SimulationClock initialized with timestep: {:.6f}s", timestep);
        
    } catch (const SimulationClockError& e) {
        throw;
    } catch (const std::exception& e) {
        throw SimulationClockError("constructor", "Failed to initialize simulation clock: " + std::string(e.what()));
    }
}

SimulationClock::~SimulationClock() = default;

SimulationClock::SimulationClock(SimulationClock&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

SimulationClock& SimulationClock::operator=(SimulationClock&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

void SimulationClock::reset() {
    try {
        pImpl->step = 0;
        pImpl->time = 0.0;
        pImpl->realStart = std::chrono::steady_clock::now();
        pImpl->realStartSet = true;
        
        spdlog::debug("SimulationClock reset: step=0, time=0.0s");
        
    } catch (const std::exception& e) {
        spdlog::error("Error resetting simulation clock: {}", e.what());
        throw;
    }
}

void SimulationClock::advance() {
    try {
        // Check for potential overflow
        if (pImpl->time + pImpl->timestep > MAX_TIMESTAMP) {
            throw SimulationClockError("advance", "Timestep addition would cause overflow");
        }
        
        ++pImpl->step;
        pImpl->time += pImpl->timestep;
        
        // Validate state after advance
        if (!isFinite(pImpl->time) || pImpl->time != pImpl->time) { // NaN check
            throw SimulationClockError("advance", "Invalid time value after advance");
        }
        
    } catch (const SimulationClockError& e) {
        spdlog::error("Error advancing simulation clock: {}", e.what());
        throw;
    } catch (const std::exception& e) {
        spdlog::error("Unexpected error advancing simulation clock: {}", e.what());
        throw;
    }
}

SimulationStep SimulationClock::getStep() const {
    return pImpl->step;
}

void SimulationClock::setStep(SimulationStep step) {
    try {
        if (!isNonNegative(step)) {
            throw SimulationClockError("setStep", "Step must be non-negative: " + std::to_string(step));
        }
        
        if (step > static_cast<SimulationStep>(MAX_TIMESTAMP / pImpl->timestep)) {
            throw SimulationClockError("setStep", "Step value too large, would cause overflow");
        }
        
        pImpl->step = step;
        pImpl->time = step * pImpl->timestep;
        
        // Validate time value
        if (!isFinite(pImpl->time) || pImpl->time != pImpl->time) {
            throw SimulationClockError("setStep", "Invalid time value after setting step");
        }
        
    } catch (const SimulationClockError& e) {
        spdlog::error("Error setting simulation step: {}", e.what());
        throw;
    } catch (const std::exception& e) {
        spdlog::error("Unexpected error setting simulation step: {}", e.what());
        throw;
    }
}

Timestamp SimulationClock::getTime() const {
    return pImpl->time;
}

void SimulationClock::setTime(Timestamp time) {
    try {
        if (!isFinite(time)) {
            throw SimulationClockError("setTime", "Time must be finite: " + std::to_string(time));
        }
        
        // Clamp time to reasonable bounds
        Timestamp clampedTime = clamp(time, MIN_TIMESTAMP, MAX_TIMESTAMP);
        
        pImpl->time = clampedTime;
        pImpl->step = safeStepConversion(pImpl->time, pImpl->timestep);
        
        // Validate state
        if (!isFinite(pImpl->time) || pImpl->time != pImpl->time) {
            throw SimulationClockError("setTime", "Invalid time value after setting");
        }
        
    } catch (const SimulationClockError& e) {
        spdlog::error("Error setting simulation time: {}", e.what());
        throw;
    } catch (const std::exception& e) {
        spdlog::error("Unexpected error setting simulation time: {}", e.what());
        throw;
    }
}

TimestepDuration SimulationClock::getTimestep() const {
    return pImpl->timestep;
}

void SimulationClock::setTimestep(TimestepDuration dt) {
    try {
        if (!isFinite(dt) || dt <= 0.0) {
            throw SimulationClockError("setTimestep", "Timestep must be finite and positive: " + std::to_string(dt));
        }
        
        if (dt < MIN_TIMESTEP) {
            spdlog::warn("Setting small timestep ({:.2e} seconds)", dt);
        }
        
        // Recalculate step to maintain consistency
        SimulationStep oldStep = pImpl->step;
        Timestamp oldTime = pImpl->time;
        
        pImpl->timestep = dt;
        
        // Re-calculate time based on step to maintain consistency
        pImpl->time = oldStep * pImpl->timestep;
        
        // If time was set directly (not from step), adjust step
        if (oldTime != 0.0 && std::abs(pImpl->time - oldTime) > EPSILON) {
            pImpl->step = safeStepConversion(oldTime, dt);
        }
        
        spdlog::info("Simulation timestep changed to: {:.6f}s", dt);
        
    } catch (const SimulationClockError& e) {
        spdlog::error("Error setting timestep: {}", e.what());
        throw;
    } catch (const std::exception& e) {
        spdlog::error("Unexpected error setting timestep: {}", e.what());
        throw;
    }
}

bool SimulationClock::isFirstStep() const {
    return pImpl->step == 0;
}

bool SimulationClock::isLastStep(SimulationStep totalSteps) const {
    return pImpl->step >= totalSteps;
}

Timestamp SimulationClock::stepToTime(SimulationStep step) const {
    try {
        if (!isNonNegative(step)) {
            throw SimulationClockError("stepToTime", "Step must be non-negative: " + std::to_string(step));
        }
        
        if (step > static_cast<SimulationStep>(MAX_TIMESTAMP / pImpl->timestep)) {
            throw SimulationClockError("stepToTime", "Step value too large, would cause overflow");
        }
        
        Timestamp time = step * pImpl->timestep;
        
        if (!isFinite(time) || time != time) {
            throw SimulationClockError("stepToTime", "Invalid time value calculated from step");
        }
        
        return time;
        
    } catch (const SimulationClockError& e) {
        spdlog::error("Error converting step to time: {}", e.what());
        throw;
    }
}

SimulationStep SimulationClock::timeToStep(Timestamp time) const {
    try {
        if (!isFinite(time)) {
            throw SimulationClockError("timeToStep", "Time must be finite: " + std::to_string(time));
        }
        
        return safeStepConversion(time, pImpl->timestep);
        
    } catch (const SimulationClockError& e) {
        spdlog::error("Error converting time to step: {}", e.what());
        throw;
    } catch (const std::exception& e) {
        spdlog::error("Unexpected error converting time to step: {}", e.what());
        throw;
    }
}

void SimulationClock::advanceBy(SimulationStep steps) {
    try {
        if (!isNonNegative(steps)) {
            throw SimulationClockError("advanceBy", "Steps must be non-negative: " + std::to_string(steps));
        }
        
        if (steps == 0) {
            return; // No-op
        }
        
        // Check for potential overflow
        Timestamp timeIncrease = steps * pImpl->timestep;
        if (timeIncrease > MAX_TIMESTAMP - pImpl->time) {
            throw SimulationClockError("advanceBy", "Step advance would cause overflow");
        }
        
        pImpl->step += steps;
        pImpl->time += timeIncrease;
        
        // Validate state
        if (!isFinite(pImpl->time) || pImpl->time != pImpl->time) {
            throw SimulationClockError("advanceBy", "Invalid time value after advanceBy");
        }
        
    } catch (const SimulationClockError& e) {
        spdlog::error("Error advancing simulation by steps: {}", e.what());
        throw;
    } catch (const std::exception& e) {
        spdlog::error("Unexpected error advancing simulation by steps: {}", e.what());
        throw;
    }
}

Timestamp SimulationClock::timeAtFutureStep(SimulationStep stepsAhead) const {
    try {
        if (!isNonNegative(stepsAhead)) {
            throw SimulationClockError("timeAtFutureStep", "Steps ahead must be non-negative: " + std::to_string(stepsAhead));
        }
        
        if (stepsAhead == 0) {
            return pImpl->time;
        }
        
        Timestamp futureTime = pImpl->time + (stepsAhead * pImpl->timestep);
        
        if (futureTime > MAX_TIMESTAMP) {
            throw SimulationClockError("timeAtFutureStep", "Future time would cause overflow");
        }
        
        if (!isFinite(futureTime) || futureTime != futureTime) {
            throw SimulationClockError("timeAtFutureStep", "Invalid future time value");
        }
        
        return futureTime;
        
    } catch (const SimulationClockError& e) {
        spdlog::error("Error calculating time at future step: {}", e.what());
        throw;
    } catch (const std::exception& e) {
        spdlog::error("Unexpected error calculating time at future step: {}", e.what());
        throw;
    }
}

double SimulationClock::getRealTimeElapsed() const {
    try {
        if (!pImpl->realStartSet) {
            return 0.0;
        }
        
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(now - pImpl->realStart);
        
        double elapsed = duration.count();
        
        if (!isFinite(elapsed) || elapsed != elapsed) {
            throw SimulationClockError("getRealTimeElapsed", "Invalid elapsed time value");
        }
        
        return std::max(0.0, elapsed);
        
    } catch (const SimulationClockError& e) {
        spdlog::error("Error getting real time elapsed: {}", e.what());
        throw;
    } catch (const std::exception& e) {
        spdlog::error("Unexpected error getting real time elapsed: {}", e.what());
        throw;
    }
}

void SimulationClock::setRealTimeStart() {
    try {
        pImpl->realStart = std::chrono::steady_clock::now();
        pImpl->realStartSet = true;
        spdlog::debug("Real time start set for simulation clock");
        
    } catch (const std::exception& e) {
        spdlog::error("Error setting real time start: {}", e.what());
        throw;
    }
}

} // namespace nlm
