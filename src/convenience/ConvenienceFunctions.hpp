#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../analysis/NeuralVisualization.hpp"
#include "../analysis/NeuralActivityAnalyzer.hpp"
#include "../analysis/TimeSeriesAnalyzer.hpp"
#include "../analysis/PerformanceMetrics.hpp"
#include "../experiments/ExperimentMetrics.hpp"

namespace nlm {

/**
 * @brief Factory function for creating visualization objects
 * 
 * Creates a NeuralVisualization object with the specified configuration.
 * This is a convenience function for quickly setting up visualization capabilities.
 * 
 * @param brain Shared pointer to the brain instance to visualize
 * @param outputFormat Output format for visualizations (default: PNG)
 * @return NeuralVisualization object
 */
NeuralVisualization createVisualizer(
    std::shared_ptr<Brain> brain,
    NeuralVisualization::OutputFormat outputFormat = NeuralVisualization::OutputFormat::PNG
);

/**
 * @brief Quick network analysis function
 * 
 * Performs a comprehensive network analysis using NeuralActivityAnalyzer.
 * This is a convenience function that runs a full analysis with default parameters.
 * 
 * @param brain Shared pointer to the brain instance to analyze
 * @param analysisType Type of analysis to perform ("full", "basic", "detailed")
 * @return NeuralActivityAnalyzer object with analysis results
 */
NeuralActivityAnalyzer analyzeNetwork(
    std::shared_ptr<Brain> brain,
    const std::string& analysisType = "full"
);

/**
 * @brief Performance report generation function
 * 
 * Generates a comprehensive performance report for the neural network simulation.
 * This function analyzes performance metrics and generates detailed reports.
 * 
 * @param brain Shared pointer to the brain instance for analysis
 * @param metricsConfig Performance metrics configuration
 * @return PerformanceMetrics object with analysis results
 */
PerformanceMetrics generatePerformanceReport(
    std::shared_ptr<Brain> brain,
    const PerformanceMetrics::MetricsConfig& metricsConfig = PerformanceMetrics::MetricsConfig()
);

/**
 * @brief Create time series analyzer from experimental metrics
 * 
 * Creates a TimeSeriesAnalyzer from experimental metrics data, extracting
 * time series information from experiment records.
 * 
 * @param metrics Experimental metrics containing time series data
 * @param samplingRate Sampling rate for time series analysis
 * @return TimeSeriesAnalyzer object
 */
TimeSeriesAnalyzer createTimeSeriesAnalyzer(
    const ExperimentMetrics& metrics,
    float samplingRate = 1000.0f
);

/**
 * @brief Visualize experimental results
 * 
 * Visualizes experimental results including metrics, performance, and analysis.
 * This is a convenience function for comprehensive visualization of experiment data.
 * 
 * @param metrics Experimental metrics to visualize
 * @param outputFormat Output format for visualizations
 * @return NeuralVisualization object with experiment visualizations
 */
NeuralVisualization visualizeExperimentalResults(
    const ExperimentMetrics& metrics,
    NeuralVisualization::OutputFormat outputFormat = NeuralVisualization::OutputFormat::PNG
);

/**
 * @brief Compare experimental runs
 * 
 * Compares multiple experimental runs and provides detailed comparison analysis.
 * This function analyzes performance differences and identifies significant
 * variations between runs.
 * 
 * @param brains Vector of brain instances to compare
 * @param runNames Names of the runs to compare
 * @return Map containing comparison results
 */
std::map<std::string, std::map<std::string, float>> compareExperimentalRuns(
    const std::vector<std::shared_ptr<Brain>>& brains,
    const std::vector<std::string>& runNames
);

/**
 * @brief Export visualization and analysis results
 * 
 * Exports comprehensive visualization and analysis results to files in
 * multiple formats (PNG, SVG, JSON, CSV). This is a convenience function
 * for saving experiment results.
 * 
 * @param visualization Neural visualization object to export
 * @param filepath Base path for exported files
 * @param formats Vector of formats to export ("png", "svg", "json", "csv")
 * @return true if export was successful, false otherwise
 */
bool exportVisualizationResults(
    const NeuralVisualization& visualization,
    const std::string& filepath,
    const std::vector<std::string>& formats = std::vector<std::string>{"png", "svg"}
);

/**
 * @brief Load experimental configuration
 * 
 * Loads experimental configuration from file and creates a complete
 * analysis environment with visualization and metrics capabilities.
 * 
 * @param configPath Path to experimental configuration file
 * @param outputFormat Output format for visualizations
 * @return Map containing configuration and initialized objects
 */
std::map<std::string, std::variant<
    std::shared_ptr<Brain>,
    NeuralVisualization,
    NeuralActivityAnalyzer,
    TimeSeriesAnalyzer,
    PerformanceMetrics,
    std::string
>> loadExperimentalConfiguration(
    const std::string& configPath,
    NeuralVisualization::OutputFormat outputFormat = NeuralVisualization::OutputFormat::PNG
);

/**
 * @brief Run comprehensive analysis pipeline
 * 
 * Runs a comprehensive analysis pipeline including visualization, activity analysis,
 * time series analysis, and performance metrics. This is a convenience function
 * for complete experiment analysis.
 * 
 * @param brain Shared pointer to the brain instance to analyze
 * @return Map containing analysis results from all components
 */
std::map<std::string, std::variant<
    NeuralVisualization,
    NeuralActivityAnalyzer,
    TimeSeriesAnalyzer,
    PerformanceMetrics,
    std::string
>> runComprehensiveAnalysis(
    std::shared_ptr<Brain> brain
);

/**
 * @brief Create visualization from brain and metrics
 * 
 * Creates a comprehensive visualization combining brain state and metrics data.
 * This function merges visualization capabilities from both brain state and
 * experimental metrics for comprehensive analysis.
 * 
 * @param brain Shared pointer to the brain instance
 * @param metrics Experimental metrics to combine with visualization
 * @return NeuralVisualization object with combined visualization
 */
NeuralVisualization createCombinedVisualization(
    std::shared_ptr<Brain> brain,
    const ExperimentMetrics& metrics
);

/**
 * @brief Generate analysis report
 * 
 * Generates a comprehensive analysis report combining visualizations,
 * activity analysis, time series analysis, and performance metrics. The report
 * includes all findings and recommendations for further analysis.
 * 
 * @param brain Shared pointer to the brain instance
 * @param analysisType Type of analysis to perform
 * @return Comprehensive analysis report as string
 */
std::string generateComprehensiveReport(
    std::shared_ptr<Brain> brain,
    const std::string& analysisType = "full"
);

} // namespace nlm
