#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <cmath>
#include "../core/Logger/Logger.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../visualization/VisualizationData.hpp"

namespace nlm {

/**
 * @file DataAnalyzer.hpp
 * 
 * Data analysis and statistics engine for NLM.
 * 
 * This file provides the DataAnalyzer class, which offers comprehensive
 * data analysis capabilities for NLM applications. It performs statistical
 * analysis, pattern recognition, and data mining on neural data to extract
 * meaningful insights and support scientific discovery.
 * 
 * Key features:
 * - Statistical analysis (mean, variance, standard deviation)
 * - Pattern recognition and classification
 * - Time series analysis
 * - Clustering analysis
 * - Correlation analysis
 * - Hypothesis testing
 * - Machine learning integration
 * - Data visualization integration
 * - Export analysis results
 * - Real-time data analysis
 * - Custom analysis functions
 * - Automated feature extraction
 * - Anomaly detection
 * - Performance metrics analysis
 * 
 * @note Integrates with NLM's visualization system
 * @see VisualizationData - Data structure for analysis
 * @see PatternRecognizer - Pattern recognition functionality
 * @see PerformanceProfiler - Performance analysis integration
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */
class DataAnalyzer {
public:
    DataAnalyzer();
    ~DataAnalyzer();
    
    // Disable copying - PImpl idiom
    DataAnalyzer(const DataAnalyzer&) = delete;
    DataAnalyzer& operator=(const DataAnalyzer&) = delete;
    DataAnalyzer(DataAnalyzer&&) noexcept;
    DataAnalyzer& operator=(DataAnalyzer&&) noexcept;
    
    /**
     * @brief Analyze spike data
     * 
     * @param spikeData Spike data to analyze
     * @return Analysis results
     */
    std::unordered_map<std::string, double> analyzeSpikeData(const std::vector<std::tuple<int64_t, double, float>>& spikeData);
    
    /**
     * @brief Analyze membrane potential data
     * 
     * @param voltageData Voltage data to analyze
     * @return Analysis results
     */
    std::unordered_map<std::string, double> analyzeVoltageData(const std::vector<std::tuple<int64_t, double, float>>& voltageData);
    
    /**
     * @brief Analyze synaptic weight data
     * 
     * @param synapticData Synaptic data to analyze
     * @return Analysis results
     */
    std::unordered_map<std::string, double> analyzeSynapticData(const std::vector<std::tuple<int64_t, int64_t, float, double>>& synapticData);
    
    /**
     * @brief Perform time series analysis
     * 
     * @param timeSeriesData Time series data
     * @return Analysis results
     */
    std::unordered_map<std::string, double> analyzeTimeSeries(const std::vector<std::pair<double, float>>& timeSeriesData);
    
    /**
     * @brief Perform correlation analysis
     * 
     * @param data1 First data series
     * @param data2 Second data series
     * @return Correlation coefficient
     */
    double correlateData(const std::vector<std::pair<double, float>>& data1, const std::vector<std::pair<double, float>>& data2);
    
    /**
     * @brief Perform clustering analysis
     * 
     * @param data Data points to cluster
     * @param numClusters Number of clusters
     * @return Clustering results
     */
    std::vector<std::vector<size_t>> clusterData(const std::vector<std::pair<double, float>>& data, size_t numClusters);
    
    /**
     * @brief Perform hypothesis testing
     * 
     * @param data1 First dataset
     * @param data2 Second dataset
     * @return Hypothesis test results
     */
    std::unordered_map<std::string, double> hypothesisTest(const std::vector<float>& data1, const std::vector<float>& data2);
    
    /**
     * @brief Detect anomalies in data
     * 
     * @param data Data to analyze
     * @param threshold Anomaly threshold
     * @return Anomalies detected
     */
    std::vector<size_t> detectAnomalies(const std::vector<float>& data, float threshold);
    
    /**
     * @brief Extract features from spike trains
     * 
     * @param spikeData Spike data
     * @return Extracted features
     */
    std::unordered_map<std::string, double> extractSpikeFeatures(const std::vector<std::tuple<int64_t, double, float>>& spikeData);
    
    /**
     * @brief Extract features from voltage traces
     * 
     * @param voltageData Voltage data
     * @return Extracted features
     */
    std::unordered_map<std::string, double> extractVoltageFeatures(const std::vector<std::tuple<int64_t, double, float>>& voltageData);
    
    /**
     * @brief Analyze neural dynamics
     * 
     * @param brain Reference to brain system
     * @return Neural dynamics analysis
     */
    std::unordered_map<std::string, double> analyzeNeuralDynamics(const Brain& brain);
    
    /**
     * @brief Export analysis results
     * 
     * @param results Analysis results
     * @param format Export format
     * @param filepath Output file path
     * @return true if export successful
     */
    bool exportAnalysisResults(const std::unordered_map<std::string, double>& results, const std::string& format, const std::string& filepath);
    
    /**
     * @brief Set analysis callback
     * 
     * @param callback Function to call on analysis completion
     */
    void setAnalysisCallback(std::function<void(const std::unordered_map<std::string, double>&)> callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @file PatternRecognizer.hpp
 * 
 * Pattern recognition for neural data analysis.
 * 
 * This file provides the PatternRecognizer class, which offers
 * pattern recognition capabilities for neural data. It identifies
 * patterns, classifies data, and extracts meaningful features from
 * neural recordings using various machine learning algorithms.
 * 
 * Key features:
 * - Spike pattern recognition
 * - Voltage pattern classification
 * - Synaptic pattern detection
 * - Time series pattern matching
 * - Neural activity pattern extraction
 * - Automatic pattern discovery
 * - Pattern validation
 * - Pattern comparison
 * - Pattern visualization
 * - Pattern export
 * - Custom pattern detection
 * - Ensemble pattern recognition
 * - Real-time pattern recognition
 * - Adaptive pattern recognition
 * - Feature-based pattern matching
 * - Template-based pattern matching
 * 
 * @note Integrates with NLM's analysis system
 * @see DataAnalyzer - For analysis integration
 * @see VisualizationEngine - For pattern visualization
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */
class PatternRecognizer {
public:
    PatternRecognizer();
    ~PatternRecognizer();
    
    // Disable copying - PImpl idiom
    PatternRecognizer(const PatternRecognizer&) = delete;
    PatternRecognizer& operator=(const PatternRecognizer&) = delete;
    PatternRecognizer(PatternRecognizer&&) noexcept;
    PatternRecognizer& operator=(PatternRecognizer&&) noexcept;
    
    /**
     * @brief Recognize spike patterns
     * 
     * @param spikeData Spike data to analyze
     * @return Recognized patterns
     */
    std::vector<std::unordered_map<std::string, double>> recognizeSpikePatterns(const std::vector<std::tuple<int64_t, double, float>>& spikeData);
    
    /**
     * @brief Classify voltage patterns
     * 
     * @param voltageData Voltage data to classify
     * @return Classification results
     */
    std::vector<std::unordered_map<std::string, double>> classifyVoltagePatterns(const std::vector<std::tuple<int64_t, double, float>>& voltageData);
    
    /**
     * @brief Detect synaptic patterns
     * 
     * @param synapticData Synaptic data to analyze
     * @return Detected patterns
     */
    std::vector<std::unordered_map<std::string, double>> detectSynapticPatterns(const std::vector<std::tuple<int64_t, int64_t, float, double>>& synapticData);
    
    /**
     * @brief Match time series patterns
     * 
     * @param timeSeriesData Time series data
     * @param patternData Pattern data to match
     * @return Match results
     */
    std::vector<std::pair<double, double>> matchTimeSeriesPatterns(const std::vector<std::pair<double, float>>& timeSeriesData, const std::vector<std::pair<double, float>>& patternData);
    
    /**
     * @brief Extract neural activity patterns
     * 
     * @param brain Reference to brain system
     * @return Extracted patterns
     */
    std::vector<std::unordered_map<std::string, double>> extractNeuralActivityPatterns(const Brain& brain);
    
    /**
     * @brief Train pattern recognition model
     * 
     * @param trainingData Training data
     * @param labels Labels for training data
     */
    void trainPatternRecognition(const std::vector<std::vector<float>>& trainingData, const std::vector<int>& labels);
    
    /**
     * @brief Predict pattern classification
     * 
     * @param data Data to classify
     * @return Prediction results
     */
    std::vector<int> predictPatternClassification(const std::vector<std::vector<float>>& data);
    
    /**
     * @brief Export patterns
     * 
     * @param patterns Patterns to export
     * @param format Export format
     * @param filepath Output file path
     * @return true if export successful
     */
    bool exportPatterns(const std::vector<std::unordered_map<std::string, double>>& patterns, const std::string& format, const std::string& filepath);
    
    /**
     * @brief Set pattern recognition callback
     * 
     * @param callback Function to call on pattern recognition
     */
    void setPatternCallback(std::function<void(const std::vector<std::unordered_map<std::string, double>>&)> callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @file StatisticalAnalyzer.hpp
 * 
 * Statistical analysis for neural data.
 * 
 * This file provides the StatisticalAnalyzer class, which offers
 * comprehensive statistical analysis capabilities for neural data.
 * It performs statistical tests, computes statistics, and provides
 * statistical insights into neural recordings.
 * 
 * Key features:
 * - Descriptive statistics
 * - Inferential statistics
 * * Hypothesis testing
 * - Regression analysis
 * - ANOVA analysis
 * - Time series statistics
 * - Distribution analysis
 * - Statistical significance testing
 * - Confidence interval calculation
 * - Statistical power analysis
 * - Effect size calculation
 * - Statistical visualization
 * - Statistical report generation
 * - Statistical validation
 * - Statistical comparison
 * - Statistical modeling
 * - Statistical inference
 * 
 * @note Integrates with NLM's analysis system
 * @see DataAnalyzer - For analysis integration
 * @see VisualizationEngine - For statistical visualization
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */
class StatisticalAnalyzer {
public:
    StatisticalAnalyzer();
    ~StatisticalAnalyzer();
    
    // Disable copying - PImpl idiom
    StatisticalAnalyzer(const StatisticalAnalyzer&) = delete;
    StatisticalAnalyzer& operator=(const StatisticalAnalyzer&) = delete;
    StatisticalAnalyzer(StatisticalAnalyzer&&) noexcept;
    StatisticalAnalyzer& operator=(StatisticalAnalyzer&&) noexcept;
    
    /**
     * @brief Compute descriptive statistics
     * 
     * @param data Data to analyze
     * @return Descriptive statistics
     */
    std::unordered_map<std::string, double> computeDescriptiveStatistics(const std::vector<float>& data);
    
    /**
     * @brief Perform t-test
     * 
     * @param data1 First dataset
     * @param data2 Second dataset
     * @return T-test results
     */
    std::unordered_map<std::string, double> performTTest(const std::vector<float>& data1, const std::vector<float>& data2);
    
    /**
     * @brief Perform ANOVA
     * 
     * @param datasets Multiple datasets
     * @return ANOVA results
     */
    std::unordered_map<std::string, double> performANOVA(const std::vector<std::vector<float>>& datasets);
    
    /**
     * @brief Perform regression analysis
     * 
     * @param x Independent variable
     * @param y Dependent variable
     * @return Regression results
     */
    std::unordered_map<std::string, double> performRegressionAnalysis(const std::vector<float>& x, const std::vector<float>& y);
    
    /**
     * @brief Calculate confidence interval
     * 
     * @param data Data
     * @param confidenceLevel Confidence level (0-1)
     * @return Confidence interval (lower, upper)
     */
    std::pair<double, double> calculateConfidenceInterval(const std::vector<float>& data, double confidenceLevel = 0.95);
    
    /**
     * @brief Perform chi-square test
     * 
     * @param observedData Observed frequencies
     * @param expectedData Expected frequencies
     * @return Chi-square results
     */
    std::unordered_map<std::string, double> performChiSquareTest(const std::vector<int>& observedData, const std::vector<int>& expectedData);
    
    /**
     * @brief Calculate statistical power
     * 
     * @param effectSize Effect size
     * @param sampleSize Sample size
     * @param alpha Significance level
     * @return Statistical power
     */
    double calculateStatisticalPower(double effectSize, size_t sampleSize, double alpha = 0.05);
    
    /**
     * @brief Export statistical results
     * 
     * @param results Statistical results
     * @param format Export format
     * @param filepath Output file path
     * @return true if export successful
     */
    bool exportStatisticalResults(const std::unordered_map<std::string, double>& results, const std::string& format, const std::string& filepath);
    
    /**
     * @brief Set statistical analysis callback
     * 
     * @param callback Function to call on statistical analysis
     */
    void setStatisticalCallback(std::function<void(const std::unordered_map<std::string, double>&)> callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm