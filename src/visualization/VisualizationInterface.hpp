#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>

namespace nlm {

// Visualization interface for debugging and analysis
// PLACEHOLDER - Phase 2 will have real visualization
class VisualizationInterface {
public:
    enum class VizType { Network, Activity, Spikes, Weights };
    
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize visualization
    bool initialize(bool enabled = false, double updateRate = 30.0, 
                   VizType vizType = VizType::Network, 
                   const std::string& windowTitle = "NLM Visualization", 
                   int width = 1280, int height = 720);
    
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
    
    // Set visualization type
    void setVizType(VizType type);
    VizType getVizType() const;
    
    // Network visualization
    void visualizeNetwork(const class Brain& brain);
    
    // Activity visualization
    void visualizeActivity(const class Brain& brain);
    
    // Spike raster plot
    void visualizeSpikes(const std::vector<class SpikeEvent>& spikes);
    
    // Weight matrix visualization
    void visualizeWeights(const class Brain& brain);
    
    // Add neural data for visualization (called from Brain::step)
    void feedNeuralData(const class Brain& brain, uint64_t step, double time);
    
    // Add spike event data for visualization
    void feedSpikeData(const class SpikeEvent& spike);
    
    // Get performance metrics
    double getFrameTime() const;
    double getFPS() const;
    uint64_t getTotalFramesRendered() const;
    
    // Toggle visualization on/off
    void toggleActive();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
