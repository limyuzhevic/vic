#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <functional>
#include <atomic>

// Forward declarations
class Brain;
class SpikeEvent;
class Neuron;
class Synapse;

namespace nlm {

// Visualization modes
enum class VisualizationMode {
    NetworkGraph,      // Brain network structure
    ActivityPlot,      // Neural activity over time
    SpikeRaster,       // Spike timing visualization
    WeightMatrix,      // Synaptic weight distribution
    Heatmap,           // Activity heatmap
    3DView,           // 3D visualization
    RealTime,         // Real-time monitoring
    Custom            // Custom user-defined visualization
};

// Data export formats
enum class ExportFormat {
    JSON,             // JSON format
    CSV,              // Comma-separated values
    TEXT,             // Plain text
    PNG,              // PNG image
    SVG,              // SVG vector graphics
    VIDEO,            // Video recording
    DAT,              // Raw data format
    MATLAB,           // MATLAB format
    PYTHON           // Python data format
};

// Visualization data structure
struct VisualizationData {
    std::string name;
    std::string description;
    VisualizationMode mode;
    double updateRate;  // Hz
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, double> metrics;
    std::vector<std::vector<double>> heatmapData;
    std::vector<std::vector<float>> weightData;
    std::vector<std::pair<double, int>> spikeData;
    std::vector<std::pair<double, double>> activityData;
    
    VisualizationData() : updateRate(30.0) {}
};

// Visualization callback for custom visualizations
using VisualizationCallback = std::function<void(const VisualizationData&)>;

// Enhanced Visualization interface for debugging and analysis
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize visualization with specific settings
    bool initialize(const std::string& title = "NLM Visualization", 
                   int width = 1024, int height = 768);
    
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
    
    // Set visualization mode
    void setMode(VisualizationMode mode);
    VisualizationMode getMode() const;
    
    // Visualization methods for different data types
    void visualizeNetwork(const Brain& brain);
    void visualizeActivity(const Brain& brain);
    void visualizeSpikes(const std::vector<SpikeEvent>& spikes);
    void visualizeWeights(const Brain& brain);
    void visualizeHeatmap(const Brain& brain);
    void visualize3D(const Brain& brain);
    
    // Real-time monitoring
    void startRealTimeMonitoring();
    void stopRealTimeMonitoring();
    bool isMonitoring() const;
    
    // Data export
    bool exportData(const std::string& filepath, ExportFormat format = ExportFormat::CSV);
    bool exportVisualizationData(const VisualizationData& data, const std::string& filepath);
    
    // Callback system for custom visualizations
    void addVisualizationCallback(VisualizationCallback callback);
    void removeVisualizationCallback(VisualizationCallback callback);
    
    // Screen capture and recording
    bool captureScreen(const std::string& filepath);
    bool startRecording(const std::string& filepath, double fps = 30.0);
    void stopRecording();
    bool isRecording() const;
    
    // Configuration
    void setConfig(const std::string& key, const std::string& value);
    std::string getConfig(const std::string& key) const;
    void loadConfig(const std::string& filepath);
    void saveConfig(const std::string& filepath) const;
    
    // Statistical analysis
    struct StatisticalAnalysis {
        double meanFiringRate;
        double stdDevFiringRate;
        double maxFiringRate;
        double minFiringRate;
        double meanWeight;
        double stdDevWeight;
        double connectivityRatio;
        size_t activeNeuronCount;
        size_t totalSpikeCount;
        double synchronyMeasure;
        double informationContent;
        std::chrono::system_clock::time_point analysisTime;
    };
    
    StatisticalAnalysis analyzeBrainStatistics(const Brain& brain);
    
    // Performance monitoring
    struct PerformanceMetrics {
        double cpuUsage;           // Percentage
        double memoryUsage;        // Percentage
        double updateTime;         // Milliseconds
        double renderTime;         // Milliseconds
        size_t neuronCount;        // Current neuron count
        size_t synapseCount;       // Current synapse count
        size_t activeSpikeCount;   // Active spike events
        double fps;                // Frames per second
        double memoryFootprint;   // MB
    };
    
    PerformanceMetrics getPerformanceMetrics() const;
    
    // Progress tracking
    struct ProgressInfo {
        std::string taskName;
        double progress;           // 0.0 to 1.0
        std::string status;        // Current status message
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point estimatedEnd;
    };
    
    void setProgress(const ProgressInfo& progress);
    ProgressInfo getProgress() const;
    
    // Color schemes and theming
    enum class ColorScheme {
        Default,
        Dark,
        Light,
        Scientific,
        Rainbow,
        Mono,
        Custom
    };
    
    void setColorScheme(ColorScheme scheme);
    ColorScheme getColorScheme() const;
    
    // Font and text settings
    void setFont(const std::string& fontName, int fontSize = 12);
    void setTextColor(double r, double g, double b, double a = 1.0);
    
    // Interaction and input handling
    bool handleInput(double deltaTime);
    bool isInputMode() const;
    
    // Window management
    void setWindowTitle(const std::string& title);
    void setWindowIcon(const std::string& iconPath);
    void setWindowSize(int width, int height);
    std::pair<int, int> getWindowSize() const;
    
    // Multiple display support
    void addDisplay(int monitorIndex, int x, int y, int width, int height);
    void removeDisplay(int monitorIndex);
    bool isDisplayActive(int monitorIndex) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
