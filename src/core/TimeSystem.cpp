#include "TimeSystem.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <ctime>

namespace nlm {

// TimeSystem implementation
TimeSystem::TimeSystem() : pImpl(std::make_unique<Impl>()) {
    pImpl->simulationTime = 0.0;
    pImpl->currentStep = 0;
    pImpl->timestep = DEFAULT_TIME_STEP;
    pImpl->lastRealTime = std::chrono::high_resolution_clock::now();
    pImpl->totalSimulationTime = 0.0;
    pImpl->totalSimulationSteps = 0;
    pImpl->minStepDuration = std::chrono::duration<double>::max();
    pImpl->maxStepDuration = std::chrono::duration<double>::min();
}

TimeSystem::~TimeSystem() = default;

TimeSystem& TimeSystem::getInstance() {
    static TimeSystem instance;
    return instance;
}

double TimeSystem::getCurrentRealTime() const {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(now - pImpl->lastRealTime);
    return duration.count();
}

Timestamp TimeSystem::getCurrentSimulationTime() const {
    return pImpl->simulationTime;
}

SimulationStep TimeSystem::getCurrentStep() const {
    return pImpl->currentStep;
}

TimestepDuration TimeSystem::getTimestep() const {
    return pImpl->timestep;
}

void TimeSystem::setTimestep(TimestepDuration timestep) {
    if (timestep <= 0.0) {
        throw std::invalid_argument("Timestep must be positive");
    }
    pImpl->timestep = timestep;
}

void TimeSystem::advanceSimulation(double timeDelta) {
    if (timeDelta < 0.0) {
        throw std::invalid_argument("Time delta cannot be negative");
    }
    
    pImpl->simulationTime += timeDelta;
    pImpl->currentStep = static_cast<SimulationStep>(pImpl->simulationTime / pImpl->timestep);
    pImpl->totalSimulationTime += timeDelta;
}

void TimeSystem::advanceSteps(SimulationStep steps) {
    if (steps == 0) return;
    
    double timeDelta = steps * pImpl->timestep;
    advanceSimulation(timeDelta);
}

void TimeSystem::recordPerformance(const std::string& operation, 
                                   const std::chrono::high_resolution_clock::duration& duration,
                                   const std::map<std::string, std::string>& context) {
    auto now = std::chrono::high_resolution_clock::now();
    auto timeSinceEpoch = std::chrono::time_point_cast<std::chrono::duration<double>>(now);
    
    PerformanceMetric metric;
    metric.operation = operation;
    metric.duration = duration;
    metric.context = context;
    metric.timestamp = timeSinceEpoch.time_since_epoch().count();
    
    pImpl->performanceMetrics.push_back(metric);
    
    // Keep only recent metrics
    if (pImpl->performanceMetrics.size() > MAX_PERFORMANCE_METRICS) {
        pImpl->performanceMetrics.erase(
            pImpl->performanceMetrics.begin(),
            pImpl->performanceMetrics.begin() + (pImpl->performanceMetrics.size() - MAX_PERFORMANCE_METRICS)
        );
    }
    
    // Update statistics
    double durationSeconds = std::chrono::duration<double>(duration).count();
    pImpl->totalStepDuration += durationSeconds;
    pImpl->totalStepCount++;
    
    if (durationSeconds < pImpl->minStepDuration) {
        pImpl->minStepDuration = durationSeconds;
    }
    if (durationSeconds > pImpl->maxStepDuration) {
        pImpl->maxStepDuration = durationSeconds;
    }
}

std::vector<TimeSystem::PerformanceMetric> TimeSystem::getPerformanceMetrics(
    const std::string& operation, Timestamp startTime, Timestamp endTime) const {
    
    std::vector<PerformanceMetric> filteredMetrics;
    
    for (const auto& metric : pImpl->performanceMetrics) {
        if (!operation.empty() && metric.operation != operation) continue;
        if (metric.timestamp < startTime) continue;
        if (metric.timestamp > endTime) continue;
        
        filteredMetrics.push_back(metric);
    }
    
    return filteredMetrics;
}

void TimeSystem::clearPerformanceMetrics() {
    pImpl->performanceMetrics.clear();
    pImpl->totalStepCount = 0;
    pImpl->totalStepDuration = 0.0;
}

void TimeSystem::addTimer(const std::string& name, Timestamp delay, Timestamp period) {
    Timer timer(name, delay, period);
    pImpl->timers.push_back(timer);
}

void TimeSystem::removeTimer(const std::string& name) {
    pImpl->timers.erase(
        std::remove_if(pImpl->timers.begin(), pImpl->timers.end(),
            [&name](const Timer& timer) { return timer.getName() == name; }),
        pImpl->timers.end()
    );
}

void TimeSystem::updateTimers(Timestamp currentTime) {
    for (auto& timer : pImpl->timers) {
        if (timer.isReady(currentTime)) {
            timer.lastExecuted_ = currentTime;
            // In a real implementation, this would trigger the timer callback
        }
    }
}

std::string TimeSystem::formatTime(Timestamp time, const std::string& format) const {
    auto timePoint = std::chrono::system_clock::time_point(
        std::chrono::duration<double>(time)
    );
    
    std::time_t timeT = std::chrono::system_clock::to_time_t(timePoint);
    std::tm tm = *std::localtime(&timeT);
    
    std::ostringstream oss;
    oss << std::put_time(&tm, format.c_str());
    
    return oss.str();
}

std::string TimeSystem::formatDuration(std::chrono::high_resolution_clock::duration duration) const {
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    duration -= seconds;
    
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    duration -= milliseconds;
    
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration);
    
    std::ostringstream oss;
    oss << seconds.count() << "."
        << std::setfill('0') << std::setw(3) << milliseconds.count() % 1000
        << std::setfill('0') << std::setw(3) << microseconds.count() % 1000;
    
    return oss.str();
}

TimeSystem::TimeStatistics TimeSystem::getStatistics() const {
    TimeStatistics stats;
    
    stats.totalSimulationTime = pImpl->totalSimulationTime;
    stats.totalSimulationSteps = pImpl->totalSimulationSteps;
    stats.performanceMetricCount = pImpl->performanceMetrics.size();
    
    if (pImpl->totalStepCount > 0) {
        stats.averageStepDuration = pImpl->totalStepDuration / pImpl->totalStepCount;
    } else {
        stats.averageStepDuration = 0.0;
    }
    
    stats.minStepDuration = pImpl->minStepDuration;
    stats.maxStepDuration = pImpl->maxStepDuration;
    
    return stats;
}

TimeSystem::TimeSnapshot TimeSystem::takeSnapshot() const {
    TimeSnapshot snapshot;
    
    snapshot.simulationTime = pImpl->simulationTime;
    snapshot.realTime = getCurrentRealTime();
    snapshot.step = pImpl->currentStep;
    snapshot.timestep = pImpl->timestep;
    snapshot.snapshotTime = std::chrono::high_resolution_clock::now();
    
    return snapshot;
}

void TimeSystem::setTimeRange(Timestamp startTime, Timestamp endTime) {
    pImpl->timeRangeStart = startTime;
    pImpl->timeRangeEnd = endTime;
}

bool TimeSystem::isTimeInRange(Timestamp time) const {
    return time >= pImpl->timeRangeStart && time <= pImpl->timeRangeEnd;
}

Timestamp TimeSystem::clampToRange(Timestamp time) const {
    if (time < pImpl->timeRangeStart) return pImpl->timeRangeStart;
    if (time > pImpl->timeRangeEnd) return pImpl->timeRangeEnd;
    return time;
}

} // namespace nlm