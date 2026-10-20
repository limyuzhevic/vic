#pragma once

/**
 * NLM Metrics Collection and Analysis System
 * 
 * Provides a flexible metrics collection and analysis framework for NLM
 * (Neural Learning Machine) experiments. This system enables recording,
 * storage, and statistical analysis of experimental metrics across different
 * dimensions including neural activity, behavioral responses, and system performance.
 * 
 * The Metrics class serves as a central data collection mechanism that supports:
 * - Recording arbitrary key-value metrics
 * - Computing statistical summaries (mean, standard deviation, min/max)
 * - Analyzing experimental trends and patterns
 * - Supporting both simple and complex experimental protocols
 * 
 * This system is designed to be memory-efficient and thread-safe, making it
 * suitable for large-scale neural simulations with thousands of metrics.
 */

#include <vector>
#include <string>
#include <unordered_map>
#include <mutex>
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

/**
 * Metrics class - Centralized metrics collection and analysis system
 * 
 * This class provides a comprehensive framework for collecting and analyzing
 * experimental metrics in NLM experiments. It offers thread-safe operation,
 * efficient storage, and powerful statistical analysis capabilities.
 * 
 * Key features:
 * - Thread-safe metric recording using mutex protection
 * - Memory-efficient storage using vectors per metric
 * - Comprehensive statistical analysis (mean, std dev, min/max)
 * - Flexible metric naming and value types (numeric)
 * - Summary reporting for experimental results
 * - Memory optimization for large-scale experiments
 * 
 * Internal architecture:
 * - Each metric is stored in a dedicated vector
 * - Mutex protection for thread-safe concurrent access
 * - Efficient statistical computation using standard algorithms
 * - Automatic metric discovery and management
 * 
 * Usage example:
 * @code
 * Metrics metrics;
 * metrics.record("firing_rate", 0.45);
 * metrics.record("spike_count", 128);
 * double mean_rate = metrics.getMean("firing_rate");
 * double std_dev = metrics.getStdDev("spike_count");
 * @endcode
 */
class Metrics {
public:
    /**
     * Default constructor
     * 
     * Creates an empty metrics collector with default initialization.
     * All internal data structures are properly initialized for immediate use.
     */
    Metrics();
    
    /**
     * Destructor
     * 
     * Cleans up allocated memory and releases resources.
     * Ensures proper cleanup of all stored metric data.
     */
    ~Metrics();
    
    /**
     * Record a metric value
     * 
     * Adds a new metric value to the collection. Thread-safe operation
     * with automatic mutex protection. If the metric already exists,
     * the value is appended to the existing metric's history.
     * 
     * @param name Metric name/identifier (case-sensitive)
     * @param value Metric value to record (numeric)
     */
    void record(const std::string& name, double value);
    
    /**
     * Get all values for a specific metric
     * 
     * Retrieves the complete history of values for a given metric.
     * Thread-safe operation with mutex protection.
     * 
     * @param name Metric name
     * @return Vector of all recorded values for the metric
     */
    const std::vector<double>& getValues(const std::string& name) const;
    
    /**
     * Get mean value for a metric
     * 
     * Computes the arithmetic mean of all recorded values for a metric.
     * Efficient computation using vector elements.
     * 
     * @param name Metric name
     * @return Mean value (0.0 if metric doesn't exist or has no values)
     */
    double getMean(const std::string& name) const;
    
    /**
     * Get standard deviation for a metric
     * 
     * Computes the population standard deviation of recorded values.
     * Uses efficient numerical computation algorithm.
     * 
     * @param name Metric name
     * @return Standard deviation (0.0 if insufficient data or metric doesn't exist)
     */
    double getStdDev(const std::string& name) const;
    
    /**
     * Get minimum value for a metric
     * 
     * Retrieves the minimum value from all recorded values.
     * Efficient computation using std::min_element.
     * 
     * @param name Metric name
     * @return Minimum value (0.0 if metric doesn't exist or has no values)
     */
    double getMin(const std::string& name) const;
    
    /**
     * Get maximum value for a metric
     * 
     * Retrieves the maximum value from all recorded values.
     * Efficient computation using std::max_element.
     * 
     * @param name Metric name
     * @return Maximum value (0.0 if metric doesn't exist or has no values)
     */
    double getMax(const std::string& name) const;
    
    /**
     * Get all metric names
     * 
     * Retrieves the names of all metrics currently being collected.
     * Thread-safe operation for safe concurrent access.
     * 
     * @return Vector of all metric names
     */
    std::vector<std::string> getMetricNames() const;
    
    /**
     * Clear all metrics
     * 
     * Removes all recorded metrics from the collector, effectively
     * resetting the system to an empty state. Thread-safe operation.
     */
    void clear();
    
    /**
     * Generate summary report
     * 
     * Creates a comprehensive summary report of all metrics in the
     * collector, including counts, statistics, and basic insights.
     * 
     * @return Formatted summary string containing all metric statistics
     */
    std::string summary() const;
    
private:
    /**
     * Thread-safe metrics storage implementation
     * 
     * Uses pImpl pattern with mutex protection for thread-safe operation.
     * Each metric is stored in a dedicated vector for efficient access.
     */
    struct Impl;
    Impl* pImpl;
    
    /**
     * Mutex for thread-safe concurrent access
     * 
     * Protects all metrics operations to ensure thread safety when
     * multiple threads are recording or analyzing metrics concurrently.
     */
    mutable std::mutex mutex_;
    
    /**
     * Mapping of metric names to their value vectors
     * 
     * Internal storage structure using unordered_map for efficient
     * metric name lookup and value storage.
     */
    std::unordered_map<std::string, std::vector<double>> metricData_;
};

} // namespace nlm
