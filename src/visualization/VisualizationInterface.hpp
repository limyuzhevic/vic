#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>

namespace nlm {

class NetworkGraph;
class ActivityPlotter;

// Advanced visualization interface for debugging and analysis
// Phase 6: Real visualization with network graphs and activity plots
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
    
    // Set update rate (frames per second)
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    
    // Enable/disable specific visualization components
    void enableNetwork(bool enable);
    void enableActivity(bool enable);
    void enableWeights(bool enable);
    
    // Toggle auto layout updates
    void setAutoLayout(bool autoLayout);
    
    // Add data to visualization
    void visualizeNetwork(const NetworkGraph& graph);
    void visualizeActivity(const ActivityPlotter& plotter);
    
    // Add individual events
    void addSpikeEvent(double time, int neuronId, float amplitude = 1.0f);
    void addTimeSeries(const std::string& name, const std::vector<float>& values, const std::string& color = "");
    
    // Update visualization from brain state
    void updateFromBrain(const class Brain& brain);
    
    // Performance measurement
    void startFrameTimer();
    void endFrameTimer();
    double getAverageFrameTime() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
