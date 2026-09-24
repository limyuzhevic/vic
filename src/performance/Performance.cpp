#include "Performance.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nlm {

PerformanceMonitor::PerformanceMonitor() : startTime_(std::chrono::high_resolution_clock::now()) {}

PerformanceMonitor::~PerformanceMonitor() = default;

void PerformanceMonitor::recordStep(uint64_t step, uint64_t neurons, uint64_t synapses) {
    std::lock_guard<std::mutex> lock(mutex_);
    StepStats stats;
    stats.step = step;
    stats.neuronCount = neurons;
    stats.synapseCount = synapses;
    stats.timestamp = std::chrono::high_resolution_clock::now();
    
    if (!stepStats_.empty()) {
        auto& last = stepStats_.back();
        stats.wallTimeSinceLast = std::chrono::duration<double>(stats.timestamp - last.timestamp).count();
    }
    
    stepStats_.push_back(stats);
}

void PerformanceMonitor::recordMemoryUsage(size_t bytes) {
    std::lock_guard<std::mutex> lock(mutex_);
    memoryUsage_ = bytes;
    lastMemoryRecord_ = std::chrono::high_resolution_clock::now();
}

double PerformanceMonitor::getAverageStepTime() const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (stepStats_.size() < 2) return 0.0;
    
    double totalTime = 0.0;
    for (size_t i = 1; i < stepStats_.size(); ++i) {
        totalTime += stepStats_[i].wallTimeSinceLast;
    }
    
    return totalTime / (stepStats_.size() - 1);
}

double PerformanceMonitor::getCurrentStepTime() const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (stepStats_.empty()) return 0.0;
    
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(now - stepStats_.back().timestamp).count();
}

size_t PerformanceMonitor::getMaxMemoryUsage() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return maxMemoryUsage_;
}

double PerformanceMonitor::getTotalSimulationTime() const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto endTime = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(endTime - startTime_).count();
}

void PerformanceMonitor::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    startTime_ = std::chrono::high_resolution_clock::now();
    stepStats_.clear();
    memoryUsage_ = 0;
    maxMemoryUsage_ = 0;
}

std::string PerformanceMonitor::getPerformanceReport() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::stringstream ss;
    
    ss << "=== Performance Monitor Report ===\n";
    ss << "Total simulation time: " << getTotalSimulationTime() << "s\n";
    ss << "Current memory usage: " << memoryUsage_ / 1024 / 1024 << " MB\n";
    ss << "Max memory usage: " << maxMemoryUsage_ / 1024 / 1024 << " MB\n";
    ss << "Average step time: " << getAverageStepTime() * 1000 << " ms\n";
    ss << "Steps recorded: " << stepStats_.size() << "\n";
    
    if (!stepStats_.empty()) {
        ss << "Last step time: " << getCurrentStepTime() * 1000 << " ms\n";
        ss << "Neurons processed: " << stepStats_.back().neuronCount << "\n";
        ss << "Synapses processed: " << stepStats_.back().synapseCount << "\n";
    }
    
    return ss.str();
}

} // namespace nlm
