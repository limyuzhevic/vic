#pragma once

#pragma once

#include "core/Types/Types.hpp"
#include <string>
#include <vector>
#include <map>

namespace nlm {

// Forward declarations
class PerformanceMonitor;
class MemoryMonitor;
class NeuralMonitor;
class NeuromodulationMonitor;

namespace monitor {

// Type aliases for convenience
using PerfMonitor = PerformanceMonitor;
using MemMonitor = MemoryMonitor;
using NeuralMonitorClass = NeuralMonitor;
using NeuroModMonitor = NeuromodulationMonitor;

// Forward declarations for tools
class MemoryLeakDetector;
class MemoryPoolManager;
class PerformanceProfiler;
class BottleneckAnalyzer;

// Common metrics structure
struct MonitoringMetrics {
    double cpuUsage;
    double memoryUsage;
    double firingRate;
    double predictionError;
    double noveltyLevel;
    double curiosityLevel;
    double dopamineLevel;
    Timestamp timestamp;
};

// Monitoring configuration
struct MonitoringConfig {
    bool enableRealTimeMonitoring;
    bool enableMemoryTracking;
    bool enableNeuralMonitoring;
    bool enableNeuromodulationMonitoring;
    bool enableLeakDetection;
    bool enablePerformanceProfiling;
    bool enableBottleneckAnalysis;
    Timestamp retentionTime;
    double alertThreshold;
    std::string exportFormat;
};

// Main monitoring class that coordinates all components
class NLMMonitor {
public:
    NLMMonitor();
    ~NLMMonitor();
    
    // Configuration
    void configure(const MonitoringConfig& config);
    MonitoringConfig getConfig() const;
    
    // Component management
    PerfMonitor& getPerformanceMonitor();
    MemMonitor& getMemoryMonitor();
    NeuralMonitorClass& getNeuralMonitor();
    NeuroModMonitor& getNeuromodulationMonitor();
    MemoryLeakDetector& getMemoryLeakDetector();
    MemoryPoolManager& getMemoryPoolManager();
    PerformanceProfiler& getPerformanceProfiler();
    BottleneckAnalyzer& getBottleneckAnalyzer();
    
    // Global monitoring control
    void startAllMonitoring();
    void stopAllMonitoring();
    bool isAllMonitoring() const;
    
    // Metrics collection
    MonitoringMetrics collectGlobalMetrics() const;
    std::vector<MonitoringMetrics> getMetricsHistory(Timestamp startTime, Timestamp endTime) const;
    
    // Alerting and reporting
    std::vector<std::string> checkForAlerts() const;
    std::string generateComprehensiveReport() const;
    
    // Export and import
    bool exportAllData(const std::string& filepath, const std::string& format) const;
    bool importAllData(const std::string& filepath, const std::string& format);
    
    // Analysis and optimization
    std::string analyzeOverallPerformance() const;
    std::string generateOptimizationRecommendations() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace monitor

} // namespace nlm