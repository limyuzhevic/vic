#include "Analyzer.hpp"
#include "PerformanceMonitor.hpp"
#include "Profiler.hpp"
#include "Benchmark.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <random>
#include <filesystem>
#include <chrono>
#include <thread>
#include <set>

namespace nlm {

Analyzer::Analyzer() 
    : isAnalyzing_(false)
    , analysisInterval_(1000.0)  // 1000 ms interval
    , enableActivityPatternAnalysis_(true)
    , enableNetworkTopologyAnalysis_(true)
    , enableLearningCurveAnalysis_(true)
    , enablePerformanceTrendAnalysis_(true)
    , enableResourceUtilizationAnalysis_(true)
{
}

Analyzer::~Analyzer() {
    clearAnalysisResults();
}

AnalysisResult Analyzer::analyze(const std::string& analysisName) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    AnalysisResult result;
    result.analysisName = analysisName;
    result.analysisType = "comprehensive";
    result.findings = "Analysis completed successfully";
    result.recommendations = "Monitor performance metrics regularly";
    result.score = 85.0;  // Default score
    result.timestamp = 0;  // Would get actual timestamp
    
    // Perform analysis based on analysis name
    if (analysisName == "activity_pattern") {
        result = analyzeActivityPatternAnalysis();
    } else if (analysisName == "network_topology") {
        result = analyzeNetworkTopologyAnalysis();
    } else if (analysisName == "learning_curve") {
        result = analyzeLearningCurveAnalysis();
    } else if (analysisName == "performance_trend") {
        result = analyzePerformanceTrendAnalysis();
    } else if (analysisName == "resource_utilization") {
        result = analyzeResourceUtilizationAnalysis();
    } else {
        result = createDefaultAnalysisResult(analysisName);
    }
    
    recordAnalysisResult(result);
    return result;
}

std::vector<AnalysisResult> Analyzer::analyzeAll() {
    std::vector<AnalysisResult> results;
    
    std::vector<std::string> analysisNames = {
        "activity_pattern",
        "network_topology", 
        "learning_curve",
        "performance_trend",
        "resource_utilization"
    };
    
    for (const auto& name : analysisNames) {
        if (isAnalysisEnabled(name)) {
            results.push_back(analyze(name));
        }
    }
    
    return results;
}

AnalysisResult Analyzer::createDefaultAnalysisResult(const std::string& analysisName) const {
    AnalysisResult result;
    result.analysisName = analysisName;
    result.analysisType = "default";
    result.findings = "Default analysis performed";
    result.recommendations = "No specific recommendations";
    result.score = 50.0;
    result.timestamp = 0;
    
    return result;
}

bool Analyzer::isAnalysisEnabled(const std::string& analysisName) const {
    if (analysisName == "activity_pattern") {
        return enableActivityPatternAnalysis_;
    } else if (analysisName == "network_topology") {
        return enableNetworkTopologyAnalysis_;
    } else if (analysisName == "learning_curve") {
        return enableLearningCurveAnalysis_;
    } else if (analysisName == "performance_trend") {
        return enablePerformanceTrendAnalysis_;
    } else if (analysisName == "resource_utilization") {
        return enableResourceUtilizationAnalysis_;
    }
    return false;
}

AnalysisResult Analyzer::analyzeActivityPatternAnalysis() {
    AnalysisResult result;
    result.analysisName = "activity_pattern";
    result.analysisType = "pattern_detection";
    result.findings = "Detected activity patterns in neural network";
    result.recommendations = "Consider optimizing for identified patterns";
    result.score = 85.0;
    result.timestamp = 0;
    
    // Simulate activity pattern analysis
    std::vector<ActivityPattern> patterns = detectActivityPatterns(
        std::vector<Timestamp>{1, 2, 3, 4, 5},  // Simulated spike times
        std::vector<size_t>{1, 2, 3, 4, 5}   // Simulated neuron IDs
    );
    
    // Extract findings from patterns
    if (!patterns.empty()) {
        result.findings = "Detected " + std::to_string(patterns.size()) + 
                         " activity patterns in neural network";
        
        double totalIntensity = 0.0;
        for (const auto& pattern : patterns) {
            totalIntensity += pattern.intensity;
        }
        
        result.score = std::min(100.0, 50.0 + totalIntensity * 35.0);
        
        if (patterns.size() > 3) {
            result.recommendations += ". Implement pattern-specific optimizations";
        }
    }
    
    return result;
}

AnalysisResult Analyzer::analyzeNetworkTopologyAnalysis() {
    AnalysisResult result;
    result.analysisName = "network_topology";
    result.analysisType = "topology_analysis";
    result.findings = "Analyzed network topology";
    result.recommendations = "Consider rewiring for better performance";
    result.score = 75.0;
    result.timestamp = 0;
    
    // Simulate network topology analysis
    NetworkTopology topology = analyzeNetworkTopology(
        std::vector<size_t>{1, 2, 2, 3, 3, 3},  // Simulated connections
        std::vector<double>{0.5, 0.7, 0.8, 0.6, 0.9, 0.4}  // Simulated weights
    );
    
    // Extract findings from topology
    result.findings = "Network has " + std::to_string(topology.nodeCount) + 
                     " nodes and " + std::to_string(topology.edgeCount) + " edges";
    
    if (topology.connectivityDensity < 0.5) {
        result.recommendations += ". Increase network connectivity";
        result.score -= 15.0;
    }
    
    if (topology.clusteringCoefficient < 0.3) {
        result.recommendations += ". Improve local clustering";
        result.score -= 10.0;
    }
    
    result.score = std::max(0.0, result.score);
    
    return result;
}

AnalysisResult Analyzer::analyzeLearningCurveAnalysis() {
    AnalysisResult result;
    result.analysisName = "learning_curve";
    result.analysisType = "learning_analysis";
    result.findings = "Analyzed learning curve";
    result.recommendations = "Consider adjusting learning rate";
    result.score = 80.0;
    result.timestamp = 0;
    
    // Simulate learning curve analysis
    LearningCurve curve = analyzeLearningCurve(
        std::vector<double>{0.1, 0.3, 0.5, 0.7, 0.8, 0.9, 0.95},  // Simulated performance
        std::vector<double>{1, 2, 3, 4, 5, 6, 7},              // Simulated time points
        "reinforcement"  // Simulated learning type
    );
    
    // Extract findings from curve
    result.findings = "Learning curve shows " + std::to_string(curve.epochs) + 
                     " epochs with final performance " + std::to_string(curve.finalPerformance);
    
    if (curve.learningRate < 0.1) {
        result.recommendations += ". Increase learning rate";
        result.score -= 20.0;
    }
    
    if (curve.convergenceTime > 100) {
        result.recommendations += ". Reduce convergence time";
        result.score -= 15.0;
    }
    
    result.score = std::max(0.0, result.score);
    
    return result;
}

AnalysisResult Analyzer::analyzePerformanceTrendAnalysis() {
    AnalysisResult result;
    result.analysisName = "performance_trend";
    result.analysisType = "trend_analysis";
    result.findings = "Analyzed performance trends";
    result.recommendations = "Monitor performance for degradation";
    result.score = 70.0;
    result.timestamp = 0;
    
    // Simulate performance trend analysis
    std::map<std::string, std::vector<std::pair<Timestamp, double>>> metricData;
    metricData["cpu_usage"] = {
        {1, 50.0}, {2, 55.0}, {3, 60.0}, {4, 65.0}, {5, 70.0}
    };
    metricData["memory_usage"] = {
        {1, 100.0}, {2, 110.0}, {3, 120.0}, {4, 130.0}, {5, 140.0}
    };
    
    std::vector<PerformanceTrend> trends = analyzePerformanceTrends(metricData);
    
    // Extract findings from trends
    if (!trends.empty()) {
        result.findings = "Identified " + std::to_string(trends.size()) + " performance trends";
        
        double totalTrendScore = 0.0;
        for (const auto& trend : trends) {
            if (trend.trendType == "increasing") {
                totalTrendScore += 10.0;
            } else if (trend.trendType == "decreasing") {
                totalTrendScore += 5.0;
            } else if (trend.trendType == "constant") {
                totalTrendScore += 8.0;
            }
        }
        
        result.score = std::min(100.0, 50.0 + totalTrendScore);
        
        bool hasIncreasingTrend = false;
        for (const auto& trend : trends) {
            if (trend.trendType == "increasing") {
                hasIncreasingTrend = true;
                break;
            }
        }
        
        if (hasIncreasingTrend) {
            result.recommendations += ". Investigate increasing resource usage";
        }
    }
    
    return result;
}

AnalysisResult Analyzer::analyzeResourceUtilizationAnalysis() {
    AnalysisResult result;
    result.analysisName = "resource_utilization";
    result.analysisType = "resource_analysis";
    result.findings = "Analyzed resource utilization";
    result.recommendations = "Optimize resource allocation";
    result.score = 75.0;
    result.timestamp = 0;
    
    // Simulate resource utilization analysis
    std::vector<ResourceUtilization> utilizationData;
    
    for (size_t i = 0; i < 10; ++i) {
        ResourceUtilization utilization;
        utilization.timestamp = i;
        utilization.cpuUtilization = 50.0 + (i * 5.0);
        utilization.memoryUtilization = 60.0 + (i * 3.0);
        utilization.ioUtilization = 30.0 + (i * 2.0);
        utilization.networkUtilization = 20.0 + (i * 1.0);
        utilization.storageUtilization = 40.0 + (i * 1.5);
        utilization.energyConsumption = 100.0 + (i * 10.0);
        utilization.powerConsumption = 50.0 + (i * 5.0);
        utilization.intervalStart = i * 100;
        utilization.intervalEnd = (i + 1) * 100;
        
        utilizationData.push_back(utilization);
    }
    
    ResourceUtilization analysis = analyzeResourceUtilization(utilizationData);
    
    // Extract findings from utilization analysis
    double avgCpuUtilization = analysis.cpuUtilization;
    double avgMemoryUtilization = analysis.memoryUtilization;
    
    result.findings = "Average CPU utilization: " + std::to_string(avgCpuUtilization) + "%";
    result.findings += ", Average memory utilization: " + std::to_string(avgMemoryUtilization) + "%";
    
    if (avgCpuUtilization > 80.0) {
        result.recommendations += ". Reduce CPU usage";
        result.score -= 20.0;
    }
    
    if (avgMemoryUtilization > 90.0) {
        result.recommendations += ". Reduce memory usage";
        result.score -= 20.0;
    }
    
    if (analysis.energyConsumption > 1500.0) {
        result.recommendations += ". Optimize energy consumption";
        result.score -= 15.0;
    }
    
    result.score = std::max(0.0, result.score);
    
    return result;
}

ActivityPattern Analyzer::analyzeActivityPattern(const std::string& patternType,
                                                 const std::vector<Timestamp>& spikeTimes,
                                                 const std::vector<size_t>& neuronIds) {
    ActivityPattern pattern;
    pattern.startTime = spikeTimes.empty() ? 0 : spikeTimes.front();
    pattern.endTime = spikeTimes.empty() ? 0 : spikeTimes.back();
    pattern.patternType = patternType;
    pattern.neuronCount = neuronIds.size();
    pattern.avgFiringRate = spikeTimes.empty() ? 0.0 : 
                            static_cast<double>(spikeTimes.size()) / 
                            static_cast<double>(pattern.endTime - pattern.startTime + 1);
    pattern.firingRateVariance = 0.0;  // Would calculate variance
    pattern.synchrony = 0.5;  // Simulated
    pattern.spikeTimes = spikeTimes;
    pattern.description = "Activity pattern of type " + patternType;
    
    // Calculate intensity based on firing rate
    pattern.intensity = std::min(1.0, pattern.avgFiringRate / 10.0);
    
    return pattern;
}

std::vector<ActivityPattern> Analyzer::detectActivityPatterns(const std::vector<Timestamp>& spikeTimes,
                                                               const std::vector<size_t>& neuronIds) {
    std::vector<ActivityPattern> patterns;
    
    if (spikeTimes.empty()) return patterns;
    
    // Simple pattern detection (in real implementation, use more sophisticated algorithms)
    size_t numPatterns = std::min(static_cast<size_t>(3), spikeTimes.size() / 2);
    
    for (size_t i = 0; i < numPatterns; ++i) {
        ActivityPattern pattern;
        pattern.startTime = spikeTimes[i * 2];
        pattern.endTime = spikeTimes[(i + 1) * 2 - 1];
        pattern.patternType = "burst";
        pattern.neuronCount = neuronIds.size();
        pattern.avgFiringRate = 2.0;
        pattern.firingRateVariance = 0.5;
        pattern.synchrony = 0.7;
        
        // Add spike times to pattern
        size_t startIdx = i * 2;
        size_t endIdx = std::min((i + 1) * 2, static_cast<size_t>(spikeTimes.size()));
        for (size_t j = startIdx; j < endIdx; ++j) {
            pattern.spikeTimes.push_back(spikeTimes[j]);
        }
        
        pattern.description = "Burst pattern from " + std::to_string(pattern.startTime) + 
                            " to " + std::to_string(pattern.endTime);
        pattern.intensity = 0.8;
        
        patterns.push_back(pattern);
    }
    
    return patterns;
}

NetworkTopology Analyzer::analyzeNetworkTopology(const std::vector<size_t>& nodeConnections,
                                                const std::vector<double>& connectionWeights) {
    NetworkTopology topology;
    topology.nodeCount = *std::max_element(nodeConnections.begin(), nodeConnections.end()) + 1;
    topology.edgeCount = nodeConnections.size();
    topology.connectivityDensity = static_cast<double>(topology.edgeCount) / 
                                  (topology.nodeCount * (topology.nodeCount - 1) / 2);
    topology.clusteringCoefficient = 0.6;  // Simulated
    topology.averagePathLength = 2.5;   // Simulated
    topology.modularity = 0.7;           // Simulated
    topology.topologyType = "small_world"; // Simulated
    topology.timestamp = 0;              // Would get actual timestamp
    
    // Calculate degree distribution
    std::vector<size_t> degreeCount(topology.nodeCount, 0);
    for (size_t connection : nodeConnections) {
        if (connection < degreeCount.size()) {
            degreeCount[connection]++;
        }
    }
    topology.degreeDistribution = degreeCount;
    
    // Calculate centrality measures
    size_t maxConnections = *std::max_element(nodeConnections.begin(), nodeConnections.end()) + 1;
    for (size_t i = 0; i < maxConnections && i < topology.nodeCount; ++i) {
        topology.betweennessCentrality.push_back(static_cast<double>(degreeCount[i]) / topology.edgeCount);
        topology.closenessCentrality.push_back(1.0 / (i + 1));
        topology.eigenvectorCentrality.push_back(static_cast<double>(degreeCount[i]) / topology.nodeCount);
    }
    
    return topology;
}

LearningCurve Analyzer::analyzeLearningCurve(const std::vector<double>& performanceValues,
                                             const std::vector<double>& timePoints,
                                             const std::string& learningType) {
    LearningCurve curve;
    curve.startTime = timePoints.empty() ? 0 : timePoints.front();
    curve.endTime = timePoints.empty() ? 0 : timePoints.back();
    curve.epochs = performanceValues.size();
    curve.performanceValues = performanceValues;
    curve.timePoints = timePoints;
    curve.errorRates.resize(performanceValues.size());
    
    // Calculate error rates (simplified)
    double maxPerformance = *std::max_element(performanceValues.begin(), performanceValues.end());
    double minPerformance = *std::min_element(performanceValues.begin(), performanceValues.end());
    double performanceRange = maxPerformance - minPerformance;
    
    for (size_t i = 0; i < performanceValues.size(); ++i) {
        curve.errorRates[i] = 1.0 - (performanceValues[i] - minPerformance) / performanceRange;
    }
    
    curve.finalPerformance = performanceValues.empty() ? 0.0 : performanceValues.back();
    curve.learningRate = calculateLearningRate(performanceValues, timePoints);
    curve.convergenceTime = 100; // Simulated
    curve.learningType = learningType;
    curve.timestamp = 0; // Would get actual timestamp
    
    return curve;
}

double Analyzer::calculateLearningRate(const std::vector<double>& performanceValues,
                                       const std::vector<double>& timePoints) {
    if (performanceValues.size() < 2) return 0.0;
    
    double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumXX = 0.0;
    size_t n = performanceValues.size();
    
    for (size_t i = 0; i < n; ++i) {
        double x = timePoints[i];
        double y = performanceValues[i];
        sumX += x;
        sumY += y;
        sumXY += x * y;
        sumXX += x * x;
    }
    
    double slope = (n * sumXY - sumX * sumY) / (n * sumXX - sumX * sumX);
    
    return std::max(0.0, slope);
}

std::vector<PerformanceTrend> Analyzer::analyzePerformanceTrends(const std::map<std::string, std::vector<std::pair<Timestamp, double>>>& metricData) {
    std::vector<PerformanceTrend> trends;
    
    for (const auto& metricPair : metricData) {
        const std::string& metricName = metricPair.first;
        const std::vector<std::pair<Timestamp, double>>& values = metricPair.second;
        
        if (values.size() < 2) continue;
        
        PerformanceTrend trend;
        trend.metricName = metricName;
        trend.startTime = values.front().first;
        trend.endTime = values.back().first;
        
        // Extract time points and values
        trend.timePoints.clear();
        trend.values.clear();
        for (const auto& pair : values) {
            trend.timePoints.push_back(pair.first);
            trend.values.push_back(pair.second);
        }
        
        // Calculate trend
        trend.slope = calculateTrendSlope(trend.values, trend.timePoints);
        trend.rSquared = 1.0; // Would calculate R-squared
        trend.trendType = identifyTrendType(trend.values);
        trend.timestamp = 0; // Would get actual timestamp
        
        trends.push_back(trend);
    }
    
    return trends;
}

std::string Analyzer::identifyTrendType(const std::vector<double>& values) {
    if (values.size() < 3) return "constant";
    
    // Simple trend identification (in real implementation, use more sophisticated methods)
    bool isIncreasing = true;
    bool isDecreasing = true;
    bool isConstant = true;
    
    for (size_t i = 1; i < values.size(); ++i) {
        if (values[i] > values[i-1]) {
            isDecreasing = false;
            isConstant = false;
        } else if (values[i] < values[i-1]) {
            isIncreasing = false;
            isConstant = false;
        } else {
            isIncreasing = false;
            isDecreasing = false;
        }
    }
    
    if (isIncreasing) return "increasing";
    else if (isDecreasing) return "decreasing";
    else if (isConstant) return "constant";
    else return "oscillating";
}

double Analyzer::calculateTrendSlope(const std::vector<double>& values, const std::vector<Timestamp>& timePoints) {
    if (values.size() < 2 || timePoints.size() != values.size()) return 0.0;
    
    double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumXX = 0.0;
    size_t n = values.size();
    
    for (size_t i = 0; i < n; ++i) {
        double x = static_cast<double>(timePoints[i]);
        double y = values[i];
        sumX += x;
        sumY += y;
        sumXY += x * y;
        sumXX += x * x;
    }
    
    double denominator = n * sumXX - sumX * sumX;
    if (denominator == 0.0) return 0.0;
    
    return (n * sumXY - sumX * sumY) / denominator;
}

ResourceUtilization Analyzer::analyzeResourceUtilization(const std::vector<ResourceUtilization>& utilizationData) {
    ResourceUtilization result;
    
    if (utilizationData.empty()) {
        result.timestamp = 0;
        return result;
    }
    
    // Calculate averages
    double sumCpu = 0.0, sumMemory = 0.0, sumIo = 0.0, sumNetwork = 0.0, sumStorage = 0.0;
    double sumEnergy = 0.0, sumPower = 0.0;
    
    for (const auto& utilization : utilizationData) {
        sumCpu += utilization.cpuUtilization;
        sumMemory += utilization.memoryUtilization;
        sumIo += utilization.ioUtilization;
        sumNetwork += utilization.networkUtilization;
        sumStorage += utilization.storageUtilization;
        sumEnergy += utilization.energyConsumption;
        sumPower += utilization.powerConsumption;
    }
    
    size_t count = utilizationData.size();
    
    result.timestamp = utilizationData.front().timestamp;
    result.cpuUtilization = sumCpu / count;
    result.memoryUtilization = sumMemory / count;
    result.ioUtilization = sumIo / count;
    result.networkUtilization = sumNetwork / count;
    result.storageUtilization = sumStorage / count;
    result.energyConsumption = sumEnergy / count;
    result.powerConsumption = sumPower / count;
    result.intervalStart = utilizationData.front().intervalStart;
    result.intervalEnd = utilizationData.back().intervalEnd;
    
    return result;
}

double Analyzer::calculateResourceEfficiency(const ResourceUtilization& utilization) {
    double totalUtilization = utilization.cpuUtilization + utilization.memoryUtilization +
                              utilization.ioUtilization + utilization.networkUtilization +
                              utilization.storageUtilization;
    
    return totalUtilization / 5.0;  // Average of all utilizations
}

std::vector<BottleneckInfo> Analyzer::detectPerformanceBottlenecks(const std::vector<ActivityPattern>& activityPatterns,
                                                                  const std::vector<BenchmarkResult>& benchmarkResults) {
    std::vector<BottleneckInfo> bottlenecks;
    
    // Analyze activity patterns for bottlenecks
    for (const auto& pattern : activityPatterns) {
        if (pattern.synchrony < 0.5) {
            BottleneckInfo bottleneck;
            bottleneck.location = "unknown";
            bottleneck.functionName = "synchrony_analysis";
            bottleneck.bottleneckScore = 1.0 - pattern.synchrony;
            bottleneck.impactOnPerformance = bottleneck.bottleneckScore * 100.0;
            bottleneck.bottleneckType = "algorithmic";
            bottleneck.suggestion = "Improve neural synchrony";
            bottleneck.timestamp = 0;
            
            bottlenecks.push_back(bottleneck);
        }
    }
    
    // Analyze benchmark results for bottlenecks
    for (const auto& benchmark : benchmarkResults) {
        if (benchmark.executionTime > 10.0) {
            BottleneckInfo bottleneck;
            bottleneck.location = benchmark.benchmarkName;
            bottleneck.functionName = "benchmark_execution";
            bottleneck.bottleneckScore = std::min(1.0, benchmark.executionTime / 100.0);
            bottleneck.impactOnPerformance = bottleneck.bottleneckScore * 100.0;
            bottleneck.bottleneckType = "algorithmic";
            bottleneck.suggestion = "Optimize " + benchmark.benchmarkName + " benchmark";
            bottleneck.timestamp = 0;
            
            bottlenecks.push_back(bottleneck);
        }
    }
    
    return bottlenecks;
}

BottleneckInfo Analyzer::analyzeSingleBottleneck(const std::string& location, const std::string& functionName,
                                                 const std::vector<double>& performanceData) {
    BottleneckInfo bottleneck;
    bottleneck.location = location;
    bottleneck.functionName = functionName;
    bottleneck.timestamp = 0;
    
    if (performanceData.empty()) {
        bottleneck.bottleneckScore = 0.0;
        bottleneck.impactOnPerformance = 0.0;
        bottleneck.bottleneckType = "unknown";
        bottleneck.suggestion = "Insufficient data";
        return bottleneck;
    }
    
    // Calculate average performance
    double sum = std::accumulate(performanceData.begin(), performanceData.end(), 0.0);
    double average = sum / performanceData.size();
    
    // Determine bottleneck based on performance data
    if (average < 1.0) {
        bottleneck.bottleneckScore = 0.8;
        bottleneck.impactOnPerformance = 80.0;
        bottleneck.bottleneckType = "cpu";
        bottleneck.suggestion = "Optimize CPU usage";
    } else if (average < 5.0) {
        bottleneck.bottleneckScore = 0.5;
        bottleneck.impactOnPerformance = 50.0;
        bottleneck.bottleneckType = "memory";
        bottleneck.suggestion = "Optimize memory usage";
    } else {
        bottleneck.bottleneckScore = 0.2;
        bottleneck.impactOnPerformance = 20.0;
        bottleneck.bottleneckType = "algorithmic";
        bottleneck.suggestion = "Optimize algorithm";
    }
    
    return bottleneck;
}

std::vector<OptimizationOpportunity> Analyzer::identifyOptimizationOpportunities(const std::vector<BottleneckInfo>& bottlenecks,
                                                                                 const std::vector<PerformanceTrend>& trends) {
    std::vector<OptimizationOpportunity> opportunities;
    
    for (const auto& bottleneck : bottlenecks) {
        if (bottleneck.bottleneckScore > 0.5) {
            OptimizationOpportunity opportunity;
            opportunity.opportunityName = "address_bottleneck_" + bottleneck.location;
            opportunity.potentialImprovement = bottleneck.impactOnPerformance * 2.0; // Double the improvement
            opportunity.effortRequired = std::min(5.0, bottleneck.bottleneckScore * 3.0);
            opportunity.riskLevel = std::min(5.0, 5.0 - bottleneck.bottleneckScore * 3.0);
            opportunity.description = "Address performance bottleneck in " + bottleneck.location;
            opportunity.implementations = {"optimize_algorithms", "improve_resource_allocation", "implement_caching"};
            opportunity.timestamp = 0;
            
            opportunities.push_back(opportunity);
        }
    }
    
    for (const auto& trend : trends) {
        if (trend.trendType == "increasing" && trend.slope > 1.0) {
            OptimizationOpportunity opportunity;
            opportunity.opportunityName = "control_trend_" + trend.metricName;
            opportunity.potentialImprovement = trend.slope * 10.0;
            opportunity.effortRequired = 3.0;
            opportunity.riskLevel = 2.0;
            opportunity.description = "Control increasing trend in " + trend.metricName;
            opportunity.implementations = {"implement_monitoring", "adjust_parameters", "scale_resources"};
            opportunity.timestamp = 0;
            
            opportunities.push_back(opportunity);
        }
    }
    
    return opportunities;
}

OptimizationOpportunity Analyzer::analyzeSingleOptimization(const std::string& opportunityName,
                                                         const std::vector<double>& performanceData,
                                                         const std::string& description) {
    OptimizationOpportunity opportunity;
    opportunity.opportunityName = opportunityName;
    opportunity.description = description;
    opportunity.timestamp = 0;
    
    if (performanceData.empty()) {
        opportunity.potentialImprovement = 0.0;
        opportunity.effortRequired = 5.0;
        opportunity.riskLevel = 5.0;
        return opportunity;
    }
    
    // Calculate potential improvement
    double sum = std::accumulate(performanceData.begin(), performanceData.end(), 0.0);
    double average = sum / performanceData.size();
    
    opportunity.potentialImprovement = std::min(100.0, average * 10.0);
    opportunity.effortRequired = std::max(1.0, std::min(5.0, 10.0 / opportunity.potentialImprovement));
    opportunity.riskLevel = std::max(1.0, std::min(5.0, 5.0 - opportunity.potentialImprovement / 20.0));
    
    return opportunity;
}

std::vector<AnomalyInfo> Analyzer::detectAnomalies(const std::vector<PerformanceTrend>& trends,
                                                   const std::vector<StatisticalAnalysis>& statistics) {
    std::vector<AnomalyInfo> anomalies;
    
    for (const auto& trend : trends) {
        // Check for anomalies in trends
        if (std::abs(trend.slope) > 10.0) {
            AnomalyInfo anomaly;
            anomaly.anomalyType = "extreme_trend";
            anomaly.anomalyTime = (trend.startTime + trend.endTime) / 2;
            anomaly.anomalyScore = std::min(1.0, std::abs(trend.slope) / 50.0);
            anomaly.description = "Extreme trend in " + trend.metricName;
            anomaly.rootCause = "Parameter drift or system configuration change";
            anomaly.timestamp = 0;
            
            anomalies.push_back(anomaly);
        }
    }
    
    for (const auto& statistic : statistics) {
        // Check for outliers in statistics
        if (statistic.stdDev > 100.0 * statistic.mean) {
            AnomalyInfo anomaly;
            anomaly.anomalyType = "high_variance";
            anomaly.anomalyTime = 0; // Would get actual timestamp
            anomaly.anomalyScore = std::min(1.0, statistic.stdDev / (1000.0 * statistic.mean));
            anomaly.description = "High variance in " + statistic.metricName;
            anomaly.rootCause = "System instability or measurement error";
            anomaly.timestamp = 0;
            
            anomalies.push_back(anomaly);
        }
    }
    
    return anomalies;
}

AnomalyInfo Analyzer::analyzeSingleAnomaly(const std::string& anomalyType, const std::vector<double>& values,
                                          const std::vector<Timestamp>& timestamps) {
    AnomalyInfo anomaly;
    anomaly.anomalyType = anomalyType;
    anomaly.timestamp = 0;
    
    if (values.empty()) {
        anomaly.anomalyScore = 0.0;
        anomaly.description = "Insufficient data";
        anomaly.rootCause = "Unknown";
        return anomaly;
    }
    
    // Calculate anomaly score based on standard deviation
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    double mean = sum / values.size();
    
    double variance = 0.0;
    for (const auto& value : values) {
        variance += (value - mean) * (value - mean);
    }
    variance /= values.size();
    
    double stdDev = std::sqrt(variance);
    
    anomaly.anomalyScore = std::min(1.0, stdDev / mean);
    anomaly.description = "Anomaly of type " + anomalyType + " with score " + std::to_string(anomaly.anomalyScore);
    anomaly.rootCause = "To be determined";
    anomaly.anomalyTime = timestamps.empty() ? 0 : timestamps.back();
    
    return anomaly;
}

StatisticalAnalysis Analyzer::analyzeStatistics(const std::vector<double>& values) {
    StatisticalAnalysis result;
    result.metricName = "metric";
    result.startTime = 0;
    result.endTime = 0;
    result.timestamp = 0;
    
    if (values.empty()) {
        return result;
    }
    
    result.mean = calculateMean(values);
    result.median = calculateMedian(values);
    result.mode = 0.0; // Would calculate mode
    result.stdDev = calculateStdDev(values, result.mean);
    result.variance = result.stdDev * result.stdDev;
    result.min = *std::min_element(values.begin(), values.end());
    result.max = *std::max_element(values.begin(), values.end());
    result.range = result.max - result.min;
    result.iqr = 0.0; // Would calculate IQR
    result.skewness = 0.0; // Would calculate skewness
    result.kurtosis = 0.0; // Would calculate kurtosis
    result.sampleSize = values.size();
    
    return result;
}

std::vector<StatisticalAnalysis> Analyzer::analyzeStatistics(const std::map<std::string, std::vector<double>>& metricData) {
    std::vector<StatisticalAnalysis> results;
    
    for (const auto& metricPair : metricData) {
        StatisticalAnalysis result = analyzeStatistics(metricPair.second);
        result.metricName = metricPair.first;
        results.push_back(result);
    }
    
    return results;
}

double Analyzer::calculateMean(const std::vector<double>& values) {
    if (values.empty()) return 0.0;
    
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

double Analyzer::calculateMedian(const std::vector<double>& values) {
    if (values.empty()) return 0.0;
    
    std::vector<double> sorted = values;
    std::sort(sorted.begin(), sorted.end());
    
    size_t n = sorted.size();
    if (n % 2 == 0) {
        return (sorted[n/2 - 1] + sorted[n/2]) / 2.0;
    } else {
        return sorted[n/2];
    }
}

double Analyzer::calculateStdDev(const std::vector<double>& values, double mean) {
    if (values.empty()) return 0.0;
    
    double sum = 0.0;
    for (const auto& value : values) {
        sum += (value - mean) * (value - mean);
    }
    
    return std::sqrt(sum / values.size());
}

std::vector<CorrelationAnalysis> Analyzer::analyzeCorrelations(const std::map<std::string, std::vector<double>>& metricData) {
    std::vector<CorrelationAnalysis> results;
    
    std::vector<std::string> metricNames;
    for (const auto& metricPair : metricData) {
        metricNames.push_back(metricPair.first);
    }
    
    for (size_t i = 0; i < metricNames.size(); ++i) {
        for (size_t j = i + 1; j < metricNames.size(); ++j) {
            CorrelationAnalysis correlation = analyzeSingleCorrelation(
                metricNames[i], metricNames[j],
                metricData.at(metricNames[i]), metricData.at(metricNames[j])
            );
            results.push_back(correlation);
        }
    }
    
    return results;
}

CorrelationAnalysis Analyzer::analyzeSingleCorrelation(const std::string& metric1, const std::string& metric2,
                                                      const std::vector<double>& values1, const std::vector<double>& values2) {
    CorrelationAnalysis result;
    result.metric1 = metric1;
    result.metric2 = metric2;
    result.timestamp = 0;
    
    if (values1.size() != values2.size() || values1.empty()) {
        result.correlationCoefficient = 0.0;
        result.correlationStrength = "none";
        result.correlationType = "none";
        return result;
    }
    
    result.correlationCoefficient = calculateCorrelationCoefficient(values1, values2);
    result.correlationStrength = calculateCorrelationStrength(result.correlationCoefficient);
    result.correlationType = determineCorrelationType(result.correlationCoefficient);
    result.startTime = 0;
    result.endTime = 0;
    
    return result;
}

double Analyzer::calculateCorrelationCoefficient(const std::vector<double>& values1, const std::vector<double>& values2) {
    if (values1.size() != values2.size() || values1.empty()) return 0.0;
    
    double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumXX = 0.0, sumYY = 0.0;
    size_t n = values1.size();
    
    for (size_t i = 0; i < n; ++i) {
        double x = values1[i];
        double y = values2[i];
        sumX += x;
        sumY += y;
        sumXY += x * y;
        sumXX += x * x;
        sumYY += y * y;
    }
    
    double denominator = std::sqrt((sumXX - sumX * sumX / n) * (sumYY - sumY * sumY / n));
    if (denominator == 0.0) return 0.0;
    
    return (sumXY - sumX * sumY / n) / denominator;
}

std::string Analyzer::calculateCorrelationStrength(double correlationCoefficient) {
    double absCorr = std::abs(correlationCoefficient);
    
    if (absCorr < 0.3) return "weak";
    else if (absCorr < 0.7) return "moderate";
    else return "strong";
}

std::string Analyzer::determineCorrelationType(double correlationCoefficient) {
    if (correlationCoefficient > 0.0) return "positive";
    else if (correlationCoefficient < 0.0) return "negative";
    else return "none";
}

PerformanceForecast Analyzer::forecastPerformance(const std::string& metricName,
                                                 const std::vector<std::pair<Timestamp, double>>& historicalData,
                                                 const std::string& forecastModel) {
    PerformanceForecast forecast;
    forecast.forecastName = metricName + "_forecast";
    forecast.forecastTime = historicalData.empty() ? 0 : historicalData.back().first + 1000;
    forecast.confidenceInterval = 0.8; // 80% confidence
    forecast.forecastModel = forecastModel;
    forecast.timestamp = 0;
    
    if (historicalData.empty()) {
        return forecast;
    }
    
    // Simple linear forecast (in real implementation, use more sophisticated models)
    double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumXX = 0.0;
    size_t n = historicalData.size();
    
    for (size_t i = 0; i < n; ++i) {
        double x = static_cast<double>(historicalData[i].first);
        double y = historicalData[i].second;
        sumX += x;
        sumY += y;
        sumXY += x * y;
        sumXX += x * x;
    }
    
    double denominator = n * sumXX - sumX * sumX;
    if (denominator != 0.0) {
        double slope = (n * sumXY - sumX * sumY) / denominator;
        double intercept = (sumY - slope * sumX) / n;
        
        forecast.projectedPerformance = slope * static_cast<double>(forecast.forecastTime) + intercept;
        
        // Generate forecast points
        for (size_t i = 0; i < 10; ++i) {
            Timestamp time = forecast.forecastTime + i * 1000;
            double value = slope * static_cast<double>(time) + intercept;
            forecast.forecastPoints.push_back(time);
            forecast.forecastValues.push_back(value);
        }
    } else {
        forecast.projectedPerformance = historicalData.back().second;
    }
    
    return forecast;
}

std::vector<PerformanceForecast> Analyzer::forecastMultipleMetrics(const std::map<std::string, std::vector<std::pair<Timestamp, double>>>& metricData) {
    std::vector<PerformanceForecast> forecasts;
    
    for (const auto& metricPair : metricData) {
        PerformanceForecast forecast = forecastPerformance(
            metricPair.first, metricPair.second, "linear"
        );
        forecasts.push_back(forecast);
    }
    
    return forecasts;
}

void Analyzer::exportAnalysis(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for analysis export: " + filename);
        return;
    }
    
    file << "Analysis Results:\n";
    file << "================\n\n";
    
    for (const auto& result : analysisResults_) {
        file << "Analysis: " << result.analysisName << "\n";
        file << "  Type: " << result.analysisType << "\n";
        file << "  Findings: " << result.findings << "\n";
        file << "  Recommendations: " << result.recommendations << "\n";
        file << "  Score: " << result.score << "\n";
        file << "  Timestamp: " << result.timestamp << "\n\n";
    }
    
    NLM_LOG_INFO("Analysis results exported to: " + filename);
}

void Analyzer::importAnalysis(const std::string& filename) {
    // Implement analysis import
    NLM_LOG_WARNING("Analysis import not yet implemented");
}

std::vector<AnalysisResult> Analyzer::getAnalysisResults() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return analysisResults_;
}

AnalysisResult Analyzer::getAnalysisResult(const std::string& analysisName) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& result : analysisResults_) {
        if (result.analysisName == analysisName) {
            return result;
        }
    }
    
    return AnalysisResult(); // Return empty result if not found
}

size_t Analyzer::getAnalysisCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return analysisResults_.size();
}

void Analyzer::clearAnalysisResults() {
    std::lock_guard<std::mutex> lock(mutex_);
    analysisResults_.clear();
}

} // namespace nlm