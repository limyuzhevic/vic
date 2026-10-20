// Enhanced statistics collection and analysis
// This file implements a comprehensive statistics collection system with:
// - Real-time metrics collection
// - Performance monitoring
// - System resource tracking
// - Neural network metrics
// - Network statistics
// - Advanced analysis and reporting

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <thread>
#include <atomic>
#include <functional>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <cmath>
#include <algorithm>
#include "core/Logger/Logger.hpp"

namespace nlm {

// System resource statistics
struct SystemStatistics {
    uint64_t timestamp;
    uint64_t memoryUsage;
    uint64_t memoryAvailable;
    uint64_t memoryTotal;
    double cpuUtilization;
    int cpuCores;
    double cpuFrequency;
    std::map<std::string, double> networkStats;
    std::chrono::milliseconds networkLatency;
    uint64_t networkBytesSent;
    uint64_t networkBytesReceived;
    
    SystemStatistics()
        : timestamp(0), memoryUsage(0), memoryAvailable(0), memoryTotal(0),
          cpuUtilization(0.0), cpuCores(1), cpuFrequency(0.0),
          networkLatency(0), networkBytesSent(0), networkBytesReceived(0) {}
};

// Neural network statistics
struct NeuralStatistics {
    std::string testName;
    uint64_t neuronCount;
    uint64_t synapseCount;
    uint32_t firingNeuronCount;
    double averageFiringRate;
    double totalSpikeCount;
    uint64_t totalMemoryUsage;
    double plasticityWeight;
    std::vector<double> layerStats;
    std::chrono::milliseconds computationTime;
    double efficiencyScore;
    
    NeuralStatistics()
        : neuronCount(0), synapseCount(0), firingNeuronCount(0),
          averageFiringRate(0.0), totalSpikeCount(0), totalMemoryUsage(0),
          plasticityWeight(0.0), computationTime(0), efficiencyScore(0.0) {}
};

// Performance metrics collection
struct PerformanceMetrics {
    std::chrono::milliseconds executionTime;
    std::chrono::milliseconds wallClockTime;
    double cpuUsage;
    double memoryUsage;
    uint64_t memoryAllocated;
    uint64_t memoryFreed;
    uint64_t cacheHits;
    uint64_t cacheMisses;
    std::vector<std::string> activeThreads;
    double cpuTemperature;
    double gpuUtilization;
    
    PerformanceMetrics()
        : executionTime(0), wallClockTime(0), cpuUsage(0.0), memoryUsage(0.0),
          memoryAllocated(0), memoryFreed(0), cacheHits(0), cacheMisses(0),
          cpuTemperature(0.0), gpuUtilization(0.0) {}
};

// Event statistics
struct EventStatistics {
    std::string eventName;
    std::chrono::milliseconds duration;
    std::chrono::milliseconds minDuration;
    std::chrono::milliseconds maxDuration;
    double averageDuration;
    uint64_t eventCount;
    double successRate;
    std::map<std::string, double> eventMetrics;
    
    EventStatistics()
        : duration(0), minDuration(0), maxDuration(0), averageDuration(0.0),
          eventCount(0), successRate(0.0) {}
};

// Statistics collector for comprehensive monitoring
class StatisticsCollector {
public:
    StatisticsCollector(std::shared_ptr<Logger> logger);
    ~StatisticsCollector();
    
    // System statistics
    void recordSystemStatistics(const SystemStatistics& stats);
    void updateSystemResourceUsage();
    SystemStatistics getLatestSystemStats() const;
    std::vector<SystemStatistics> getSystemStatisticsHistory(size_t limit = 100) const;
    
    // Neural network statistics
    void recordNeuralStatistics(const NeuralStatistics& stats);
    void updateNeuralNetworkMetrics(std::shared_ptr<Brain> brain, const std::string& testName);
    NeuralStatistics getLatestNeuralStats(const std::string& testName = "") const;
    std::vector<NeuralStatistics> getNeuralStatisticsHistory(size_t limit = 100) const;
    
    // Performance metrics
    void recordPerformanceMetrics(const PerformanceMetrics& metrics);
    void updatePerformanceCounters();
    PerformanceMetrics getLatestPerformanceMetrics() const;
    std::vector<PerformanceMetrics> getPerformanceHistory(size_t limit = 100) const;
    
    // Event statistics
    void recordEventStatistics(const EventStatistics& stats);
    void recordEvent(const std::string& eventName, const std::chrono::milliseconds& duration,
                    bool success = true, std::map<std::string, double> metrics = {});
    EventStatistics getEventStatistics(const std::string& eventName) const;
    std::map<std::string, EventStatistics> getAllEventStatistics() const;
    
    // Custom metrics
    void recordMetric(const std::string& name, double value, const std::string& category = "general",
                     std::map<std::string, std::string> tags = {});
    double getMetric(const std::string& name, const std::string& category = "general") const;
    std::vector<std::pair<std::string, double>> getMetricsByCategory(const std::string& category) const;
    
    // Configuration and control
    void setEnableSystemTracking(bool enable);
    void setEnableNeuralTracking(bool enable);
    void setEnablePerformanceTracking(bool enable);
    void setEnableEventTracking(bool enable);
    void setSamplingInterval(std::chrono::milliseconds interval);
    void setHistorySize(size_t size);
    
    // Statistics analysis
    std::string generateSystemReport() const;
    std::string generateNeuralReport(const std::string& testName = "") const;
    std::string generatePerformanceReport() const;
    std::string generateEventReport() const;
    
    // Data management
    void clearAllStatistics();
    bool exportStatistics(const std::string& filepath) const;
    bool importStatistics(const std::string& filepath);
    
    // Real-time statistics
    void startRealTimeMonitoring();
    void stopRealTimeMonitoring();
    bool isRealTimeMonitoring() const;
    std::string generateRealTimeStats() const;
    
    // Thresholds and alerts
    void setPerformanceThreshold(const std::string& metric, double threshold);
    void setMemoryThreshold(uint64_t threshold);
    void setCPUThreshold(double threshold);
    bool checkThresholds() const;
    
    // Statistics aggregation
    struct AggregatedStatistics {
        std::chrono::milliseconds totalExecutionTime;
        uint64_t totalMemoryUsage;
        double averageCPUUsage;
        double efficiencyScore;
        std::map<std::string, double> aggregatedMetrics;
        std::map<std::string, EventStatistics> aggregatedEvents;
        
        AggregatedStatistics()
            : totalExecutionTime(0), totalMemoryUsage(0), averageCPUUsage(0.0),
              efficiencyScore(0.0) {}
    };
    
    AggregatedStatistics getAggregatedStatistics() const;
    
    // Progress integration
    void setProgressReporter(std::shared_ptr<EnhancedProgressReporter> reporter);
    void updateProgressFromMetrics();
    
    // Neural network connectivity analysis
    struct ConnectivityAnalysis {
        uint64_t totalConnections;
        double averageConnectionStrength;
        uint32_t stronglyConnectedComponents;
        std::map<std::string, double> regionConnectivity;
        std::chrono::milliseconds analysisTime;
        
        ConnectivityAnalysis()
            : totalConnections(0), averageConnectionStrength(0.0),
              stronglyConnectedComponents(0), analysisTime(0) {}
    };
    
    ConnectivityAnalysis analyzeNeuralConnectivity(std::shared_ptr<Brain> brain) const;
    
    // Learning dynamics analysis
    struct LearningDynamics {
        double learningRate;
        double plasticityStrength;
        double memoryRetention;
        std::vector<double> learningCurve;
        std::chrono::milliseconds learningTime;
        double convergenceScore;
        
        LearningDynamics()
            : learningRate(0.0), plasticityStrength(0.0), memoryRetention(0.0),
              convergenceScore(0.0), learningTime(0) {}
    };
    
    LearningDynamics analyzeLearningDynamics(const std::string& testName) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    void sampleSystemResources();
    void sampleNeuralResources();
    void samplePerformanceResources();
    void checkAndAlertThresholds() const;
    std::string formatSystemReport() const;
    std::string formatNeuralReport(const std::string& testName) const;
    std::string formatPerformanceReport() const;
    std::string formatEventReport() const;
    double calculateEfficiencyScore() const;
    void cleanupExpiredData();
};