#pragma once

#include <vector>
#include <string>
#include <map>

namespace nlm {

/**
 * @file Metrics.hpp
 * 
 * Metrics collection and analysis system for NLM
 * 
 * This file provides the metrics collection and analysis system for NLM, implementing:
 * - Metrics collection
 * - Metrics analysis
 * - Metric statistics
 * - Metrics reporting
 * - Metrics management
 * 
 * The metrics collection and analysis system in NLM:
 * - **Metrics**: Metrics collection and analysis
 * - **Metrics collection**: Collects metrics
 * - **Metrics analysis**: Analyzes metrics
 * - **Metric statistics**: Provides metric statistics
 * - **Metrics reporting**: Reports metrics
 * - **Metrics management**: Manages metrics
 * 
 * Key features:
 * - **Metrics collection**: Collects metrics
 * - **Metrics analysis**: Analyzes metrics
 * - **Metric statistics**: Provides metric statistics
 * - **Metrics reporting**: Reports metrics
 * - **Metrics management**: Manages metrics
 * 
 * Metrics characteristics in NLM:
 * - **Metrics collection**: Collects metrics
 * - **Metrics analysis**: Analyzes metrics
 * - **Metric statistics**: Provides metric statistics
 * - **Metrics reporting**: Reports metrics
 * - **Metrics management**: Manages metrics
 * 
 * @note Critical for metrics collection
 * @note Used extensively in metrics systems
 * @note Essential for metrics analysis
 * @see Metrics.cpp - For implementation details
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */

/**
 * @class Metrics
 * 
 * Neural implementation of metrics collection and analysis.
 * 
 * This class implements the metrics collection and analysis system, providing
 * metrics collection and analysis. It is essential for:
 * - **Metrics collection**: Collects metrics
 * - **Metrics analysis**: Analyzes metrics
 * - **Metric statistics**: Provides metric statistics
 * - **Metrics reporting**: Reports metrics
 * - **Metrics management**: Manages metrics
 * 
 * Metrics characteristics in NLM:
 * - **Metrics collection**: Collects metrics
 * - **Metrics analysis**: Analyzes metrics
 * - **Metric statistics**: Provides metric statistics
 * - **Metrics reporting**: Reports metrics
 * - **Metrics management**: Manages metrics
 * 
 * Key components:
 * - **Metric recording**: Records metric values
 * - **Metric retrieval**: Retrieves metric values
 * - **Metric statistics**: Provides metric statistics
 * - **Metrics listing**: Lists metric names
 * - **Metrics clearing**: Clears all metrics
 * - **Metrics reporting**: Generates summary report
 * 
 * @note Critical for metrics collection
 * @see Metrics.cpp - For implementation details
 * 
 * @author NLM Development Team
 */
class Metrics {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a default metrics instance with default parameters.
     * The metrics instance is initialized with default values for
     * all metrics fields.
     */
    Metrics();
    
    /**
     * @brief Destructor
     * 
     * Cleans up metrics resources and performs any necessary
     * cleanup operations. The metrics instance is left in a valid but
     * non-operational state after destruction.
     */
    ~Metrics();
    
    /**
     * @brief Record a metric value
     * 
     * Records a metric value. This is used for:
     * - Metric recording
     * - Metric management
     * - Metric processing
     * - Metric control
     */
    void record(const std::string& name, double value);
    
    /**
     * @brief Get metric values
     * 
     * Returns metric values. This is used for:
     * - Metric value access
     * - Metric value management
     * - Metric value processing
     * - Metric value control
     */
    const std::vector<double>& getValues(const std::string& name) const;
    
    /**
     * @brief Get mean
     * 
     * Returns the mean. This is used for:
     * - Mean access
     * - Mean management
     * - Mean processing
     * - Mean control
     */
    double getMean(const std::string& name) const;
    
    /**
     * @brief Get standard deviation
     * 
     * Returns the standard deviation. This is used for:
     * - Standard deviation access
     * - Standard deviation management
     * - Standard deviation processing
     * - Standard deviation control
     */
    double getStdDev(const std::string& name) const;
    
    /**
     * @brief Get minimum
     * 
     * Returns the minimum. This is used for:
     * - Minimum access
     * - Minimum management
     * - Minimum processing
     * - Minimum control
     */
    double getMin(const std::string& name) const;
    
    /**
     * @brief Get maximum
     * 
     * Returns the maximum. This is used for:
     * - Maximum access
     * - Maximum management
     * - Maximum processing
     * - Maximum control
     */
    double getMax(const std::string& name) const;
    
    /**
     * @brief Get all metric names
     * 
     * Returns all metric names. This is used for:
     * - Metric name access
     * - Metric name management
     * - Metric name processing
     * - Metric name control
     */
    std::vector<std::string> getMetricNames() const;
    
    /**
     * @brief Clear all metrics
     * 
     * Clears all metrics. This is used for:
     * - Metric clearing
     * - Metric management
     * - Metric processing
     * - Metric control
     */
    void clear();
    
    /**
     * @brief Generate summary report
     * 
     * Generates a summary report. This is used for:
     * - Report generation
     * - Report management
     * - Report processing
     * - Report control
     */
    std::string summary() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
