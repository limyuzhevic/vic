#pragma once

/**
 * Analyzer - Performance analysis tools for NLM neural simulation
 * 
 * Provides comprehensive analysis capabilities including:
 * - Activity pattern analysis
 * - Network topology analysis
 * - Learning curve visualization
 * - Performance trend analysis
 * - Resource utilization analysis
 * - Performance bottleneck detection
 * - Optimization opportunity identification
 * - Comparative analysis
 * - Statistical analysis
 * - Correlation analysis
 * - Anomaly detection
 * - Performance forecasting
 * - Efficiency analysis
 * - Scalability analysis
 * - Resource optimization
 * 
 * Integration:
 * - Integrates with PerformanceMonitor for metrics collection
 * - Provides analysis for Brain.cpp components
 * - Analyzes performance patterns and trends
 * - Identifies optimization opportunities
 */

#ifndef NLM_ANALYZER_HPP
#define NLM_ANALYZER_HPP

#include <chrono>
#include <deque>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <atomic>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <functional>
#include <numeric>
#include <random>
#include <filesystem>
#include <memory>
#include <limits>
#include <set>

namespace nlm {

// Activity pattern analysis
struct ActivityPattern {
    Timestamp startTime;               // Pattern start time
    Timestamp endTime;                 // Pattern end time
    std::string patternType;           // Type of pattern (burst, steady, oscillatory)
    double intensity;                  // Pattern intensity (0-1)
    size_t neuronCount;                // Number of neurons involved
    double avgFiringRate;              // Average firing rate during pattern
    double firingRateVariance;         // Variance in firing rates
    double synchrony;                  // Neural synchrony (0-1)
    std::vector<Timestamp> spikeTimes; // Spike times within pattern
    std::string description;           // Pattern description
};

// Network topology analysis
struct NetworkTopology {
    size_t nodeCount;                   // Number of nodes (neurons)
    size_t edgeCount;                   // Number of edges (synapses)
    double connectivityDensity;         // Network connectivity density (0-1)
    double clusteringCoefficient;       // Network clustering coefficient
    double averagePathLength;           // Average shortest path length
    double modularity;                  // Network modularity
    std::vector<size_t> degreeDistribution; // Node degree distribution
    std::vector<double> betweennessCentrality; // Betweenness centrality for each node
    std::vector<double> closenessCentrality;   // Closeness centrality for each node
    std::vector<double> eigenvectorCentrality; // Eigenvector centrality for each node
    std::string topologyType;           // Network topology type (random, small_world, scale_free)
    Timestamp timestamp;                // When topology was analyzed
};

// Learning curve analysis
struct LearningCurve {
    Timestamp startTime;                // Learning curve start time
    Timestamp endTime;                  // Learning curve end time
    size_t epochs;                       // Number of epochs
    std::vector<double> performanceValues; // Performance values per epoch
    std::vector<double> timePoints;     // Time points for each epoch
    std::vector<double> errorRates;     // Error rates per epoch
    double finalPerformance;            // Final performance value
    double learningRate;                // Learning rate
    double convergenceTime;             // Time to convergence
    std::string learningType;           // Learning type (supervised, unsupervised, reinforcement)
    Timestamp timestamp;                // When analysis was performed
};

// Performance trend analysis
struct PerformanceTrend {
    std::string metricName;             // Metric name
    std::vector<Timestamp> timePoints;  // Time points
    std::vector<double> values;        // Corresponding values
    double slope;                       // Trend slope
    double rSquared;                    // R-squared value
    std::string trendType;              // Trend type (increasing, decreasing, constant, oscillating)
    Timestamp startTime;                // Analysis start time
    Timestamp endTime;                  // Analysis end time
    Timestamp timestamp;                // When analysis was performed
};

// Resource utilization analysis
struct ResourceUtilization {
    Timestamp timestamp;                // Timestamp
    double cpuUtilization;             // CPU utilization (0-1)
    double memoryUtilization;          // Memory utilization (0-1)
    double ioUtilization;              // I/O utilization (0-1)
    double networkUtilization;         // Network utilization (0-1)
    double storageUtilization;         // Storage utilization (0-1)
    double energyConsumption;          // Energy consumption (Joules)
    double powerConsumption;            // Power consumption (Watts)
    Timestamp intervalStart;            // Interval start time
    Timestamp intervalEnd;              // Interval end time
};

// Performance bottleneck analysis
struct BottleneckInfo {
    std::string location;               // Location (file:line)
    std::string functionName;           // Function name
    double bottleneckScore;             // Bottleneck severity (0-1)
    double impactOnPerformance;         // Impact on overall performance
    std::string bottleneckType;         // Type (cpu, memory, io, network, algorithmic)
    std::string suggestion;             // Optimization suggestion
    Timestamp timestamp;                // When bottleneck was detected
};

// Optimization opportunity analysis
struct OptimizationOpportunity {
    std::string opportunityName;        // Opportunity name
    double potentialImprovement;        // Potential performance improvement (%)
    double effortRequired;              // Effort required (1-5 scale)
    double riskLevel;                   // Risk level (1-5 scale)
    std::string description;           // Opportunity description
    std::vector<std::string> implementations; // Possible implementations
    Timestamp timestamp;               // When opportunity was identified
};

// Anomaly detection result
struct AnomalyInfo {
    std::string anomalyType;            // Type of anomaly
    Timestamp anomalyTime;              // When anomaly occurred
    double anomalyScore;                // Anomaly severity (0-1)
    std::string description;           // Anomaly description
    std::string rootCause;             // Root cause
    std::vector<Timestamp> relatedTimestamps; // Related timestamps
    Timestamp timestamp;               // When analysis was performed
};

// Statistical analysis result
struct StatisticalAnalysis {
    std::string metricName;             // Metric name
    double mean;                        // Mean value
    double median;                      // Median value
    double mode;                        // Mode value
    double stdDev;                      // Standard deviation
    double variance;                    // Variance
    double min;                        // Minimum value
    double max;                        // Maximum value
    double range;                       // Range
    double iqr;                        // Interquartile range
    double skewness;                    // Skewness
    double kurtosis;                    // Kurtosis
    size_t sampleSize;                  // Sample size
    Timestamp startTime;                // Analysis start time
    Timestamp endTime;                  // Analysis end time
    Timestamp timestamp;                // When analysis was performed
};

// Correlation analysis result
struct CorrelationAnalysis {
    std::string metric1;                // First metric
    std::string metric2;                // Second metric
    double correlationCoefficient;      // Correlation coefficient (-1 to 1)
    double correlationStrength;         // Correlation strength (weak, moderate, strong)
    std::string correlationType;        // Correlation type (positive, negative, none)
    Timestamp startTime;                // Analysis start time
    Timestamp endTime;                  // Analysis end time
    Timestamp timestamp;                // When analysis was performed
};

// Performance forecasting result
struct PerformanceForecast {
    std::string forecastName;           // Forecast name
    Timestamp forecastTime;             // Forecast time
    double projectedPerformance;        // Projected performance
    double confidenceInterval;          // Confidence interval
    std::vector<Timestamp> forecastPoints; // Forecast points
    std::vector<double> forecastValues;  // Forecast values
    std::string forecastModel;          // Forecast model
    Timestamp timestamp;                // When forecast was generated
};

class Analyzer {
public:
    Analyzer();
    ~Analyzer();
    
    // Analysis management
    AnalysisResult analyze(const std::string& analysisName);
    std::vector<AnalysisResult> analyzeAll();
    bool isAnalyzing() const { return isAnalyzing_; }
    
    // Activity pattern analysis
    ActivityPattern analyzeActivityPattern(const std::string& patternType,
                                           const std::vector<Timestamp>& spikeTimes,
                                           const std::vector<size_t>& neuronIds);
    std::vector<ActivityPattern> detectActivityPatterns(const std::vector<Timestamp>& spikeTimes,
                                                        const std::vector<size_t>& neuronIds);
    
    // Network topology analysis
    NetworkTopology analyzeNetworkTopology(const std::vector<size_t>& nodeConnections,
                                          const std::vector<double>& connectionWeights);
    double calculateNetworkMetrics(const std::vector<size_t>& nodeConnections,
                                   std::vector<double>& centralityMeasures);
    
    // Learning curve analysis
    LearningCurve analyzeLearningCurve(const std::vector<double>& performanceValues,
                                       const std::vector<double>& timePoints,
                                       const std::string& learningType);
    double calculateLearningRate(const std::vector<double>& performanceValues,
                                const std::vector<double>& timePoints);
    
    // Performance trend analysis
    std::vector<PerformanceTrend> analyzePerformanceTrends(const std::map<std::string, std::vector<std::pair<Timestamp, double>>>& metricData);
    std::string identifyTrendType(const std::vector<double>& values);
    double calculateTrendSlope(const std::vector<double>& values, const std::vector<Timestamp>& timePoints);
    
    // Resource utilization analysis
    ResourceUtilization analyzeResourceUtilization(const std::vector<ResourceUtilization>& utilizationData);
    double calculateResourceEfficiency(const ResourceUtilization& utilization);
    
    // Performance bottleneck detection
    std::vector<BottleneckInfo> detectPerformanceBottlenecks(const std::vector<ActivityPattern>& activityPatterns,
                                                            const std::vector<BenchmarkResult>& benchmarkResults);
    BottleneckInfo analyzeSingleBottleneck(const std::string& location, const std::string& functionName,
                                          const std::vector<double>& performanceData);
    
    // Optimization opportunity identification
    std::vector<OptimizationOpportunity> identifyOptimizationOpportunities(const std::vector<BottleneckInfo>& bottlenecks,
                                                                         const std::vector<PerformanceTrend>& trends);
    OptimizationOpportunity analyzeSingleOptimization(const std::string& opportunityName,
                                                    const std::vector<double>& performanceData,
                                                    const std::string& description);
    
    // Anomaly detection
    std::vector<AnomalyInfo> detectAnomalies(const std::vector<PerformanceTrend>& trends,
                                            const std::vector<StatisticalAnalysis>& statistics);
    AnomalyInfo analyzeSingleAnomaly(const std::string& anomalyType, const std::vector<double>& values,
                                    const std::vector<Timestamp>& timestamps);
    
    // Statistical analysis
    StatisticalAnalysis analyzeStatistics(const std::vector<double>& values);
    std::vector<StatisticalAnalysis> analyzeStatistics(const std::map<std::string, std::vector<double>>& metricData);
    double calculateMean(const std::vector<double>& values);
    double calculateMedian(const std::vector<double>& values);
    double calculateStdDev(const std::vector<double>& values, double mean);
    
    // Correlation analysis
    std::vector<CorrelationAnalysis> analyzeCorrelations(const std::map<std::string, std::vector<double>>& metricData);
    CorrelationAnalysis analyzeSingleCorrelation(const std::string& metric1, const std::string& metric2,
                                                const std::vector<double>& values1, const std::vector<double>& values2);
    double calculateCorrelationCoefficient(const std::vector<double>& values1, const std::vector<double>& values2);
    
    // Performance forecasting
    PerformanceForecast forecastPerformance(const std::string& metricName,
                                           const std::vector<std::pair<Timestamp, double>>& historicalData,
                                           const std::string& forecastModel);
    std::vector<PerformanceForecast> forecastMultipleMetrics(const std::map<std::string, std::vector<std::pair<Timestamp, double>>>& metricData);
    
    // Export/import analysis results
    void exportAnalysis(const std::string& filename) const;
    void importAnalysis(const std::string& filename);
    
    // Statistics
    size_t getAnalysisCount() const;
    AnalysisResult getAnalysisResult(const std::string& analysisName) const;
    
    // Clear analysis results
    void clearAnalysisResults();
    
    // Configuration
    void setAnalysisInterval(double interval) { analysisInterval_ = interval; }
    double getAnalysisInterval() const { return analysisInterval_; }
    
    void enableActivityPatternAnalysis(bool enable) { enableActivityPatternAnalysis_ = enable; }
    void enableNetworkTopologyAnalysis(bool enable) { enableNetworkTopologyAnalysis_ = enable; }
    void enableLearningCurveAnalysis(bool enable) { enableLearningCurveAnalysis_ = enable; }
    void enablePerformanceTrendAnalysis(bool enable) { enablePerformanceTrendAnalysis_ = enable; }
    void enableResourceUtilizationAnalysis(bool enable) { enableResourceUtilizationAnalysis_ = enable; }
    
private:
    bool isAnalyzing_;
    double analysisInterval_;
    
    // Analysis results
    std::vector<AnalysisResult> analysisResults_;
    
    // Configuration
    bool enableActivityPatternAnalysis_;
    bool enableNetworkTopologyAnalysis_;
    bool enableLearningCurveAnalysis_;
    bool enablePerformanceTrendAnalysis_;
    bool enableResourceUtilizationAnalysis_;
    
    mutable std::mutex mutex_;
    
    // Helper methods
    void recordAnalysisResult(const AnalysisResult& result);
    AnalysisResult createDefaultAnalysisResult(const std::string& analysisName) const;
    std::string determineTrendType(const std::vector<double>& values);
    double calculateCorrelationStrength(double correlationCoefficient);
    std::string determineCorrelationType(double correlationCoefficient);
    std::string suggestOptimization(const std::string& bottleneckLocation, const std::string& bottleneckType);
    std::string generateImplementationSuggestions(const std::string& bottleneckLocation, const std::string& bottleneckType);
    double calculateAnomalyScore(const std::vector<double>& values, const std::vector<double>& mean, const std::vector<double>& stdDev);
};

#endif // NLM_ANALYZER_HPP