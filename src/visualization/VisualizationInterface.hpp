#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <map>
#include <unordered_map>
#include <functional>

// Forward declarations
namespace nlm {
    class Brain;
    class AgentBrain;
    class SimpleWorld;
    class Neuron;
    class Synapse;
    class SpikeEvent;
}

namespace nlm {

// Visualization data structures
struct NeuronActivityData {
    uint64_t neuronId;
    float membranePotential;
    float firingRate;
    float inhibitionLevel;
    std::chrono::steady_clock::time_point lastSpikeTime;
    bool isActive;
};

struct SynapseActivityData {
    uint64_t synapseId;
    uint64_t sourceNeuronId;
    uint64_t targetNeuronId;
    float weight;
    float eligibilityTrace;
    std::chrono::steady_clock::time_point lastUpdateTime;
    bool isPlastic;
};

struct PopulationActivityData {
    uint64_t populationId;
    std::string type; // "excitatory", "inhibitory", "sensory", "motor", etc.
    float averageFiringRate;
    float totalActivity;
    size_t activeNeuronCount;
    size_t totalNeuronCount;
};

struct WorldStateData {
    uint64_t worldWidth;
    uint64_t worldHeight;
    std::vector<std::vector<float>> environmentMap;
    std::vector<std::pair<uint64_t, uint64_t>> agentPositions;
    std::vector<float> resourceDensities;
    double currentTime;
};

struct TimeSeriesDataPoint {
    std::chrono::steady_clock::time_point timestamp;
    std::string metricName;
    double value;
    std::unordered_map<std::string, std::string> metadata;
};

struct HeatMapData {
    std::vector<std::vector<float>> heatValues;
    uint64_t width;
    uint64_t height;
    std::string colorScheme; // "thermal", "viridis", "plasma", etc.
    float minValue;
    float maxValue;
};

struct DebugInfo {
    std::string componentName;
    std::string currentState;
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
    std::chrono::steady_clock::time_point lastUpdateTime;
    bool isHealthy;
};

// Visualization modes
enum class VisualizationMode {
    Static,
    RealTime,
    Animated,
    TimeSeries,
    HeatMap,
    3DNeural,
    WorldSimulation
};

// Output formats
enum class OutputFormat {
    Text,
    JSON,
    CSV,
    Binary,
    WebSocket,
    NetworkStream
};

class VisualizationInterface {
public:
    VisualizationInterface();
    virtual ~VisualizationInterface();
    
    // Initialize visualization system
    virtual bool initialize() = 0;
    
    // Update visualization with new data
    virtual void update() = 0;
    
    // Render current state
    virtual void render() = 0;
    
    // Close and cleanup visualization
    virtual void close() = 0;
    
    // Check if visualization is active
    virtual bool isActive() const = 0;
    
    // Set update rate and configuration
    virtual void setUpdateRate(double hz) = 0;
    virtual double getUpdateRate() const = 0;
    virtual void setVisualizationMode(VisualizationMode mode) = 0;
    virtual VisualizationMode getVisualizationMode() const = 0;
    
    // Brain visualization methods
    virtual void visualizeBrain(const Brain& brain) = 0;
    virtual void visualizeNetwork(const Brain& brain, bool highlightActive = true) = 0;
    virtual void visualizeSpikes(const std::vector<SpikeEvent>& spikes, SimulationStep step) = 0;
    virtual void visualizeWeights(const Brain& brain) = 0;
    virtual void visualizeActivity(const Brain& brain, const std::vector<NeuronActivityData>& activityData) = 0;
    
    // Population and connectivity visualization
    virtual void visualizePopulations(const Brain& brain) = 0;
    virtual void visualizeConnections(const Brain& brain, bool highlightPlastic = true) = 0;
    virtual void visualizeSynapses(const std::vector<SynapseActivityData>& synapseData) = 0;
    
    // World simulation visualization
    virtual void visualizeWorld(const SimpleWorld& world) = 0;
    virtual void visualizeAgent(const AgentBrain& agent, const SimpleWorld& world) = 0;
    virtual void visualizeEnvironment(const std::vector<std::vector<float>>& environment) = 0;
    
    // Time-series visualization
    virtual void visualizeTimeSeries(const std::vector<TimeSeriesDataPoint>& data) = 0;
    virtual void addTimeSeriesMetric(const std::string& name, double value) = 0;
    virtual std::vector<TimeSeriesDataPoint> getTimeSeriesData(const std::string& metricName) const = 0;
    
    // Heat maps and activity patterns
    virtual void visualizeHeatMap(const HeatMapData& heatMap) = 0;
    virtual HeatMapData generateActivityHeatMap(const Brain& brain) = 0;
    virtual HeatMapData generateWorldHeatMap(const SimpleWorld& world) = 0;
    
    // Advanced visualization features
    virtual void enableRealTimeUpdates(bool enable) = 0;
    virtual bool areRealTimeUpdatesEnabled() const = 0;
    virtual void setFrameRateLimit(double maxFps) = 0;
    virtual void captureScreenshot(const std::string& filename) = 0;
    virtual void startRecording(const std::string& filename) = 0;
    virtual void stopRecording() = 0;
    
    // Interactive features
    virtual void setInteractiveMode(bool interactive) = 0;
    virtual bool isInteractiveMode() const = 0;
    virtual void handleInput(const std::string& input) = 0;
    virtual std::string getHelpText() const = 0;
    
    // Debug and monitoring integration
    virtual void enableDebugOverlay(bool enable) = 0;
    virtual bool isDebugOverlayEnabled() const = 0;
    virtual void addDebugInfo(const DebugInfo& debugInfo) = 0;
    virtual std::vector<DebugInfo> getDebugInfo() const = 0;
    
    // Configuration and export
    virtual void loadConfiguration(const std::string& configFile) = 0;
    virtual void saveConfiguration(const std::string& configFile) const = 0;
    virtual void exportData(const std::string& filename, OutputFormat format) = 0;
    
    // Performance and monitoring
    virtual double getLastFrameTime() const = 0;
    virtual uint64_t getTotalFramesRendered() const = 0;
    virtual float getFPS() const = 0;
    virtual void resetPerformanceCounters() = 0;
    
    // Callbacks and event handling
    virtual void setDataCallback(std::function<void(const std::string&, const std::vector<double>&)> callback) = 0;
    virtual void setEventCallback(std::function<void(const std::string&, const std::string&)> callback) = 0;
    
protected:
    // Internal helper methods
    virtual void initializeBackend() = 0;
    virtual void cleanupBackend() = 0;
    virtual bool isBackendInitialized() const = 0;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm