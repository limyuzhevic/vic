#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <functional>
#include <array>
#include <mutex>
#include <atomic>

namespace nlm {

// Advanced visualization and metrics system for NLM research
class NLMLiveVisualization {
public:
    struct NeuralNode {
        int id = 0;
        float x = 0.0f, y = 0.0f, z = 0.0f;
        float radius = 0.0f;
        float activation = 0.0f;
        float previousActivation = 0.0f;
        std::array<float, 4> color = {1.0f, 1.0f, 1.0f, 1.0f};
        bool isSpiking = false;
        uint64_t lastSpikeTime = 0;
    };

    struct NeuralConnection {
        int fromId = 0;
        int toId = 0;
        float weight = 0.0f;
        float normalizedWeight = 0.0f;
        bool isExcitatory = true;
        float transparency = 0.5f;
    };

    struct ActivityHeatmap {
        std::vector<std::vector<float>> grid;
        int gridWidth = 0;
        int gridHeight = 0;
        float minValue = 0.0f;
        float maxValue = 0.0f;
        std::chrono::system_clock::time_point lastUpdate;
    };

    struct MemoryState {
        std::string workingMemoryContent;
        size_t workingMemoryActiveTraces = 0;
        uint32_t episodicMemoryCount = 0;
        std::string episodicMemorySnapshots;
        float memoryConsolidationProgress = 0.0f;
        std::chrono::system_clock::time_point lastUpdate;
    };

    struct NeuromodulationState {
        float dopamine = 0.0f;
        float curiosity = 0.0f;
        float novelty = 0.0f;
        float predictionError = 0.0f;
        float attention = 0.0f;
        float learningRate = 0.0f;
        std::chrono::system_clock::time_point lastUpdate;
    };

    struct DevelopmentState {
        float developmentalStage = 0.0f;
        float plasticityRate = 0.0f;
        float synapticMaturation = 0.0f;
        float myelinationProgress = 0.0f;
        std::chrono::system_clock::time_point lastUpdate;
    };

    struct SystemFlowState {
        std::string brainStepStatus;
        std::string memorySystemStatus;
        std::string neuromodulationStatus;
        std::string predictionSystemStatus;
        std::string planningSystemStatus;
        std::chrono::system_clock::time_point lastUpdate;
    };

public:
    NLMLiveVisualization();
    ~NLMLiveVisualization();
    
    // Initialize visualization system
    bool initialize(int width = 1920, int height = 1080, bool headless = false);
    
    // Update visualization data from brain state
    void update(const class Brain& brain, const class AgentBrain* agent = nullptr);
    
    // Render visualization
    void render();
    
    // Close visualization
    void close();
    
    // Check if visualization is active
    bool isActive() const;
    
    // Set update rate
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    
    // Export capabilities
    bool exportToPNG(const std::string& filepath);
    bool exportToSVG(const std::string& filepath);
    bool exportToJSON(const std::string& filepath);
    bool exportToMP4(const std::string& filepath, double fps = 30.0);
    
    // Configuration methods
    void setNeuronRenderingMode(bool showConnections, bool showActivity, bool showHeatmap);
    void setMemoryVisualizationMode(bool showWorkingMemory, bool showEpisodicMemory);
    void setNeuromodulationVisualizationMode(bool showAllNeuromodulators);
    void setDevelopmentVisualizationMode(bool showDevelopmentStages);
    void setIntegrationFlowVisualizationMode(bool showSystemFlow);
    
    // Interactive controls
    void setCameraPosition(float x, float y, float z);
    void setZoomLevel(float zoom);
    void setRotation(float yaw, float pitch, float roll);
    void setTimeScale(double timeScale);
    void togglePause(bool pause);
    bool isPaused() const;
    
    // Get current visualization data
    const std::vector<NeuralNode>& getNeuralNodes() const;
    const std::vector<NeuralConnection>& getNeuralConnections() const;
    const ActivityHeatmap& getActivityHeatmap() const;
    const MemoryState& getMemoryState() const;
    const NeuromodulationState& getNeuromodulationState() const;
    const DevelopmentState& getDevelopmentState() const;
    const SystemFlowState& getSystemFlowState() const;
    
    // Research data collection
    void captureFrame();
    const std::vector<std::chrono::system_clock::time_point>& getCaptureTimestamps() const;
    const std::vector<std::vector<NeuralNode>>& getCapturedFrames() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
