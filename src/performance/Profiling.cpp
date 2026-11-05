// NLM Performance Profiling
// Phase 6: Basic Performance Monitoring and Profiling Tools

#include "performance/Profiling.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <algorithm>
#include <numeric>

namespace nlm {

Profiler::Profiler() {
    NLM_LOG_INFO("Performance Profiler initialized");
}

Profiler::~Profiler() {
    NLM_LOG_INFO("Performance Profiler destroyed");
}

void Profiler::startSection(const std::string& name) {
    Section section;
    section.name = name;
    section.startTime = std::chrono::high_resolution_clock::now();
    sections.push_back(section);
}

void Profiler::endSection() {
    if (sections.empty()) {
        return;
    }
    
    auto& section = sections.back();
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(endTime - section.startTime).count() * 1000.0; // milliseconds
    
    section.endTime = endTime;
    section.duration = duration;
}

void Profiler::printReport() const {
    std::cout << "=== Performance Profile Report ===" << std::endl;
    std::cout << std::endl;
    
    if (sections.empty()) {
        std::cout << "No profiling data available." << std::endl;
        return;
    }
    
    double totalTime = 0.0;
    for (const auto& section : sections) {
        totalTime += section.duration;
    }
    
    std::cout << "Total Time: " << std::fixed << std::setprecision(2) << totalTime << " ms" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Section Breakdown:" << std::endl;
    for (const auto& section : sections) {
        double percentage = (section.duration / totalTime) * 100.0;
        std::cout << "  " << std::left << std::setw(30) << section.name 
                  << std::fixed << std::setprecision(3) << section.duration << " ms ("
                  << std::fixed << std::setprecision(1) << percentage << "%)" << std::endl;
    }
    
    std::cout << std::endl;
}

void Profiler::reset() {
    sections.clear();
}

PerformanceMonitor::PerformanceMonitor() {
    NLM_LOG_INFO("Performance Monitor initialized");
}

PerformanceMonitor::~PerformanceMonitor() {
    NLM_LOG_INFO("Performance Monitor destroyed");
}

void PerformanceMonitor::recordMetrics(const Brain& brain, double wallClockTime) {
    std::lock_guard<std::mutex> lock(mutex);
    
    PerformanceMetrics metrics;
    metrics.wallClockTime = wallClockTime;
    metrics.simStepTime = wallClockTime / brain.getTotalSpikeCount();
    metrics.avgFiringRate = brain.getAverageFiringRate();
    metrics.totalSpikes = brain.getTotalSpikeCount();
    metrics.activeNeurons = brain.getActiveNeuronCount();
    metrics.totalNeurons = brain.getTotalNeuronCount();
    metrics.totalSynapses = brain.getTotalSynapseCount();
    
    if (brain.getWorkingMemory()) {
        metrics.workingMemoryActive = brain.getWorkingMemory()->getActiveTraces();
        metrics.workingMemoryCapacity = brain.getWorkingMemory()->getCapacity();
    }
    
    if (brain.getEpisodicMemory()) {
        metrics.episodicMemoryEpisodes = brain.getEpisodicMemory()->getEpisodeCount();
        metrics.episodicMemoryCapacity = brain.getEpisodicMemory()->getMaxEpisodes();
    }
    
    metrics.timestamp = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    metricsHistory.push_back(metrics);
    
    // Keep only last 1000 metrics
    if (metricsHistory.size() > 1000) {
        metricsHistory.erase(metricsHistory.begin());
    }
}

void PerformanceMonitor::printSummary() const {
    std::cout << "=== Performance Summary ===" << std::endl;
    std::cout << std::endl;
    
    if (metricsHistory.empty()) {
        std::cout << "No performance metrics available." << std::endl;
        return;
    }
    
    const auto& latest = metricsHistory.back();
    
    std::cout << "Latest Metrics:" << std::endl;
    std::cout << "  Wall Clock Time: " << std::fixed << std::setprecision(3) << latest.wallClockTime << " seconds" << std::endl;
    std::cout << "  Average Firing Rate: " << std::fixed << std::setprecision(3) << latest.avgFiringRate << " Hz" << std::endl;
    std::cout << "  Total Spike Events: " << latest.totalSpikes << std::endl;
    std::cout << "  Active Neurons: " << latest.activeNeurons << " out of " << latest.totalNeurons << std::endl;
    std::cout << "  Total Synapses: " << latest.totalSynapses << std::endl;
    std::cout << "  Steps per Second: " << std::fixed << std::setprecision(1) << latest.totalSpikes / latest.wallClockTime << std::endl;
    std::cout << std::endl;
    
    // Calculate averages over all recorded metrics
    double totalWallClock = 0.0;
    double totalFiringRate = 0.0;
    double totalSpikes = 0.0;
    
    for (const auto& metrics : metricsHistory) {
        totalWallClock += metrics.wallClockTime;
        totalFiringRate += metrics.avgFiringRate;
        totalSpikes += metrics.totalSpikes;
    }
    
    size_t count = metricsHistory.size();
    
    std::cout << "Historical Averages (last " << count << " samples):" << std::endl;
    std::cout << "  Average Wall Clock Time: " << std::fixed << std::setprecision(3) << totalWallClock / count << " seconds" << std::endl;
    std::cout << "  Average Firing Rate: " << std::fixed << std::setprecision(3) << totalFiringRate / count << " Hz" << std::endl;
    std::cout << "  Average Spike Events: " << std::fixed << std::setprecision(0) << totalSpikes / count << std::endl;
    std::cout << "  Memory Efficiency: " << std::fixed << std::setprecision(2) << 
        (static_cast<double>(latest.activeNeurons) / latest.totalNeurons * 100.0) << "% active" << std::endl;
    std::cout << std::endl;
}

void PerformanceMonitor::exportMetrics(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open metrics file for export: " + filename);
        return;
    }
    
    file << "# NLM Performance Metrics Export" << std::endl;
    file << "# Generated at: " << getCurrentTimestamp() << std::endl;
    file << std::endl;
    
    file << "wall_clock_time,avg_firing_rate,total_spikes,active_neurons,total_neurons,total_synapses" << std::endl;
    
    for (const auto& metrics : metricsHistory) {
        file << std::fixed << std::setprecision(6) << metrics.wallClockTime << ","
             << std::fixed << std::setprecision(6) << metrics.avgFiringRate << ","
             << metrics.totalSpikes << ","
             << metrics.activeNeurons << ","
             << metrics.totalNeurons << ","
             << metrics.totalSynapses << std::endl;
    }
    
    file.close();
    NLM_LOG_INFO("Performance metrics exported to: " + filename);
}

std::string PerformanceMonitor::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;
    std::tm* ptm = std::localtime(&time);
    if (ptm) {
        char buf[100];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ptm);
        return std::string(buf);
    }
    return "unknown";
}

} // namespace nlm
