// NLM Interactive Visualization System
// Provides real-time visualization of NLM brain simulation and experiment results

#ifndef NLM_VISUALIZATION_HPP
#define NLM_VISUALIZATION_HPP

#include "brain/Brain.hpp"
#include "environment/Environment.hpp"
#include "agent/AgentBrain.hpp"
#include "runtime/RuntimeInspector.hpp"
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <thread>
#include <atomic>
#include <chrono>

namespace nlm {

// Visualization Data Types
enum class VisualizationType {
    None,
    Simple2D,
    3DGl,
    WebGL,
    Terminal,
    CSV,
    JSON,
    Custom
};

enum class VisualizationMode {
    Live,
    Recorded,
    RealTime,
    Simulation,
    Experiment
};

enum class VisualizationElement {
    Neurons,
    Synapses,
    Regions,
    Populations,
    Activity,
    Plasticity,
    Memory,
    Neuromodulation,
    Cognition,
    Environment,
    Metrics,
    TimeSeries,
    Graphs,
    Heatmap,
    Custom
};

// Visualization Configuration
struct VisualizationConfig {
    std::string name;
    VisualizationType type;
    VisualizationMode mode;
    double updateInterval;
    size_t maxDataPoints;
    bool showLegend;
    bool showGrid;
    bool showLabels;
    bool animate;
    std::string backgroundColor;
    std::string foregroundColor;
    double width;
    double height;
    std::vector<VisualizationElement> elements;
    std::map<std::string, std::string> customSettings;
    
    VisualizationConfig() : type(VisualizationType::Simple2D), mode(VisualizationMode::Live),
                           updateInterval(0.1), maxDataPoints(1000), showLegend(true),
                           showGrid(true), showLabels(true), animate(true),
                           backgroundColor("#000000"), foregroundColor("#FFFFFF"),
                           width(800), height(600) {}
};

// Visualization Data
struct VisualizationData {
    SimulationStep step;
    std::chrono::duration<double> timestamp;
    
    // Neural data
    std::vector<float> neuronActivity;
    std::vector<std::vector<float>> synapticWeights;
    std::vector<size_t> regionPopulations;
    std::vector<float> firingRates;
    
    // Memory data
    std::vector<size_t> memoryContents;
    std::vector<float> memoryStrengths;
    
    // Plasticity data
    std::vector<float> plasticityRates;
    std::vector<std::pair<size_t, size_t>> changedConnections;
    
    // Cognitive data
    std::vector<std::string> activeConcepts;
    std::vector<size_t> planActivations;
    std::vector<float> decisionConfidences;
    
    // Neuromodulation data
    float dopamineLevel;
    float curiosityLevel;
    float noveltyLevel;
    float predictionError;
    
    // Environmental data
    std::vector<std::vector<int>> environmentGrid;
    std::vector<std::string> objectTypes;
    
    // Metrics
    std::map<std::string, double> metrics;
    std::map<std::string, std::vector<double>> timeSeriesMetrics;
    
    VisualizationData() : step(0), dopamineLevel(0.0f), curiosityLevel(0.0f),
                         noveltyLevel(0.0f), predictionError(0.0f) {}
};

// Visualization Output Formats
enum class OutputFormat {
    Text,
    JSON,
    CSV,
    HTML,
    SVG,
    PNG,
    Video,
    WebSocket,
    Custom
};

// Visualization Element Renderer Interface
class VisualizationElementRenderer {
public:
    virtual ~VisualizationElementRenderer() = default;
    
    virtual void render(const VisualizationData& data, 
                       const VisualizationConfig& config) = 0;
    
    virtual std::string getName() const = 0;
    virtual std::vector<VisualizationElement> getSupportedElements() const = 0;
    virtual VisualizationType getType() const = 0;
};

// 2D Renderer
class Simple2DRenderer : public VisualizationElementRenderer {
public:
    void render(const VisualizationData& data, 
                const VisualizationConfig& config) override;
    
    std::string getName() const override { return "Simple 2D Renderer"; }
    std::vector<VisualizationElement> getSupportedElements() const override {
        return {VisualizationElement::Neurons, VisualizationElement::Synapses,
                VisualizationElement::Regions, VisualizationElement::Activity,
                VisualizationElement::Plasticity, VisualizationElement::Memory,
                VisualizationElement::Environment, VisualizationElement::Metrics};
    }
    VisualizationType getType() const override { return VisualizationType::Simple2D; }
};

// 3D Renderer (OpenGL)
class OpenGL3DRenderer : public VisualizationElementRenderer {
public:
    void render(const VisualizationData& data,
                const VisualizationConfig& config) override;
    
    std::string getName() const override { return "OpenGL 3D Renderer"; }
    std::vector<VisualizationElement> getSupportedElements() const override {
        return {VisualizationElement::Neurons, VisualizationElement::Synapses,
                VisualizationElement::Regions, VisualizationElement::Populations,
                VisualizationElement::Activity, VisualizationElement::Plasticity,
                VisualizationElement::Cognition, VisualizationElement::Neuromodulation,
                VisualizationElement::Custom};
    }
    VisualizationType getType() const override { return VisualizationType::3DGl; }
};

// WebGL Renderer
class WebGLRenderer : public VisualizationElementRenderer {
public:
    void render(const VisualizationData& data,
                const VisualizationConfig& config) override;
    
    std::string getName() const override { return "WebGL Renderer"; }
    std::vector<VisualizationElement> getSupportedElements() const override {
        return {VisualizationElement::Neurons, VisualizationElement::Synapses,
                VisualizationElement::Regions, VisualizationElement::Activity,
                VisualizationElement::Plasticity, VisualizationElement::Memory,
                VisualizationElement::Neuromodulation, VisualizationElement::Prediction,
                VisualizationElement::Custom};
    }
    VisualizationType getType() const override { return VisualizationType::WebGL; }
};

// Terminal Renderer
class TerminalRenderer : public VisualizationElementRenderer {
public:
    void render(const VisualizationData& data,
                const VisualizationConfig& config) override;
    
    std::string getName() const override { return "Terminal Renderer"; }
    std::vector<VisualizationElement> getSupportedElements() const override {
        return {VisualizationElement::Neurons, VisualizationElement::Activity,
                VisualizationElement::Metrics, VisualizationElement::TimeSeries};
    }
    VisualizationType getType() const override { return VisualizationType::Terminal; }
};

// Main Visualization System
class VisualizationSystem {
public:
    VisualizationSystem() = default;
    ~VisualizationSystem() = default;
    
    // Configuration management
    void setConfig(const VisualizationConfig& config) { currentConfig = config; }
    const VisualizationConfig& getConfig() const { return currentConfig; }
    
    // Visualization control
    void start() { isRunning = true; }
    void stop() { isRunning = false; }
    bool isRunning() const { return isRunning; }
    
    void setTarget(std::shared_ptr<Brain> brain) { targetBrain = brain; }
    void setTarget(std::shared_ptr<Environment> environment) { targetEnvironment = environment; }
    void setTarget(std::shared_ptr<AgentBrain> agent) { targetAgent = agent; }
    
    // Rendering
    void update();
    void render();
    std::string getLastFrame() const { return lastFrame; }
    
    // Configuration
    void setType(VisualizationType type) { currentConfig.type = type; }
    void setMode(VisualizationMode mode) { currentConfig.mode = mode; }
    void setOutputFormat(OutputFormat format) { outputFormat = format; }
    
    void setUpdateInterval(double interval) { currentConfig.updateInterval = interval; }
    void setDimensions(double width, double height) { 
        currentConfig.width = width; 
        currentConfig.height = height; 
    }
    
    // Elements
    void addElement(VisualizationElement element) {
        currentConfig.elements.push_back(element);
    }
    
    void removeElement(VisualizationElement element) {
        currentConfig.elements.erase(
            std::remove(currentConfig.elements.begin(), 
                       currentConfig.elements.end(), element),
            currentConfig.elements.end());
    }
    
    bool hasElement(VisualizationElement element) const {
        return std::find(currentConfig.elements.begin(), 
                        currentConfig.elements.end(), element) != currentConfig.elements.end();
    }
    
    // Output
    bool saveFrame(const std::string& filename) const;
    bool saveVideo(const std::string& filename, double fps) const;
    bool exportData(const std::string& filename) const;
    
    // Callbacks
    void setDataCallback(std::function<VisualizationData(std::shared_ptr<Brain> brain)> callback) {
        dataCallback = callback;
    }
    
    void setRenderCallback(std::function<std::string(const VisualizationData&)> callback) {
        renderCallback = callback;
    }
    
    // Control
    void pause() { isPaused = true; }
    void resume() { isPaused = false; }
    bool isPaused() const { return isPaused; }
    
    void setSpeed(double speed) { renderSpeed = speed; }
    double getSpeed() const { return renderSpeed; }
    
    // History
    void enableRecording(bool enable) { recordData = enable; }
    bool isRecording() const { return recordData; }
    
    std::vector<VisualizationData> getHistory() const { return historyData; }
    void clearHistory() { historyData.clear(); }
    
    // Runtime integration
    void integrateWithInspector(std::shared_ptr<RuntimeInspector> inspector) {
        runtimeInspector = inspector;
    }
    
    // Factory methods
    static std::unique_ptr<VisualizationElementRenderer> createRenderer(VisualizationType type);
    static VisualizationConfig getDefaultConfig(VisualizationType type);
    
    // Utility methods
    static std::string generateID() {
        return "vis_" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count());
    }

private:
    VisualizationConfig currentConfig;
    OutputFormat outputFormat = OutputFormat::Text;
    
    // Targets
    std::shared_ptr<Brain> targetBrain;
    std::shared_ptr<Environment> targetEnvironment;
    std::shared_ptr<AgentBrain> targetAgent;
    
    // State
    bool isRunning = false;
    bool isPaused = false;
    double renderSpeed = 1.0;
    bool recordData = false;
    
    // Data and rendering
    VisualizationData currentFrameData;
    std::string lastFrame;
    std::vector<VisualizationData> historyData;
    
    // Runtime integration
    std::shared_ptr<RuntimeInspector> runtimeInspector;
    
    // Callbacks
    std::function<VisualizationData(std::shared_ptr<Brain> brain)> dataCallback;
    std::function<std::string(const VisualizationData&)> renderCallback;
    
    // Rendering
    std::unique_ptr<VisualizationElementRenderer> createRendererInstance() const;
    
    // Data collection
    VisualizationData collectData() const;
    VisualizationData collectFromInspector() const;
    VisualizationData collectFromBrain() const;
    VisualizationData collectFromEnvironment() const;
    VisualizationData collectFromAgent() const;
    
    // Frame generation
    std::string generateFrame(const VisualizationData& data) const;
    std::string renderToText(const VisualizationData& data) const;
    std::string renderToJSON(const VisualizationData& data) const;
    std::string renderToCSV(const VisualizationData& data) const;
    std::string renderToHTML(const VisualizationData& data) const;
}; 

// WebSocket Visualization Server
class WebSocketVisualizationServer {
public:
    WebSocketVisualizationServer(unsigned short port = 8080);
    ~WebSocketVisualizationServer();
    
    void start();
    void stop();
    bool isRunning() const { return running; }
    
    void setVisualizationSystem(std::shared_ptr<VisualizationSystem> system) {
        visualizationSystem = system;
    }
    
    void broadcastFrame(const std::string& frame) const;
    
    // Configuration
    void setPort(unsigned short port) { this->port = port; }
    unsigned short getPort() const { return port; }
    
    void setHost(const std::string& host) { this->host = host; }
    std::string getHost() const { return host; }
    
private:
    unsigned short port;
    std::string host;
    bool running;
    std::shared_ptr<VisualizationSystem> visualizationSystem;
    
    // WebSocket implementation details
    void setupWebSocket();
    void handleConnections();
    void handleMessages();
};

// Convenience functions
namespace Visualization {
    // Quick visualization setup
    void createDefaultVisualization();
    
    // Preset configurations
    VisualizationConfig getMinimalConfig();
    VisualizationConfig getStandardConfig();
    VisualizationConfig getAdvancedConfig();
    
    // Common renderers
    std::unique_ptr<VisualizationElementRenderer> createTextRenderer();
    std::unique_ptr<VisualizationElementRenderer> createTerminalRenderer();
    std::unique_ptr<VisualizationElementRenderer> createWebGLRenderer();
    
    // Output utilities
    void saveToFile(const std::string& filename, const std::string& data);
    void saveFrame(const std::string& filename, const std::string& frameData);
    void saveVideo(const std::string& filename, const std::vector<std::string>& frames, 
                  double fps = 30.0);
    
    // Control functions
    void enableAutoUpdate(bool enable);
    bool isAutoUpdateEnabled();
    
    void setVisualizationTarget(std::shared_ptr<Brain> brain);
    void setVisualizationTarget(std::shared_ptr<Environment> environment);
    void setVisualizationTarget(std::shared_ptr<AgentBrain> agent);
    
    // Inspection integration
    void enableRuntimeInspection(bool enable);
    bool isRuntimeInspectionEnabled();
}

} // namespace nlm

#endif // NLM_VISUALIZATION_HPP
