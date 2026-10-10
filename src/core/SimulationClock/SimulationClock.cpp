// Simulation clock system implementation for NLM
#include "SimulationClock.hpp"
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace nlm {

SimulationClock::SimulationClock(double timestep) 
    : currentTime_(0.0), currentStep_(0), timestep_(timestep), 
      running_(false), startTime_(), realTimeElapsed_(0.0) {
    if (timestep_ <= 0.0) {
        timestep_ = 0.001;
    }
}

void SimulationClock::advance() {
    if (!running_) {
        running_ = true;
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    currentStep_++;
    currentTime_ = currentStep_ * timestep_;
    updateRealTime();
}

Timestamp SimulationClock::getTime() const {
    return currentTime_;
}

SimulationStep SimulationClock::getStep() const {
    return currentStep_;
}

double SimulationClock::getTimestep() const {
    return timestep_;
}

void SimulationClock::setTimestep(double timestep) {
    if (timestep > 0.0) {
        timestep_ = timestep;
    }
}

void SimulationClock::reset() {
    currentTime_ = 0.0;
    currentStep_ = 0;
    running_ = false;
    realTimeElapsed_ = std::chrono::duration<double>(0.0);
}

bool SimulationClock::isRunning() const {
    return running_;
}

double SimulationClock::getRealTimeElapsed() const {
    return realTimeElapsed_.count();
}

double SimulationClock::getSimulationTimeElapsed() const {
    return currentTime_;
}

std::string SimulationClock::getFormattedTime() const {
    auto hours = static_cast<int>(currentTime_ / 3600.0);
    auto minutes = static_cast<int>((currentTime_ - hours * 3600.0) / 60.0);
    auto seconds = static_cast<int>(currentTime_ - hours * 3600.0 - minutes * 60.0);
    auto milliseconds = static_cast<int>((currentTime_ - hours * 3600.0 - minutes * 60.0 - seconds) * 1000.0);
    
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << hours << ":"
       << std::setw(2) << minutes << ":"
       << std::setw(2) << seconds << "."
       << std::setw(3) << milliseconds;
    
    return ss.str();
}

std::string SimulationClock::getFormattedSimulationTime() const {
    auto hours = static_cast<int>(currentTime_ / 3600.0);
    auto minutes = static_cast<int>((currentTime_ - hours * 3600.0) / 60.0);
    auto seconds = static_cast<int>(currentTime_ - hours * 3600.0 - minutes * 60.0);
    auto milliseconds = static_cast<int>((currentTime_ - hours * 3600.0 - minutes * 60.0 - seconds) * 1000.0);
    
    std::stringstream ss;
    ss << hours << "h " << minutes << "m " << seconds << "s " << milliseconds << "ms";
    
    return ss.str();
}

SimulationClock SimulationClock::copy() const {
    SimulationClock copy(timestep_);
    copy.currentTime_ = currentTime_;
    copy.currentStep_ = currentStep_;
    copy.running_ = running_;
    copy.realTimeElapsed_ = realTimeElapsed_;
    copy.startTime_ = startTime_;
    return copy;
}

void SimulationClock::restore(const SimulationClock& other) {
    currentTime_ = other.currentTime_;
    currentStep_ = other.currentStep_;
    timestep_ = other.timestep_;
    running_ = other.running_;
    realTimeElapsed_ = other.realTimeElapsed_;
    startTime_ = other.startTime_;
}

void SimulationClock::updateRealTime() {
    if (running_) {
        auto now = std::chrono::high_resolution_clock::now();
        realTimeElapsed_ = std::chrono::duration<double>(now - startTime_);
    }
}

} // namespace nlm