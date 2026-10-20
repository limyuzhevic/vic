#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <functional>
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Visualization configuration
struct VisualizationConfig {
    bool enableVisualization = false;
    std::chrono::milliseconds updateInterval = std::chrono::milliseconds(100);
    bool enableNetworkVisualization = true;
    bool enableActivityVisualization = true;
    bool enableSpikeRaster = true;
    bool enableWeightMatrix = true;
    bool enableConnectivityMaps = true;
    bool enableActivityHeatmaps = true;
    double windowWidth = 1920.0;
    double windowHeight = 1080.0;
    std::string backend = "default"; // "default", "opengl", "vulkan", "webgl"
    bool enable3D = false;
    std::chrono::milliseconds renderInterval = std::chrono::milliseconds(16); // 60 Hz
    bool enableDebugVisualization = false;
    bool enableExportEnabled = true;
    std::string exportDirectory;
};

// Neural activity data for visualization
struct NeuralActivityData {
    std::vector<double> neuronActivations;
    std::vector<double> neuronMembranes;
    std::vector<double> firingRates;
    std::vector<bool> firingStates;
    std::vector<std::vector<float>> connectivityMatrix;
    std::vector<std::vector<double>> weightMatrix;
    std::vector<double> spikeTimes;
    std::chrono::high_resolution_clock::time_point timestamp;
    
    NeuralActivityData() {
        timestamp = std::chrono::high_resolution_clock::now();
    }
};

// Visualization data types
struct VisualizationData {
    NeuralActivityData neuralData;
    std::vector<DetailedSpikeEvent> spikeEvents;
    std::vector<SynapseCheckpointData> synapseData;
    std::vector<NeuronCheckpointData> neuronData;
    PerformanceMetrics performanceMetrics;
    
    // Generate from brain
    void fromBrain(const class Brain& brain, Timestamp currentTime, SimulationStep currentStep);
    
    // Generate from metrics
    void fromMetrics(const PerformanceMetrics& metrics);
};

// Visualization render targets
enum class RenderTarget {
    CONSOLE,
    WINDOW,
    FILE,
    WEB
};

// Visualization export formats
enum class ExportFormat {
    PNG,
    JPG,
    SVG,
    PDF,
    HTML,
    JSON,
    CSV,
    TEXT
};

// Visualization rendering engine for neural activity display
class VisualizationEngine {
public:
    VisualizationEngine();
    ~VisualizationEngine();
    
    // Disable copying
    VisualizationEngine(const VisualizationEngine&) = delete;
    VisualizationEngine& operator=(const VisualizationEngine&) = delete;
    
    // Initialize visualization engine
    bool initialize(const VisualizationConfig& config = VisualizationConfig());
    
    // Start/stop visualization
    void start();
    void stop();
    bool isRunning() const { return running_.load(); }
    
    // Configure visualization
    void configure(const VisualizationConfig& config);
    VisualizationConfig getConfig() const { return config_; }
    
    // Update visualization data
    void update();
    void update(const class Brain& brain, Timestamp currentTime, SimulationStep currentStep);
    
    // Render visualization
    void render();
    void render(RenderTarget target, const std::string& filename = "");
    
    // Visualization types
    void visualizeNetwork(const class Brain& brain);
    void visualizeActivity(const class Brain& brain);
    void visualizeSpikes(const std::vector<class DetailedSpikeEvent>& spikes);
    void visualizeWeights(const class Brain& brain);
    void visualizeConnectivity(const class Brain& brain);
    void visualizeHeatmaps(const class Brain& brain);
    
    // Export functionality
    bool exportVisualization(const std::string& filename, ExportFormat format = ExportFormat::PNG);
    bool exportFrame(const std::string& filename, ExportFormat format = ExportFormat::PNG);
    
    // Configuration management
    void setUpdateInterval(const std::chrono::milliseconds& interval);
    std::chrono::milliseconds getUpdateInterval() const { return config_.updateInterval; }
    
    // Enable/disable components
    void setNetworkVisualizationEnabled(bool enabled) { config_.enableNetworkVisualization = enabled; }
    void setActivityVisualizationEnabled(bool enabled) { config_.enableActivityVisualization = enabled; }
    void setSpikeRasterEnabled(bool enabled) { config_.enableSpikeRaster = enabled; }
    void setWeightMatrixEnabled(bool enabled) { config_.enableWeightMatrix = enabled; }
    void setConnectivityMapsEnabled(bool enabled) { config_.enableConnectivityMaps = enabled; }
    void setActivityHeatmapsEnabled(bool enabled) { config_.enableActivityHeatmaps = enabled; }
    
    bool isNetworkVisualizationEnabled() const { return config_.enableNetworkVisualization; }
    bool isActivityVisualizationEnabled() const { return config_.enableActivityVisualization; }
    bool isSpikeRasterEnabled() const { return config_.enableSpikeRaster; }
    bool isWeightMatrixEnabled() const { return config_.enableWeightMatrix; }
    bool isConnectivityMapsEnabled() const { return config_.enableConnectivityMaps; }
    bool isActivityHeatmapsEnabled() const { return config_.enableActivityHeatmaps; }
    
    // Debug visualization
    void enableDebugVisualization(bool enabled) { config_.enableDebugVisualization = enabled; }
    void setDebugMode(const std::string& mode) { debugMode_ = mode; }
    
    // Data access
    const VisualizationData& getCurrentData() const { return currentData_; }
    const NeuralActivityData& getNeuralData() const { return currentData_.neuralData; }
    const std::vector<DetailedSpikeEvent>& getSpikeEvents() const { return currentData_.spikeEvents; }
    
    // Get singleton instance
    static VisualizationEngine& getInstance() {
        static VisualizationEngine instance;
        return instance;
    }
    
    // Global visualization functions
    static void updateGlobal(const class Brain& brain, Timestamp currentTime, SimulationStep currentStep);
    static void renderGlobal(RenderTarget target = RenderTarget::WINDOW, const std::string& filename = "");
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    mutable std::shared_mutex mutex_;
    std::atomic<bool> running_;
    VisualizationConfig config_;
    VisualizationData currentData_;
    std::chrono::high_resolution_clock::time_point lastUpdateTime_;
    std::string debugMode_;
    
    // Internal helper methods
    void updateData(const class Brain& brain, Timestamp currentTime, SimulationStep currentStep);
    void renderConsole();
    void renderWindow();
    void renderFile(const std::string& filename);
    void renderWeb(const std::string& filename);
    
    // Render helpers
    void renderNetworkGraph(const class Brain& brain);
    void renderActivityHeatmap(const class Brain& brain);
    void renderSpikeRaster(const std::vector<class DetailedSpikeEvent>& spikes);
    void renderWeightMatrix(const class Brain& brain);
    void renderConnectivityMap(const class Brain& brain);
    void renderDebugInfo(const class Brain& brain);
    
    // Export helpers
    bool exportToPNG(const std::string& filename);
    bool exportToJPG(const std::string& filename);
    bool exportToSVG(const std::string& filename);
    bool exportToPDF(const std::string& filename);
    bool exportToHTML(const std::string& filename);
    bool exportToJSON(const std::string& filename);
    bool exportToCSV(const std::string& filename);
    bool exportToText(const std::string& filename);
    
    // Console output formatting
    std::string formatConsoleOutput(const VisualizationData& data) const;
    std::string formatNetworkInfo(const class Brain& brain) const;
    std::string formatActivityInfo(const class Brain& brain) const;
    std::string formatSpikeInfo(const std::vector<class DetailedSpikeEvent>& spikes) const;
    std::string formatPerformanceInfo(const PerformanceMetrics& metrics) const;
    
    // Color and style helpers
    std::string getActivityColor(double activation) const;
    std::string getSpikeColor(const DetailedSpikeEvent& spike) const;
    std::string getConnectionColor(float weight) const;
};

} // namespace nlm
