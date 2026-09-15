#pragma once

namespace nlm {

// Forward declarations
class Brain;
class VisualizationSystem;

// Core visualization system integration with Brain
class VisualizationSystemIntegration {
public:
    VisualizationSystemIntegration();
    ~VisualizationSystemIntegration();
    
    // Initialize integration with Brain
    bool initialize(Brain* brain, VisualizationSystem* visualizer);
    
    // Update integration (called from Brain::step)
    void update(double timestep);
    
    // Synchronize visualization with Brain state
    void synchronize();
    
    // Close integration
    void close();
    
    // Configuration
    void setUpdateRate(double hz);
    void setSyncMode(int mode);  // 0: per-step, 1: per-frame, 2: adaptive
    void setVisualizationMode(int mode); // Refer to VisualizationSystem::VisualizationMode
    
    // Getters
    bool isActive() const;
    double getUpdateRate() const;
    Brain* getBrain(); // Add this method
    
    // Statistics
    struct IntegrationStats {
        double integrationTime;          // Time spent on integration (ms)
        double syncTime;                 // Time spent on synchronization (ms)
        size_t framesRendered;           // Total frames rendered
        size_t updatesProcessed;         // Total updates processed
        double cpuUsage;                 // CPU usage percentage
        double memoryUsage;              // Memory usage (MB)
        int droppedFrames;               // Number of dropped frames
        double averageFrameTime;         // Average frame time (ms)
        double frameRate;                // Current frame rate (Hz)
        std::vector<double> frameTimes;  // Frame time history
    };
    
    IntegrationStats getStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
