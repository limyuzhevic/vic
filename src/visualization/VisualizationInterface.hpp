#pragma once

#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Visualization interface for debugging and analysis
// Phase 2: Real visualization with GLFW/OpenGL support
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
    
    // Configure visualization options
    void setShowNetwork(bool show);
    void setShowActivity(bool show);
    void setShowSpikes(bool show);
    void setShowWeights(bool show);
    void setView(int view); // 0: Network, 1: Activity, 2: Spikes, 3: Weights
    void setNodeSize(float size);
    void setConnectionWidth(float width);
    
    // Network visualization
    void visualizeNetwork(const class Brain& brain);
    
    // Activity visualization
    void visualizeActivity(const class Brain& brain);
    
    // Spike raster plot
    void visualizeSpikes(const std::vector<class SpikeEvent>& spikes);
    
    // Weight matrix visualization
    void visualizeWeights(const class Brain& brain);
    
    // Get current view for debugging
    int getCurrentView() const;
    float getNodeSize() const;
    float getConnectionWidth() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
