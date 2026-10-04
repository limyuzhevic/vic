#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <functional>

namespace nlm {

// Forward declarations for brain components
class Brain;
class Neuron;
class Synapse;
class NeuralRegion;
class NeuralPopulation;
class NeuronId;
class SynapseId;
class SpikeEvent;

// Visualization configuration
struct VisualizationConfig {
    bool enableRealTimeUpdates = true;
    double updateInterval = 1000.0; // milliseconds
    std::string outputFormat = "terminal"; // terminal, svg, png, web
    std::string outputDirectory = ".";
    bool saveSnapshots = false;
    int snapshotInterval = 100; // steps
    bool showGrid = true;
    bool showLabels = true;
    double zoomLevel = 1.0;
    std::string colorScheme = "default";
};

// Performance metrics for monitoring
struct PerformanceMetrics {
    double cpuUsage = 0.0;
    double memoryUsage = 0.0;
    double simulationTime = 0.0;
    double renderingTime = 0.0;
    int spikeCount = 0;
    int activeNeuronCount = 0;
    double averageFiringRate = 0.0;
    std::chrono::system_clock::time_point timestamp;
    
    PerformanceMetrics() : timestamp(std::chrono::system_clock::now()) {}
};

// Alert system for system issues
struct SystemAlert {
    enum class Level { INFO, WARNING, ERROR, CRITICAL };
    Level level;
    std::string message;
    std::chrono::system_clock::time_point timestamp;
    std::string source;
    
    SystemAlert(Level lvl, const std::string& msg, const std::string& src = "")
        : level(lvl), message(msg), source(src), timestamp(std::chrono::system_clock::now()) {}
};

// Visualization interface for debugging and analysis
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize visualization
    bool initialize();
    
    // Update visualization data
    void update();
    
    // Render current state
    void render();
    
    // Close visualization
    void close();
    
    // Check if visualization is active
    bool isActive() const;
    
    // Set update rate
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    
    // Configuration
    void setConfig(const VisualizationConfig& config);
    const VisualizationConfig& getConfig() const;
    
    // Network visualization
    void visualizeNetwork(const Brain& brain);
    
    // Activity visualization
    void visualizeActivity(const Brain& brain);
    
    // Spike raster plot
    void visualizeSpikes(const std::vector<SpikeEvent>& spikes);
    
    // Weight matrix visualization
    void visualizeWeights(const Brain& brain);
    
    // Brain region visualization
    void visualizeRegions(const Brain& brain);
    
    // Development visualization
    void visualizeDevelopment(const Brain& brain);
    
    // Plasticity visualization
    void visualizePlasticity(const Brain& brain);
    
    // Memory system visualization
    void visualizeMemory(const Brain& brain);
    
    // Heatmap visualization for brain regions
    void visualizeRegionHeatmaps(const Brain& brain);
    
    // Real-time monitoring
    void startMonitoring(std::function<void(const PerformanceMetrics&)> callback);
    void stopMonitoring();
    bool isMonitoring() const;
    
    // Alert management
    void addAlert(const SystemAlert& alert);
    std::vector<SystemAlert> getAlerts() const;
    void clearAlerts();
    
    // Snapshot management
    bool saveSnapshot(const std::string& filename = "");
    bool loadSnapshot(const std::string& filename);
    
    // Integration with existing systems
    void integrateWithExperiment(const std::string& experimentName);
    void integrateWithPhase2(const std::string& phase2Output = "");
    void integrateWithPhase6(const std::string& phase6Output = "");
    
    // Performance metrics collection
    PerformanceMetrics getCurrentMetrics() const;
    std::vector<PerformanceMetrics> getMetricsHistory() const;
    void resetMetrics();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
