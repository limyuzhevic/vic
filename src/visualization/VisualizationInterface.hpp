// Visualization Interface for NLM Brain State Monitoring
//
// This module provides comprehensive visualization capabilities for the NLM (Neural Learning Machine)
// brain simulation, enabling real-time monitoring and analysis of brain state, performance,
// and system metrics.
//
// Features:
// - Multi-mode visualization (2D graphs, 3D neural network, time series)
// - Real-time updating displays
// - Multiple output formats (console, web, file)
// - Interactive controls
// - Integration with all brain systems (memory, neuromodulation, prediction, cognition)
// - Performance metrics visualization
// - Historical data tracking
// - Comparative analysis
//

#ifndef NLM_VISUALIZATION_HPP
#define NLM_VISUALIZATION_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <mutex>
#include <atomic>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <functional>

// Forward declarations
namespace nlm {
    class Brain;
    class VisualizationData;
    class BrainStateData;
    class PerformanceMetrics;
}

using namespace std::chrono;

namespace nlm {

// Visualization modes
enum class VisualizationMode {
    None,
    TwoDGraph,           // Line graphs, scatter plots, heat maps
    ThreeDNeural,        // 3D neural network representation
    TimeSeries,          // Historical time series
    Comparative,         // Multiple simulation comparison
    RealTime,            // Live updating displays
    ActivityMap,         // Neural activity patterns
    MemoryView,          // Memory system visualization
    Neuromodulation,     // Neuromodulator effects
    Performance,         // Performance metrics
    SystemStatus        // Overall system status
};

// Output formats
enum class OutputFormat {
    Console,     // Text-based output
    Terminal,    // Terminal-friendly ASCII graphics
    HTML,        // Web-based visualization
    SVG,         // Scalable vector graphics
    PNG,         // Raster image
    JSON,        // Structured data export
    CSV          // Comma-separated values
};

// Interaction modes
enum class InteractionMode {
    None,        // No interaction
    Mouse,       // Mouse controls
    Keyboard,    // Keyboard shortcuts
    Touch,       // Touch interface
    Voice        // Voice commands
};

// Color schemes for visualization
struct ColorScheme {
    std::string background;
    std::string grid;
    std::string neurons;
    std::string synapses;
    std::string memory;
    std::string neuromodulation;
    std::string performance;
    std::string text;
    std::string highlight;
    std::string warning;
    
    ColorScheme() {
        background = "#2d2d2d";
        grid = "#555555";
        neurons = "#4a9eff";
        synapses = "#666666";
        memory = "#4caf50";
        neuromodulation = "#ff9800";
        performance = "#2196f3";
        text = "#ffffff";
        highlight = "#ffeb3b";
        warning = "#f44336";
    }
};

// Brain state data structure for visualization
struct BrainStateData {
    // Core brain information
    size_t neuronCount;
    size_t synapseCount;
    size_t regionCount;
    float simulationTime;
    size_t currentStep;
    
    // Activity data
    float averageFiringRate;
    float excitationInhibitionRatio;
    size_t activeNeuronCount;
    size_t firingNeuronCount;
    
    // Memory system data
    size_t workingMemoryTraces;
    size_t episodicMemoryEpisodes;
    size_t associativeMemoryAssociations;
    
    // Neuromodulation data
    float dopamineLevel;
    float curiosityLevel;
    float noveltyLevel;
    float predictionError;
    
    // Development data
    int developmentalStage;
    float plasticityRate;
    
    // Performance data
    double cpuUsage;
    double memoryUsage;
    double memoryAvailable;
    
    // Time series data
    std::vector<float> firingRateHistory;
    std::vector<float> rewardHistory;
    std::vector<size_t> spikeCountHistory;
    std::vector<time_point<system_clock>> timestamps;
    
    // Spatial data for 3D visualization
    struct NeuronInfo {
        size_t id;
        float x, y, z;
        float radius;
        float activation;
        float type;  // 0: sensory, 1: internal, 2: motor
    };
    
    std::vector<NeuronInfo> neurons;
    
    struct SynapseInfo {
        size_t sourceId;
        size_t targetId;
        float weight;
        float delay;
        bool isActive;
    };
    
    std::vector<SynapseInfo> synapses;
    
    // Regional data
    struct RegionInfo {
        size_t id;
        std::string name;
        float x, y, z;
        size_t neuronCount;
        size_t populationCount;
        float averageWeight;
    };
    
    std::vector<RegionInfo> regions;
};

// Performance metrics for visualization
struct PerformanceMetrics {
    double cpuUsage;
    double cpuUsagePeak;
    double memoryUsage;
    double memoryUsagePeak;
    double memoryAvailable;
    double simulationSpeed;  // steps per second
    double simulationEfficiency;  // performance per CPU cycle
    
    // Per-system performance
    struct SystemPerformance {
        std::string name;
        double processingTime;
        double memoryUsage;
        double efficiency;
        size_t operations;
    };
    
    std::vector<SystemPerformance> systemPerformance;
    
    // Timeline data
    std::vector<double> cpuHistory;
    std::vector<double> memoryHistory;
    std::vector<double> simulationSpeedHistory;
    std::vector<time_point<system_clock>> perfTimestamps;
};

// Visualization data structure
struct VisualizationData {
    VisualizationMode mode;
    OutputFormat outputFormat;
    InteractionMode interaction;
    ColorScheme colors;
    double updateInterval;  // seconds
    bool autoUpdate;
    bool showLegend;
    bool showGrid;
    bool showTimestamps;
    bool enableAnimations;
    
    // Window/area settings
    int width;
    int height;
    int xOffset;
    int yOffset;
    
    // Filtering and selection
    bool filterByType;
    std::vector<float> neuronTypeFilter;
    bool filterByActivity;
    float activityThreshold;
    bool filterByRegion;
    std::vector<size_t> regionFilter;
    
    // Time range selection
    time_point<system_clock> timeStart;
    time_point<system_clock> timeEnd;
    bool useTimeRange;
};

// Abstract base class for all visualizers
class VisualizationInterface {
public:
    virtual ~VisualizationInterface() = default;
    
    // Initialize the visualizer
    virtual bool initialize(const VisualizationData& config) = 0;
    
    // Update visualization with new data
    virtual void update(const BrainStateData& brainState,
                       const PerformanceMetrics& performance,
                       const VisualizationData& config) = 0;
    
    // Render the visualization
    virtual std::string render() = 0;
    
    // Save visualization to file
    virtual bool save(const std::string& filepath) = 0;
    
    // Handle user input
    virtual bool handleInput(const std::string& input) = 0;
    
    // Get visualization info
    virtual std::string getInfo() const = 0;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    
    // Control methods
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void pause() = 0;
    virtual void resume() = 0;
    virtual bool isRunning() const = 0;
    
    // Configuration methods
    virtual void setConfig(const VisualizationData& config) = 0;
    virtual VisualizationData getConfig() const = 0;
    
    // Data management
    virtual void setBrainData(const BrainStateData& data) = 0;
    virtual void setPerformanceData(const PerformanceMetrics& data) = 0;
    
protected:
    mutable std::mutex dataMutex_;
    bool running_;
    bool paused_;
};

// 2D Graph Visualizer - Line graphs, scatter plots, heat maps
class TwoDGraphVisualizer : public VisualizationInterface {
public:
    TwoDGraphVisualizer();
    ~TwoDGraphVisualizer() override;
    
    bool initialize(const VisualizationData& config) override;
    void update(const BrainStateData& brainState,
               const PerformanceMetrics& performance,
               const VisualizationData& config) override;
    std::string render() override;
    bool save(const std::string& filepath) override;
    bool handleInput(const std::string& input) override;
    
    std::string getInfo() const override;
    std::string getName() const override;
    std::string getDescription() const override;
    
    void start() override;
    void stop() override;
    void pause() override;
    void resume() override;
    bool isRunning() const override;
    
    void setConfig(const VisualizationData& config) override;
    VisualizationData getConfig() const override;
    
    void setBrainData(const BrainStateData& data) override;
    void setPerformanceData(const PerformanceMetrics& data) override;
    
private:
    struct GraphData {
        std::vector<std::pair<time_point<system_clock>, double>> data;
        std::string title;
        std::string xLabel;
        std::string yLabel;
        std::string color;
        bool showPoints;
        bool connectLines;
        double lineWidth;
    };
    
    std::vector<GraphData> graphs_;
    bool running_;
    bool paused_;
    VisualizationData config_;
    BrainStateData brainState_;
    PerformanceMetrics performance_;
    
    // Graph rendering helpers
    std::string renderGraph(const GraphData& graph, int width, int height);
    std::string createAsciiGraph(const GraphData& graph, int width, int height);
    std::string createHtmlGraph(const GraphData& graph);
    std::string createSvgGraph(const GraphData& graph);
    
    // Data processing helpers
    std::vector<std::pair<time_point<system_clock>, double>> extractData(const std::string& dataType) const;
    std::vector<std::pair<std::string, std::string>> getAvailableDataTypes() const;
};

// 3D Neural Network Visualizer
class ThreeDNeuralVisualizer : public VisualizationInterface {
public:
    ThreeDNeuralVisualizer();
    ~ThreeDNeuralVisualizer() override;
    
    bool initialize(const VisualizationData& config) override;
    void update(const BrainStateData& brainState,
               const PerformanceMetrics& performance,
               const VisualizationData& config) override;
    std::string render() override;
    bool save(const std::string& filepath) override;
    bool handleInput(const std::string& input) override;
    
    std::string getInfo() const override;
    std::string getName() const override;
    std::string getDescription() const override;
    
    void start() override;
    void stop() override;
    void pause() override;
    void resume() override;
    bool isRunning() const override;
    
    void setConfig(const VisualizationData& config) override;
    VisualizationData getConfig() const override;
    
    void setBrainData(const BrainStateData& data) override;
    void setPerformanceData(const PerformanceMetrics& data) override;
    
    // 3D specific methods
    void generateNeuralPositions(const BrainStateData& brainState);
    void generateSynapseConnections(const BrainStateData& brainState);
    void updateNeuralActivity(const BrainStateData& brainState);
    std::string createHtml3DVisualization() const;
    std::string createSvg3DVisualization() const;
    
private:
    struct NeuralNode {
        size_t id;
        float x, y, z;
        float radius;
        float activation;
        int type;  // 0: sensory, 1: internal, 2: motor
        float color[3];  // RGB values 0-1
    };
    
    struct NeuralConnection {
        size_t sourceId;
        size_t targetId;
        float weight;
        float transparency;
        float color[3];  // RGB values 0-1
    };
    
    struct Region {
        size_t id;
        std::string name;
        float x, y, z;
        float radius;
        float color[3];  // RGB values 0-1
    };
    
    std::vector<NeuralNode> nodes_;
    std::vector<NeuralConnection> connections_;
    std::vector<Region> regions_;
    
    bool running_;
    bool paused_;
    VisualizationData config_;
    BrainStateData brainState_;
    PerformanceMetrics performance_;
    
    // Camera and view settings
    struct Camera {
        float x, y, z;      // Camera position
        float targetX, targetY, targetZ;  // Look-at point
        float upX, upY, upZ;  // Up vector
        float fov;          // Field of view
        float aspectRatio;  // Aspect ratio
        float nearPlane;
        float farPlane;
        float distance;     // Distance from target
        float azimuth;      // Rotation around Y axis
        float elevation;    // Rotation around X axis
    };
    
    Camera camera_;
    
    // Interaction state
    bool mouseLeftDown_;
    bool mouseRightDown_;
    bool mouseMiddleDown_;
    float lastMouseX_;
    float lastMouseY_;
    float rotationX_;
    float rotationY_;
    
    // Animation
    float animationTime_;
    bool animate_;  // Whether to animate neural activity
    
    // Helper methods
    void updateCamera(float deltaX, float deltaY, float deltaZ);
    void rotateCamera(float azimuth, float elevation);
    void zoomCamera(float factor);
    std::string createHtml3DScene() const;
    std::string createWebGLShaders() const;
    std::string createThreeJSText() const;
};

// Time Series Visualizer
class TimeSeriesVisualizer : public VisualizationInterface {
public:
    TimeSeriesVisualizer();
    ~TimeSeriesVisualizer() override;
    
    bool initialize(const VisualizationData& config) override;
    void update(const BrainStateData& brainState,
               const PerformanceMetrics& performance,
               const VisualizationData& config) override;
    std::string render() override;
    bool save(const std::string& filepath) override;
    bool handleInput(const std::string& input) override;
    
    std::string getInfo() const override;
    std::string getName() const override;
    std::string getDescription() const override;
    
    void start() override;
    void stop() override;
    void pause() override;
    void resume() override;
    bool isRunning() const override;
    
    void setConfig(const VisualizationData& config) override;
    VisualizationData getConfig() const override;
    
    void setBrainData(const BrainStateData& data) override;
    void setPerformanceData(const PerformanceMetrics& data) override;
    
    // Time series specific methods
    void addDataPoint(const std::string& seriesName, double value,
                     time_point<system_clock> timestamp);
    void setTimeRange(const time_point<system_clock>& start,
                     const time_point<system_clock>& end);
    std::vector<std::pair<time_point<system_clock>, double>> getSeriesData(const std::string& seriesName) const;
    
    std::string createMultiSeriesGraph() const;
    std::string createComparisonGraph() const;
    std::string createHeatMap() const;
    
private:
    struct TimeSeries {
        std::string name;
        std::vector<std::pair<time_point<system_clock>, double>> data;
        std::string color;
        bool showPoints;
        bool connectLines;
        std::string yAxisLabel;
        double yMin, yMax;  // Auto-scale ranges
    };
    
    std::vector<TimeSeries> series_;
    bool running_;
    bool paused_;
    VisualizationData config_;
    BrainStateData brainState_;
    PerformanceMetrics performance_;
    
    // Time range
    time_point<system_clock> timeStart_;
    time_point<system_clock> timeEnd_;
    bool useTimeRange_;
    
    // Animation
    time_point<system_clock> lastUpdateTime_;
    float updateInterval_;
};

// Performance Monitor Visualizer
class PerformanceMonitorVisualizer : public VisualizationInterface {
public:
    PerformanceMonitorVisualizer();
    ~PerformanceMonitorVisualizer() override;
    
    bool initialize(const VisualizationData& config) override;
    void update(const BrainStateData& brainState,
               const PerformanceMetrics& performance,
               const VisualizationData& config) override;
    std::string render() override;
    bool save(const std::string& filepath) override;
    bool handleInput(const std::string& input) override;
    
    std::string getInfo() const override;
    std::string getName() const override;
    std::string getDescription() const override;
    
    void start() override;
    void stop() override;
    void pause() override;
    void resume() override;
    bool isRunning() const override;
    
    void setConfig(const VisualizationData& config) override;
    VisualizationData getConfig() const override;
    
    void setBrainData(const BrainStateData& data) override;
    void setPerformanceData(const PerformanceMetrics& data) override;
    
    // Performance monitoring specific methods
    void updateSystemMetrics(const PerformanceMetrics& metrics);
    void updateBrainPerformance(const BrainStateData& brainState);
    std::string createResourceMonitor() const;
    std::string createPerformanceGauge() const;
    std::string createSystemStatus() const;
    
private:
    struct PerformanceIndicator {
        std::string name;
        double value;
        double maxValue;
        double minValue;
        std::string color;
        std::string unit;
        std::string description;
        bool isWarning;
    };
    
    std::vector<PerformanceIndicator> indicators_;
    bool running_;
    bool paused_;
    VisualizationData config_;
    BrainStateData brainState_;
    PerformanceMetrics performance_;
    
    // Historical data
    std::vector<double> cpuHistory_;
    std::vector<double> memoryHistory_;
    std::vector<time_point<system_clock>> timeHistory_;
    
    // Status indicators
    enum class SystemStatus {
        Good,
        Warning,
        Critical
    };
    
    SystemStatus getSystemStatus() const;
    std::string getStatusColor(SystemStatus status) const;
};

// Composite Visualization - Multiple visualization modes
class CompositeVisualizer : public VisualizationInterface {
public:
    CompositeVisualizer();
    ~CompositeVisualizer() override;
    
    bool initialize(const VisualizationData& config) override;
    void update(const BrainStateData& brainState,
               const PerformanceMetrics& performance,
               const VisualizationData& config) override;
    std::string render() override;
    bool save(const std::string& filepath) override;
    bool handleInput(const std::string& input) override;
    
    std::string getInfo() const override;
    std::string getName() const override;
    std::string getDescription() const override;
    
    void start() override;
    void stop() override;
    void pause() override;
    void resume() override;
    bool isRunning() const override;
    
    void setConfig(const VisualizationData& config) override;
    VisualizationData getConfig() const override;
    
    void setBrainData(const BrainStateData& data) override;
    void setPerformanceData(const PerformanceMetrics& data) override;
    
    // Composite visualization specific methods
    void addVisualizer(std::unique_ptr<VisualizationInterface> visualizer);
    void removeVisualizer(const std::string& name);
    void setActiveVisualizers(const std::vector<std::string>& names);
    std::vector<std::string> getAvailableVisualizers() const;
    
    std::string createLayout() const;
    
private:
    std::map<std::string, std::unique_ptr<VisualizationInterface>> visualizers_;
    std::vector<std::string> activeVisualizerNames_;
    bool running_;
    bool paused_;
    VisualizationData config_;
    BrainStateData brainState_;
    PerformanceMetrics performance_;
};

// Main visualization manager
class VisualizationManager {
public:
    VisualizationManager();
    ~VisualizationManager();
    
    // Manager control
    bool initialize();
    void shutdown();
    bool isInitialized() const;
    
    // Visualization control
    bool createVisualizer(const std::string& name, const std::string& type);
    bool removeVisualizer(const std::string& name);
    bool activateVisualizer(const std::string& name);
    bool deactivateVisualizer(const std::string& name);
    bool activateVisualizerGroup(const std::vector<std::string>& names);
    
    // Update and render
    void updateAll(const BrainStateData& brainState,
                  const PerformanceMetrics& performance,
                  const VisualizationData& config);
    std::string renderActiveVisualizers();
    
    // Configuration
    bool setGlobalConfig(const VisualizationData& config);
    VisualizationData getGlobalConfig() const;
    bool setVisualizerConfig(const std::string& name, const VisualizationData& config);
    VisualizationData getVisualizerConfig(const std::string& name) const;
    
    // Data management
    void setBrainData(const BrainStateData& data);
    void setPerformanceData(const PerformanceMetrics& data);
    
    // Export and save
    bool exportVisualization(const std::string& filepath);
    std::string generateReport() const;
    bool generateHtmlPage(const std::string& filepath);
    
    // Status and info
    std::vector<std::string> getActiveVisualizerNames() const;
    std::string getVisualizerInfo(const std::string& name) const;
    std::string getManagerStatus() const;
    
    // Visualization creation helpers
    static std::unique_ptr<VisualizationInterface> createVisualizer(const std::string& type);
    static std::vector<std::string> getAvailableVisualizerTypes();
    
    // Global configuration for all visualizers
    static void setGlobalVisualizationConfig(const VisualizationData& config);
    static VisualizationData getGlobalVisualizationConfig();
    
private:
    std::map<std::string, std::unique_ptr<VisualizationInterface>> visualizers_;
    std::map<std::string, VisualizationData> visualizerConfigs_;
    VisualizationData globalConfig_;
    
    BrainStateData brainState_;
    PerformanceMetrics performance_;
    
    bool initialized_;
    bool running_;
    bool paused_;
    
    mutable std::mutex dataMutex_;
};

// Global visualization manager instance
inline VisualizationManager globalVisualizationManager;

// Convenience functions
inline bool initializeVisualization() {
    return globalVisualizationManager.initialize();
}

inline void shutdownVisualization() {
    globalVisualizationManager.shutdown();
}

inline bool createVisualizer(const std::string& name, const std::string& type) {
    return globalVisualizationManager.createVisualizer(name, type);
}

inline bool activateVisualizer(const std::string& name) {
    return globalVisualizationManager.activateVisualizer(name);
}

inline std::string renderVisualization() {
    return globalVisualizationManager.renderActiveVisualizers();
}

inline void updateVisualization(const BrainStateData& brainState,
                                const PerformanceMetrics& performance,
                                const VisualizationData& config = VisualizationData()) {
    globalVisualizationManager.updateAll(brainState, performance, config);
}

inline bool exportVisualization(const std::string& filepath) {
    return globalVisualizationManager.exportVisualization(filepath);
}

} // namespace nlm

#endif // NLM_VISUALIZATION_HPP
