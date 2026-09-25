#include "advanced/PerformanceMonitoring.hpp"
#include <iostream>
#include <iomanip>
#include <numeric>

namespace nlm {

PerformanceMonitor::PerformanceMonitor(size_t maxHistory) : maxHistorySize(maxHistory) {
    startTime = std::chrono::steady_clock::now();
}

void PerformanceMonitor::recordPerformance(double cpu, double memory, size_t spikes, size_t steps) {
    PerformanceData data;
    data.cpuUsage = cpu;
    data.memoryUsage = memory;
    data.totalSpikes = spikes;
    data.totalSteps = steps;
    data.timestamp = std::chrono::steady_clock::now();
    
    history.push_back(data);
    if (history.size() > maxHistorySize) {
        history.erase(history.begin());
    }
}

double PerformanceMonitor::calculateAverageCPU() const {
    if (history.empty()) return 0.0;
    double sum = 0.0;
    for (const auto& data : history) {
        sum += data.cpuUsage;
    }
    return sum / history.size();
}

double PerformanceMonitor::calculateAverageMemory() const {
    if (history.empty()) return 0.0;
    double sum = 0.0;
    for (const auto& data : history) {
        sum += data.memoryUsage;
    }
    return sum / history.size();
}

size_t PerformanceMonitor::getCurrentStep() const {
    return history.empty() ? 0 : history.back().totalSteps;
}

size_t PerformanceMonitor::getTotalSpikeCount() const {
    return history.empty() ? 0 : history.back().totalSpikes;
}

void PerformanceMonitor::printPerformanceReport() const {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                       NLM Performance Monitoring Report                 ║" << std::endl;
    std::cout << "╠═════════════╦══════════════╦══════════════╦══════════════╦════════════╣" << std::endl;
    std::cout << "║   CPU Usage ║   Memory Usage║   Total Steps║   Total Spikes║  Steps/sec ║" << std::endl;
    std::cout << "╠═════════════╬══════════════╬══════════════╬══════════════╬════════════╣" << std::endl;
    
    double avgCpu = calculateAverageCPU();
    double avgMemory = calculateAverageMemory();
    size_t totalSteps = getCurrentStep();
    size_t totalSpikes = getTotalSpikeCount();
    double stepsPerSecond = totalSteps / 60.0; // Approximate
    
    std::cout << "║ " << std::setw(11) << std::fixed << std::setprecision(1) << avgCpu << "% "
              << "║ " << std::setw(14) << std::fixed << std::setprecision(1) << avgMemory << "% "
              << "║ " << std::setw(14) << totalSteps 
              << " ║ " << std::setw(14) << totalSpikes
              << " ║ " << std::setw(12) << stepsPerSecond << " ║" << std::endl;
    
    std::cout << "╚═════════════╩══════════════╩══════════════╩══════════════╩════════════╝" << std::endl;
    std::cout << std::endl;
}

} // namespace nlm