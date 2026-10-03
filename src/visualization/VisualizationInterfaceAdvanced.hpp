#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <chrono>
#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

// Forward declarations for visualization types
class VisualizationEngine;
class VisualizationScene;
class VisualizationRenderer;
class VisualizationData;
class VisualizationController;
class NetworkView;
class ActivityView;
class SpikeView;
class WeightView;
class View3D;
class TimeSeriesView;
class MemoryView;
class PredictionView;
class ConsoleRenderer;
class SVGRenderer;
class PDFRenderer;
class WebRenderer;
class ScientificRenderer;
class OpenGLBackend;
class SDLBackend;
class WebGLBackend;
class CairoBackend;
class NLMIntegration;

// Visualization configuration
struct VisualizationConfig {
    std::string backend;              // Rendering backend (OpenGL, SDL, WebGL, Cairo)
    int width;                        // Width in pixels
    int height;                       // Height in pixels
    double updateRate;                // Update rate in Hz
    bool autoRefresh;                 // Enable auto-refresh
    bool enable3D;                    // Enable 3D rendering
    std::string exportFormat;         // Export format (PNG, SVG, PDF)
    std::string outputPath;           // Output directory
    bool enablePlugins;               // Enable plugin architecture
    std::vector<std::string> plugins; // Plugin paths
    bool enablePerformanceMonitoring; // Enable performance metrics
    bool enableRealTime;              // Enable real-time updates
    int maxDataPoints;                // Maximum data points for streaming
    bool enableAnnotations;           // Enable annotations
    bool enableInteractivity;         // Enable user interaction
    std::string colorScheme;          // Color scheme (default, scientific, heatmap)
};

// Visualization data structure
struct VisualizationData {
    // Network data
    struct Network {
        std::vector<NeuronId> neurons;
        std::vector<std::pair<NeuronId, NeuronId>> connections;
        std::vector<SynapticWeight> weights;
        std::vector<NeuronType> neuronTypes;
        std::vector<RegionId> regionIds;
        std::map<NeuronId, std::string> neuronLabels;
        std::map<std::pair<NeuronId, NeuronId>, std::string> connectionLabels;
    } network;

    // Activity data
    struct Activity {
        std::vector<NeuronId> neuronIds;
        std::vector<FiringRate> firingRates;
        std::vector<std::vector<float>> populationActivity; // 2D grid
        std::vector<std::vector<float>> spatialActivity;   // Spatial activity
        std::vector<Timestamp> timestamps;
        std::vector<std::vector<float>> heatmaps; // Heatmap data
    } activity;

    // Spike data
    struct Spikes {
        std::vector<SpikeEvent> spikeEvents;
        std::vector<std::vector<Timestamp>> neuronSpikeHistory;
        std::map<NeuronId, std::vector<Timestamp>> neuronSpikes;
    } spikes;

    // Weight data
    struct Weights {
        std::vector<std::tuple<NeuronId, NeuronId, SynapticWeight>> weightMatrix;
        std::vector<std::vector<SynapticWeight>> adjacencyMatrix;
        std::vector<std::vector<float>> weightDistribution;
    } weights;

    // Time series data
    struct TimeSeries {
        std::vector<Timestamp> timestamps;
        std::vector<std::map<NeuronId, FiringRate>> neuronActivity;
        std::vector<std::map<RegionId, float>> regionActivity;
        std::vector<std::vector<float>> populationActivity;
    } timeSeries;

    // Memory data
    struct Memory {
        std::vector<std::string> memoryEntries;
        std::vector<std::vector<float>> memoryHeatmaps;
        std::vector<float> memoryTraces;
    } memory;

    // Prediction data
    struct Prediction {
        std::vector<std::vector<float>> confidenceMaps;
        std::vector<std::vector<float>> errorMaps;
        std::vector<std::vector<float>> predictionProbabilities;
        std::vector<Timestamp> predictionTimestamps;
    } prediction;

    // Simulation state
    struct Simulation {
        SimulationStep currentStep;
        Timestamp currentTime;
        double simulationTime;
        size_t totalSpikeCount;
        size_t activeNeuronCount;
    } simulation;

    // Performance metrics
    struct Performance {
        double frameTime;
        double renderTime;
        double updateTime;
        size_t vertexCount;
        size_t triangleCount;
        float memoryUsage;
        float cpuUsage;
    } performance;

    VisualizationData() : currentStep(0), currentTime(0.0), simulationTime(0.0),
                         totalSpikeCount(0), activeNeuronCount(0) {}
};

// Visualization event types
enum class VisualizationEvent {
    DataUpdate,
    ViewChange,
    FilterChange,
    ExportRequest,
    Interaction,
    Annotation,
    PerformanceUpdate,
    Error
};

// Event handler function type
using EventHandler = std::function<void(const VisualizationEvent&, const VisualizationData&)>;

// Visualization interface with advanced capabilities
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();

    // Initialize visualization with configuration
    bool initialize(const VisualizationConfig& config);

    // Update visualization data
    void update(const VisualizationData& data);

    // Render current state
    void render();

    // Close visualization
    void close();

    // Check if visualization is active
    bool isActive() const;

    // Set update rate
    void setUpdateRate(double hz);
    double getUpdateRate() const;

    // Configuration management
    void setConfig(const VisualizationConfig& config);
    const VisualizationConfig& getConfig() const;

    // Data management
    void setData(const std::string& type, const VisualizationData& data);
    const VisualizationData& getData(const std::string& type) const;

    // Visualization type management
    void addVisualizationType(const std::string& name, std::unique_ptr<NetworkView> view);
    NetworkView* getVisualizationType(const std::string& name);

    // Output management
    void addOutputRenderer(const std::string& name, std::unique_ptr<ConsoleRenderer> renderer);
    bool exportVisualization(const std::string& format, const std::string& filepath);
    void addEventHandler(const EventHandler& handler);

    // Real-time features
    void enableRealTimeUpdates(bool enabled);
    void setTimeWindow(Timestamp startTime, Timestamp endTime);
    void addFilter(const std::string& filterType, const std::string& parameter);
    void clearFilters();

    // Interaction
    void enableInteractivity(bool enabled);
    void highlightNeuron(NeuronId id, bool highlight);
    void highlightConnection(NeuronId source, NeuronId target, bool highlight);
    void addAnnotation(const std::string& text, double x, double y);

    // Multi-view support
    void addView(const std::string& name, std::unique_ptr<NetworkView> view);
    void removeView(const std::string& name);
    void synchronizeViews(bool synchronized);
    void setViewActive(const std::string& name, bool active);

    // Performance monitoring
    void enablePerformanceMonitoring(bool enabled);
    void setPerformanceMetricsTarget(const std::string& metrics);
    std::map<std::string, double> getPerformanceMetrics() const;

    // Integration
    void integrateWithBrain(Brain* brain);
    void integrateWithPluginSystem(class PluginSystem* pluginSystem);

    // Backend management
    void setBackend(std::unique_ptr<OpenGLBackend> backend);
    void setBackend(std::unique_ptr<SDLBackend> backend);
    void setBackend(std::unique_ptr<WebGLBackend> backend);
    void setBackend(std::unique_ptr<CairoBackend> backend);

    // Scene management
    void createScene(const std::string& name);
    void removeScene(const std::string& name);
    void setActiveScene(const std::string& name);
    void addActorToScene(const std::string& sceneName, class VisualizationActor* actor);
    void removeActorFromScene(const std::string& sceneName, class VisualizationActor* actor);

    // View management
    void setViewTransform(const std::string& viewName, const std::string& transform);
    void resetViewTransform(const std::string& viewName);

    // Export management
    void addExportOption(const std::string& format, const std::string& extension);
    void removeExportOption(const std::string& format);
    bool exportToFormat(const std::string& format, const std::string& filepath, const VisualizationData& data);

    // Plugin architecture
    void loadPlugin(const std::string& path);
    void unloadPlugin(const std::string& name);
    std::vector<std::string> getLoadedPlugins() const;
    std::vector<std::string> getAvailablePlugins() const;

    // Statistics and reporting
    std::string generateStatisticsReport() const;
    void exportStatistics(const std::string& filepath);
    std::map<std::string, double> collectMetrics() const;

    // Advanced features
    void enableMultiView(bool enabled);
    void enableComparison(bool enabled);
    void enableHistorical(bool enabled);
    void enableProjection(bool enabled);
    void enableStatisticsOverlay(bool enabled);

    // Collaboration and sharing
    void shareVisualization(const std::string& host, int port);
    void connectToVisualization(const std::string& host, int port);
    void broadcastState(const std::string& message);

    // Web integration
    void enableWebInterface(bool enabled);
    void setWebPort(int port);
    void serveStaticFiles(const std::string& path);
    void enableWebSocket(bool enabled);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm