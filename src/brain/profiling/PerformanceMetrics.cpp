#include "PerformanceMetrics.hpp"
#include <nlohmann/json.hpp>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>

using json = nlohmann::json;

namespace nlm {

struct PerformanceMetrics::Impl {
    TimeMetrics timeMetrics;
    MemoryMetrics memoryMetrics;
    NetworkMetrics networkMetrics;
    IntegrationMetrics integrationMetrics;
    SystemHealthMetrics healthMetrics;
    
    mutable std::mutex metricsMutex;
    
    Impl() : timeMetrics(), memoryMetrics(), networkMetrics(),
             integrationMetrics(), healthMetrics() {}
};

PerformanceMetrics::PerformanceMetrics() : pImpl(std::make_unique<Impl>()) {}

PerformanceMetrics::~PerformanceMetrics() = default;

void PerformanceMetrics::updateTimeMetrics(double wallTime, double simTime, double stepDuration) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    pImpl->timeMetrics.wallClockTime = wallTime;
    pImpl->timeMetrics.simulationTime = simTime;
    pImpl->timeMetrics.stepDuration = stepDuration;
    pImpl->timeMetrics.recordStepTime(stepDuration);
}

void PerformanceMetrics::updateMemoryMetrics(size_t used, size_t free, size_t allocatedBytes) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    if (allocatedBytes > 0) {
        pImpl->memoryMetrics.recordAllocation(allocatedBytes);
    }
    
    pImpl->memoryMetrics.updateMemoryUsage(used, free);
}

void PerformanceMetrics::updateNetworkMetrics(size_t totalNeurons, size_t activeNeurons,
                                             size_t firingNeurons, size_t totalSynapses,
                                             double avgFiring, double eiratio) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    pImpl->networkMetrics.updateStatistics(totalNeurons, activeNeurons, firingNeurons,
                                           totalSynapses, avgFiring, eiratio);
}

void PerformanceMetrics::updateIntegrationMetrics(size_t steps, double reward,
                                                  size_t episodes, double dopamine,
                                                  double curiosity) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    pImpl->integrationMetrics.updateIntegrationMetrics(steps, reward, episodes, dopamine, curiosity);
}

void PerformanceMetrics::updateHealthMetrics(double cpuUsage, double memoryUsage,
                                            double stability, bool hasLeaks,
                                            bool isStable) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    pImpl->healthMetrics.updateHealth(cpuUsage, memoryUsage, stability, hasLeaks, isStable);
}

PerformanceMetrics::PerformanceReport PerformanceMetrics::getPerformanceReport() const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    PerformanceReport report;
    report.timeMetrics = pImpl->timeMetrics;
    report.memoryMetrics = pImpl->memoryMetrics;
    report.networkMetrics = pImpl->networkMetrics;
    report.integrationMetrics = pImpl->integrationMetrics;
    report.healthMetrics = pImpl->healthMetrics;
    
    return report;
}

std::string PerformanceMetrics::exportToJSON() const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    json j;
    
    // Time metrics
    j["time_metrics"]["wall_clock_time"] = pImpl->timeMetrics.wallClockTime;
    j["time_metrics"]["simulation_time"] = pImpl->timeMetrics.simulationTime;
    j["time_metrics"]["step_duration"] = pImpl->timeMetrics.stepDuration;
    j["time_metrics"]["average_step_time"] = pImpl->timeMetrics.getAverageStepTime();
    j["time_metrics"]["max_step_time"] = pImpl->timeMetrics.getMaxStepTime();
    j["time_metrics"]["step_count"] = static_cast<int>(pImpl->timeMetrics.stepTimes.size());
    
    // Memory metrics
    j["memory_metrics"]["total_allocated"] = pImpl->memoryMetrics.totalAllocated;
    j["memory_metrics"]["used_memory"] = pImpl->memoryMetrics.usedMemory;
    j["memory_metrics"]["free_memory"] = pImpl->memoryMetrics.freeMemory;
    j["memory_metrics"]["allocation_count"] = pImpl->memoryMetrics.allocationCount;
    j["memory_metrics"]["memory_usage_percentage"] = pImpl->memoryMetrics.memoryUsagePercentage;
    
    // Network metrics
    j["network_metrics"]["total_neurons"] = pImpl->networkMetrics.totalNeurons;
    j["network_metrics"]["active_neurons"] = pImpl->networkMetrics.activeNeurons;
    j["network_metrics"]["firing_neurons"] = pImpl->networkMetrics.firingNeurons;
    j["network_metrics"]["total_synapses"] = pImpl->networkMetrics.totalSynapses;
    j["network_metrics"]["average_firing_rate"] = pImpl->networkMetrics.averageFiringRate;
    j["network_metrics"]["excitation_inhibition_ratio"] = pImpl->networkMetrics.excitationInhibitionRatio;
    j["network_metrics"]["firing_rate_std_dev"] = pImpl->networkMetrics.getFiringRateStdDev();
    
    // Integration metrics
    j["integration_metrics"]["steps_completed"] = pImpl->integrationMetrics.stepsCompleted;
    j["integration_metrics"]["total_reward"] = pImpl->integrationMetrics.totalReward;
    j["integration_metrics"]["average_reward"] = pImpl->integrationMetrics.getAverageReward();
    j["integration_metrics"]["episodes_stored"] = pImpl->integrationMetrics.episodesStored;
    j["integration_metrics"]["dopamine_level"] = pImpl->integrationMetrics.dopamineLevel;
    j["integration_metrics"]["curiosity_level"] = pImpl->integrationMetrics.curiosityLevel;
    
    // System health metrics
    j["health_metrics"]["cpu_usage"] = pImpl->healthMetrics.cpuUsage;
    j["health_metrics"]["memory_usage"] = pImpl->healthMetrics.memoryUsage;
    j["health_metrics"]["stability_score"] = pImpl->healthMetrics.stabilityScore;
    j["health_metrics"]["has_memory_leaks"] = pImpl->healthMetrics.hasMemoryLeaks;
    j["health_metrics"]["is_stable"] = pImpl->healthMetrics.isStable;
    j["health_metrics"]["health_status"] = pImpl->healthMetrics.healthStatus;
    
    return j.dump(2);
}

std::string PerformanceMetrics::exportToCSV() const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    std::ostringstream oss;
    oss << "Metric,Value,Unit\n";
    
    // Time metrics
    oss << "Wall Clock Time," << pImpl->timeMetrics.wallClockTime << " seconds\n";
    oss << "Simulation Time," << pImpl->timeMetrics.simulationTime << " seconds\n";
    oss << "Average Step Duration," << pImpl->timeMetrics.getAverageStepTime() << " seconds\n";
    oss << "Max Step Duration," << pImpl->timeMetrics.getMaxStepTime() << " seconds\n";
    
    // Memory metrics
    oss << "Total Allocations," << pImpl->memoryMetrics.allocationCount << " count\n";
    oss << "Total Allocated," << pImpl->memoryMetrics.totalAllocated << " bytes\n";
    oss << "Used Memory," << pImpl->memoryMetrics.usedMemory << " bytes\n";
    oss << "Free Memory," << pImpl->memoryMetrics.freeMemory << " bytes\n";
    oss << "Memory Usage Percentage," << pImpl->memoryMetrics.memoryUsagePercentage << " %\n";
    
    // Network metrics
    oss << "Total Neurons," << pImpl->networkMetrics.totalNeurons << " count\n";
    oss << "Active Neurons," << pImpl->networkMetrics.activeNeurons << " count\n";
    oss << "Firing Neurons," << pImpl->networkMetrics.firingNeurons << " count\n";
    oss << "Total Synapses," << pImpl->networkMetrics.totalSynapses << " count\n";
    oss << "Average Firing Rate," << pImpl->networkMetrics.averageFiringRate << " Hz\n";
    oss << "E/I Ratio," << pImpl->networkMetrics.excitationInhibitionRatio << "\n";
    oss << "Firing Rate Std Dev," << pImpl->networkMetrics.getFiringRateStdDev() << " Hz\n";
    
    // Integration metrics
    oss << "Steps Completed," << pImpl->integrationMetrics.stepsCompleted << " count\n";
    oss << "Total Reward," << pImpl->integrationMetrics.totalReward << "\n";
    oss << "Average Reward," << pImpl->integrationMetrics.getAverageReward() << "\n";
    oss << "Episodes Stored," << pImpl->integrationMetrics.episodesStored << " count\n";
    oss << "Dopamine Level," << pImpl->integrationMetrics.dopamineLevel << "\n";
    oss << "Curiosity Level," << pImpl->integrationMetrics.curiosityLevel << "\n";
    
    // System health metrics
    oss << "CPU Usage," << pImpl->healthMetrics.cpuUsage << " %\n";
    oss << "Memory Usage," << pImpl->healthMetrics.memoryUsage << " %\n";
    oss << "Stability Score," << pImpl->healthMetrics.stabilityScore << "\n";
    oss << "Has Memory Leaks," << (pImpl->healthMetrics.hasMemoryLeaks ? "true" : "false") << "\n";
    oss << "Is Stable," << (pImpl->healthMetrics.isStable ? "true" : "false") << "\n";
    oss << "Health Status," << pImpl->healthMetrics.healthStatus << "\n";
    
    return oss.str();
}

void PerformanceMetrics::reset() {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    pImpl->timeMetrics = TimeMetrics();
    pImpl->memoryMetrics = MemoryMetrics();
    pImpl->networkMetrics = NetworkMetrics();
    pImpl->integrationMetrics = IntegrationMetrics();
    pImpl->healthMetrics = SystemHealthMetrics();
}

void PerformanceMetrics::merge(const PerformanceMetrics& other) {
    std::lock_guard<std::mutex> lock1(pImpl->metricsMutex);
    std::lock_guard<std::mutex> lock2(other.pImpl->metricsMutex);
    
    // Merge time metrics
    pImpl->timeMetrics.wallClockTime = std::max(pImpl->timeMetrics.wallClockTime,
                                               other.pImpl->timeMetrics.wallClockTime);
    pImpl->timeMetrics.simulationTime = std::max(pImpl->timeMetrics.simulationTime,
                                                 other.pImpl->timeMetrics.simulationTime);
    
    for (double time : other.pImpl->timeMetrics.stepTimes) {
        pImpl->timeMetrics.stepTimes.push_back(time);
    }
    
    // Merge memory metrics
    pImpl->memoryMetrics.totalAllocated += other.pImpl->memoryMetrics.totalAllocated;
    pImpl->memoryMetrics.usedMemory += other.pImpl->memoryMetrics.usedMemory;
    pImpl->memoryMetrics.freeMemory += other.pImpl->memoryMetrics.freeMemory;
    pImpl->memoryMetrics.allocationCount += other.pImpl->memoryMetrics.allocationCount;
    pImpl->memoryMetrics.memoryUsagePercentage = (pImpl->memoryMetrics.usedMemory + pImpl->memoryMetrics.freeMemory > 0) ?
        (static_cast<double>(pImpl->memoryMetrics.usedMemory) / 
         (pImpl->memoryMetrics.usedMemory + pImpl->memoryMetrics.freeMemory) * 100.0) : 0.0;
    
    // Merge network metrics
    pImpl->networkMetrics.totalNeurons = std::max(pImpl->networkMetrics.totalNeurons,
                                                  other.pImpl->networkMetrics.totalNeurons);
    pImpl->networkMetrics.activeNeurons = std::max(pImpl->networkMetrics.activeNeurons,
                                                  other.pImpl->networkMetrics.activeNeurons);
    pImpl->networkMetrics.firingNeurons = std::max(pImpl->networkMetrics.firingNeurons,
                                                  other.pImpl->networkMetrics.firingNeurons);
    pImpl->networkMetrics.totalSynapses = std::max(pImpl->networkMetrics.totalSynapses,
                                                  other.pImpl->networkMetrics.totalSynapses);
    
    // Use average if both have values
    if (pImpl->networkMetrics.totalNeurons > 0 && other.pImpl->networkMetrics.totalNeurons > 0) {
        double avgFiring = (pImpl->networkMetrics.averageFiringRate + other.pImpl->networkMetrics.averageFiringRate) / 2.0;
        double avgEiratio = (pImpl->networkMetrics.excitationInhibitionRatio + other.pImpl->networkMetrics.excitationInhibitionRatio) / 2.0;
        pImpl->networkMetrics.averageFiringRate = avgFiring;
        pImpl->networkMetrics.excitationInhibitionRatio = avgEiratio;
    }
    
    // Merge integration metrics
    pImpl->integrationMetrics.stepsCompleted += other.pImpl->integrationMetrics.stepsCompleted;
    pImpl->integrationMetrics.totalReward += other.pImpl->integrationMetrics.totalReward;
    pImpl->integrationMetrics.episodesStored += other.pImpl->integrationMetrics.episodesStored;
    pImpl->integrationMetrics.dopamineLevel = (pImpl->integrationMetrics.dopamineLevel + other.pImpl->integrationMetrics.dopamineLevel) / 2.0;
    pImpl->integrationMetrics.curiosityLevel = (pImpl->integrationMetrics.curiosityLevel + other.pImpl->integrationMetrics.curiosityLevel) / 2.0;
    
    // Merge system health metrics
    pImpl->healthMetrics.cpuUsage = (pImpl->healthMetrics.cpuUsage + other.pImpl->healthMetrics.cpuUsage) / 2.0;
    pImpl->healthMetrics.memoryUsage = (pImpl->healthMetrics.memoryUsage + other.pImpl->healthMetrics.memoryUsage) / 2.0;
    pImpl->healthMetrics.stabilityScore = (pImpl->healthMetrics.stabilityScore + other.pImpl->healthMetrics.stabilityScore) / 2.0;
    pImpl->healthMetrics.hasMemoryLeaks = pImpl->healthMetrics.hasMemoryLeaks || other.pImpl->healthMetrics.hasMemoryLeaks;
    pImpl->healthMetrics.isStable = pImpl->healthMetrics.isStable && other.pImpl->healthMetrics.isStable;
}

} // namespace nlm
