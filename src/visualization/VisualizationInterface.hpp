#pragma once

#include <string>
#include <vector>
#include <memory>

namespace nlm {

class Brain;

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
    
    // Weight matrix visualization
    void visualizeWeights(const class Brain& brain);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// File-based visualization functions for debugging and analysis
namespace DebugVisualization {
    
    // Save brain state as a graph file
    void saveBrainGraph(const Brain& brain, const std::string& filename);
    
    // Generate spike train visualization
    void generateSpikeTrainPlot(const Brain& brain, const std::string& filename);
    
    // Create network graph representation
    void createNetworkGraph(const Brain& brain, const std::string& filename);
    
    // Generate comprehensive debug report
    void generateDebugReport(const Brain& brain, const std::string& filename);
    
    // Get current timestamp as string
    std::string getCurrentTimestamp();
    
} // namespace DebugVisualization

} // namespace nlm