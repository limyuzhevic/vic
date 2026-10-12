#pragma once

#include <string>
#include <vector>
#include <memory>
#include "AdvancedVisualization.hpp"

namespace nlm {

// Visualization interface for debugging and analysis
// Updated to use the comprehensive AdvancedVisualization system
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize visualization
    bool initialize(const std::string& configFile = "");
    
    // Update visualization data
    void update(double deltaTime = 0.0);
    
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
    
    // Advanced visualization integration
    void setAdvancedVisualization(AdvancedVisualization* advancedVis);
    void run(const class Brain& brain, double deltaTime);
    
    // Component control
    void enableComponent(const std::string& name);
    void disableComponent(const std::string& name);
    bool isComponentEnabled(const std::string& name) const;
    
    // Export capabilities
    bool exportData(const std::string& filename, OutputFormat format = OutputFormat::JSON);
    
    // Performance information
    double getFPS() const;
    double getLastFrameTime() const;
    uint64_t getTotalFramesRendered() const;
    std::string getSystemStatus() const;
    
    // Debug console integration
    void setDebugConsole(class DebugConsole* console);
    
    // Plugin management
    bool registerPlugin(const std::string& pluginName);
    void unregisterPlugin(const std::string& pluginName);
    std::vector<std::string> getActivePlugins() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
