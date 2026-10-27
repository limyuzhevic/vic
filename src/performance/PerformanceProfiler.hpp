#pragma once

#include "PerformanceMonitor.hpp"
#include <chrono>
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

namespace nlm {

/**
 * @class PerformanceProfiler
 * @brief High-resolution performance profiler for detailed timing analysis
 * 
 * Provides detailed timing information for specific code sections and functions.
 * Useful for identifying performance bottlenecks and optimization opportunities.
 * 
 * The PerformanceProfiler allows:
 * - High-precision timing of specific operations
 * - Aggregation of timing statistics across multiple calls
 * - Comparison of different code paths and algorithms
 * - Identification of performance hotspots
 * - Optimization guidance and recommendations
 * 
 * @author NLM Research Team
 * @version 0.1.0
 */
class PerformanceProfiler {
public:
    /**
     * @brief Profiler scope for automatic timing management
     * 
     * RAII-style scope that automatically starts and stops timing
     * when entering and leaving its scope.
     * 
     * @author NLM Research Team
     * @version 0.1.0
     */
    class Scope {
    public:
        /**
         * @brief Construct a new Scope object
         * 
         * @param profiler Reference to the profiler to record timing
         * @param name Name of the profiled operation
         */
        Scope(PerformanceProfiler& profiler, const std::string& name);
        
        /**
         * @brief Destroy the Scope object and record timing
         */
        ~Scope();
        
        // Disable copying
        Scope(const Scope&) = delete;
        Scope& operator=(const Scope&) = delete;

    private:
        PerformanceProfiler& profiler_;
        std::string name_;
        std::chrono::high_resolution_clock::time_point startTime_;
    };
    
    /**
     * @brief Construct a new PerformanceProfiler object
     */
    PerformanceProfiler()
    {
        reset();
    }
    
    /**
     * @brief Destroy the PerformanceProfiler object
     */
    ~PerformanceProfiler() = default;
    
    // Disable copying
    PerformanceProfiler(const PerformanceProfiler&) = delete;
    PerformanceProfiler& operator=(const PerformanceProfiler&) = delete;
    
    /**
     * @brief Start profiling a specific operation
     * 
     * @param name Name of the operation to profile
     * @return Scope RAII scope for automatic timing
     */
    Scope profile(const std::string& name) {
        return Scope(*this, name);
    }
    
    /**
     * @brief Start manual timing for an operation
     * 
     * @param name Name of the operation to start timing
     * @return uint64_t Start time in microseconds
     */
    uint64_t startTiming(const std::string& name) {
        timingStartTimes_[name] = getCurrentTimestamp();
        return timingStartTimes_[name];
    }
    
    /**
     * @brief Stop manual timing and record the duration
     * 
     * @param name Name of the operation to stop timing
     * @return uint64_t Duration in microseconds, or 0 if not started
     */
    uint64_t stopTiming(const std::string& name) {
        auto it = timingStartTimes_.find(name);
        if (it == timingStartTimes_.end()) {
            return 0;
        }
        
        uint64_t endTime = getCurrentTimestamp();
        uint64_t startTime = it->second;
        uint64_t duration = endTime - startTime;
        
        // Record in statistics
        timingStatistics_[name].push_back(duration);
        
        // Clean up
        timingStartTimes_.erase(it);
        
        return duration;
    }
    
    /**
     * @brief Get timing statistics for an operation
     * 
     * @param name Name of the operation
     * @return const std::vector<uint64_t>& Vector of timing samples
     */
    const std::vector<uint64_t>& getTimingStatistics(const std::string& name) const {
        auto it = timingStatistics_.find(name);
        return it != timingStatistics_.end() ? it->second : emptyStatistics_;
    }
    
    /**
     * @brief Get all timing operation names
     */
    std::vector<std::string> getOperationNames() const {
        std::vector<std::string> names;
        names.reserve(timingStatistics_.size());
        for (const auto& pair : timingStatistics_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    /**
     * @brief Get the count of operations for a given name
     */
    size_t getOperationCount(const std::string& name) const {
        auto it = timingStatistics_.find(name);
        return it != timingStatistics_.end() ? it->second.size() : 0;
    }
    
    /**
     * @brief Get total timing count across all operations
     */
    size_t getTotalTimingCount() const {
        size_t total = 0;
        for (const auto& pair : timingStatistics_) {
            total += pair.second.size();
        }
        return total;
    }
    
    /**
     * @brief Get average time for an operation
     */
    double getAverageTime(const std::string& name) const {
        const auto& samples = getTimingStatistics(name);
        return !samples.empty() ? calculateAverage(samples) : 0.0;
    }
    
    /**
     * @brief Get maximum time for an operation
     */
    uint64_t getMaxTime(const std::string& name) const {
        const auto& samples = getTimingStatistics(name);
        return !samples.empty() ? *std::max_element(samples.begin(), samples.end()) : 0;
    }
    
    /**
     * @brief Get minimum time for an operation
     */
    uint64_t getMinTime(const std::string& name) const {
        const auto& samples = getTimingStatistics(name);
        return !samples.empty() ? *std::min_element(samples.begin(), samples.end()) : 0;
    }
    
    /**
     * @brief Get standard deviation of times for an operation
     */
    double getStandardDeviation(const std::string& name) const {
        const auto& samples = getTimingStatistics(name);
        if (samples.size() <= 1) return 0.0;
        return calculateStandardDeviation(samples);
    }
    
    /**
     * @brief Get total time for an operation
     */
    uint64_t getTotalTime(const std::string& name) const {
        const auto& samples = getTimingStatistics(name);
        return std::accumulate(samples.begin(), samples.end(), 0ULL);
    }
    
    /**
     * @brief Get rate (operations per second) for an operation
     */
    double getRate(const std::string& name) const {
        const auto& samples = getTimingStatistics(name);
        if (samples.empty()) return 0.0;
        
        uint64_t totalTime = getTotalTime(name);
        double totalSeconds = totalTime / 1000000.0;  // Convert to seconds
        return !totalSeconds ? 0.0 : samples.size() / totalSeconds;
    }
    
    /**
     * @brief Get detailed report for all operations
     */
    std::string getDetailedReport() const {
        std::stringstream ss;
        ss << "Performance Profiler Detailed Report" << std::endl;
        ss << "=====================================" << std::endl;
        ss << "Total Operations: " << getTotalTimingCount() << std::endl;
        ss << std::endl;
        
        for (const auto& name : getOperationNames()) {
            ss << "Operation: " << name << std::endl;
            ss << "  Count: " << getOperationCount(name) << std::endl;
            ss << "  Total Time: " << getTotalTime(name) << " μs" << std::endl;
            ss << "  Average Time: " << getAverageTime(name) << " μs" << std::endl;
            ss << "  Min Time: " << getMinTime(name) << " μs" << std::endl;
            ss << "  Max Time: " << getMaxTime(name) << " μs" << std::endl;
            ss << "  Standard Deviation: " << getStandardDeviation(name) << " μs" << std::endl;
            ss << "  Rate: " << getRate(name) << " ops/sec" << std::endl;
            ss << std::endl;
        }
        
        return ss.str();
    }
    
    /**
     * @brief Get bottleneck analysis report
     */
    std::string getBottleneckAnalysis() const {
        std::stringstream ss;
        ss << "Performance Bottleneck Analysis" << std::endl;
        ss << "===============================" << std::endl;
        ss << std::endl;
        
        if (getTotalTimingCount() == 0) {
            ss << "No performance data available." << std::endl;
            return ss.str();
        }
        
        // Find slowest operation
        std::string slowestOperation;
        uint64_t slowestTime = 0;
        for (const auto& name : getOperationNames()) {
            uint64_t totalTime = getTotalTime(name);
            if (totalTime > slowestTime) {
                slowestTime = totalTime;
                slowestOperation = name;
            }
        }
        
        if (!slowestOperation.empty()) {
            ss << "Slowest Operation: " << slowestOperation << std::endl;
            ss << "  Total Time: " << slowestTime << " μs" << std::endl;
            ss << "  Average Time: " << getAverageTime(slowestOperation) << " μs" << std::endl;
            ss << "  Percentage of Total Time: " << 
                (slowestTime * 100.0 / getTotalTime("_total_")) << "%" << std::endl;
            ss << std::endl;
        }
        
        // Find most frequent operation
        std::string mostFrequentOperation;
        size_t mostFrequentCount = 0;
        for (const auto& name : getOperationNames()) {
            size_t count = getOperationCount(name);
            if (count > mostFrequentCount) {
                mostFrequentCount = count;
                mostFrequentOperation = name;
            }
        }
        
        if (!mostFrequentOperation.empty()) {
            ss << "Most Frequent Operation: " << mostFrequentOperation << std::endl;
            ss << "  Count: " << mostFrequentCount << std::endl;
            ss << "  Average Time: " << getAverageTime(mostFrequentOperation) << " μs" << std::endl;
            ss << std::endl;
        }
        
        return ss.str();
    }
    
    /**
     * @brief Get performance trend analysis
     */
    std::string getPerformanceTrendAnalysis() const {
        std::stringstream ss;
        ss << "Performance Trend Analysis" << std::endl;
        ss << "=========================" << std::endl;
        ss << std::endl;
        
        if (getTotalTimingCount() == 0) {
            ss << "No performance data available for trend analysis." << std::endl;
            return ss.str();
        }
        
        // Calculate performance metrics
        double averageTime = 0.0;
        uint64_t totalTime = 0;
        size_t totalCount = 0;
        
        for (const auto& name : getOperationNames()) {
            const auto& samples = getTimingStatistics(name);
            if (!samples.empty()) {
                averageTime += getAverageTime(name) * samples.size();
                totalTime += getTotalTime(name);
                totalCount += samples.size();
            }
        }
        
        if (totalCount > 0) {
            averageTime /= totalCount;
            ss << "Average Time per Operation: " << averageTime << " μs" << std::endl;
            ss << "Total Time Across All Operations: " << totalTime << " μs" << std::endl;
            ss << "Operations per Second: " << (totalCount * 1000000.0 / totalTime) << std::endl;
            ss << std::endl;
            
            // Performance classification
            if (averageTime < 10.0) {
                ss << "Performance Classification: Excellent (< 10 μs)" << std::endl;
            } else if (averageTime < 100.0) {
                ss << "Performance Classification: Good (< 100 μs)" << std::endl;
            } else if (averageTime < 1000.0) {
                ss << "Performance Classification: Acceptable (< 1000 μs)" << std::endl;
            } else if (averageTime < 10000.0) {
                ss << "Performance Classification: Needs Attention (< 10000 μs)" << std::endl;
            } else {
                ss << "Performance Classification: Critical (> 10000 μs)" << std::endl;
            }
        }
        
        return ss.str();
    }
    
    /**
     * @brief Reset all timing statistics
     */
    void reset() {
        timingStatistics_.clear();
        timingStartTimes_.clear();
    }
    
    /**
     * @brief Get current timestamp in microseconds
     */
    uint64_t getCurrentTimestamp() const {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            now - startTime_);
        return duration.count();
    }
    
    /**
     * @brief Calculate average of timing samples
     */
    double calculateAverage(const std::vector<uint64_t>& samples) const {
        if (samples.empty()) return 0.0;
        uint64_t total = std::accumulate(samples.begin(), samples.end(), 0ULL);
        return static_cast<double>(total) / samples.size();
    }
    
    /**
     * @brief Calculate standard deviation of timing samples
     */
    double calculateStandardDeviation(const std::vector<uint64_t>& samples) const {
        if (samples.size() <= 1) return 0.0;
        
        double mean = calculateAverage(samples);
        double variance = 0.0;
        
        for (uint64_t sample : samples) {
            double diff = static_cast<double>(sample) - mean;
            variance += diff * diff;
        }
        
        return std::sqrt(variance / samples.size());
    }

private:
    std::unordered_map<std::string, std::vector<uint64_t>> timingStatistics_;  // Timing statistics per operation
    std::unordered_map<std::string, uint64_t> timingStartTimes_;              // Start times for manual timing
    std::chrono::high_resolution_clock::time_point startTime_;               // Profiler start time
    static const std::vector<uint64_t> emptyStatistics_;                    // Empty statistics vector
};

// Static member definition
const std::vector<uint64_t> PerformanceProfiler::emptyStatistics_;

} // namespace nlm
