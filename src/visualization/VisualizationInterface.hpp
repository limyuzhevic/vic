#pragma once

#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Visualization interface for debugging and analysis
// Real terminal-based visualization implementation
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
    
    // Terminal-based visualization methods (private implementation details)
    void visualizeNetworkTerminal();
    void visualizeActivityTerminal();
    void visualizeSpikesTerminal();
    void visualizeWeightsTerminal();
    
    // Helper methods
    std::string getColorCode(float value, const std::string& scheme);
    std::string getNeuronSymbol(float activity);
    void drawRectangle(int x, int y, int width, int height);
    void drawText(int x, int y, const std::string& text);
    void drawLine(int x1, int y1, int x2, int y2);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
