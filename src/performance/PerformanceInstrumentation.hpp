#pragma once

/**
 * Performance Instrumentation Framework
 * 
 * Core infrastructure for high-resolution performance monitoring and profiling
 * in the NLM brain simulation project.
 * 
 * This module provides:
 * - High-resolution timing with nanosecond precision
 * - Memory allocation tracking
 * - CPU performance monitoring
 * - I/O performance monitoring
 * - Thread performance analysis
 * 
 * Designed for minimal overhead to support production use.
 */

#include <chrono>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace nlm {

class PerformanceInstrumentation {
public:
    /**
     * High-resolution timer for critical operations
     * 
     * Supports nanosecond precision timing with automatic unit conversion
     * to seconds, milliseconds, microseconds, or nanoseconds as needed.
     */
    class HighResolutionTimer {
    public:
        HighResolutionTimer();
        ~HighResolutionTimer();
        
        /** Start timing */
        void start();
        
        /** Stop timing and return elapsed time in specified units */
        double stop(const std::string& units = "seconds");
        
        /** Reset timer for reuse */
        void reset();
        
        /** Check if timer is currently running */
        bool isRunning() const { return running_; }
        
    private:
        std::chrono::high_resolution_clock::time_point start_time_;
        std::chrono::high_resolution_clock::time_point stop_time_;
        bool running_;
    };

    /**
     * Memory allocation tracking for heap allocations
     * 
     * Tracks all memory allocations and deallocations to identify leaks,
     * excessive allocations, and memory fragmentation patterns.
     */
    class MemoryTracker {
    public:
        struct AllocationInfo {
            void* address;
            size_t size;
            const char* file;
            int line;
            std::chrono::high_resolution_clock::time_point timestamp;
        };
        
        MemoryTracker();
        ~MemoryTracker();
        
        /** Track a memory allocation */
        void trackAllocation(void* address, size_t size, const char* file, int line);
        
        /** Track a memory deallocation */
        void trackDeallocation(void* address);
        
        /** Get current memory usage */
        size_t getCurrentMemoryUsage() const;
        
        /** Get total memory allocated */
        size_t getTotalMemoryAllocated() const;
        
        /** Get peak memory usage */
        size_t getPeakMemoryUsage() const;
        
        /** Get allocation statistics */
        const std::vector<AllocationInfo>& getAllocations() const { return allocations_; }
        
        /** Clear all tracked allocations */
        void clear();
        
    private:
        std::vector<AllocationInfo> allocations_;
        size_t current_memory_usage_;
        size_t total_memory_allocated_;
        size_t peak_memory_usage_;
    };

    /**
     * CPU performance monitoring for instruction timing
     * 
     * Monitors CPU utilization, instruction timing, and cache performance
     * to identify CPU bottlenecks and optimization opportunities.
     */
    class CpuProfiler {
    public:
        CpuProfiler();
        ~CpuProfiler();
        
        /** Record CPU cycle count */
        void recordCycles(uint64_t cycles);
        
        /** Record CPU instructions retired */
        void recordInstructions(uint64_t instructions);
        
        /** Get total CPU cycles */
        uint64_t getTotalCycles() const { return total_cycles_; }
        
        /** Get total instructions retired */
        uint64_t getTotalInstructions() const { return total_instructions_; }
        
        /** Get cycles per instruction (CPI) */
        double getCPI() const;
        
        /** Get instructions per cycle (IPC) */
        double getIPC() const;
        
        /** Get current CPU utilization percentage */
        double getCpuUtilization() const;
        
    private:
        uint64_t total_cycles_;
        uint64_t total_instructions_;
        std::chrono::high_resolution_clock::time_point last_time_;
    };

    /**
     * I/O performance monitoring for file system and network operations
     * 
     * Tracks all I/O operations to identify bottlenecks and optimize
     * data transfer patterns for large-scale simulations.
     */
    class IoProfiler {
    public:
        struct IoOperation {
            std::chrono::high_resolution_clock::time_point timestamp;
            std::string operation_type; // "read", "write", "seek", "open", "close"
            uint64_t bytes_transferred;
            double duration_seconds;
            std::string file_path;
        };
        
        IoProfiler();
        ~IoProfiler();
        
        /** Record an I/O operation */
        void recordOperation(const std::string& operation_type, 
                           uint64_t bytes_transferred,
                           const std::string& file_path);
        
        /** Get I/O statistics */
        const std::vector<IoOperation>& getOperations() const { return operations_; }
        
        /** Get total bytes read */
        uint64_t getTotalBytesRead() const;
        
        /** Get total bytes written */
        uint64_t getTotalBytesWritten() const;
        
        /** Get average I/O throughput (bytes/sec) */
        double getAverageThroughput() const;
        
    private:
        std::vector<IoOperation> operations_;
    };

    /**
     * Thread performance analysis for synchronization
     * 
     * Analyzes thread scheduling, synchronization patterns, and contention
     * to optimize parallel processing in multi-threaded simulations.
     */
    class ThreadProfiler {
    public:
        struct ThreadStats {
            int thread_id;
            double cpu_time;
            double wall_time;
            int context_switches;
            int lock_contention_time;
        };
        
        ThreadProfiler();
        ~ThreadProfiler();
        
        /** Record thread statistics */
        void recordThreadStats(const ThreadStats& stats);
        
        /** Get all thread statistics */
        const std::vector<ThreadStats>& getThreadStats() const { return thread_stats_; }
        
        /** Get average thread context switches */
        double getAverageContextSwitches() const;
        
        /** Get total lock contention time */
        double getTotalLockContention() const;
        
    private:
        std::vector<ThreadStats> thread_stats_;
    };
};

} // namespace nlm
