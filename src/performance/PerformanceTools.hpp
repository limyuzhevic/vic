#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <thread>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include "../core/Logger/Logger.hpp"

namespace nlm {

/**
 * @file Profiler.hpp
 * 
 * Performance profiling system for NLM.
 * 
 * This file provides the Profiler class, which offers comprehensive
 * performance profiling capabilities for NLM applications. It captures
 * detailed performance metrics, analyzes bottlenecks, and provides
 * insights for performance optimization.
 * 
 * Key features:
 * - Function-level performance profiling
 * - Memory usage tracking
 * - CPU time measurement
 * - GPU performance monitoring
 * - Profile data export
 * - Comparison between profiles
 * - Performance regression detection
 * - Automated optimization suggestions
 * - Real-time performance monitoring
 * - Custom profiler hooks
 * - Thread-safe profiling
 * - Detailed timing analysis
 * 
 * @note Integrates with NLM's logging system
 * @see PerformanceMetrics - Data structure for performance data
 * @see PerformanceReport - Formatted performance reports
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */
class Profiler {
public:
    Profiler();
    ~Profiler();
    
    // Disable copying - PImpl idiom
    Profiler(const Profiler&) = delete;
    Profiler& operator=(const Profiler&) = delete;
    Profiler(Profiler&&) noexcept;
    Profiler& operator=(Profiler&&) noexcept;
    
    /**
     * @brief Start profiling a function
     * 
     * @param functionName Name of function to profile
     * @return Profile ID for the started profile
     */
    std::string startProfile(const std::string& functionName);
    
    /**
     * @brief Stop profiling a function
     * 
     * @param profileId Profile ID to stop
     */
    void stopProfile(const std::string& profileId);
    
    /**
     * @brief Start a scoped profiling operation
     * 
     * @param scopeName Scope name
     * @return Scoped profiler
     */
    std::unique_ptr<Profiler> startScopedProfile(const std::string& scopeName);
    
    /**
     * @brief Get profiling data
     * 
     * @param profileId Profile ID
     * @return Profiling data
     */
    std::unordered_map<std::string, double> getProfileData(const std::string& profileId) const;
    
    /**
     * @brief Get all profiling data
     * 
     * @return Map of profile ID to data
     */
    std::unordered_map<std::string, std::unordered_map<std::string, double>> getAllProfileData() const;
    
    /**
     * @brief Clear all profiling data
     */
    void clearProfiles();
    
    /**
     * @brief Export profiling data
     * 
     * @param format Export format
     * @param filepath Output file path
     * @return true if export successful
     */
    bool exportProfiles(const std::string& format, const std::string& filepath);
    
    /**
     * @brief Get profiling statistics
     * 
     * @return Profiling statistics
     */
    std::unordered_map<std::string, double> getProfilingStats() const;
    
    /**
     * @brief Check if profiling is active
     * 
     * @return true if profiling is active
     */
    bool isProfiling() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @file PerformanceProfiler.hpp
 * 
 * Advanced performance profiler with multiple measurement modes.
 * 
 * This file provides the PerformanceProfiler class, which offers
 * advanced performance profiling with multiple measurement modes,
 * automated analysis, and integration with NLM's visualization system.
 * 
 * Key features:
 * - Multi-threaded profiling
 * - Automated bottleneck detection
 * - Performance trend analysis
 * - Memory usage tracking
 * - CPU performance analysis
 * - GPU profiling integration
 * - Network performance monitoring
 * - I/O performance measurement
 * - Profile comparison and diffing
 * - Performance regression detection
 * - Optimization recommendations
 * - Real-time performance monitoring
 * - Custom measurement hooks
 * - Data export and reporting
 * 
 * @note Enhanced profiling capabilities
 * @see Profiler - Base profiler class
 * @see PerformanceMetrics - Performance data structure
 * @see RealTimeMonitor - Integration for monitoring
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */
class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    // Disable copying - PImpl idiom
    PerformanceProfiler(const PerformanceProfiler&) = delete;
    PerformanceProfiler& operator=(const PerformanceProfiler&) = delete;
    PerformanceProfiler(PerformanceProfiler&&) noexcept;
    PerformanceProfiler& operator=(PerformanceProfiler&&) noexcept;
    
    /**
     * @brief Initialize the performance profiler
     * 
     * @param brain Reference to brain system for system monitoring
     * @return true if initialization successful
     */
    bool initialize(const Brain& brain);
    
    /**
     * @brief Start comprehensive performance profiling
     * 
     * @return true if profiling started
     */
    bool startProfiling();
    
    /**
     * @brief Stop performance profiling
     * 
     * @return true if profiling stopped
     */
    bool stopProfiling();
    
    /**
     * @brief Get current performance metrics
     * 
     * @return Current performance metrics
     */
    std::unordered_map<std::string, double> getCurrentMetrics() const;
    
    /**
     * @brief Get performance history
     * 
     * @param count Number of data points to retrieve
     * @return Performance history
     */
    std::vector<std::pair<double, std::unordered_map<std::string, double>>> getPerformanceHistory(size_t count = 100) const;
    
    /**
     * @brief Get performance bottlenecks
     * 
     * @return List of performance bottlenecks
     */
    std::vector<std::string> getPerformanceBottlenecks() const;
    
    /**
     * @brief Get performance optimization suggestions
     * 
     * @return Optimization suggestions
     */
    std::vector<std::string> getOptimizationSuggestions() const;
    
    /**
     * @brief Compare two performance profiles
     * 
     * @param profile1 First profile
     * @param profile2 Second profile
     * @return Comparison results
     */
    std::unordered_map<std::string, double> compareProfiles(const std::string& profile1, const std::string& profile2) const;
    
    /**
     * @brief Export performance data
     * 
     * @param format Export format
     * @param filepath Output file path
     * @return true if export successful
     */
    bool exportPerformanceData(const std::string& format, const std::string& filepath);
    
    /**
     * @brief Check if profiling is active
     * 
     * @return true if profiling is active
     */
    bool isProfiling() const;
    
    /**
     * @brief Set callback for performance alerts
     * 
     * @param callback Function to call on performance alert
     */
    void setAlertCallback(std::function<void(const std::string&, double)> callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @file ResourceMonitor.hpp
 * 
 * System resource monitoring for NLM.
 * 
 * This file provides the ResourceMonitor class, which offers
 * comprehensive monitoring of system resources including CPU, memory,
 * disk I/O, network usage, and GPU resources. This is essential for
 * understanding system resource consumption and identifying resource
 * bottlenecks in large-scale neural simulations.
 * 
 * Key features:
 * - CPU usage monitoring
 * - Memory usage tracking
 * - Disk I/O monitoring
 * * Network traffic monitoring
 * - GPU resource monitoring
 * - Process resource tracking
 * - Resource usage history
 * - Resource alerts and thresholds
 * - Resource optimization suggestions
 * - Real-time resource monitoring
 * - Resource bottleneck detection
 * - Resource comparison
 * - Export resource data
 * - Resource usage analysis
 * - Integration with performance tools
 * 
 * @note Integrates with system monitoring APIs
 * @see PerformanceProfiler - For integration
 * @see VisualizationEngine - For resource visualization
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */
class ResourceMonitor {
public:
    ResourceMonitor();
    ~ResourceMonitor();
    
    // Disable copying - PImpl idiom
    ResourceMonitor(const ResourceMonitor&) = delete;
    ResourceMonitor& operator=(const ResourceMonitor&) = delete;
    ResourceMonitor(ResourceMonitor&&) noexcept;
    ResourceMonitor& operator=(ResourceMonitor&&) noexcept;
    
    /**
     * @brief Initialize the resource monitor
     * 
     * @return true if initialization successful
     */
    bool initialize();
    
    /**
     * @brief Start resource monitoring
     * 
     * @return true if monitoring started
     */
    bool startMonitoring();
    
    /**
     * @brief Stop resource monitoring
     * 
     * @return true if monitoring stopped
     */
    bool stopMonitoring();
    
    /**
     * @brief Get current resource usage
     * 
     * @return Current resource usage
     */
    std::unordered_map<std::string, double> getCurrentResourceUsage() const;
    
    /**
     * @brief Get resource usage history
     * 
     * @param count Number of data points to retrieve
     * @return Resource usage history
     */
    std::vector<std::pair<double, std::unordered_map<std::string, double>>> getResourceUsageHistory(size_t count = 100) const;
    
    /**
     * @brief Get resource bottlenecks
     * 
     * @return List of resource bottlenecks
     */
    std::vector<std::string> getResourceBottlenecks() const;
    
    /**
     * @brief Get resource alerts
     * 
     * @return List of resource alerts
     */
    std::vector<std::string> getResourceAlerts() const;
    
    /**
     * @brief Set resource alert threshold
     * 
     * @param resourceName Resource name
     * @param threshold Threshold value
     * @param level Alert level
     */
    void setResourceAlert(const std::string& resourceName, double threshold, int level = 1);
    
    /**
     * @brief Export resource data
     * 
     * @param format Export format
     * @param filepath Output file path
     * @return true if export successful
     */
    bool exportResourceData(const std::string& format, const std::string& filepath);
    
    /**
     * @brief Check if monitoring is active
     * 
     * @return true if monitoring is active
     */
    bool isMonitoring() const;
    
    /**
     * @brief Set callback for resource alerts
     * 
     * @param callback Function to call on resource alert
     */
    void setAlertCallback(std::function<void(const std::string&, double)> callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm