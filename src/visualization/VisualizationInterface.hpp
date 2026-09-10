#pragma once

#include <string>
#include <vector>
#include <memory>
#include "VisualizationConfig.hpp"
#include "VisualizationSystem.hpp"

namespace nlm {

// Visualization interface for debugging and analysis
// EXTENDED - Full visualization system with recording and monitoring
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize visualization
    bool initialize(const VisualizationSettings& settings = VisualizationSettings());
    
    // Initialize with configuration
    bool initialize(std::shared_ptr<class Config> config);
    
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
    void visualizeNetwork(const class Brain& brain);
    
    // Activity visualization
    void visualizeActivity(const class Brain& brain);
    
    // Spike raster plot
    void visualizeSpikes(const std::vector<class SpikeEvent>& spikes);
    
    // Weight matrix visualization
    void visualizeWeights(const class Brain& brain);
    
    // Set brain for visualization
    void setBrain(std::shared_ptr<class Brain> brain);
    
    // Update with brain state (hook for automatic updates)
    void updateWithBrain(const class Brain& brain, size_t step, double time);
    
    // Recording system
    bool startRecording(const std::string& filepath = "");
    void stopRecording();
    bool isRecording() const;
    
    // Replay system
    bool loadReplay(const std::string& filepath);
    void startReplay();
    void stopReplay();
    bool isReplaying() const;
    size_t getCurrentReplayFrame() const;
    size_t getTotalFrames() const;
    
    // Export data
    bool exportData(const std::string& outputPath, const std::string& format = "csv");
    bool exportVisualizationData(const std::string& outputPath, const std::string& format = "csv");
    
    // Performance monitoring
    const PerformanceMonitor::Metrics& getMetrics() const;
    
    // Visualization settings
    void setSettings(const VisualizationSettings& settings);
    const VisualizationSettings& getSettings() const;
    
    // System status
    bool isInitialized() const;
    bool isActive() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
