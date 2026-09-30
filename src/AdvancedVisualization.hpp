#pragma once

#include "../brain/Brain.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../environment/Observation.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../development/DevelopmentSystem.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <array>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <thread>
#include <functional>
#include <queue>
#include <sstream>
#include <iomanip>

namespace nlm {

/**
 * @brief Advanced visualization and analysis tools for NLM
 * 
 * This namespace provides comprehensive visualization and analysis capabilities
 * for neural network simulation, including network metrics, activity patterns,
 * learning dynamics, and real-time monitoring tools.
 */
namespace AdvancedVisualization {

/**
 * @brief Network topology analysis
 */
struct NetworkTopologyAnalysis {
    // Structural metrics
    float density;                    // Connection density (0-1)
    float clusteringCoefficient;      // Clustering coefficient
    float averagePathLength;          // Average shortest path length
    float smallWorldCoefficient;      // Small-worldness metric
    float hierarchyScore;             // Network hierarchy level
    
    // Functional metrics
    float modularity;                 // Community structure strength
    float integratedEfficiency;       // Information flow efficiency
    float robustness;                 // Network robustness to perturbations
    
    // Dynamic metrics
    float synchronizationLevel;       // Overall network synchronization
    float informationFlow;            // Information transmission rate
    float feedbackStrength;           // Feedback loop strength
    
    NetworkTopologyAnalysis() : density(0.0f), clusteringCoefficient(0.0f),
                               averagePathLength(0.0f), smallWorldCoefficient(0.0f),
                               hierarchyScore(0.0f), modularity(0.0f),
                               integratedEfficiency(0.0f), robustness(0.0f),
                               synchronizationLevel(0.0f), informationFlow(0.0f),
                               feedbackStrength(0.0f) {}
};

/**
 * @brief Neural activity patterns and dynamics
 */
struct NeuralActivityPattern {
    // Temporal dynamics
    std::vector<float> firingRates;     // Firing rates over time
    std::vector<float> populationRates; // Activity in neural populations
    std::vector<float> membranePotentials; // Membrane potential dynamics
    std::vector<std::vector<float>> spikeTrains; // Individual neuron spike trains
    
    // Spatial patterns
    std::map<RegionId, std::vector<float>> regionalActivity;  // Activity by region
    std::map<NeuronId, float> neuronActivations;   // Individual neuron activation levels
    
    // Spectral analysis
    std::vector<float> powerSpectrum;   // Frequency spectrum of activity
    float dominantFrequency;            // Dominant oscillation frequency
    float coherence;                    // Network coherence
    
    // Pattern characteristics
    float regularity;                   // Spike train regularity
    float burstiness;                    // Burstiness of activity
    float complexity;                    // Dynamic complexity
    float predictability;               // Predictability of patterns
    
    NeuralActivityPattern() : dominantFrequency(0.0f), coherence(0.0f),
                             regularity(0.0f), burstiness(0.0f),
                             complexity(0.0f), predictability(0.0f) {}
};

/**
 * @brief Learning dynamics analysis
 */
struct LearningDynamics {
    // Plasticity metrics
    float synapticWeightChange;         // Total synaptic weight change
    float homeostaticAdjustment;       // Homeostatic regulation strength
    float neuromodulationEffect;        // Neuromodulation impact
    float developmentalInfluence;       // Developmental stage effects
    
    // Learning progress
    std::vector<float> learningCurve;  // Learning curve over time
    float learningRate;                 // Instantaneous learning rate
    float retentionLevel;               // Information retention
    float transferability;              // Generalization to new tasks
    
    // Memory effects
    float memoryConsolidation;          // Memory consolidation strength
    float replayEfficiency;             // Memory replay effectiveness
    float interference;                 // Interference between memories
    float noveltyEncoding;              // Novelty detection and encoding
    
    // Performance metrics
    float taskPerformance;              // Current task performance
    float transferPerformance;         // Transfer learning performance
    float adaptationSpeed;              // Speed of adaptation to new conditions
    float stability;                    // Learning stability
    
    LearningDynamics() : synapticWeightChange(0.0f), homeostaticAdjustment(0.0f),
                        neuromodulationEffect(0.0f), developmentalInfluence(0.0f),
                        learningRate(0.0f), retentionLevel(0.0f), transferability(0.0f),
                        memoryConsolidation(0.0f), replayEfficiency(0.0f), interference(0.0f),
                        noveltyEncoding(0.0f), taskPerformance(0.0f), transferPerformance(0.0f),
                        adaptationSpeed(0.0f), stability(0.0f) {}
};

/**
 * @brief Visualization output formats
 */
enum class VisualizationFormat {
    ASCII,      // Text-based visualization
    SVG,        // Scalable Vector Graphics
    JSON,       // JSON-based visualization data
    CSV,        // Comma-separated values
    PNG,        // Raster image format
    PDF,        // Portable Document Format
    HTML,       // Web-based visualization
    MATPLOTLIB  // Matplotlib visualization
};

/**
 * @brief Heatmap visualization for neural activity
 */
class ActivityHeatmap {
public:
    struct HeatmapData {
        std::vector<std::vector<float>> values;      // Activity values
        std::vector<std::vector<float>> normalized;  // Normalized values (0-1)
        std::vector<std::vector<std::string>> labels; // Cell labels
        float minValue;                              // Minimum value
        float maxValue;                              // Maximum value
        std::string colorScheme;                     // Color scheme name
    };

private:
    HeatmapData data_;
    size_t width_;
    size_t height_;
    std::string title_;
    
public:
    ActivityHeatmap(size_t width, size_t height) : width_(width), height_(height) {}
    
    void setData(const std::vector<std::vector<float>>& values, const std::vector<std::vector<std::string>>& labels = {});
    void setColorScheme(const std::string& scheme) { data_.colorScheme = scheme; }
    void setRange(float min, float max) { data_.minValue = min; data_.maxValue = max; }
    
    std::string generateASCII() const;
    std::string generateSVG() const;
    std::string generateJSON() const;
    std::string generateHTML() const;
    
    std::string getHeatmapImage(VisualizationFormat format) const;
    void saveToFile(const std::string& filename, VisualizationFormat format) const;
    
    const HeatmapData& getData() const { return data_; }
};

/**
 * @brief Connectivity matrix visualization
 */
class ConnectivityMatrix {
public:
    struct MatrixData {
        std::vector<std::vector<float>> weights;     // Connection weights
        std::vector<std::vector<std::string>> labels; // Region labels
        std::vector<std::vector<float>> distances;   // Physical/structural distances
        std::vector<std::vector<bool>> directed;     // Directionality
        std::string metricType;                       // "weight", "strength", "frequency"
    };

private:
    MatrixData data_;
    std::string title_;
    float threshold_;  // Threshold for showing connections
    bool symmetric_;   // Whether to show symmetric matrix
    
public:
    ConnectivityMatrix() : threshold_(0.0f), symmetric_(true) {}
    
    void setData(const std::vector<std::vector<float>>& weights,
                 const std::vector<std::vector<std::string>>& labels = {});
    void setThreshold(float threshold) { threshold_ = threshold; }
    void setSymmetric(bool symmetric) { symmetric_ = symmetric; }
    
    std::string generateASCII() const;
    std::string generateSVG() const;
    std::string generateJSON() const;
    
    std::string getNetworkGraph(VisualizationFormat format) const;
    void saveToFile(const std::string& filename, VisualizationFormat format) const;
    
    const MatrixData& getData() const { return data_; }
};

/**
 * @brief Neural activity time series visualization
 */
class ActivityTimeSeries {
public:
    struct TimeSeriesData {
        std::vector<double> timestamps;            // Time points
        std::vector<std::vector<float>> timeSeries; // Time series data
        std::vector<std::vector<std::string>> labels;  // Series labels
        std::string title;                           // Chart title
        std::vector<std::string> yAxisLabels;        // Y-axis labels for each series
        bool showGrid;                               // Grid visibility
        bool showLegend;                             // Legend visibility
    };

private:
    TimeSeriesData data_;
    std::string xlabel_;
    std::string ylabel_;
    
public:
    ActivityTimeSeries() {}
    
    void addTimeSeries(const std::vector<float>& values, const std::string& label = "",
                      const std::string& yLabel = "");
    void setTimestamps(const std::vector<double>& timestamps) { data_.timestamps = timestamps; }
    void setLabels(const std::vector<std::string>& labels) { data_.labels = labels; }
    
    std::string generateASCII() const;
    std::string generateSVG() const;
    std::string generateJSON() const;
    
    std::string getPlot(VisualizationFormat format) const;
    void saveToFile(const std::string& filename, VisualizationFormat format) const;
    
    const TimeSeriesData& getData() const { return data_; }
};

/**
 * @brief Statistical analysis tools
 */
class StatisticalAnalyzer {
public:
    struct StatisticalResults {
        // Descriptive statistics
        float mean;                    // Mean
        float median;                  // Median
        float mode;                    // Mode
        float standardDeviation;       // Standard deviation
        float variance;                // Variance
        float skewness;                 // Skewness
        float kurtosis;                // Kurtosis
        
        // Distribution characteristics
        float min;                     // Minimum
        float max;                     // Maximum
        float range;                   // Range
        float interquartileRange;      // Interquartile range
        
        // Outlier detection
        std::vector<float> outliers;   // Outlier values
        size_t outlierCount;           // Number of outliers
        
        // Confidence intervals
        float confidenceInterval95;    // 95% confidence interval width
        float confidenceInterval99;    // 99% confidence interval width
        
        // Normality test
        bool isNormal;                 // Whether data is normally distributed
        float normalityPValue;         // p-value for normality test
        
        StatisticalResults() : mean(0.0f), median(0.0f), mode(0.0f), standardDeviation(0.0f),
                             variance(0.0f), skewness(0.0f), kurtosis(0.0f), min(0.0f),
                             max(0.0f), range(0.0f), interquartileRange(0.0f), outliers(),
                             outlierCount(0), confidenceInterval95(0.0f), confidenceInterval99(0.0f),
                             isNormal(false), normalityPValue(0.0f) {}
    };

    struct TimeSeriesAnalysis {
        StatisticalResults stats;                    // Overall statistics
        std::vector<StatisticalResults> windowStats; // Rolling window statistics
        std::vector<float> trend;                   // Trend line
        std::vector<float> seasonality;            // Seasonal components
        std::vector<float> residuals;              // Residuals
        float autocorrelation;                      // Autocorrelation
        std::vector<float> partialAutocorrelation;   // Partial autocorrelation
        bool isStationary;                          // Whether series is stationary
        
        TimeSeriesAnalysis() : autocorrelation(0.0f), isStationary(false) {}
    };

public:
    StatisticalAnalyzer() = default;
    
    // Basic statistical analysis
    static StatisticalResults analyze(const std::vector<float>& data);
    static StatisticalResults analyze(const std::vector<std::vector<float>>& data);
    
    // Time series analysis
    static TimeSeriesAnalysis analyzeTimeSeries(const std::vector<float>& timeSeries,
                                               size_t windowSize = 100);
    
    // Pattern detection
    static bool detectOutliers(const std::vector<float>& data, float threshold = 3.0f);
    static bool detectAnomalies(const std::vector<float>& data, float threshold = 2.0f);
    static bool detectTrends(const std::vector<float>& data, float threshold = 0.01f);
    
    // Distribution analysis
    static bool isNormallyDistributed(const std::vector<float>& data, float alpha = 0.05f);
    static float calculateSkewness(const std::vector<float>& data);
    static float calculateKurtosis(const std::vector<float>& data);
    
    // Time series forecasting
    static std::vector<float> forecastARIMA(const std::vector<float>& data, size_t steps = 10);
    static std::vector<float> forecastExponentialSmoothing(const std::vector<float>& data, size_t steps = 10);
    
    // Network-specific analysis
    static float calculateNetworkEfficiency(const std::vector<std::vector<float>>& adjacencyMatrix);
    static float calculateClusteringCoefficient(const std::vector<std::vector<float>>& adjacencyMatrix);
    static float calculateModularity(const std::vector<std::vector<float>>& adjacencyMatrix);
    
    // Activity pattern analysis
    static float calculateBurstIndex(const std::vector<float>& spikeTrain);
    static float calculateCoefficientOfVariation(const std::vector<float>& data);
    static float calculateSignalToNoiseRatio(const std::vector<float>& signal, const std::vector<float>& noise);
};

/**
 * @brief Real-time monitoring system
 */
class RealTimeMonitor {
public:
    struct MonitorData {
        double timestamp;                    // Current timestamp
        float cpuUsage;                      // CPU usage
        float memoryUsage;                   // Memory usage
        float networkBandwidth;              // Network bandwidth
        size_t activeNeurons;                // Number of active neurons
        size_t firingRate;                    // Firing rate
        float averageFiringRate;             // Average firing rate
        std::map<std::string, float> metrics; // Additional metrics
    };

    struct Alert {
        std::string type;                   // Alert type ("threshold", "performance", "error")
        std::string message;                 // Alert message
        float severity;                      // Alert severity (0.0-1.0)
        double timestamp;                    // Alert timestamp
        std::string source;                  // Alert source
        std::map<std::string, float> context; // Alert context
        
        Alert() : severity(0.0f), timestamp(0.0) {}
    };

private:
    MonitorData currentData_;
    std::vector<MonitorData> history_;
    std::vector<Alert> alerts_;
    
    std::function<void(const MonitorData&)> dataCallback_;
    std::function<void(const Alert&)> alertCallback_;
    
    size_t historySize_;
    float performanceThreshold_;
    float resourceThreshold_;
    float firingRateThreshold_;
    
    std::thread monitoringThread_;
    std::atomic<bool> monitoringActive_;
    
public:
    RealTimeMonitor() : historySize_(1000), performanceThreshold_(0.8f),
                       resourceThreshold_(0.9f), firingRateThreshold_(100.0f),
                       monitoringActive_(false) {}
    
    ~RealTimeMonitor() {
        stopMonitoring();
    }
    
    // Monitoring control
    void startMonitoring(double interval = 1.0);  // Interval in seconds
    void stopMonitoring();
    bool isMonitoring() const { return monitoringActive_; }
    
    // Data collection
    void updateMonitorData(const MonitorData& data);
    MonitorData getCurrentData() const { return currentData_; }
    std::vector<MonitorData> getHistory() const { return history_; }
    
    // Alert management
    void addAlert(const Alert& alert);
    std::vector<Alert> getAlerts(double since = 0.0) const;
    void clearAlerts() { alerts_.clear(); }
    size_t getAlertCount() const { return alerts_.size(); }
    
    // Configuration
    void setHistorySize(size_t size) { historySize_ = size; }
    void setPerformanceThreshold(float threshold) { performanceThreshold_ = threshold; }
    void setResourceThreshold(float threshold) { resourceThreshold_ = threshold; }
    void setFiringRateThreshold(float threshold) { firingRateThreshold_ = threshold; }
    
    // Callbacks
    void setDataCallback(std::function<void(const MonitorData&)> callback) {
        dataCallback_ = callback;
    }
    
    void setAlertCallback(std::function<void(const Alert&)> callback) {
        alertCallback_ = callback;
    }
    
    // Data analysis
    float calculateAverageMetric(const std::string& metricName, double timeWindow = 60.0) const;
    float calculatePeakMetric(const std::string& metricName, double timeWindow = 60.0) const;
    float calculateTrend(const std::string& metricName, double timeWindow = 60.0) const;
    
    // Export
    std::string exportData(const std::string& format = "csv") const;
    void saveToFile(const std::string& filename, const std::string& format = "csv") const;
    
    // Monitoring integration with NLM
    void integrateWithBrain(Brain* brain);
    void integrateWithAgentBrain(AgentBrain* agentBrain);
    
private:
    void monitoringLoop(double interval);
    void checkThresholds();
    void processData(const MonitorData& data);
    
    static double getCurrentTime() {
        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() / 1000.0;
    }
};

/**
 * @brief Comprehensive visualization dashboard
 */
class VisualizationDashboard {
public:
    struct DashboardConfig {
        std::string title;                      // Dashboard title
        double refreshInterval;                 // Refresh interval in seconds
        std::vector<std::string> activeWidgets; // Active widget names
        bool autoRefresh;                       // Auto-refresh enabled
        bool showTimestamps;                    // Show timestamps
        bool showLegend;                        // Show legend
        std::string theme;                      // Color theme
        
        DashboardConfig() : refreshInterval(5.0), autoRefresh(true),
                          showTimestamps(true), showLegend(true), theme("default") {}
    };

    struct WidgetData {
        std::string widgetId;                    // Widget ID
        std::string widgetType;                  // Widget type ("heatmap", "timeseries", "metrics", "alerts")
        std::string title;                       // Widget title
        double x;                                // X position (0-1)
        double y;                                // Y position (0-1)
        double width;                            // Widget width (0-1)
        double height;                           // Widget height (0-1)
        std::map<std::string, std::string> config; // Widget configuration
        std::string dataSource;                  // Data source
        
        WidgetData() : x(0.0), y(0.0), width(1.0), height(1.0) {}
    };

private:
    DashboardConfig config_;
    std::vector<WidgetData> widgets_;
    RealTimeMonitor monitor_;
    ActivityHeatmap heatmap_;
    ConnectivityMatrix connectivityMatrix_;
    ActivityTimeSeries timeSeries_;
    
    std::map<std::string, std::vector<float>> widgetData_;
    
    std::thread dashboardThread_;
    std::atomic<bool> dashboardActive_;
    
public:
    VisualizationDashboard() : dashboardActive_(false) {}
    
    ~VisualizationDashboard() {
        stopDashboard();
    }
    
    // Dashboard control
    void startDashboard();
    void stopDashboard();
    bool isDashboardActive() const { return dashboardActive_; }
    
    // Widget management
    void addWidget(const WidgetData& widget);
    bool removeWidget(const std::string& widgetId);
    WidgetData* getWidget(const std::string& widgetId);
    const WidgetData* getWidget(const std::string& widgetId) const;
    std::vector<WidgetData> getWidgets() const { return widgets_; }
    
    // Data management
    void setWidgetData(const std::string& widgetId, const std::vector<float>& data);
    std::vector<float> getWidgetData(const std::string& widgetId) const;
    void clearAllData() { widgetData_.clear(); }
    
    // Integration
    void integrateWithMonitor(const RealTimeMonitor& monitor) {
        monitor_ = monitor;
    }
    
    void integrateWithBrain(Brain* brain);
    void integrateWithAgentBrain(AgentBrain* agentBrain);
    
    // Visualization generation
    std::string generateHTML() const;
    std::string generateSVG() const;
    std::string generateJSON() const;
    
    std::string getDashboardImage(VisualizationFormat format) const;
    void saveToFile(const std::string& filename, VisualizationFormat format) const;
    
    // Configuration
    void setConfig(const DashboardConfig& config) { config_ = config; }
    const DashboardConfig& getConfig() const { return config_; }
    
    // Analysis
    void analyzeNetworkTopology(Brain* brain);
    void analyzeActivityPatterns(Brain* brain);
    void analyzeLearningDynamics(Brain* brain);
    
    // Export
    std::string exportDashboard(const std::string& format = "html") const;
    void saveToFile(const std::string& filename, const std::string& format = "html") const;
    
private:
    void dashboardLoop();
    void updateWidgetData();
    void renderWidgets();
};

} // namespace AdvancedVisualization

} // namespace nlm