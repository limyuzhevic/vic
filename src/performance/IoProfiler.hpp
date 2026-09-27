#pragma once

/**
 * I/O Profiler
 * 
 * I/O performance monitoring for file system and network I/O tracking
 * in the NLM brain simulation project.
 * 
 * Features:
 * - File system I/O monitoring
 * - Network I/O tracking
 * - I/O throughput analysis
 * - I/O latency measurement
 * - I/O pattern analysis
 */

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>

namespace nlm {

class IoProfiler {
public:
    /**
     * I/O operation record
     */
    struct IoOperation {
        std::chrono::high_resolution_clock::time_point timestamp;
        std::string operation_type; // "read", "write", "seek", "open", "close"
        uint64_t bytes_transferred;
        double duration_seconds;
        std::string file_path;
        std::string stream_type; // "file", "network", "memory"
        int thread_id;
    };

    /**
     * I/O performance metrics
     */
    struct IoMetrics {
        double read_throughput; // bytes per second
        double write_throughput; // bytes per second
        double read_latency; // seconds
        double write_latency; // seconds
        uint64_t total_bytes_read;
        uint64_t total_bytes_written;
        int read_operations;
        int write_operations;
    };

    IoProfiler();
    ~IoProfiler();
    
    /** Record a file read operation */
    void recordRead(uint64_t bytes, const std::string& file_path, double duration);
    
    /** Record a file write operation */
    void recordWrite(uint64_t bytes, const std::string& file_path, double duration);
    
    /** Record a network operation */
    void recordNetworkOperation(const std::string& operation, uint64_t bytes, 
                               double duration, const std::string& endpoint);
    
    /** Get I/O metrics */
    const IoMetrics& getIoMetrics() const { return io_metrics_; }
    
    /** Get read throughput */
    double getReadThroughput() const { return io_metrics_.read_throughput; }
    
    /** Get write throughput */
    double getWriteThroughput() const { return io_metrics_.write_throughput; }
    
    /** Get read operations */
    int getReadOperations() const { return io_metrics_.read_operations; }
    
    /** Get write operations */
    int getWriteOperations() const { return io_metrics_.write_operations; }
    
    /** Get all I/O operations */
    const std::vector<IoOperation>& getIoOperations() const { return io_operations_; }
    
    /** Calculate I/O efficiency */
    double calculateIoEfficiency() const;
    
    /** Get operations by type */
    std::unordered_map<std::string, int> getOperationsByType() const;
    
    /** Clear operations */
    void clear();
    
    /** Enable network I/O tracking */
    void enableNetworkTracking(bool enable) { track_network_ = enable; }
    
private:
    std::vector<IoOperation> io_operations_;
    IoMetrics io_metrics_;
    bool track_network_;
};

} // namespace nlm
