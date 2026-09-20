#pragma once

#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Enhanced visualization interface for debugging and analysis
// Real implementation with modern graphics capabilities
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize visualization with window settings
    bool initialize(int width = 1600, int height = 900, 
                   const std::string& title = "NLM Neural Network Visualization");
    
    // Update visualization data
    void update();
    
    // Render current state
    void render();
    
    // Close visualization
    void close();
    
    // Check if visualization is active
    bool isActive() const;
    
    // Set update rate
    void setUpdateRate(double hz) override;
    double getUpdateRate() const override;
    
    // Network visualization
    void visualizeNetwork(const class Brain& brain) override;
    
    // Activity visualization
    void visualizeActivity(const class Brain& brain) override;
    
    // Spike raster plot
    void visualizeSpikes(const std::vector<class SpikeEvent>& spikes) override;
    
    // Weight matrix visualization
    void visualizeWeights(const class Brain& brain) override;
    
    // Real-time monitoring controls
    void setSimulationSpeed(double speed);
    void togglePause();
    bool isPaused() const;
    
    // Debug controls
    void toggleDebugMode();
    void toggleGrid(bool show);
    void toggleLabels(bool show);
    void setColorScheme(int scheme);
    
    // Export capabilities
    void exportVisualizationData(const std::string& filename);
    void exportFrame(int frameIndex);
    
    // Event timeline
    void addEvent(const std::string& eventType, const std::string& description, double timestamp);
    
    // Interactive features
    bool handleInput();
    void processZoom(int delta);
    void processPan(float dx, float dy);
    
    // State inspection
    void inspectNeuralState(const class Brain& brain);
    void updateMemoryUsage();
    void updatePerformanceMetrics();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
