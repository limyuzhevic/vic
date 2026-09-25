#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <thread>
#include <mutex>

namespace nlm {

// Forward declarations
class Brain;
class NeuralRegion;
class Neuron;
class Synapse;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class Neuromodulator;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class DevelopmentSystem;
class SpikeEvent;

/**
 * Visualization modes for the NLM system
 */
enum class VisualizationMode {
    Text,          // Simple text-based visualization
    TextAdvanced,  // Advanced text visualization with more details
    Graphical,     // Graphical visualization (requires GUI libraries)
    WebSocket,     // WebSocket streaming for web visualization
    PythonAPI,     // Python API integration
    Compact       // Compact text display
};

/**
 * Python bindings for visualization tools
 */
void bindVisualization(pybind11::module_& m);

/**
 * Real-time brain state visualization
 */
class VisualizationInterface {
public:
    VisualizationInterface(VisualizationMode mode = VisualizationMode::Text);
    ~VisualizationInterface();
    
    // Initialize visualization with configuration
    bool initialize(const std::string& title = "NLM Phase 6 Visualization", 
                   int width = 120, int height = 80);
    
    // Update visualization data
    void update(const class Brain& brain, Timestamp timestamp);
    
    // Render current state
    void render();
    
    // Close visualization
    void close();
    
    // Check if visualization is active
    bool isActive() const;
    
    // Set visualization mode
    void setMode(VisualizationMode mode);
    VisualizationMode getMode() const;
    
    // Set update rate
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    
    // Toggle features
    void toggleNetworkVisualization() { networkVisible = !networkVisible; }
    void toggleActivityVisualization() { activityVisible = !activityVisible; }
    void toggleMemoryVisualization() { memoryVisible = !memoryVisible; }
    void toggleNeuromodulationVisualization() { neuromodulationVisible = !neuromodulationVisible; }
    void togglePredictionVisualization() { predictionVisible = !predictionVisible; }
    void toggleDevelopmentVisualization() { developmentVisible = !developmentVisible; }
    
    // Get current metrics for visualization
    struct VisualizationMetrics {
        float totalReward = 0.0f;
        float avgFiringRate = 0.0f;
        float avgSynapticWeight = 0.0f;
        float noveltyLevel = 0.0f;
        float curiosityLevel = 0.0f;
        float dopamineLevel = 0.0f;
        float workingMemoryActiveTraces = 0.0f;
        float episodicMemoryEpisodes = 0.0f;
        float predictionConfidence = 0.0f;
        float developmentStage = 0.0f;
        float simulationTime = 0.0f;
        float wallClockTime = 0.0f;
        bool memoryIntegrated = false;
        bool episodicMemoryIntegrated = false;
        bool neuromodulationIntegrated = false;
        bool predictionIntegrated = false;
        bool developmentIntegrated = false;
        bool checkpointingWorks = false;
        bool replayWorks = false;
    };
    
    VisualizationMetrics getCurrentMetrics() const { return currentMetrics; }
    
    // Visualization for specific systems
    void visualizeNetwork(const Brain& brain);
    void visualizeActivity(const Brain& brain);
    void visualizeSpikes(const std::vector<SpikeEvent>& spikes);
    void visualizeWeights(const Brain& brain);
    void visualizeMemory(const Brain& brain);
    void visualizeNeuromodulation(const Brain& brain);
    void visualizePrediction(const Brain& brain);
    void visualizeDevelopment(const Brain& brain);
    void visualizeIntegration(const Brain& brain);
    
    // Export visualization data
    void exportData(const std::string& filepath) const;
    void exportMetrics(const std::string& filepath) const;
    
    // Configuration
    struct VisualizationConfig {
        std::string windowTitle = "NLM Phase 6 Visualization";
        int width = 120;
        int height = 80;
        bool enableTextVisualization = true;
        bool enableGraphicalVisualization = false;
        bool enableWebSocket = false;
        int webSocketPort = 8080;
        std::string websocketHost = "localhost";
        bool enableExport = true;
        std::string exportFormat = "json";
        std::string exportPath = "./visualization_output";
        bool autoSave = false;
        double updateInterval = 1.0 / 30.0; // 30 Hz
        bool showHelp = true;
        bool showPerformance = true;
        std::string colorScheme = "default";
    };
    
    void setConfig(const VisualizationConfig& config) { config_ = config; }
    const VisualizationConfig& getConfig() const { return config_; }
    
    // Command-line interface
    static std::string getCommandLineHelp();
    static bool parseCommandLine(int argc, char* argv[], VisualizationConfig& config);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    VisualizationMode currentMode;
    mutable std::mutex updateMutex;
    
    // Visualization data
    VisualizationMetrics currentMetrics;
    Timestamp lastUpdateTime;
    double updateRate;
    
    // Feature toggles
    bool networkVisible;
    bool activityVisible;
    bool memoryVisible;
    bool neuromodulationVisible;
    bool predictionVisible;
    bool developmentVisible;
    bool integrationVisible;
    
    // Configuration
    VisualizationConfig config_;
    
    // Internal methods
    void updateData(const Brain& brain, Timestamp timestamp);
    void renderText() const;
    void renderTextAdvanced() const;
    void renderGraphical() const;
    void renderWebSocket() const;
    void renderPythonAPI() const;
    void renderCompact() const;
    
    // Data collection methods
    void collectBrainMetrics(const Brain& brain);
    void collectMemoryMetrics(const Brain& brain);
    void collectNeuromodulationMetrics(const Brain& brain);
    void collectPredictionMetrics(const Brain& brain);
    void collectDevelopmentMetrics(const Brain& brain);
};

} // namespace nlm