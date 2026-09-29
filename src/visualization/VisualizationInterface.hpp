#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>
#include <chrono>

namespace nlm {

// Visualization interface for debugging and analysis
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize visualization
    bool initialize();
    
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
    
    // Network visualization
    void visualizeNetwork(const class Brain& brain);
    
    // Activity visualization
    void visualizeActivity(const class Brain& brain);
    
    // Spike raster plot
    void visualizeSpikes(const std::vector<class SpikeEvent>& spikes);
    
    // Weight matrix visualization
    void visualizeWeights(const class Brain& brain);
    
    // Enhanced visualization methods
    
    // Multi-panel visualization with synchronized views
    void visualizeMultiPanel(const class Brain& brain, bool enableSync = true);
    
    // Time-series visualization for metrics
    void visualizeTimeSeries(const std::vector<double>& data, const std::string& title = "Data");
    
    // Statistical analysis visualization
    void visualizeStatistics(const class Brain& brain);
    
    // Connectivity analysis visualization
    void visualizeConnectivityAnalysis(const class Brain& brain);
    
    // Learning progress visualization
    void visualizeLearningProgress(const std::vector<double>& rewards, 
                                  const std::vector<double>& curiosityLevels);
    
    // Development stage visualization
    void visualizeDevelopmentStages(const class Brain& brain);
    
    // Real-time monitoring dashboard
    void visualizeDashboard(const class Brain& brain, const class AgentBrain& agent);
    
    // Export visualization data
    bool exportVisualization(const std::string& filename, 
                           const std::string& format = "png");
    
    // Interactive mode
    void enableInteractiveMode(bool enabled);
    bool isInteractiveMode() const;
    
    // Save visualization state
    void saveState(const std::string& stateFile);
    
    // Load visualization state
    bool loadState(const std::string& stateFile);
    
    // Comparison visualization
    void visualizeComparison(const class Brain& brain1, const class Brain& brain2);
    
    // Performance metrics visualization
    void visualizePerformance(const std::map<std::string, double>& metrics);
    
    // Neural population activity visualization
    void visualizePopulationActivity(const class Brain& brain, 
                                   const std::vector<class PopulationId>& populations);
    
    // Synaptic dynamics visualization
    void visualizeSynapticDynamics(const class Brain& brain, 
                                  double timeWindow = 1.0);
    
    // Memory system visualization
    void visualizeMemorySystems(const class Brain& brain);
    
    // Neuromodulation visualization
    void visualizeNeuromodulation(const class Brain& brain);
    
    // Prediction system visualization
    void visualizePredictionSystem(const class Brain& brain);
    
    // Cognitive functions visualization
    void visualizeCognitiveFunctions(const class Brain& brain);
    
    // Environment interaction visualization
    void visualizeEnvironmentInteraction(const class SimpleWorld& world);
    
    // Agent behavior visualization
    void visualizeAgentBehavior(const class AgentBrain& agent, 
                              const class SimpleWorld& world);
    
    // Phase progression visualization
    void visualizePhaseProgression(int currentPhase, int totalPhases);
    
    // Quality metrics visualization
    void visualizeQualityMetrics(const std::map<std::string, double>& qualityMetrics);
    
    // Error visualization
    void visualizeErrors(const std::vector<std::string>& errorMessages);
    
    // Performance profiling visualization
    void visualizePerformanceProfile(const std::string& profileFile);
    
    // Memory usage visualization
    void visualizeMemoryUsage(const class Brain& brain);
    
    // Network topology visualization
    void visualizeNetworkTopology(const class Brain& brain);
    
    // Dynamics visualization
    void visualizeDynamics(const class Brain& brain, 
                         double duration = 1.0,
                         int resolution = 100);
    
    // Chemical visualization (for neuromodulators)
    void visualizeChemicalDiffusion(const class Brain& brain);
    
    // 3D visualization support
    void enable3DMode(bool enabled);
    bool is3DMode() const;
    
    // Custom visualization hooks
    using VisualizationHook = std::function<void(const class Brain& brain)>;
    void addVisualizationHook(VisualizationHook hook);
    
    // Live data streaming visualization
    void enableLiveStreaming(bool enabled, const std::string& host = "localhost", int port = 8080);
    
    // Multi-window support
    void enableMultiWindow(bool enabled, int numWindows = 2);
    
    // GPU acceleration support
    bool isGPUSupported() const;
    void enableGPURendering(bool enabled);
    
    // Export options
    struct ExportOptions {
        bool includeLabels = true;
        bool includeLegend = true;
        bool includeTimestamp = true;
        std::string colormap = "viridis";
        double quality = 1.0; // 0.0 to 1.0
    };
    
    // Visualization configuration
    struct VisualizationConfig {
        double defaultUpdateRate = 30.0;
        bool autoScale = true;
        double refreshInterval = 0.1; // seconds
        bool showStatistics = true;
        bool showLegend = true;
        std::string defaultStyle = "minimal";
    };
    
    // Set configuration
    void setConfiguration(const VisualizationConfig& config);
    VisualizationConfig getConfiguration() const;
    
    // Get available visualization backends
    std::vector<std::string> getAvailableBackends() const;
    
    // Set visualization backend
    bool setBackend(const std::string& backend);
    
    // Get current backend
    std::string getCurrentBackend() const;
    
    // Performance monitoring integration
    void integratePerformanceMonitor(class PerformanceMonitor& monitor);
    
    // Real-time data visualization
    void updateRealTimeData(const std::string& source, const std::vector<double>& data);
    
    // Statistical overlay visualization
    void enableStatisticalOverlay(bool enabled);
    bool isStatisticalOverlayEnabled() const;
    
    // Error overlay visualization
    void enableErrorOverlay(bool enabled);
    bool isErrorOverlayEnabled() const;
    
    // Annotation support
    void addAnnotation(const std::string& text, double x, double y);
    void removeAnnotation(size_t index);
    void clearAnnotations();
    
    // Mouse interaction callbacks
    using MouseCallback = std::function<void(double x, double y, bool leftButton)>;
    void setMouseCallback(MouseCallback callback);
    
    // Keyboard interaction callbacks
    using KeyboardCallback = std::function<void(int key, bool pressed)>;
    void setKeyboardCallback(KeyboardCallback callback);
    
    // Visualization state
    struct VisualizationState {
        bool active = false;
        std::string currentBackend;
        VisualizationConfig config;
        std::vector<std::pair<std::string, double>> recentMetrics;
        std::vector<std::string> annotations;
        double lastUpdateTime = 0.0;
        size_t totalUpdates = 0;
        double averageFrameTime = 0.0;
    };
    
    // Get current state
    VisualizationState getState() const;
    
    // Reset visualization
    void reset();
    
    // Check for changes
    bool hasChanged() const;
    
    // Force update
    void forceUpdate();
    
    // Visualization statistics
    struct VisualizationStats {
        double fps = 0.0;
        double updateTime = 0.0;
        size_t totalDrawCalls = 0;
        size_t memoryUsage = 0;
        double cpuUsage = 0.0;
        double gpuUsage = 0.0;
        std::chrono::system_clock::time_point lastFrameTime;
        
        double getUptime() const;
        std::string report() const;
    };
    
    VisualizationStats getStats() const;
    
    // Health check
    bool isHealthy() const;
    std::string getHealthReport() const;
    
    // Cleanup
    void cleanup();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Visualization backends
enum class VisualizationBackend {
    Console,
    OpenGL,
    WebGL,
    Vulkan,
    DirectX,
    Metal,
    Custom
};

// Visualization styles
enum class VisualizationStyle {
    Minimal,
    Classic,
    Modern,
    Scientific,
    Game,
    Astronomical
};

// Data types for visualization
enum class VisualizationDataType {
    Scalar,
    Vector2D,
    Vector3D,
    Matrix,
    TimeSeries,
    Network,
    Heatmap,
    Histogram
};

// Color schemes
enum class ColorScheme {
    Viridis,
    Plasma,
    Inferno,
    Magma,
    Cividis,
    Blues,
    Greens,
    Reds,
    Purples,
    Sequential,
    Diverging,
    Qualitative
};

} // namespace nlm
