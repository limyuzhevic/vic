// Visualization Interface for NLM
// Provides real-time visualization of neural network activity, spikes, and network dynamics
// Supports multiple output backends: OpenGL, SDL, console, web, etc.

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <chrono>

#include "../brain/Brain.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Visualization modes
enum class VisualizationMode {
    StaticGraph,    // Static network graph
    SpikeRaster,    // Spike raster plot
    ActivityHeatmap, // Neural activity heatmap
    WeightMatrix,   // Synaptic weight visualization
    TimeSeries,     // Time-series of neural activity
    3DNeural,       // 3D neural structure visualization
    Custom         // User-defined visualization
};

// Output backends
enum class VisualizationBackend {
    OpenGL,         // OpenGL rendering
    SDL,           // SDL windowing + OpenGL
    Console,        // Console-based visualization
    WebGL,          // Web-based visualization
    Image,          // Static image output
    Video,          // Video output
    File,           // File-based visualization
    Null           // No visualization
};

// Visualization configuration
struct VisualizationConfig {
    // Core settings
    bool enabled = false;
    VisualizationMode mode = VisualizationMode::StaticGraph;
    VisualizationBackend backend = VisualizationBackend::Null;
    
    // Display settings
    int width = 1280;
    int height = 720;
    double updateRate = 30.0; // Hz
    bool fullscreen = false;
    
    // Network visualization settings
    bool showNeurons = true;
    bool showSynapses = true;
    bool showConnections = true;
    double neuronSize = 3.0;
    double synapseWidth = 1.0;
    
    // Activity visualization settings
    bool showSpikeTrails = true;
    bool showActivityHeatmap = true;
    bool showWeightMatrix = true;
    double spikeTrailAlpha = 0.7;
    double heatmapIntensity = 1.0;
    
    // Color schemes
    std::string neuronColorScheme = "default"; // "default", "grayscale", "temperature", "cyclic"
    std::string synapseColorScheme = "weight";   // "weight", "type", "strength"
    std::string backgroundColor = "#000000";
    
    // File output settings
    std::string outputPath = ".";
    double videoFps = 30.0;
    std::string videoCodec = "h264";
    
    // Performance settings
    bool enableAntialiasing = true;
    bool enableBloom = false;
    bool enableShadows = false;
    
    // User interaction
    bool allowInteraction = true;
    std::string interactionMode = "pan"; // "pan", "zoom", "rotate"
    
    // Data processing
    bool enableDataFiltering = true;
    size_t maxSpikeHistory = 10000;
    bool enableSmoothing = true;
    double smoothingFactor = 0.1;
};

// Visualization data structures
struct NetworkVisualizationData {
    std::vector<std::pair<NeuronId, double>> neuronPositions;
    std::vector<std::pair<SynapseId, std::pair<NeuronId, NeuronId>>> synapses;
    std::vector<double> neuronActivations;
    std::vector<double> neuronExcitability;
    std::vector<double> neuronResonance;
    std::map<NeuronId, std::vector<double>> neuronSpikeTimes;
    std::map<SynapseId, double> synapseWeights;
    std::map<SynapseId, std::vector<double>> synapseEligibilityTraces;
};

struct ActivityVisualizationData {
    std::vector<std::vector<double>> spikeRaster; // time x neuron
    std::vector<std::vector<double>> activityHeatmap; // time x space
    std::vector<std::vector<double>> localFieldPotentials; // neuron x time
    std::vector<double> globalStats;
};

struct PerformanceMetrics {
    double fps = 0.0;
    double renderTime = 0.0;
    double cpuUsage = 0.0;
    double memoryUsage = 0.0;
    std::chrono::system_clock::time_point lastUpdate;
};

// Main visualization interface
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Configuration
    void configure(const VisualizationConfig& config);
    VisualizationConfig& getConfig();
    const VisualizationConfig& getConfig() const;
    
    // Initialization and lifecycle
    bool initialize();
    void update();
    void render();
    void close();
    
    // Core visualization methods
    void visualizeNetwork(const Brain& brain, const NetworkVisualizationData& data);
    void visualizeActivity(const ActivityVisualizationData& data);
    void visualizeSpikes(const std::vector<SpikeEvent>& spikes, const Brain& brain);
    void visualizeWeights(const Brain& brain);
    void visualizeTimeSeries(const std::vector<std::vector<double>>& timeSeries);
    
    // Advanced visualization methods
    void visualize3D(const Brain& brain);
    void visualizeCustom(const std::function<void()>& renderCallback);
    void visualizeWeb(const std::string& htmlFile);
    
    // User interaction
    void handleMouseClick(int x, int y, bool leftButton);
    void handleMouseMotion(int x, int y, int dx, int dy);
    void handleKeyPress(int key, int scancode, bool pressed);
    void handleScroll(double xoffset, double yoffset);
    
    // Data management
    void setNetworkData(const NetworkVisualizationData& data);
    void setActivityData(const ActivityVisualizationData& data);
    void setExternalData(const std::string& key, const std::vector<double>& data);
    
    // Performance and control
    bool isActive() const;
    void setActive(bool active);
    void toggleActive();
    
    PerformanceMetrics getPerformanceMetrics() const;
    void resetPerformanceMetrics();
    
    // Visualization control
    void setVisualizationMode(VisualizationMode mode);
    VisualizationMode getVisualizationMode() const;
    void setBackend(VisualizationBackend backend);
    VisualizationBackend getBackend() const;
    
    // Data processing callbacks
    using DataProcessor = std::function<std::vector<double>(const std::vector<double>& data)>;
    void registerDataProcessor(const std::string& name, const DataProcessor& processor);
    void unregisterDataProcessor(const std::string& name);
    
    // Event callbacks
    using VisualizationEvent = std::function<void(const std::string& eventName, const std::map<std::string, std::string>& data)>;
    void addEventCallback(const std::string& eventType, const VisualizationEvent& callback);
    void removeEventCallback(const std::string& eventType);
    
    // Utility methods
    void logStatus() const;
    void printHelp() const;
    void toggleHelp();
    bool isHelpVisible() const;
    
    // Statistics and monitoring
    struct Statistics {
        size_t framesRendered = 0;
        size_t neuronsVisualized = 0;
        size_t synapsesVisualized = 0;
        size_t spikesVisualized = 0;
        double averageFrameTime = 0.0;
        double memoryUsage = 0.0;
    };
    
    Statistics getStatistics() const;
    void resetStatistics();
    
    // Color utilities
    static std::string interpolateColor(const std::string& color1, const std::string& color2, double t);
    static std::string getNeuronColor(double activation, const std::string& scheme = "default");
    static std::string getSynapseColor(double weight, const std::string& scheme = "weight");
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void initializeBackend();
    void cleanupBackend();
    void processEvents();
    void updateDataProcessors();
    void triggerEvents(const std::string& eventType, const std::map<std::string, std::string>& data);
};

// Utility classes for visualization
namespace vis {
    
    // Color utilities
    class Color {
    public:
        static std::string toHex(double r, double g, double b, double a = 1.0);
        static std::pair<double, double, double> fromHex(const std::string& hex);
        static std::string interpolate(const std::string& color1, const std::string& color2, double t);
        static double lightness(const std::string& color);
        static double saturation(const std::string& color);
        static std::string adjustBrightness(const std::string& color, double factor);
    };
    
    // Math utilities for visualization
    class Math {
    public:
        static std::vector<double> normalize(const std::vector<double>& data);
        static std::vector<double> clamp(const std::vector<double>& data, double min, double max);
        static std::vector<double> smooth(const std::vector<double>& data, double factor);
        static double gaussian(double x, double sigma = 1.0);
        static std::vector<std::pair<double, double>> computeGradient(const std::vector<std::vector<double>>& grid);
        static double distance(double x1, double y1, double x2, double y2);
        static double dotProduct(const std::vector<double>& a, const std::vector<double>& b);
        static std::vector<double> crossProduct(const std::vector<double>& a, const std::vector<double>& b);
    };
    
    // Data utilities for visualization
    class Data {
    public:
        static std::vector<std::vector<double>> computeHeatmap(const std::vector<std::vector<double>>& data, 
                                                               int width, int height);
        static std::vector<std::pair<double, double>> computeSpikeTimes(const std::vector<double>& spikeTrain,
                                                                        double dt);
        static std::vector<std::vector<double>> computeConnectivityMatrix(const Brain& brain);
        static std::vector<std::pair<double, double>> computeNetworkLayout(const Brain& brain);
        static std::vector<double> computeStats(const std::vector<double>& data);
        static size_t findNearestNeuron(const std::vector<double>& position, 
                                       const std::vector<std::pair<NeuronId, double>>& neuronPositions);
    };
    
    // Geometry utilities
    class Geometry {
    public:
        static std::vector<std::pair<double, double>> circlePoints(int segments, double radius = 1.0);
        static std::vector<std::pair<double, double>> rectanglePoints(double width, double height);
        static std::vector<std::pair<double, double>> trianglePoints(double size);
        static std::vector<std::pair<double, double>> linePoints(double x1, double y1, double x2, double y2, int segments);
        static double distance(const std::pair<double, double>& p1, const std::pair<double, double>& p2);
        static std::pair<double, double> add(const std::pair<double, double>& p1, const std::pair<double, double>& p2);
        static std::pair<double, double> subtract(const std::pair<double, double>& p1, const std::pair<double, double>& p2);
        static std::pair<double, double> multiply(const std::pair<double, double>& p, double scalar);
    };
}

} // namespace nlm