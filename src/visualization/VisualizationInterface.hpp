#pragma once

#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Forward declarations
class Brain;
class SpikeEvent;

// Visualization interface for debugging and analysis
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
    
    // Performance profiling helper functions
    void startPerformanceProfile();
    void endPerformanceProfile();
    double getProfileDuration() const;
    
    // Debug session management
    void startDebugSession(const std::string& sessionName);
    void endDebugSession();
    bool isDebugMode() const;
    void setDebugData(const std::string& key, const std::string& value);
    std::string getDebugData(const std::string& key) const;
    
    // Export debugging data
    bool exportDebugData(const std::string& filepath);
    
    // Batch processing for debugging
    void batchProcess(const Brain& brain, size_t steps);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
