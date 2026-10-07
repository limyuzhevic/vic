#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <unordered_map>
#include <mutex>

namespace nlm {

class Brain;
class SpikeEvent;

// Visualization output formats
enum class OutputFormat {
    PNG,
    SVG,
    PDF,
    Video,
    WebGL,
    JSON
};

// Network visualization styles
enum class NetworkStyle {
    Graph,
    Circular,
    Hierarchical,
    Planar,
    ForceDirected
};

// Activity visualization modes
enum class ActivityMode {
    FiringRate,
    MembranePotential,
    Calcium,
    SpikeTiming,
    PopulationActivity
};

// Weight matrix visualization modes
enum class WeightMode {
    SignedWeights,
    AbsoluteWeights,
    NormalizedWeights,
    LogTransformed
};

class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize visualization with OpenGL/Vulkan backend
    bool initialize(bool useVulkan = false, bool useOpenGL = true, 
                   int width = 1920, int height = 1080);
    
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
    
    // Configuration methods
    void setWindowTitle(const std::string& title);
    void setBackgroundColor(float r, float g, float b, float a = 1.0f);
    void setShowGrid(bool show);
    void setShowLabels(bool show);
    void setShowConnections(bool show);
    
    // Network visualization
    void visualizeNetwork(const Brain& brain, 
                        NetworkStyle style = NetworkStyle::Graph,
                        float nodeScale = 1.0f);
    
    // Activity visualization
    void visualizeActivity(const Brain& brain,
                         ActivityMode mode = ActivityMode::FiringRate,
                         float intensityScale = 1.0f);
    
    // Spike raster plot
    void visualizeSpikes(const std::vector<SpikeEvent>& spikes,
                       double duration = 1000.0,
                       int maxSpikes = 10000);
    
    // Weight matrix visualization
    void visualizeWeights(const Brain& brain,
                        WeightMode mode = WeightMode::SignedWeights,
                        float minWeight = -1.0f,
                        float maxWeight = 1.0f);
    
    // Export functionality
    bool exportVisualization(const std::string& filename, 
                           OutputFormat format = OutputFormat::PNG,
                           int quality = 90);
    
    // Capture screenshot
    std::vector<unsigned char> captureScreenshot();
    
    // Get visualization statistics
    struct VisualizationStats {
        double lastFrameTime;
        double fps;
        uint64_t trianglesRendered;
        uint64_t verticesRendered;
        size_t memoryUsage;
        double cpuUsage;
        double gpuUsage;
    };
    
    VisualizationStats getStats() const;
    
    // Debugging integration
    void enableDebugMode(bool enable);
    void setDebugLevel(int level);
    
    // Progress tracking
    void setProgress(double progress);
    void setStatus(const std::string& status);
    
    // Time series for plotting
    void addTimeSeriesData(const std::string& name,
                         const std::vector<double>& data,
                         double duration = 60.0);
    void clearTimeSeries();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
