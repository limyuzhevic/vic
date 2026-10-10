#include "SimulationClock.hpp"
#include "../ErrorHandling.hpp"
#include <chrono>

namespace nlm {

struct SimulationClock::Impl {
    SimulationStep step;
    Timestamp time;
    TimestepDuration timestep;
    std::chrono::steady_clock::time_point realStart;
    bool realStartSet;
    
    Impl(TimestepDuration dt) 
        : step(0), time(0.0), timestep(dt), realStartSet(false) {}
};

SimulationClock::SimulationClock(TimestepDuration timestep) : pImpl(std::make_unique<Impl>(timestep)) {}

SimulationClock::~SimulationClock() = default;

SimulationClock::SimulationClock(SimulationClock&& other) noexcept : pImpl(std::move(other.pImpl)) {}

SimulationClock& SimulationClock::operator=(SimulationClock&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void SimulationClock::reset() {
    try {
        pImpl->step = 0;
        pImpl->time = 0.0;
        pImpl->realStartSet = false;
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::Component_OperationFailed,
                       "Failed to reset simulation clock");
    }
}

void SimulationClock::advance() {
    try {
        ++pImpl->step;
        pImpl->time += pImpl->timestep;
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::Component_OperationFailed,
                       "Failed to advance simulation clock");
    }
}

SimulationStep SimulationClock::getStep() const {
    try {
        return pImpl->step;
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::Component_OperationFailed,
                       "Failed to get simulation step");
    }
}

void SimulationClock::setStep(SimulationStep step) {
    try {
        pImpl->step = step;
        pImpl->time = step * pImpl->timestep;
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::Component_OperationFailed,
                       "Failed to set simulation step");
    }
}

Timestamp SimulationClock::getTime() const {
    try {
        return pImpl->time;
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::Component_OperationFailed,
                       "Failed to get simulation time");
    }
}

void SimulationClock::setTime(Timestamp time) {
    try {
        pImpl->time = time;
        pImpl->step = static_cast<SimulationStep>(time / pImpl->timestep);
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::Component_OperationFailed,
                       "Failed to set simulation time");
    }
}

TimestepDuration SimulationClock::getTimestep() const {
    try {
        return pImpl->timestep;
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::Component_OperationFailed,
                       "Failed to get simulation timestep");
    }
}

void SimulationClock::setTimestep(TimestepDuration dt) {
    try {
        pImpl->timestep = dt;
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::Component_OperationFailed,
                       "Failed to set simulation timestep");
    }
}

bool SimulationClock::isFirstStep() const {
    try {
        return pImpl->step == 0;
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::Component_OperationFailed,
                       "Failed to check if simulation is at first step");
    }
}

bool SimulationClock::isLastStep(SimulationStep totalSteps) const {
    try {
        return pImpl->step >= totalSteps;
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::Component_OperationFailed,
                       "Failed to check if simulation is at last step");
    }
}

Timestamp SimulationClock::stepToTime(SimulationStep step) const {
    try {
        return step * pImpl->timestep;
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::Component_OperationFailed,
                       "Failed to convert step to time");
    }
}

SimulationStep SimulationClock::timeToStep(Timestamp time) const {
    try {
        return static_cast<SimulationStep>(time / pImpl->timestep);
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::Component_OperationFailed,
                       "Failed to convert time to step");
    }
}

void SimulationClock::advanceBy(SimulationStep steps) {
    try {
        pImpl->step += steps;
        pImpl->time += steps * pImpl->timestep;
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::Component_OperationFailed,
                       "Failed to advance simulation by steps");
    }
}

Timestamp SimulationClock::timeAtFutureStep(SimulationStep stepsAhead) const {
    try {
        return pImpl->time + (stepsAhead * pImpl->timestep);
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::Component_OperationFailed,
                       "Failed to get time at future step");
    }
}

double SimulationClock::getRealTimeElapsed() const {
    try {
        if (!pImpl->realStartSet) {
            return 0.0;
        }
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(now - pImpl->realStart);
        return duration.count();
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::Component_OperationFailed,
                       "Failed to get real time elapsed");
    }
}

void SimulationClock::setRealTimeStart() {
    try {
        pImpl->realStart = std::chrono::steady_clock::now();
        pImpl->realStartSet = true;
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::Component_OperationFailed,
                       "Failed to set real time start");
    }
}

} // namespace nlm
