#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <mutex>
#include <chrono>
#include <atomic>

namespace nlm::visualization {

enum class VisualizationState {
    Uninitialized,
    Initializing,
    Ready,
    Running,
    Paused,
    Stopped,
    Error
};

enum class OutputFormat {
    Terminal,
    OpenGL,
    WebGL,
    HTML,
    CSV,
    JSON,
    PNG,
    Video
};

enum class VisualizerType {
    Unknown,
    BrainState,
    NeuralNetwork,
    NeuronActivity,
    SynapseDynamics,
    Memory,
    WorkingMemory,
    EpisodicMemory,
    SemanticMemory,
    Plasticity,
    STDP,
    Hebbian,
    StructuralPlasticity,
    Neuromodulation,
    Dopamine,
    Curiosity,
    Novelty,
    Development,
    Cognition,
    Prediction,
    Planning
};

struct VisualizationSettings {
    OutputFormat outputFormat = OutputFormat::Terminal;
    float updateFrequency = 30.0f; // Hz
    bool showNeurons = true;
    bool showSynapses = true;
    bool showConnections = true;
    bool showWeights = true;
    bool showActivity = true;
    bool showMemory = true;
    bool showPlasticity = true;
    bool showNeuromodulation = true;
    bool showDevelopment = true;
    bool showPredictions = true;
    bool interactive = true;
    int width = 1920;
    int height = 1080;
    float neuronSize = 1.0f;
    float synapseThickness = 0.5f;
    std::string colorScheme = "default";
    std::string fontFamily = "monospace";
    float animationSpeed = 1.0f;
    bool showGrid = false;
    bool showLegend = true;
    bool showStatistics = true;
    int maxHistoryPoints = 10000;
    bool enableCompression = true;
    bool enableStreaming = true;
    std::string dataFormat = "json";
    std::string exportDirectory = "./visualization_output";
    bool saveFrames = false;
    bool recordVideo = false;
    float videoFPS = 30.0f;
};

class VisualizationBase {
public:
    virtual ~VisualizationBase() = default;
    
    virtual const std::string& getName() const = 0;
    virtual VisualizerType getType() const = 0;
    virtual const std::string& getVersion() const = 0;
    virtual bool initialize(const VisualizationSettings& settings) = 0;
    virtual void shutdown() = 0;
    virtual VisualizationState getState() const = 0;
    virtual bool update(float timestep) = 0;
    virtual bool render() = 0;
    virtual bool configure(const std::string& key, const std::string& value) = 0;
    virtual std::string getConfiguration() const = 0;
    
    // Static methods
    static std::string getInterfaceVersion() { return "1.0.0"; }
    static std::vector<std::string> getAvailableFormats();
    static bool validateSettings(const VisualizationSettings& settings);
};

} // namespace nlm::visualization