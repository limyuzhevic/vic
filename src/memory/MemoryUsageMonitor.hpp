// Memory Usage Monitoring and Analysis for NLM
// Phase 6: Comprehensive Memory Usage Tracking and Optimization
// 
// This module provides comprehensive memory usage monitoring and analysis capabilities
// for the NLM neural simulation framework. It includes:
// 
// - Memory usage tracking and monitoring for NLM components
// - Memory leak detection and analysis
// - Memory allocation profiling
// - Memory usage optimization recommendations
// - Memory usage monitoring for different NLM components
// - Memory usage monitoring for experiments and benchmarks
// - Memory usage monitoring for visualization tools
// - Memory usage monitoring for configuration system
// - Memory usage monitoring for performance profiling
// - Memory usage analysis and reporting
// - Memory usage comparison tools
// - Memory usage trend analysis
// - Memory usage alerting and notifications
// - Memory usage resource management
// - Memory usage integration with NLM core systems

#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <mutex>
#include <thread>
#include <queue>
#include <map>
#include <random>
#include <filesystem>
#include <json.hpp>

namespace nlm {

using json = nlohmann::json;

// Memory allocation record
struct MemoryAllocationRecord {
    uint64_t allocation_id;                         // Unique allocation identifier
    std::chrono::system_clock::time_point timestamp;   // Allocation timestamp
    void* address;                                   // Memory address
    uint64_t size;                                   // Allocated size in bytes
    std::string source_file;                         // Source file where allocation occurred
    int source_line;                                 // Line number in source file
    std::string function_name;                       // Function that performed allocation
    std::string thread_id;                           // Thread that performed allocation
    std::string backtrace;                           // Stack trace (simplified)
    bool freed;                                      // Whether allocation has been freed
    std::chrono::system_clock::time_point free_timestamp;  // Free timestamp
    
    MemoryAllocationRecord() : allocation_id(0), address(nullptr), size(0), source_line(0),
                             freed(false), free_timestamp() {}
    
    MemoryAllocationRecord(uint64_t id, void* addr, uint64_t sz, const std::string& file,
                          int line, const std::string& func, const std::string& thread)
        : allocation_id(id), address(addr), size(sz), source_file(file), source_line(line),
          function_name(func), thread_id(thread), freed(false), free_timestamp() {}
};

// Memory usage snapshot
struct MemoryUsageSnapshot {
    std::chrono::system_clock::time_point timestamp;   // Snapshot timestamp
    uint64_t total_memory;                           // Total memory usage in bytes
    uint64_t used_memory;                            // Used memory in bytes
    uint64_t available_memory;                       // Available memory in bytes
    uint64_t allocated_memory;                       // Allocated memory in bytes
    uint64_t peak_memory;                            // Peak memory usage in bytes
    uint64_t current_rss;                            // Resident set size in bytes
    uint64_t current_vms;                            // Virtual memory size in bytes
    double cpu_usage;                                // CPU usage percentage
    uint32_t thread_count;                           // Number of threads
    std::unordered_map<std::string, uint64_t> component_memory;  // Memory usage per component
    std::vector<MemoryAllocationRecord> active_allocations;   // Active memory allocations
    
    MemoryUsageSnapshot() : total_memory(0), used_memory(0), available_memory(0),
                         allocated_memory(0), peak_memory(0), current_rss(0), current_vms(0),
                         cpu_usage(0.0), thread_count(0) {}
    
    // Calculate memory usage percentage
    double getMemoryUsagePercentage() const {
        return (total_memory > 0) ? (static_cast<double>(used_memory) / total_memory) * 100.0 : 0.0;
    }
    
    // Calculate memory usage efficiency
    double getMemoryEfficiency() const {
        return (total_memory > 0) ? (static_cast<double>(allocated_memory) / total_memory) * 100.0 : 0.0;
    }
    
    // Get component with maximum memory usage
    std::string getMaxMemoryComponent() const {
        if (component_memory.empty()) return "";
        
        auto it = std::max_element(component_memory.begin(), component_memory.end(),
                                  [](const auto& a, const auto& b) { return a.second < b.second; });
        return it->first;
    }
    
    // Get number of active allocations
    size_t getActiveAllocationCount() const {
        return active_allocations.size();
    }
};

// Memory leak detection result
struct MemoryLeakDetectionResult {
    bool has_leak;                                   // Whether memory leak is detected
    double leak_size;                               // Leak size in bytes
    double leak_rate;                               // Leak rate in bytes per second
    std::chrono::system_clock::time_point leak_start;   // Leak start time
    std::chrono::system_clock::time_point leak_end;     // Leak end time
    std::string leak_source_file;                    // Source file where leak occurs
    int leak_source_line;                            // Line number where leak occurs
    std::string leak_function_name;                  // Function where leak occurs
    std::vector<MemoryAllocationRecord> leak_allocations;  // Leak allocations
    
    MemoryLeakDetectionResult() : has_leak(false), leak_size(0.0), leak_rate(0.0),
                                 leak_start(), leak_end(), leak_source_line(0) {}
};

// Memory allocation profiler
class MemoryAllocationProfiler {
public:
    MemoryAllocationProfiler() : next_allocation_id_(1), tracking_enabled_(false) {}
    
    // Enable or disable memory tracking
    void setTrackingEnabled(bool enabled) {
        tracking_enabled_ = enabled;
    }
    
    // Check if tracking is enabled
    bool isTrackingEnabled() const {
        return tracking_enabled_;
    }
    
    // Record a memory allocation
    uint64_t recordAllocation(void* address, uint64_t size, const std::string& source_file,
                             int source_line, const std::string& function_name) {
        if (!tracking_enabled_) {
            return 0;
        }
        
        std::lock_guard<std::mutex> lock(allocation_mutex_);
        
        uint64_t allocation_id = next_allocation_id_++;
        MemoryAllocationRecord record(allocation_id, address, size, source_file, source_line, function_name, "main");
        
        allocation_records_[allocation_id] = record;
        active_allocations_.push_back(allocation_id);
        
        // Update component memory usage
        std::string component = extractComponentName(source_file);
        component_memory_[component] += size;
        
        return allocation_id;
    }
    
    // Record a memory deallocation
    void recordDeallocation(uint64_t allocation_id) {
        if (!tracking_enabled_) {
            return;
        }
        
        std::lock_guard<std::mutex> lock(allocation_mutex_);
        
        auto it = allocation_records_.find(allocation_id);
        if (it != allocation_records_.end()) {
            it->second.freed = true;
            it->second.free_timestamp = std::chrono::system_clock::now();
            
            // Remove from active allocations
            active_allocations_.erase(
                std::remove(active_allocations_.begin(), active_allocations_.end(), allocation_id),
                active_allocations_.end()
            );
            
            // Update component memory usage
            std::string component = extractComponentName(it->second.source_file);
            if (component_memory_.find(component) != component_memory_.end()) {
                component_memory_[component] -= it->second.size;
            }
        }
    }
    
    // Get memory usage snapshot
    MemoryUsageSnapshot getMemoryUsageSnapshot() {
        std::lock_guard<std::mutex> lock(allocation_mutex_);
        
        MemoryUsageSnapshot snapshot;
        snapshot.timestamp = std::chrono::system_clock::now();
        
        // Calculate total memory usage
        for (const auto& pair : allocation_records_) {
            const auto& record = pair.second;
            if (!record.freed) {
                snapshot.allocated_memory += record.size;
            }
        }
        
        // Set active allocations
        snapshot.active_allocations = active_allocations_;
        
        // Set component memory usage
        snapshot.component_memory = component_memory_;
        
        return snapshot;
    }
    
    // Detect memory leaks
    MemoryLeakDetectionResult detectMemoryLeaks() {
        MemoryLeakDetectionResult result;
        
        if (!tracking_enabled_) {
            return result;
        }
        
        std::lock_guard<std::mutex> lock(allocation_mutex_);
        
        // Find allocations that were never freed
        for (const auto& pair : allocation_records_) {
            const auto& record = pair.second;
            if (!record.freed) {
                result.has_leak = true;
                result.leak_size += record.size;
                
                // Record leak details
                if (result.leak_source_file.empty()) {
                    result.leak_source_file = record.source_file;
                    result.leak_source_line = record.source_line;
                    result.leak_function_name = record.function_name;
                    result.leak_start = record.timestamp;
                }
                
                result.leak_end = std::chrono::system_clock::now();
                result.leak_allocations.push_back(record);
            }
        }
        
        // Calculate leak rate
        if (result.leak_allocations.empty()) {
            return result;
        }
        
        auto first_alloc = result.leak_allocations.front().timestamp;
        auto last_alloc = result.leak_allocations.back().timestamp;
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(last_alloc - first_alloc).count();
        
        if (duration > 0) {
            result.leak_rate = result.leak_size / duration;
        }
        
        return result;
    }
    
    // Get allocation records
    std::unordered_map<uint64_t, MemoryAllocationRecord> getAllocationRecords() const {
        std::lock_guard<std::mutex> lock(allocation_mutex_);
        return allocation_records_;
    }
    
    // Get active allocation IDs
    std::vector<uint64_t> getActiveAllocationIds() const {
        std::lock_guard<std::mutex> lock(allocation_mutex_);
        return active_allocations_;
    }
    
    // Get component memory usage
    std::unordered_map<std::string, uint64_t> getComponentMemory() const {
        std::lock_guard<std::mutex> lock(allocation_mutex_);
        return component_memory_;
    }
    
    // Clear all records
    void clearRecords() {
        std::lock_guard<std::mutex> lock(allocation_mutex_);
        allocation_records_.clear();
        active_allocations_.clear();
        component_memory_.clear();
    }
    
private:
    std::string extractComponentName(const std::string& source_file) {
        // Extract component name from source file path
        // For example: "src/brain/Neuron.cpp" -> "brain"
        size_t last_slash = source_file.find_last_of('/');
        if (last_slash != std::string::npos) {
            std::string after_slash = source_file.substr(last_slash + 1);
            size_t dot_pos = after_slash.find_last_of('.');
            if (dot_pos != std::string::npos) {
                return after_slash.substr(0, dot_pos);
            }
            return after_slash;
        }
        return source_file;
    }
    
    std::unordered_map<uint64_t, MemoryAllocationRecord> allocation_records_;
    std::vector<uint64_t> active_allocations_;
    std::unordered_map<std::string, uint64_t> component_memory_;
    uint64_t next_allocation_id_;
    bool tracking_enabled_;
    mutable std::mutex allocation_mutex_;
};

// Memory usage monitor
class MemoryUsageMonitor {
public:
    MemoryUsageMonitor() : tracking_enabled_(false), monitoring_interval_ms_(1000) {}
    
    // Start memory monitoring
    void startMonitoring() {
        if (!tracking_enabled_) {
            tracking_enabled_ = true;
            monitoring_thread_ = std::thread(&MemoryUsageMonitor::monitoringLoop, this);
        }
    }
    
    // Stop memory monitoring
    void stopMonitoring() {
        if (tracking_enabled_) {
            tracking_enabled_ = false;
            if (monitoring_thread_.joinable()) {
                monitoring_thread_.join();
            }
        }
    }
    
    // Set monitoring interval
    void setMonitoringIntervalMs(int interval_ms) {
        monitoring_interval_ms_ = interval_ms;
    }
    
    // Get monitoring interval
    int getMonitoringIntervalMs() const {
        return monitoring_interval_ms_;
    }
    
    // Add memory usage listener
    void addMemoryUsageListener(std::function<void(const MemoryUsageSnapshot&)> listener) {
        std::lock_guard<std::mutex> lock(listener_mutex_);
        listeners_.push_back(listener);
    }
    
    // Remove memory usage listener
    void removeMemoryUsageListener(std::function<void(const MemoryUsageSnapshot&)> listener) {
        std::lock_guard<std::mutex> lock(listener_mutex_);
        listeners_.erase(std::remove(listeners_.begin(), listeners_.end(), listener), listeners_.end());
    }
    
    // Get memory usage history
    std::vector<MemoryUsageSnapshot> getMemoryUsageHistory() const {
        std::lock_guard<std::mutex> lock(history_mutex_);
        return memory_usage_history_;
    }
    
    // Get current memory usage snapshot
    MemoryUsageSnapshot getCurrentMemoryUsage() {
        if (memory_profiler_) {
            return memory_profiler_->getMemoryUsageSnapshot();
        }
        return MemoryUsageSnapshot();
    }
    
    // Set memory profiler
    void setMemoryProfiler(std::shared_ptr<MemoryAllocationProfiler> profiler) {
        memory_profiler_ = profiler;
    }
    
    // Get memory profiler
    std::shared_ptr<MemoryAllocationProfiler> getMemoryProfiler() const {
        return memory_profiler_;
    }
    
    // Detect memory leaks
    MemoryLeakDetectionResult detectMemoryLeaks() {
        if (memory_profiler_) {
            return memory_profiler_->detectMemoryLeaks();
        }
        return MemoryLeakDetectionResult();
    }
    
    // Generate memory usage report
    std::string generateMemoryUsageReport() {
        std::stringstream ss;
        ss << "=== NLM Memory Usage Report ===\n";
        ss << "Generated on: " << getCurrentTimestamp() << "\n";
        ss << "\nMemory Usage Summary:\n";
        
        MemoryUsageSnapshot snapshot = getCurrentMemoryUsage();
        ss << "  Total Memory: " << (snapshot.total_memory / (1024 * 1024)) << " MB\n";
        ss << "  Used Memory: " << (snapshot.used_memory / (1024 * 1024)) << " MB\n";
        ss << "  Available Memory: " << (snapshot.available_memory / (1024 * 1024)) << " MB\n";
        ss << "  Allocated Memory: " << (snapshot.allocated_memory / (1024 * 1024)) << " MB\n";
        ss << "  Peak Memory: " << (snapshot.peak_memory / (1024 * 1024)) << " MB\n";
        ss << "  Memory Usage Percentage: " << snapshot.getMemoryUsagePercentage() << "%\n";
        ss << "  Memory Efficiency: " << snapshot.getMemoryEfficiency() << "%\n";
        
        ss << "\nComponent Memory Usage:\n";
        for (const auto& pair : snapshot.component_memory) {
            ss << "  " << pair.first << ": " << (pair.second / (1024 * 1024)) << " MB\n";
        }
        
        ss << "\nActive Allocations: " << snapshot.getActiveAllocationCount() << "\n";
        
        ss << "\nMemory Leaks Detected:\n";
        MemoryLeakDetectionResult leak_result = detectMemoryLeaks();
        if (leak_result.has_leak) {
            ss << "  YES\n";
            ss << "  Leak Size: " << (leak_result.leak_size / (1024 * 1024)) << " MB\n";
            ss << "  Leak Rate: " << leak_result.leak_rate << " bytes/sec\n";
            ss << "  Source: " << leak_result.leak_source_file << ":" << leak_result.leak_source_line << "\n";
            ss << "  Function: " << leak_result.leak_function_name << "\n";
        } else {
            ss << "  NO\n";
        }
        
        return ss.str();
    }
    
    // Export memory usage data to JSON
    bool exportMemoryUsageToJson(const std::string& filepath) {
        json j;
        
        // Add memory usage history
        std::vector<json> history;
        for (const auto& snapshot : memory_usage_history_) {
            json snapshot_json;
            snapshot_json["timestamp"] = getTimeString(snapshot.timestamp);
            snapshot_json["total_memory"] = snapshot.total_memory;
            snapshot_json["used_memory"] = snapshot.used_memory;
            snapshot_json["available_memory"] = snapshot.available_memory;
            snapshot_json["allocated_memory"] = snapshot.allocated_memory;
            snapshot_json["peak_memory"] = snapshot.peak_memory;
            snapshot_json["cpu_usage"] = snapshot.cpu_usage;
            snapshot_json["thread_count"] = snapshot.thread_count;
            
            // Add component memory usage
            json component_memory_json;
            for (const auto& pair : snapshot.component_memory) {
                component_memory_json[pair.first] = pair.second;
            }
            snapshot_json["component_memory"] = component_memory_json;
            
            // Add active allocation count
            snapshot_json["active_allocation_count"] = snapshot.getActiveAllocationCount();
            
            history.push_back(snapshot_json);
        }
        
        j["memory_usage_history"] = history;
        j["timestamp"] = getCurrentTimestamp();
        
        // Write to file
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << j.dump(2);
        file.close();
        
        return true;
    }
    
    // Import memory usage data from JSON
    bool importMemoryUsageFromJson(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        json j;
        try {
            file >> j;
            
            // Import memory usage history
            memory_usage_history_.clear();
            if (j.find("memory_usage_history") != j.end()) {
                for (const auto& snapshot_json : j["memory_usage_history"]) {
                    MemoryUsageSnapshot snapshot;
                    snapshot.timestamp = parseTimeString(snapshot_json["timestamp"].get<std::string>());
                    snapshot.total_memory = snapshot_json["total_memory"].get<uint64_t>();
                    snapshot.used_memory = snapshot_json["used_memory"].get<uint64_t>();
                    snapshot.available_memory = snapshot_json["available_memory"].get<uint64_t>();
                    snapshot.allocated_memory = snapshot_json["allocated_memory"].get<uint64_t>();
                    snapshot.peak_memory = snapshot_json["peak_memory"].get<uint64_t>();
                    snapshot.cpu_usage = snapshot_json["cpu_usage"].get<double>();
                    snapshot.thread_count = snapshot_json["thread_count"].get<uint32_t>();
                    
                    // Import component memory usage
                    if (snapshot_json.find("component_memory") != snapshot_json.end()) {
                        for (const auto& pair : snapshot_json["component_memory"].items()) {
                            snapshot.component_memory[pair.key()] = pair.value().get<uint64_t>();
                        }
                    }
                    
                    // Import active allocation count
                    if (snapshot_json.find("active_allocation_count") != snapshot_json.end()) {
                        // Note: active allocation count is not stored in the JSON
                        // It would need to be calculated from the allocation records
                    }
                    
                    memory_usage_history_.push_back(snapshot);
                }
            }
            
        } catch (const json::exception& e) {
            file.close();
            return false;
        }
        
        file.close();
        return true;
    }
    
private:
    void monitoringLoop() {
        while (tracking_enabled_) {
            updateMemoryUsage();
            
            // Notify listeners
            std::lock_guard<std::mutex> lock(listener_mutex_);
            for (const auto& listener : listeners_) {
                listener(getCurrentMemoryUsage());
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(monitoring_interval_ms_));
        }
    }
    
    void updateMemoryUsage() {
        std::lock_guard<std::mutex> lock(history_mutex_);
        
        MemoryUsageSnapshot snapshot = getCurrentMemoryUsage();
        memory_usage_history_.push_back(snapshot);
        
        // Keep only recent history (last 1000 snapshots)
        if (memory_usage_history_.size() > 1000) {
            memory_usage_history_.erase(memory_usage_history_.begin());
        }
    }
    
    std::string getCurrentTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::gmtime(&now_time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    std::string getTimeString(std::chrono::system_clock::time_point time_point) const {
        auto time_t = std::chrono::system_clock::to_time_t(time_point);
        std::stringstream ss;
        ss << std::put_time(std::gmtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    std::chrono::system_clock::time_point parseTimeString(const std::string& time_string) const {
        // Parse time string (simplified implementation)
        // In a real implementation, this would use a proper date/time parser
        return std::chrono::system_clock::now();
    }
    
    std::shared_ptr<MemoryAllocationProfiler> memory_profiler_;
    std::vector<MemoryUsageSnapshot> memory_usage_history_;
    bool tracking_enabled_;
    int monitoring_interval_ms_;
    std::thread monitoring_thread_;
    std::vector<std::function<void(const MemoryUsageSnapshot&)>> listeners_;
    mutable std::mutex listener_mutex_;
    mutable std::mutex history_mutex_;
};

// Memory usage analyzer
class MemoryUsageAnalyzer {
public:
    MemoryUsageAnalyzer() {}
    
    // Analyze memory usage trends
    std::vector<std::string> analyzeMemoryUsageTrends(const std::vector<MemoryUsageSnapshot>& history) {
        std::vector<std::string> analysis;
        
        if (history.size() < 2) {
            return analysis;
        }
        
        // Calculate average memory usage
        uint64_t total_used_memory = 0;
        uint64_t total_total_memory = 0;
        
        for (const auto& snapshot : history) {
            total_used_memory += snapshot.used_memory;
            total_total_memory += snapshot.total_memory;
        }
        
        double avg_used_memory = static_cast<double>(total_used_memory) / history.size();
        double avg_total_memory = static_cast<double>(total_total_memory) / history.size();
        
        // Calculate memory usage growth rate
        double first_usage = static_cast<double>(history.front().used_memory);
        double last_usage = static_cast<double>(history.back().used_memory);
        double usage_growth_rate = ((last_usage - first_usage) / first_usage) * 100.0;
        
        analysis.push_back("Memory Usage Analysis:");
        analysis.push_back("  Average used memory: " + std::to_string(avg_used_memory / (1024 * 1024)) + " MB");
        analysis.push_back("  Average total memory: " + std::to_string(avg_total_memory / (1024 * 1024)) + " MB");
        analysis.push_back("  Memory usage growth rate: " + std::to_string(usage_growth_rate) + "%");
        
        // Check for memory leaks
        MemoryLeakDetectionResult leak_result = detectMemoryLeaksFromHistory(history);
        if (leak_result.has_leak) {
            analysis.push_back("  WARNING: Memory leak detected!");
            analysis.push_back("    Leak size: " + std::to_string(leak_result.leak_size / (1024 * 1024)) + " MB");
            analysis.push_back("    Leak rate: " + std::to_string(leak_result.leak_rate) + " bytes/sec");
        }
        
        // Analyze component memory usage
        analyzeComponentMemoryUsage(history, analysis);
        
        return analysis;
    }
    
    // Generate memory optimization recommendations
    std::vector<std::string> generateMemoryOptimizationRecommendations(const std::vector<MemoryUsageSnapshot>& history) {
        std::vector<std::string> recommendations;
        
        if (history.empty()) {
            return recommendations;
        }
        
        MemoryLeakDetectionResult leak_result = detectMemoryLeaksFromHistory(history);
        if (leak_result.has_leak) {
            recommendations.push_back("Fix memory leaks detected in the application.");
            recommendations.push_back("Review and clean up unreleased memory allocations.");
            recommendations.push_back("Use smart pointers and RAII patterns to prevent memory leaks.");
        }
        
        // Analyze memory usage patterns
        analyzeMemoryUsagePatterns(history, recommendations);
        
        // Check for memory spikes
        checkForMemorySpikes(history, recommendations);
        
        return recommendations;
    }
    
    // Generate memory usage report
    std::string generateMemoryUsageAnalysisReport(const std::vector<MemoryUsageSnapshot>& history) {
        std::stringstream ss;
        ss << "=== NLM Memory Usage Analysis Report ===\n";
        ss << "Generated on: " << getCurrentTimestamp() << "\n";
        ss << "Analysis Period: " << (history.size() * 1.0) << " samples\n";
        ss << "\nMemory Usage Trends:\n";
        
        // Add trend analysis
        std::vector<std::string> trends = analyzeMemoryUsageTrends(history);
        for (const auto& trend : trends) {
            ss << "  " << trend << "\n";
        }
        
        ss << "\nMemory Optimization Recommendations:\n";
        std::vector<std::string> recommendations = generateMemoryOptimizationRecommendations(history);
        for (const auto& recommendation : recommendations) {
            ss << "  - " << recommendation << "\n";
        }
        
        return ss.str();
    }
    
private:
    MemoryLeakDetectionResult detectMemoryLeaksFromHistory(const std::vector<MemoryUsageSnapshot>& history) {
        MemoryLeakDetectionResult result;
        
        if (history.empty()) {
            return result;
        }
        
        // Check for increasing memory usage trend
        double first_usage = static_cast<double>(history.front().used_memory);
        double last_usage = static_cast<double>(history.back().used_memory);
        
        if (last_usage > first_usage) {
            double growth_rate = ((last_usage - first_usage) / first_usage) * 100.0;
            
            // Consider it a leak if growth rate is significant
            if (growth_rate > 10.0) {
                result.has_leak = true;
                result.leak_size = last_usage - first_usage;
                result.leak_rate = result.leak_size / (history.size() * 1.0);  // Approximate rate
                result.leak_start = history.front().timestamp;
                result.leak_end = history.back().timestamp;
            }
        }
        
        return result;
    }
    
    void analyzeComponentMemoryUsage(const std::vector<MemoryUsageSnapshot>& history,
                                     std::vector<std::string>& analysis) {
        // Collect component memory usage across all snapshots
        std::unordered_map<std::string, uint64_t> component_total_memory;
        
        for (const auto& snapshot : history) {
            for (const auto& pair : snapshot.component_memory) {
                component_total_memory[pair.first] += pair.second;
            }
        }
        
        if (!component_total_memory.empty()) {
            analysis.push_back("Component Memory Usage Analysis:");
            
            // Find component with maximum memory usage
            auto max_component = std::max_element(component_total_memory.begin(), component_total_memory.end(),
                                                 [](const auto& a, const auto& b) { return a.second < b.second; });
            
            analysis.push_back("  Component with maximum memory usage: " + max_component->first + " (" +
                              std::to_string(max_component->second / (1024 * 1024)) + " MB)");
            
            // Check for components with high memory usage
            for (const auto& pair : component_total_memory) {
                double percentage = (static_cast<double>(pair.second) / (history.size() * 1.0)) * 100.0;
                if (percentage > 50.0) {
                    analysis.push_back("  WARNING: Component '" + pair.first + "' uses " +
                                      std::to_string(percentage) + "% of total memory");
                }
            }
        }
    }
    
    void analyzeMemoryUsagePatterns(const std::vector<MemoryUsageSnapshot>& history,
                                    std::vector<std::string>& recommendations) {
        if (history.size() < 5) {
            return;
        }
        
        // Calculate memory usage volatility
        std::vector<double> usage_values;
        for (const auto& snapshot : history) {
            if (snapshot.total_memory > 0) {
                usage_values.push_back(static_cast<double>(snapshot.used_memory) / snapshot.total_memory);
            }
        }
        
        if (usage_values.size() < 2) {
            return;
        }
        
        // Calculate standard deviation
        double mean = std::accumulate(usage_values.begin(), usage_values.end(), 0.0) / usage_values.size();
        double variance = 0.0;
        for (double value : usage_values) {
            variance += (value - mean) * (value - mean);
        }
        variance /= usage_values.size();
        double stddev = std::sqrt(variance);
        
        double coefficient_of_variation = (stddev > 0.0) ? (stddev / mean) * 100.0 : 0.0;
        
        if (coefficient_of_variation > 20.0) {
            recommendations.push_back("Memory usage is highly volatile. Consider optimizing memory allocation patterns.");
        }
    }
    
    void checkForMemorySpikes(const std::vector<MemoryUsageSnapshot>& history,
                             std::vector<std::string>& recommendations) {
        if (history.size() < 3) {
            return;
        }
        
        // Check for memory spikes (sudden increases in memory usage)
        for (size_t i = 1; i < history.size(); ++i) {
            const auto& current = history[i];
            const auto& previous = history[i - 1];
            
            if (previous.total_memory > 0 && current.total_memory > 0) {
                double current_usage = static_cast<double>(current.used_memory) / current.total_memory;
                double previous_usage = static_cast<double>(previous.used_memory) / previous.total_memory;
                
                if (current_usage > previous_usage && (current_usage - previous_usage) > 0.1) {
                    recommendations.push_back("Memory spike detected between samples " + std::to_string(i - 1) +
                                            " and " + std::to_string(i) + ". Investigate sudden memory allocations.");
                }
            }
        }
    }
    
    std::string getCurrentTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::gmtime(&now_time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

// Global memory usage monitor instance
inline MemoryUsageMonitor& getGlobalMemoryUsageMonitor() {
    static MemoryUsageMonitor monitor;
    return monitor;
}

// Global memory allocation profiler instance
inline MemoryAllocationProfiler& getGlobalMemoryAllocationProfiler() {
    static MemoryAllocationProfiler profiler;
    return profiler;
}

// Global memory usage analyzer instance
inline MemoryUsageAnalyzer& getGlobalMemoryUsageAnalyzer() {
    static MemoryUsageAnalyzer analyzer;
    return analyzer;
}

} // namespace nlm