#pragma once

#include <string>
#include <vector>
#include <memory>
#include <string>
#include "../brain/Brain.hpp"
#include "../dynamics/SpikeSystem.hpp"

namespace nlm {

// Display modes for visualization
enum class DisplayMode {
    NetworkGraph,
    SpikeRaster,
    WeightMatrix,
    ActivityHeatmap
};

// Visualization data structures
struct VisualizationInterface {
    struct NetworkNode {
        int id;
        std::string name;
        size_t neuronCount;
        size_t synapseCount;
        float averageWeight;
        
        NetworkNode() : id(0), neuronCount(0), synapseCount(0), averageWeight(0.0f) {}
    };
    
    struct SpikePlotData {
        SimulationStep step;
        size_t totalSpikes;
        size_t firingNeurons;
        
        SpikePlotData() : step(0), totalSpikes(0), firingNeurons(0) {}
    };
    
    struct WeightMatrixData {
        SimulationStep step;
        float averageWeight;
        float minWeight;
        float maxWeight;
        size_t totalConnections;
        
        WeightMatrixData() : step(0), averageWeight(0.0f), minWeight(0.0f), 
                           maxWeight(0.0f), totalConnections(0) {}
    };
};

// Enhanced Visualization interface for debugging and analysis
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize visualization with external libraries
    bool initialize();
    
    // Update visualization data with current brain state
    void update(const Brain& brain, SimulationStep step);
    
    // Render current state
    void render(const Brain& brain, SimulationStep step);
    
    // Close visualization
    void close();
    
    // Check if visualization is active
    bool isActive() const;
    
    // Set update rate
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    
    // Configuration methods
    void configure(int width, int height);
    void toggleDisplayMode(DisplayMode mode);
    void setAutoRefresh(bool enabled, int interval);
    
    // Getters for visualization properties
    int getWindowWidth() const;
    int getWindowHeight() const;
    bool isNetworkGraphEnabled() const;
    bool isSpikeRasterEnabled() const;
    bool isWeightMatrixEnabled() const;
    bool isActivityHeatmapEnabled() const;
    bool isAutoRefreshEnabled() const;
    
    // Legacy methods (for backward compatibility)
    void visualizeNetwork(const Brain& brain);
    void visualizeActivity(const Brain& brain);
    void visualizeSpikes(const std::vector<SpikeEvent>& spikes);
    void visualizeWeights(const Brain& brain);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Enhanced visualization properties
    int windowWidth_;
    int windowHeight_;
    bool showNetworkGraph_;
    bool showSpikeRaster_;
    bool showWeightMatrix_;
    bool showActivityHeatmap_;
    bool autoRefresh_;
    int refreshInterval_;
    SimulationStep lastUpdateStep_;
    
    // Visualization data
    std::vector<VisualizationInterface::NetworkNode> networkData_;
    std::vector<VisualizationInterface::SpikePlotData> spikeData_;
    std::vector<VisualizationInterface::WeightMatrixData> weightData_;
};}

// Helper function for visualization
void printVisualizationInfo(const VisualizationInterface& viz);

} // namespace nlm
