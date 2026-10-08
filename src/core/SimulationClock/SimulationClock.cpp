#include "SimulationClock.hpp"
#include <chrono>
#include <algorithm>

namespace nlm {

struct SimulationClock::Impl {
    SimulationStep currentStep;
    Timestamp currentTime;
    TimestepDuration timestep;
    
    // Real-time tracking
    std::chrono::steady_clock::time_point realTimeStart;
    bool realTimeMode;
    
    Impl(TimestepDuration ts) 
        : currentStep(0), currentTime(0.0), timestep(ts)
        , realTimeMode(false) {}
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
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->realTimeMode = false;
}

void SimulationClock::advance() {
    pImpl->currentStep++;
    pImpl->currentTime = pImpl->currentStep * pImpl->timestep;
}

SimulationStep SimulationClock::getStep() const {
    return pImpl->currentStep;
}

void SimulationClock::setStep(SimulationStep step) {
    pImpl->currentStep = step;
    pImpl->currentTime = step * pImpl->timestep;
}

Timestamp SimulationClock::getTime() const {
    if (pImpl->realTimeMode) {
        auto elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - pImpl->realTimeStart).count();
        return elapsed;
    }
    return pImpl->currentTime;
}

void SimulationClock::setTime(Timestamp time) {
    pImpl->currentTime = time;
    pImpl->currentStep = static_cast<SimulationStep>(time / pImpl->timestep);
}

TimestepDuration SimulationClock::getTimestep() const {
    return pImpl->timestep;
}

void SimulationClock::setTimestep(TimestepDuration dt) {
    pImpl->timestep = dt;
}

bool SimulationClock::isFirstStep() const {
    return pImpl->currentStep == 0;
}

bool SimulationClock::isLastStep(SimulationStep totalSteps) const {
    return pImpl->currentStep >= totalSteps;
}

Timestamp SimulationClock::stepToTime(SimulationStep step) const {
    return step * pImpl->timestep;
}

SimulationStep SimulationClock::timeToStep(Timestamp time) const {
    return static_cast<SimulationStep>(time / pImpl->timestep);
}

void SimulationClock::advanceBy(SimulationStep steps) {
    pImpl->currentStep += steps;
    pImpl->currentTime = pImpl->currentStep * pImpl->timestep;
}

Timestamp SimulationClock::timeAtFutureStep(SimulationStep stepsAhead) const {
    return (pImpl->currentStep + stepsAhead) * pImpl->timestep;
}

double SimulationClock::getRealTimeElapsed() const {
    if (!pImpl->realTimeMode) return 0.0;
    auto elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - pImpl->realTimeStart).count();
    return elapsed;
}

void SimulationClock::setRealTimeStart() {
    pImpl->realTimeStart = std::chrono::steady_clock::now();
    pImpl->realTimeMode = true;
}

} // namespace nlm
