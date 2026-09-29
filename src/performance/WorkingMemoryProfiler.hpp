#pragma once

#include "EnhancedWorkingMemory.hpp"
#include "WorkingMemoryPerformanceMonitor.hpp"
#include "../core/Types/Types.hpp"
#include <chrono>
#include <unordered_map>
#include <vector>
#include <string>

namespace nlm {

class WorkingMemoryProfiler {
public:
    WorkingMemoryProfiler() : profilingEnabled_(true), samplingRate_(0.1f), 
                              maxHistorySize_(10000), enableRealTimeAnalysis_(true),
                              enableTrendAnalysis_(true), enableAnomalyDetection_(true) {}
    
    ~WorkingMemoryProfiler() {}
    
    // Start profiling for a working memory operation
    template<typename Func>
    auto profileOperation(const std::string& operationName, Func&& func) {
        if (!profilingEnabled_) {
            return func();
        }
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Execute the operation
        auto result = func();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
        
        // Record the operation
        recordOperation(operationName, duration.count(), currentStep_);
        
        return result;
    }
    
    // Start profiling with parameters
    template<typename Func, typename... Args>
    auto profileOperationWithParams(const std::string& operationName, 
                                    const std::unordered_map<std::string, double>& params,
                                    Func&& func, Args&&... args) {
        if (!profilingEnabled_) {
            return func(std::forward<Args>(args)...);
        }
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Execute the operation
        auto result = func(std::forward<Args>(args)...);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
        
        // Record the operation with parameters
        recordOperationWithParams(operationName, duration.count(), params, currentStep_);
        
        return result;
    }
    
    // Profile a specific neuron operation
    void profileNeuronOperation(NeuronId neuronId, const std::string& operationName,
                                SimulationStep step, size_t operationCount = 1) {
        if (!profilingEnabled_) return;
        
        auto key = std::make_pair(neuronId.value, operationName);
        neuronOperationTimes_[key] += std::chrono::nanoseconds(1000 * operationCount);  // Approximate
        neuronOperationCounts_[key] += operationCount;
        
        recordNeuronActivity(neuronId, step, operationName);
    }
    
    // Profile memory storage operation
    void profileStorageOperation(const std::vector<float>& pattern, 
                                 float strength, SimulationStep step) {
        if (!profilingEnabled_) return;
        
        std::unordered_map<std::string, double> params;
        params["patternSize"] = pattern.size();
        params["strength"] = strength;
        
        recordOperation("store", 1000, params, step);  // Approximate
        
        // Track pattern statistics
        updatePatternStats(pattern);
    }
    
    // Profile memory retrieval operation
    void profileRetrievalOperation(SimulationStep step) {
        if (!profilingEnabled_) return;
        
        recordOperation("retrieve", 1000, {}, step);  // Approximate
    }
    
    // Profile competition operation
    void profileCompetitionOperation(size_t competitors, SimulationStep step) {
        if (!profilingEnabled_) return;
        
        std::unordered_map<std::string, double> params;
        params["competitors"] = competitors;
        
        recordOperation("competition", 5000 + competitors * 100, params, step);  // Approximate
    }
    
    // Profile consolidation operation
    void profileConsolidationOperation(SimulationStep step) {
        if (!profilingEnabled_) return;
        
        recordOperation("consolidation", 2000, {}, step);  // Approximate
    }
    
    // Profile replay operation
    void profileReplayOperation(size_t tracesReplayed, SimulationStep step) {
        if (!profilingEnabled_) return;
        
        std::unordered_map<std::string, double> params;
        params["tracesReplayed"] = tracesReplayed;
        
        recordOperation("replay", 3000 + tracesReplayed * 50, params, step);  // Approximate
    }
    
    // Get profiling statistics
    struct OperationStats {
        std::string name;
        size_t callCount;
        double totalTimeNs;
        double averageTimeNs;
        double minTimeNs;
        double maxTimeNs;
        double standardDeviationNs;
        std::unordered_map<std::string, double> averageParams;
    };
    
    std::vector<OperationStats> getOperationStats() const {
        std::vector<OperationStats> stats;
        
        for (const auto& pair : operationStats_) {
            OperationStats stat;
            stat.name = pair.first;
            stat.callCount = pair.second.callCount;
            stat.totalTimeNs = pair.second.totalTimeNs;
            stat.averageTimeNs = pair.second.averageTimeNs;
            stat.minTimeNs = pair.second.minTimeNs;
            stat.maxTimeNs = pair.second.maxTimeNs;
            stat.standardDeviationNs = pair.second.standardDeviationNs;
            
            // Calculate average params
            size_t paramCount = 0;
            for (const auto& paramPair : pair.second.paramStats) {
                stat.averageParams[paramPair.first] = paramPair.second.average;
                paramCount++;
            }
            
            stats.push_back(stat);
        }
        
        return stats;
    }
    
    // Get neuron operation statistics
    std::unordered_map<std::string, double> getNeuronOperationStats(NeuronId neuronId) const {
        std::unordered_map<std::string, double> stats;
        
        auto neuronPrefix = std::to_string(neuronId.value) + ":";
        for (const auto& pair : neuronOperationTimes_) {
            if (pair.first.first == neuronId.value) {
                std::string operationName = pair.first.second;
                auto timeIt = neuronOperationTimes_.find(pair.first);
                auto countIt = neuronOperationCounts_.find(pair.first);
                
                if (timeIt != neuronOperationTimes_.end() && countIt != neuronOperationCounts_.end()) {
                    size_t count = countIt->second;
                    auto duration = timeIt->second;
                    
                    stats[operationName + "_average_ns"] = duration.count() / count;
                    stats[operationName + "_count"] = count;
                }
            }
        }
        
        return stats;
    }
    
    // Get pattern statistics
    struct PatternStats {
        std::vector<float> averagePattern;
        std::vector<float> minPattern;
        std::vector<float> maxPattern;
        std::vector<float> patternVariance;
        size_t patternCount;
    };
    
    PatternStats getPatternStats() const {
        PatternStats stats;
        stats.patternCount = patternHistory_.size();
        
        if (stats.patternCount > 0) {
            size_t patternSize = patternHistory_.front().size();
            stats.averagePattern.resize(patternSize, 0.0f);
            stats.minPattern.resize(patternSize, std::numeric_limits<float>::max());
            stats.maxPattern.resize(patternSize, std::numeric_limits<float>::lowest());
            stats.patternVariance.resize(patternSize, 0.0f);
            
            // Calculate statistics
            for (const auto& pattern : patternHistory_) {
                for (size_t i = 0; i < patternSize && i < pattern.size(); ++i) {
                    stats.averagePattern[i] += pattern[i];
                    stats.minPattern[i] = std::min(stats.minPattern[i], pattern[i]);
                    stats.maxPattern[i] = std::max(stats.maxPattern[i], pattern[i]);
                }
            }
            
            for (size_t i = 0; i < patternSize; ++i) {
                stats.averagePattern[i] /= stats.patternCount;
                
                // Calculate variance
                float sumSq = 0.0f;
                for (const auto& pattern : patternHistory_) {
                    if (i < pattern.size()) {
                        float diff = pattern[i] - stats.averagePattern[i];
                        sumSq += diff * diff;
                    }
                }
                stats.patternVariance[i] = sumSq / stats.patternCount;
            }
        }
        
        return stats;
    }
    
    // Get performance trends
    struct PerformanceTrend {
        std::string metric;
        std::vector<std::pair<SimulationStep, double>> values;
        double trendSlope;
        double trendIntercept;
    };
    
    std::vector<PerformanceTrend> getPerformanceTrends() const {
        std::vector<PerformanceTrend> trends;
        
        if (!enableTrendAnalysis_) return trends;
        
        // Analyze trends for key metrics
        std::vector<std::string> keyMetrics = {
            "store_average_time_ns",
            "retrieve_average_time_ns",
            "competition_average_time_ns",
            "memory_usage_ratio"
        };
        
        for (const auto& metric : keyMetrics) {
            PerformanceTrend trend;
            trend.metric = metric;
            
            // Extract values over time (simplified)
            for (const auto& stepData : stepData_) {
                if (stepData.second.find(metric) != stepData.second.end()) {
                    trend.values.emplace_back(stepData.first, stepData.second.at(metric));
                }
            }
            
            // Calculate linear regression
            if (trend.values.size() >= 2) {
                double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0;
                for (const auto& point : trend.values) {
                    sumX += point.first;
                    sumY += point.second;
                    sumXY += point.first * point.second;
                    sumX2 += point.first * point.first;
                }
                
                double n = trend.values.size();
                trend.trendSlope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
                trend.trendIntercept = (sumY - trend.trendSlope * sumX) / n;
            }
            
            trends.push_back(trend);
        }
        
        return trends;
    }
    
    // Get anomaly detection results
    std::vector<std::string> detectAnomalies() const {
        std::vector<std::string> anomalies;
        
        if (!enableAnomalyDetection_) return anomalies;
        
        // Check for operation time spikes
        for (const auto& pair : operationStats_) {
            const auto& stat = pair.second;
            
            if (stat.callCount > 0 && stat.averageTimeNs > 0.0) {
                double expectedTime = stat.averageTimeNs;
                double actualTime = stat.averageTimeNs;
                
                // Simple anomaly detection (spike > 3 standard deviations)
                if (stat.standardDeviationNs > 0.0 && 
                    std::abs(actualTime - expectedTime) > 3.0 * stat.standardDeviationNs) {
                    anomalies.push_back("Operation " + pair.first + " has anomalous timing: " +
                                      std::to_string(actualTime) + " ns vs expected " + std::to_string(expectedTime) + " ns");
                }
            }
        }
        
        return anomalies;
    }
    
    // Export profiling data to JSON (for analysis)
    std::string exportToJSON() const {
        // Generate JSON representation of profiling data
        // This is a simplified version - in practice, you'd use a JSON library
        return "{\"profilingData\": \"TODO - export to JSON\"}";
    }
    
    // Import profiling data from JSON
    void importFromJSON(const std::string& jsonData) {
        // Import profiling data from JSON
        // This is a simplified version - in practice, you'd use a JSON library
    }
    
    // Enable or disable profiling
    void enableProfiling(bool enable) {
        profilingEnabled_ = enable;
    }
    
    // Set sampling rate
    void setSamplingRate(float rate) {
        samplingRate_ = std::max(0.0f, std::min(1.0f, rate));
    }
    
    // Set max history size
    void setMaxHistorySize(size_t size) {
        maxHistorySize_ = size;
        // Trim history if it exceeds the new size
        while (stepData_.size() > maxHistorySize_) {
            stepData_.erase(stepData_.begin());
        }
    }
    
    // Set current simulation step
    void setCurrentStep(SimulationStep step) {
        currentStep_ = step;
    }
    
    // Reset profiling data
    void reset() {
        operationStats_.clear();
        neuronOperationTimes_.clear();
        neuronOperationCounts_.clear();
        neuronActivity_.clear();
        patternHistory_.clear();
        stepData_.clear();
        patternStats_ = PatternStats();
    }

private:
    // Operation statistics
    struct OperationMetric {
        size_t callCount;
        double totalTimeNs;
        double averageTimeNs;
        double minTimeNs;
        double maxTimeNs;
        double standardDeviationNs;
        std::unordered_map<std::string, ParamMetric> paramStats;
    };
    
    struct ParamMetric {
        double sum;
        double sumSq;
        size_t count;
        double average;
    };
    
    // Record an operation
    void recordOperation(const std::string& name, uint64_t timeNs, SimulationStep step) {
        if (!profilingEnabled_) return;
        
        recordOperationWithParams(name, timeNs, {}, step);
    }
    
    void recordOperationWithParams(const std::string& name, uint64_t timeNs,
                                   const std::unordered_map<std::string, double>& params,
                                   SimulationStep step) {
        if (!profilingEnabled_ || (rand() / (double)RAND_MAX) > samplingRate_) {
            return;
        }
        
        // Update operation stats
        auto it = operationStats_.find(name);
        if (it == operationStats_.end()) {
            operationStats_[name] = OperationMetric();
            it = operationStats_.find(name);
        }
        
        OperationMetric& metric = it->second;
        metric.callCount++;
        metric.totalTimeNs += timeNs;
        metric.averageTimeNs = metric.totalTimeNs / metric.callCount;
        metric.minTimeNs = std::min(metric.minTimeNs, static_cast<double>(timeNs));
        metric.maxTimeNs = std::max(metric.maxTimeNs, static_cast<double>(timeNs));
        
        if (metric.callCount > 1) {
            double variance = (metric.totalTimeNs * metric.totalTimeNs - 
                             metric.callCount * metric.averageTimeNs * metric.averageTimeNs) / 
                            (metric.callCount * (metric.callCount - 1));
            metric.standardDeviationNs = std::sqrt(variance);
        }
        
        // Update step data
        if (stepData_.size() > maxHistorySize_) {
            stepData_.erase(stepData_.begin());
        }
        stepData_[step] = std::unordered_map<std::string, double>();
        stepData_[step][name + "_time_ns"] = timeNs;
        
        for (const auto& param : params) {
            stepData_[step][name + "_param_" + param.first] = param.second;
        }
    }
    
    // Record neuron activity
    void recordNeuronActivity(NeuronId neuronId, SimulationStep step,
                              const std::string& operationName) {
        neuronActivity_[step].push_back(std::make_pair(neuronId.value, operationName));
    }
    
    // Update pattern statistics
    void updatePatternStats(const std::vector<float>& pattern) {
        if (patternHistory_.size() > maxHistorySize_) {
            patternHistory_.erase(patternHistory_.begin());
        }
        
        patternHistory_.push_back(pattern);
    }
    
    // Profiling data
    bool profilingEnabled_;
    float samplingRate_;
    size_t maxHistorySize_;
    bool enableRealTimeAnalysis_;
    bool enableTrendAnalysis_;
    bool enableAnomalyDetection_;
    
    SimulationStep currentStep_;
    
    // Operation statistics
    std::unordered_map<std::string, OperationMetric> operationStats_;
    
    // Neuron operation statistics
    std::unordered_map<std::pair<uint64_t, std::string>, std::chrono::nanoseconds> neuronOperationTimes_;
    std::unordered_map<std::pair<uint64_t, std::string>, size_t> neuronOperationCounts_;
    std::unordered_map<SimulationStep, std::vector<std::pair<uint64_t, std::string>>> neuronActivity_;
    
    // Pattern statistics
    std::vector<std::vector<float>> patternHistory_;
    PatternStats patternStats_;
    
    // Step data for trend analysis
    std::unordered_map<SimulationStep, std::unordered_map<std::string, double>> stepData_;
};

} // namespace nlm