#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <algorithm>
#include <cmath>

namespace nlm {

// Real-time console visualization for NLM debugging and analysis
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize visualization
    bool initialize();
    
    // Update visualization data
    void update();
    
    // Render current state to console
    void render();
    
    // Close visualization
    void close();
    
    // Check if visualization is active
    bool isActive() const;
    
    // Set update rate
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    
    // Enable/disable different visualization modes
    void enableNetworkVisualization(bool enable);
    void enableActivityVisualization(bool enable);
    void enableSpikeVisualization(bool enable);
    void enableWeightVisualization(bool enable);
    void enableStatisticsVisualization(bool enable);
    
    // Network visualization
    void visualizeNetwork(const class Brain& brain);
    
    // Activity visualization
    void visualizeActivity(const class Brain& brain);
    
    // Spike raster plot
    void visualizeSpikes(const std::vector<class SpikeEvent>& spikes);
    
    // Weight matrix visualization
    void visualizeWeights(const class Brain& brain);
    
    // Statistical visualization
    void visualizeStatistics(const class Brain& brain, double timestep);
    
    // Progress tracking
    void setProgress(const std::string& phase, double progress);
    
    // Set color theme
    void setColorTheme(const std::string& theme);
    
    // Clear screen
    static void clearScreen();
    
    // Get formatted timestamp
    static std::string getTimestamp();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm