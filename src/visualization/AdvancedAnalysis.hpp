#pragma once

#include "VisualizationManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <mutex>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <complex>
#include <nlohmann/json.hpp>
#include <eigen3/Eigen/Dense>

namespace nlm {

// Advanced analysis features for neural simulation data
class AdvancedAnalysis {
public:
    struct PatternDetection {
        std::string patternType;
        double confidence;
        std::chrono::system_clock::time_point detectionTime;
        std::vector<std::string> supportingFeatures;
        std::string description;
    };

    struct NetworkTopology {
        size_t neuronCount;
        size_t synapseCount;
        size_t activeNeuronCount;
        size_t firingNeuronCount;
        double averageFiringRate;
        double excitationInhibitionRatio;
        std::vector<double> degreeDistribution;
        std::vector<double> clusteringCoefficients;
        std::vector<std::vector<double>> adjacencyMatrix;
        std::chrono::system_clock::time_point analysisTime;
    };

    struct LearningCurve {
        std::string simulationId;
        std::vector<double> performanceMetrics;
        std::vector<double> timePoints;
        std::vector<double> confidenceIntervals;
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
        std::string trend;
        double convergenceRate;
        double learningEfficiency;
    };

    struct ComparativeResult {
        std::string baselineId;
        std::string testId;
        std::unordered_map<std::string, double> metricsComparison;
        std::vector<std::string> significantDifferences;
        std::chrono::system_clock::time_point comparisonTime;
    };

    AdvancedAnalysis(std::shared_ptr<VisualizationManager> manager);
    ~AdvancedAnalysis();

    // Disable copying, enable moving
    AdvancedAnalysis(const AdvancedAnalysis&) = delete;
    AdvancedAnalysis& operator=(const AdvancedAnalysis&) = delete;
    AdvancedAnalysis(AdvancedAnalysis&&) noexcept;
    AdvancedAnalysis& operator=(AdvancedAnalysis&&) noexcept;

    // Pattern Recognition
    bool enablePatternRecognition(const std::string& patternTypes = "all");
    void setPatternDetectionThresholds(const std::unordered_map<std::string, double>& thresholds);
    std::vector<PatternDetection> detectPatterns(const std::vector<json>& data);
    std::vector<PatternDetection> detectActivityPatterns(const std::vector<json>& neuralActivity);
    std::vector<PatternDetection> detectTemporalPatterns(const std::vector<json>& timeSeriesData);
    std::vector<PatternDetection> detectSpatialPatterns(const std::vector<json>& spatialData);

    // Network Topology Analysis
    bool enableNetworkTopologyAnalysis(bool enable = true);
    NetworkTopology analyzeNetworkTopology(const Brain& brain);
    double calculateClusteringCoefficient(const std::vector<std::vector<double>>& adjacencyMatrix);
    std::vector<double> computeDegreeDistribution(const std::vector<std::vector<double>>& adjacencyMatrix);
    std::vector<std::vector<double>> buildAdjacencyMatrix(const Brain& brain);
    std::vector<double> identifyNetworkModules(const std::vector<std::vector<double>>& adjacencyMatrix);

    // Learning Curve Visualization
    bool enableLearningCurveAnalysis(bool enable = true);
    LearningCurve analyzeLearningCurve(const std::vector<json>& data, const std::string& simulationId);
    std::vector<double> calculatePerformanceTrajectory(const std::vector<json>& data);
    double calculateConvergenceRate(const std::vector<double>& performance, double threshold = 0.01);
    std::string determineTrend(const std::vector<double>& data);
    double calculateLearningEfficiency(const std::vector<double>& performance, const std::vector<double>& timePoints);

    // Comparative Analysis
    bool enableComparativeAnalysis(bool enable = true);
    ComparativeResult compareSimulations(const std::vector<std::string>& simulationIds, const std::string& baselineId = "");
    std::unordered_map<std::string, double> calculateMetricStatistics(const std::vector<json>& data, const std::string& metricName);
    std::vector<std::string> identifySignificantDifferences(const std::unordered_map<std::string, double>& baseline,
                                                           const std::unordered_map<std::string, double>& test,
                                                           double significanceThreshold = 0.05);
    std::string generateComparativeReport(const ComparativeResult& result);

    // Statistical Analysis
    void enableStatisticalAnalysis(bool enable = true);
    json performStatisticalAnalysis(const std::vector<json>& data, const std::vector<std::string>& metrics);
    json calculateDescriptiveStatistics(const std::vector<double>& values);
    json calculateInferentialStatistics(const std::vector<double>& baseline, const std::vector<double>& test);
    json performTimeSeriesAnalysis(const std::vector<json>& timeSeriesData, const std::string& analysisType = "all");

    // Machine Learning Integration
    bool enableMachineLearning(const std::string& modelPath = "");
    void trainPatternClassifier(const std::vector<json>& trainingData);
    void loadPreTrainedModel(const std::string& modelPath);
    std::vector<double> predictPattern(const json& dataPoint);
    double evaluateModelPerformance(const std::vector<json>& testData);

    // Advanced Metrics
    void enableAdvancedMetrics(bool enable = true);
    json calculateAdvancedNeuralMetrics(const std::vector<json>& neuralData);
    json calculateTemporalMetrics(const std::vector<json>& temporalData);
    json calculateSpatialMetrics(const std::vector<json>& spatialData);
    json calculateConnectivityMetrics(const Brain& brain);

    // Configuration and setup
    void setAnalysisWindow(const std::chrono::system_clock::time_point& start, const std::chrono::system_clock::time_point& end);
    void setSamplingRate(size_t samplesPerSecond);
    void setAnalysisDepth(size_t depth);

    // Integration with visualization
    void integrateWithVisualization(std::shared_ptr<VisualizationManager> manager);

    // Report generation
    std::string generatePatternReport(const std::vector<PatternDetection>& patterns);
    std::string generateTopologyReport(const NetworkTopology& topology);
    std::string generateLearningCurveReport(const LearningCurve& curve);
    std::string generateComparativeReport(const ComparativeResult& result);
    std::string generateStatisticalReport(const json& statistics);

    // Status and control
    bool isPatternRecognitionEnabled() const;
    bool isNetworkTopologyAnalysisEnabled() const;
    bool isLearningCurveAnalysisEnabled() const;
    bool isComparativeAnalysisEnabled() const;
    bool isStatisticalAnalysisEnabled() const;
    bool isMachineLearningEnabled() const;
    bool isAdvancedMetricsEnabled() const;

    // Export and import
    bool exportAnalysisResults(const std::string& filepath);
    bool importAnalysisResults(const std::string& filepath);
    json getAnalysisResults() const;

    // Resource management
    void clearData();
    size_t getDataSize() const;

    // Configuration
    void loadConfiguration(const std::string& configPath);
    void saveConfiguration() const;
    std::string getConfigurationJSON() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
