#pragma once

#include <string>
#include <vector>
#include <memory>

namespace nlm {

class Brain;
class VisualizationEngine;

// Visualization interface for debugging and analysis
// Updated to use the new comprehensive visualization system
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize visualization with brain
    bool initialize(std::shared_ptr<Brain> brain);
    
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
    
    // Set display mode
    void setDisplayMode(int mode);
    int getDisplayMode() const;
    
    // Set brain reference
    void setBrain(std::shared_ptr<Brain> brain);
    
    // Network visualization (delegated to visualization engine)
    void visualizeNetwork(const Brain& brain);
    
    // Activity visualization (delegated to visualization engine)
    void visualizeActivity(const Brain& brain);
    
    // Spike raster plot (delegated to visualization engine)
    void visualizeSpikes(const std::vector<class SpikeEvent>& spikes);
    
    // Weight matrix visualization (delegated to visualization engine)
    void visualizeWeights(const Brain& brain);
    
    // Export functionality
    bool exportData(const std::string& filename, const std::string& format = "ascii");
    
    // Configuration management
    void loadConfiguration(const std::string& configFile);
    void saveConfiguration() const;
    
    // Engine access
    VisualizationEngine* getEngine() const { return engine_.get(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::unique_ptr<VisualizationEngine> engine_;
};

} // namespace nlm
