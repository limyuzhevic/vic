#pragma once

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace nlm {

// Visualization interface for debugging and analysis
// Provides basic text-based visualization and logging for brain monitoring
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize visualization with logging file
    bool initialize(const std::string& logFile = "nlm_visualization.log");
    
    // Update visualization data
    void update();
    
    // Render current state to log file
    void render();
    
    // Close visualization
    void close();
    
    // Check if visualization is active
    bool isActive() const;
    
    // Set update rate
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    
    // Log brain statistics
    void logStatistics(const class Brain& brain, const std::string& label = "");
    
    // Log network connectivity
    void logNetwork(const class Brain& brain);
    
    // Log activity patterns
    void logActivity(const class Brain& brain);
    
    // Log spike events
    void logSpikes(const std::vector<class SpikeEvent>& spikes);
    
    // Log weight matrices
    void logWeights(const class Brain& brain);
    
    // Generate simple ASCII visualization
    std::string visualizeNetworkASCII(const class Brain& brain) const;
    std::string visualizeActivityASCII(const class Brain& brain) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
