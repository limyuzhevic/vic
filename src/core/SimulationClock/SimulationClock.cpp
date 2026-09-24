#include "SimulationClock.hpp"
#include <chrono>

namespace nlm {

struct SimulationClock::Impl {
    SimulationStep step;
    Timestamp time;
    TimestepDuration timestep;
    std::chrono::steady_clock::time_point realStart;
    bool realStartSet;
    
    Impl(TimestepDuration dt) 
        : step(0), time(0.0), timestep(dt), realStartSet(false) {
        // Validate timestep to prevent numerical issues
        if (dt <= 0.0) {
            throw std::invalid_argument("Timestep must be positive");
        }
    }
};

SimulationClock::SimulationClock(TimestepDuration timestep) : pImpl(new Impl(timestep)) {}

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
    auto errorContext = NLM_CURRENT_ERROR_CONTEXT;
    
    try {
        pImpl->step = 0;
        pImpl->time = 0.0;
        pImpl->realStartSet = false;
    } catch (const std::exception& e) {
        ErrorContext context(__FILE__, __func__, __LINE__);
        SimulationClockException genericEx(ErrorCode::InvalidArgument, std::string("Failed to reset simulation clock: ") + e.what(), context);
        nlm::ErrorHandling::handleException(genericEx);
        throw genericEx;
    }
}

void SimulationClock::advance() {
    auto errorContext = NLM_CURRENT_ERROR_CONTEXT;
    
    try {
        // Validate timestep to prevent numerical instability
        if (pImpl->timestep <= 0.0) {
            NLM_THROW_CURRENT_ERROR(ErrorCode::InvalidArgument, "Timestep is not positive", ErrorCategory::System);
        }
        
        ++pImpl->step;
        pImpl->time += pImpl->timestep;
        
        // Validate time to prevent overflow
        if (std::isinf(pImpl->time) || std::isnan(pImpl->time)) {
            NLM_THROW_CURRENT_ERROR(ErrorCode::InvalidArgument, "Simulation time is invalid", ErrorCategory::System);
        }
    } catch (const NLMException& e) {
        nlm::ErrorHandling::handleException(e);
        throw;
    } catch (const std::exception& e) {
        ErrorContext context(__FILE__, __func__, __LINE__);
        SimulationClockException genericEx(ErrorCode::UnknownError, std::string("Failed to advance simulation clock: ") + e.what(), context);
        nlm::ErrorHandling::handleException(genericEx);
        throw genericEx;
    }
}

SimulationStep SimulationClock::getStep() const {
    return pImpl->step;
}

void SimulationClock::setStep(SimulationStep step) {
    auto errorContext = NLM_CURRENT_ERROR_CONTEXT;
    
    try {
        // Validate step to prevent overflow
        if (step > std::numeric_limits<SimulationStep>::max() / 100) {
            NLM_THROW_CURRENT_ERROR(ErrorCode::OutOfRange, "Step value is too large", ErrorCategory::System);
        }
        
        pImpl->step = step;
        pImpl->time = step * pImpl->timestep;
        
        // Validate time to prevent numerical issues
        if (std::isinf(pImpl->time) || std::isnan(pImpl->time)) {
            NLM_THROW_CURRENT_ERROR(ErrorCode::InvalidArgument, "Calculated simulation time is invalid", ErrorCategory::System);
        }
    } catch (const NLMException& e) {
        nlm::ErrorHandling::handleException(e);
        throw;
    } catch (const std::exception& e) {
        ErrorContext context(__FILE__, __func__, __LINE__);
        SimulationClockException genericEx(ErrorCode::UnknownError, std::string("Failed to set simulation step: ") + e.what(), context);
        nlm::ErrorHandling::handleException(genericEx);
        throw genericEx;
    }
}

Timestamp SimulationClock::getTime() const {
    return pImpl->time;
}

void SimulationClock::setTime(Timestamp time) {
    auto errorContext = NLM_CURRENT_ERROR_CONTEXT;
    
    try {
        // Validate time to prevent numerical issues
        if (std::isinf(time) || std::isnan(time)) {
            NLM_THROW_CURRENT_ERROR(ErrorCode::InvalidArgument, "Invalid time value", ErrorCategory::System);
        }
        
        pImpl->time = time;
        pImpl->step = static_cast<SimulationStep>(time / pImpl->timestep);
        
        // Validate step to prevent overflow
        if (pImpl->step > std::numeric_limits<SimulationStep>::max() / 10) {
            NLM_THROW_CURRENT_ERROR(ErrorCode::OutOfRange, "Calculated step value is too large", ErrorCategory::System);
        }
    } catch (const NLMException& e) {
        nlm::ErrorHandling::handleException(e);
        throw;
    } catch (const std::exception& e) {
        ErrorContext context(__FILE__, __func__, __LINE__);
        SimulationClockException genericEx(ErrorCode::UnknownError, std::string("Failed to set simulation time: ") + e.what(), context);
        nlm::ErrorHandling::handleException(genericEx);
        throw genericEx;
    }
}

TimestepDuration SimulationClock::getTimestep() const {
    return pImpl->timestep;
}

void SimulationClock::setTimestep(TimestepDuration dt) {
    auto errorContext = NLM_CURRENT_ERROR_CONTEXT;
    
    try {
        // Validate timestep to prevent numerical issues
        if (dt <= 0.0) {
            NLM_THROW_CURRENT_ERROR(ErrorCode::InvalidArgument, "Timestep must be positive", ErrorCategory::System);
        }
        
        // Validate timestep doesn't cause numerical overflow
        if (dt > 1.0) {
            NLM_THROW_CURRENT_ERROR(ErrorCode::InvalidArgument, "Timestep is too large and may cause numerical overflow", ErrorCategory::System);
        }
        
        pImpl->timestep = dt;
        
        // Recalculate step from time to maintain consistency
        if (pImpl->timestep > 0.0) {
            pImpl->step = static_cast<SimulationStep>(pImpl->time / pImpl->timestep);
        }
    } catch (const NLMException& e) {
        nlm::ErrorHandling::handleException(e);
        throw;
    } catch (const std::exception& e) {
        ErrorContext context(__FILE__, __func__, __LINE__);
        SimulationClockException genericEx(ErrorCode::UnknownError, std::string("Failed to set timestep: ") + e.what(), context);
        nlm::ErrorHandling::handleException(genericEx);
        throw genericEx;
    }
}

bool SimulationClock::isFirstStep() const {
    return pImpl->step == 0;
}

bool SimulationClock::isLastStep(SimulationStep totalSteps) const {
    return pImpl->step >= totalSteps;
}

Timestamp SimulationClock::stepToTime(SimulationStep step) const {
    auto errorContext = NLM_CURRENT_ERROR_CONTEXT;
    
    try {
        // Validate step to prevent overflow
        if (step > std::numeric_limits<SimulationStep>::max() / 100) {
            NLM_THROW_CURRENT_ERROR(ErrorCode::OutOfRange, "Step value is too large for time conversion", ErrorCategory::System);
        }
        
        return step * pImpl->timestep;
    } catch (const NLMException& e) {
        nlm::ErrorHandling::handleException(e);
        throw;
    } catch (const std::exception& e) {
        ErrorContext context(__FILE__, __func__, __LINE__);
        SimulationClockException genericEx(ErrorCode::UnknownError, std::string("Failed to convert step to time: ") + e.what(), context);
        nlm::ErrorHandling::handleException(genericEx);
        throw genericEx;
    }
}

SimulationStep SimulationClock::timeToStep(Timestamp time) const {
    auto errorContext = NLM_CURRENT_ERROR_CONTEXT;
    
    try {
        // Validate time to prevent numerical issues
        if (std::isinf(time) || std::isnan(time)) {
            NLM_THROW_CURRENT_ERROR(ErrorCode::InvalidArgument, "Invalid time value for step conversion", ErrorCategory::System);
        }
        
        return static_cast<SimulationStep>(time / pImpl->timestep);
    } catch (const NLMException& e) {
        nlm::ErrorHandling::handleException(e);
        throw;
    } catch (const std::exception& e) {
        ErrorContext context(__FILE__, __func__, __LINE__);
        SimulationClockException genericEx(ErrorCode::UnknownError, std::string("Failed to convert time to step: ") + e.what(), context);
        nlm::ErrorHandling::handleException(genericEx);
        throw genericEx;
    }
}

void SimulationClock::advanceBy(SimulationStep steps) {
    auto errorContext = NLM_CURRENT_ERROR_CONTEXT;
    
    try {
        // Validate steps to prevent overflow
        if (steps > std::numeric_limits<SimulationStep>::max() / 10) {
            NLM_THROW_CURRENT_ERROR(ErrorCode::OutOfRange, "Number of steps is too large", ErrorCategory::System);
        }
        
        pImpl->step += steps;
        pImpl->time += steps * pImpl->timestep;
        
        // Validate time to prevent numerical issues
        if (std::isinf(pImpl->time) || std::isnan(pImpl->time)) {
            NLM_THROW_CURRENT_ERROR(ErrorCode::InvalidArgument, "Simulation time is invalid after advance", ErrorCategory::System);
        }
    } catch (const NLMException& e) {
        nlm::ErrorHandling::handleException(e);
        throw;
    } catch (const std::exception& e) {
        ErrorContext context(__FILE__, __func__, __LINE__);
        SimulationClockException genericEx(ErrorCode::UnknownError, std::string("Failed to advance simulation clock by steps: ") + e.what(), context);
        nlm::ErrorHandling::handleException(genericEx);
        throw genericEx;
    }
}

Timestamp SimulationClock::timeAtFutureStep(SimulationStep stepsAhead) const {
    auto errorContext = NLM_CURRENT_ERROR_CONTEXT;
    
    try {
        // Validate steps to prevent overflow
        if (stepsAhead > std::numeric_limits<SimulationStep>::max() / 10) {
            NLM_THROW_CURRENT_ERROR(ErrorCode::OutOfRange, "Future step value is too large", ErrorCategory::System);
        }
        
        return pImpl->time + (stepsAhead * pImpl->timestep);
    } catch (const NLMException& e) {
        nlm::ErrorHandling::handleException(e);
        throw;
    } catch (const std::exception& e) {
        ErrorContext context(__FILE__, __func__, __LINE__);
        SimulationClockException genericEx(ErrorCode::UnknownError, std::string("Failed to calculate time at future step: ") + e.what(), context);
        nlm::ErrorHandling::handleException(genericEx);
        throw genericEx;
    }
}

double SimulationClock::getRealTimeElapsed() const {
    auto errorContext = NLM_CURRENT_ERROR_CONTEXT;
    
    try {
        if (!pImpl->realStartSet) {
            return 0.0;
        }
        
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(now - pImpl->realStart);
        double result = duration.count();
        
        // Validate result to prevent numerical issues
        if (std::isinf(result) || std::isnan(result) || result < 0.0) {
            NLM_THROW_CURRENT_ERROR(ErrorCode::InvalidArgument, "Real time elapsed is invalid", ErrorCategory::System);
        }
        
        return result;
    } catch (const NLMException& e) {
        nlm::ErrorHandling::handleException(e);
        throw;
    } catch (const std::exception& e) {
        ErrorContext context(__FILE__, __func__, __LINE__);
        SimulationClockException genericEx(ErrorCode::UnknownError, std::string("Failed to get real time elapsed: ") + e.what(), context);
        nlm::ErrorHandling::handleException(genericEx);
        throw genericEx;
    }
}

void SimulationClock::setRealTimeStart() {
    auto errorContext = NLM_CURRENT_ERROR_CONTEXT;
    
    try {
        pImpl->realStart = std::chrono::steady_clock::now();
        pImpl->realStartSet = true;
    } catch (const std::exception& e) {
        ErrorContext context(__FILE__, __func__, __LINE__);
        SimulationClockException genericEx(ErrorCode::UnknownError, std::string("Failed to set real time start: ") + e.what(), context);
        nlm::ErrorHandling::handleException(genericEx);
        throw genericEx;
    }
}

} // namespace nlm
