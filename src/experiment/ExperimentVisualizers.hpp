#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "ExperimentProtocol.hpp"
#include "ExperimentStateManager.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

// Visualizer type
enum class VisualizerType {
    Chart,
    Graph,
    Table,
    Heatmap,
    Histogram,
    ScatterPlot,
    Timeline,
    Dashboard,
    Custom
};

// Visualization data
struct VisualizationData {
    std::string dataId;
    std::string dataSource;
    std::map<std::string, std::vector<double>> datasets;
    std::map<std::string, std::string> dataLabels;
    std::chrono::system_clock::time_point timestamp;
    
    VisualizationData() : timestamp(std::chrono::system_clock::now()) {}
    VisualizationData(const std::string& dataId, const std::string& dataSource)
        : dataId(dataId), dataSource(dataSource), timestamp(std::chrono::system_clock::now()) {}
};

// Visualizer base
class Visualizer {
public:
    virtual ~Visualizer() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getVersion() const = 0;
    virtual bool initialize(const Config& config) = 0;
    virtual void update(double timestep) = 0;
    virtual void shutdown() = 0;
    virtual bool isEnabled() const = 0;
    virtual void setEnabled(bool enabled) = 0;
    virtual VisualizerType getType() const = 0;
    virtual std::string generateVisualization(const VisualizationData& data) = 0;
    virtual std::string getVisualizationFormat() const = 0;
};

class ExperimentVisualizers {
public:
    ExperimentVisualizers();
    ~ExperimentVisualizers();
    
    // Disable copying, enable moving
    ExperimentVisualizers(const ExperimentVisualizers&) = delete;
    ExperimentVisualizers& operator=(const ExperimentVisualizers&) = delete;
    ExperimentVisualizers(ExperimentVisualizers&&) noexcept;
    ExperimentVisualizers& operator=(ExperimentVisualizers&&) noexcept;
    
    // Visualizer management
    bool registerVisualizer(const std::string& name, std::shared_ptr<Visualizer> visualizer);
    bool unregisterVisualizer(const std::string& name);
    bool updateVisualizer(const std::string& name, std::shared_ptr<Visualizer> visualizer);
    std::shared_ptr<Visualizer> getVisualizer(const std::string& name) const;
    std::vector<std::string> getAllVisualizerNames() const;
    
    // Visualizer lifecycle
    bool initializeVisualizer(const std::string& name, const Config& config);
    bool shutdownVisualizer(const std::string& name);
    bool updateVisualizer(const std::string& name, double timestep);
    bool enableVisualizer(const std::string& name);
    bool disableVisualizer(const std::string& name);
    
    // Visualization generation
    std::string createVisualization(const std::string& visualizerName, const VisualizationData& data);
    std::vector<std::string> createMultipleVisualizations(const std::vector<std::pair<std::string, VisualizationData>>& visualizations);
    std::string exportVisualization(const std::string& visualizerName, const VisualizationData& data, const std::string& format);
    
    // Visualizer status
    bool isVisualizerEnabled(const std::string& name) const;
    bool isVisualizerInitialized(const std::string& name) const;
    std::chrono::system_clock::time_point getVisualizerLastUpdate(const std::string& name) const;
    std::string getVisualizerStatus(const std::string& name) const;
    
    // Visualizer analysis
    std::map<std::string, double> getVisualizerPerformance(const std::string& name) const;
    std::vector<std::string> getVisualizerDependencies(const std::string& name) const;
    std::vector<std::string> getVisualizersByType(VisualizerType type) const;
    
    // Visualizer backup and recovery
    bool backupVisualizerData(const std::string& name, const std::string& backupPath) const;
    bool restoreVisualizerData(const std::string& name, const std::string& backupPath);
    
    // Visualizer cleanup
    bool cleanupVisualizerData(const std::string& name);
    bool cleanupAllVisualizerData();
    
    // Visualizer system management
    bool enableVisualizerMonitoring(const std::string& name, bool enable);
    bool isVisualizerMonitoringEnabled(const std::string& name) const;
    bool enableRealTimeVisualizerUpdates(bool enable);
    bool isRealTimeVisualizerUpdatesEnabled() const;
    
    // Visualizer utilities
    static std::string generateVisualizerName(const std::string& prefix = "visualizer") const;
    static bool isValidVisualizerName(const std::string& name) const;
    static VisualizerType stringToVisualizerType(const std::string& typeStr) const;
    static std::string visualizerTypeToString(VisualizerType type) const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm