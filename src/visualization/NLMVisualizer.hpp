// NLM Visualization and Monitoring Headers
#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <mutex>
#include <condition_variable>

namespace nlm {

/**
 * Data structure for visualization
 */
struct VisualizationData {
    // Brain state
    size_t totalNeurons;
    size_t totalSynapses;
    float firingRate;
    float totalReward;
    
    // Neuromodulation
    float novelty;
    float curiosity;
    float dopamineLevel;
    
    // Memory systems
    bool workingMemoryActive;
    size_t episodicMemoryEpisodes;
    
    // Prediction
    float predictionAccuracy;
    
    // Development
    std::string developmentStage;
    float developmentAge;
};

/**
 * Options for data plotting
 */
struct PlotOptions {
    std::string xLabel;
    std::string yLabel;
    double lineWidth;
    double markerSize;
    bool showGrid;
    std::string legend;
    
    PlotOptions() : lineWidth(2.0), markerSize(5.0), showGrid(true), legend("") {}
};

/**
 * Main visualizer for NLM
 */
class NLMVisualizer {
public:
    NLMVisualizer();
    ~NLMVisualizer();
    
    /**
     * Start the visualizer
     */
    void start();
    
    /**
     * Stop the visualizer
     */
    void stop();
    
    /**
     * Update visualization with new data
     */
    void update(const VisualizationData& data);
    
    /**
     * Set update interval
     */
    void setUpdateInterval(double interval);
    
private:
    void visualizeLoop();
    void generatePlots();
    void generateStatusDisplay();
    void saveVisualization();
    static std::string getCurrentTimestamp();
    
    bool _isRunning;
    double _updateInterval;
    
    mutable std::mutex _mutex;
    std::condition_variable _plotUpdateCondition;
    
    VisualizationData _currentData;
    std::map<std::string, std::vector<double>> _plotData;
    std::thread _visualizerThread;
};

/**
 * Real-time monitor for NLM
 */
class RealTimeMonitor {
public:
    struct MonitorData {
        // System resources
        double cpuUsage;
        double memoryUsage;
        double diskUsage;
        
        // NLM brain state
        size_t totalNeurons;
        size_t totalSynapses;
        float firingRate;
        float avgSynapticWeight;
        size_t totalSpikes;
        
        // Memory systems
        bool workingMemoryActive;
        size_t episodicMemoryEpisodes;
    };
    
    RealTimeMonitor();
    ~RealTimeMonitor();
    
    /**
     * Start the monitor
     */
    void start();
    
    /**
     * Stop the monitor
     */
    void stop();
    
    /**
     * Update monitor with new data
     */
    void update(const MonitorData& data);
    
    /**
     * Set check interval
     */
    void setCheckInterval(double interval);
    
private:
    void monitorLoop();
    void checkSystemHealth();
    void checkBrainHealth();
    void checkResourceUsage();
    void logWarning(const std::string& message);
    void logCritical(const std::string& message);
    static std::string getCurrentTimestamp();
    std::string getTimestampString(const std::chrono::high_resolution_clock::time_point& timestamp);
    
    bool _isRunning;
    double _checkInterval;
    
    mutable std::mutex _mutex;
    std::condition_variable _monitorUpdateCondition;
    
    MonitorData _currentData;
    std::thread _monitorThread;
};

/**
 * Data plotter for NLM
 */
class DataPlotter {
public:
    DataPlotter();
    ~DataPlotter();
    
    /**
     * Plot data with given options
     */
    void plotData(const std::string& title, const std::vector<double>& xData,
                 const std::vector<double>& yData, const std::string& filePath,
                 const PlotOptions& options = PlotOptions());
    
    /**
     * Plot multiple datasets
     */
    void plotMultiple(const std::vector<std::string>& titles,
                     const std::vector<std::vector<double>>& xData,
                     const std::vector<std::vector<double>>& yData,
                     const std::string& filePath, const PlotOptions& options = PlotOptions());
    
    /**
     * Plot time series data
     */
    void plotTimeSeries(const std::string& title, const std::map<double, double>& dataPoints,
                       const std::string& filePath, const PlotOptions& options = PlotOptions());
    
    /**
     * Plot histogram of data
     */
    void plotHistogram(const std::string& title, const std::vector<double>& data,
                       int bins, const std::string& filePath,
                       const PlotOptions& options = PlotOptions());
};

/**
 * Metric monitor for NLM
 */
class MetricMonitor {
public:
    struct MetricData {
        std::string name;
        double value;
        std::string category;
        std::chrono::high_resolution_clock::time_point timestamp;
        std::thread::id threadId;
    };
    
    struct MetricStats {
        double value;
        size_t count;
        std::chrono::high_resolution_clock::time_point lastUpdate;
    };
    
    MetricMonitor();
    ~MetricMonitor();
    
    /**
     * Add metric
     */
    void addMetric(const std::string& name, double value, const std::string& category = "");
    
    /**
     * Update existing metric
     */
    void updateMetric(const std::string& name, double value);
    
    /**
     * Get all metrics
     */
    const std::vector<MetricData>& getMetrics() const;
    
    /**
     * Get metric statistics
     */
    const std::map<std::string, MetricStats>& getMetricStats() const;
    
    /**
     * Generate report
     */
    void generateReport(const std::string& filePath);
    
private:
    static std::string getCurrentTimestamp();
    std::string getTimestampString(const std::chrono::high_resolution_clock::time_point& timestamp);
    
    mutable std::mutex _mutex;
    std::vector<MetricData> _metrics;
    std::map<std::string, MetricStats> _metricStats;
};

} // namespace nlm