#include "SimulationClock.hpp"
#include <chrono>
#include <limits>
#include "NLMErrors.hpp"
#include "ErrorUtils.hpp"

namespace nlm {

struct SimulationClock::Impl {
    SimulationStep step;
    Timestamp time;
    TimestepDuration timestep;
    std::chrono::steady_clock::time_point realStart;
    bool realStartSet;
    ErrorHandler errorHandler;
    
    Impl(TimestepDuration dt) 
        : step(0), time(0.0), timestep(dt), realStartSet(false) {
        validateTimestep(dt);
    }
    
    void validateTimestep(TimestepDuration dt) {
        NLM_VALIDATE(dt > 0.0, "Timestep must be positive");
        NLM_VALIDATE(dt < std::numeric_limits<double>::max() / 1000.0, 
                   "Timestep is too large and may cause overflow");
    }
    
    void validateTimestamp(Timestamp t) {
        NLM_VALIDATE(std::isfinite(t), "Timestamp must be finite");
        NLM_VALIDATE(std::abs(t) < std::numeric_limits<double>::max() / 1000.0,
                   "Timestamp magnitude may cause overflow");
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
    try {
        pImpl->step = 0;
        pImpl->time = 0.0;
        pImpl->realStartSet = false;
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "SimulationClock"));
    }
}

void SimulationClock::advance() {
    try {
        // Validate current state
        NLM_VALIDATE(pImpl->timestep > 0.0, "Timestep must be positive");
        
        // Check for potential overflow
        if (pImpl->time + pImpl->timestep > std::numeric_limits<double>::max() / 2.0) {
            NLM_THROW_RUNTIME_ERROR("Time overflow detected");
        }
        
        ++pImpl->step;
        pImpl->time += pImpl->timestep;
        
        // Validate time after advance
        pImpl->validateTimestamp(pImpl->time);
        
        NLM_THROW_ON_ERROR();
        
    } catch (const NLMError& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(
            "Failed to advance clock: " + std::string(e.what()), "SimulationClock"));
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "SimulationClock"));
    }
}

SimulationStep SimulationClock::getStep() const {
    try {
        return pImpl->step;
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "SimulationClock"));
        return 0;
    }
}

void SimulationClock::setStep(SimulationStep step) {
    try {
        NLM_VALIDATE(step <= std::numeric_limits<SimulationStep>::max(), 
                   "Step value exceeds maximum allowed value");
        
        pImpl->step = step;
        pImpl->time = step * pImpl->timestep;
        
        // Validate time
        pImpl->validateTimestamp(pImpl->time);
        
    } catch (const NLMError& e) {
        pImpl->errorHandler.addError(std::make_shared<ValidationError>(
            "Failed to set step: " + std::string(e.what()), "SimulationClock"));
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "SimulationClock"));
    }
}

Timestamp SimulationClock::getTime() const {
    try {
        return pImpl->time;
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "SimulationClock"));
        return 0.0;
    }
}

void SimulationClock::setTime(Timestamp time) {
    try {
        pImpl->validateTimestamp(time);
        
        NLM_VALIDATE(pImpl->timestep > 0.0, "Timestep must be positive");
        
        // Check for overflow
        if (time > std::numeric_limits<double>::max() / 1000.0) {
            NLM_THROW_RUNTIME_ERROR("Time value exceeds maximum allowed value");
        }
        
        pImpl->time = time;
        pImpl->step = static_cast<SimulationStep>(time / pImpl->timestep);
        
    } catch (const NLMError& e) {
        pImpl->errorHandler.addError(std::make_shared<ValidationError>(
            "Failed to set time: " + std::string(e.what()), "SimulationClock"));
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "SimulationClock"));
    }
}

TimestepDuration SimulationClock::getTimestep() const {
    try {
        return pImpl->timestep;
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "SimulationClock"));
        return 0.001; // Default timestep
    }
}

void SimulationClock::setTimestep(TimestepDuration dt) {
    try {
        pImpl->validateTimestep(dt);
        pImpl->timestep = dt;
    } catch (const NLMError& e) {
        pImpl->errorHandler.addError(std::make_shared<ValidationError>(
            "Failed to set timestep: " + std::string(e.what()), "SimulationClock"));
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "SimulationClock"));
    }
}

bool SimulationClock::isFirstStep() const {
    try {
        return pImpl->step == 0;
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "SimulationClock"));
        return true;
    }
}

bool SimulationClock::isLastStep(SimulationStep totalSteps) const {
    try {
        NLM_VALIDATE(totalSteps <= std::numeric_limits<SimulationStep>::max(),
                   "Total steps exceeds maximum allowed value");
        
        return pImpl->step >= totalSteps;
    } catch (const NLMError& e) {
        pImpl->errorHandler.addError(std::make_shared<ValidationError>(
            "Failed to check last step: " + std::string(e.what()), "SimulationClock"));
        return false;
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "SimulationClock"));
        return false;
    }
}

Timestamp SimulationClock::stepToTime(SimulationStep step) const {
    try {
        NLM_VALIDATE(static_cast<uint64_t>(step) <= std::numeric_limits<SimulationStep>::max(),
                   "Step value exceeds maximum allowed value");
        
        return static_cast<Timestamp>(step) * pImpl->timestep;
    } catch (const NLMError& e) {
        pImpl->errorHandler.addError(std::make_shared<ValidationError>(
            "Failed to convert step to time: " + std::string(e.what()), "SimulationClock"));
        return 0.0;
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "SimulationClock"));
        return 0.0;
    }
}

SimulationStep SimulationClock::timeToStep(Timestamp time) const {
    try {
        pImpl->validateTimestamp(time);
        
        NLM_VALIDATE(pImpl->timestep > 0.0, "Timestep must be positive");
        
        return static_cast<SimulationStep>(time / pImpl->timestep);
    } catch (const NLMError& e) {
        pImpl->errorHandler.addError(std::make_shared<ValidationError>(
            "Failed to convert time to step: " + std::string(e.what()), "SimulationClock"));
        return 0;
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "SimulationClock"));
        return 0;
    }
}

void SimulationClock::advanceBy(SimulationStep steps) {
    try {
        NLM_VALIDATE(steps <= std::numeric_limits<SimulationStep>::max(),
                   "Step count exceeds maximum allowed value");
        
        // Check for overflow
        if (pImpl->time + steps * pImpl->timestep > std::numeric_limits<double>::max() / 2.0) {
            NLM_THROW_RUNTIME_ERROR("Step advance would cause time overflow");
        }
        
        pImpl->step += steps;
        pImpl->time += steps * pImpl->timestep;
        
        // Validate time
        pImpl->validateTimestamp(pImpl->time);
        
        NLM_THROW_ON_ERROR();
        
    } catch (const NLMError& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(
            "Failed to advance clock by steps: " + std::string(e.what()), "SimulationClock"));
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "SimulationClock"));
    }
}

Timestamp SimulationClock::timeAtFutureStep(SimulationStep stepsAhead) const {
    try {
        NLM_VALIDATE(stepsAhead <= std::numeric_limits<SimulationStep>::max(),
                   "Step count exceeds maximum allowed value");
        
        Timestamp result = pImpl->time + (stepsAhead * pImpl->timestep);
        pImpl->validateTimestamp(result);
        
        return result;
    } catch (const NLMError& e) {
        pImpl->errorHandler.addError(std::make_shared<ValidationError>(
            "Failed to calculate future time: " + std::string(e.what()), "SimulationClock"));
        return pImpl->time;
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "SimulationClock"));
        return pImpl->time;
    }
}

double SimulationClock::getRealTimeElapsed() const {
    try {
        if (!pImpl->realStartSet) {
            return 0.0;
        }
        
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(now - pImpl->realStart);
        
        double result = duration.count();
        
        NLM_VALIDATE(std::isfinite(result), "Real time elapsed is not finite");
        
        return result;
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "SimulationClock"));
        return 0.0;
    }
}

void SimulationClock::setRealTimeStart() {
    try {
        pImpl->realStart = std::chrono::steady_clock::now();
        pImpl->realStartSet = true;
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "SimulationClock"));
    }
}

} // namespace nlm
