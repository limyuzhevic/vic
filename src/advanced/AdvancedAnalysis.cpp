#include "AdvancedAnalysis.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../experiments/Experiment.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iostream>
#include <numeric>
#include <random>
#include <unordered_map>
#include <chrono>

namespace nlm {

struct AdvancedAnalysis::Impl {
    AdvancedAnalysisConfig config;
    
    // Results storage
    std::map<std::string, PatternAnalysisResult> patternResults;
    std::map<std::string, LearningCurveResult> curveResults;
    std::map<std::string, StatisticalAnalysisResult> statisticalResults;
    
    // Monitoring
    bool monitoringEnabled;
    bool streamingEnabled;
    std::string currentStreamType;
    std::vector<std::vector<float>> streamingBuffer;
    std::chrono::steady_clock::time_point lastStreamUpdate;
    
    // Parallel processing
    bool parallelEnabled;
    size_t analysisThreads;
    
    // Performance metrics
    float totalAnalysisTime;
    size_t totalAnalysisOperations;
    
    Impl() : monitoringEnabled(false), streamingEnabled(false),
             parallelEnabled(false), analysisThreads(std::thread::hardware_concurrency()),
             totalAnalysisTime(0.0f), totalAnalysisOperations(0) {
        
        loadDefaultConfig();
        resetConfig();
    }
    
    void loadDefaultConfig() {
        // Neural pattern analysis defaults
        config.enableNeuralPatternAnalysis = true;
        config.patternMatchingThreshold = 0.7f;
        config.patternTypes = {"spiking", "rate", "phase", "spatial"};
        config.patternWeights = {
            {"spiking", 1.0f},
            {"rate", 0.8f},
            {"phase", 0.6f},
            {"spatial", 0.9f}
        };
        
        // Learning curve analysis defaults
        config.enableLearningCurveAnalysis = true;
        config.learningCurveSampleSize = 100;
        config.learningCurveInterpolation = "spline";
        config.enableFitMetrics = true;
        config.learningCurveMetrics = {"accuracy", "loss", "fitness", "convergence"};
        
        // Statistical analysis defaults
        config.enableStatisticalAnalysis = true;
        config.statisticalTests = {"t-test", "ANOVA", "correlation", "regression"};
        config.significanceLevel = 0.05f;
        config.enableConfidenceIntervals = true;
        config.enableEffectSize = true;
        
        // Visualization defaults
        config.enableVisualization = true;
        config.visualizationBackend = "matplotlib";
        config.enableInteractivePlots = false;
        config.enableExportFormats = true;
        
        // Machine learning defaults
        config.enableMachineLearning = false;  // Disabled by default
        config.mlFramework = "scikit-learn";
        config.mlTasks = {"classification", "regression"};
        config.enableOnlineLearning = false;
        
        // Performance optimization
        config.enableParallelAnalysis = false;
        config.analysisThreads = std::thread::hardware_concurrency();
        config.enableStreamingAnalysis = false;
        config.analysisBufferSize = 1000.0f;
        
        // Export defaults
        config.enableDataExport = true;
        config.exportFormat = "json";
        config.exportPath = "analysis_results";
        config.enableCloudStorage = false;
    }
    
    void resetConfig() {
        patternResults.clear();
        curveResults.clear();
        statisticalResults.clear();
        
        streamingBuffer.clear();
        currentStreamType = "";
        
        totalAnalysisTime = 0.0f;
        totalAnalysisOperations = 0;
    }
    
    // Statistical helper functions
    float calculateMean(const std::vector<float>& data) const {
        if (data.empty()) return 0.0f;
        float sum = std::accumulate(data.begin(), data.end(), 0.0f);
        return sum / data.size();
    }
    
    float calculateVariance(const std::vector<float>& data) const {
        if (data.size() <= 1) return 0.0f;
        
        float mean = calculateMean(data);
        float sumSquaredDiff = 0.0f;
        for (float value : data) {
            float diff = value - mean;
            sumSquaredDiff += diff * diff;
        }
        
        return sumSquaredDiff / (data.size() - 1);
    }
    
    float calculateStandardDeviation(const std::vector<float>& data) const {
        return std::sqrt(calculateVariance(data));
    }
    
    float calculatePearsonCorrelation(const std::vector<size_t>& x,
                                      const std::vector<float>& y) const {
        if (x.size() != y.size() || x.size() < 2) return 0.0f;
        
        float xMean = calculateMean(std::vector<float>(x.begin(), x.end()));
        float yMean = calculateMean(y);
        
        float numerator = 0.0f;
        float xVariance = 0.0f;
        float yVariance = 0.0f;
        
        for (size_t i = 0; i < x.size(); ++i) {
            float xDiff = static_cast<float>(x[i]) - xMean;
            float yDiff = y[i] - yMean;
            numerator += xDiff * yDiff;
            xVariance += xDiff * xDiff;
            yVariance += yDiff * yDiff;
        }
        
        if (xVariance == 0.0f || yVariance == 0.0f) return 0.0f;
        
        return numerator / (std::sqrt(xVariance) * std::sqrt(yVariance));
    }
    
    // Pattern analysis helpers
    float calculatePatternSimilarity(const std::vector<float>& pattern1,
                                    const std::vector<float>& pattern2) const {
        if (pattern1.size() != pattern2.size() || pattern1.empty()) return 0.0f;
        
        float dotProduct = 0.0f;
        float norm1 = 0.0f;
        float norm2 = 0.0f;
        
        for (size_t i = 0; i < pattern1.size(); ++i) {
            dotProduct += pattern1[i] * pattern2[i];
            norm1 += pattern1[i] * pattern1[i];
            norm2 += pattern2[i] * pattern2[i];
        }
        
        norm1 = std::sqrt(norm1);
        norm2 = std::sqrt(norm2);
        
        if (norm1 == 0.0f || norm2 == 0.0f) return 0.0f;
        
        return dotProduct / (norm1 * norm2);
    }
    
    // Learning curve fitting helpers
    std::map<std::string, float> fitExponentialCurve(const std::vector<size_t>& steps,
                                                     const std::vector<float>& values) const {
        std::map<std::string, float> params;
        
        // Simple exponential fit: y = a * exp(b * x)
        if (steps.empty() || values.empty()) return params;
        
        // Find asymptote (final value)
        float asymptote = values.back();
        params["asymptote"] = asymptote;
        
        // Find initial value
        float initial = values.front();
        params["initial"] = initial;
        
        // Estimate growth rate
        float finalStep = static_cast<float>(steps.back());
        float timeSpan = finalStep - steps.front();
        if (timeSpan > 0.0f) {
            float growthRate = std::log(asymptote / initial) / timeSpan;
            params["growth_rate"] = growthRate;
        }
        
        // Calculate R-squared
        float r2 = 0.0f;
        if (asymptote > 0.0f) {
            float ssTot = 0.0f;
            float ssRes = 0.0f;
            float mean = calculateMean(values);
            
            for (size_t i = 0; i < steps.size(); ++i) {
                float predicted = initial * std::exp(params["growth_rate"] * (steps[i] - steps.front()));
                ssTot += (values[i] - mean) * (values[i] - mean);
                ssRes += (values[i] - predicted) * (values[i] - predicted);
            }
            
            if (ssTot > 0.0f) {
                r2 = 1.0f - (ssRes / ssTot);
            }
        }
        
        params["r_squared"] = r2;
        
        return params;
    }
};

AdvancedAnalysis::AdvancedAnalysis() : pImpl(std::make_unique<Impl>()) {
}

AdvancedAnalysis::~AdvancedAnalysis() = default;

void AdvancedAnalysis::configure(const AdvancedAnalysisConfig& config) {
    pImpl->config = config;
    NLM_LOG_INFO("Advanced analysis configured");
}

void AdvancedAnalysis::updateConfig(const std::string& key, const std::string& value) {
    // Update specific config based on key
    NLM_LOG_INFO("Updated advanced analysis config: " + key + " = " + value);
}

PatternAnalysisResult AdvancedAnalysis::analyzeNeuralPattern(const std::vector<float>& data,
                                                           NeuralPatternType patternType,
                                                           const std::string& patternName) const {
    PatternAnalysisResult result;
    result.type = patternType;
    result.name = patternName.empty() ? "Pattern_" + std::to_string(patternResults.size()) : patternName;
    
    if (data.empty()) {
        NLM_LOG_WARNING("Empty data for pattern analysis");
        return result;
    }
    
    pImpl->totalAnalysisOperations++;
    
    // Analyze pattern based on type
    switch (patternType) {
        case NeuralPatternType::SpikingPattern:
            result = analyzeSpikingPattern(data);
            break;
        case NeuralPatternType::RatePattern:
            result = analyzeRatePattern(data);
            break;
        case NeuralPatternType::PhasePattern:
            result = analyzePhasePattern(data);
            break;
        case NeuralPatternType::SpatialPattern:
            result = analyzeSpatialPattern(data);
            break;
        case NeuralPatternType::FrequencyPattern:
            result = analyzeFrequencyPattern(data);
            break;
        case NeuralPatternType::CrossRegionPattern:
            result = analyzeCrossRegionPattern(data);
            break;
        default:
            result = analyzeCustomPattern(data);
            break;
    }
    
    // Store result
    pImpl->patternResults[result.name] = result;
    
    NLM_LOG_INFO("Analyzed neural pattern: " + result.name + " with similarity " + std::to_string(result.similarity));
    return result;
}

PatternAnalysisResult AdvancedAnalysis::analyzeSpikingPattern(const std::vector<float>& data) const {
    PatternAnalysisResult result;
    result.type = NeuralPatternType::SpikingPattern;
    
    // Analyze spiking pattern statistics
    float mean = pImpl->calculateMean(data);
    float stddev = pImpl->calculateStandardDeviation(data);
    
    result.similarity = 1.0f - std::abs(mean) / 10.0f;  // Simplified similarity measure
    result.metrics["mean"] = mean;
    result.metrics["stddev"] = stddev;
    result.metrics["cv"] = stddev > 0.0f ? stddev / std::abs(mean) : 0.0f;  // Coefficient of variation
    
    // Generate signature
    for (float value : data) {
        result.signature.push_back(std::tanh(value));
    }
    
    result.confidence = 1.0f - 0.5f * std::min(1.0f, stddev);  // Confidence based on variability
    
    return result;
}

PatternAnalysisResult AdvancedAnalysis::analyzeRatePattern(const std::vector<float>& data) const {
    PatternAnalysisResult result;
    result.type = NeuralPatternType::RatePattern;
    
    // Analyze firing rate pattern
    float mean = pImpl->calculateMean(data);
    float min = *std::min_element(data.begin(), data.end());
    float max = *std::max_element(data.begin(), data.end());
    
    result.similarity = 1.0f - (max - min) / (mean * 10.0f);  // Normalized range
    result.metrics["mean"] = mean;
    result.metrics["min"] = min;
    result.metrics["max"] = max;
    result.metrics["range"] = max - min;
    
    // Check for rhythmic patterns
    float periodicity = 0.0f;
    if (data.size() >= 3) {
        // Simple periodicity detection
        float sumDiff = 0.0f;
        for (size_t i = 2; i < data.size(); ++i) {
            sumDiff += std::abs(data[i] - data[i-2]);
        }
        periodicity = 1.0f - sumDiff / (data.size() - 2) / 10.0f;
    }
    result.metrics["periodicity"] = periodicity;
    
    return result;
}

PatternAnalysisResult AdvancedAnalysis::analyzePhasePattern(const std::vector<float>& data) const {
    PatternAnalysisResult result;
    result.type = NeuralPatternType::PhasePattern;
    
    // Analyze phase relationships (simplified)
    float mean = pImpl->calculateMean(data);
    float variance = pImpl->calculateVariance(data);
    
    result.similarity = 1.0f / (1.0f + variance);  // Lower variance = higher similarity
    result.metrics["mean"] = mean;
    result.metrics["variance"] = variance;
    result.metrics["coherence"] = 1.0f - std::sqrt(variance / 10.0f);
    
    return result;
}

PatternAnalysisResult AdvancedAnalysis::analyzeSpatialPattern(const std::vector<float>& data) const {
    PatternAnalysisResult result;
    result.type = NeuralPatternType::SpatialPattern;
    
    // Analyze spatial distribution (simplified 1D case)
    float mean = pImpl->calculateMean(data);
    float stddev = pImpl->calculateStandardDeviation(data);
    
    result.similarity = 1.0f / (1.0f + stddev);  // Lower stddev = more clustered
    result.metrics["mean"] = mean;
    result.metrics["stddev"] = stddev;
    result.metrics["entropy"] = 1.0f - std::min(1.0f, stddev);
    
    return result;
}

PatternAnalysisResult AdvancedAnalysis::analyzeFrequencyPattern(const std::vector<float>& data) const {
    PatternAnalysisResult result;
    result.type = NeuralPatternType::FrequencyPattern;
    
    // Analyze frequency distribution
    std::map<size_t, size_t> histogram;
    float min = *std::min_element(data.begin(), data.end());
    float max = *std::max_element(data.begin(), data.end());
    size_t numBins = static_cast<size_t>(std::min(10.0f, max - min + 1.0f));
    
    if (numBins > 0) {
        float binWidth = (max - min) / numBins;
        for (float value : data) {
            size_t bin = static_cast<size_t>((value - min) / binWidth);
            if (bin >= numBins) bin = numBins - 1;
            histogram[bin]++;
        }
        
        // Calculate entropy
        float entropy = 0.0f;
        size_t totalCount = data.size();
        for (const auto& pair : histogram) {
            float probability = static_cast<float>(pair.second) / totalCount;
            if (probability > 0.0f) {
                entropy -= probability * std::log(probability);
            }
        }
        
        result.metrics["entropy"] = entropy;
        result.similarity = entropy / std::log(static_cast<float>(numBins));  // Normalized entropy
    }
    
    return result;
}

PatternAnalysisResult AdvancedAnalysis::analyzeCrossRegionPattern(const std::vector<float>& data) const {
    PatternAnalysisResult result;
    result.type = NeuralPatternType::CrossRegionPattern;
    
    // Analyze cross-region interactions (simplified)
    float mean = pImpl->calculateMean(data);
    float correlation = 0.0f;
    
    if (data.size() >= 2) {
        // Simple auto-correlation at lag 1
        correlation = pImpl->calculatePearsonCorrelation(
            std::vector<size_t>(data.size(), 0),  // Dummy x
            data
        );
    }
    
    result.similarity = 1.0f - std::abs(correlation);  // Lower correlation = more independent
    result.metrics["mean"] = mean;
    result.metrics["correlation"] = correlation;
    result.metrics["autocorrelation"] = correlation;
    
    return result;
}

PatternAnalysisResult AdvancedAnalysis::analyzeCustomPattern(const std::vector<float>& data) const {
    PatternAnalysisResult result;
    result.type = NeuralPatternType::CustomPattern;
    
    // Generic custom pattern analysis
    float mean = pImpl->calculateMean(data);
    float stddev = pImpl->calculateStandardDeviation(data);
    
    result.similarity = 1.0f / (1.0f + stddev);  // Similarity decreases with variability
    result.metrics["mean"] = mean;
    result.metrics["stddev"] = stddev;
    
    // Calculate skewness and kurtosis (simplified)
    if (data.size() >= 3) {
        float skewness = 0.0f;
        float kurtosis = 0.0f;
        
        for (float value : data) {
            skewness += (value - mean) * (value - mean) * (value - mean);
            kurtosis += (value - mean) * (value - mean) * (value - mean) * (value - mean);
        }
        
        skewness /= data.size();
        kurtosis /= data.size();
        
        result.metrics["skewness"] = skewness;
        result.metrics["kurtosis"] = kurtosis;
    }
    
    return result;
}

std::vector<PatternAnalysisResult> AdvancedAnalysis::analyzeMultiplePatterns(
    const std::vector<std::vector<float>>& datasets,
    NeuralPatternType patternType) const {
    std::vector<PatternAnalysisResult> results;
    
    for (size_t i = 0; i < datasets.size(); ++i) {
        results.push_back(analyzeNeuralPattern(datasets[i], patternType, "Pattern_" + std::to_string(i)));
    }
    
    return results;
}

bool AdvancedAnalysis::detectPatternAnomalies(const std::vector<float>& data,
                                             float threshold) const {
    if (data.size() < 3) return false;
    
    float mean = pImpl->calculateMean(data);
    float stddev = pImpl->calculateStandardDeviation(data);
    
    // Check for outliers using z-score
    for (float value : data) {
        float zScore = std::abs(value - mean) / (stddev > 0.0f ? stddev : 1.0f);
        if (zScore > threshold) {
            NLM_LOG_INFO("Detected pattern anomaly: z-score = " + std::to_string(zScore));
            return true;
        }
    }
    
    return false;
}

LearningCurveResult AdvancedAnalysis::analyzeLearningCurve(const std::vector<size_t>& steps,
                                                           const std::vector<float>& metricValues,
                                                           const std::string& metricName,
                                                           const std::string& interpolation) const {
    LearningCurveResult result;
    result.metric = metricName;
    
    if (steps.empty() || metricValues.empty() || steps.size() != metricValues.size()) {
        NLM_LOG_WARNING("Invalid data for learning curve analysis");
        return result;
    }
    
    pImpl->totalAnalysisOperations++;
    
    // Store steps and values
    result.steps = steps;
    result.curve = metricValues;
    
    // Fit curve based on interpolation type
    if (interpolation == "exponential") {
        result.fitParameters = pImpl->fitExponentialCurve(steps, metricValues);
    } else {
        // Default linear/spline fit
        result.fitParameters["slope"] = calculateLearningSlope(steps, metricValues);
        result.fitParameters["intercept"] = calculateLearningIntercept(steps, metricValues);
        result.fitParameters["r_squared"] = calculateRSquared(steps, metricValues);
    }
    
    // Calculate statistics
    result.statistics["mean"] = pImpl->calculateMean(metricValues);
    result.statistics["min"] = *std::min_element(metricValues.begin(), metricValues.end());
    result.statistics["max"] = *std::max_element(metricValues.begin(), metricValues.end());
    result.statistics["stddev"] = pImpl->calculateStandardDeviation(metricValues);
    
    // Calculate convergence point (where curve reaches 95% of asymptote)
    float asymptote = result.statistics["max"];
    float convergenceThreshold = 0.95f * asymptote;
    
    for (size_t i = 0; i < metricValues.size(); ++i) {
        if (metricValues[i] >= convergenceThreshold) {
            result.convergencePoint = static_cast<float>(steps[i]);
            break;
        }
    }
    
    // Store result
    pImpl->curveResults["Curve_" + metricName] = result;
    
    NLM_LOG_INFO("Analyzed learning curve: " + metricName + ", convergence at step " + std::to_string(result.convergencePoint));
    return result;
}

std::vector<LearningCurveResult> AdvancedAnalysis::analyzeMultipleLearningCurves(
    const std::map<std::string, std::vector<std::pair<size_t, float>>>& curves) const {
    std::vector<LearningCurveResult> results;
    
    for (const auto& curvePair : curves) {
        std::vector<size_t> steps;
        std::vector<float> values;
        
        for (const auto& point : curvePair.second) {
            steps.push_back(point.first);
            values.push_back(point.second);
        }
        
        results.push_back(analyzeLearningCurve(steps, values, curvePair.first));
    }
    
    return results;
}

float AdvancedAnalysis::predictLearningProgress(const LearningCurveResult& curve, size_t futureSteps) const {
    if (curve.fitParameters.empty()) return 0.0f;
    
    // Predict future values based on fitted parameters
    float predictedValue = 0.0f;
    
    if (curve.fitParameters.find("growth_rate") != curve.fitParameters.end()) {
        // Exponential model
        float initial = curve.fitParameters.at("initial");
        float growthRate = curve.fitParameters.at("growth_rate");
        predictedValue = initial * std::exp(growthRate * static_cast<float>(futureSteps));
    } else {
        // Linear model
        float slope = curve.fitParameters.at("slope");
        float intercept = curve.fitParameters.at("intercept");
        predictedValue = slope * static_cast<float>(futureSteps) + intercept;
    }
    
    return std::max(0.0f, predictedValue);  // Ensure non-negative
}

StatisticalAnalysisResult AdvancedAnalysis::performStatisticalTest(const std::vector<float>& data1,
                                                                   const std::vector<float>& data2,
                                                                   const std::string& testType) const {
    StatisticalAnalysisResult result;
    result.testName = testType + "_test_" + std::to_string(statisticalResults.size());
    
    if (data1.empty() || data2.empty()) {
        NLM_LOG_WARNING("Empty data for statistical test");
        return result;
    }
    
    pImpl->totalAnalysisOperations++;
    
    if (testType == "t-test") {
        result = performTTest(data1, data2);
    } else if (testType == "correlation") {
        result = performCorrelationTest(data1, data2);
    } else if (testType == "regression") {
        result = performRegressionTest(data1, data2);
    }
    
    // Store result
    pImpl->statisticalResults[result.testName] = result;
    
    NLM_LOG_INFO("Performed statistical test: " + testType + ", p-value = " + std::to_string(result.pValue));
    return result;
}

StatisticalAnalysisResult AdvancedAnalysis::performRegressionAnalysis(const std::vector<size_t>& x,
                                                                      const std::vector<float>& y,
                                                                      const std::string& regressionType) const {
    StatisticalAnalysisResult result;
    result.testName = "regression_" + regressionType + "_analysis";
    
    if (x.size() != y.size() || x.size() < 2) {
        NLM_LOG_WARNING("Invalid data for regression analysis");
        return result;
    }
    
    // Perform linear regression
    float slope = calculateLearningSlope(x, y);
    float intercept = calculateLearningIntercept(x, y);
    float rSquared = calculateRSquared(x, y);
    
    result.testStatistic = rSquared;
    result.pValue = 1.0f - rSquared;  // Simplified p-value calculation
    result.effectSize = slope;
    
    // Calculate confidence interval for slope (simplified)
    float stdErr = std::sqrt((1.0f - rSquared) / (x.size() - 2));
    result.confidenceInterval = {
        slope - 1.96f * stdErr,  // 95% CI
        slope + 1.96f * stdErr
    };
    
    // Interpretation
    if (result.pValue < pImpl->config.significanceLevel) {
        result.interpretation = "Significant relationship found (p < " + std::to_string(pImpl->config.significanceLevel) + ")";
    } else {
        result.interpretation = "No significant relationship found (p > " + std::to_string(pImpl->config.significanceLevel) + ")";
    }
    
    return result;
}

StatisticalAnalysisResult AdvancedAnalysis::performANOVA(const std::vector<std::vector<float>>& groups) const {
    StatisticalAnalysisResult result;
    result.testName = "ANOVA_analysis";
    
    if (groups.size() < 2) {
        NLM_LOG_WARNING("Need at least 2 groups for ANOVA");
        return result;
    }
    
    // Perform one-way ANOVA (simplified)
    float overallMean = pImpl->calculateMean(flattenVectors(groups));
    float betweenGroupSS = 0.0f;
    float withinGroupSS = 0.0f;
    
    for (size_t g = 0; g < groups.size(); ++g) {
        float groupMean = pImpl->calculateMean(groups[g]);
        float groupSS = 0.0f;
        
        for (float value : groups[g]) {
            float diff = value - groupMean;
            groupSS += diff * diff;
            withinGroupSS += diff * diff;
        }
        
        betweenGroupSS += static_cast<float>(groups[g].size()) * (groupMean - overallMean) * (groupMean - overallMean);
    }
    
    float totalSS = betweenGroupSS + withinGroupSS;
    
    if (totalSS > 0.0f) {
        // Calculate F-statistic
        float dfBetween = static_cast<float>(groups.size()) - 1.0f;
        float dfWithin = static_cast<float>(totalSS) - static_cast<float>(groups.size());  // Simplified
        
        if (dfWithin > 0.0f) {
            float fStatistic = (betweenGroupSS / dfBetween) / (withinGroupSS / dfWithin);
            result.testStatistic = fStatistic;
            
            // Calculate p-value (simplified)
            result.pValue = std::exp(-fStatistic / 4.0f);  // Approximation
            
            // Calculate effect size (eta squared)
            result.effectSize = betweenGroupSS / totalSS;
        }
    }
    
    // Set confidence interval
    if (result.testStatistic > 0.0f) {
        float ciWidth = 2.0f * std::sqrt(2.0f / result.testStatistic);  // Simplified
        result.confidenceInterval = {result.testStatistic - ciWidth, result.testStatistic + ciWidth};
    }
    
    // Interpretation
    if (result.pValue < pImpl->config.significanceLevel) {
        result.interpretation = "Significant differences between groups (p < " + std::to_string(pImpl->config.significanceLevel) + ")";
    } else {
        result.interpretation = "No significant differences between groups (p > " + std::to_string(pImpl->config.significanceLevel) + ")";
    }
    
    return result;
}

StatisticalAnalysisResult AdvancedAnalysis::performTTest(const std::vector<float>& data1,
                                                        const std::vector<float>& data2) const {
    StatisticalAnalysisResult result;
    result.testName = "t_test";
    
    if (data1.size() < 2 || data2.size() < 2) {
        NLM_LOG_WARNING("Need at least 2 data points per group for t-test");
        return result;
    }
    
    float mean1 = pImpl->calculateMean(data1);
    float mean2 = pImpl->calculateMean(data2);
    float std1 = pImpl->calculateStandardDeviation(data1);
    float std2 = pImpl->calculateStandardDeviation(data2);
    
    // Pooled standard deviation
    float pooledStd = std::sqrt(((data1.size() - 1) * std1 * std1 + (data2.size() - 1) * std2 * std2) /
                               (data1.size() + data2.size() - 2));
    
    if (pooledStd > 0.0f) {
        float tStatistic = (mean1 - mean2) / pooledStd;
        result.testStatistic = tStatistic;
        
        // Calculate p-value (simplified)
        result.pValue = 2.0f * (1.0f - std::erf(std::abs(tStatistic) / std::sqrt(2.0f)));  // Two-tailed
        
        // Calculate effect size (Cohen's d)
        result.effectSize = (mean1 - mean2) / pooledStd;
    }
    
    // Set confidence interval for mean difference
    if (pooledStd > 0.0f) {
        float margin = 1.96f * pooledStd * std::sqrt(1.0f / data1.size() + 1.0f / data2.size());
        result.confidenceInterval = {
            (mean1 - mean2) - margin,
            (mean1 - mean2) + margin
        };
    }
    
    // Interpretation
    if (result.pValue < pImpl->config.significanceLevel) {
        result.interpretation = "Significant difference between groups (p < " + std::to_string(pImpl->config.significanceLevel) + ")";
    } else {
        result.interpretation = "No significant difference between groups (p > " + std::to_string(pImpl->config.significanceLevel) + ")";
    }
    
    return result;
}

StatisticalAnalysisResult AdvancedAnalysis::performCorrelationTest(const std::vector<float>& data1,
                                                                  const std::vector<float>& data2) const {
    StatisticalAnalysisResult result;
    result.testName = "correlation_test";
    
    if (data1.size() != data2.size() || data1.size() < 2) {
        NLM_LOG_WARNING("Need equal number of data points for correlation test");
        return result;
    }
    
    float correlation = pImpl->calculatePearsonCorrelation(
        std::vector<size_t>(data1.size(), 0),  // Dummy x
        data2
    );
    
    result.testStatistic = correlation;
    
    // Calculate p-value (simplified)
    float n = static_cast<float>(data1.size());
    float t = correlation * std::sqrt((n - 2) / (1 - correlation * correlation));
    result.pValue = 2.0f * (1.0f - std::erf(std::abs(t) / std::sqrt(2.0f)));
    
    // Calculate effect size (Pearson's r is already effect size)
    result.effectSize = correlation;
    
    // Set confidence interval (simplified)
    float se = std::sqrt((1.0f - correlation * correlation) / (n - 2));
    result.confidenceInterval = {
        correlation - 1.96f * se,
        correlation + 1.96f * se
    };
    
    // Interpretation
    if (std::abs(correlation) > 0.5f) {
        result.interpretation = "Strong correlation";
    } else if (std::abs(correlation) > 0.3f) {
        result.interpretation = "Moderate correlation";
    } else {
        result.interpretation = "Weak correlation";
    }
    
    return result;
}

StatisticalAnalysisResult AdvancedAnalysis::performRegressionTest(const std::vector<float>& data1,
                                                                 const std::vector<float>& data2) const {
    StatisticalAnalysisResult result;
    result.testName = "regression_test";
    
    // Simplified regression test
    float slope = calculateLearningSlope(std::vector<size_t>(data1.size(), 0), data2);
    float intercept = calculateLearningIntercept(std::vector<size_t>(data1.size(), 0), data2);
    
    result.testStatistic = slope;
    result.pValue = 1.0f - std::abs(slope);  // Simplified
    result.effectSize = slope;
    
    return result;
}

PatternAnalysisResult AdvancedAnalysis::analyzeNeuralDynamics(const Brain* brain,
                                                             const std::string& analysisType) const {
    PatternAnalysisResult result;
    result.type = NeuralPatternType::SpikingPattern;
    result.name = "NeuralDynamics_" + analysisType;
    
    if (!brain) {
        NLM_LOG_WARNING("Brain pointer is null for neural dynamics analysis");
        return result;
    }
    
    // Collect neural activity data
    std::vector<float> neuralActivity;
    
    for (const auto& region : brain->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                neuralActivity.push_back(std::abs(state.membranePotential - state.restingPotential));
            }
        }
    }
    
    // Analyze based on analysis type
    if (analysisType == "spiking") {
        result = analyzeSpikingPattern(neuralActivity);
    } else if (analysisType == "rate") {
        result = analyzeRatePattern(neuralActivity);
    } else if (analysisType == "connectivity") {
        result = analyzeNetworkConnectivity(brain);
    }
    
    // Store result
    pImpl->patternResults[result.name] = result;
    
    return result;
}

PatternAnalysisResult AdvancedAnalysis::analyzeNetworkConnectivity(const Brain* brain) const {
    PatternAnalysisResult result;
    result.type = NeuralPatternType::CrossRegionPattern;
    result.name = "NetworkConnectivity";
    
    if (!brain) {
        return result;
    }
    
    // Analyze connectivity patterns
    std::vector<float> connectivityData;
    
    for (const auto& region : brain->getRegions()) {
        for (const auto* syn : region->getSynapses()) {
            connectivityData.push_back(syn->getWeight());
        }
    }
    
    // Analyze connectivity statistics
    float meanWeight = pImpl->calculateMean(connectivityData);
    float stddevWeight = pImpl->calculateStandardDeviation(connectivityData);
    
    result.similarity = 1.0f / (1.0f + stddevWeight);  // Lower stddev = more stable network
    result.metrics["mean_weight"] = meanWeight;
    result.metrics["stddev_weight"] = stddevWeight;
    result.metrics["connection_density"] = static_cast<float>(brain->getTotalSynapseCount()) / 
                                          (static_cast<float>(brain->getTotalNeuronCount()) * brain->getTotalNeuronCount());
    
    return result;
}

PatternAnalysisResult AdvancedAnalysis::analyzeRegionalActivity(const Brain* brain,
                                                               const std::vector<RegionId>& regions) const {
    PatternAnalysisResult result;
    result.type = NeuralPatternType::SpatialPattern;
    result.name = "RegionalActivity";
    
    if (!brain) {
        return result;
    }
    
    // Analyze activity in specific regions
    std::vector<float> regionalActivity;
    
    for (RegionId regionId : regions) {
        const NeuralRegion* region = brain->getRegion(regionId);
        if (region) {
            float regionActivity = static_cast<float>(region->getActiveNeuronCount()) / 
                                  static_cast<float>(region->getTotalNeuronCount());
            regionalActivity.push_back(regionActivity);
        }
    }
    
    // Analyze regional activity pattern
    if (!regionalActivity.empty()) {
        float meanActivity = pImpl->calculateMean(regionalActivity);
        float activityVariance = pImpl->calculateVariance(regionalActivity);
        
        result.similarity = 1.0f / (1.0f + activityVariance);  // Lower variance = more uniform
        result.metrics["mean_activity"] = meanActivity;
        result.metrics["activity_variance"] = activityVariance;
        result.metrics["regional_dominance"] = *std::max_element(regionalActivity.begin(), regionalActivity.end());
    }
    
    return result;
}

LearningCurveResult AdvancedAnalysis::analyzePlasticityLearning(const std::vector<float>& plasticityValues,
                                                               const std::vector<size_t>& steps) const {
    LearningCurveResult result;
    result.metric = "plasticity_learning";
    
    if (plasticityValues.empty() || steps.empty() || plasticityValues.size() != steps.size()) {
        NLM_LOG_WARNING("Invalid data for plasticity learning analysis");
        return result;
    }
    
    result.steps = steps;
    result.curve = plasticityValues;
    
    // Analyze learning metrics
    result.statistics["mean"] = pImpl->calculateMean(plasticityValues);
    result.statistics["final"] = plasticityValues.back();
    result.statistics["learning_rate"] = calculateLearningRate(plasticityValues, steps);
    
    // Store result
    pImpl->curveResults[result.metric] = result;
    
    return result;
}

LearningCurveResult AdvancedAnalysis::analyzeDevelopmentLearning(const std::vector<float>& developmentMetrics,
                                                                 const std::vector<size_t>& steps) const {
    LearningCurveResult result;
    result.metric = "development_learning";
    
    if (developmentMetrics.empty() || steps.empty() || developmentMetrics.size() != steps.size()) {
        NLM_LOG_WARNING("Invalid data for development learning analysis");
        return result;
    }
    
    result.steps = steps;
    result.curve = developmentMetrics;
    
    // Analyze development metrics
    result.statistics["mean"] = pImpl->calculateMean(developmentMetrics);
    result.statistics["final"] = developmentMetrics.back();
    
    // Calculate developmental trajectory
    if (developmentMetrics.size() >= 3) {
        float acceleration = 0.0f;
        for (size_t i = 1; i < developmentMetrics.size() - 1; ++i) {
            acceleration += developmentMetrics[i+1] - developmentMetrics[i];
        }
        acceleration /= static_cast<float>(developmentMetrics.size() - 2);
        result.statistics["acceleration"] = acceleration;
    }
    
    // Store result
    pImpl->curveResults[result.metric] = result;
    
    return result;
}

std::vector<float> AdvancedAnalysis::flattenVectors(const std::vector<std::vector<float>>& vectors) const {
    std::vector<float> flat;
    for (const auto& vector : vectors) {
        flat.insert(flat.end(), vector.begin(), vector.end());
    }
    return flat;
}

float AdvancedAnalysis::calculateLearningSlope(const std::vector<size_t>& steps,
                                               const std::vector<float>& values) const {
    if (steps.size() < 2 || values.size() < 2) return 0.0f;
    
    // Linear regression slope
    float xMean = pImpl->calculateMean(std::vector<float>(steps.begin(), steps.end()));
    float yMean = pImpl->calculateMean(values);
    
    float numerator = 0.0f;
    float denominator = 0.0f;
    
    for (size_t i = 0; i < steps.size(); ++i) {
        float xDiff = static_cast<float>(steps[i]) - xMean;
        float yDiff = values[i] - yMean;
        numerator += xDiff * yDiff;
        denominator += xDiff * xDiff;
    }
    
    if (denominator > 0.0f) {
        return numerator / denominator;
    }
    
    return 0.0f;
}

float AdvancedAnalysis::calculateLearningIntercept(const std::vector<size_t>& steps,
                                                   const std::vector<float>& values) const {
    if (steps.empty() || values.empty()) return 0.0f;
    
    float xMean = pImpl->calculateMean(std::vector<float>(steps.begin(), steps.end()));
    float yMean = pImpl->calculateMean(values);
    float slope = calculateLearningSlope(steps, values);
    
    return yMean - slope * xMean;
}

float AdvancedAnalysis::calculateRSquared(const std::vector<size_t>& steps,
                                          const std::vector<float>& values) const {
    if (steps.size() < 2 || values.size() < 2) return 0.0f;
    
    float xMean = pImpl->calculateMean(std::vector<float>(steps.begin(), steps.end()));
    float yMean = pImpl->calculateMean(values);
    
    float slope = calculateLearningSlope(steps, values);
    float intercept = calculateLearningIntercept(steps, values);
    
    float ssTot = 0.0f;
    float ssRes = 0.0f;
    
    for (size_t i = 0; i < steps.size(); ++i) {
        float predicted = slope * static_cast<float>(steps[i]) + intercept;
        ssTot += (values[i] - yMean) * (values[i] - yMean);
        ssRes += (values[i] - predicted) * (values[i] - predicted);
    }
    
    if (ssTot > 0.0f) {
        return 1.0f - (ssRes / ssTot);
    }
    
    return 0.0f;
}

float AdvancedAnalysis::calculateLearningRate(const std::vector<float>& metricValues,
                                             const std::vector<size_t>& steps) const {
    if (metricValues.size() < 2 || steps.size() < 2) return 0.0f;
    
    // Calculate average learning rate across the period
    float totalChange = metricValues.back() - metricValues.front();
    float totalSteps = static_cast<float>(steps.back() - steps.front());
    
    if (totalSteps > 0.0f) {
        return totalChange / totalSteps;
    }
    
    return 0.0f;
}

void AdvancedAnalysis::clearResults() {
    pImpl->patternResults.clear();
    pImpl->curveResults.clear();
    pImpl->statisticalResults.clear();
    NLM_LOG_INFO("Cleared all analysis results");
}

void AdvancedAnalysis::clearResults(const std::string& resultType) {
    if (resultType == "patterns") {
        pImpl->patternResults.clear();
        NLM_LOG_INFO("Cleared pattern analysis results");
    } else if (resultType == "curves") {
        pImpl->curveResults.clear();
        NLM_LOG_INFO("Cleared learning curve results");
    } else if (resultType == "statistics") {
        pImpl->statisticalResults.clear();
        NLM_LOG_INFO("Cleared statistical analysis results");
    }
}

void AdvancedAnalysis::enableMonitoring(bool enable) {
    pImpl->monitoringEnabled = enable;
    NLM_LOG_INFO("Analysis monitoring " + std::string(enable ? "enabled" : "disabled"));
}

bool AdvancedAnalysis::isMonitoringEnabled() const {
    return pImpl->monitoringEnabled;
}

void AdvancedAnalysis::logAnalysisStatus() const {
    NLM_LOG_INFO("=== Advanced Analysis Status ===");
    NLM_LOG_INFO("Pattern Results: " + std::to_string(pImpl->patternResults.size()));
    NLM_LOG_INFO("Curve Results: " + std::to_string(pImpl->curveResults.size()));
    NLM_LOG_INFO("Statistical Results: " + std::to_string(pImpl->statisticalResults.size()));
    NLM_LOG_INFO("Total Operations: " + std::to_string(pImpl->totalAnalysisOperations));
    NLM_LOG_INFO("Total Analysis Time: " + std::to_string(pImpl->totalAnalysisTime) + "ms");
    
    if (pImpl->monitoringEnabled) {
        NLM_LOG_INFO("Monitoring: enabled");
    } else {
        NLM_LOG_INFO("Monitoring: disabled");
    }
    
    NLM_LOG_INFO("=== End Advanced Analysis Status ===");
}

void AdvancedAnalysis::startStreamingAnalysis(const std::string& analysisType) {
    pImpl->streamingEnabled = true;
    pImpl->currentStreamType = analysisType;
    pImpl->streamingBuffer.clear();
    
    NLM_LOG_INFO("Started streaming analysis: " + analysisType);
}

void AdvancedAnalysis::stopStreamingAnalysis() {
    if (pImpl->streamingEnabled) {
        pImpl->streamingEnabled = false;
        pImpl->currentStreamType = "";
        
        NLM_LOG_INFO("Stopped streaming analysis");
    }
}

bool AdvancedAnalysis::isStreaming() const {
    return pImpl->streamingEnabled;
}

std::vector<float> AdvancedAnalysis::getStreamingResults() const {
    return pImpl->streamingBuffer;
}

void AdvancedAnalysis::enableParallelAnalysis(bool enable) {
    pImpl->parallelEnabled = enable;
    NLM_LOG_INFO("Parallel analysis " + std::string(enable ? "enabled" : "disabled"));
}

bool AdvancedAnalysis::isParallelAnalysisEnabled() const {
    return pImpl->parallelEnabled;
}

void AdvancedAnalysis::setAnalysisThreads(size_t count) {
    pImpl->analysisThreads = count;
    NLM_LOG_INFO("Set analysis thread count to " + std::to_string(count));
}

PatternAnalysisResult AdvancedAnalysis::getPatternAnalysisResult(const std::string& patternName) const {
    auto it = pImpl->patternResults.find(patternName);
    if (it != pImpl->patternResults.end()) {
        return it->second;
    }
    return PatternAnalysisResult();
}

LearningCurveResult AdvancedAnalysis::getLearningCurveResult(const std::string& curveName) const {
    auto it = pImpl->curveResults.find(curveName);
    if (it != pImpl->curveResults.end()) {
        return it->second;
    }
    return LearningCurveResult();
}

StatisticalAnalysisResult AdvancedAnalysis::getStatisticalResult(const std::string& testName) const {
    auto it = pImpl->statisticalResults.find(testName);
    if (it != pImpl->statisticalResults.end()) {
        return it->second;
    }
    return StatisticalAnalysisResult();
}

} // namespace nlm
