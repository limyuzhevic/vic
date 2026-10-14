#include "DebugAdvanced.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <map>
#include <algorithm>

namespace nlm {

struct DebugProfiler::Impl {
    struct ProfileData {
        std::chrono::high_resolution_clock::time_point start_time;
        std::chrono::duration<double> duration;
        double memory_usage;
        int cpu_cycles;
        std::chrono::system_clock::time_point timestamp;
    };
    
    std::unordered_map<std::string, ProfileData> profiles;
    std::vector<PerformanceMetrics> benchmark_results;
    std::shared_ptr<IAdvancedLogger> advanced_logger;
};

DebugProfiler::DebugProfiler() : pImpl(std::make_unique<Impl>()) {}
DebugProfiler::~DebugProfiler() = default;

void DebugProfiler::startProfile(const std::string& name) {
    auto& data = pImpl->profiles[name];
    data.start_time = std::chrono::high_resolution_clock::now();
    data.timestamp = std::chrono::system_clock::now();
    data.memory_usage = 0.0;
    data.cpu_cycles = 0;
}

void DebugProfiler::stopProfile(const std::string& name) {
    auto& data = pImpl->profiles[name];
    auto end_time = std::chrono::high_resolution_clock::now();
    data.duration = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - data.start_time);
}

PerformanceMetrics DebugProfiler::getLastProfile(const std::string& name) const {
    PerformanceMetrics metrics;
    auto it = pImpl->profiles.find(name);
    if (it != pImpl->profiles.end()) {
        metrics.name = name;
        metrics.duration = it->second.duration;
        metrics.memory_usage = it->second.memory_usage;
        metrics.cpu_cycles = it->second.cpu_cycles;
        metrics.timestamp = it->second.timestamp;
    }
    return metrics;
}

void DebugProfiler::runBenchmark(const std::string& name, std::function<void()> func) {
    startProfile(name);
    func();
    stopProfile(name);
    
    PerformanceMetrics metrics;
    metrics.name = name;
    metrics.duration = pImpl->profiles[name].duration;
    metrics.memory_usage = pImpl->profiles[name].memory_usage;
    metrics.cpu_cycles = pImpl->profiles[name].cpu_cycles;
    metrics.timestamp = pImpl->profiles[name].timestamp;
    
    pImpl->benchmark_results.push_back(metrics);
    
    // Log if advanced logger is available
    if (pImpl->advanced_logger) {
        StructuredLogEntry entry;
        entry.category = "performance";
        entry.level = DebugLevel::Info;
        entry.message = "Benchmark completed: " + name + " took " + 
                       std::to_string(metrics.duration.count()) + " seconds";
        entry.timestamp = std::chrono::system_clock::now();
        pImpl->advanced_logger->logStructured(entry);
    }
}

std::vector<PerformanceMetrics> DebugProfiler::getBenchmarkResults() const {
    return pImpl->benchmark_results;
}

void DebugProfiler::setAdvancedLogger(std::shared_ptr<IAdvancedLogger> logger) {
    pImpl->advanced_logger = logger;
}

struct MemoryTracker::Impl {
    MemoryInfo memory_info;
    std::mutex mutex;
    double leak_threshold_mb;
    std::shared_ptr<IAdvancedLogger> advanced_logger;
};

MemoryTracker::MemoryTracker() : pImpl(std::make_unique<Impl>()) {
    pImpl->memory_info.total_allocated = 0;
    pImpl->memory_info.current_usage = 0;
    pImpl->memory_info.peak_usage = 0;
    pImpl->memory_info.leak_threshold_mb = 10.0; // Default 10MB threshold
}

MemoryTracker::~MemoryTracker() = default;

void MemoryTracker::trackAllocation(void* ptr, size_t size) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->memory_info.allocations[ptr] = size;
    pImpl->memory_info.total_allocated += size;
    pImpl->memory_info.current_usage += size;
    
    if (pImpl->memory_info.current_usage > pImpl->memory_info.peak_usage) {
        pImpl->memory_info.peak_usage = pImpl->memory_info.current_usage;
    }
}

void MemoryTracker::trackDeallocation(void* ptr) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->memory_info.allocations.find(ptr);
    if (it != pImpl->memory_info.allocations.end()) {
        pImpl->memory_info.current_usage -= it->second;
        pImpl->memory_info.allocations.erase(it);
    }
}

MemoryInfo MemoryTracker::getMemoryInfo() const {
    return pImpl->memory_info;
}

std::vector<void*> MemoryTracker::detectLeaks() const {
    std::vector<void*> leaks;
    for (const auto& pair : pImpl->memory_info.allocations) {
        leaks.push_back(pair.first);
    }
    return leaks;
}

void MemoryTracker::setLeakThreshold(double threshold_mb) {
    pImpl->memory_info.leak_threshold_mb = threshold_mb;
}

void MemoryTracker::setAdvancedLogger(std::shared_ptr<IAdvancedLogger> logger) {
    pImpl->advanced_logger = logger;
}

struct NeuralStateSnapshotter::Impl {
    std::vector<NeuralStateSnapshot> snapshots;
    std::string current_correlation_id;
    std::shared_ptr<IAdvancedLogger> advanced_logger;
};

NeuralStateSnapshotter::NeuralStateSnapshotter() : pImpl(std::make_unique<Impl>()) {}
NeuralStateSnapshotter::~NeuralStateSnapshotter() = default;

void NeuralStateSnapshotter::takeSnapshot(const std::string& region_name) {
    NeuralStateSnapshot snapshot;
    snapshot.region_name = region_name;
    snapshot.timestep = snapshots.size();
    snapshot.timestamp = std::chrono::system_clock::now();
    snapshot.correlation_id = pImpl->current_correlation_id;
    
    // Simulate neural state data (in real implementation, this would capture actual brain state)
    snapshot.neuron_states = std::vector<double>(100, 0.0); // 100 neurons
    snapshot.synapse_weights = std::vector<std::vector<double>>(100, std::vector<double>(100, 0.0));
    
    pImpl->snapshots.push_back(snapshot);
    
    // Log if advanced logger is available
    if (pImpl->advanced_logger) {
        StructuredLogEntry entry;
        entry.category = "neural_state";
        entry.level = DebugLevel::Info;
        entry.message = "Neural state snapshot taken for region: " + region_name;
        entry.timestamp = snapshot.timestamp;
        entry.neural_state_data = snapshot;
        pImpl->advanced_logger->logStructured(entry);
    }
}

NeuralStateSnapshot NeuralStateSnapshotter::getLastSnapshot(const std::string& region_name) const {
    NeuralStateSnapshot result;
    for (const auto& snapshot : pImpl->snapshots) {
        if (snapshot.region_name == region_name) {
            result = snapshot;
        }
    }
    return result;
}

double NeuralStateSnapshotter::compareSnapshots(const std::string& region_name,
                                              const NeuralStateSnapshot& snapshot1,
                                              const NeuralStateSnapshot& snapshot2) const {
    if (snapshot1.neuron_states.size() != snapshot2.neuron_states.size()) {
        return 1.0; // Maximum difference
    }
    
    double total_diff = 0.0;
    for (size_t i = 0; i < snapshot1.neuron_states.size(); ++i) {
        total_diff += std::abs(snapshot1.neuron_states[i] - snapshot2.neuron_states[i]);
    }
    
    return total_diff / snapshot1.neuron_states.size(); // Normalized difference
}

void NeuralStateSnapshotter::setCorrelationID(const std::string& id) {
    pImpl->current_correlation_id = id;
}

std::string NeuralStateSnapshotter::getCurrentCorrelationID() const {
    return pImpl->current_correlation_id;
}

void NeuralStateSnapshotter::setAdvancedLogger(std::shared_ptr<IAdvancedLogger> logger) {
    pImpl->advanced_logger = logger;
}

struct LogAnalyzer::Impl {
    std::vector<StructuredLogEntry> log_entries;
    std::shared_ptr<IAdvancedLogger> advanced_logger;
};

LogAnalyzer::LogAnalyzer() : pImpl(std::make_unique<Impl>()) {}
LogAnalyzer::~LogAnalyzer() = default;

void LogAnalyzer::addLogEntry(const StructuredLogEntry& entry) {
    pImpl->log_entries.push_back(entry);
    
    // Log if advanced logger is available
    if (pImpl->advanced_logger) {
        pImpl->advanced_logger->logStructured(entry);
    }
}

std::vector<StructuredLogEntry> LogAnalyzer::analyzePatterns(const std::string& pattern) const {
    std::vector<StructuredLogEntry> results;
    for (const auto& entry : pImpl->log_entries) {
        if (entry.message.find(pattern) != std::string::npos) {
            results.push_back(entry);
        }
    }
    return results;
}

std::unordered_map<std::string, int> LogAnalyzer::analyzeFrequency(const std::string& category) const {
    std::unordered_map<std::string, int> frequency;
    for (const auto& entry : pImpl->log_entries) {
        if (entry.category == category) {
            frequency[entry.message]++;
        }
    }
    return frequency;
}

void LogAnalyzer::exportToJSON(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return;
    }
    
    file << "[";
    for (size_t i = 0; i < pImpl->log_entries.size(); ++i) {
        const auto& entry = pImpl->log_entries[i];
        file << "{\"timestamp\": \"" << std::chrono::system_clock::to_time_t(entry.timestamp) << "\", \"category\": \"" 
             << entry.category << "\", \"level\": \"" << static_cast<int>(entry.level) 
             << "\", \"message\": \"" << entry.message << "\"}";
        if (i < pImpl->log_entries.size() - 1) {
            file << ",";
        }
    }
    file << "]";
    file.close();
}

void LogAnalyzer::exportToCSV(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return;
    }
    
    file << "timestamp,category,level,message" << std::endl;
    for (const auto& entry : pImpl->log_entries) {
        file << std::chrono::system_clock::to_time_t(entry.timestamp) << "," 
             << entry.category << "," << static_cast<int>(entry.level) << "," 
             << entry.message << std::endl;
    }
    file.close();
}

 size_t LogAnalyzer::getTotalLogCount() const {
    return pImpl->log_entries.size();
}

double LogAnalyzer::getErrorRate() const {
    if (pImpl->log_entries.empty()) {
        return 0.0;
    }
    
    int error_count = 0;
    for (const auto& entry : pImpl->log_entries) {
        if (entry.level >= DebugLevel::Error) {
            error_count++;
        }
    }
    
    return static_cast<double>(error_count) / pImpl->log_entries.size();
}

std::chrono::duration<double> LogAnalyzer::getLogFrequency(const std::string& category) const {
    if (pImpl->log_entries.empty()) {
        return std::chrono::duration<double>(0);
    }
    
    auto first_time = pImpl->log_entries.front().timestamp;
    auto last_time = pImpl->log_entries.back().timestamp;
    auto duration = last_time - first_time;
    
    int category_count = 0;
    for (const auto& entry : pImpl->log_entries) {
        if (entry.category == category) {
            category_count++;
        }
    }
    
    if (category_count == 0) {
        return std::chrono::duration<double>(0);
    }
    
    return duration / category_count;
}

void LogAnalyzer::setAdvancedLogger(std::shared_ptr<IAdvancedLogger> logger) {
    pImpl->advanced_logger = logger;
}

namespace Debug {
    std::shared_ptr<DebugProfiler> profiler_instance;
    std::shared_ptr<MemoryTracker> memory_tracker_instance;
    std::shared_ptr<NeuralStateSnapshotter> snapshotter_instance;
    std::shared_ptr<LogAnalyzer> log_analyzer_instance;
    std::shared_ptr<IAdvancedLogger> global_advanced_logger;
    std::string global_correlation_id;
    
    std::shared_ptr<DebugProfiler> getProfiler() {
        if (!profiler_instance) {
            profiler_instance = std::make_shared<DebugProfiler>();
        }
        return profiler_instance;
    }
    
    std::shared_ptr<MemoryTracker> getMemoryTracker() {
        if (!memory_tracker_instance) {
            memory_tracker_instance = std::make_shared<MemoryTracker>();
        }
        return memory_tracker_instance;
    }
    
    std::shared_ptr<NeuralStateSnapshotter> getSnapshotter() {
        if (!snapshotter_instance) {
            snapshotter_instance = std::make_shared<NeuralStateSnapshotter>();
        }
        return snapshotter_instance;
    }
    
    std::shared_ptr<LogAnalyzer> getLogAnalyzer() {
        if (!log_analyzer_instance) {
            log_analyzer_instance = std::make_shared<LogAnalyzer>();
        }
        return log_analyzer_instance;
    }
    
    void setAdvancedLogger(std::shared_ptr<IAdvancedLogger> logger) {
        global_advanced_logger = logger;
        if (profiler_instance) profiler_instance->setAdvancedLogger(logger);
        if (memory_tracker_instance) memory_tracker_instance->setAdvancedLogger(logger);
        if (snapshotter_instance) snapshotter_instance->setAdvancedLogger(logger);
        if (log_analyzer_instance) log_analyzer_instance->setAdvancedLogger(logger);
    }
    
    void setGlobalCorrelationID(const std::string& id) {
        global_correlation_id = id;
        if (snapshotter_instance) snapshotter_instance->setCorrelationID(id);
    }
}

} // namespace nlm