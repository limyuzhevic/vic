// VisualizationTools.hpp
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <mutex>
#include <thread>
#include <atomic>
#include <optional>
#include <variant>
#include <filesystem>

// Forward declarations
namespace nlm {
    class Brain;
    class Neuron;
    class Synapse;
    class NeuralPopulation;
    struct SpikeEvent;
    struct DelayedSpikeEvent;
    class VisualizationTools;
}

// Data structures for visualization
struct VisualizationData {
    using TimePoint = std::chrono::high_resolution_clock::time_point;
    
    // Network topology data
    struct NetworkTopology {
        std::vector<NeuronId> neuronIds;
        std::vector<SynapseId> synapseIds;
        std::vector<std::pair<NeuronId, SynapseId>> connections;
        std::unordered_map<NeuronId, std::vector<SynapseId>> incomingSynapses;
        std::unordered_map<NeuronId, std::vector<SynapseId>> outgoingSynapses;
    };
    
    // Neural activity data
    struct NeuralActivity {
        Timestamp timestamp;
        SimulationStep step;
        std::vector<float> neuronPotentials;
        std::vector<float> neuronFiringRates;
        std::vector<std::vector<float>> weightMatrix;
        std::vector<std::vector<float>> adjacencyMatrix;
    };
    
    // Spike data
    struct SpikeData {
        std::vector<SpikeEvent> spikes;
        std::vector<DelayedSpikeEvent> delayedSpikes;
        std::unordered_map<NeuronId, std::vector<Timestamp>> neuronSpikes;
    };
    
    // Population statistics
    struct PopulationStats {
        PopulationId populationId;
        NeuronType neuronType;
        size_t size;
        float meanPotential;
        float variancePotential;
        float meanFiringRate;
        size_t activeCount;
        size_t firingCount;
        std::vector<float> membranePotentials;
    };
    
    // Time series data
    using TimeSeriesData = std::vector<std::pair<Timestamp, float>>;
    
    // Visualization state
    TimePoint lastUpdate;
    NetworkTopology topology;
    NeuralActivity currentActivity;
    SpikeData spikeData;
    std::vector<PopulationStats> populationStats;
    std::unordered_map<PopulationId, TimeSeriesData> activityHistory;
};

// Output formats
enum class VisualizationFormat {
    HTML,
    JSON,
    CSV,
    VR_SCENE,
    WEBGL,
    THREEJS,
    GLTF,
    STL,
    OBJ
};

// Visualization types
enum class VisualizationType {
    NETWORK_GRAPH,
    NEURON_ACTIVITY,
    SPIKE_RASTER,
    WEIGHT_MATRIX,
    POPULATION_STATISTICS,
    3D_NEURAL_NETWORK,
    TIME_SERIES,
    INTERACTIVE_DASHBOARD,
    VR_SCENE
};

// Animation parameters
struct AnimationParams {
    double duration;           // seconds
    double frameRate;          // fps
    bool loop;                 // loop animation
    std::string easing;        // linear, ease-in, ease-out, bounce
    std::string interpolation; // linear, cubic, spline
    std::vector<Timestamp> keyframes;
    std::unordered_map<std::string, std::string> transitions;
};

// Report generation parameters
struct ReportParams {
    std::string title;
    std::string description;
    std::vector<std::string> sections;
    bool includeRawData;
    bool includeStatistics;
    std::string outputPath;
    std::vector<std::string> visualTypes;
    std::unordered_map<std::string, std::string> metadata;
};

// 3D scene configuration
struct SceneConfig {
    std::string name;
    float cameraDistance;
    float fieldOfView;
    std::string backgroundColor;
    std::string lightingMode;
    std::vector<std::string> renderModes;
    bool enableShadows;
    bool enableAntialiasing;
    std::unordered_map<std::string, std::string> customSettings;
};

// VR configuration
struct VRConfig {
    std::string trackingSystem;
    std::string displayMode;
    bool enablePhysics;
    bool enableInteractions;
    std::string interactionMode;
    std::vector<std::string> supportedDevices;
    std::unordered_map<std::string, std::string> customSettings;
};

// Web dashboard configuration
struct DashboardConfig {
    std::string title;
    std::vector<std::string> widgets;
    bool enableRealTimeUpdates;
    double updateInterval;
    std::string layout;           // grid, flex, absolute
    std::unordered_map<std::string, std::string> customSettings;
};

class VisualizationTools {
public:
    VisualizationTools();
    ~VisualizationTools();
    
    // Core functionality
    bool initialize(const std::string& outputDirectory = ".");
    void update(double deltaTime);
    void render();
    void close();
    
    // Configuration
    void setOutputDirectory(const std::string& path);
    void setUpdateRate(double hz);
    void setMaxHistorySize(size_t maxPoints);
    void setCompressionLevel(int level);
    
    // Visualization generation
    bool generateVisualization(VisualizationType type, const std::string& filename);
    bool generate3DVisualization(const Brain& brain, const std::string& filename, const SceneConfig& config);
    bool generateVRScene(const Brain& brain, const std::string& filename, const VRConfig& config);
    bool generateInteractiveDashboard(const std::string& filename, const DashboardConfig& config);
    
    // Animation support
    bool generateAnimation(const std::vector<Timestamp>& keyframes, const std::string& filename, const AnimationParams& params);
    bool generateTimeSeriesAnimation(const Brain& brain, const std::string& filename, const AnimationParams& params);
    
    // Report generation
    bool generateReport(const std::string& filename, const ReportParams& params);
    bool generateComprehensiveReport(const Brain& brain, const std::string& filename);
    
    // Multiple format support
    bool exportVisualization(VisualizationType type, const std::string& filename, VisualizationFormat format);
    bool exportToHTML(const Brain& brain, const std::string& filename);
    bool exportToJSON(const Brain& brain, const std::string& filename);
    bool exportToCSV(const Brain& brain, const std::string& filename);
    bool exportToVR(const Brain& brain, const std::string& filename);
    
    // Web-based visualization
    bool generateWebGLVisualization(const Brain& brain, const std::string& filename);
    bool generateThreeJSVisualization(const Brain& brain, const std::string& filename);
    bool generateGLTFSnapshot(const Brain& brain, const std::string& filename);
    
    // State management
    void setVisualizationData(const VisualizationData& data);
    VisualizationData getVisualizationData() const;
    void clearData();
    void resetStatistics();
    
    // File management
    std::vector<std::string> getAvailableFiles() const;
    bool exportData(const std::string& filename);
    bool importData(const std::string& filename);
    
    // Utility functions
    static std::string getVisualizationTypeName(VisualizationType type);
    static std::string getFormatExtension(VisualizationFormat format);
    static std::vector<VisualizationFormat> getSupportedFormats(VisualizationType type);
    
    // Status and configuration
    bool isInitialized() const;
    bool isRendering() const;
    double getCurrentFPS() const;
    size_t getDataSize() const;
    
    // Batch processing
    void startBatchMode(size_t batchSize = 10);
    void endBatchMode();
    bool isInBatchMode() const;
    
    // Progress tracking
    void setProgressCallback(std::function<void(double)> callback);
    void setErrorCallback(std::function<void(const std::string&)> callback);
    
    // Threading and async
    void enableMultiThreading(bool enable);
    void setThreadCount(size_t count);
    
    // Resource management
    void enableResourceOptimization(bool enable);
    void setMemoryLimit(size_t bytes);
    
    // Internal data processing
    void processNetworkTopology(const Brain& brain);
    void processNeuralActivity(const Brain& brain);
    void processSpikeData(const std::vector<SpikeEvent>& spikes);
    void processPopulationStats(const Brain& brain);
    
    // Data aggregation
    VisualizationData aggregateVisualizationData(const Brain& brain) const;
    bool validateVisualizationData(const VisualizationData& data) const;
    
    // Performance optimization
    void optimizeForSpeed();
    void optimizeForQuality();
    
    // Statistics and monitoring
    void recordVisualizationTime(VisualizationType type, double timeMs);
    double getAverageVisualizationTime(VisualizationType type) const;
    void printPerformanceStats() const;
    
    // Configuration validation
    static bool validateSceneConfig(const SceneConfig& config);
    static bool validateVRConfig(const VRConfig& config);
    static bool validateDashboardConfig(const DashboardConfig& config);
    
    // Static factory methods
    static std::unique_ptr<VisualizationTools> createInteractiveDashboard(const std::string& title);
    static std::unique_ptr<VisualizationTools> create3DVisualizer(const SceneConfig& config);
    static std::unique_ptr<VisualizationTools> createVRVisualizer(const VRConfig& config);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Private helper methods
    void initializeOutputDirectory();
    std::string generateUniqueFilename(const std::string& baseName, VisualizationFormat format) const;
    bool validateFilename(const std::string& filename) const;
    void updateProgress(double progress);
    void handleError(const std::string& error);
    void cleanupTemporaryFiles();
    void saveVisualizationData(const VisualizationData& data, const std::string& filename);
    VisualizationData loadVisualizationData(const std::string& filename);
    
    // Threading helpers
    void processBatchAsync(const std::vector<std::pair<VisualizationType, std::string>>& tasks);
    void renderFrame();
    
    // Export helpers
    bool exportToHTMLInternal(const Brain& brain, const std::string& filename, const DashboardConfig& config);
    bool exportToJSONInternal(const Brain& brain, const std::string& filename);
    bool exportToCSVInternal(const Brain& brain, const std::string& filename);
    bool exportToVRInternal(const Brain& brain, const std::string& filename);
    bool exportToThreeJSInternal(const Brain& brain, const std::string& filename);
    
    // Data serialization
    bool serializeVisualizationData(const VisualizationData& data, const std::string& filename) const;
    bool deserializeVisualizationData(VisualizationData& data, const std::string& filename) const;
    
    // Configuration loading
    bool loadConfiguration(const std::string& configFile);
    bool saveConfiguration(const std::string& configFile) const;
};