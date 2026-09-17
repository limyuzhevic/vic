#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <optional>

namespace nlm {

// Forward declarations
class Brain;
class PerformanceProfiler;
class CheckpointManager;

/**
 * VisualizationInterface for production-ready neural network visualization
 * 
 * Provides real-time visualization with WebGL/OpenGL rendering,
 * neural activity monitoring, network topology visualization,
 * and learning progress monitoring.
 * 
 * Features:
 * - Real-time neural activity monitoring with activity heatmaps
 * - Interactive 3D visualization using WebGL/OpenGL
 * - Network topology visualization with synaptic connections
 * - Learning progress monitoring with metrics dashboard
 * - Performance metrics integration
 * - Checkpoint synchronization
 * 
 * Components:
 * - Real-time visualization of neural activity
 * - Interactive network graphs with zoom/pan
 * - Neural activity heatmaps (activity over time)
 * - Synaptic weight visualization
 * - Spike raster plots
 * - Simulation timeline with progress tracking
 */
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize visualization with optional headless mode
    bool initialize(bool headless = false, const std::string& configPath = "");
    
    // Update visualization data
    void update(const Brain& brain, PerformanceProfiler* profiler = nullptr,
                CheckpointManager* checkpointManager = nullptr);
    
    // Render current state
    void render();
    
    // Close visualization and cleanup
    void close();
    
    // Check if visualization is active
    bool isActive() const;
    
    // Set update rate
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    
    // Configuration
    bool setRenderMode(const std::string& mode);
    bool setColorMap(const std::string& colormap);
    bool setNetworkLayout(const std::string& layout);
    
    // Data visualization
    void visualizeNetwork(const Brain& brain);
    void visualizeActivity(const Brain& brain);
    void visualizeSpikes(const std::vector<class SpikeEvent>& spikes);
    void visualizeWeights(const Brain& brain);
    
    // Progress and metrics visualization
    void visualizeProgress(double progress, const std::string& message = "");
    void visualizeMetrics(const PerformanceMetrics& metrics);
    void visualizeCheckpointStatus(const std::string& checkpointPath, 
                                   double progress = 0.0);
    
    // Interactive controls
    bool setCameraPosition(double x, double y, double z);
    bool setZoomLevel(double level);
    bool resetView();
    
    // File output
    bool saveScreenshot(const std::string& filepath);
    bool exportAnimation(const std::string& filepath, double fps = 30.0,
                        int durationSeconds = 60);
    
    // Monitoring APIs
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoring() const;
    
    // Event callbacks
    using VisualizationEventCallback = std::function<void(const std::string&)>;
    void setEventCallback(VisualizationEventCallback callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Prevent copying
    VisualizationInterface(const VisualizationInterface&) = delete;
    VisualizationInterface& operator=(const VisualizationInterface&) = delete;
};

// Progress reporting for long-running operations
struct ProgressReport {
    double progress;                    // 0.0 to 1.0
    std::string message;                // Human-readable message
    std::chrono::steady_clock::time_point startTime;  // When operation started
    std::optional<std::chrono::steady_clock::time_point> endTime;  // When operation ends
    double estimatedTimeRemaining;       // Estimated seconds remaining
    std::string currentPhase;           // Current phase of operation
    
    ProgressReport(double p = 0.0, const std::string& msg = "",
                   const std::string& phase = "Idle")
        : progress(p), message(msg), startTime(std::chrono::steady_clock::now()),
          endTime(std::nullopt), estimatedTimeRemaining(0.0), currentPhase(phase) {}
    
    bool isActive() const { return !endTime.has_value(); }
    double getElapsedTime() const {
        auto end = endTime.has_value() ? endTime.value() : std::chrono::steady_clock::now();
        return std::chrono::duration<double>(end - startTime).count();
    }
};

} // namespace nlm
