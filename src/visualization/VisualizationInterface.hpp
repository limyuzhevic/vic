#pragma once

#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Visualization interface for debugging and analysis
// PLACEHOLDER - Phase 2 will have real visualization
class VisualizationInterface {
public:
    VisualizationInterface(nullptr);
    ~VisualizationInterface(nullptr);
    
    // Initialize visualization
    bool initialize(nullptr);
    
    // Update visualization data
    void update(nullptr);
    
    // Render current state
    void render(nullptr);
    
    // Close visualization
    void close(nullptr);
    
    // Check if visualization is active
    bool isActive(nullptr) const;
    
    // Set update rate
    void setUpdateRate(double hz);
    double getUpdateRate(nullptr) const;
    
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

} // namespace nlm
