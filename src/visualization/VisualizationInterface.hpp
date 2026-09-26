#pragma once

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <map>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <sstream>

namespace nlm {

// Visualization mode enum
enum class VisualizationMode {
    NetworkGraph,
    ActivityHeatmap,
    SpikeRaster,
    WeightMatrix,
    ConnectivityMatrix,
    PopulationStats,
    MemoryTraces,
    Neuromodulation,
    Development,
    Custom
};

// Export format enum
enum class ExportFormat {
    Text,
    CSV,
    JSON,
    PNG,
    SVG,
    PDF,
    HDF5
};

// Debug level enum
enum class DebugLevel {
    None,
    Basic,
    Detailed,
    Intensive
};

// Advanced visualization interface for debugging and analysis
// Enhanced Phase 2 with comprehensive visualization capabilities
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize visualization with configuration
    bool initialize(const std::string& configFile = "");
    
    // Update visualization data
    void update();
    
    // Render current state with specified mode
    void render(VisualizationMode mode = VisualizationMode::NetworkGraph);
    
    // Close visualization and cleanup
    void close();
    
    // Check if visualization is active
    bool isActive() const;
    
    // Set update rate
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    
    // Configuration methods
    void setDebugLevel(DebugLevel level);
    void setShowLabels(bool show);
    void setColorScheme(const std::string& scheme);
    void setExportPath(const std::string& path);
    
    // Advanced visualization methods
    void visualizeNetwork(const class Brain& brain, VisualizationMode mode = VisualizationMode::NetworkGraph);
    void visualizeActivity(const class Brain& brain);
    void visualizeSpikes(const std::vector<class SpikeEvent>& spikes);
    void visualizeWeights(const class Brain& brain);
    void visualizeConnectivity(const class Brain& brain);
    void visualizeMemory(const class Brain& brain);
    void visualizeNeuromodulation(const class Brain& brain);
    void visualizeDevelopment(const class Brain& brain);
    
    // Data export methods
    bool exportData(const std::string& filename, ExportFormat format = ExportFormat::Text);
    bool exportNetworkGraph(const std::string& filename);
    bool exportSpikeData(const std::string& filename);
    bool exportWeightData(const std::string& filename);
    
    // Analysis methods
    void computeNetworkMetrics(const class Brain& brain);
    void computeActivityStatistics(const class Brain& brain);
    void computePlasticityMetrics(const class Brain& brain);
    void generateReport(const std::string& filename);
    
    // Debug methods
    void enableDebugOutput(DebugLevel level);
    void logState(const class Brain& brain);
    void printNetworkStatistics(const class Brain& brain);
    void printPlasticityStatistics(const class Brain& brain);
    
    // Performance monitoring
    void startPerformanceTimer();
    void stopPerformanceTimer();
    double getLastFrameTime() const;
    double getAverageFrameTime() const;
    
    // Interaction methods
    void onMouseClick(int x, int y);
    void onKeyPress(int key);
    void onWindowResize(int width, int height);
    
    // Configuration file parsing
    bool loadConfiguration(const std::string& filename);
    bool saveConfiguration(const std::string& filename) const;
    
    // Get current state
    std::string getCurrentModeName() const;
    std::vector<std::string> getAvailableModes() const;
    std::map<std::string, std::string> getStatistics() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
