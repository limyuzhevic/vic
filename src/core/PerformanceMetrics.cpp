#pragma once

#include "PerformanceMetrics.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <nlohmann/json.hpp>

namespace nlm {

struct PerformanceMetrics::Impl {
    MetricsSnapshot latestSnapshot;
    std::vector<MetricsSnapshot> history;
    uint64_t totalSpikes;
    uint64_t maxSpikesPerStep;
    double totalMemoryUsage;
    double totalCPUUsage;
    uint64_t checkpointCount;
    double totalCheckpointDuration;
    uint64_t successfulCheckpoints;
    std::chrono::system_clock::time_point lastUpdate;
    
    Impl() : totalSpikes(0), maxSpikesPerStep(0), 
             totalMemoryUsage(0.0), totalCPUUsage(0.0),
             checkpointCount(0), totalCheckpointDuration(0.0),
             successfulCheckpoints(0) {}
};

PerformanceMetrics::PerformanceMetrics() : pImpl(std::make_unique<Impl>()) {}

PerformanceMetrics::~PerformanceMetrics() = default;

void PerformanceMetrics::updateMetrics(uint64_t step, double simTime,
                                       const std::vector<Neuron*>& neurons,
                                       const std::vector<Synapse*>& synapses,
                                       uint64_t spikeCount, uint64_t spikeCountTotal) {
    MetricsSnapshot snapshot;
    snapshot.step = step;
    snapshot.simulationTime = simTime;
    snapshot.wallClockTime = std::chrono::duration<double>(std::chrono::system_clock::now() - pImpl->lastUpdate).count();
    snapshot.neuronCount = neurons.size();
    snapshot.synapseCount = synapses.size();
    snapshot.spikeCount = spikeCount;
    snapshot.spikeCountTotal = spikeCountTotal;
    
    // Calculate average firing rate
    if (!neurons.empty()) {
        float totalFiringRate = 0.0f;
        for (const auto* neuron : neurons) {
            totalFiringRate += neuron->getFiringRate();
        }
        snapshot.averageFiringRate = totalFiringRate / static_cast<float>(neurons.size());
    } else {
        snapshot.averageFiringRate = 0.0f;
    }
    
    // Memory usage would be obtained from system monitoring
    snapshot.memoryUsageMB = MonitoringUtils::getCurrentMemoryUsage();
    
    // CPU usage would be obtained from system monitoring
    snapshot.cpuUsagePercent = MonitoringUtils::getCurrentCPUUsage();
    
    snapshot.checkpointCount = pImpl->checkpointCount;
    snapshot.lastCheckpointDuration = pImpl->totalCheckpointDuration / 
                                     (pImpl->successfulCheckpoints > 0 ? pImpl->successfulCheckpoints : 1);
    
    snapshot.timestamp = std::chrono::system_clock::now();
    
    // Update internal statistics
    pImpl->totalSpikes = spikeCountTotal;
    pImpl->maxSpikesPerStep = std::max(pImpl->maxSpikesPerStep, spikeCount);
    pImpl->totalMemoryUsage += snapshot.memoryUsageMB;
    pImpl->totalCPUUsage += snapshot.cpuUsagePercent;
    
    // Add to history
    pImpl->history.push_back(snapshot);
    if (pImpl->history.size() > Constants::MAX_HISTORICAL_ENTRIES) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    pImpl->lastUpdate = snapshot.timestamp;
    NLM_LOG_DEBUG("Performance metrics updated: step=" + std::to_string(step) + 
                 ", spikes=" + std::to_string(spikeCount) + 
                 ", firing_rate=" + std::to_string(snapshot.averageFiringRate));
}

void PerformanceMetrics::updateMemoryUsage(double memoryMB) {
    // Update latest snapshot
    if (!pImpl->history.empty()) {
        pImpl->history.back().memoryUsageMB = memoryMB;
    }
}

void PerformanceMetrics::updateCPUUsage(double cpuPercent) {
    // Update latest snapshot
    if (!pImpl->history.empty()) {
        pImpl->history.back().cpuUsagePercent = cpuPercent;
    }
}

void PerformanceMetrics::recordCheckpoint(bool success, double duration) {
    pImpl->checkpointCount++;
    pImpl->totalCheckpointDuration += duration;
    
    if (success) {
        pImpl->successfulCheckpoints++;
    }
    
    NLM_LOG_INFO("Checkpoint recorded: success=" + std::string(success ? "true" : "false") +
                 ", duration=" + std::to_string(duration) + "s");
}

PerformanceMetrics::MetricsSnapshot PerformanceMetrics::getLatestSnapshot() const {
    if (pImpl->history.empty()) {
        return MetricsSnapshot();
    }
    return pImpl->history.back();
}

std::vector<PerformanceMetrics::MetricsSnapshot> PerformanceMetrics::getMetricsHistory(size_t maxSamples) const {
    if (maxSamples >= pImpl->history.size()) {
        return pImpl->history;
    }
    
    size_t startIndex = pImpl->history.size() - maxSamples;
    return std::vector<MetricsSnapshot>(pImpl->history.begin() + startIndex, pImpl->history.end());
}

PerformanceMetrics::PerformanceStats PerformanceMetrics::calculatePerformanceStats() const {
    PerformanceStats stats;
    
    if (pImpl->history.empty()) {
        return stats;
    }
    
    size_t validSamples = 0;
    double totalSpikeRate = 0.0;
    double totalMemoryUsage = 0.0;
    double totalCPUUsage = 0.0;
    double successfulCheckpointRate = 0.0;
    double totalCheckpointDurations = 0.0;
    size_t checkpointSamples = 0;
    
    for (const auto& snapshot : pImpl->history) {
        validSamples++;
        
        // Spike rate (spikes per second)
        if (snapshot.simulationTime > 0.0) {
            totalSpikeRate += static_cast<double>(snapshot.spikeCount) / snapshot.simulationTime;
        }
        
        // Memory usage
        totalMemoryUsage += snapshot.memoryUsageMB;
        
        // CPU usage
        totalCPUUsage += snapshot.cpuUsagePercent;
        
        // Checkpoint stats
        if (snapshot.checkpointCount > 0) {
            // Estimate success rate based on total checkpoints
            successfulCheckpointRate = static_cast<double>(pImpl->successfulCheckpoints) / 
                                       pImpl->checkpointCount;
            totalCheckpointDurations += snapshot.lastCheckpointDuration;
            checkpointSamples++;
        }
    }
    
    if (validSamples > 0) {
        stats.averageSpikeRate = totalSpikeRate / static_cast<double>(validSamples);
        stats.averageMemoryUsage = totalMemoryUsage / static_cast<double>(validSamples);
        stats.averageCPUUsage = totalCPUUsage / static_cast<double>(validSamples);
        stats.checkpointSuccessRate = successfulCheckpointRate;
        stats.avgCheckpointDuration = checkpointSamples > 0 ? totalCheckpointDurations / static_cast<double>(checkpointSamples) : 0.0;
    }
    
    stats.maxSpikesPerStep = pImpl->maxSpikesPerStep;
    stats.totalSpikes = pImpl->totalSpikes;
    
    return stats;
}

void PerformanceMetrics::reset() {
    pImpl->history.clear();
    pImpl->totalSpikes = 0;
    pImpl->maxSpikesPerStep = 0;
    pImpl->totalMemoryUsage = 0.0;
    pImpl->totalCPUUsage = 0.0;
    pImpl->checkpointCount = 0;
    pImpl->totalCheckpointDuration = 0.0;
    pImpl->successfulCheckpoints = 0;
    NLM_LOG_INFO("Performance metrics reset");
}

} // namespace nlm