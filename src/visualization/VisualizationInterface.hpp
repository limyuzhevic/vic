#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <functional>
#include <map>
#include "../brain/Brain.hpp"

namespace nlm {

// Visualization modes for different analysis views
enum class VisualizationMode {
    NetworkGraph,
    ActivityHeatmap,
    SpikeRaster,
    WeightMatrix,
    MemoryTrace,
    Neuromodulation,
    DevelopmentTimeline,
    Custom
};

// Visualization output formats
enum class OutputFormat {
    ASCII,
    Text,
    CSV,
    JSON,
    HTML,
    PNG,
    SVG,
    Custom
};

// Visualization statistics
struct VisualizationStats {
    double updateRate;
    double renderTime;
    size_t totalFrames;
    float memoryUsage;
    int activeWindows;
    std::chrono::steady_clock::time_point lastUpdate;
};

// Visualization interface for debugging and analysis
// Enhanced for Phase 6 with comprehensive visualization capabilities
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize visualization with configuration
    bool initialize(const std::string& config = "auto");
    
    // Update visualization data
    void update();
    
    // Render current state with specified mode
    void render(VisualizationMode mode = VisualizationMode::NetworkGraph);
    
    // Close visualization
    void close();
    
    // Check if visualization is active
    bool isActive() const;
    
    // Set update rate with bounds checking
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    
    // Visualization methods for different data types
    void visualizeNetwork(const Brain& brain);
    void visualizeActivity(const Brain& brain);
    void visualizeSpikes(const std::vector<SpikeEvent>& spikes);
    void visualizeWeights(const Brain& brain);
    void visualizeMemory(const Brain& brain);
    void visualizeNeuromodulation(const Brain& brain);
    void visualizeDevelopment(const Brain& brain);
    
    // Export capabilities for analysis
    bool exportData(const std::string& filename, OutputFormat format = OutputFormat::JSON);
    void exportNetworkGraph(const std::string& filename);
    void exportActivityData(const std::string& filename);
    void exportSpikeData(const std::string& filename);
    
    // Interactive features
    void setInteractiveMode(bool enabled);
    bool isInteractiveMode() const;
    
    // Visualization control
    void setVisualizationMode(VisualizationMode mode);
    VisualizationMode getVisualizationMode() const;
    
    // View control
    void zoom(float factor);
    void pan(float dx, float dy);
    void resetView();
    
    // Color schemes and theming
    void setColorScheme(const std::string& scheme);
    void setHighlightNeurons(const std::vector<NeuronId>& neuronIds);
    void clearHighlights();
    
    // Statistical overlay
    void toggleStatsOverlay(bool enabled);
    void setStatDisplayPosition(const std::string& position);
    
    // Callbacks for interactive visualization
    using VisualizationCallback = std::function<void(const Brain&, VisualizationMode)>;
    
    void setUpdateCallback(VisualizationCallback callback);
    void setRenderCallback(VisualizationCallback callback);
    
    // Statistics
    VisualizationStats getStats() const;
    
    // Information display
    void displayNetworkInfo(const Brain& brain) const;
    void displayMemoryInfo(const Brain& brain) const;
    void displayPerformanceInfo() const;
    
    // Configuration management
    bool loadConfig(const std::string& configFile);
    bool saveConfig(const std::string& configFile) const;
    
    // Multiple window support
    void createWindow(const std::string& title, VisualizationMode mode, int x = 0, int y = 0);
    void destroyWindow(const std::string& title);
    void arrangeWindows(const std::string& layout);
    
    // Real-time monitoring
    void startMonitoring(bool enabled = true);
    void setMonitoringInterval(double interval);
    
    // Data logging
    void startLogging(const std::string& logfile);
    void stopLogging();
    bool isLogging() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
