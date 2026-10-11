// Profiler.hpp - Performance profiling for NLM
#pragma once

#include <string>
#include <memory>
#include <chrono>
#include <vector>
#include <map>
#include <unordered_map>
#include <atomic>

namespace nlm {

/**
 * Profiler class - Performance profiling for NLM
 * 
 * @brief Provides comprehensive performance profiling including:
 *        - Performance profiling mode
 *        - Memory usage tracking
 *        - Timing statistics
 *        - Debug modes with detailed output
 *        - Error injection and recovery testing
 *        - Resource monitoring
 * 
 * Features:
 * - Start/stop profiling with timestamps
 * - Collect and aggregate timing statistics
 * - Track memory usage
 * - Generate profiling reports
 * - Support for different profiling modes
 * - Integration with error injection and recovery testing
 * - Real-time performance monitoring
 * 
 * @note Integrates with existing error reporting for validation
 * @see CLIEngine for profiling integration
 * @see AdvancedArgs for profiling options
 */
class Profiler {
public:
    /**
     * @brief Profiler constructor
     * @details Initializes profiler with default settings
     * @post Profiler ready for profiling operations
     */
    Profiler();
    
    /**
     * @brief Profiler destructor
     * @details Cleans up profiler resources
     * @post All resources released
     */
    ~Profiler();
    
    /**
     * @brief Start profiling
     * @param sessionName Name of profiling session
     * @post Profiling started
     * 
     * @details Begins collecting performance metrics:
     * - Wall-clock time
     * - CPU time
     * - Memory usage
     * - System resources
     * 
     * @warning Profiling must be stopped before starting again
     * @see stopProfiling()
     */
    void startProfiling(const std::string& sessionName = "default");
    
    /**
     * @brief Stop profiling
     * @post Profiling stopped and metrics collected
     * 
     * @details Stops collecting performance metrics and stores results:
     * - Finalizes timing measurements
     * - Updates memory usage statistics
     * - Generates summary statistics
     * 
     * @warning Cannot stop profiling if not started
     * @see startProfiling()
     */
    void stopProfiling();
    
    /**
     * @brief Check if profiling is active
     * @return true if profiling active, false otherwise
     * @post Profiling status accessible
     */
    bool isProfilingActive() const { return isActive; }
    
    /**
     * @brief Get profiling session name
     * @return Profiling session name
     * @post Session name accessible
     */
    const std::string& getSessionName() const { return sessionName; }
    
    /**
     * @brief Get elapsed time
     * @return Elapsed time in milliseconds
     * @post Elapsed time accessible
     */
    int64_t getElapsedTime() const;
    
    /**
     * @brief Get total CPU time
     * @return Total CPU time in milliseconds
     * @post CPU time accessible
     */
    int64_t getTotalCPUTime() const { return totalCPUTime; }
    
    /**
     * @brief Get peak memory usage
     * @return Peak memory usage in bytes
     * @post Memory usage accessible
     */
    int64_t getPeakMemoryUsage() const { return peakMemoryUsage; }
    
    /**
     * @brief Get number of operations
     * @return Number of operations
     * @post Operation count accessible
     */
    int64_t getOperationsCount() const { return operationsCount; }
    
    /**
     * @brief Generate profiling report
     * @return Profiling report as string
     * @post Report generated
     * 
     * @details Generates comprehensive profiling report including:
     * - Session information
     * - Timing statistics
     * - Memory usage statistics
     * - Performance metrics
     * - Recommendations
     */
    std::string generateReport() const;
    
    /**
     * @brief Record operation timing
     * @param operationName Name of operation
     * @param duration Duration in milliseconds
     * @post Operation timing recorded
     */
    void recordOperation(const std::string& operationName, int64_t duration);
    
    /**
     * @brief Record memory usage
     * @param memoryUsage Memory usage in bytes
     * @post Memory usage recorded
     */
    void recordMemoryUsage(int64_t memoryUsage);
    
    /**
     * @brief Increment operations count
     * @param count Number of operations to add
     * @post Operations count updated
     */
    void incrementOperations(int64_t count = 1) { operationsCount += count; }
    
    /**
     * @brief Start error injection simulation
     * @post Error injection started
     * 
     * @details Begins simulating errors for testing error recovery:
     * - Injects errors at specified rates
     * - Monitors error handling
     * - Collects recovery statistics
     */
    void startErrorInjectionSimulation();
    
    /**
     * @brief Stop error injection simulation
     * @post Error injection stopped
     */
    void stopErrorInjectionSimulation();
    
    /**
     * @brief Check if error injection is active
     * @return true if error injection active, false otherwise
     * @post Error injection status accessible
     */
    bool isErrorInjectionActive() const { return errorInjectionActive; }

private:
    bool isActive;
    std::string sessionName;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
    int64_t totalCPUTime;
    int64_t peakMemoryUsage;
    int64_t operationsCount;
    
    // Statistics tracking
    std::map<std::string, int64_t> operationTimes;
    std::vector<int64_t> memoryUsages;
    
    // Error injection simulation
    bool errorInjectionActive;
    std::chrono::steady_clock::time_point errorStartTime;
    int64_t errorCount;
    
    void updateCPUTime();
    double calculateOperationsPerSecond() const;
    std::string formatTime(int64_t milliseconds) const;
    std::string formatMemory(int64_t bytes) const;
    
    // Suppress warnings about unused private members
    Profiler(const Profiler&) = delete;
    Profiler& operator=(const Profiler&) = delete;
};

} // namespace nlm