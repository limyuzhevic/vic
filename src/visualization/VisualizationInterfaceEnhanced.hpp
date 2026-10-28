// Enhanced visualization interface for Phase 6 - Advanced debugging and analysis
// This replaces the placeholder VisualizationInterface with comprehensive functionality

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <functional>

namespace nlm {

// Visualization modes
enum class VisualizationMode {
    NetworkGraph,
    ActivityHeatmap,
    SpikeRaster,
    WeightMatrix,
    MemoryTraces,
    Neuromodulation,
    Development,
    Performance,
    Comparative
};

// Export formats
enum class ExportFormat {
    PNG,
    SVG,
    PDF,
    JSON,
    CSV,
    MATLAB
};

// Visualization configuration
struct VisualizationConfig {
    VisualizationMode mode;
    double updateRate;
    bool showLabels;
    bool showConnections;
    bool animate;
    double animationSpeed;
    std::string colorScheme;
    double opacity;
    std::map<std::string, double> thresholds;
    std::map<std::string, std::string> metadata;
};

// Real-time monitoring data
struct MonitoringData {
    std::chrono::system_clock::time_point timestamp;
    size_t stepCount;
    double simulationTime;
    size_t activeNeurons;
    size_t firingNeurons;
    size_t totalSpikes;
    double averageFiringRate;
    double excitationInhibitionRatio;
    float averageDopamineLevel;
    float averageCuriosityLevel;
    float averageNoveltyLevel;
    float averagePredictionError;
    size_t workingMemoryUsage;
    size_t episodicMemoryUsage;
    size_t associativeMemoryUsage;
    std::vector<float> recentFiringRates;
    std::vector<double> neuralActivity;
    std::map<std::string, double> systemMetrics;
    std::vector<std::string> activeEvents;
};

// Event callback for real-time updates
using VisualizationCallback = std::function<void(const MonitoringData&)>;

// Advanced visualization interface for debugging and analysis
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize visualization with configuration
    bool initialize(const VisualizationConfig& config = VisualizationConfig{});
    
    // Update visualization data with monitoring info
    void update(const MonitoringData& data);
    
    // Render current state
    void render();
    
    // Close visualization
    void close();
    
    // Check if visualization is active
    bool isActive() const;
    
    // Set update rate
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    
    // Control visualization modes
    void setVisualizationMode(VisualizationMode mode);
    VisualizationMode getVisualizationMode() const;
    
    // Configuration management
    void setVisualizationConfig(const VisualizationConfig& config);
    const VisualizationConfig& getVisualizationConfig() const;
    
    // Real-time monitoring
    void startRealTimeMonitoring(bool enable);
    bool isRealTimeMonitoringEnabled() const;
    void setMonitoringCallback(VisualizationCallback callback);
    
    // Network visualization
    void visualizeNetwork(const class Brain& brain);
    void visualizeNetwork(const std::vector<std::pair<size_t, size_t>>& connections);
    
    // Activity visualization
    void visualizeActivity(const class Brain& brain);
    void visualizeActivity(const std::vector<float>& activityData, const std::vector<size_t>& neuronIds);
    
    // Spike visualization
    void visualizeSpikes(const std::vector<class SpikeEvent>& spikes);
    void visualizeSpikeRaster(const std::vector<class SpikeEvent>& spikes, double timeWindow = 1000.0);
    
    // Weight visualization
    void visualizeWeights(const class Brain& brain);
    void visualizeWeights(const std::vector<std::vector<float>>& weightMatrix);
    
    // Memory system visualization
    void visualizeMemory(const class Brain& brain);
    void visualizeWorkingMemory(const class Brain& brain);
    void visualizeEpisodicMemory(const class Brain& brain);
    void visualizeAssociativeMemory(const class Brain& brain);
    
    // Neuromodulation visualization
    void visualizeNeuromodulation(const class Brain& brain);
    void visualizeDopamine(const class Brain& brain);
    void visualizeCuriosity(const class Brain& brain);
    void visualizeNovelty(const class Brain& brain);
    void visualizePredictionError(const class Brain& brain);
    
    // Development visualization
    void visualizeDevelopment(const class Brain& brain);
    void visualizePlasticity(const class Brain& brain);
    void visualizeStructuralPlasticity(const class Brain& brain);
    
    // Performance visualization
    void visualizePerformance(const MonitoringData& data);
    void visualizePerformanceMetrics(const std::map<std::string, double>& metrics);
    
    // Statistical visualization
    void visualizeStatistics(const std::vector<std::vector<double>>& dataMatrix);
    void visualizeTimeSeries(const std::string& name, const std::vector<double>& data);
    void visualizeHistogram(const std::string& name, const std::vector<double>& data, size_t bins = 50);
    
    // Export functionality
    bool exportVisualization(const std::string& filepath, ExportFormat format = ExportFormat::PNG);
    bool exportData(const std::string& filepath, const std::string& dataType = "csv");
    bool exportConfiguration(const std::string& filepath);
    
    // Analysis tools
    std::vector<std::string> detectAnomalies(const MonitoringData& data) const;
    double calculateStabilityMetric(const MonitoringData& data) const;
    std::vector<size_t> findCriticalNeurons(const class Brain& brain) const;
    std::map<size_t, double> analyzeCommunityStructure(const class Brain& brain) const;
    
    // Interactive debugging
    void setDebugMode(bool enable);
    bool isDebugMode() const;
    void captureFrame();
    void saveScreenshot(const std::string& filepath);
    void createAnimation(const std::string& filepath, double fps = 30.0);
    
    // Multi-window support
    void createWindow(const std::string& title, double x, double y, double width, double height);
    void destroyWindow(const std::string& title);
    void setWindowFocus(const std::string& title);
    
    // Color schemes
    void setColorScheme(const std::string& scheme);
    std::vector<std::string> getAvailableColorSchemes() const;
    
    // Threshold configuration
    void setThreshold(const std::string& metric, double threshold);
    double getThreshold(const std::string& metric) const;
    
    // Performance analysis
    void enablePerformanceAnalysis(bool enable);
    bool isPerformanceAnalysisEnabled() const;
    std::map<std::string, double> analyzePerformance() const;
    
    // Memory analysis
    void enableMemoryAnalysis(bool enable);
    bool isMemoryAnalysisEnabled() const;
    std::map<std::string, size_t> analyzeMemoryUsage(const class Brain& brain) const;
    
    // System health
    bool isSystemHealthy() const;
    std::vector<std::string> getSystemStatus() const;
    
    // Calibration and normalization
    void calibrateData(const std::string& metric, double min, double max);
    void normalizeData(const std::string& metric);
    
    // Event filtering
    void addEventFilter(const std::string& eventType, std::function<bool(const MonitoringData&)> filter);
    void removeEventFilter(const std::string& eventType);
    bool shouldFilterEvent(const std::string& eventType, const MonitoringData& data) const;
    
    // Configuration presets
    static VisualizationConfig getPresetConfig(const std::string& presetName);
    static std::vector<std::string> getAvailablePresets();
    
    // Help and documentation
    void showHelp() const;
    void showDocumentation() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
