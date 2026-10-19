// Comprehensive visualization and debugging framework
// Tools for visualizing neural simulations and debugging complex behavior

#ifndef VISUALIZATION_DEBUG_HPP
#define VISUALIZATION_DEBUG_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "Brain.hpp"
#include "SimpleWorld.hpp"
#include "Config.hpp"

namespace nlm {

// Visualization types
enum class VisualizationType {
    NetworkGraph,
    ActivityHeatmap,
    SpikeRaster,
    3DNeurons,
    MemoryMap,
    ActionTimeline,
    PredictionGraph,
    Custom
};

// Debug modes
enum class DebugMode {
    None,
    Normal,
    Detailed,
    Expert,
    Scientific
};

// Visualization output formats
enum class OutputFormat {
    Text,
    ASCII,
    HTML,
    JSON,
    CSV,
    PNG,
    SVG,
    PDF
};

// Visualization data structure
struct VisualizationData {
    std::string name;
    std::string description;
    std::map<std::string, std::vector<double>> timeSeries;
    std::map<std::string, std::vector<std::vector<double>>> spatialData;
    std::map<std::string, std::string> metadata;
    double timestamp;
    std::chrono::system_clock::time_point captureTime;
    
    VisualizationData(const std::string& name, const std::string& description)
        : name(name), description(description), timestamp(std::time(nullptr)) {
        captureTime = std::chrono::system_clock::now();
    }
};

// Network visualization
class NetworkVisualizer {
public:
    NetworkVisualizer(const std::string& name);
    ~NetworkVisualizer();
    
    // Set visualization data
    void setData(const std::shared_ptr<Brain>& brain);
    
    // Generate network graph
    std::string generateNetworkGraph() const;
    
    // Generate 3D visualization
    std::string generate3DVisualization() const;
    
    // Generate activity heatmap
    std::string generateActivityHeatmap() const;
    
    // Generate spike raster plot
    std::string generateSpikeRaster() const;
    
    // Generate neuron connectivity map
    std::string generateConnectivityMap() const;
    
    // Save visualization to file
    bool saveToFile(const std::string& filepath, OutputFormat format = OutputFormat::ASCII);
    
    // Export to different formats
    bool exportToJSON(const std::string& filepath) const;
    bool exportToCSV(const std::string& filepath) const;
    bool exportToHTML(const std::string& filepath) const;
    bool exportToSVG(const std::string& filepath) const;
    
private:
    std::string name_;
    std::shared_ptr<Brain> brain_;
    mutable std::mutex mutex_;
    
    // Generate graph in DOT format
    std::string generateDOTGraph() const;
    
    // Generate 3D scene description
    std::string generate3DScene() const;
    
    // Generate heatmap data
    std::string generateHeatmapData() const;
    
    // Generate spike data
    std::string generateSpikeData() const;
    
    // Generate connectivity data
    std::string generateConnectivityData() const;
};

// Activity monitor for real-time visualization
class ActivityMonitor {
public:
    ActivityMonitor(const std::string& name);
    ~ActivityMonitor();
    
    // Start monitoring
    void startMonitoring(const std::shared_ptr<Brain>& brain, size_t updateIntervalMs = 1000);
    
    // Stop monitoring
    void stopMonitoring();
    
    // Check if monitoring is active
    bool isMonitoring() const { return monitoring_; }
    
    // Get current activity data
    std::vector<VisualizationData> getCurrentData() const;
    
    // Add custom metric
    void addMetric(const std::string& name, std::function<double()> metricFunc);
    
    // Remove metric
    void removeMetric(const std::string& name);
    
    // Set update interval
    void setUpdateInterval(size_t intervalMs) { updateIntervalMs_ = intervalMs; }
    
    // Get update interval
    size_t getUpdateInterval() const { return updateIntervalMs_; }
    
    // Save monitoring data
    bool saveMonitoringData(const std::string& filepath) const;
    
    // Load monitoring data
    bool loadMonitoringData(const std::string& filepath);
    
    // Generate monitoring report
    std::string generateMonitoringReport() const;
    
    // Export monitoring data to different formats
    bool exportToJSON(const std::string& filepath) const;
    bool exportToCSV(const std::string& filepath) const;
    bool exportToHTML(const std::string& filepath) const;
    
private:
    std::string name_;
    std::shared_ptr<Brain> brain_;
    bool monitoring_;
    size_t updateIntervalMs_;
    std::vector<VisualizationData> currentData_;
    std::map<std::string, std::function<double()>> metrics_;
    mutable std::mutex mutex_;
    
    // Monitoring thread
    std::thread monitoringThread_;
    
    // Monitoring loop
    void monitoringLoop();
    
    // Update activity data
    void updateActivityData();
    
    // Calculate metrics
    void calculateMetrics();
    
    // Generate visualization data
    void generateVisualizationData();
};

// Debug session for detailed debugging
class DebugSession {
public:
    DebugSession(const std::string& name, DebugMode mode = DebugMode::Normal);
    ~DebugSession();
    
    // Set brain to debug
    void setBrain(const std::shared_ptr<Brain>& brain);
    
    // Set debug mode
    void setDebugMode(DebugMode mode) { mode_ = mode; }
    
    // Get debug mode
    DebugMode getDebugMode() const { return mode_; }
    
    // Set breakpoint
    void setBreakpoint(size_t step, const std::string& condition = "");
    
    // Remove breakpoint
    void removeBreakpoint(size_t step);
    
    // Check if breakpoint is hit
    bool isBreakpointHit(size_t step) const;
    
    // Step through simulation
    bool stepThrough(size_t steps = 1);
    
    // Set simulation speed
    void setSimulationSpeed(double speed) { simulationSpeed_ = speed; }
    
    // Get simulation speed
    double getSimulationSpeed() const { return simulationSpeed_; }
    
    // Capture snapshot
    void captureSnapshot(const std::string& name = "snapshot");
    
    // Get snapshots
    const std::vector<VisualizationData>& getSnapshots() const { return snapshots_; }
    
    // Remove snapshot
    void removeSnapshot(const std::string& name);
    
    // Clear all snapshots
    void clearSnapshots() { snapshots_.clear(); }
    
    // Save debug session
    bool saveDebugSession(const std::string& filepath) const;
    
    // Load debug session
    bool loadDebugSession(const std::string& filepath);
    
    // Generate debug report
    std::string generateDebugReport() const;
    
    // Export debug data
    bool exportToJSON(const std::string& filepath) const;
    bool exportToCSV(const std::string& filepath) const;
    
private:
    std::string name_;
    std::shared_ptr<Brain> brain_;
    DebugMode mode_;
    double simulationSpeed_;
    std::map<size_t, std::string> breakpoints_;
    std::vector<VisualizationData> snapshots_;
    mutable std::mutex mutex_;
    
    // Debug state
    size_t currentStep_;
    bool simulationRunning_;
    
    // Debug functions
    void updateDebugState();
    void checkBreakpoints();
    void generateDebugSnapshot();
    void captureBrainState();
    void analyzeBehavior();
    
    // Debug analysis
    void analyzeNeuronalActivity() const;
    void analyzeSynapticPlasticity() const;
    void analyzeMemoryUsage() const;
    void analyzePredictionPerformance() const;
    
    // Debug visualization
    std::string generateDebugVisualization() const;
    std::string generateActivityTimeline() const;
    std::string generateMemoryUsageChart() const;
    std::string generatePredictionGraph() const;
};

// Debugger for interactive debugging
class Debugger {
public:
    Debugger(const std::string& name);
    ~Debugger();
    
    // Start debugging
    void start(const std::shared_ptr<Brain>& brain);
    
    // Stop debugging
    void stop();
    
    // Check if debugging is active
    bool isActive() const { return active_; }
    
    // Set debug mode
    void setDebugMode(DebugMode mode);
    
    // Get debug mode
    DebugMode getDebugMode() const { return currentDebugMode_; }
    
    // Set visualization type
    void setVisualizationType(VisualizationType type);
    
    // Get visualization type
    VisualizationType getVisualizationType() const { return currentVisualizationType_; }
    
    // Start visual debugging
    void startVisualDebugging(const std::string& backend = "terminal");
    
    // Stop visual debugging
    void stopVisualDebugging();
    
    // Check if visual debugging is active
    bool isVisualDebuggingActive() const { return visualDebuggingActive_; }
    
    // Set update callback
    void setUpdateCallback(std::function<void(const std::vector<VisualizationData>&)> callback);
    
    // Set step callback
    void setStepCallback(std::function<void(size_t)> callback);
    
    // Set breakpoint callback
    void setBreakpointCallback(std::function<void(size_t)> callback);
    
    // Add debug command
    void addDebugCommand(const std::string& name, std::function<void()> command);
    
    // Execute debug command
    bool executeDebugCommand(const std::string& name);
    
    // List debug commands
    std::vector<std::string> listDebugCommands() const;
    
    // Save debug session
    bool saveDebugSession(const std::string& filepath) const;
    
    // Load debug session
    bool loadDebugSession(const std::string& filepath);
    
    // Generate debug report
    std::string generateDebugReport() const;
    
    // Export debug data
    bool exportToJSON(const std::string& filepath) const;
    bool exportToCSV(const std::string& filepath) const;
    
private:
    std::string name_;
    std::shared_ptr<Brain> brain_;
    bool active_;
    DebugMode currentDebugMode_;
    VisualizationType currentVisualizationType_;
    bool visualDebuggingActive_;
    
    std::function<void(const std::vector<VisualizationData>&)> updateCallback_;
    std::function<void(size_t)> stepCallback_;
    std::function<void(size_t)> breakpointCallback_;
    
    std::map<std::string, std::function<void()>> debugCommands_;
    
    mutable std::mutex mutex_;
    
    // Debugging components
    ActivityMonitor activityMonitor_;
    DebugSession debugSession_;
    NetworkVisualizer networkVisualizer_;
    
    // Debugging thread
    std::thread debugThread_;
    
    // Debugging loop
    void debuggingLoop();
    
    // Update debugging state
    void updateDebuggingState();
    
    // Render visualization
    void renderVisualization();
    
    // Handle user input
    void handleUserInput();
    
    // Execute debug commands
    void executeDebugCommands();
};

// Visualization factory for creating visualization components
class VisualizationFactory {
public:
    static std::unique_ptr<NetworkVisualizer> createNetworkVisualizer(const std::string& name);
    
    static std::unique_ptr<ActivityMonitor> createActivityMonitor(const std::string& name);
    
    static std::unique_ptr<DebugSession> createDebugSession(const std::string& name, DebugMode mode = DebugMode::Normal);
    
    static std::unique_ptr<Debugger> createDebugger(const std::string& name);
    
    // Create visualization based on type
    static std::unique_ptr<NetworkVisualizer> createVisualizer(VisualizationType type, const std::string& name);
    
    // Create debugger based on mode
    static std::unique_ptr<Debugger> createDebugger(DebugMode mode, const std::string& name);
    
    // Create visualization for brain
    static std::unique_ptr<NetworkVisualizer> createBrainVisualizer(const std::shared_ptr<Brain>& brain,
                                                                     VisualizationType type = VisualizationType::NetworkGraph);
    
    // Create activity monitor for brain
    static std::unique_ptr<ActivityMonitor> createBrainActivityMonitor(const std::shared_ptr<Brain>& brain,
                                                                        size_t updateIntervalMs = 1000);
    
    // Create debug session for brain
    static std::unique_ptr<DebugSession> createBrainDebugSession(const std::shared_ptr<Brain>& brain,
                                                                  DebugMode mode = DebugMode::Normal);
    
    // Create debugger for brain
    static std::unique_ptr<Debugger> createBrainDebugger(const std::shared_ptr<Brain>& brain,
                                                          DebugMode mode = DebugMode::Normal);
    
    // Register custom visualizer
    static void registerVisualizer(const std::string& name, std::function<std::unique_ptr<NetworkVisualizer>(const std::string&)> factory);
    
    // Get registered visualizer
    static std::unique_ptr<NetworkVisualizer> getVisualizer(const std::string& name, const std::string& param = "");
    
    // List all registered visualizers
    static std::vector<std::string> listVisualizers();
    
    // Register custom debug command
    static void registerDebugCommand(const std::string& name, std::function<void()> command);
    
    // Get debug command
    static std::function<void()> getDebugCommand(const std::string& name);
    
    // List all debug commands
    static std::vector<std::string> listDebugCommands();
    
    // Get visualization supported types
    static std::vector<VisualizationType> getSupportedVisualizationTypes();
    
    // Get debug supported modes
    static std::vector<DebugMode> getSupportedDebugModes();
    
    // Get output supported formats
    static std::vector<OutputFormat> getSupportedOutputFormats();
    
    // Create visualization configuration
    static std::string createVisualizationConfig(const std::string& name,
                                                 VisualizationType type,
                                                 DebugMode debugMode,
                                                 OutputFormat outputFormat);
    
    // Load visualization configuration
    static std::string loadVisualizationConfig(const std::string& filepath);
    
    // Save visualization configuration
    static bool saveVisualizationConfig(const std::string& config, const std::string& filepath);
};

// Convenience functions for quick debugging
namespace Debug {
    // Create debug session
    std::unique_ptr<DebugSession> createDebugSession(const std::string& name, DebugMode mode = DebugMode::Normal);
    
    // Create debugger
    std::unique_ptr<Debugger> createDebugger(const std::string& name, DebugMode mode = DebugMode::Normal);
    
    // Create network visualizer
    std::unique_ptr<NetworkVisualizer> createNetworkVisualizer(const std::string& name);
    
    // Create activity monitor
    std::unique_ptr<ActivityMonitor> createActivityMonitor(const std::string& name);
    
    // Debug brain simulation
    void debugBrain(const std::shared_ptr<Brain>& brain, DebugMode mode = DebugMode::Normal);
    
    // Visualize brain
    void visualizeBrain(const std::shared_ptr<Brain>& brain, VisualizationType type = VisualizationType::NetworkGraph);
    
    // Monitor brain activity
    void monitorBrainActivity(const std::shared_ptr<Brain>& brain, size_t updateIntervalMs = 1000);
    
    // Run quick benchmark with visualization
    std::string benchmarkWithVisualization(const std::string& benchmarkName,
                                           size_t neuronCount = 1000,
                                           size_t steps = 1000);
    
    // Generate visualization report
    std::string generateVisualizationReport(const std::shared_ptr<Brain>& brain);
    
    // Export visualization data
    bool exportVisualizationData(const std::shared_ptr<Brain>& brain,
                                 const std::string& filepath,
                                 const std::string& format = "json");
    
    // Import visualization data
    bool importVisualizationData(std::shared_ptr<Brain>& brain,
                                 const std::string& filepath);
}

} // namespace nlm

#endif // VISUALIZATION_DEBUG_HPP
