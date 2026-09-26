#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <thread>
#include <atomic>
#include <functional>
#include <chrono>
#include "core/Types/Types.hpp"
#include "visualization/VisualizationInterface.hpp"

namespace nlm {

// Visualization update priority levels
enum class UpdatePriority {
    Low = 0,
    Normal = 1,
    High = 2,
    Critical = 3
};

// Visualization data buffer
struct VisualizationData {
    SimulationStep step;
    Timestamp timestamp;
    std::vector<NeuronId> activeNeurons;
    std::vector<MembranePotential> membranePotentials;
    std::vector<float> synapticWeights;
    std::vector<SpikeEvent> spikeEvents;
    std::vector<Timestamp> spikeTimestamps;
    std::map<SynapseId, float> connectionWeights;
    std::map<NeuronId, std::vector<Timestamp>> neuronSpikes;
    bool isDirty;
    
    VisualizationData() : step(0), timestamp(0.0), isDirty(false) {}
    
    void clear() {
        activeNeurons.clear();
        membranePotentials.clear();
        synapticWeights.clear();
        spikeEvents.clear();
        spikeTimestamps.clear();
        connectionWeights.clear();
        neuronSpikes.clear();
        isDirty = false;
    }
};

// Visualization render command
struct RenderCommand {
    UpdatePriority priority;
    std::function<void()> command;
    SimulationStep timestamp;
    std::string id;
    
    RenderCommand(UpdatePriority p, std::function<void()> cmd, SimulationStep ts, const std::string& cmdId)
        : priority(p), command(cmd), timestamp(ts), id(cmdId) {}
    
    bool operator<(const RenderCommand& other) const {
        return priority > other.priority;  // Higher priority first
    }
};

// Visualization frame buffer for smooth rendering
struct FrameBuffer {
    std::vector<VisualizationData> frames;
    size_t maxFrames;
    size_t currentIndex;
    std::atomic<bool> ready;
    std::mutex mutex;
    
    FrameBuffer(size_t maxFrames = 60) : maxFrames(maxFrames), currentIndex(0), ready(false) {
        frames.reserve(maxFrames);
    }
    
    void push(const VisualizationData& data) {
        std::lock_guard<std::mutex> lock(mutex);
        if (frames.size() < maxFrames) {
            frames.push_back(data);
        } else {
            frames[currentIndex] = data;
        }
        currentIndex = (currentIndex + 1) % maxFrames;
        ready = true;
    }
    
    VisualizationData getCurrentFrame() {
        std::lock_guard<std::mutex> lock(mutex);
        if (frames.empty()) {
            return VisualizationData();
        }
        return frames[currentIndex];
    }
};

// Main visualization engine class
class VisualizationEngine {
public:
    VisualizationEngine();
    ~VisualizationEngine();
    
    // Core engine lifecycle
    bool initialize(int width, int height, const std::string& title);
    void shutdown();
    void update(SimulationStep step, Timestamp timestamp);
    void render();
    
    // Data management
    void setBrainData(const class Brain& brain, const VisualizationData& data);
    void updateNeuronData(const class Brain& brain);
    void updateSpikeData(const class Brain& brain);
    void updateConnectionData(const class Brain& brain);
    
    // Visualizer management
    void registerVisualizer(const std::string& name, std::shared_ptr<class VisualizationInterface> visualizer);
    void unregisterVisualizer(const std::string& name);
    void setActiveVisualizer(const std::string& name);
    
    // Real-time control
    void setTargetFPS(double fps);
    void setPerformanceMode(bool highPerformance);
    void toggleFullscreen();
    
    // Event handling
    void handleMouseInput(double x, double y, bool isPressed);
    void handleKeyboardInput(int key, bool isPressed);
    void handleScrollInput(double offset);
    
    // Configuration
    void setTheme(const std::string& theme);
    void setStyle(const std::string& styleName, const std::map<std::string, std::string>& properties);
    
    // Performance monitoring
    double getCurrentFPS() const;
    uint64_t getDrawCalls() const;
    size_t getMemoryUsage() const;
    std::chrono::microseconds getRenderTime() const;
    
    // Debug and analysis
    void enableDebugMode(bool enabled);
    void captureFrame(const std::string& filename);
    void exportData(const std::string& filename, const std::string& format);
    
    // Plugin system
    void loadPlugin(const std::string& pluginPath);
    void unloadPlugin(const std::string& pluginName);
    
    // Callback system
    void addUpdateCallback(const std::function<void(SimulationStep, Timestamp)>& callback);
    void addRenderCallback(const std::function<void()>& callback);
    
    // Statistics and metrics
    struct PerformanceMetrics {
        double fps;
        double updateTime;
        double renderTime;
        size_t vertexCount;
        size_t drawCallCount;
        size_t memoryUsage;
        uint32_t activeVisualizers;
    };
    
    PerformanceMetrics getMetrics() const;
    
    // Status
    bool isInitialized() const;
    bool isRunning() const;
    std::vector<std::string> getAvailableVisualizers() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Core engine methods
    void updateRenderQueue();
    void processRenderCommands();
    void maintainFrameBuffer();
    void optimizeMemoryUsage();
    
    void initializeRenderSystem();
    void shutdownRenderSystem();
    void setupInputHandling();
    void setupPluginSystem();
    
    void loadDefaultTheme();
    void applyStyle(const std::string& styleName);
};

} // namespace nlm
