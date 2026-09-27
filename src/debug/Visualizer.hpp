// Visualization System for NLM v2.0+
// Real-time neural network visualization with multiple output formats

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <deque>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>
#include <optional>
#include <map>
#include <set>

namespace nlm {

// Visualization targets
enum class VisualizationTarget {
    Network,
    Spikes,
    Memory,
    Behavior,
    Neuromodulators,
    All
};

// Network visualization node
struct NetworkNode {
    uint64_t id;
    std::string type;           // neuron, region, population
    double x;                   // Position in visualization space
    double y;                   // Position in visualization space
    double radius;              // Size in visualization space
    std::string label;          // Display label
    std::string color;          // Hex color code
    double activityLevel;       // 0.0 to 1.0
    uint64_t incomingConnections;
    uint64_t outgoingConnections;
    bool isActive;
    std::map<std::string, std::string> metadata;
    
    NetworkNode(uint64_t nodeId, const std::string& nodeType, double nx, double ny, double r)
        : id(nodeId), type(nodeType), x(nx), y(ny), radius(r), activityLevel(0.0), 
          incomingConnections(0), outgoingConnections(0), isActive(false) {}
};

// Network visualization edge (connection)
struct NetworkEdge {
    uint64_t sourceId;
    uint64_t targetId;
    double weight;             // Connection strength
    double activity;           // Current activity level
    std::string color;         // Edge color
    bool isActive;
    std::string label;         // Optional edge label
    double delay;              // Synaptic delay in ms
    
    NetworkEdge(uint64_t src, uint64_t tgt, double w) : sourceId(src), targetId(tgt), weight(w),
                                                        activity(0.0), isActive(false), delay(0.0) {}
};

// Spikes visualization data
struct SpikeEvent {
    uint64_t neuronId;
    std::chrono::system_clock::time_point timestamp;
    double time;              // Time in seconds
    uint64_t step;
    std::string color;         // Spike color (depends on neuron type)
    double amplitude;          // Spike amplitude
    
    SpikeEvent(uint64_t id, double t, uint64_t s) : neuronId(id), time(t), step(s),
                                                   amplitude(1.0), color("#FF0000") {}
};

// Memory visualization data
struct MemoryNode {
    uint64_t id;
    std::string type;          // working, episodic, semantic, procedural, associative
    std::string label;
    double x;                  // Position in memory space
    double y;                  // Position in memory space
    double size;               // Size in memory space
    double activationLevel;    // 0.0 to 1.0
    std::string color;         // Color based on type
    uint64_t accessCount;
    double lastAccessTime;     // Time since last access
    std::map<std::string, std::string> metadata;
    
    MemoryNode(uint64_t nodeId, const std::string& nodeType, double nx, double ny, double sz)
        : id(nodeId), type(nodeType), x(nx), y(ny), size(sz), activationLevel(0.0),
          accessCount(0), lastAccessTime(0.0) {}
};

// Memory connection (association)
struct MemoryConnection {
    uint64_t sourceId;
    uint64_t targetId;
    double strength;           // Association strength
    double activation;         // Current activation level
    std::string color;
    bool isBidirectional;
    std::string connectionType; // pattern, temporal, semantic, procedural
    
    MemoryConnection(uint64_t src, uint64_t tgt, double str) : sourceId(src), targetId(tgt),
                                                             strength(str), activation(0.0),
                                                             isBidirectional(false), 
                                                             connectionType("pattern") {}
};

// Neuromodulator visualization
struct NeuromodulatorNode {
    std::string type;          // dopamine, curiosity, novelty, acetylcholine, norepinephrine, serotonin, nicotine, orexin
    double level;              // Current level (0.0 to 1.0)
    double target;             // Target level (for visualization)
    double x;                  // Position in neuromodulator space
    double y;                  // Position in neuromodulator space
    double radius;             // Size in neuromodulator space
    std::string color;         // Color based on neuromodulator type
    double changeRate;         // Rate of change
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, std::string> metadata;
    
    NeuromodulatorNode(const std::string& modType, double x, double y, double r)
        : type(modType), level(0.5), target(0.5), x(x), y(y), radius(r),
          changeRate(0.0), timestamp(std::chrono::system_clock::now()) {}
};

// Behavior visualization data
struct BehaviorEvent {
    std::chrono::system_clock::time_point timestamp;
    uint64_t step;
    std::string actionType;
    std::string actionDescription;
    double confidence;         // Confidence in action (0.0 to 1.0)
    double reward;            // Reward value
    std::string color;         // Color based on action type
    std::map<std::string, double> metrics; // Additional metrics
    
    BehaviorEvent(uint64_t s, const std::string& type, const std::string& desc, 
                   double conf, double reward) : step(s), actionType(type), 
                                                 actionDescription(desc), confidence(conf),
                                                 reward(reward), timestamp(std::chrono::system_clock::now()) {}
};

// Visualization update callback
using VisualizationUpdateCallback = std::function<void(const std::string& updateType, const std::map<std::string, std::string>& data)>;

// Network visualization listener
using NetworkUpdateListener = std::function<void(const std::vector<NetworkNode>&, const std::vector<NetworkEdge>&)>;

// Spikes visualization listener
using SpikesUpdateListener = std::function<void(const std::vector<SpikeEvent>&)>;

// Memory visualization listener
using MemoryUpdateListener = std::function<void(const std::vector<MemoryNode>&, const std::vector<MemoryConnection>&)>;

// Neuromodulator visualization listener
using NeuromodulatorUpdateListener = std::function<void(const std::vector<NeuromodulatorNode>&)>;

// Behavior visualization listener
using BehaviorUpdateListener = std::function<void(const std::vector<BehaviorEvent>&)>;

// Visualization renderer interface
class VisualizationRenderer {
public:
    virtual ~VisualizationRenderer() = default;
    virtual void render() = 0;
    virtual void setSize(int width, int height) = 0;
    virtual void setScale(double scale) = 0;
    virtual void setOffset(double x, double y) = 0;
};

// Network renderer implementation
class NetworkRenderer : public VisualizationRenderer {
public:
    NetworkRenderer();
    ~NetworkRenderer() override = default;
    
    void render() override;
    void setSize(int width, int height) override;
    void setScale(double scale) override;
    void setOffset(double x, double y) override;
    
    void setNodePositions(const std::vector<NetworkNode>& nodes);
    void setEdges(const std::vector<NetworkEdge>& edges);
    void setActiveNodes(const std::set<uint64_t>& activeNodes);
    void setHighlightedNodes(const std::set<uint64_t>& highlightedNodes);
    
    std::string exportAsDOT() const;
    std::string exportAsGEXF() const;
    std::string exportAsGraphML() const;
    
    void setLayoutAlgorithm(const std::string& algorithm);
    void setNodeColorMapping(const std::map<std::string, std::string>& colorMap);
    void setEdgeColorMapping(const std::map<std::string, std::string>& colorMap);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Spikes renderer implementation
class SpikesRenderer : public VisualizationRenderer {
public:
    SpikesRenderer();
    ~SpikesRenderer() override = default;
    
    void render() override;
    void setSize(int width, int height) override;
    void setScale(double scale) override;
    void setOffset(double x, double y) override;
    
    void setSpikes(const std::vector<SpikeEvent>& spikes);
    void setTimeWindow(double startTime, double endTime);
    void setSpikeFilter(const std::function<bool(const SpikeEvent&)>& filter);
    
    std::string exportAsTXT() const;
    std::string exportAsCSV() const;
    
    void setSpikeColorScheme(const std::string& scheme);
    void setTimeScale(double scale);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Memory renderer implementation
class MemoryRenderer : public VisualizationRenderer {
public:
    MemoryRenderer();
    ~MemoryRenderer() override = default;
    
    void render() override;
    void setSize(int width, int height) override;
    void setScale(double scale) override;
    void setOffset(double x, double y) override;
    
    void setMemoryNodes(const std::vector<MemoryNode>& nodes);
    void setMemoryConnections(const std::vector<MemoryConnection>& connections);
    void setActiveNodes(const std::set<uint64_t>& activeNodes);
    void setHighlightedNodes(const std::set<uint64_t>& highlightedNodes);
    
    std::string exportAsTXT() const;
    std::string exportAsJSON() const;
    
    void setMemoryTypeColorMapping(const std::map<std::string, std::string>& colorMap);
    void setActivationColorMapping(const std::map<double, std::string>& colorMap);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Main visualization system
class Visualizer {
public:
    Visualizer();
    ~Visualizer() = default;
    
    // Visualization control
    void startVisualization(const std::vector<VisualizationTarget>& targets);
    void stopVisualization();
    bool isVisualizing() const;
    
    // Data input
    void updateNetworkData(const std::vector<NetworkNode>& nodes, const std::vector<NetworkEdge>& edges);
    void updateSpikesData(const std::vector<SpikeEvent>& spikes);
    void updateMemoryData(const std::vector<MemoryNode>& nodes, const std::vector<MemoryConnection>& connections);
    void updateNeuromodulatorData(const std::vector<NeuromodulatorNode>& neuromodulators);
    void updateBehaviorData(const std::vector<BehaviorEvent>& behaviors);
    
    // Visualization configuration
    void setVisualizationPort(int port);
    void setUpdateInterval(double intervalSeconds);
    void setWindowSize(int width, int height);
    void setLayoutAlgorithm(const std::string& algorithm);
    
    // Export functionality
    std::string exportNetwork(const std::string& format = "dot") const;
    std::string exportSpikes(const std::string& format = "txt") const;
    std::string exportMemory(const std::string& format = "txt") const;
    std::string exportBehavior(const std::string& format = "txt") const;
    
    // Visualization mode
    void enableLiveMode();
    void disableLiveMode();
    bool isInLiveMode() const;
    
    // Update callbacks
    void addNetworkUpdateListener(const NetworkUpdateListener& listener);
    void removeNetworkUpdateListener(const NetworkUpdateListener& listener);
    
    void addSpikesUpdateListener(const SpikesUpdateListener& listener);
    void removeSpikesUpdateListener(const SpikesUpdateListener& listener);
    
    void addMemoryUpdateListener(const MemoryUpdateListener& listener);
    void removeMemoryUpdateListener(const MemoryUpdateListener& listener);
    
    void addNeuromodulatorUpdateListener(const NeuromodulatorUpdateListener& listener);
    void removeNeuromodulatorUpdateListener(const NeuromodulatorUpdateListener& listener);
    
    void addBehaviorUpdateListener(const BehaviorUpdateListener& listener);
    void removeBehaviorUpdateListener(const BehaviorUpdateListener& listener);
    
    // Configuration
    void loadConfiguration(const std::string& filepath = "");
    void saveConfiguration(const std::string& filepath = "") const;
    
    // Network server (for live visualization)
    void startVisualizationServer();
    void stopVisualizationServer();
    bool isVisualizationServerRunning() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Thread safety
    mutable std::mutex mutex;
    
    // Visualization components
    std::unique_ptr<NetworkRenderer> networkRenderer;
    std::unique_ptr<SpikesRenderer> spikesRenderer;
    std::unique_ptr<MemoryRenderer> memoryRenderer;
    
    // Data storage
    std::vector<NetworkNode> currentNetworkNodes;
    std::vector<NetworkEdge> currentNetworkEdges;
    std::vector<SpikeEvent> currentSpikes;
    std::vector<MemoryNode> currentMemoryNodes;
    std::vector<MemoryConnection> currentMemoryConnections;
    std::vector<NeuromodulatorNode> currentNeuromodulators;
    std::vector<BehaviorEvent> currentBehaviors;
    
    // Visualization state
    std::vector<VisualizationTarget> enabledTargets;
    bool liveMode;
    bool serverRunning;
    int visualizationPort;
    double updateInterval;
    int windowWidth;
    int windowHeight;
    std::string layoutAlgorithm;
    
    // Listeners
    std::vector<NetworkUpdateListener> networkListeners;
    std::vector<SpikesUpdateListener> spikesListeners;
    std::vector<MemoryUpdateListener> memoryListeners;
    std::vector<NeuromodulatorUpdateListener> neuromodulatorListeners;
    std::vector<BehaviorUpdateListener> behaviorListeners;
    
    // Server thread
    void visualizationServerThread();
    std::thread serverThread;
    std::atomic<bool> serverThreadRunning;
    
    // Update management
    void updateAllRenderers();
    void notifyListeners();
    
    // Configuration management
    void loadDefaultConfiguration();
    void saveDefaultConfiguration() const;
};

} // namespace nlm
