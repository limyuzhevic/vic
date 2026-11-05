// Performance profiling and monitoring for NLM
// Phase 6: Development and optimization tools

#pragma once

#include <vector>
#include <string>
#include <chrono>
#include <mutex>

namespace nlm {

// Profiler section for timing code blocks
struct ProfilerSection {
    std::string name;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    double duration; // in milliseconds
};

class Profiler {
public:
    Profiler();
    ~Profiler();
    
    // Start timing a code section
    void startSection(const std::string& name);
    
    // End timing the current section
    void endSection();
    
    // Print profiling report
    void printReport() const;
    
    // Reset all data
    void reset();
    
private:
    std::vector<ProfilerSection> sections;
};

// Performance metrics for brain simulation
struct PerformanceMetrics {
    double wallClockTime;        // Total wall clock time in seconds
    double simStepTime;          // Average time per simulation step
    float avgFiringRate;         // Average firing rate in Hz
    size_t totalSpikes;          // Total number of spike events
    size_t activeNeurons;        // Number of active neurons
    size_t totalNeurons;         // Total number of neurons
    size_t totalSynapses;        // Total number of synapses
    size_t workingMemoryActive;  // Active working memory traces
    size_t workingMemoryCapacity; // Working memory capacity
    size_t episodicMemoryEpisodes; // Number of stored episodes
    size_t episodicMemoryCapacity; // Maximum episodes
    double timestamp;            // Unix timestamp when recorded
};

class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Record performance metrics from brain simulation
    void recordMetrics(const class Brain& brain, double wallClockTime);
    
    // Print performance summary
    void printSummary() const;
    
    // Export metrics to file
    void exportMetrics(const std::string& filename) const;
    
private:
    mutable std::mutex mutex;
    std::vector<PerformanceMetrics> metricsHistory;
    
    std::string getCurrentTimestamp() const;
};

} // namespace nlm
