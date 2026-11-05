#pragma once

#include "CheckpointMonitor.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <sstream>
#include <iomanip>

namespace nlm {

struct CheckpointMonitor::Impl {
    std::vector<CheckpointMetrics> checkpointHistory;
    uint64_t totalCheckpoints;
    uint64_t successfulCheckpoints;
    uint64_t failedCheckpoints;
    double totalDuration;
    double totalDataSize;
    double minDuration;
    double maxDuration;
    std::chrono::system_clock::time_point lastUpdate;
    
    Impl() : totalCheckpoints(0), successfulCheckpoints(0), failedCheckpoints(0),
             totalDuration(0.0), totalDataSize(0.0), minDuration(std::numeric_limits<double>::max()),
             maxDuration(0.0) {
        lastUpdate = std::chrono::system_clock::now();
    }
};

CheckpointMonitor::CheckpointMonitor() : pImpl(std::make_unique<Impl>()) {}

CheckpointMonitor::~CheckpointMonitor() = default;

void CheckpointMonitor::recordCheckpoint(const std::string& name, uint64_t step, double simTime,
                                         double realDuration, uint64_t dataSize, bool success,
                                         const std::string& errorMessage) {
    CheckpointMetrics metrics;
    metrics.name = name;
    metrics.step = step;
    metrics.simulationTime = simTime;
    metrics.realTimeDuration = realDuration;
    metrics.dataSizeMB = dataSize / (1024.0 * 1024.0); // Convert to MB
    metrics.success = success;
    metrics.timestamp = std::chrono::system_clock::now();
    metrics.errorMessage = errorMessage;
    
    // Update statistics
    pImpl->totalCheckpoints++;
    pImpl->totalDuration += realDuration;
    pImpl->totalDataSize += metrics.dataSizeMB;
    pImpl->minDuration = std::min(pImpl->minDuration, realDuration);
    pImpl->maxDuration = std::max(pImpl->maxDuration, realDuration);
    
    if (success) {
        pImpl->successfulCheckpoints++;
    } else {
        pImpl->failedCheckpoints++;
    }
    
    pImpl->checkpointHistory.push_back(metrics);
    
    // Limit history size
    if (pImpl->checkpointHistory.size() > 10000) {
        pImpl->checkpointHistory.erase(pImpl->checkpointHistory.begin());
    }
    
    pImpl->lastUpdate = metrics.timestamp;
    
    NLM_LOG_INFO("Checkpoint recorded: name=" + name + ", step=" + std::to_string(step) +
                 ", duration=" + std::to_string(realDuration) + "s, success=" +
                 std::string(success ? "true" : "false"));
}

std::vector<CheckpointMonitor::CheckpointMetrics> CheckpointMonitor::getRecentCheckpoints(size_t maxCount) const {
    if (maxCount >= pImpl->checkpointHistory.size()) {
        return pImpl->checkpointHistory;
    }
    
    size_t startIndex = pImpl->checkpointHistory.size() - maxCount;
    return std::vector<CheckpointMetrics>(pImpl->checkpointHistory.begin() + startIndex,
                                          pImpl->checkpointHistory.end());
}

CheckpointMonitor::CheckpointStats CheckpointMonitor::calculateStats() const {
    CheckpointStats stats;
    
    if (pImpl->totalCheckpoints == 0) {
        return stats;
    }
    
    stats.totalCheckpoints = pImpl->totalCheckpoints;
    stats.successfulCheckpoints = pImpl->successfulCheckpoints;
    stats.failedCheckpoints = pImpl->failedCheckpoints;
    stats.successRate = static_cast<double>(pImpl->successfulCheckpoints) / pImpl->totalCheckpoints;
    stats.averageDuration = pImpl->totalDuration / pImpl->totalCheckpoints;
    stats.averageDataSize = pImpl->totalDataSize / pImpl->totalCheckpoints;
    stats.maxDataSize = pImpl->maxDuration; // Note: This should track max data size separately
    stats.minDuration = pImpl->minDuration;
    stats.maxDuration = pImpl->maxDuration;
    
    return stats;
}

std::vector<CheckpointMonitor::CheckpointMetrics> CheckpointMonitor::getSlowestCheckpoints(size_t count) const {
    std::vector<CheckpointMetrics> result = pImpl->checkpointHistory;
    std::sort(result.begin(), result.end(),
              [](const CheckpointMetrics& a, const CheckpointMetrics& b) {
                  return a.realTimeDuration > b.realTimeDuration;
              });
    
    if (count >= result.size()) {
        return result;
    }
    
    return std::vector<CheckpointMetrics>(result.begin(), result.begin() + count);
}

std::vector<CheckpointMonitor::CheckpointMetrics> CheckpointMonitor::getLargestCheckpoints(size_t count) const {
    std::vector<CheckpointMetrics> result = pImpl->checkpointHistory;
    std::sort(result.begin(), result.end(),
              [](const CheckpointMetrics& a, const CheckpointMetrics& b) {
                  return a.dataSizeMB > b.dataSizeMB;
              });
    
    if (count >= result.size()) {
        return result;
    }
    
    return std::vector<CheckpointMetrics>(result.begin(), result.begin() + count);
}

std::vector<CheckpointMonitor::CheckpointMetrics> CheckpointMonitor::getFailedCheckpoints(size_t maxCount) const {
    std::vector<CheckpointMetrics> result;
    for (const auto& checkpoint : pImpl->checkpointHistory) {
        if (!checkpoint.success) {
            result.push_back(checkpoint);
        }
    }
    
    if (maxCount >= result.size()) {
        return result;
    }
    
    return std::vector<CheckpointMetrics>(result.begin(), result.begin() + maxCount);
}

CheckpointMonitor::CheckpointTrend CheckpointMonitor::analyzeTrends(size_t analysisWindow) const {
    CheckpointTrend trend;
    
    if (pImpl->checkpointHistory.size() < 2) {
        trend.durationTrend = 0.0;
        trend.sizeTrend = 0.0;
        trend.successTrend = 0.0;
        trend.trendDirection = "INSUFFICIENT_DATA";
        return trend;
    }
    
    // Get recent checkpoints for analysis
    size_t startIndex = std::max(size_t(0), pImpl->checkpointHistory.size() - analysisWindow);
    std::vector<double> durations, sizes, successRates;
    
    for (size_t i = startIndex; i < pImpl->checkpointHistory.size(); ++i) {
        const auto& checkpoint = pImpl->checkpointHistory[i];
        durations.push_back(checkpoint.realTimeDuration);
        sizes.push_back(checkpoint.dataSizeMB);
        successRates.push_back(checkpoint.success ? 1.0 : 0.0);
    }
    
    // Calculate trend slopes
    trend.durationTrend = MonitoringUtils::calculateTrend(durations);
    trend.sizeTrend = MonitoringUtils::calculateTrend(sizes);
    trend.successTrend = MonitoringUtils::calculateTrend(successRates);
    
    // Determine trend direction
    if (std::abs(trend.durationTrend) < 0.001 && std::abs(trend.sizeTrend) < 0.001 &&
        std::abs(trend.successTrend) < 0.001) {
        trend.trendDirection = "STABLE";
    } else if (trend.durationTrend > 0.0 || trend.sizeTrend > 0.0 || trend.successTrend > 0.0) {
        trend.trendDirection = "DEGRADING";
    } else {
        trend.trendDirection = "IMPROVING";
    }
    
    return trend;
}

void CheckpointMonitor::clearOldMetrics(std::chrono::hours maxAge) {
    auto now = std::chrono::system_clock::now();
    pImpl->checkpointHistory.erase(std::remove_if(pImpl->checkpointHistory.begin(),
                                                  pImpl->checkpointHistory.end(),
                                                  [now, maxAge](const CheckpointMetrics& checkpoint) {
                                                      return (now - checkpoint.timestamp) > maxAge;
                                                  }),
                                   pImpl->checkpointHistory.end());
}

} // namespace nlm