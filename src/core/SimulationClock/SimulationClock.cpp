#include "SimulationClock.hpp"
#include <chrono>
#include <numeric>

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

SimulationClock::SimulationClock(TimestepDuration timestep) : pImpl(std::make_unique<Impl>(timestep)) {
}

SimulationClock::~SimulationClock() = default;

SimulationClock::SimulationClock(SimulationClock&& other) noexcept : pImpl(std::move(other.pImpl)) {
}

SimulationClock& SimulationClock::operator=(SimulationClock&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
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
    pImpl->step = step;
    pImpl->time = step * pImpl->timestep;
}

Timestamp SimulationClock::getTime() const {
    return pImpl->time;
}

void SimulationClock::setTime(Timestamp time) {
    pImpl->time = time;
    pImpl->step = static_cast<SimulationStep>(time / pImpl->timestep);
}

TimestepDuration SimulationClock::getTimestep() const {
    return pImpl->timestep;
}

void SimulationClock::setTimestep(TimestepDuration dt) {
    pImpl->timestep = dt;
}

bool SimulationClock::isFirstStep() const {
    return pImpl->step == 0;
}

bool SimulationClock::isLastStep(SimulationStep totalSteps) const {
    return pImpl->step >= totalSteps;
}

Timestamp SimulationClock::stepToTime(SimulationStep step) const {
    return step * pImpl->timestep;
}

SimulationStep SimulationClock::timeToStep(Timestamp time) const {
    return static_cast<SimulationStep>(time / pImpl->timestep);
}

void SimulationClock::advanceBy(SimulationStep steps) {
    pImpl->step += steps;
    pImpl->time += steps * pImpl->timestep;
}

Timestamp SimulationClock::timeAtFutureStep(SimulationStep stepsAhead) const {
    return pImpl->time + (stepsAhead * pImpl->timestep);
}

double SimulationClock::getRealTimeElapsed() const {
    if (!pImpl->realStartSet) {
        return 0.0;
    }
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(now - pImpl->realStart);
    return elapsed.count();
}

void SimulationClock::setRealTimeStart() {
    pImpl->realStart = std::chrono::steady_clock::now();
    pImpl->realStartSet = true;
}

bool SimulationClock::operator==(const SimulationClock& other) const {
    return pImpl->step == other.pImpl->step && 
           pImpl->time == other.pImpl->time && 
           pImpl->timestep == other.pImpl->timestep &&
           pImpl->realStartSet == other.pImpl->realStartSet;
}

bool SimulationClock::operator!=(const SimulationClock& other) const {
    return !(*this == other);
}

size_t SimulationClock::hash() const {
    auto seed = std::hash<SimulationStep>{}(pImpl->step);
    seed ^= std::hash<Timestamp>{}(pImpl->time) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= std::hash<TimestepDuration>{}(pImpl->timestep) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= std::hash<bool>{}(pImpl->realStartSet) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    return seed;
}

} // namespace nlm
