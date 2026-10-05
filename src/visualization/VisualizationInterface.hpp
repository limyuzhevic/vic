#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include "../core/Types/Types.hpp"
#include "MonitoringSystem.hpp"

namespace nlm {

// Real-time visualization with multiple output modes
enum class VisualizationMode {
    NONE,
    NETWORK_GRAPH,
    ACTIVITY_HEATMAP,
    SPIKE_RASTER,
    WEIGHT_MATRIX,
    MEMORY_VISUALIZATION,
    NEUROMODULATION_DISPLAY,
    DEVELOPMENT_STAGE,
    CHECKPOINT_PROGRESS,
    EXPERIMENT_TRACKING
};

// Visualization window configuration
struct VisualizationConfig {
    int windowWidth;
    int windowHeight;
    double updateFrequency;
    bool showGrid;
    bool showLabels;
    bool showTooltips;
    std::string colorScheme;
    double opacity;
    bool enableAntialiasing;
    bool enableShadows;
    bool enable3D;
    
    VisualizationConfig()
        : windowWidth(1920), windowHeight(1080), updateFrequency(30.0), showGrid(true)
        , showLabels(true), showTooltips(true), colorScheme("default"), opacity(1.0)
        , enableAntialiasing(true), enableShadows(true), enable3D(false) {}
};

// Real visualization implementation with actual monitoring capabilities
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize real visualization (with actual rendering)
    bool initialize(const VisualizationConfig& config);
    
    // Update visualization data
    void update();
    
    // Render current state to screen
    void render();
    
    // Close visualization and cleanup
    void close();
    
    // Check if visualization is active
    bool isActive() const;
    
    // Set update rate
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    
    // Real visualization methods (implementations provided)
    void visualizeNetwork(const Brain& brain);
    void visualizeActivity(const Brain& brain);
    void visualizeSpikes(const std::vector<SpikeEvent>& spikes);
    void visualizeWeights(const Brain& brain);
    
    // New comprehensive visualization methods
    void visualizeMemorySystems(const Brain& brain);
    void visualizeNeuromodulation(const Brain& brain);
    void visualizeDevelopmentStage(const Brain& brain);
    void visualizeCheckpointProgress(const std::vector<std::pair<uint64_t, double>>& progress);
    void visualizeExperimentProgress(const Experiment& experiment);
    void visualizePerformanceMetrics(const std::unordered_map<std::string, double>& metrics);
    
    // Set monitoring system for real-time data
    void setMonitoringSystem(std::shared_ptr<MonitoringSystem> monitoring);
    
    // Control visualization modes
    void setVisualizationMode(VisualizationMode mode);
    VisualizationMode getVisualizationMode() const;
    
    void addVisualizationMode(VisualizationMode mode);
    void removeVisualizationMode(VisualizationMode mode);
    bool hasVisualizationMode(VisualizationMode mode) const;
    
    // Export visualization data
    void exportVisualization(const std::string& filename);
    
    // Configuration
    void setConfig(const VisualizationConfig& config);
    VisualizationConfig getConfig() const;
    
    // Statistical analysis and insights
    std::string generateInsights() const;
    std::vector<std::string> detectAnomalies() const;
    double calculateStabilityScore() const;
    
    // Real-time interaction
    void onMouseClick(int x, int y);
    void onMouseMove(int x, int y);
    void onKeyPress(int key, bool pressed);
    void onWindowResize(int width, int height);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Private helper methods
    void drawNetworkGraph(const Brain& brain);
    void drawActivityHeatmap(const Brain& brain);
    void drawSpikeRaster(const std::vector<SpikeEvent>& spikes);
    void drawWeightMatrix(const Brain& brain);
    void drawMemoryVisualization(const Brain& brain);
    void drawNeuromodulationDisplay(const Brain& brain);
    void drawDevelopmentVisualization(const Brain& brain);
    void drawCheckpointVisualization();
    void drawExperimentVisualization();
    
    void calculateNetworkMetrics(const Brain& brain);
    void calculateActivityMetrics(const Brain& brain);
    void calculateMemoryMetrics(const Brain& brain);
    void calculateNeuromodulationMetrics(const Brain& brain);
    
    void drawGrid();
    void drawLabels();
    void drawColorLegend();
    void drawStatisticsPanel();
    void drawControlPanel();
    
    void saveScreenshot(const std::string& filename);
    void saveAnimationFrame(int frame);
    void loadVisualizationState(const std::string& filename);
    void saveVisualizationState(const std::string& filename);
};

} // namespace nlm
