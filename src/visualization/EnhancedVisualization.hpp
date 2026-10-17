#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <map>
#include <iomanip>
#include <sstream>

namespace nlm {

// Forward declarations
class Brain;
class SimulationClock;

// Visualization data structures
struct TimeSeriesData {
    std::vector<double> timestamps;
    std::vector<double> values;
    std::string name;
    std::string units;
    std::string color;
    
    TimeSeriesData() : color("#0000FF") {}
    
    void addPoint(double timestamp, double value) {
        timestamps.push_back(timestamp);
        values.push_back(value);
    }
};

struct NetworkActivity {
    std::vector<double> firingRates;
    std::vector<double> membranePotentials;
    std::vector<double> synapticWeights;
    std::vector<std::string> neuronTypes;
    std::vector<size_t> regionIds;
    
    size_t getNeuronCount() const { return firingRates.size(); }
};

struct PlasticityMetrics {
    double hebbianWeightChange;
    double stdpWeightChange;
    double structuralPlasticityRate;
    double learningEfficiency;
    double weightStability;
};

struct MemoryMetrics {
    size_t workingMemorySize;
    size_t episodicMemoryCount;
    size_t semanticMemoryCount;
    size_t proceduralMemoryCount;
    double memoryConsolidationRate;
    double retrievalAccuracy;
};

// Visualization interface
class VisualizationInterface {
public:
    virtual ~VisualizationInterface() = default;
    
    virtual void initialize() = 0;
    virtual void shutdown() = 0;
    virtual void update(float deltaTime) = 0;
    
    // Time series visualization
    virtual void plotTimeSeries(const std::string& title, const TimeSeriesData& data) = 0;
    virtual void plotMultipleTimeSeries(const std::string& title, 
                                       const std::vector<TimeSeriesData>& datasets) = 0;
    
    // Network visualization
    virtual void renderNetworkGraph(const NetworkActivity& activity) = 0;
    virtual void highlightActiveNeurons(const std::vector<size_t>& neuronIds) = 0;
    virtual void showSynapseWeights(const NetworkActivity& activity) = 0;
    
    // Metrics dashboard
    virtual void updateMetricsDashboard(const PlasticityMetrics& plasticity,
                                        const MemoryMetrics& memory) = 0;
    virtual void showPerformanceMetrics(const std::string& title,
                                        double fps, double memoryUsage, 
                                        double cpuUsage) = 0;
    
    // Simulation control
    virtual void showSimulationControls(bool& running, float& timeScale) = 0;
    virtual void showBrainParameters(const std::map<std::string, std::string>& parameters) = 0;
};

// Real-time data collector
class RealTimeDataCollector {
public:
    RealTimeDataCollector();
    ~RealTimeDataCollector();
    
    void setBrain(std::shared_ptr<Brain> brain);
    void setSimulationClock(std::shared_ptr<SimulationClock> clock);
    
    void update(float deltaTime);
    
    // Data collection
    void collectTimeSeriesData();
    void collectNetworkActivity();
    void collectPlasticityMetrics();
    void collectMemoryMetrics();
    
    // Data access
    const TimeSeriesData& getTimeSeriesData(const std::string& name) const;
    const NetworkActivity& getNetworkActivity() const;
    const PlasticityMetrics& getPlasticityMetrics() const;
    const MemoryMetrics& getMemoryMetrics() const;
    
    // Data management
    void clearData();
    void saveData(const std::string& filepath) const;
    bool loadData(const std::string& filepath);
    
    // Callbacks
    void setDataUpdateCallback(std::function<void()> callback);
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimulationClock> clock_;
    
    std::mutex dataMutex_;
    
    std::map<std::string, TimeSeriesData> timeSeriesData_;
    NetworkActivity networkActivity_;
    PlasticityMetrics plasticityMetrics_;
    MemoryMetrics memoryMetrics_;
    
    double currentTime_;
    std::function<void()> dataUpdateCallback_;
};

// Real-time data visualizer
class RealTimeDataVisualizer : public VisualizationInterface {
public:
    RealTimeDataVisualizer();
    ~RealTimeDataVisualizer() override;
    
    void initialize() override;
    void shutdown() override;
    void update(float deltaTime) override;
    
    // Time series visualization
    void plotTimeSeries(const std::string& title, const TimeSeriesData& data) override;
    void plotMultipleTimeSeries(const std::string& title, 
                               const std::vector<TimeSeriesData>& datasets) override;
    
    // Network visualization
    void renderNetworkGraph(const NetworkActivity& activity) override;
    void highlightActiveNeurons(const std::vector<size_t>& neuronIds) override;
    void showSynapseWeights(const NetworkActivity& activity) override;
    
    // Metrics dashboard
    void updateMetricsDashboard(const PlasticityMetrics& plasticity,
                                const MemoryMetrics& memory) override;
    void showPerformanceMetrics(const std::string& title,
                                double fps, double memoryUsage, 
                                double cpuUsage) override;
    
    // Simulation control
    void showSimulationControls(bool& running, float& timeScale) override;
    void showBrainParameters(const std::map<std::string, std::string>& parameters) override;
    
    // Data management
    void setDataCollector(std::shared_ptr<RealTimeDataCollector> collector);
    
    // Visualization settings
    void setWindowTitle(const std::string& title);
    void setWindowSize(int width, int height);
    void setUpdateFrequency(double frequency);
    
    // Export functionality
    void exportData(const std::string& filepath);
    void generateReport(const std::string& filepath);

private:
    std::shared_ptr<RealTimeDataCollector> dataCollector_;
    
    std::string windowTitle_;
    int windowWidth_;
    int windowHeight_;
    double updateFrequency_;
    
    bool showTimeSeriesView_;
    bool showNetworkView_;
    bool showMetricsView_;
    bool showControlsView_;
    
    std::chrono::steady_clock::time_point lastUpdateTime_;
};

// Analysis tools
class AnalysisTools {
public:
    AnalysisTools();
    ~AnalysisTools();
    
    void setDataCollector(std::shared_ptr<RealTimeDataCollector> collector);
    
    // Statistical analysis
    double calculateMean(const std::vector<double>& data) const;
    double calculateStandardDeviation(const std::vector<double>& data) const;
    double calculateCorrelation(const std::vector<double>& x, const std::vector<double>& y) const;
    
    // Pattern detection
    std::vector<size_t> detectActivityPatterns(const NetworkActivity& activity) const;
    double detectLearningTrends(const TimeSeriesData& weightData) const;
    std::vector<std::string> detectAnomalies(const std::vector<double>& metrics) const;
    
    // Performance analysis
    double calculatePerformanceScore(const PlasticityMetrics& plasticity,
                                    const MemoryMetrics& memory) const;
    std::string analyzeMemoryEfficiency(const MemoryMetrics& memory) const;
    std::string analyzePlasticityStability(const PlasticityMetrics& plasticity) const;
    
    // Export functionality
    void exportAnalysisResults(const std::string& filepath);
    std::string generateAnalysisReport() const;
    
private:
    std::shared_ptr<RealTimeDataCollector> dataCollector_;
};

// Visualization utilities
class VisualizationUtils {
public:
    // Color utilities
    static std::string interpolateColor(const std::string& color1, const std::string& color2, 
                                        double factor);
    static std::string getColorForValue(double value, double min, double max);
    static std::string getColorForNeuronType(const std::string& neuronType);
    
    // Data formatting utilities
    static std::string formatTime(double seconds);
    static std::string formatValue(double value, const std::string& units);
    static std::string formatPercentage(double value);
    
    // Math utilities
    static double normalizeValue(double value, double min, double max);
    static double clamp(double value, double min, double max);
    
    // Network layout utilities
    static std::vector<std::pair<size_t, size_t>> computeNetworkLayout(const NetworkActivity& activity);
    static double calculateNodePosition(size_t nodeId, const NetworkActivity& activity);
    
private:
    static std::map<std::string, std::string> neuronTypeColors_;
};

} // namespace nlm