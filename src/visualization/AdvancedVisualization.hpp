#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <mutex>
#include <unordered_map>
#include <functional>
#include <nlohmann/json.hpp>

namespace nlm {

// Visualization configuration
struct VisualizationConfig {
    std::string backend = "opengl"; // opengl, vulkan, software, webgl
    bool enableRealTime = true;
    float refreshRate = 60.0f; // Hz
    std::string renderMode = "wireframe"; // wireframe, solid, points
    bool enableLighting = true;
    bool enableShadows = false;
    std::string colorScheme = "default"; // default, thermal, spectrum, grayscale
    
    // 3D visualization settings
    float cameraDistance = 10.0f;
    float cameraPitch = 0.0f;
    float cameraYaw = 0.0f;
    float fieldOfView = 45.0f;
    
    // Performance settings
    bool enableLevelOfDetail = true;
    float lodThreshold = 0.5f;
    bool enableCulling = true;
    
    // Interaction settings
    bool enableMouseInteraction = true;
    bool enableKeyboardShortcuts = true;
    bool enableTouchGestures = false;
};

// Visualization data structures
struct NeuronVisualization {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float radius = 0.1f;
    float activation = 0.0f;
    std::string color = "#0000FF";
    bool isActive = false;
    bool isHighlighted = false;
    std::vector<std::pair<float, float>> trajectory; // For animation
};

struct SynapseVisualization {
    float x1 = 0.0f;
    float y1 = 0.0f;
    float z1 = 0.0f;
    float x2 = 0.0f;
    float y2 = 0.0f;
    float z2 = 0.0f;
    float strength = 0.0f;
    std::string color = "#808080";
    bool isActive = false;
    float animationPhase = 0.0f;
};

struct RegionVisualization {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float width = 1.0f;
    float height = 1.0f;
    float depth = 1.0f;
    std::string color = "#808080";
    float transparency = 0.5f;
    std::string label = "";
    bool showLabel = true;
};

// Visualization modes
enum class VisualizationMode {
    3D,           // 3D neural network view
    2DProjection, // 2D projection of 3D network
    Topological,  // Topological map view
    ActivityHeatmap, // Activity heatmap overlay
    ConnectionDensity, // Connection density visualization
    Timeseries,   // Time series of activity
    Statistical,  // Statistical plots and summaries
    Custom       // User-defined visualization
};

// Export formats
enum class ExportFormat {
    PNG,
    JPEG,
    SVG,
    GLTF,
    OBJ,
    JSON,
    CSV,
    Video,
    HTML
};

// Animation configuration
struct AnimationConfig {
    float frameRate = 30.0f; // fps
    float duration = 10.0f; // seconds
    std::string easing = "linear"; // linear, ease_in, ease_out, ease_in_out
    bool loop = false;
    bool exportAsVideo = true;
    std::string videoCodec = "h264";
    float quality = 0.8f;
};

// Interactive visualization
struct InteractiveFeature {
    std::string id;
    std::string type; // "zoom", "pan", "rotate", "filter", "highlight"
    std::function<void(const InteractiveFeature&)> callback;
    bool enabled = true;
    std::unordered_map<std::string, std::string> parameters;
};

// Statistical visualization data
struct StatisticalData {
    std::string name;
    std::vector<float> values;
    std::vector<float> timestamps;
    std::string unit;
    std::string aggregation = "mean"; // mean, median, std, min, max
};

// Visualization statistics
struct VisualizationStats {
    double updateTime = 0.0; // milliseconds
    double renderTime = 0.0;
    float fps = 60.0f;
    size_t trianglesRendered = 0;
    size_t verticesRendered = 0;
    size_t drawCalls = 0;
    float memoryUsage = 0.0f; // MB
    std::chrono::steady_clock::time_point lastUpdate;
};

// Base visualization interface
class VisualizationInterface {
public:
    virtual ~VisualizationInterface() = default;
    
    virtual bool initialize(const VisualizationConfig& config) = 0;
    virtual void shutdown() = 0;
    
    virtual void updateData(const std::vector<NeuronVisualization>& neurons,
                           const std::vector<SynapseVisualization>& synapses,
                           const std::vector<RegionVisualization>& regions) = 0;
    
    virtual void render() = 0;
    virtual void resize(int width, int height) = 0;
    
    virtual bool saveImage(const std::string& filename, ExportFormat format = ExportFormat::PNG) = 0;
    virtual bool exportAnimation(const std::string& filename, const AnimationConfig& config) = 0;
    
    virtual void setVisualizationMode(VisualizationMode mode) = 0;
    virtual VisualizationMode getVisualizationMode() const = 0;
    
    virtual void addInteractiveFeature(const InteractiveFeature& feature) = 0;
    virtual void removeInteractiveFeature(const std::string& id) = 0;
    virtual void clearInteractiveFeatures() = 0;
    
    virtual void addStatisticalData(const StatisticalData& data) = 0;
    virtual void removeStatisticalData(const std::string& name) = 0;
    virtual void clearStatisticalData() = 0;
    
    virtual VisualizationStats getStats() const = 0;
    virtual std::string getVersion() const = 0;
    
    virtual bool isRunning() const = 0;
    virtual void pause() = 0;
    virtual void resume() = 0;
};

// Advanced visualization manager
class AdvancedVisualization {
public:
    AdvancedVisualization();
    ~AdvancedVisualization();
    
    // Configuration
    void setConfig(const VisualizationConfig& config);
    VisualizationConfig getConfig() const;
    
    // Data management
    void updateNeuralData(const std::vector<NeuronVisualization>& neurons,
                        const std::vector<SynapseVisualization>& synapses,
                        const std::vector<RegionVisualization>& regions);
    
    // Visualization control
    void setVisualizationMode(VisualizationMode mode);
    void toggleVisualizationMode();
    
    // Export functionality
    bool saveCurrentView(const std::string& filename, ExportFormat format = ExportFormat::PNG);
    bool exportAnimation(const std::string& filename, const AnimationConfig& config);
    
    // Interactive features
    void addInteractiveFeature(const InteractiveFeature& feature);
    void removeInteractiveFeature(const std::string& id);
    std::vector<InteractiveFeature> getInteractiveFeatures() const;
    
    // Statistical visualization
    void addStatisticalData(const StatisticalData& data);
    void removeStatisticalData(const std::string& name);
    std::vector<StatisticalData> getStatisticalData() const;
    
    // Performance monitoring
    VisualizationStats getStats() const;
    void enablePerformanceMonitoring(bool enable);
    
    // Real-time control
    void startRealTimeUpdates();
    void stopRealTimeUpdates();
    bool isRealTimeUpdatesEnabled() const;
    
    // Configuration presets
    void loadPreset(const std::string& presetName);
    void savePreset(const std::string& presetName, const VisualizationConfig& config);
    std::vector<std::string> getPresetNames() const;
    
    // Callback system
    void addUpdateCallback(std::function<void(const VisualizationStats&)> callback);
    void removeUpdateCallback(std::function<void(const VisualizationStats&)> callback);
    
    // Backend management
    std::vector<std::string> getAvailableBackends() const;
    bool setBackend(const std::string& backend);
    std::string getCurrentBackend() const;
    
    // Export formats
    std::vector<ExportFormat> getAvailableExportFormats() const;
    bool isExportFormatAvailable(ExportFormat format) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Real-time visualization streaming
class VisualizationStreamer {
public:
    VisualizationStreamer();
    ~VisualizationStreamer();
    
    // Stream configuration
    void setStreamConfig(const VisualizationConfig& config, const AnimationConfig& animConfig);
    
    // Data streaming
    void feedDataChunk(const std::vector<NeuronVisualization>& neurons,
                      const std::vector<SynapseVisualization>& synapses,
                      const std::vector<RegionVisualization>& regions,
                      float timestamp);
    
    // Stream control
    void startStreaming();
    void stopStreaming();
    bool isStreaming() const;
    
    // Quality control
    void setQualityLevel(float level); // 0.0 to 1.0
    void setFrameRate(float fps);
    
    // Export streaming
    bool startExportStream(const std::string& filename, const AnimationConfig& config);
    void stopExportStream();
    bool isExporting() const;
    
    // Statistics
    struct StreamStats {
        size_t framesProcessed = 0;
        double averageFrameTime = 0.0;
        float currentFps = 0.0f;
        size_t totalDataPoints = 0;
        std::chrono::steady_clock::time_point sessionStart;
    };
    
    StreamStats getStats() const;
    
    // Callbacks
    void addFrameCallback(std::function<void(const std::vector<NeuronVisualization>&,
                                            const std::vector<SynapseVisualization>&,
                                            const std::vector<RegionVisualization>&)> callback);
    void removeFrameCallback(std::function<void(const std::vector<NeuronVisualization>&,
                                               const std::vector<SynapseVisualization>&,
                                               const std::vector<RegionVisualization>&)> callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Visualization data processor
class VisualizationDataProcessor {
public:
    VisualizationDataProcessor();
    ~VisualizationDataProcessor();
    
    // Data preprocessing
    std::vector<NeuronVisualization> processNeurons(const std::vector<NeuronVisualization>& neurons);
    std::vector<SynapseVisualization> processSynapses(const std::vector<SynapseVisualization>& synapses);
    std::vector<RegionVisualization> processRegions(const std::vector<RegionVisualization>& regions);
    
    // Data enhancement
    void enableDataEnhancement(bool enable);
    void setEnhancementLevel(float level); // 0.0 to 1.0
    
    // Filtering
    void addFilter(std::function<bool(const NeuronVisualization&)> filter);
    void clearFilters();
    
    // Transformation
    void addTransformer(std::function<NeuronVisualization(const NeuronVisualization&)> transformer);
    void clearTransformers();
    
    // Statistics
    struct ProcessingStats {
        size_t totalNeuronsProcessed = 0;
        size_t totalSynapsesProcessed = 0;
        size_t totalRegionsProcessed = 0;
        double processingTime = 0.0;
        double enhancementTime = 0.0;
    };
    
    ProcessingStats getStats() const;
    
    // Configuration
    void setMaxNeuronsPerFrame(size_t maxNeurons);
    void setMaxSynapsesPerFrame(size_t maxSynapses);
    void setEnableLOD(bool enable);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Visualization exporter
class VisualizationExporter {
public:
    VisualizationExporter();
    ~VisualizationExporter();
    
    // Export formats
    bool exportToPNG(const std::string& filename, const std::vector<NeuronVisualization>& neurons,
                    const std::vector<SynapseVisualization>& synapses,
                    const std::vector<RegionVisualization>& regions,
                    const VisualizationConfig& config);
    
    bool exportToGLTF(const std::string& filename, const std::vector<NeuronVisualization>& neurons,
                     const std::vector<SynapseVisualization>& synapses,
                     const std::vector<RegionVisualization>& regions);
    
    bool exportToOBJ(const std::string& filename, const std::vector<NeuronVisualization>& neurons,
                    const std::vector<SynapseVisualization>& synapses,
                    const std::vector<RegionVisualization>& regions);
    
    bool exportToJSON(const std::string& filename, const std::vector<NeuronVisualization>& neurons,
                     const std::vector<SynapseVisualization>& synapses,
                     const std::vector<RegionVisualization>& regions);
    
    bool exportToCSV(const std::string& filename, const std::vector<NeuronVisualization>& neurons,
                    const std::vector<SynapseVisualization>& synapses,
                    const std::vector<RegionVisualization>& regions);
    
    bool exportToVideo(const std::string& filename, const VisualizationConfig& config,
                      const AnimationConfig& animConfig);
    
    bool exportToHTML(const std::string& filename, const std::vector<NeuronVisualization>& neurons,
                     const std::vector<SynapseVisualization>& synapses,
                     const std::vector<RegionVisualization>& regions);
    
    // Export configuration
    void setExportPath(const std::string& path);
    void setCompressionLevel(int level);
    
    // Format availability
    bool isFormatAvailable(ExportFormat format) const;
    std::vector<ExportFormat> getAvailableFormats() const;
    
    // Quality control
    void setQuality(ExportFormat format, float quality);
    float getQuality(ExportFormat format) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
