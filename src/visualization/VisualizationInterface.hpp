#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <functional>

namespace nlm {

// Advanced visualization for brain monitoring and analysis
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize visualization with backend selection
    bool initialize(const std::string& backend = "auto");
    
    // Update visualization data
    void update();
    
    // Render current state
    void render();
    
    // Close visualization
    void close();
    
    // Check if visualization is active
    bool isActive() const;
    
    // Set update rate and performance monitoring
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    
    // Performance monitoring
    double getFps() const;
    double getFrameTime() const;
    
    // Visualization modes
    enum class VisualizationMode {
        Network,
        Activity,
        Spikes,
        Weights,
        Memory,
        Neuromodulation,
        Development,
        Prediction,
        Cognitive,
        Custom
    };
    
    // Set visualization mode
    void setVisualizationMode(VisualizationMode mode);
    VisualizationMode getVisualizationMode() const;
    
    // Network visualization with advanced features
    void visualizeNetwork(const class Brain& brain, 
                        bool showConnections = true,
                        bool showNeurons = true,
                        double connectionThreshold = 0.5);
    
    // Enhanced activity visualization
    void visualizeActivity(const class Brain& brain,
                         const std::string& metric = "firing_rate",
                         bool showHeatmap = true,
                         bool showTimeSeries = false,
                         size_t historySize = 1000);
    
    // Advanced spike visualization
    void visualizeSpikes(const std::vector<class SpikeEvent>& spikes,
                       const std::map<int, std::vector<double>>& neuronSpikeTimes,
                       bool showRaster = true,
                       bool showHistograms = true,
                       bool showCrossCorrelations = false);
    
    // Weight matrix visualization with plasticity tracking
    void visualizeWeights(const class Brain& brain,
                        bool showHebbian = true,
                        bool showStdp = true,
                        bool showStructural = true,
                        double minWeight = -1.0,
                        double maxWeight = 1.0);
    
    // Multi-panel visualization dashboard
    void renderDashboard(const class Brain& brain,
                        const std::vector<VisualizationMode>& panels);
    
    // Real-time monitoring and statistics
    struct BrainStats {
        double averageFiringRate;
        double excitationInhibitionRatio;
        size_t totalSpikeCount;
        size_t activeNeuronCount;
        float averageWeight;
        float maxWeight;
        float minWeight;
        size_t totalSynapseCount;
        size_t totalNeuronCount;
        double energyConsumption;
        std::chrono::milliseconds simulationTime;
    };
    
    BrainStats getBrainStats(const class Brain& brain) const;
    
    // Export capabilities for analysis and documentation
    bool exportVisualization(const std::string& filename,
                            const std::string& format = "png",
                            const std::string& dataType = "all");
    
    // Custom visualization callbacks
    using VisualizationCallback = std::function<void(const class Brain&, double) >;
    void registerVisualizationCallback(VisualizationCallback callback);
    void unregisterVisualizationCallback(VisualizationCallback callback);
    
    // Configuration and settings
    struct VisualizationSettings {
        std::string backend;
        int windowWidth;
        int windowHeight;
        bool fullscreen;
        double targetFramerate;
        bool showStats;
        bool showControls;
        std::string theme;
        std::map<std::string, std::string> customSettings;
    };
    
    void setSettings(const VisualizationSettings& settings);
    const VisualizationSettings& getSettings() const;
    
    // Color schemes and theming
    enum class ColorScheme {
        Default,
        Scientific,
        Inverted,
        Custom
    };
    
    void setColorScheme(ColorScheme scheme);
    
    // Interaction and manipulation
    bool isMouseOver() const;
    std::pair<double, double> getMousePosition() const;
    void setInteractionEnabled(bool enabled);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
