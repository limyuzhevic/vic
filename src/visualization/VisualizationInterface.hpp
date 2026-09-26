#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <map>
#include <unordered_map>
#include <algorithm>

namespace nlm {

// Enhanced visualization interface for Phase 6
// Provides comprehensive neural network visualization and analysis
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize visualization with backend selection
    // backend: "text", "simple", "advanced", "auto"
    bool initialize(const std::string& backend = "auto");
    
    // Update visualization data
    void update();
    
    // Render current state with specified mode
    // mode: "network", "activity", "spikes", "weights", "integrated"
    void render(const std::string& mode = "integrated");
    
    // Close visualization and cleanup
    void close();
    
    // Check if visualization is active
    bool isActive() const;
    
    // Set update rate (Hz)
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    
    // Configuration
    void setOutputFile(const std::string& filename);
    void setMaxDisplayNeurons(size_t maxNeurons);
    void setEnableAnimation(bool enabled);
    void setAnimationSpeed(float speed);
    
    // Network visualization
    void visualizeNetwork(const class Brain& brain);
    
    // Activity visualization  
    void visualizeActivity(const class Brain& brain);
    
    // Spike raster plot
    void visualizeSpikes(const std::vector<class SpikeEvent>& spikes);
    
    // Weight matrix visualization
    void visualizeWeights(const class Brain& brain);
    
    // Performance metrics visualization
    void visualizeMetrics(const class BenchmarkMetrics& metrics);
    
    // Integration visualization (all systems)
    void visualizeIntegration(const class Brain& brain, const class BenchmarkMetrics& metrics);
    
    // Text-based output for terminal
    void outputTextReport(const std::string& filename = "");
    
    // Simple console visualization
    void consoleUpdate(const class Brain& brain, const class BenchmarkMetrics& metrics);
    
    // Export data for external tools
    bool exportData(const std::string& filename, const std::string& format = "csv");
    
    // Generate snapshots
    void captureSnapshot(const std::string& label);
    
    // Visualization backends
    enum class Backend { Text, Simple, Advanced, None };
    
    // Visualization modes
    enum class Mode { Network, Activity, Spikes, Weights, Metrics, Integration };
    
    // Get current visualization statistics
    struct VisualizationStats {
        size_t framesRendered = 0;
        double totalRenderTimeMs = 0.0;
        size_t neuronsVisualized = 0;
        size_t spikesRendered = 0;
        double cpuTimePercent = 0.0;
    };
    
    VisualizationStats getStats() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
