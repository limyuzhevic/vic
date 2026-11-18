#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <map>
#include <functional>

namespace nlm {

// Forward declarations
class Brain;
class Neuron;
class Synapse;
class SpikeEvent;
class Action;

// Visualization modes
enum class VisualizationMode {
    None,
    Network,
    Activity,
    Spikes,
    Weights,
    Learning,
    Memory,
    Neuromodulation,
    Development,
    Complete
};

// Debug information structure
struct DebugInfo {
    double timestamp = 0.0;
    size_t neuronCount = 0;
    size_t synapseCount = 0;
    float averageFiringRate = 0.0f;
    float excitationInhibitionRatio = 0.0f;
    size_t spikeCount = 0;
    size_t activeNeuronCount = 0;
    std::map<std::string, float> neuromodulationLevels;
    std::map<std::string, float> plasticityRates;
    std::vector<float> memoryTraces;
    float learningProgress = 0.0f;
    float curiosityLevel = 0.0f;
    float noveltyLevel = 0.0f;
};

// Event log structure
struct DebugEvent {
    double timestamp;
    std::string eventType;
    std::string description;
    std::string severity; // INFO, WARN, ERROR
    std::unordered_map<std::string, std::string> metadata;
};

// Performance metrics
struct PerformanceMetrics {
    double simulationTime = 0.0;
    double cpuTime = 0.0;
    double memoryUsage = 0.0;
    size_t spikeEventsProcessed = 0;
    size_t synapticChanges = 0;
    double frameRate = 0.0;
    std::vector<double> performanceHistory;
};

// Visualization interface for debugging and analysis
// Enhanced version with comprehensive debugging and visualization capabilities
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize visualization with options
    bool initialize(const std::unordered_map<std::string, std::string>& options = std::unordered_map<std::string, std::string>());
    
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
    
    // Visualization modes
    void setVisualizationMode(VisualizationMode mode);
    VisualizationMode getVisualizationMode() const;
    
    // Network visualization
    void visualizeNetwork(const Brain& brain);
    
    // Activity visualization
    void visualizeActivity(const Brain& brain);
    
    // Spike raster plot
    void visualizeSpikes(const std::vector<SpikeEvent>& spikes);
    
    // Weight matrix visualization
    void visualizeWeights(const Brain& brain);
    
    // Learning dynamics visualization
    void visualizeLearning(const Brain& brain);
    
    // Memory system visualization
    void visualizeMemory(const Brain& brain);
    
    // Neuromodulation visualization
    void visualizeNeuromodulation(const Brain& brain);
    
    // Development visualization
    void visualizeDevelopment(const Brain& brain);
    
    // Complete brain visualization
    void visualizeComplete(const Brain& brain);
    
    // Export visualization data
    void exportNetworkGraph(const std::string& filepath);
    void exportActivityData(const std::string& filepath);
    void exportDebugLog(const std::string& filepath);
    
    // Debug functionality
    void enableDebugMode(bool enable);
    bool isDebugModeEnabled() const;
    
    // Event logging
    void logEvent(const std::string& eventType, const std::string& description, 
                  const std::string& severity = "INFO",
                  const std::unordered_map<std::string, std::string>& metadata = std::unordered_map<std::string, std::string>());
    
    // Get debug information
    DebugInfo getDebugInfo() const;
    std::vector<DebugEvent> getDebugLog() const;
    
    // Performance monitoring
    void startPerformanceTimer();
    void stopPerformanceTimer();
    PerformanceMetrics getPerformanceMetrics() const;
    
    // Highlight specific neurons or synapses
    void highlightNeuron(const NeuronId& neuronId, const std::string& color = "red");
    void highlightSynapse(const SynapseId& synapseId, const std::string& color = "yellow");
    void clearHighlights();
    
    // Time travel / replay
    void setReplaySpeed(double speed);
    double getReplaySpeed() const;
    void jumpToStep(size_t step);
    bool isReplaying() const;
    
    // Export/Import
    bool saveSession(const std::string& filepath);
    bool loadSession(const std::string& filepath);
    
    // Interactive features
    bool handleMouseClick(float x, float y);
    bool handleKeyPress(int key);
    void setCameraAngle(float angleX, float angleY, float distance);
    
    // Statistical analysis
    void computeStatisticalAnalysis(const Brain& brain);
    std::unordered_map<std::string, double> getStatisticalAnalysis() const;
    
    // Pattern detection
    std::vector<std::string> detectPattern(const Brain& brain, const std::string& patternType);
    void highlightPattern(const std::string& patternId, const std::string& color = "cyan");
    
    // Time series analysis
    std::vector<double> getTimeSeries(const std::string& metric, size_t windowSize = 100);
    void plotTimeSeries(const std::string& metric, const std::string& filepath);
    
    // Machine learning visualization
    void visualizeMLModel(const std::string& modelFile);
    void visualizeFeatureImportance(const Brain& brain);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
