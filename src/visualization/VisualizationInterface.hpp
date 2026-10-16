#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../core/Types/Types.hpp"

namespace nlm {

class Brain;
class SpikeEvent;

// Forward declarations for visualization modes
class TextVisualizationMode;
class GraphicalVisualizationMode;
class WebVisualizationMode;

// Visualization interface for debugging and analysis
// Now supports multiple visualization modes
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
    
    // Get visualization status
    std::string getStatus() const;
    
    // Get web visualization status
    std::string getWebSocketStatus() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
