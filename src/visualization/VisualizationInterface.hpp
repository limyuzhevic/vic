#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../sensory/Vision.hpp"
#include "../dynamics/SpikeSystem.hpp"

namespace nlm {

// Visualization interface for debugging and analysis
// REAL IMPLEMENTATION - Phase 2 real visualization
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
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
