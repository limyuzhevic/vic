// NLM Enhanced Visualization Interface
// Advanced visualization system with real-time updates and multiple output formats
// Supports network visualization, activity monitoring, and performance analysis

#ifndef NLM_ENHANCED_VISUALIZATION_HPP
#define NLM_ENHANCED_VISUALIZATION_HPP

#include "agent/AgentBody.hpp"
#include "agent/SensoryPercept.hpp"
#include "brain/Brain.hpp"
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <chrono>
#include <functional>

namespace nlm {

class EnhancedVisualization {
public:
    // Visualization modes
    enum class Mode {
        None,           // No visualization
        Network,        // Network topology visualization
        Activity,       // Real-time activity monitoring
        SpikeRaster,    // Spike raster plots
        Connectivity,   // Connection strength visualization
        Performance,    // Performance metrics dashboard
        Memory,         // Memory usage and allocation visualization
        Full           // All available visualizations
    };

    // Output formats
    enum class OutputFormat {
        ASCII,          // ASCII/terminal-based visualization
        HTML,           // HTML web visualization
        JSON,           // JSON data for external visualization
        SVG,            // Scalable Vector Graphics
        PNG,            // PNG image output
        Video,          // Video recording
        Live           // Live updating visualization
    };

    // Visualization data structures
    struct NetworkData {
        std::vector<std::pair<float, float>> neuronPositions;
        std::vector<std::pair<size_t, size_t>> connections;
        std::vector<float> neuronExcitability;
        std::vector<size_t> neuronTypes;
        std::map<size_t, std::string> neuronLabels;
    };

    struct ActivityData {
        std::vector<std::vector<float>> firingRates;
        std::vector<std::vector<float>> membranePotentials;
        std::vector<std::vector<size_t>> spikeTimes;
        std::chrono::steady_clock::time_point timestamp;
    };

    struct PerformanceMetrics {
        double fps;                      // Frames per second
        double simulationSpeed;          // Simulation steps per second
        double memoryUsage;              // Memory usage in MB
        size_t activeNeurons;            // Number of active neurons
        size_t totalSpikes;              // Total spikes processed
        double averageFiringRate;        // Average firing rate across network
        double synchronizationLatency;  // Synchronization latency
        size_t pendingSpikeEvents;       // Number of pending spike events
    };

    struct VisualizationConfig {
        Mode mode;                       // Current visualization mode
        OutputFormat outputFormat;       // Output format
        std::string outputPath;          // Output file path
        int updateInterval;              // Update interval in milliseconds
        bool enableRealTime;             // Enable real-time updates
        bool enableAnnotations;          // Enable annotations and labels
        std::string colorScheme;         // Color scheme name
        double scaleFactor;              // Visualization scale factor
        std::map<std::string, std::string> customOptions; // Custom options
    };

public:
    EnhancedVisualization();
    ~EnhancedVisualization();
    
    // Initialize visualization system
    bool initialize(const VisualizationConfig& config);
    
    // Update visualization data
    void update(const NetworkData& networkData, const ActivityData& activityData,
                const PerformanceMetrics& metrics, std::shared_ptr<Brain> brain = nullptr);
    
    // Render visualization
    bool render();
    
    // Save visualization to file
    bool saveToFile(const std::string& filename);
    
    // Export visualization data
    std::string exportData() const;
    
    // Get current visualization data
    const NetworkData& getNetworkData() const { return currentNetworkData_; }
    const ActivityData& getActivityData() const { return currentActivityData_; }
    const PerformanceMetrics& getPerformanceMetrics() const { return currentMetrics_; }
    
    // Configure visualization
    void setMode(Mode mode) { config_.mode = mode; }
    void setOutputFormat(OutputFormat format) { config_.outputFormat = format; }
    void setOutputPath(const std::string& path) { config_.outputPath = path; }
    void setUpdateInterval(int interval) { config_.updateInterval = interval; }
    void setRealTimeEnabled(bool enabled) { config_.enableRealTime = enabled; }
    void setColorScheme(const std::string& scheme) { config_.colorScheme = scheme; }
    
    // Get current configuration
    const VisualizationConfig& getConfig() const { return config_; }
    
    // Generate preview
    std::string generatePreview() const;
    
    // Get supported features
    static std::vector<Mode> getSupportedModes() {
        return {Mode::Network, Mode::Activity, Mode::SpikeRaster, 
                Mode::Connectivity, Mode::Performance, Mode::Memory, Mode::Full};
    }
    
    static std::vector<OutputFormat> getSupportedOutputFormats() {
        return {OutputFormat::ASCII, OutputFormat::HTML, OutputFormat::JSON,
                OutputFormat::SVG, OutputFormat::PNG, OutputFormat::Video, OutputFormat::Live};
    }
    
    // Get supported color schemes
    static std::vector<std::string> getSupportedColorSchemes() {
        return {"default", "fire", "ocean", "forest", "medical", "monochrome"};
    }
    
    // Validate configuration
    std::vector<std::string> validateConfiguration() const;
    
    // Get system requirements
    struct SystemRequirements {
        double minMemoryGB;      // Minimum memory in GB
        double minCPUUsage;      // Minimum CPU usage percentage
        int minResolutionWidth;  // Minimum screen width
        int minResolutionHeight;// Minimum screen height
        bool requiresGPU;        // Requires GPU acceleration
    };
    
    static SystemRequirements getSystemRequirements() {
        return {1.0, 10.0, 800, 600, false};
    }

private:
    // Internal visualization methods
    void setupRendering();
    void cleanupRendering();
    void createVisualElements(const NetworkData& networkData, const ActivityData& activityData);
    void updateVisualElements(const NetworkData& networkData, const ActivityData& activityData);
    void generateOutput(const std::string& filename);
    void applyColorScheme(const std::string& scheme);
    
    // ASCII visualization methods
    std::string renderASCII(const NetworkData& networkData, const ActivityData& activityData) const;
    std::string renderNetworkASCII(const NetworkData& networkData) const;
    std::string renderActivityASCII(const ActivityData& activityData) const;
    std::string renderSpikeRasterASCII(const ActivityData& activityData) const;
    
    // HTML visualization methods
    std::string renderHTML(const NetworkData& networkData, const ActivityData& activityData) const;
    std::string renderNetworkHTML(const NetworkData& networkData) const;
    std::string renderActivityHTML(const ActivityData& activityData) const;
    
    // JSON visualization methods
    std::string renderJSON(const NetworkData& networkData, const ActivityData& activityData) const;
    
    // Data processing methods
    void processNetworkData(const NetworkData& networkData);
    void processActivityData(const ActivityData& activityData);
    void calculatePerformanceMetrics(const ActivityData& activityData, std::shared_ptr<Brain> brain);
    
    // File I/O methods
    bool saveASCIIVisualization(const std::string& filename) const;
    bool saveHTMLVisualization(const std::string& filename) const;
    bool saveJSONVisualization(const std::string& filename) const;
    bool saveSVGVisualization(const std::string& filename) const;
    bool savePNGVisualization(const std::string& filename) const;
    
    // Utility methods
    std::string escapeHTML(const std::string& text) const;
    std::string escapeJSON(const std::string& text) const;
    double clamp(double value, double min, double max) const;
    int mapValue(double value, double fromMin, double fromMax, double toMin, double toMax) const;
    
    // Member variables
    VisualizationConfig config_;
    NetworkData currentNetworkData_;
    ActivityData currentActivityData_;
    PerformanceMetrics currentMetrics_;
    bool initialized_;
    std::chrono::steady_clock::time_point lastUpdateTime_;
    
    // Rendering backend (placeholder)
    struct RenderBackend {
        bool available;
        std::string name;
        double version;
    } backend_;
};

// Specialized visualization classes
class NetworkTopologyVisualizer {
public:
    NetworkTopologyVisualizer();
    ~NetworkTopologyVisualizer();
    
    // Visualize neural network topology
    std::string visualize(const NetworkData& networkData, const std::string& format = "ascii") const;
    
    // Generate connectivity matrix
    std::vector<std::vector<double>> generateConnectivityMatrix(const NetworkData& networkData) const;
    
    // Calculate network metrics
    struct NetworkMetrics {
        double density;              // Network density (0-1)
        double averagePathLength;    // Average path length
        double clusteringCoefficient; // Clustering coefficient
        double modularity;           // Network modularity
        double smallWorldness;       // Small-world coefficient
        size_t connectedComponents;  // Number of connected components
        bool isConnected;            // Whether network is fully connected
    };
    
    NetworkMetrics calculateMetrics(const NetworkData& networkData) const;
    
    // Visualize community structure
    std::vector<std::vector<size_t>> detectCommunities(const NetworkData& networkData) const;
    
    // Visualize hub nodes
    std::vector<size_t> identifyHubNodes(const NetworkData& networkData, double threshold = 0.1) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

class ActivityVisualizer {
public:
    ActivityVisualizer();
    ~ActivityVisualizer();
    
    // Visualize spike activity
    std::string visualizeSpikes(const ActivityData& activityData, const std::string& format = "ascii") const;
    
    // Visualize firing rate dynamics
    std::string visualizeFiringRates(const ActivityData& activityData, const std::string& format = "ascii") const;
    
    // Visualize membrane potential dynamics
    std::string visualizeMembranePotentials(const ActivityData& activityData, const std::string& format = "ascii") const;
    
    // Calculate activity statistics
    struct ActivityStatistics {
        double totalActivity;         // Total neural activity
        double peakActivity;          // Peak activity level
        double averageActivity;       // Average activity level
        double activityVariability;   // Standard deviation of activity
        size_t maxActiveNeurons;      // Maximum number of active neurons
        double temporalCoherence;    // Temporal coherence of activity
        double spatialCoherence;     // Spatial coherence of activity
    };
    
    ActivityStatistics calculateStatistics(const ActivityData& activityData) const;
    
    // Detect activity patterns
    std::vector<std::string> detectActivityPatterns(const ActivityData& activityData) const;
    
    // Visualize activity over time
    std::string visualizeActivityTimeSeries(const ActivityData& activityData, const std::string& format = "ascii") const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

class PerformanceDashboard {
public:
    PerformanceDashboard();
    ~PerformanceDashboard();
    
    // Update performance metrics
    void update(const PerformanceMetrics& metrics);
    
    // Generate performance report
    std::string generateReport() const;
    
    // Generate real-time status
    std::string generateRealTimeStatus() const;
    
    // Visualize performance over time
    std::string visualizePerformanceHistory(const std::vector<PerformanceMetrics>& history, 
                                            const std::string& format = "ascii") const;
    
    // Calculate performance trends
    struct PerformanceTrends {
        double fpsTrend;              // FPS trend (positive = improving)
        double memoryTrend;           // Memory usage trend
        double activityTrend;         // Activity level trend
        double efficiencyTrend;       // Efficiency trend
        double stabilityScore;        // Stability score (0-1)
    };
    
    PerformanceTrends calculateTrends(const std::vector<PerformanceMetrics>& history) const;
    
    // Detect performance anomalies
    std::vector<std::string> detectAnomalies(const std::vector<PerformanceMetrics>& history) const;
    
    // Generate alerts
    std::vector<std::string> generateAlerts(const PerformanceMetrics& current) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

#endif // NLM_ENHANCED_VISUALIZATION_HPP