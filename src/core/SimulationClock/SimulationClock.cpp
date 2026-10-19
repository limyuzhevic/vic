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
        : step(0), time(0.0), timestep(dt), realStartSet(false) {}
};

SimulationClock::SimulationClock(TimestepDuration timestep) {
    if (timestep <= 0.0) {
        throw std::invalid_argument("Timestep must be positive: " + std::to_string(timestep));
    }
    if (timestep > 1.0) {
        throw std::invalid_argument("Timestep too large (max 1.0s): " + std::to_string(timestep));
    }
    pImpl = new Impl(timestep);
}

SimulationClock::~SimulationClock() {
    delete pImpl;
}

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
    pImpl->step = 0;
    pImpl->time = 0.0;
    pImpl->realStartSet = false;
}

void SimulationClock::advance() {
    ++pImpl->step;
    pImpl->time += pImpl->timestep;
}

SimulationStep SimulationClock::getStep() const {
    return pImpl->step;
}

void SimulationClock::setStep(SimulationStep step) {
    if (step < 0) {
        throw std::invalid_argument("Step cannot be negative: " + std::to_string(step));
    }
    pImpl->step = step;
    pImpl->time = step * pImpl->timestep;
}

Timestamp SimulationClock::getTime() const {
    return pImpl->time;
}

void SimulationClock::setTime(Timestamp time) {
    if (time < 0.0) {
        throw std::invalid_argument("Time cannot be negative: " + std::to_string(time));
    }
    pImpl->time = time;
    pImpl->step = static_cast<SimulationStep>(time / pImpl->timestep);
}

TimestepDuration SimulationClock::getTimestep() const {
    return pImpl->timestep;
}

void SimulationClock::setTimestep(TimestepDuration dt) {
    if (dt <= 0.0) {
        throw std::invalid_argument("Timestep must be positive: " + std::to_string(dt));
    }
    if (dt > 1.0) {
        throw std::invalid_argument("Timestep too large (max 1.0s): " + std::to_string(dt));
    }
    pImpl->timestep = dt;
}

bool SimulationClock::isFirstStep() const {
    return pImpl->step == 0;
}

bool SimulationClock::isLastStep(SimulationStep totalSteps) const {
    if (totalSteps < 0) {
        throw std::invalid_argument("Total steps cannot be negative: " + std::to_string(totalSteps));
    }
    return pImpl->step >= totalSteps;
}

Timestamp SimulationClock::stepToTime(SimulationStep step) const {
    if (step < 0) {
        throw std::invalid_argument("Step cannot be negative: " + std::to_string(step));
    }
    return step * pImpl->timestep;
}

SimulationStep SimulationClock::timeToStep(Timestamp time) const {
    if (time < 0.0) {
        throw std::invalid_argument("Time cannot be negative: " + std::to_string(time));
    }
    return static_cast<SimulationStep>(time / pImpl->timestep);
}

void SimulationClock::advanceBy(SimulationStep steps) {
    if (steps < 0) {
        throw std::invalid_argument("Steps cannot be negative: " + std::to_string(steps));
    }
    pImpl->step += steps;
    pImpl->time += steps * pImpl->timestep;
}

Timestamp SimulationClock::timeAtFutureStep(SimulationStep stepsAhead) const {
    if (stepsAhead < 0) {
        throw std::invalid_argument("Steps ahead cannot be negative: " + std::to_string(stepsAhead));
    }
    return pImpl->time + (stepsAhead * pImpl->timestep);
}

double SimulationClock::getRealTimeElapsed() const {
    if (!pImpl->realStartSet) {
        return 0.0;
    }
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(now - pImpl->realStart);
    return duration.count();
}

void SimulationClock::setRealTimeStart() {
    pImpl->realStart = std::chrono::steady_clock::now();
    pImpl->realStartSet = true;
}

} // namespace nlm
