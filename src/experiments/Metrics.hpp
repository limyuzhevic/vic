// Metrics System for NLM (Neural Learning Machine)
//
// This file defines the Metrics class for collecting, analyzing, and reporting
// experimental data from brain simulations. The metrics system provides
// comprehensive statistical analysis of experimental results.
//
// The metrics system in NLM is used to:
// - Collect quantitative data from brain simulations
// - Calculate statistical summaries (mean, standard deviation, min/max)
// - Analyze temporal and spatial patterns in neural activity
// - Generate human-readable reports for experimental results
// - Support comparison between different experimental conditions
//
// Metrics in NLM can track:
// - Neural activity statistics (firing rates, spike counts)
// - Network properties (neuron counts, synapse counts)
// - System performance (efficiency, convergence)
// - Learning progress (weight changes, plasticity)
// - Memory performance (retention, recall accuracy)
// - Neuromodulation effects (dopamine levels, curiosity)
// - Developmental trajectories (plasticity over time)
//
// The metrics system is used by all experiment phases:
// - Phase 3: World interaction experiments
// - Phase 4: Cognition experiments
// - Phase 5: Scaling experiments
// - Phase 6: Integration experiments

#pragma once

#include <vector>
#include <string>
#include <map>

namespace nlm {

/**
 * @brief Metrics collection and analysis system
 * 
 * The Metrics class provides a comprehensive framework for collecting,
 * storing, and analyzing experimental data from brain simulations.
 * It supports multiple metrics per experimental condition, statistical
 * analysis, and report generation.
 * 
 * The metrics system is designed to be:
 * - Flexible: Support multiple metric types and names
 * - Scalable: Handle large datasets from long simulations
 * - Analytical: Provide statistical summaries and insights
 * - Reportable: Generate human-readable summaries
 * - Efficient: Optimized for real-time and post-simulation analysis
 * 
 * @note This class uses a simple pointer-based implementation for
 * performance reasons, with statistics calculated on demand.
 */
class Metrics {
public:
    /**
     * @brief Default constructor
     * 
     * Creates an empty metrics system with no recorded data.
     */
    Metrics();
    
    /**
     * @brief Destructor
     */
    ~Metrics();
    
    /**
     * @brief Disable copying
     */
    Metrics(const Metrics&) = delete;
    
    /**
     * @brief Disable assignment
     */
    Metrics& operator=(const Metrics&) = delete;
    
    /**
     * @brief Record a metric value
     * 
     * Records a single value for a named metric. The same metric name can
     * be recorded multiple times to collect a time series or sample data.
     * 
     * @param name Name of the metric (e.g., "firing_rate", "weight_change")
     * @param value Numeric value to record
     * 
     * @note Multiple values can be recorded under the same name to create
     * time series or sample datasets for statistical analysis.
     */
    void record(const std::string& name, double value);
    
    /**
     * @brief Get metric values
     * 
     * Retrieves all recorded values for a specific metric.
     * 
     * @param name Name of the metric
     * @return const std::vector<double>& Reference to the vector of metric values
     * 
     * @note The returned vector is ordered chronologically by recording time.
     */
    const std::vector<double>& getValues(const std::string& name) const;
    
    /**
     * @brief Get mean value
     * 
     * Calculates the arithmetic mean of all recorded values for a metric.
     * 
     * @param name Name of the metric
     * @return double Mean value, or NaN if no data available
     */
    double getMean(const std::string& name) const;
    
    /**
     * @brief Get standard deviation
     * 
     * Calculates the standard deviation of all recorded values for a metric,
     * providing a measure of variability in the data.
     * 
     * @param name Name of the metric
     * @return double Standard deviation, or NaN if insufficient data (less than 2 samples)
     */
    double getStdDev(const std::string& name) const;
    
    /**
     * @brief Get minimum value
     * 
     * Finds the minimum value in the recorded data for a metric.
     * 
     * @param name Name of the metric
     * @return double Minimum value, or NaN if no data available
     */
    double getMin(const std::string& name) const;
    
    /**
     * @brief Get maximum value
     * 
     * Finds the maximum value in the recorded data for a metric.
     * 
     * @param name Name of the metric
     * @return double Maximum value, or NaN if no data available
     */
    double getMax(const std::string& name) const;
    
    /**
     * @brief Get all metric names
     * 
     * @return std::vector<std::string> List of all recorded metric names
     */
    std::vector<std::string> getMetricNames() const;
    
    /**
     * @brief Clear all metrics
     * 
     * Removes all recorded metric data. This is useful for reusing the
     * metrics object for different experimental conditions or datasets.
     */
    void clear();
    
    /**
     * @brief Generate summary report
     * 
     * Creates a human-readable summary of all recorded metrics and their
     * statistics. The report includes:
     * - Total number of metrics
     * - Names and values of all metrics
     * - Statistical summaries where applicable
     * - Timestamp information (if available)
     * 
     * @return std::string Formatted summary report
     */
    std::string summary() const;
    
private:
    /**
     * @brief Forward declaration of implementation
     * 
     * Implementation details are hidden in this private structure.
     */
    struct Impl;
    /**
     * @brief Pointer to implementation
     * 
     * Uses a simple pointer-based implementation for performance.
     */
    Impl* pImpl;
};

} // namespace nlm