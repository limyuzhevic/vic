#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <fstream>
#include <map>
#include <unordered_map>

namespace nlm {

// Forward declarations
class Brain;
class Neuron;
class Synapse;
struct SpikeEvent;

// Visualization modes
enum class VisualizationMode {
    OVERVIEW,
    DETAILED,
    INTERACTIVE,
    HEATMAP,
    TIME_SERIES
};

// Recording modes
enum class RecordingMode {
    NONE,
    CONTINUOUS,
    ON_DEMAND,
    TRIGGERED
};

// Visualization statistics structure
struct VisualizationStats {
    size_t neuronCount;
    size_t synapseCount;
    size_t activeNeuronCount;
    size_t firingNeuronCount;
    float averageFiringRate;
    size_t totalSpikeCount;
    double lastUpdateTime;
    double startTime;
};

// Visualization interface for debugging and analysis
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize visualization with window size and title
    bool initialize(int width = 1920, int height = 1080, const std::string& title = "NLM Visualization");
    
    // Update visualization data from brain state
    void update(const Brain& brain);
    
    // Render current visualization state
    void render();
    
    // Close visualization and cleanup
    void close();
    
    // Check if visualization is active
    bool isActive() const;
    
    // Set update rate (FPS)
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    
    // Set visualization mode
    void setVisualizationMode(VisualizationMode mode);
    VisualizationMode getVisualizationMode() const;
    
    // Toggle recording
    void setRecordingMode(RecordingMode mode);
    void toggleRecording(const std::string& filename = "");
    bool isRecording() const;
    
    // Export visualization data
    bool exportData(const std::string& filename);
    bool exportFrames(const std::string& directory);
    
    // Handle mouse and keyboard input (for interactive mode)
    bool handleMouse(double xpos, double ypos);
    bool handleKeyboard(int key, int action, int mods);
    
    // Get visualization statistics
    VisualizationStats getStats() const;
    
    // Network visualization methods
    void visualizeNetwork(const Brain& brain);
    void visualizeNeurons(const Brain& brain);
    void visualizeSynapses(const Brain& brain);
    
    // Activity visualization methods
    void visualizeActivity(const Brain& brain);
    void visualizeFiringRates(const Brain& brain);
    void visualizeSpikeRaster(const std::vector<SpikeEvent>& spikes);
    
    // Weight matrix visualization
    void visualizeWeights(const Brain& brain);
    void visualizeWeightMatrix(const Brain& brain);
    
    // Monitoring and analysis
    void visualizeMonitoring(const Brain& brain);
    void visualizeMemorySystems(const Brain& brain);
    void visualizeDevelopment(const Brain& brain);
    void visualizeNeuromodulation(const Brain& brain);
    
    // Region-based visualization
    void visualizeRegionHeatmap(const Brain& brain);
    void visualizeRegionalActivity(const Brain& brain);
    
    // Time-series visualization
    void visualizeTimeSeries(const Brain& brain);
    void addTimeSeriesData(const std::string& name, double value);
    
    // Debug visualization
    void visualizeDebugInfo(const Brain& brain);
    void visualizePerformanceStats();
    
    // Camera control for 3D views
    void setCameraPosition(float x, float y, float z);
    void setCameraRotation(float pitch, float yaw);
    void zoom(float factor);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
