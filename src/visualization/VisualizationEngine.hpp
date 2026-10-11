#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Forward declarations
class Brain;
class DebugConsole;

// Visualization modes
enum class VisualizationMode {
    NetworkGraph,
    SpikeRaster,
    SynapticWeights,
    BrainActivity,
    MemorySystem,
    Neuromodulation,
    Development,
    Custom
};

// Visualization data structures
struct NeuralNetworkData {
    std::vector<NeuronId> neurons;
    std::vector<std::pair<NeuronId, NeuronId>> connections;
    std::vector<std::pair<NeuronId, NeuronId>> excitatoryConnections;
    std::vector<std::pair<NeuronId, NeuronId>> inhibitoryConnections;
    std::map<NeuronId, float> neuronActivations;
    std::map<std::pair<NeuronId, NeuronId>, float> synapticWeights;
};

struct SpikeData {
    std::vector<std::pair<SimulationStep, NeuronId>> spikes;
    SimulationStep startStep;
    SimulationStep endStep;
    std::map<NeuronId, std::vector<SimulationStep>> neuronSpikes;
};

struct SynapticData {
    std::vector<SynapseId> synapses;
    std::map<SynapseId, float> weights;
    std::map<SynapseId, Delay> delays;
    std::map<SynapseId, SynapseType> types;
};

struct BrainActivityData {
    SimulationStep currentStep;
    double currentTime;
    size_t totalNeurons;
    size_t activeNeurons;
    size_t firingNeurons;
    float averageFiringRate;
    float excitationInhibitionRatio;
    std::map<std::string, float> regionActivities;
    std::vector<std::pair<std::string, float>> neuromodulatorLevels;
};

struct MemoryData {
    size_t workingMemorySize;
    size_t episodicMemorySize;
    size_t associativeMemorySize;
    std::vector<std::pair<Timestamp, float>> workingMemoryTrace;
    std::vector<std::pair<Timestamp, std::string>> episodicMemoryItems;
};

// Visualization window properties
struct VisualizationWindow {
    std::string title;
    double x;  // position (percentage 0-1)
    double y;
    double width;
    double height;
    bool visible;
    VisualizationMode mode;
    
    VisualizationWindow(const std::string& title = "", double x = 0.0, double y = 0.0,
                       double width = 1.0, double height = 1.0, VisualizationMode mode = VisualizationMode::NetworkGraph)
        : title(title), x(x), y(y), width(width), height(height), visible(true), mode(mode) {}
};

// Visualization update callback
struct VisualizationUpdateCallback {
    VisualizationMode mode;
    std::function<void(VisualizationWindow&, const Brain*, double)> callback;
    int priority;  // Higher priority callbacks update first
    
    VisualizationUpdateCallback(VisualizationMode m, std::function<void(VisualizationWindow&, const Brain*, double)> cb, int prio = 0)
        : mode(m), callback(cb), priority(prio) {}
};

// Visualization configuration
struct VisualizationEngineConfig {
    bool enabled = true;
    double updateInterval = 0.016;  // ~60Hz
    bool enableNetworkView = true;
    bool enableSpikeView = true;
    bool enableWeightView = true;
    bool enableActivityView = true;
    bool enableMemoryView = true;
    bool enableNeuromodulationView = true;
    bool enableDevelopmentView = true;
    std::vector<VisualizationWindow> windows;
    std::vector<VisualizationUpdateCallback> updateCallbacks;
    std::string backend;  // "opengl", "sdl", "software", "web"
    int width = 1920;
    int height = 1080;
    bool fullscreen = false;
    bool showGrid = true;
    bool showLabels = true;
    bool showTooltips = true;
    std::string colorScheme;  // "default", "neuro", "dark", "light"
};

// Visualization statistics
struct VisualizationStats {
    size_t framesRendered;
    double lastFrameTime;
    double averageFrameTime;
    double fps;
    size_t totalMemoryUsage;
    bool networkUpdated;
    bool spikesUpdated;
    bool weightsUpdated;
    bool activityUpdated;
    bool memoryUpdated;
    bool neuromodulationUpdated;
    bool developmentUpdated;
    
    VisualizationStats() : framesRendered(0), lastFrameTime(0.0), averageFrameTime(0.0), fps(0.0),
                           totalMemoryUsage(0), networkUpdated(false), spikesUpdated(false),
                           weightsUpdated(false), activityUpdated(false), memoryUpdated(false),
                           neuromodulationUpdated(false), developmentUpdated(false) {}
};

// Visualization engine class
class VisualizationEngine {
public:
    VisualizationEngine();
    VisualizationEngine(const VisualizationEngineConfig& config);
    ~VisualizationEngine();
    
    // Disable copying
    VisualizationEngine(const VisualizationEngine&) = delete;
    VisualizationEngine& operator=(const VisualizationEngine&) = delete;
    
    // Initialize engine
    bool initialize();
    
    // Update visualization data
    void update(const Brain* brain, double deltaTime);
    
    // Render visualization
    void render();
    
    // Close engine
    void close();
    
    // Configuration
    void updateConfig(const VisualizationEngineConfig& config);
    const VisualizationEngineConfig& getConfig() const;
    
    // Window management
    void addWindow(const VisualizationWindow& window);
    void removeWindow(const std::string& title);
    void clearWindows();
    VisualizationWindow* getWindow(const std::string& title);
    const VisualizationWindow* getWindow(const std::string& title) const;
    
    // Update callbacks
    void addUpdateCallback(const VisualizationUpdateCallback& callback);
    void removeUpdateCallback(VisualizationMode mode);
    void clearUpdateCallbacks();
    
    // Data access
    NeuralNetworkData getNetworkData() const;
    SpikeData getSpikeData() const;
    SynapticData getSynapticData() const;
    BrainActivityData getBrainActivityData() const;
    MemoryData getMemoryData() const;
    
    // Control
    void pause();
    void resume();
    bool isPaused() const;
    
    // Statistics
    const VisualizationStats& getStats() const;
    
    // File operations
    bool saveView(const std::string& filepath);
    bool loadView(const std::string& filepath);
    
    // Export functionality
    bool exportVisualization(const std::string& format, const std::string& filepath);
    
    // Interaction
    void onMouseClick(double x, double y, int button);
    void onMouseMove(double x, double y);
    void onKeyPress(int key, int mods);
    void onScroll(double offsetX, double offsetY);
    
    // Debug integration
    void setDebugConsole(DebugConsole* console);
    
    // Check if engine is active
    bool isActive() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
