#pragma once

namespace nlm {

// Forward declarations
class VisualizationEngine;
class NetworkVisualizer;
class ActivityVisualizer;
class RasterVisualizer;
class WeightVisualizer;
class DevelopmentVisualizer;

class VisualizationSystem {
public:
    VisualizationSystem();
    ~VisualizationSystem();
    
    // Initialize the visualization system
    bool initialize();
    
    // Update all visualization components
    void update(const class Brain& brain, double timestep);
    
    // Render all visualizations
    void render();
    
    // Close and cleanup visualization resources
    void close();
    
    // Check if visualization system is active
    bool isActive() const;
    
    // Set update rate in Hz
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    
    // Register individual visualizers
    void registerNetworkVisualizer(std::unique_ptr<NetworkVisualizer> visualizer);
    void registerActivityVisualizer(std::unique_ptr<ActivityVisualizer> visualizer);
    void registerRasterVisualizer(std::unique_ptr<RasterVisualizer> visualizer);
    void registerWeightVisualizer(std::unique_ptr<WeightVisualizer> visualizer);
    void registerDevelopmentVisualizer(std::unique_ptr<DevelopmentVisualizer> visualizer);
    
    // Configure visualization settings
    void setShowNeurons(bool show);
    void setShowConnections(bool show);
    void setColorScheme(int scheme);  // 0: default, 1: heat, 2: type-based, etc.
    void setNeuronLabels(bool show);
    void setBackgroundColor(float r, float g, float b, float a = 1.0f);
    void setZoom(float factor);
    void setRotation(float x, float y, float z);
    void setProjectionMode(int mode);  // 0: orthographic, 1: perspective
    
    // Export functionality
    bool exportData(const std::string& filepath, ExportFormat format);
    
    // Getters
    NetworkVisualizer* getNetworkVisualizer();
    ActivityVisualizer* getActivityVisualizer();
    RasterVisualizer* getRasterVisualizer();
    WeightVisualizer* getWeightVisualizer();
    DevelopmentVisualizer* getDevelopmentVisualizer();
    
    // Performance monitoring
    struct PerformanceMetrics {
        double currentFrameTime;      // ms
        double averageFrameTime;      // ms
        double frameRate;             // Hz
        size_t currentMemoryUsage;    // bytes
        size_t peakMemoryUsage;       // bytes
        int activeVisualizers;        // count
        double lastUpdateTime;        // seconds
    };
    
    PerformanceMetrics getPerformanceMetrics() const;
    void resetPerformanceMetrics();
    
    // Toggle visualization modes
    enum VisualizationMode {
        NetworkOnly,
        ActivityOnly,
        RasterOnly,
        WeightOnly,
        DevelopmentOnly,
        CombinedNetworkActivity,
        CombinedAll
    };
    
    void setVisualizationMode(VisualizationMode mode);
    VisualizationMode getVisualizationMode() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
