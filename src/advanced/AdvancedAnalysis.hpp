#pragma once

#include "../src/experiments/Experiment.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/NeuralRegion.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/dynamics/SpikeSystem.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <complex>
#include <functional>
#include <numeric>

namespace nlm {

// Advanced analysis configuration
struct AdvancedAnalysisConfig {
    // Neural pattern analysis
    bool enableNeuralPatternAnalysis;
    float patternMatchingThreshold;
    std::vector<std::string> patternTypes;  // "spiking", "rate", "phase", "spatial"
    std::map<std::string, float> patternWeights;
    
    // Learning curve analysis
    bool enableLearningCurveAnalysis;
    size_t learningCurveSampleSize;
    std::string learningCurveInterpolation;  // "linear", "spline", "exponential"
    bool enableFitMetrics;
    std::vector<std::string> learningCurveMetrics;
    
    // Statistical analysis
    bool enableStatisticalAnalysis;
    std::vector<std::string> statisticalTests;  // "t-test", "ANOVA", "correlation", "regression"
    float significanceLevel;
    bool enableConfidenceIntervals;
    bool enableEffectSize;
    
    // Visualization
    bool enableVisualization;
    std::string visualizationBackend;  // "matplotlib", "plotly", "custom"
    std::map<std::string, std::string> plotConfigurations;
    bool enableInteractivePlots;
    bool enableExportFormats;  // "pdf", "png", "svg", "html"
    
    // Machine learning integration
    bool enableMachineLearning;
    std::string mlFramework;  // "scikit-learn", "tensorflow", "pytorch", "custom"
    std::vector<std::string> mlTasks;  // "classification", "regression", "clustering", "dimensionality_reduction"
    std::map<std::string, float> mlHyperparameters;
    bool enableOnlineLearning;
    
    // Performance optimization
    bool enableParallelAnalysis;
    size_t analysisThreads;
    bool enableStreamingAnalysis;
    float analysisBufferSize;
    
    // Export and storage
    bool enableDataExport;
    std::string exportFormat;  // "json", "csv", "hdf5", "parquet"
    std::string exportPath;
    bool enableCloudStorage;
};

// Neural pattern types
enum class NeuralPatternType {
    SpikingPattern,
    RatePattern,
    PhasePattern,
    SpatialPattern,
    FrequencyPattern,
    CrossRegionPattern,
    CustomPattern
};

// Pattern analysis result
struct PatternAnalysisResult {
    NeuralPatternType type;
    std::string name;
    float similarity;
    std::map<std::string, float> metrics;
    std::vector<float> signature;
    float confidence;
    
    PatternAnalysisResult() : type(NeuralPatternType::CustomPattern), similarity(0.0f),
                               confidence(0.0f) {}
};

// Learning curve analysis result
struct LearningCurveResult {
    std::vector<float> curve;
    std::vector<size_t> steps;
    std::vector<size_t> trials;
    std::string metric;
    std::map<std::string, float> fitParameters;
    std::map<std::string, float> statistics;
    float rSquared;
    float convergencePoint;
    
    LearningCurveResult() : rSquared(0.0f), convergencePoint(0.0f) {}
};

// Statistical analysis result
struct StatisticalAnalysisResult {
    std::string testName;
    float testStatistic;
    float pValue;
    float effectSize;
    std::vector<float> confidenceInterval;
    std::string interpretation;
    std::map<std::string, float> additionalMetrics;
    
    StatisticalAnalysisResult() : testStatistic(0.0f), pValue(1.0f), effectSize(0.0f) {}
};

// Advanced Analysis Engine
class AdvancedAnalysis {
public:
    AdvancedAnalysis();
    ~AdvancedAnalysis();
    
    // Disable copying, enable moving
    AdvancedAnalysis(const AdvancedAnalysis&) = delete;
    AdvancedAnalysis& operator=(const AdvancedAnalysis&) = delete;
    AdvancedAnalysis(AdvancedAnalysis&&) noexcept = default;
    AdvancedAnalysis& operator=(AdvancedAnalysis&&) noexcept = default;
    
    // Configuration
    void configure(const AdvancedAnalysisConfig& config);
    void updateConfig(const std::string& key, const std::string& value);
    
    // Neural pattern analysis
    PatternAnalysisResult analyzeNeuralPattern(const std::vector<float>& data,
                                              NeuralPatternType patternType = NeuralPatternType::SpikingPattern,
                                              const std::string& patternName = "") const;
    std::vector<PatternAnalysisResult> analyzeMultiplePatterns(
        const std::vector<std::vector<float>>& datasets,
        NeuralPatternType patternType) const;
    bool detectPatternAnomalies(const std::vector<float>& data,
                               float threshold = 2.0f) const;
    
    // Learning curve analysis
    LearningCurveResult analyzeLearningCurve(const std::vector<size_t>& steps,
                                             const std::vector<float>& metricValues,
                                             const std::string& metricName = "performance",
                                             const std::string& interpolation = "linear") const;
    std::vector<LearningCurveResult> analyzeMultipleLearningCurves(
        const std::map<std::string, std::vector<std::pair<size_t, float>>>& curves) const;
    float predictLearningProgress(const LearningCurveResult& curve, size_t futureSteps) const;
    
    // Statistical analysis
    StatisticalAnalysisResult performStatisticalTest(const std::vector<float>& data1,
                                                     const std::vector<float>& data2,
                                                     const std::string& testType = "t-test") const;
    StatisticalAnalysisResult performRegressionAnalysis(const std::vector<size_t>& x,
                                                        const std::vector<float>& y,
                                                        const std::string& regressionType = "linear") const;
    StatisticalAnalysisResult performANOVA(const std::vector<std::vector<float>>& groups) const;
    std::map<std::string, StatisticalAnalysisResult> performMultipleStatisticalTests(
        const std::map<std::string, std::vector<float>>& datasets) const;
    
    // Neural network analysis
    PatternAnalysisResult analyzeNeuralDynamics(const Brain* brain,
                                               const std::string& analysisType = "spiking") const;
    PatternAnalysisResult analyzeNetworkConnectivity(const Brain* brain) const;
    PatternAnalysisResult analyzeRegionalActivity(const Brain* brain,
                                                 const std::vector<RegionId>& regions) const;
    
    // Learning and adaptation analysis
    LearningCurveResult analyzePlasticityLearning(const std::vector<float>& plasticityValues,
                                                  const std::vector<size_t>& steps) const;
    LearningCurveResult analyzeDevelopmentLearning(const std::vector<float>& developmentMetrics,
                                                   const std::vector<size_t>& steps) const;
    
    // Visualization tools
    std::string generateNeuralPatternPlot(const PatternAnalysisResult& pattern) const;
    std::string generateLearningCurvePlot(const LearningCurveResult& curve) const;
    std::string generateStatisticalPlot(const StatisticalAnalysisResult& result) const;
    std::string generateMultiPanelPlot(const std::vector<std::string>& plots) const;
    
    // Machine learning integration
    std::map<std::string, float> classifyNeurons(const std::vector<std::vector<float>>& features,
                                                const std::string& modelType = "random_forest") const;
    std::map<std::string, float> predictLearningOutcome(const std::vector<float>& features) const;
    std::vector<size_t> clusterNeurons(const std::vector<std::vector<float>>& features,
                                       const std::string& clusteringType = "kmeans") const;
    std::vector<float> reduceDimensionality(const std::vector<std::vector<float>>& features,
                                           const std::string& reductionType = "pca",
                                           size_t targetDimensions = 2) const;
    
    // Performance analysis
    float calculateNeuralEfficiency(const Brain* brain) const;
    float calculateLearningRate(const std::vector<float>& metricValues,
                               const std::vector<size_t>& steps) const;
    float calculateStability(const std::vector<float>& values) const;
    
    // Export and import
    bool exportResults(const std::string& filepath) const;
    bool importResults(const std::string& filepath);
    bool exportConfiguration(const std::string& filepath) const;
    bool importConfiguration(const std::string& filepath);
    
    // Get current configuration
    const AdvancedAnalysisConfig& getConfig() const;
    void updateConfig(const AdvancedAnalysisConfig& config);
    
    // Get analysis results
    PatternAnalysisResult getPatternAnalysisResult(const std::string& patternName) const;
    LearningCurveResult getLearningCurveResult(const std::string& curveName) const;
    StatisticalAnalysisResult getStatisticalResult(const std::string& testName) const;
    
    // Clear results
    void clearResults();
    void clearResults(const std::string& resultType);  // "patterns", "curves", "statistics"
    
    // Monitoring and logging
    void enableMonitoring(bool enable);
    bool isMonitoringEnabled() const;
    void logAnalysisStatus() const;
    
    // Streaming analysis
    void startStreamingAnalysis(const std::string& analysisType);
    void stopStreamingAnalysis();
    bool isStreaming() const;
    std::vector<float> getStreamingResults() const;
    
    // Parallel analysis
    void enableParallelAnalysis(bool enable);
    bool isParallelAnalysisEnabled() const;
    void setAnalysisThreads(size_t count);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
