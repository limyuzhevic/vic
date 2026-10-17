#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>

namespace nlm {

// Visualization output formats
enum class VisualizationFormat {
    TEXT,
    ASCII_ART,
    JSON,
    CSV,
    HTML,
    SVG,
    PNG,
    MATLAB
};

// Visualization backend types
enum class VisualizationBackend {
    NONE,
    ASCII_CONSOLE,
    FILE_OUTPUT,
    WEB_SOCKET,
    DISPLAY_WINDOW
};

// Visualization interface for debugging and analysis
// Enhanced with multiple backends, formats, and advanced features
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize visualization with specified backend
    bool initialize(VisualizationBackend backend = VisualizationBackend::ASCII_CONSOLE);
    
    // Update visualization data
    void update();
    
    // Render current state to configured output
    void render();
    
    // Close visualization and cleanup resources
    void close();
    
    // Check if visualization is active
    bool isActive() const;
    
    // Set update rate
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    
    // Configure output format and destination
    void setOutputFormat(VisualizationFormat format);
    VisualizationFormat getOutputFormat() const;
    
    void setOutputFile(const std::string& filepath);
    std::string getOutputFile() const;
    
    // Network visualization
    void visualizeNetwork(const class Brain& brain);
    
    // Activity visualization with advanced options
    void visualizeActivity(const class Brain& brain, 
                          bool showNeurons = true,
                          bool showConnections = true,
                          bool showSpikes = true);
    
    // Spike raster plot with statistics
    void visualizeSpikes(const std::vector<class SpikeEvent>& spikes,
                        bool showStatistics = true,
                        double timeWindow = 1000.0);
    
    // Weight matrix visualization with color mapping
    void visualizeWeights(const class Brain& brain,
                         bool showPositive = true,
                         bool showNegative = true,
                         double minWeight = -1.0,
                         double maxWeight = 1.0);
    
    // Multi-layer visualization
    void visualizeBrainLayers(const class Brain& brain);
    
    // Activity heatmaps
    void visualizeActivityHeatmap(const class Brain& brain,
                                 const std::string& metric = "firing_rate");
    
    // Time series visualization
    void visualizeTimeSeries(const class Brain& brain,
                           const std::string& seriesName,
                           size_t duration = 1000);
    
    // Set custom visualization callback
    void setCustomRenderer(std::function<void()> renderer);
    
    // Batch visualization updates
    void addBatchUpdate(std::function<void()> updateFunc);
    void processBatchUpdates();
    
    // Configuration and parameters
    struct VisualizationConfig {
        VisualizationBackend backend = VisualizationBackend::ASCII_CONSOLE;
        VisualizationFormat format = VisualizationFormat::ASCII_ART;
        std::string outputFile;
        double updateRate = 30.0;
        bool enableColor = true;
        bool enableAnimations = false;
        size_t maxHistoryPoints = 1000;
        std::map<std::string, std::string> parameters;
    };
    
    void setConfig(const VisualizationConfig& config);
    VisualizationConfig getConfig() const;
    
    // Performance and export
    double getLastFrameTime() const;
    size_t getTotalFramesRendered() const;
    void exportVisualization(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
