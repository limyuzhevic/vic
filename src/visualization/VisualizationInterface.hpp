#pragma once

#include <string>
#include <vector>
#include <memory>

namespace nlm {

class Config;
class Brain;
class SpikeEvent;
class NeuronId;

// Visualization interface for debugging and analysis
// Enhanced Phase 2 implementation with real-time monitoring
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize visualization with enhanced capabilities
    bool initialize();
    
    // Update visualization data with brain state
    void update(const class Brain& brain);
    
    // Update visualization (legacy method)
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
    
    // Enhanced monitoring configuration
    void setHistorySize(size_t size);
    void setUpdateInterval(size_t interval);
    void setVisualizationOption(const std::string& option, bool enabled);
    
    // Network visualization
    void visualizeNetwork(const Brain& brain);
    
    // Activity visualization
    void visualizeActivity(const Brain& brain);
    
    // Spike raster plot
    void visualizeSpikes(const std::vector<SpikeEvent>& spikes);
    
    // Weight matrix visualization
    void visualizeWeights(const Brain& brain);
    
    // Memory system visualization
    void visualizeMemorySystems(const Brain& brain);
    
    // Neuromodulation visualization
    void visualizeNeuromodulation(const Brain& brain);
    
    // Development stage visualization
    void visualizeDevelopment(const Brain& brain);
    
    // Comprehensive console visualization
    void printConsoleVisualization(const Brain& brain);
    
private:
    // Real-time logging
    void logRealTimeStatus(const Brain& brain);
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
