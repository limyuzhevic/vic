#include "SimulationClock.hpp"
#include <chrono>
#include <mutex>

namespace nlm {

struct SimulationClock::Impl {
    SimulationStep step;
    Timestamp time;
    TimestepDuration timestep;
    std::chrono::steady_clock::time_point realStart;
    bool realStartSet;
    mutable std::mutex mtx;  // Thread safety
    
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
    std::lock_guard<std::mutex> lock(pImpl->mtx);
    pImpl->step = 0;
    pImpl->time = 0.0;
    pImpl->realStartSet = false;
}

void SimulationClock::advance() {
    std::lock_guard<std::mutex> lock(pImpl->mtx);
    ++pImpl->step;
    pImpl->time += pImpl->timestep;
}

SimulationStep SimulationClock::getStep() const {
    std::lock_guard<std::mutex> lock(pImpl->mtx);
    return pImpl->step;
}

void SimulationClock::setStep(SimulationStep step) {
    std::lock_guard<std::mutex> lock(pImpl->mtx);
    pImpl->step = step;
    pImpl->time = step * pImpl->timestep;
}

Timestamp SimulationClock::getTime() const {
    std::lock_guard<std::mutex> lock(pImpl->mtx);
    return pImpl->time;
}

void SimulationClock::setTime(Timestamp time) {
    std::lock_guard<std::mutex> lock(pImpl->mtx);
    pImpl->time = time;
    pImpl->step = static_cast<SimulationStep>(time / pImpl->timestep);
}

TimestepDuration SimulationClock::getTimestep() const {
    std::lock_guard<std::mutex> lock(pImpl->mtx);
    return pImpl->timestep;
}

void SimulationClock::setTimestep(TimestepDuration dt) {
    std::lock_guard<std::mutex> lock(pImpl->mtx);
    pImpl->timestep = dt;
}

bool SimulationClock::isFirstStep() const {
    std::lock_guard<std::mutex> lock(pImpl->mtx);
    return pImpl->step == 0;
}

bool SimulationClock::isLastStep(SimulationStep totalSteps) const {
    std::lock_guard<std::mutex> lock(pImpl->mtx);
    return pImpl->step >= totalSteps;
}

Timestamp SimulationClock::stepToTime(SimulationStep step) const {
    std::lock_guard<std::mutex> lock(pImpl->mtx);
    return step * pImpl->timestep;
}

SimulationStep SimulationClock::timeToStep(Timestamp time) const {
    std::lock_guard<std::mutex> lock(pImpl->mtx);
    return static_cast<SimulationStep>(time / pImpl->timestep);
}

void SimulationClock::advanceBy(SimulationStep steps) {
    std::lock_guard<std::mutex> lock(pImpl->mtx);
    pImpl->step += steps;
    pImpl->time += steps * pImpl->timestep;
}

Timestamp SimulationClock::timeAtFutureStep(SimulationStep stepsAhead) const {
    std::lock_guard<std::mutex> lock(pImpl->mtx);
    return pImpl->time + (stepsAhead * pImpl->timestep);
}

double SimulationClock::getRealTimeElapsed() const {
    std::lock_guard<std::mutex> lock(pImpl->mtx);
    if (!pImpl->realStartSet) {
        return 0.0;
    }
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(now - pImpl->realStart);
    return duration.count();
}

void SimulationClock::setRealTimeStart() {
    std::lock_guard<std::mutex> lock(pImpl->mtx);
    pImpl->realStart = std::chrono::steady_clock::now();
    pImpl->realStartSet = true;
}

} // namespace nlm
