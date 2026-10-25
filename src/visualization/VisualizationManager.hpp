// Visualization and Monitoring Tools for NLM
// Comprehensive visualization and monitoring system for neural network analysis
// 
// Provides real-time visualization of neural activity, memory traces, cognitive
// functions, and system performance metrics.

#pragma once

#include "Brain.hpp"
#include <memory>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <mutex>
#include <functional>
#include <atomic>
#include <unordered_map>
#include <queue>

namespace nlm {

// Visualization formats
enum class VisualizationFormat {
    TEXT,
    JSON,
    CSV,
    BINARY,
    GRAPHML,
    HTML,
    SVG
};

// Monitoring data types
enum class MetricType {
    NeuralActivity,
    MemoryUsage,
    CognitivePerformance,
    Neuromodulation,
    Plasticity,
    Development,
    SimulationPerformance,
    PredictionAccuracy
};

// Data point for monitoring
struct MonitorDataPoint {
    std::chrono::system_clock::time_point timestamp;
    MetricType metric;
    std::string label;
    double value;
    std::vector<double> additionalValues;
    std::unordered_map<std::string, std::string> metadata;
    
    MonitorDataPoint(MetricType m, const std::string& l, double v)
        : timestamp(std::chrono::system_clock::now()), metric(m), label(l), value(v) {}
    
    std::string toJSON() const;
    std::string toCSV() const;
};

// Visualization output interface
class VisualizationOutput {
public:
    virtual ~VisualizationOutput() = default;
    virtual void write(const std::vector<MonitorDataPoint>& data) = 0;
    virtual void close() = 0;
    virtual std::string getFormatName() const = 0;
};

// File-based visualization outputs
class TextVisualization : public VisualizationOutput {
public:
    TextVisualization(const std::string& filename, bool includeTimestamps = true);
    ~TextVisualization() override;
    
    void write(const std::vector<MonitorDataPoint>& data) override;
    void close() override;
    std::string getFormatName() const override { return "Text"; }
    
    void setIncludeTimestamps(bool include) { includeTimestamps_ = include; }
    void setMaxPoints(size_t max) { maxPoints_ = max; }
    void setPrecision(int precision) { precision_ = precision; }
    
private:
    std::string filename_;
    bool includeTimestamps_;
    size_t maxPoints_;
    int precision_;
    std::ofstream file_;
};

class JSONVisualization : public VisualizationOutput {
public:
    JSONVisualization(const std::string& filename);
    ~JSONVisualization() override;
    
    void write(const std::vector<MonitorDataPoint>& data) override;
    void close() override;
    std::string getFormatName() const override { return "JSON"; }
    
    void setIndent(int indent) { indent_ = indent; }
    void setIncludeMetadata(bool include) { includeMetadata_ = include; }
    
private:
    std::string filename_;
    int indent_;
    bool includeMetadata_;
    std::ofstream file_;
};

class CSVVisualization : public VisualizationOutput {
public:
    CSVVisualization(const std::string& filename);
    ~CSVVisualization() override;
    
    void write(const std::vector<MonitorDataPoint>& data) override;
    void close() override;
    std::string getFormatName() const override { return "CSV"; }
    
    void setIncludeHeaders(bool include) { includeHeaders_ = include; }
    
private:
    std::string filename_;
    bool includeHeaders_;
    std::ofstream file_;
};

// Real-time visualization manager
class RealTimeVisualizer {
public:
    RealTimeVisualizer(std::shared_ptr<Brain> brain);
    ~RealTimeVisualizer();
    
    // Start visualization in a separate thread
    bool start(size_t updateIntervalMs = 100);
    
    // Stop visualization
    void stop();
    
    // Add visualization output
    void addOutput(std::unique_ptr<VisualizationOutput> output);
    
    // Set visualization options
    void setVisualizationOptions(const std::string& options);
    void enableNeuronVisualization(bool enable) { visualizeNeurons_ = enable; }
    void enableMemoryVisualization(bool enable) { visualizeMemory_ = enable; }
    void enableCognitiveVisualization(bool enable) { visualizeCognitive_ = enable; }
    
    // Set data filtering
    void setDataFilter(const std::function<bool(const MonitorDataPoint&)>& filter);
    
    // Get current status
    bool isRunning() const { return running_; }
    std::vector<std::string> getActiveOutputs() const;
    
private:
    std::shared_ptr<Brain> brain_;
    std::atomic<bool> running_;
    std::thread visualizationThread_;
    mutable std::mutex dataMutex_;
    
    std::vector<std::unique_ptr<VisualizationOutput>> outputs_;
    std::vector<MonitorDataPoint> dataBuffer_;
    
    bool visualizeNeurons_;
    bool visualizeMemory_;
    bool visualizeCognitive_;
    std::function<bool(const MonitorDataPoint&)> dataFilter_;
    size_t maxDataPoints_;
    
    void visualizationLoop();
    void collectData();
    void writeData();
};

// System performance monitor
class PerformanceMonitor {
public:
    PerformanceMonitor(std::shared_ptr<Brain> brain);
    ~PerformanceMonitor();
    
    // Start monitoring
    bool start(size_t sampleIntervalMs = 1000);
    
    // Stop monitoring
    void stop();
    
    // Add performance metrics collector
    void addMetricCollector(const std::string& name,
                          const std::function<double()>& collector);
    
    // Get performance statistics
    struct PerformanceStats {
        double cpuUsage;
        double memoryUsage;
        double diskUsage;
        size_t totalSteps;
        size_t totalSpikes;
        size_t activeNeurons;
        float averageFiringRate;
        size_t checkpointCount;
        double simulationTime;
        double stepsPerSecond;
    };
    
    PerformanceStats getStats() const;
    
    // Get historical data
    std::vector<MonitorDataPoint> getHistoricalData(MetricType metric,
                                                   std::chrono::system_clock::time_point from,
                                                   std::chrono::system_clock::time_point to) const;
    
    // Performance analysis
    double calculatePerformanceTrend(MetricType metric, size_t windowSize = 100) const;
    double calculatePredictionAccuracy() const;
    
private:
    std::shared_ptr<Brain> brain_;
    std::atomic<bool> running_;
    std::thread monitoringThread_;
    mutable std::mutex dataMutex_;
    
    std::vector<MonitorDataPoint> performanceData_;
    std::unordered_map<std::string, std::function<double()>> metricCollectors_;
    size_t sampleIntervalMs_;
    
    void monitoringLoop();
};

// Memory visualization
class MemoryVisualizer {
public:
    MemoryVisualizer(std::shared_ptr<Brain> brain);
    
    // Generate memory visualization data
    std::vector<MonitorDataPoint> getWorkingMemoryData() const;
    std::vector<MonitorDataPoint> getEpisodicMemoryData() const;
    std::vector<MonitorDataPoint> getAssociativeMemoryData() const;
    
    // Memory statistics
    struct MemoryStats {
        size_t workingMemorySize;
        size_t workingMemoryCapacity;
        float workingMemoryUtilization;
        size_t episodicMemoryCount;
        size_t episodicMemoryMaxAge;
        size_t associativeMemoryAssociations;
        size_t associativeMemoryPatterns;
    };
    
    MemoryStats getMemoryStats() const;
    
    // Memory activity patterns
    std::vector<std::pair<std::string, double>> getActivityPatterns() const;
    
private:
    std::shared_ptr<Brain> brain_;
};

// Neural visualization
class NeuralVisualizer {
public:
    NeuralVisualizer(std::shared_ptr<Brain> brain);
    
    // Generate neural activity visualization data
    std::vector<MonitorDataPoint> getNeuralActivityData() const;
    std::vector<MonitorDataPoint> getRegionActivityData() const;
    std::vector<MonitorDataPoint> getSynapseActivityData() const;
    
    // Neural statistics
    struct NeuralStats {
        size_t totalNeurons;
        size_t activeNeurons;
        size_t firingNeurons;
        float averageFiringRate;
        float excitationInhibitionRatio;
        size_t totalSynapses;
        float averageSynapticWeight;
        size_t regionCount;
        size_t populationCount;
    };
    
    NeuralStats getNeuralStats() const;
    
    // Network metrics
    double calculateNetworkDensity() const;
    double calculateClusteringCoefficient() const;
    double calculatePathLength() const;
    
private:
    std::shared_ptr<Brain> brain_;
};

// Cognitive visualization
class CognitiveVisualizer {
public:
    CognitiveVisualizer(std::shared_ptr<Brain> brain);
    
    // Generate cognitive function visualization data
    std::vector<MonitorDataPoint> getPredictionData() const;
    std::vector<MonitorDataPoint> getPlanningData() const;
    std::vector<MonitorDataPoint> getAttentionData() const;
    std::vector<MonitorDataPoint> getConceptData() const;
    
    // Cognitive statistics
    struct CognitiveStats {
        float predictionAccuracy;
        float predictionError;
        float predictionConfidence;
        size_t planningSolutions;
        float attentionFocus;
        size_t conceptCount;
        float conceptCohesion;
    };
    
    CognitiveStats getCognitiveStats() const;
    
    // Cognitive performance
    std::vector<std::pair<std::string, double>> getCognitivePerformance() const;
    
private:
    std::shared_ptr<Brain> brain_;
};

// Neuromodulation visualization
class NeuromodulationVisualizer {
public:
    NeuromodulationVisualizer(std::shared_ptr<Brain> brain);
    
    // Generate neuromodulation visualization data
    std::vector<MonitorDataPoint> getDopamineData() const;
    std::vector<MonitorDataPoint> getCuriosityData() const;
    std::vector<MonitorDataPoint> getNoveltyData() const;
    std::vector<MonitorDataPoint> getPredictionErrorData() const;
    
    // Neuromodulation statistics
    struct NeuromodulationStats {
        float dopamineLevel;
        float curiosityLevel;
        float noveltyLevel;
        float predictionError;
        float neuromodulationBalance;
    };
    
    NeuromodulationStats getNeuromodulationStats() const;
    
private:
    std::shared_ptr<Brain> brain_;
};

// Development visualization
class DevelopmentVisualizer {
public:
    DevelopmentVisualizer(std::shared_ptr<Brain> brain);
    
    // Generate development visualization data
    std::vector<MonitorDataPoint> getDevelopmentData() const;
    std::vector<MonitorDataPoint> getPlasticityData() const;
    
    // Development statistics
    struct DevelopmentStats {
        size_t developmentalStage;
        float plasticityLevel;
        float synaptogenesisRate;
        float pruningRate;
        float criticalPeriodActive;
        size_t developmentProgress;
    };
    
    DevelopmentStats getDevelopmentStats() const;
    
private:
    std::shared_ptr<Brain> brain_;
};

// Unified visualization manager
class VisualizationManager {
public:
    VisualizationManager(std::shared_ptr<Brain> brain);
    ~VisualizationManager();
    
    // Initialize visualizers
    void initialize();
    
    // Start all visualizers
    bool startAll(size_t updateIntervalMs = 100);
    
    // Stop all visualizers
    void stopAll();
    
    // Generate comprehensive report
    std::string generateReport() const;
    
    // Export data for external tools
    bool exportData(const std::string& format, const std::string& filepath);
    
    // Set visualization options
    void setOption(const std::string& key, const std::string& value);
    
    // Get status
    bool isRunning() const;
    std::vector<std::string> getActiveVisualizers() const;
    
private:
    std::shared_ptr<Brain> brain_;
    
    std::unique_ptr<RealTimeVisualizer> realTimeVisualizer_;
    std::unique_ptr<PerformanceMonitor> performanceMonitor_;
    
    std::unique_ptr<MemoryVisualizer> memoryVisualizer_;
    std::unique_ptr<NeuralVisualizer> neuralVisualizer_;
    std::unique_ptr<CognitiveVisualizer> cognitiveVisualizer_;
    std::unique_ptr<NeuromodulationVisualizer> neuromodulationVisualizer_;
    std::unique_ptr<DevelopmentVisualizer> developmentVisualizer_;
    
    std::unordered_map<std::string, std::string> options_;
    
    void updateVisualizers();
};

// Convenience functions
inline std::unique_ptr<RealTimeVisualizer> createRealTimeVisualizer(std::shared_ptr<Brain> brain) {
    return std::make_unique<RealTimeVisualizer>(brain);
}

inline std::unique_ptr<PerformanceMonitor> createPerformanceMonitor(std::shared_ptr<Brain> brain) {
    return std::make_unique<PerformanceMonitor>(brain);
}

inline std::unique_ptr<VisualizationManager> createVisualizationManager(std::shared_ptr<Brain> brain) {
    return std::make_unique<VisualizationManager>(brain);
}

// Configuration for visualization
struct VisualizationConfig {
    bool enableRealTimeVisualization = false;
    bool enablePerformanceMonitoring = true;
    bool enableMemoryVisualization = true;
    bool enableNeuralVisualization = true;
    bool enableCognitiveVisualization = true;
    bool enableNeuromodulationVisualization = true;
    bool enableDevelopmentVisualization = true;
    
    std::string outputFormat = "text";
    std::string outputDirectory = "./visualization";
    size_t updateIntervalMs = 100;
    size_t maxDataPoints = 10000;
    
    // File-specific settings
    bool includeTimestamps = true;
    bool includeMetadata = true;
    int jsonIndent = 2;
    
    // Performance settings
    size_t monitoringIntervalMs = 1000;
    size_t parallelThreads = 1;
    
    VisualizationConfig() = default;
};

} // namespace nlm