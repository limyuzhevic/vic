#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <functional>
#include <unordered_map>
#include <iomanip>
#include <sstream>

namespace nlm {

// Visualization output formats
enum class OutputFormat {
    TEXT,
    JSON,
    CSV,
    PNG,
    SVG,
    HTML
};

// Visualization data structures
struct VisualizationData {
    // Time series data
    std::vector<std::pair<std::chrono::system_clock::time_point, double>> timeSeries;
    
    // Network data
    std::vector<std::pair<size_t, size_t>> connections;  // (source, target)
    std::vector<double> connectionWeights;
    std::vector<size_t> neuronGroups;
    
    // Activity data
    std::vector<std::pair<size_t, double>> neuronActivity;  // (neuronId, activityLevel)
    std::vector<std::pair<size_t, double>> spikeActivity;   // (neuronId, spikeCount)
    
    // Memory data
    std::vector<std::pair<std::string, size_t>> memoryUsage;  // (component, bytes)
    std::vector<std::pair<std::string, double>> performanceMetrics;  // (metric, value)
    
    // State information
    std::map<std::string, std::string> stateInfo;
    std::string brainStatus;
    std::chrono::system_clock::time_point timestamp;
    
    VisualizationData() {
        timestamp = std::chrono::system_clock::now();
    }
};

// Visualization component interface
class IVizComponent {
public:
    virtual ~IVizComponent() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual bool isEnabled() const = 0;
    
    virtual void setEnabled(bool enabled) = 0;
    virtual void update(const VisualizationData& data) = 0;
    virtual void render() = 0;
    
    virtual std::string getOutput(const OutputFormat& format) const = 0;
    virtual bool saveToFile(const std::string& filename, const OutputFormat& format) const = 0;
};

// Real-time monitoring dashboard
class MonitoringDashboard : public IVizComponent {
public:
    MonitoringDashboard() = default;
    ~MonitoringDashboard() = default;
    
    std::string getName() const override { return "MonitoringDashboard"; }
    std::string getDescription() const override {
        return "Real-time monitoring dashboard displaying key performance metrics and system status.";
    }
    bool isEnabled() const override { return enabled_; }
    
    void setEnabled(bool enabled) override { enabled_ = enabled; }
    
    void addMetric(const std::string& name, const std::string& description, double initialValue);
    void updateMetric(const std::string& name, double value);
    double getMetric(const std::string& name) const;
    
    void addMemoryComponent(const std::string& name, size_t initialMemory);
    void updateMemoryComponent(const std::string& name, size_t memory);
    
    void addNeuron(const std::string& name, size_t neuronId, double activity);
    void updateNeuron(const std::string& name, size_t neuronId, double activity);
    
    void addSpike(const std::string& name, size_t neuronId, uint64_t spikeCount);
    void updateSpike(const std::string& name, size_t neuronId, uint64_t spikeCount);
    
    void setBrainStatus(const std::string& status);
    void setStateInfo(const std::string& key, const std::string& value);
    
    std::string getFormattedMetrics() const;
    std::string getMemoryUsageReport() const;
    std::string getSpikeActivityReport() const;
    
    void render() override;
    std::string getOutput(const OutputFormat& format) const override;
    bool saveToFile(const std::string& filename, const OutputFormat& format) const override;
    
    void update(const VisualizationData& data) override {
        if (enabled_) {
            updateFromData(data);
        }
    }
    
private:
    void updateFromData(const VisualizationData& data);
    
    bool enabled_ = true;
    std::map<std::string, std::pair<double, std::string>> metrics_;
    std::map<std::string, size_t> memoryComponents_;
    std::map<std::string, std::map<size_t, double>> neurons_;
    std::map<std::string, std::map<size_t, uint64_t>> spikes_;
    std::string brainStatus_;
    std::map<std::string, std::string> stateInfo_;
};

// Neural network visualization
class NeuralNetworkVisualizer : public IVizComponent {
public:
    NeuralNetworkVisualizer() = default;
    ~NeuralNetworkVisualizer() = default;
    
    std::string getName() const override { return "NeuralNetworkVisualizer"; }
    std::string getDescription() const override {
        return "Visualizes the neural network structure, connections, and activity patterns.";
    }
    bool isEnabled() const override { return enabled_; }
    
    void setEnabled(bool enabled) override { enabled_ = enabled; }
    
    void setLayout(const std::string& layout) { layout_ = layout; }
    void setNeuronRadius(double radius) { neuronRadius_ = radius; }
    void setConnectionThreshold(double threshold) { connectionThreshold_ = threshold; }
    
    void addRegion(const std::string& name, const std::string& type, double x, double y, double radius);
    void addConnection(size_t sourceNeuron, size_t targetNeuron, double weight);
    void updateNeuronActivity(size_t neuronId, double activity);
    void updateConnectionActivity(size_t sourceNeuron, size_t targetNeuron, double weight);
    
    std::string getNetworkLayout() const;
    std::string getConnectionMatrix() const;
    std::string getActivityHeatmap() const;
    
    void render() override;
    std::string getOutput(const OutputFormat& format) const override;
    bool saveToFile(const std::string& filename, const OutputFormat& format) const override;
    
    void update(const VisualizationData& data) override {
        if (enabled_) {
            updateFromData(data);
        }
    }
    
private:
    void updateFromData(const VisualizationData& data);
    std::string generateGraphVizOutput() const;
    std::string generateTextOutput() const;
    
    bool enabled_ = true;
    std::string layout_ = "force_directed";
    double neuronRadius_ = 5.0;
    double connectionThreshold_ = 0.1;
    
    struct Region {
        std::string name;
        std::string type;
        double x, y, radius;
    };
    
    std::vector<Region> regions_;
    std::vector<std::pair<size_t, size_t>> connections_;
    std::vector<double> connectionWeights_;
    
    std::map<size_t, double> neuronActivities_;
    std::map<std::pair<size_t, size_t>, double> connectionActivities_;
};

// Performance metrics visualization
class PerformanceMetricsViz : public IVizComponent {
public:
    PerformanceMetricsViz() = default;
    ~PerformanceMetricsViz() = default;
    
    std::string getName() const override { return "PerformanceMetricsViz"; }
    std::string getDescription() const override {
        return "Visualizes performance metrics over time, including memory usage, firing rates, and system efficiency.";
    }
    bool isEnabled() const override { return enabled_; }
    
    void setEnabled(bool enabled) override { enabled_ = enabled; }
    
    void setTimeRange(double start, double end) {
        timeRangeStart_ = start;
        timeRangeEnd_ = end;
    }
    
    void addTimeSeries(const std::string& name, const std::vector<std::pair<double, double>>& data);
    void updateTimeSeries(const std::string& name, const std::vector<std::pair<double, double>>& data);
    
    std::string getTimeSeriesPlot(const std::string& name) const;
    std::string getCombinedMetricsPlot() const;
    std::string getStatisticalSummary() const;
    
    void render() override;
    std::string getOutput(const OutputFormat& format) const override;
    bool saveToFile(const std::string& filename, const OutputFormat& format) const override;
    
    void update(const VisualizationData& data) override {
        if (enabled_) {
            updateFromData(data);
        }
    }
    
private:
    void updateFromData(const VisualizationData& data);
    std::string generatePlotlyOutput() const;
    std::string generateTextPlot() const;
    
    bool enabled_ = true;
    double timeRangeStart_ = 0.0;
    double timeRangeEnd_ = 100.0;
    
    std::map<std::string, std::vector<std::pair<double, double>>> timeSeries_;
    std::map<std::string, std::vector<double>> historicalData_;
};

// Spike activity visualization
class SpikeActivityViz : public IVizComponent {
public:
    SpikeActivityViz() = default;
    ~SpikeActivityViz() = default;
    
    std::string getName() const override { return "SpikeActivityViz"; }
    std::string getDescription() const override {
        return "Visualizes spike activity patterns, including per-neuron and population-level statistics.";
    }
    bool isEnabled() const override { return enabled_; }
    
    void setEnabled(bool enabled) override { enabled_ = enabled; }
    
    void setTimeWindow(double windowSize) { timeWindow_ = windowSize; }
    void setMaxSpikesPerNeuron(size_t maxSpikes) { maxSpikesPerNeuron_ = maxSpikes; }
    
    void addSpikeTrain(size_t neuronId, const std::vector<double>& spikeTimes);
    void updateSpikeTrain(size_t neuronId, const std::vector<double>& spikeTimes);
    
    std::string getSpikeRasterPlot() const;
    std::string getSpikeHistogram() const;
    std::string getFiringRateDistribution() const;
    
    void render() override;
    std::string getOutput(const OutputFormat& format) const override;
    bool saveToFile(const std::string& filename, const OutputFormat& format) const override;
    
    void update(const VisualizationData& data) override {
        if (enabled_) {
            updateFromData(data);
        }
    }
    
private:
    void updateFromData(const VisualizationData& data);
    std::string generateRasterPlot() const;
    std::string generateHistogramPlot() const;
    
    bool enabled_ = true;
    double timeWindow_ = 10.0;
    size_t maxSpikesPerNeuron_ = 1000;
    
    std::map<size_t, std::vector<double>> spikeTrains_;
    std::map<size_t, uint64_t> spikeCounts_;
    std::map<size_t, double> firingRates_;
};

// Configuration and state visualization
class ConfigStateViz : public IVizComponent {
public:
    ConfigStateViz() = default;
    ~ConfigStateViz() = default;
    
    std::string getName() const override { return "ConfigStateViz"; }
    std::string getDescription() const override {
        return "Visualizes configuration parameters and system state information.";
    }
    bool isEnabled() const override { return enabled_; }
    
    void setEnabled(bool enabled) override { enabled_ = enabled; }
    
    void setMaxDepth(size_t depth) { maxDepth_ = depth; }
    void setShowAdvanced(bool show) { showAdvanced_ = show; }
    
    void setConfiguration(const std::map<std::string, std::string>& config);
    void setState(const std::map<std::string, std::string>& state);
    
    std::string getConfigurationTree() const;
    std::string getStateSummary() const;
    std::string getParameterChart() const;
    
    void render() override;
    std::string getOutput(const OutputFormat& format) const override;
    bool saveToFile(const std::string& filename, const OutputFormat& format) const override;
    
    void update(const VisualizationData& data) override {
        if (enabled_) {
            updateFromData(data);
        }
    }
    
private:
    void updateFromData(const VisualizationData& data);
    std::string generateJSONOutput() const;
    std::string generateTreeTextOutput() const;
    
    bool enabled_ = true;
    size_t maxDepth_ = 3;
    bool showAdvanced_ = false;
    
    std::map<std::string, std::string> configuration_;
    std::map<std::string, std::string> state_;
    std::vector<std::string> configKeys_;
};

// Visualization container
class VisualizationContainer {
public:
    VisualizationContainer() = default;
    ~VisualizationContainer() = default;
    
    // Component management
    void addComponent(std::unique_ptr<IVizComponent> component);
    void removeComponent(const std::string& name);
    IVizComponent* getComponent(const std::string& name);
    const IVizComponent* getComponent(const std::string& name) const;
    
    // Management
    void clear();
    void enableAll();
    void disableAll();
    void toggleComponent(const std::string& name);
    
    // Update all components
    void update(const VisualizationData& data);
    
    // Render all components
    void renderAll();
    
    // Output generation
    std::string generateCombinedOutput(const OutputFormat& format) const;
    bool saveAllToFile(const std::string& prefix, const OutputFormat& format) const;
    
    // Configuration
    void setUpdateInterval(double interval) { updateInterval_ = interval; }
    void setAutoSave(bool autoSave) { autoSave_ = autoSave; }
    void setOutputDirectory(const std::string& directory) { outputDirectory_ = directory; }
    
    // Statistics
    size_t getComponentCount() const { return components_.size(); }
    std::vector<std::string> getComponentNames() const;
    
private:
    std::map<std::string, std::unique_ptr<IVizComponent>> components_;
    double updateInterval_ = 0.1;  // seconds
    bool autoSave_ = true;
    std::string outputDirectory_ = "./viz_output";
    std::chrono::system_clock::time_point lastUpdate_;
};

// Visualization factory
class VisualizationFactory {
public:
    VisualizationFactory() = default;
    ~VisualizationFactory() = default;
    
    // Create standard visualization components
    static std::unique_ptr<MonitoringDashboard> createMonitoringDashboard();
    static std::unique_ptr<NeuralNetworkVisualizer> createNeuralNetworkVisualizer();
    static std::unique_ptr<PerformanceMetricsViz> createPerformanceMetricsViz();
    static std::unique_ptr<SpikeActivityViz> createSpikeActivityViz();
    static std::unique_ptr<ConfigStateViz> createConfigStateViz();
    
    // Create visualization container with default components
    static std::unique_ptr<VisualizationContainer> createDefaultVisualizationContainer();
    
    // Create custom visualization
    static std::unique_ptr<VisualizationContainer> createCustomVisualization(
        const std::vector<std::string>& componentNames);
    
    // Load visualization from configuration
    static std::unique_ptr<VisualizationContainer> loadFromConfig(
        const std::string& configFile);
    
    // Save configuration
    static bool saveConfig(const std::string& configFile,
                          const VisualizationContainer& container);
};

// Convenience functions
namespace Visualization {
    // Global visualization container
    static VisualizationContainer globalContainer;
    
    // Initialize default components
    static void initializeDefaultVisualization();
    
    // Update with current system data (would need integration with actual system)
    static void updateGlobalVisualization();
    
    // Generate comprehensive report
    static std::string generateVisualizationReport();
    
    // Save all visualizations
    static void saveAllVisualizations(const std::string& directory);
    
    // Get component by name
    static IVizComponent* getComponent(const std::string& name);
    
    // Add component
    static void addComponent(std::unique_ptr<IVizComponent> component);
}

} // namespace nlm
