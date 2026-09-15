#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <array>
#include <algorithm>
#include <functional>
#include <cmath>
#include <deque>
#include <atomic>
#include <mutex>

#include "../core/Types/Types.hpp"

namespace nlm {

// Forward declarations
class Brain;
class SpikeEvent;
class Neuromodulator;
class SensoryInput;

// Visualization data structures
struct NeuronVisualizationData {
    NeuronId id;
    NeuronType type;
    float x, y;          // Position for 2D/3D display
    float z;             // 3D depth
    float membranePotential;
    float firingRate;
    uint32_t spikeCount;
    bool isActive;
    bool isFiring;
    bool isRefractory;
    std::array<float, 4> color;  // RGBA
    
    NeuronVisualizationData()
        : id(), type(NeuronType::Internal), x(0), y(0), z(0), membranePotential(0),
          firingRate(0), spikeCount(0), isActive(false), isFiring(false), 
          isRefractory(false) {
        std::fill(color.begin(), color.end(), 0.5f);
    }
};

struct SynapseVisualizationData {
    SynapseId id;
    NeuronId preNeuron;
    NeuronId postNeuron;
    float weight;
    float maxWeight;
    float minWeight;
    float effectiveWeight;  // Dynamic weight (plastic)
    float length;           // For 3D rendering
    std::array<float, 4> color;  // RGBA based on weight/type
    bool isPlastic;
    bool isActive;
    Timestamp lastUpdate;
    
    SynapseVisualizationData()
        : id(), preNeuron(), postNeuron(), weight(0), maxWeight(1.0f), minWeight(-1.0f),
          effectiveWeight(0), length(0), isPlastic(false), isActive(false), 
          lastUpdate(0) {
        std::fill(color.begin(), color.end(), 0.5f);
    }
};

struct SpikeVisualizationData {
    Timestamp timestamp;
    NeuronId neuronId;
    std::array<float, 4> color;
    
    SpikeVisualizationData() : timestamp(0), neuronId(), color(0, 1, 0, 1) {}
};

struct NeuromodulatorVisualizationData {
    enum Type { Dopamine, Curiosity, PredictionError, Novelty };
    Type type;
    float amplitude;
    Timestamp timestamp;
    std::array<float, 4> color;
    std::string label;
    
    NeuromodulatorVisualizationData()
        : type(Dopamine), amplitude(0), timestamp(0) {
        color = {0.2f, 0.8f, 0.2f, 0.8f};
        label = "Neuromodulator";
    }
};

struct MemoryVisualizationData {
    enum Type { WorkingMemory, EpisodicMemory, AssociativeMemory };
    Type type;
    std::string content;
    Timestamp timestamp;
    std::array<float, 4> color;
    float strength;
    
    MemoryVisualizationData()
        : type(WorkingMemory), content(""), timestamp(0), strength(0) {
        color = {0.8f, 0.8f, 0.0f, 0.8f};
    }
};

struct DevelopmentStageVisualizationData {
    DevelopmentalStage stage;
    float plasticity;
    float maturation;
    std::array<float, 4> color;
    std::string description;
    
    DevelopmentStageVisualizationData()
        : stage(DevelopmentalStage::Initial), plasticity(0), maturation(0) {
        color = {0.7f, 0.4f, 0.9f, 0.8f};
        description = "Initial";
    }
};

// Panel information structure
struct PanelInfo {
    PanelType type;
    std::string title;
    float x, y, width, height;
    bool visible;
    
    PanelInfo() : type(PanelType::NetworkTopology), title(""), x(0), y(0), width(0), height(0), visible(true) {}
};

// Export formats
enum class ExportFormat {
    PNG,
    CSV,
    JSON,
    SVG,
    Video
};

// Interactive elements
struct UIElement {
    std::string id;
    std::string label;
    float x, y, width, height;
    std::function<void()> callback;
    bool enabled;
    std::array<float, 4> color;
    
    UIElement() : x(0), y(0), width(0), height(0), enabled(true) {
        std::fill(color.begin(), color.end(), 0.8f);
    }
};

// Coordinate transformation utilities
struct CoordinateTransformer {
    float zoom;
    float panX, panY;
    float minX, maxX, minY, maxY;
    float centerX, centerY;
    
    CoordinateTransformer() : zoom(1.0f), panX(0), panY(0), centerX(0), centerY(0) {
        minX = -100; maxX = 100;
        minY = -100; maxY = 100;
    }
    
    void updateLimits(size_t neuronCount) {
        if (neuronCount > 100) {
            minX = -10; maxX = 10;
            minY = -10; maxY = 10;
            centerX = 0; centerY = 0;
        } else {
            minX = -50; maxX = 50;
            minY = -50; maxY = 50;
            centerX = 0; centerY = 0;
        }
    }
    
    float worldToScreenX(float worldX) const {
        return (worldX - minX) / (maxX - minX) * 800;
    }
    
    float worldToScreenY(float worldY) const {
        return 600 - ((worldY - minY) / (maxY - minY) * 600);
    }
};

class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Initialize visualization
    bool initialize(const std::string& title = "NLM Brain Visualization", 
                   int width = 1280, int height = 720);
    
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
    
    // Network visualization
    void visualizeNetwork(const Brain& brain);
    
    // Activity visualization
    void visualizeActivity(const Brain& brain);
    
    // Spike raster plot
    void visualizeSpikes(const std::vector<SpikeEvent>& spikes);
    
    // Weight matrix visualization
    void visualizeWeights(const Brain& brain);
    
    // Real-time monitoring and data access
    void addNeuronData(const NeuronVisualizationData& data);
    void addSynapseData(const SynapseVisualizationData& data);
    void addSpikeData(const SpikeVisualizationData& data);
    void addNeuromodulatorData(const NeuromodulatorVisualizationData& data);
    void addMemoryData(const MemoryVisualizationData& data);
    void addDevelopmentStageData(const DevelopmentStageVisualizationData& data);
    
    // Interactive controls
    void enableInteractiveMode(bool enable);
    bool isInteractiveModeEnabled() const;
    void handleMouseClick(float x, float y);
    void handleMouseMove(float x, float y);
    void handleMouseWheel(float delta);
    void handleKeyPress(int key, int action);
    
    // Panel management
    void addPanel(PanelType type, const std::string& title, float x, float y, float width, float height);
    void removePanel(PanelType type);
    void clearPanels();
    void updatePanelLayout();
    
    // Time series for neural activity
    void addFiringRateData(NeuronId neuronId, float rate, Timestamp timestamp);
    void addSpikeCountData(NeuronId neuronId, uint32_t count, Timestamp timestamp);
    void addWeightChangeData(NeuronId pre, NeuronId post, float oldWeight, float newWeight, Timestamp timestamp);
    
    // Export capabilities
    bool exportImage(const std::string& filepath, ExportFormat format = ExportFormat::PNG);
    bool exportData(const std::string& filepath, ExportFormat format = ExportFormat::CSV);
    void setExportPath(const std::string& path);
    
    // Simulation control integration
    void syncWithSimulation(bool enabled);
    void setSimulationSpeed(float speed);
    void pauseSimulation(bool pause);
    bool isSimulationPaused() const;
    
    // Highlighting and annotations
    void highlightActiveNeurons(const std::vector<NeuronId>& neuronIds);
    void highlightLearningEvents(const std::vector<std::pair<NeuronId, NeuronId>>& learningPairs);
    void clearHighlights();
    
    // Debug and analysis tools
    void showDebugInfo(bool show);
    void showPerformanceMetrics(bool show);
    void showResourceUsage(bool show);
    void showConfigurationValidation(bool show);
    
    // Multi-panel displays
    void createMultiPanelDisplay(const std::vector<PanelType>& panels);
    void updateTimeSeries(const std::string& seriesId, const std::vector<float>& values);
    
    // Scientific visualization
    void showHeatmap(const std::string& title, const std::vector<std::vector<float>>& data);
    void show3DProjection(const std::vector<std::vector<float>>& coordinates, 
                         const std::vector<NeuronId>& neuronIds);
    void showStatisticalAnalysis(const std::string& title, const std::string& analysisType);
    
    // Scientific visualization helpers
    void showLearningCurve(const std::vector<float>& rewards, const std::vector<float>& errors);
    void showSpikeTrainAlignment(const std::vector<Timestamp>& spikeTimes, 
                                const std::string& label);
    
    // Configuration
    void configure(const std::string& key, const std::string& value);
    std::string getConfiguration(const std::string& key) const;
    
    // Performance monitoring
    float getFPS() const;
    float getRenderTime() const;
    size_t getActiveNeuronCount() const;
    size_t getTotalSpikeCount() const;
    
    // State management
    void resetVisualization();
    void clearAllData();
    
    // Error handling and logging
    void logVisualizationError(const std::string& message);
    void logVisualizationWarning(const std::string& message);
    
    // Singleton pattern for easy access
    static VisualizationInterface& getInstance();
    static void destroyInstance();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Prevent copying
    VisualizationInterface(const VisualizationInterface&) = delete;
    VisualizationInterface& operator=(const VisualizationInterface&) = delete;
};

} // namespace nlm
