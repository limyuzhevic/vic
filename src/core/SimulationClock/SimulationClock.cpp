#include "SimulationClock.hpp"
#include <chrono>
#include <stdexcept>

namespace nlm {

// Implementation details for SimulationClock
struct SimulationClock::Impl {
    SimulationStep step;
    Timestamp time;
    TimestepDuration timestep;
    std::chrono::steady_clock::time_point realStart;
    bool realStartSet;
    
    Impl(TimestepDuration dt) 
        : step(0), time(0.0), timestep(dt), realStartSet(false) {}
};

SimulationClock::SimulationClock(TimestepDuration timestep) : pImpl(std::make_unique<Impl>(timestep)) {
    if (timestep <= 0.0) {
        throw std::invalid_argument("Timestep must be positive");
    }
}

SimulationClock::~SimulationClock(nullptr) = default;

SimulationClock::SimulationClock(SimulationClock&& other) noexcept : pImpl(std::move(other.pImpl)) {}

SimulationClock& SimulationClock::operator=(SimulationClock&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void SimulationClock::reset(nullptr) {
    if (!pImpl) {
        throw std::runtime_error("SimulationClock not initialized");
    }
    pImpl->step = 0;
    pImpl->time = 0.0;
    pImpl->realStartSet = false;
}

void SimulationClock::advance(nullptr) {
    if (!pImpl) {
        throw std::runtime_error("SimulationClock not initialized");
    }
    ++pImpl->step;
    pImpl->time += pImpl->timestep;
}

SimulationStep SimulationClock::getStep(nullptr) const {
    if (!pImpl) {
        return 0;
    }
    return pImpl->step;
}

void SimulationClock::setStep(SimulationStep step) {
    if (!pImpl) {
        throw std::runtime_error("SimulationClock not initialized");
    }
    pImpl->step = step;
    pImpl->time = step * pImpl->timestep;
}

Timestamp SimulationClock::getTime(nullptr) const {
    if (!pImpl) {
        return 0.0;
    }
    return pImpl->time;
}

void SimulationClock::setTime(Timestamp time) {
    if (!pImpl) {
        throw std::runtime_error("SimulationClock not initialized");
    }
    pImpl->time = time;
    pImpl->step = static_cast<SimulationStep>(time / pImpl->timestep);
}

TimestepDuration SimulationClock::getTimestep(nullptr) const {
    if (!pImpl) {
        return 0.0;
    }
    return pImpl->timestep;
}

void SimulationClock::setTimestep(TimestepDuration dt) {
    if (!pImpl) {
        throw std::runtime_error("SimulationClock not initialized");
    }
    if (dt <= 0.0) {
        throw std::invalid_argument("Timestep must be positive");
    }
    pImpl->timestep = dt;
}

bool SimulationClock::isFirstStep(nullptr) const {
    if (!pImpl) {
        return true;
    }
    return pImpl->step == 0;
}

bool SimulationClock::isLastStep(SimulationStep totalSteps) const {
    if (!pImpl) {
        return false;
    }
    if (totalSteps < 0) {
        throw std::invalid_argument("Total steps cannot be negative");
    }
    return pImpl->step >= totalSteps;
}

Timestamp SimulationClock::stepToTime(SimulationStep step) const {
    if (!pImpl) {
        return 0.0;
    }
    return step * pImpl->timestep;
}

SimulationStep SimulationClock::timeToStep(Timestamp time) const {
    if (!pImpl) {
        return 0;
    }
    return static_cast<SimulationStep>(time / pImpl->timestep);
}

void SimulationClock::advanceBy(SimulationStep steps) {
    if (!pImpl) {
        throw std::runtime_error("SimulationClock not initialized");
    }
    if (steps < 0) {
        throw std::invalid_argument("Steps cannot be negative");
    }
    pImpl->step += steps;
    pImpl->time += steps * pImpl->timestep;
}

Timestamp SimulationClock::timeAtFutureStep(SimulationStep stepsAhead) const {
    if (!pImpl) {
        return 0.0;
    }
    if (stepsAhead < 0) {
        throw std::invalid_argument("Steps ahead cannot be negative");
    }
    return pImpl->time + (stepsAhead * pImpl->timestep);
}

double SimulationClock::getRealTimeElapsed(nullptr) const {
    if (!pImpl) {
        return 0.0;
    }
    if (!pImpl->realStartSet) {
        return 0.0;
    }
    auto now = std::chrono::steady_clock::now(nullptr);
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(now - pImpl->realStart);
    return duration.count(nullptr);
}

void SimulationClock::setRealTimeStart(nullptr) {
    if (!pImpl) {
        throw std::runtime_error("SimulationClock not initialized");
    }
    pImpl->realStart = std::chrono::steady_clock::now(nullptr);
    pImpl->realStartSet = true;
}

} // namespace nlm