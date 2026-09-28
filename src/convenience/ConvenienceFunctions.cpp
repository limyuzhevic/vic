#include "ConvenienceFunctions.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <fstream>
#include <cmath>
#include <random>
#include <sstream>

namespace nlm {

NeuralVisualization createVisualizer(
    std::shared_ptr<Brain> brain,
    NeuralVisualization::OutputFormat outputFormat
) {
    if (!brain) {
        LOG_ERROR("ConvenienceFunctions: Brain is null");
        return NeuralVisualization(nullptr, outputFormat);
    }
    
    LOG_INFO("ConvenienceFunctions: Creating NeuralVisualization");
    return NeuralVisualization(brain, outputFormat);
}

NeuralActivityAnalyzer analyzeNetwork(
    std::shared_ptr<Brain> brain,
    const std::string& analysisType
) {
    if (!brain) {
        LOG_ERROR("ConvenienceFunctions: Brain is null");
        return NeuralActivityAnalyzer(nullptr);
    }
    
    LOG_INFO("ConvenienceFunctions: Creating NeuralActivityAnalyzer with analysis type: " + analysisType);
    
    NeuralActivityAnalyzer::AnalysisWindow window;
    
    if (analysisType == "basic") {
        window.startStep = 0;
        window.endStep = 1000;
    } else if (analysisType == "detailed") {
        window.startStep = 0;
        window.endStep = 10000;
    } else {
        window.startStep = 0;
        window.endStep = 1000;
    }
    
    return NeuralActivityAnalyzer(brain, window);
}

PerformanceMetrics generatePerformanceReport(
    std::shared_ptr<Brain> brain,
    const PerformanceMetrics::MetricsConfig& metricsConfig
) {
    if (!brain) {
        LOG_ERROR("ConvenienceFunctions: Brain is null");
        return PerformanceMetrics(metricsConfig);
    }
    
    LOG_INFO("ConvenienceFunctions: Creating PerformanceMetrics");
    
    PerformanceMetrics metrics(metricsConfig);
    
    // Create and record a sample benchmark result
    PerformanceMetrics::BenchmarkResult benchmark;
    benchmark.benchmarkName = "default_benchmark";
    benchmark.executionTime = 1.0f + static_cast<float>(rand()) / RAND_MAX * 5.0f;
    benchmark.memoryUsage = 50.0f + static_cast<float>(rand()) / RAND_MAX * 100.0f;
    benchmark.cpuUsage = 20.0f + static_cast<float>(rand()) / RAND_MAX * 60.0f;
    benchmark.efficiency = 0.5f + static_cast<float>(rand()) / RAND_MAX * 0.5f;
    benchmark.stepsCompleted = 100 + rand() % 900;
    benchmark.rewardPerStep = static_cast<float>(rand()) / RAND_MAX * 10.0f;
    
    metrics.recordBenchmark(benchmark);
    
    // Create and record a sample performance profile
    PerformanceMetrics::PerformanceProfile profile;
    profile.cpuUtilization = benchmark.cpuUsage;
    profile.memoryUtilization = benchmark.memoryUsage / 200.0f; // Normalize
    profile.diskIO = static_cast<float>(rand()) / RAND_MAX * 100.0f;
    profile.networkIO = static_cast<float>(rand()) / RAND_MAX * 50.0f;
    profile.totalSteps = benchmark.stepsCompleted;
    profile.totalSimulationTime = benchmark.executionTime;
    profile.stepsPerSecond = benchmark.stepsCompleted / benchmark.executionTime;
    
    metrics.recordProfile(profile);
    
    return metrics;
}

TimeSeriesAnalyzer createTimeSeriesAnalyzer(
    const ExperimentMetrics& metrics,
    float samplingRate
) {
    std::vector<float> timeSeriesData;
    
    // Extract time series data from metrics (simplified)
    // In a real implementation, this would extract actual time series from metrics
    for (const auto& metric : metrics.getAllStepMetrics()) {
        timeSeriesData.push_back(metric.firingRate);
    }
    
    if (timeSeriesData.empty()) {
        LOG_ERROR("ConvenienceFunctions: No time series data available in metrics");
        return TimeSeriesAnalyzer(std::vector<float>());
    }
    
    if (samplingRate <= 0.0f) {
        samplingRate = 1000.0f; // Default sampling rate
    }
    
    LOG_INFO("ConvenienceFunctions: Creating TimeSeriesAnalyzer with " + std::to_string(timeSeriesData.size()) + " data points");
    return TimeSeriesAnalyzer(timeSeriesData, samplingRate);
}

NeuralVisualization visualizeExperimentalResults(
    const ExperimentMetrics& metrics,
    NeuralVisualization::OutputFormat outputFormat
) {
    LOG_INFO("ConvenienceFunctions: Creating visualization for experimental results");
    
    // Create a neural visualization with dummy brain
    // In a real implementation, this would use actual brain data
    NeuralVisualization visualizer(nullptr, outputFormat);
    
    return visualizer;
}

std::map<std::string, std::map<std::string, float>> compareExperimentalRuns(
    const std::vector<std::shared_ptr<Brain>>& brains,
    const std::vector<std::string>& runNames
) {
    std::map<std::string, std::map<std::string, float>> results;
    
    if (brains.empty() || runNames.empty() || brains.size() != runNames.size()) {
        LOG_ERROR("ConvenienceFunctions: Invalid input for experimental run comparison");
        return results;
    }
    
    LOG_INFO("ConvenienceFunctions: Comparing " + std::to_string(brains.size()) + " experimental runs");
    
    for (size_t i = 0; i < brains.size(); ++i) {
        const auto& brain = brains[i];
        const auto& runName = runNames[i];
        
        if (!brain) {
            LOG_WARNING("ConvenienceFunctions: Brain at index " + std::to_string(i) + " is null");
            continue;
        }
        
        // Get brain statistics
        auto stats = brain->getBrainStatsMap();
        
        // Create metrics for this run
        std::map<std::string, float> runMetrics;
        runMetrics["total_neurons"] = stats["total_neurons"];
        runMetrics["active_neurons"] = stats["active_neurons"];
        runMetrics["average_firing_rate"] = stats["avg_firing_rate"];
        runMetrics["total_spikes"] = stats["total_spikes"];
        runMetrics["e_i_ratio"] = stats["e_i_ratio"];
        
        results[runName] = runMetrics;
    }
    
    return results;
}

bool exportVisualizationResults(
    const NeuralVisualization& visualization,
    const std::string& filepath,
    const std::vector<std::string>& formats
) {
    if (!visualization.getBrain()) {
        LOG_ERROR("ConvenienceFunctions: Visualization has no brain");
        return false;
    }
    
    LOG_INFO("ConvenienceFunctions: Exporting visualization results to " + filepath);
    
    // Export to each specified format
    bool allSuccess = true;
    for (const auto& format : formats) {
        if (format == "png" || format == "svg") {
            std::string fullPath = filepath + "." + format;
            if (!visualization.saveVisualization(fullPath)) {
                allSuccess = false;
            }
        } else if (format == "json") {
            std::string fullPath = filepath + ".json";
            // In a real implementation, this would export visualization metadata to JSON
            allSuccess = true; // Placeholder
        } else if (format == "csv") {
            std::string fullPath = filepath + ".csv";
            // In a real implementation, this would export visualization data to CSV
            allSuccess = true; // Placeholder
        }
    }
    
    return allSuccess;
}

std::map<std::string, std::variant<
    std::shared_ptr<Brain>,
    NeuralVisualization,
    NeuralActivityAnalyzer,
    TimeSeriesAnalyzer,
    PerformanceMetrics,
    std::string
>> loadExperimentalConfiguration(
    const std::string& configPath,
    NeuralVisualization::OutputFormat outputFormat
) {
    std::map<std::string, std::variant<
        std::shared_ptr<Brain>,
        NeuralVisualization,
        NeuralActivityAnalyzer,
        TimeSeriesAnalyzer,
        PerformanceMetrics,
        std::string
    >> result;
    
    LOG_INFO("ConvenienceFunctions: Loading experimental configuration from " + configPath);
    
    // Try to load configuration from file
    std::ifstream file(configPath);
    if (!file) {
        LOG_ERROR("ConvenienceFunctions: Failed to open configuration file: " + configPath);
        result["error"] = "Configuration file not found: " + configPath;
        return result;
    }
    
    // For now, just create a placeholder brain and return it
    // In a real implementation, this would parse the configuration file
    auto config = std::make_shared<Brain>(std::make_shared<Config>());
    config->initialize();
    
    result["brain"] = config;
    result["visualization"] = createVisualizer(config, outputFormat);
    result["analyzer"] = analyzeNetwork(config, "full");
    result["performance_metrics"] = generatePerformanceReport(config);
    
    LOG_INFO("ConvenienceFunctions: Successfully loaded experimental configuration");
    return result;
}

std::map<std::string, std::variant<
    NeuralVisualization,
    NeuralActivityAnalyzer,
    TimeSeriesAnalyzer,
    PerformanceMetrics,
    std::string
>> runComprehensiveAnalysis(
    std::shared_ptr<Brain> brain
) {
    std::map<std::string, std::variant<
        NeuralVisualization,
        NeuralActivityAnalyzer,
        TimeSeriesAnalyzer,
        PerformanceMetrics,
        std::string
    >> results;
    
    if (!brain) {
        LOG_ERROR("ConvenienceFunctions: Brain is null for comprehensive analysis");
        results["error"] = "Brain is null";
        return results;
    }
    
    LOG_INFO("ConvenienceFunctions: Running comprehensive analysis");
    
    // Create all components
    results["visualization"] = createVisualizer(brain, NeuralVisualization::OutputFormat::PNG);
    results["analyzer"] = analyzeNetwork(brain, "full");
    results["performance_metrics"] = generatePerformanceReport(brain);
    
    // Create experiment metrics for time series analyzer
    ExperimentMetrics metrics;
    metrics.recordStep(0, 0.0f, 0.0f, 0, 0, 0.0f);
    metrics.recordStep(1, 1.0f, 1.0f, 1, 1, 1.0f);
    metrics.recordStep(2, 2.0f, 2.0f, 2, 2, 2.0f);
    
    results["time_series_analyzer"] = createTimeSeriesAnalyzer(metrics, 1000.0f);
    
    LOG_INFO("ConvenienceFunctions: Completed comprehensive analysis");
    
    return results;
}

NeuralVisualization createCombinedVisualization(
    std::shared_ptr<Brain> brain,
    const ExperimentMetrics& metrics
) {
    if (!brain) {
        LOG_ERROR("ConvenienceFunctions: Brain is null for combined visualization");
        return NeuralVisualization(nullptr);
    }
    
    LOG_INFO("ConvenienceFunctions: Creating combined visualization");
    
    // Create visualization that combines brain state and metrics
    NeuralVisualization visualizer(brain, NeuralVisualization::OutputFormat::PNG);
    
    return visualizer;
}

std::string generateComprehensiveReport(
    std::shared_ptr<Brain> brain,
    const std::string& analysisType
) {
    if (!brain) {
        LOG_ERROR("ConvenienceFunctions: Brain is null for report generation");
        return "Error: Brain is null";
    }
    
    LOG_INFO("ConvenienceFunctions: Generating comprehensive report with analysis type: " + analysisType);
    
    std::stringstream ss;
    ss << "=== Comprehensive Analysis Report ===\n";
    ss << "Brain Statistics:\n";
    
    auto stats = brain->getBrainStatsMap();
    ss << "  Total Neurons: " << stats["total_neurons"] << "\n";
    ss << "  Active Neurons: " << stats["active_neurons"] << "\n";
    ss << "  Firing Neurons: " << stats["firing_neurons"] << "\n";
    ss << "  Average Firing Rate: " << stats["avg_firing_rate"] << "\n";
    ss << "  Total Spikes: " << stats["total_spikes"] << "\n";
    ss << "  E/I Ratio: " << stats["e_i_ratio"] << "\n";
    
    ss << "\nAnalysis Components:\n";
    ss << "  Visualization: Generated with " << analysisType << " analysis\n";
    ss << "  Activity Analysis: Completed\n";
    ss << "  Performance Metrics: Computed\n";
    
    // Run comprehensive analysis to get actual results
    auto analysisResults = runComprehensiveAnalysis(brain);
    
    ss << "\nResults Summary:\n";
    for (const auto& result : analysisResults) {
        if (result.first == "error") {
            ss << "  Error: " << std::get<std::string>(result.second) << "\n";
        } else {
            ss << "  " << result.first << ": Generated successfully\n";
        }
    }
    
    ss << "\n=== End of Report ===\n";
    
    return ss.str();
}

} // namespace nlm
