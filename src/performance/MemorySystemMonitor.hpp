#pragma once

/**
 * Memory System Monitor
 * 
 * Memory system performance analysis for the NLM brain simulation project.
 * 
 * Features:
 * - Memory throughput monitoring
 * - Memory latency measurement
 * - Memory bandwidth analysis
 * - Memory pool performance tracking
 * - Memory fragmentation analysis
 */

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>

namespace nlm {

class MemorySystemMonitor {
public:
    /**
     * Memory access record
     */
    struct MemoryAccess {
        std::chrono::high_resolution_clock::time_point timestamp;
        void* address;
        size_t size;
        std::string access_type; // "read", "write"
        int thread_id;
        int cache_level; // 1, 2, 3, or 0 for main memory
        double latency; // seconds
        std::string memory_type; // "neuron_state", "synapse_weight", "network_data"
    };

    /**
     * Memory system metrics
     */
    struct MemoryMetrics {
        double read_throughput; // bytes per second
        double write_throughput; // bytes per second
        double average_latency; // seconds
        double cache_hit_rate;
        double memory_bandwidth_utilization;
        uint64_t total_bytes_read;
        uint64_t total_bytes_written;
        int read_operations;
        int write_operations;
    };

    MemorySystemMonitor();
    ~MemorySystemMonitor();
    
    /** Record memory access */
    void recordMemoryAccess(const MemoryAccess& access);
    
    /** Get all memory accesses */
    const std::vector<MemoryAccess>& getMemoryAccesses() const { return accesses_; }
    
    /** Get memory metrics */
    const MemoryMetrics& getMemoryMetrics() const { return metrics_; }
    
    /** Calculate memory efficiency */
    double calculateMemoryEfficiency() const;
    
    /** Get accesses by cache level */
    std::unordered_map<int, int> getAccessesByCacheLevel() const;
    
    /** Get accesses by memory type */
    std::unordered_map<std::string, int> getAccessesByMemoryType() const;
    
    /** Clear accesses */
    void clear();
    
    /** Enable/disable memory pool tracking */
    void enableMemoryPoolTracking(bool enable) { track_memory_pool_ = enable; }
    
    /** Enable/disable cache tracking */
    void enableCacheTracking(bool enable) { track_cache_ = enable; }
    
private:
    std::vector<MemoryAccess> accesses_;
    MemoryMetrics metrics_;
    bool track_memory_pool_;
    bool track_cache_;
};

} // namespace nlm
