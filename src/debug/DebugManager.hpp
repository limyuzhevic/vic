// src/debug/DebugManager.hpp
#pragma once

#include <memory>
#include <string>
#include <map>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <mutex>

namespace nlm {
class Config;
class Brain;

namespace debug {

// Debug levels
enum class DebugLevel {
    None = 0,
    Basic = 1,
    Detailed = 2,
    Verbose = 3
};

// Performance measurement
struct PerformanceMetrics {
    std::string operation_name;
    double execution_time_ms;
    uint64_t memory_usage_bytes;
    uint64_t peak_memory_bytes;
    int cpu_cycles;
    std::chrono::high_resolution_clock::time_point start_time;
    
    PerformanceMetrics(const std::string& name) : operation_name(name) {
        start_time = std::chrono::high_resolution_clock::now();
    }
    
    void stop() {
        auto end_time = std::chrono::high_resolution_clock::now();
        execution_time_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
    }
};

// Spike type enum
enum class SpikeType {
    Regular,
    Burst,
    Adaptive,
    Modulated
};

// Memory tracker
class MemoryTracker {
public:
    MemoryTracker();
    ~MemoryTracker();
    
    void trackAllocation(void* ptr, size_t size, const std::string& allocation_site = "");
    void trackDeallocation(void* ptr);
    size_t getTotalAllocated() const;
    size_t getCurrentAllocated() const;
    void printMemoryReport() const;
    
private:
    struct AllocationInfo {
        void* ptr;
        size_t size;
        std::string allocation_site;
        uint64_t timestamp;
    };
    
    std::unordered_map<void*, AllocationInfo> allocations_;
    size_t total_allocated_;
    mutable std::mutex mutex_;
};

// Spike profiler
class SpikeProfiler {
public:
    SpikeProfiler();
    ~SpikeProfiler();
    
    void recordSpike(NeuronId neuronId, Timestamp time, SimulationStep step, SpikeType type = SpikeType::Regular);
    void recordPreSpike(SynapseId synapseId, NeuronId neuronId, Timestamp time);
    void recordPostSpike(SynapseId synapseId, NeuronId neuronId, Timestamp time);
    
    void generateSTDPReport(const std::string& output_file) const;
    void generateSpikeActivityReport(const std::string& output_file) const;
    void generatePerformanceReport(const std::string& output_file) const;
    
    void startProfiling();
    void stopProfiling();
    bool isProfiling() const { return is_profiling_; }
    
private:
    struct SpikeEvent {
        NeuronId neuron_id;
        Timestamp time;
        SimulationStep step;
        SpikeType type;
    };
    
    struct PrePostPair {
        SynapseId synapse_id;
        NeuronId pre_neuron_id;
        NeuronId post_neuron_id;
        Timestamp pre_time;
        Timestamp post_time;
        double time_difference;
    };
    
    std::vector<SpikeEvent> spike_events_;
    std::vector<PrePostPair> pre_post_pairs_;
    bool is_profiling_;
    std::chrono::high_resolution_clock::time_point session_start_;
    std::string session_id_;
};

// Configuration profiler
class ConfigurationProfiler {
public:
    ConfigurationProfiler(std::shared_ptr<Config> config);
    ~ConfigurationProfiler();
    
    void profilePerformance(std::shared_ptr<Brain> brain, const std::string& description);
    void profileMemoryUsage(std::shared_ptr<Brain> brain);
    void profileNeuralDynamics(std::shared_ptr<Brain> brain);
    void generateConfigurationReport(const std::string& output_file) const;
    
    struct BrainPerformanceMetrics {
        std::string configuration_id;
        double execution_time_ms;
        uint64_t total_spikes;
        uint64_t total_neurons;
        uint64_t total_synapses;
        double average_firing_rate;
        double weight_change_magnitude;
        uint64_t memory_footprint_bytes;
    };
    
    std::vector<BrainPerformanceMetrics> getPerformanceHistory() const { return performance_history_; }
    
private:
    std::shared_ptr<Config> config_;
    std::vector<BrainPerformanceMetrics> performance_history_;
    std::string session_id_;
};

// Debug interface
class DebugManager {
public:
    DebugManager();
    ~DebugManager();
    
    static DebugManager& getInstance() {
        static DebugManager instance;
        return instance;
    }
    
    void initialize(DebugLevel level = DebugLevel::Detailed);
    void shutdown();
    
    void setDebugLevel(DebugLevel level) { debug_level_ = level; }
    DebugLevel getDebugLevel() const { return debug_level_; }
    
    // Performance tracking
    std::shared_ptr<PerformanceMetrics> startPerformanceTracking(const std::string& operation_name);
    void stopPerformanceTracking(std::shared_ptr<PerformanceMetrics> metrics);
    
    // Memory tracking
    std::shared_ptr<MemoryTracker> getMemoryTracker() { return memory_tracker_; }
    
    // Spike analysis
    std::shared_ptr<SpikeProfiler> getSpikeProfiler() { return spike_profiler_; }
    
    // Configuration analysis
    std::shared_ptr<ConfigurationProfiler> getConfigurationProfiler(std::shared_ptr<Config> config) {
        if (!config_profiler_) {
            config_profiler_ = std::make_shared<ConfigurationProfiler>(config);
        }
        return config_profiler_;
    }
    
    // Debug output
    void logDebug(const std::string& message, DebugLevel level = DebugLevel::Detailed);
    void logPerformance(const std::string& category, const std::string& operation, double time_ms);
    void logMemory(const std::string& category, size_t allocated_bytes);
    
    // Report generation
    void generateDebugReport(const std::string& output_dir = "debug_reports");
    void generateComprehensiveReport(const std::string& output_dir = "debug_reports");
    
    // Utility functions
    void collectBrainSnapshot(std::shared_ptr<Brain> brain, const std::string& snapshot_name);
    void collectSystemSnapshot();
    
private:
    DebugLevel debug_level_;
    std::shared_ptr<MemoryTracker> memory_tracker_;
    std::shared_ptr<SpikeProfiler> spike_profiler_;
    std::shared_ptr<ConfigurationProfiler> config_profiler_;
    
    std::vector<std::shared_ptr<PerformanceMetrics>> performance_metrics_;
    std::map<std::string, std::vector<double>> performance_history_;
    
    bool initialized_;
    std::string session_id_;
    std::chrono::high_resolution_clock::time_point session_start_;
};

} // namespace debug
} // namespace nlm

