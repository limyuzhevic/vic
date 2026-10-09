// Simulation clock implementation

#include "SimulationClock.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <stdexcept>

namespace nlm {

struct SimulationClock::Impl {
    double timestep;
    SimulationStep currentStep;
    Timestamp currentTime;
    double realTimeStart;
    bool isPaused;
    bool realTimeMode;
    std::unordered_map<size_t, std::pair<Timestamp, bool>> scheduledEvents;
    std::chrono::system_clock::time_point realTimeStartPoint;
    double avgTimestep;
    double maxTimestep;
    double minTimestep;
    Timestamp maxTime;
    bool timeLimitReached;
};

SimulationClock::SimulationClock() : pImpl(new Impl) {
    pImpl->timestep = 0.001; // Default 1ms
    pImpl->currentStep = SimulationStep(0);
    pImpl->currentTime = Timestamp(0.0);
    pImpl->isPaused = false;
    pImpl->realTimeMode = false;
    pImpl->realTimeStart = 0.0;
    pImpl->avgTimestep = pImpl->timestep;
    pImpl->maxTimestep = pImpl->timestep;
    pImpl->minTimestep = pImpl->timestep;
    pImpl->maxTime = Timestamp(std::numeric_limits<double>::max());
    pImpl->timeLimitReached = false;
}

SimulationClock::SimulationClock(double timestep) : SimulationClock() {
    setTimestep(timestep);
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

void SimulationClock::setTimestep(double timestep) {
    if (!validateTimestep(timestep)) {
        throw std::invalid_argument("Invalid timestep value");
    }
    
    // Update statistics
    pImpl->minTimestep = std::min(pImpl->minTimestep, timestep);
    pImpl->maxTimestep = std::max(pImpl->maxTimestep, timestep);
    pImpl->avgTimestep = (pImpl->avgTimestep + timestep) / 2.0;
    
    pImpl->timestep = timestep;
}

bool SimulationClock::validateTimestep(double timestep) const {
    return timestep > 0.0 && timestep <= 0.1; // Reasonable timestep limits
}

double SimulationClock::getTimestep() const {
    return pImpl->timestep;
}

void SimulationClock::advanceStep() {
    if (isPaused()) {
        return;
    }
    
    pImpl->currentStep = SimulationStep(pImpl->currentStep.index() + 1);
    pImpl->currentTime = Timestamp(pImpl->currentTime.value + pImpl->timestep);
}

SimulationStep SimulationClock::getCurrentStep() const {
    return pImpl->currentStep;
}

Timestamp SimulationClock::getCurrentTime() const {
    return pImpl->currentTime;
}

void SimulationClock::syncRealTime() {
    if (!pImpl->realTimeMode) {
        return;
    }
    
    auto now = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(now - pImpl->realTimeStartPoint);
    
    double expectedSteps = elapsed.count() / pImpl->timestep;
    size_t expectedStep = static_cast<size_t>(expectedSteps);
    
    // Adjust simulation time to match real time
    pImpl->currentTime = Timestamp(expectedStep * pImpl->timestep);
}

void SimulationClock::setRealTimeMode(bool enabled) {
    if (enabled && !pImpl->realTimeMode) {
        pImpl->realTimeStart = 0.0;
        pImpl->realTimeStartPoint = std::chrono::system_clock::now();
        pImpl->realTimeMode = true;
    } else if (!enabled && pImpl->realTimeMode) {
        pImpl->realTimeMode = false;
    }
}

bool SimulationClock::isRealTimeMode() const {
    return pImpl->realTimeMode;
}

Timestamp SimulationClock::stepToTime(SimulationStep step) const {
    return Timestamp(step.value * pImpl->timestep);
}

SimulationStep SimulationClock::timeToStep(Timestamp time) const {
    return SimulationStep(static_cast<size_t>(time.value / pImpl->timestep));
}

bool SimulationClock::scheduleEvent(Timestamp time, size_t eventId) {
    if (validateTime(time) && !pImpl->timeLimitReached) {
        pImpl->scheduledEvents[eventId] = std::make_pair(time, false);
        return true;
    }
    return false;
}

bool SimulationClock::cancelEvent(size_t eventId) {
    auto it = pImpl->scheduledEvents.find(eventId);
    if (it != pImpl->scheduledEvents.end()) {
        pImpl->scheduledEvents.erase(it);
        return true;
    }
    return false;
}

std::vector<std::pair<Timestamp, size_t>> SimulationClock::getScheduledEvents() const {
    std::vector<std::pair<Timestamp, size_t>> result;
    result.reserve(pImpl->scheduledEvents.size());
    
    for (const auto& pair : pImpl->scheduledEvents) {
        result.push_back(std::make_pair(pair.second.first, pair.first));
    }
    
    return result;
}

double SimulationClock::getAverageTimestep() const {
    return pImpl->avgTimestep;
}

double SimulationClock::getMaxTimestep() const {
    return pImpl->maxTimestep;
}

double SimulationClock::getMinTimestep() const {
    return pImpl->minTimestep;
}

size_t SimulationClock::getStepCount() const {
    return pImpl->currentStep.index();
}

void SimulationClock::setMaxTime(Timestamp maxTime) {
    pImpl->maxTime = maxTime;
    pImpl->timeLimitReached = (pImpl->currentTime >= maxTime);
}

Timestamp SimulationClock::getMaxTime() const {
    return pImpl->maxTime;
}

bool SimulationClock::hasReachedTimeLimit() const {
    return pImpl->currentTime >= pImpl->maxTime;
}

void SimulationClock::pause() {
    pImpl->isPaused = true;
}

void SimulationClock::resume() {
    pImpl->isPaused = false;
}

bool SimulationClock::isPaused() const {
    return pImpl->isPaused;
}

SimulationClock::TimeSnapshot SimulationClock::getSnapshot() const {
    TimeSnapshot snapshot;
    snapshot.step = pImpl->currentStep;
    snapshot.time = pImpl->currentTime;
    snapshot.realTime = pImpl->realTimeStart;
    snapshot.timestep = pImpl->timestep;
    return snapshot;
}

void SimulationClock::restoreSnapshot(const TimeSnapshot& snapshot) {
    pImpl->currentStep = snapshot.step;
    pImpl->currentTime = snapshot.time;
    pImpl->realTimeStart = snapshot.realTime;
    pImpl->timestep = snapshot.timestep;
}

bool SimulationClock::validateTime(Timestamp time) const {
    return time.value >= 0.0 && time.value <= pImpl->maxTime.value;
}

} // namespace nlm
