#pragma once

#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Visualization interface for debugging and analysis
// PLACEHOLDER - Phase 2 will have real visualization
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
    void visualizeNetwork(const class Brain& brain);
    
    // Activity visualization
    void visualizeActivity(const class Brain& brain);
    
    // Spike raster plot
    void visualizeSpikes(const std::vector<class SpikeEvent>& spikes);
    
    // Network visualization
    void visualizeNetwork(const class Brain& brain);
    
    // Activity visualization
    void visualizeActivity(const class Brain& brain);
    
    // Spike raster plot
    void visualizeSpikes(const std::vector<class SpikeEvent>& spikes);
    
    // Weight matrix visualization
    void visualizeWeights(const class Brain& brain);
    
    // === ADVANCED VISUALIZATION FEATURES ===
    
    // Time-series visualization - plots metrics over time
    void visualizeTimeSeries(const std::vector<float>& data, const std::string& label);
    
    // Real-time monitoring - displays live performance metrics
    void visualizePerformance(const class Brain& brain);
    
    // Memory visualization - shows memory system state
    void visualizeMemory(const class Brain& brain);
    
    // Development visualization - shows developmental stage progression
    void visualizeDevelopment(const class Brain& brain);
    
    // Multiple visualization modes
    enum class VisualizationMode {
        Network,           // Static network structure
        Activity,          // Dynamic neural activity
        Spikes,            // Spike raster plots
        Weights,           // Synaptic weight matrices
        Performance,       // Performance metrics
        Memory,            // Memory system state
        Development,       // Development stages
        All                // All visualization modes
    };
    
    void setVisualizationMode(VisualizationMode mode);
    VisualizationMode getVisualizationMode() const;
    
    // Visualization configuration
    struct VisualizationConfig {
        bool showNeurons = true;
        bool showSynapses = true;
        bool showSpikes = true;
        bool showLabels = false;
        bool showMetrics = true;
        float neuronSize = 5.0f;
        float minWeight = -1.0f;
        float maxWeight = 1.0f;
        bool animate = true;
        double refreshRate = 30.0; // Hz
        std::string outputFormat = "text"; // "text", "json", "csv"
    };
    
    void configure(const VisualizationConfig& config);
    VisualizationConfig getConfiguration() const;
    
    // Data export capabilities
    void exportData(const std::string& filename, const std::string& format = "csv");
    void exportNetworkData(const std::string& filename);
    void exportActivityData(const std::string& filename);
    void exportMetricsData(const std::string& filename);
    
    // Performance profiling
    void startProfiling();
    void stopProfiling();
    bool isProfiling() const;
    double getLastFrameTime() const;
    double getAverageFrameTime() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Helper function for easy visualization setup
void setupDefaultVisualization();
std::unique_ptr<VisualizationInterface> createVisualization();

} // namespace nlm
