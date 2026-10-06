#pragma once

#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Forward declarations
class Brain;
class SpikeEvent;

// Visualization data structures
struct VisualizationData {
    std::vector<NeuronId> neuronIds;
    std::vector<float> neuronActivities;
    std::vector<std::vector<float>> weightMatrices;
    std::vector<SpikeEvent> spikeEvents;
    double updateInterval;
    
    VisualizationData();
};

// Performance metrics for profiling
struct PerformanceMetrics {
    double cpuUsage;           // CPU utilization percentage
    double memoryUsage;        // Memory usage in MB
    double renderingTime;      // Time spent rendering in ms
    double updateTime;         // Time spent updating data in ms
    size_t neuronCount;        // Number of neurons in visualization
    size_t synapseCount;        // Number of synapses in visualization
    
    PerformanceMetrics();
};

// Export formats
enum class ExportFormat {
    TEXT,
    CSV,
    JSON,
    PNG,
    SVG,
    MATLAB
};

// Visualization interface for debugging and analysis
// Enhanced version with real visualization capabilities and performance profiling
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
    
    // Network visualization
    void visualizeNetwork(const Brain& brain);
    
    // Activity visualization
    void visualizeActivity(const Brain& brain);
    
    // Spike raster plot
    void visualizeSpikes(const std::vector<SpikeEvent>& spikes);
    
    // Weight matrix visualization
    void visualizeWeights(const Brain& brain);
    
    // Advanced visualization methods
    
    // Export options
    void setExportOptions(const std::string& path, ExportFormat format);
    
    // Performance profiling
    void setProfilingEnabled(bool enabled);
    PerformanceMetrics getPerformanceMetrics() const;
    
    // Visualization modes
    void enableHeatmap(bool enable);
    void enableTimeSeries(bool enable);
    void enableStatisticalAnalysis(bool enable);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal methods
    void updateData();
    void collectBrainData(const Brain& brain);
    void renderNetworkGraph(const Brain& brain);
    void renderActivityHeatmap(const Brain& brain);
    void renderSpikeRaster(const std::vector<SpikeEvent>& spikes);
    void renderWeightMatrix(const Brain& brain);
    void exportData();
    void exportToText();
    void exportToCSV();
    void exportToJSON();
    void exportToMATLAB();
};

} // namespace nlm

