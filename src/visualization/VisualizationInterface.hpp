#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <array>
#include <atomic>
#include <optional>

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../development/DevelopmentSystem.hpp"

// Forward declarations for visualization components
namespace nlm {

class NetworkGraphRenderer;
class ActivityProfiler;
class PlasticityTracker;
class TemporalVisualizer;
class AdvancedAnalyzer;

// Visualization configuration structure
struct VisualizationConfig {
    // Display settings
    float pointSize = 3.0f;
    float lineWidth = 1.5f;
    float fontSize = 12.0f;
    bool showLabels = true;
    bool showWeights = true;
    bool showFiringRates = true;
    bool showPlasticity = true;
    
    // Color scheme
    float excitatoryColor[3] = {0.2f, 0.8f, 0.2f};    // Green
    float inhibitoryColor[3] = {0.8f, 0.2f, 0.2f};    // Red
    float modulatoryColor[3] = {0.2f, 0.2f, 0.8f};    // Blue
    float activeColor[3] = {1.0f, 1.0f, 0.0f};        // Yellow
    float backgroundColor[3] = {0.1f, 0.1f, 0.15f};   // Dark blue-gray
    
    // Rendering modes
    bool render2D = true;
    bool render3D = false;
    bool renderNeuronStates = true;
    bool renderSynapses = true;
    bool renderConnections = true;
    
    // Performance settings
    double updateInterval = 0.033;  // 30 Hz
    size_t maxDataPoints = 10000;
    bool enableAntiAliasing = true;
    
    // Analysis parameters
    double correlationWindow = 1.0;  // seconds
    bool computeCrossCorrelation = true;
    bool computeSynchronizability = true;
    
    // Animation and interaction
    bool enableAnimation = true;
    double animationSpeed = 1.0;
    bool enableZoom = true;
    bool enablePan = true;
    bool enableRotate = false;
};

// Neural network node for visualization
struct NeuralNode {
    NeuronId id;
    RegionId regionId;
    PopulationId populationId;
    NeuronType type;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float radius = 5.0f;
    float color[3] = {0.5f, 0.5f, 0.5f};
    float intensity = 1.0f;
    float firingRate = 0.0f;
    float plasticityStrength = 0.0f;
    Timestamp lastSpike = -1.0;
    bool isActive = false;
    bool isFiring = false;
    
    NeuralNode() = default;
    NeuralNode(NeuronId nid, NeuronType t, float cx, float cy, float cz)
        : id(nid), type(t), x(cx), y(cy), z(cz) {
        updateColor();
    }
    
    void updateColor() {
        switch (type) {
            case NeuronType::Excitatory:
                color[0] = 0.2f; color[1] = 0.8f; color[2] = 0.2f;
                break;
            case NeuronType::Inhibitory:
                color[0] = 0.8f; color[1] = 0.2f; color[2] = 0.2f;
                break;
            case NeuronType::Modulatory:
                color[0] = 0.2f; color[1] = 0.2f; color[2] = 0.8f;
                break;
            case NeuronType::Sensory:
                color[0] = 0.8f; color[1] = 0.8f; color[2] = 0.2f;
                break;
            case NeuronType::Motor:
                color[0] = 0.8f; color[1] = 0.2f; color[2] = 0.8f;
                break;
            case NeuronType::Internal:
                color[0] = 0.5f; color[1] = 0.5f; color[2] = 0.5f;
                break;
        }
        
        if (isActive) {
            color[0] *= 1.5f; color[1] *= 1.5f; color[2] *= 1.5f;
        }
        if (intensity > 1.0f) {
            color[0] = std::min(color[0], 1.0f);
            color[1] = std::min(color[1], 1.0f);
            color[2] = std::min(color[2], 1.0f);
        }
    }
};

// Neural connection for visualization
struct NeuralConnection {
    SynapseId id;
    NeuronId sourceId;
    NeuronId targetId;
    SynapticWeight weight;
    Delay delay;
    SynapseType type;
    float x1 = 0.0f, y1 = 0.0f, z1 = 0.0f;
    float x2 = 0.0f, y2 = 0.0f, z2 = 0.0f;
    float opacity = 0.5f;
    float width = 1.0f;
    float color[3] = {0.3f, 0.3f, 0.3f};
    float flowSpeed = 0.0f;
    bool isActive = false;
    bool isPlastic = false;
    
    NeuralConnection() = default;
    NeuralConnection(SynapseId sid, NeuronId src, NeuronId tgt, 
                     SynapticWeight w, Delay d, SynapseType t)
        : id(sid), sourceId(src), targetId(tgt), weight(w), delay(d), type(t) {
        updateColor();
    }
    
    void updateColor() {
        switch (type) {
            case SynapseType::Excitatory:
                color[0] = 0.2f; color[1] = 0.8f; color[2] = 0.2f;
                break;
            case SynapseType::Inhibitory:
                color[0] = 0.8f; color[1] = 0.2f; color[2] = 0.2f;
                break;
            case SynapseType::Modulatory:
                color[0] = 0.2f; color[1] = 0.2f; color[2] = 0.8f;
                break;
            case SynapseType::Electrical:
                color[0] = 0.8f; color[1] = 0.8f; color[2] = 0.8f;
                break;
            case SynapseType::GapJunction:
                color[0] = 0.5f; color[1] = 0.5f; color[2] = 1.0f;
                break;
        }
        
        if (isActive) {
            color[0] *= 1.2f; color[1] *= 1.2f; color[2] *= 1.2f;
        }
        if (isPlastic) {
            color[0] *= 1.3f; color[1] *= 1.3f; color[2] *= 1.3f;
        }
    }
};

// Time-series data point
struct TimeSeriesPoint {
    Timestamp timestamp;
    float value;
    std::string label;
    float color[3] = {0.5f, 0.5f, 0.5f};
};

// Correlation analysis result
struct CorrelationResult {
    NeuronId neuron1;
    NeuronId neuron2;
    float crossCorrelation;
    float timeLag;
    CorrelationSignificance significance;
    std::string description;
};

// Network topology metrics
struct NetworkTopologyMetrics {
    size_t neuronCount;
    size_t synapseCount;
    float connectionDensity;
    float clusteringCoefficient;
    float averageDegree;
    float modularity;
    std::vector<size_t> degreeDistribution;
    std::vector<float> communitySizes;
};

// Plasticity evolution data
struct PlasticityEvolution {
    Timestamp timestamp;
    std::vector<float> weightChanges;
    std::vector<float> stdpTraces[3];  // LTP, LTD traces
    std::vector<bool> structuralEvents;
    float averageWeightChange;
    float learningRate;
};

class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize visualization with configuration
    bool initialize(const VisualizationConfig& config = VisualizationConfig());
    
    // Update visualization data from brain state
    void update(const Brain& brain);
    
    // Render current state to display
    void render();
    
    // Close visualization and cleanup
    void close();
    
    // Check if visualization is active and ready
    bool isActive() const;
    
    // Configuration management
    void setConfig(const VisualizationConfig& config);
    const VisualizationConfig& getConfig() const;
    
    // Export capabilities
    bool exportToPNG(const std::string& filepath, int width = 1920, int height = 1080);
    bool exportToSVG(const std::string& filepath);
    bool exportToPDF(const std::string& filepath);
    bool exportToHTML(const std::string& filepath);
    
    // Data access for external tools
    const std::vector<NeuralNode>& getNodes() const;
    const std::vector<NeuralConnection>& getConnections() const;
    const std::vector<TimeSeriesPoint>& getPopulationActivity() const;
    const std::vector<CorrelationResult>& getCrossCorrelations() const;
    const NetworkTopologyMetrics& getTopologyMetrics() const;
    
    // Interactive features
    void setCameraPosition(float x, float y, float z);
    void setCameraLookAt(float x, float y, float z);
    void zoom(float factor);
    void pan(float dx, float dy);
    void rotate(float dx, float dy);
    
    // Analysis control
    void enableAnalysis(const std::string& analysisType, bool enable);
    void setAnalysisParameter(const std::string& paramName, float value);
    void computeAnalysis();
    
    // Frame management
    void startFrame();
    void endFrame();
    size_t getCurrentFrame() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm