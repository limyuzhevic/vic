#pragma once

#include "VisualizationEngine.hpp"
#include <chrono>
#include <deque>
#include <numeric>

namespace nlm {

/**
 * @file MetricsVisualizer.hpp
 * 
 * Performance metrics visualization for system monitoring.
 * 
 * This file provides the MetricsVisualizer class, which renders
 * performance metrics and system statistics in real-time. This visualization
 * is essential for monitoring system health, identifying performance
 * bottlenecks, and ensuring optimal operation of the NLM system.
 * 
 * Key features:
 * - Real-time performance metrics monitoring
 * - Multiple metric types (CPU, memory, I/O, network)
 * - Interactive metric selection and filtering
 * - Historical metrics history with customizable time windows
 * - Alert thresholds and warning visualization
 * - Performance comparison charts
 * - Resource utilization heatmaps
 * - System status indicators
 * - Custom metric definitions
 * - Exportable performance reports
 * 
 * @note This visualizer works with performance metrics data
 * @see VisualizationEngine - Main visualization framework
 * @see PerformanceProfiler - Source of performance metrics
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */
class MetricsVisualizer : public Visualizer {
public:
    MetricsVisualizer();
    ~MetricsVisualizer() override;
    
    // Disable copying - PImpl idiom
    MetricsVisualizer(const MetricsVisualizer&) = delete;
    MetricsVisualizer& operator=(const MetricsVisualizer&) = delete;
    MetricsVisualizer(MetricsVisualizer&&) noexcept;
    MetricsVisualizer& operator=(MetricsVisualizer&&) noexcept;
    
    /**
     * @brief Initialize the metrics visualizer
     * 
     * @param config Visualization configuration
     * @return true if initialization successful
     */
    bool initialize(const VisualizationConfig& config) override;
    
    /**
     * @brief Update with new metrics data
     * 
     * @param data Metrics data from visualization data container
     * @post Metrics updated and ready for rendering
     */
    void update(const VisualizationData& data) override;
    
    /**
     * @brief Render metrics visualization
     * 
     * @param output Whether to output to display/file
     */
    void render(bool output = true) override;
    
    /**
     * @brief Configure visualizer parameters
     * 
     * @param config New configuration
     */
    void configure(const VisualizationConfig& config) override;
    
    /**
     * @brief Get visualizer type
     * 
     * @return VisualizationType::SimulationResults
     */
    VisualizationType getType() const override;
    
    /**
     * @brief Get visualizer name
     * 
     * @return "MetricsVisualizer"
     */
    std::string getName() const override;
    
    /**
     * @brief Add metric to display
     * 
     * @param name Metric name
     * @param unit Metric unit
     */
    void addMetric(const std::string& name, const std::string& unit = "");
    
    /**
     * @brief Set alert threshold for a metric
     * 
     * @param metricName Metric name
     * @param threshold Value that triggers alert
     * @param level Alert level (0=warning, 1=error, 2=critical)
     */
    void setMetricAlert(const std::string& metricName, double threshold, int level = 1);
    
    /**
     * @brief Get current metric values
     * 
     * @return Current metric values
     */
    std::unordered_map<std::string, double> getCurrentMetrics() const;
    
    /**
     * @brief Get metric history
     * 
     * @param metricName Metric name
     * @return Historical data points
     */
    std::vector<std::pair<double, double>> getMetricHistory(const std::string& metricName) const;
    
    /**
     * @brief Export metrics data
     * 
     * @param format Export format
     * @param filepath Output file path
     * @return true if export successful
     */
    bool exportData(const std::string& format, const std::string& filepath) override;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @file TimeSeriesVisualizer.hpp
 * 
 * Time series visualization for temporal data analysis.
 * 
 * This file provides the TimeSeriesVisualizer class, which renders
 * temporal data as time series plots, enabling the analysis of patterns,
 * trends, and relationships over time. This visualization is essential
 * for analyzing neural activity patterns, system performance trends,
 * and experimental results over time.
 * 
 * Key features:
 * - Multi-series time series plotting
 * - Real-time time series updates
 * - Interactive time window selection
 * - Annotation and event markers
 * - Statistical overlays (mean, std dev, confidence intervals)
 * - Smoothing and filtering capabilities
 * - Multiple y-axis support
 * - Time aggregation (seconds, minutes, hours)
 * - Export to various time series formats
 * - Interactive data point inspection
 * - Cursor time tracking
 * 
 * @note This visualizer works with time series data from various sources
 * @see VisualizationEngine - Main visualization framework
 * @see NeuralActivityVisualizers - Source of neural time series
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */
class TimeSeriesVisualizer : public Visualizer {
public:
    TimeSeriesVisualizer();
    ~TimeSeriesVisualizer() override;
    
    // Disable copying - PImpl idiom
    TimeSeriesVisualizer(const TimeSeriesVisualizer&) = delete;
    TimeSeriesVisualizer& operator=(const TimeSeriesVisualizer&) = delete;
    TimeSeriesVisualizer(TimeSeriesVisualizer&&) noexcept;
    TimeSeriesVisualizer& operator=(TimeSeriesVisualizer&&) noexcept;
    
    /**
     * @brief Initialize the time series visualizer
     * 
     * @param config Visualization configuration
     * @return true if initialization successful
     */
    bool initialize(const VisualizationConfig& config) override;
    
    /**
     * @brief Update with new time series data
     * 
     * @param data Time series data from visualization data container
     * @post Time series updated and ready for rendering
     */
    void update(const VisualizationData& data) override;
    
    /**
     * @brief Render time series plots
     * 
     * @param output Whether to output to display/file
     */
    void render(bool output = true) override;
    
    /**
     * @brief Configure visualizer parameters
     * 
     * @param config New configuration
     */
    void configure(const VisualizationConfig& config) override;
    
    /**
     * @brief Get visualizer type
     * 
     * @return VisualizationType::SimulationResults
     */
    VisualizationType getType() const override;
    
    /**
     * @brief Get visualizer name
     * 
     * @return "TimeSeriesVisualizer"
     */
    std::string getName() const override;
    
    /**
     * @brief Add time series to display
     * 
     * @param name Series name
     * @param unit Series unit
     */
    void addTimeSeries(const std::string& name, const std::string& unit = "");
    
    /**
     * @brief Set time window for display
     * 
     * @param startTime Window start time
     * @param endTime Window end time
     */
    void setTimeWindow(double startTime, double endTime);
    
    /**
     * @brief Get current time window
     * 
     * @return Current time window (start, end)
     */
    std::pair<double, double> getTimeWindow() const;
    
    /**
     * @brief Export time series data
     * 
     * @param format Export format
     * @param filepath Output file path
     * @return true if export successful
     */
    bool exportData(const std::string& format, const std::string& filepath) override;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @file ResultsVisualizer.hpp
 * 
 * Experimental results visualization for scientific analysis.
 * 
 * This file provides the ResultsVisualizer class, which renders
 * experimental results and findings with comprehensive analysis
 * and visualization capabilities. This visualization is essential
 * for presenting research findings, validating experimental hypotheses,
 * and ensuring the scientific rigor of NLM experiments.
 * 
 * Key features:
 * - Multi-panel result displays
 * - Statistical analysis visualization
 * * Error bar visualization
 * - Result comparison charts
 * - Hypothesis validation displays
 * - Confidence interval visualization
 * - Effect size representation
 * - Power analysis visualization
 * - Publication-quality outputs
 * - Customizable result layouts
 * - Interactive result exploration
 * - Result grouping and categorization
 * 
 * @note This visualizer works with experimental result data
 * @see VisualizationEngine - Main visualization framework
 * @see Experiments - Source of experimental results
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */
class ResultsVisualizer : public Visualizer {
public:
    ResultsVisualizer();
    ~ResultsVisualizer() override;
    
    // Disable copying - PImpl idiom
    ResultsVisualizer(const ResultsVisualizer&) = delete;
    ResultsVisualizer& operator=(const ResultsVisualizer&) = delete;
    ResultsVisualizer(ResultsVisualizer&&) noexcept;
    ResultsVisualizer& operator=(ResultsVisualizer&&) noexcept;
    
    /**
     * @brief Initialize the results visualizer
     * 
     * @param config Visualization configuration
     * @return true if initialization successful
     */
    bool initialize(const VisualizationConfig& config) override;
    
    /**
     * @brief Update with new results data
     * 
     * @param data Results data from visualization data container
     * @post Results updated and ready for rendering
     */
    void update(const VisualizationData& data) override;
    
    /**
     * @brief Render results visualization
     * 
     * @param output Whether to output to display/file
     */
    void render(bool output = true) override;
    
    /**
     * @brief Configure visualizer parameters
     * 
     * @param config New configuration
     */
    void configure(const VisualizationConfig& config) override;
    
    /**
     * @brief Get visualizer type
     * 
     * @return VisualizationType::SimulationResults
     */
    VisualizationType getType() const override;
    
    /**
     * @brief Get visualizer name
     * 
     * @return "ResultsVisualizer"
     */
    std::string getName() const override;
    
    /**
     * @brief Add experimental result to display
     * 
     * @param resultName Result identifier
     * @param metricName Metric name
     * @param value Result value
     * @param error Error/standard deviation
     */
    void addResult(const std::string& resultName, const std::string& metricName,
                  double value, double error = 0.0);
    
    /**
     * @brief Set result group
     * 
     * @param groupName Result group name
     */
    void setResultGroup(const std::string& groupName);
    
    /**
     * @brief Get results statistics
     * 
     * @return Results statistics
     */
    std::unordered_map<std::string, double> getResultsStatistics() const;
    
    /**
     * @brief Export results data
     * 
     * @param format Export format
     * @param filepath Output file path
     * @return true if export successful
     */
    bool exportData(const std::string& format, const std::string& filepath) override;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm