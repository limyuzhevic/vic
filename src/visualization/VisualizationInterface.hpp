#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <map>

namespace nlm {

// Visualization modes
enum class VisualizationMode {
    None,
    NetworkGraph,
    SpikeRaster,
    WeightMatrix,
    ActivityHeatmap,
    All
};

// Network visualization styles
enum class NetworkStyle {
    Circular,
    Hierarchical,
    Grid,
    Random,
    Biological
};

// Visualization data structures
struct NeuronVisualizationData {
    NeuronId id;
    float x, y;           // Position in 2D space
    float radius;         // Size based on firing rate or importance
    float color[3];       // RGB color (0-1)
    float opacity;        // Transparency
    bool highlighted;     // For emphasis
    bool selected;        // User-selected
    std::string label;    // Display label
    float firingRate;     // Current firing rate
    float timeActive;     // Total time active
};

struct SynapseVisualizationData {
    NeuronId sourceId;
    NeuronId targetId;
    float weight;         // Synaptic strength
    float opacity;        // Transparency based on weight
    bool active;          // Whether synapse is active
    float pulseTime;      // For animation effects
    float color[3];       // Color based on type/strength
};

struct SpikeVisualizationData {
    uint64_t time;        // Simulation step
    NeuronId neuronId;
    float intensity;      // Spike strength
    float duration;       // Display duration
    std::string type;     // "excitatory", "inhibitory", etc.
};

// Visualization configuration
struct VisualizationConfig {
    VisualizationMode mode = VisualizationMode::NetworkGraph;
    NetworkStyle style = NetworkStyle::Biological;
    double scale = 1.0;                 // Scale factor for rendering
    double nodeSize = 1.0;              // Base neuron size
    double edgeWidth = 0.5;             // Base synapse thickness
    bool showLabels = true;             // Display neuron/synapse labels
    bool showTooltips = true;           // Show information on hover
    std::string backgroundColor = "#000000";  // Background color
    std::string neuronColor = "#00ff00";     // Default neuron color
    std::string synapseColor = "#ffffff";    // Default synapse color
    double animationSpeed = 1.0;          // Animation speed factor
    bool enableAntialiasing = true;      // Smooth rendering
    bool enableShadows = false;          // 3D effects
    bool enableLighting = false;         // Lighting effects
};

// Performance monitoring for visualization
struct VisualizationPerformance {
    uint64_t framesRendered = 0;
    double averageFrameTime = 0.0;        // milliseconds
    double fps = 0.0;
    uint64_t memoryUsage = 0;              // bytes
    uint64_t drawCalls = 0;
    double cpuTime = 0.0;                 // milliseconds
    double gpuTime = 0.0;                 // milliseconds (if applicable)
    std::chrono::system_clock::time_point lastFrameTime;
};

// Visualization interface with comprehensive capabilities
class VisualizationInterface {
public:
    VisualizationInterface();
    ~VisualizationInterface();
    
    // Configuration and setup
    bool initialize();
    void close();
    
    // Core visualization methods
    void update();
    void render();
    
    // Visualization control
    void setMode(VisualizationMode mode);
    VisualizationMode getMode() const;
    void setStyle(NetworkStyle style);
    NetworkStyle getStyle() const;
    void setConfig(const VisualizationConfig& config);
    const VisualizationConfig& getConfig() const;
    
    // Data management
    void clearData();
    void setNetworkData(const std::vector<NeuronVisualizationData>& neurons,
                       const std::vector<SynapseVisualizationData>& synapses);
    void addSpikeData(const SpikeVisualizationData& spike);
    void addActivityData(const std::vector<float>& activityProfile);
    
    // Brain integration
    void updateFromBrain(const class Brain& brain, double timestep);
    void visualizeBrain(const class Brain& brain);
    void visualizeBrainState(const class Brain& brain, VisualizationMode mode = VisualizationMode::NetworkGraph);
    
    // Analysis and export
    void exportData(const std::string& filepath, const std::string& format = "json");
    std::string generateReport() const;
    void saveScreenshot(const std::string& filepath);
    
    // Performance monitoring
    VisualizationPerformance getPerformance() const;
    void resetPerformanceStats();
    
    // Interaction and user input
    bool isActive() const;
    void setActive(bool active);
    void handleMouseClick(double x, double y, bool controlKey = false);
    void handleKeyPress(int key, int action);
    std::vector<NeuronId> getSelectedNeurons() const;
    std::vector<SynapseVisualizationData> getSelectedSynapses() const;
    
    // Analysis tools
    void startRecording();
    void stopRecording();
    bool isRecording() const;
    std::vector<SpikeVisualizationData> getSpikeHistory() const;
    std::vector<std::vector<float>> getActivityHistory() const;
    
    // Enhanced visualization methods
    void visualizeNetworkGraph(const class Brain& brain, const VisualizationConfig& config);
    void visualizeSpikeRasterPlot(const class Brain& brain, const VisualizationConfig& config);
    void visualizeWeightMatrix(const class Brain& brain, const VisualizationConfig& config);
    void visualizeActivityHeatmap(const class Brain& brain, const VisualizationConfig& config);
    void visualizeDynamicNetwork(const class Brain& brain, double timeStep);
    
    // Advanced features
    void enableRealTimeMode(bool enabled);
    void setFrameRateLimit(int fps);
    void enableAntiAliasing(bool enabled);
    void enableShadows(bool enabled);
    void enableLighting(bool enabled);
    
    // Statistical analysis
    double calculateNetworkDensity() const;
    double calculateClusteringCoefficient() const;
    double calculateAveragePathLength() const;
    std::vector<NeuronId> findMostActiveNeurons(int count) const;
    std::vector<SynapseId> findStrongestSynapses(int count) const;
    
    // Export and import
    bool loadConfiguration(const std::string& filepath);
    bool saveConfiguration(const std::string& filepath) const;
    bool importData(const std::string& filepath);
    bool exportData(const std::string& filepath, const std::string& format) const;
    
    // Utility methods
    void resetView();
    void zoom(double factor);
    void pan(double dx, double dy);
    void rotate(double angle);
    
    // Debug and logging
    void printDebugInfo() const;
    void logVisualizationStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods
    void calculateNetworkLayout();
    void calculatePositionsUsingLayout(NetworkStyle style);
    void applyStyling();
    void updateShaders();
    void renderNetworkGraph();
    void renderSpikeRaster();
    void renderWeightMatrix();
    void renderActivityHeatmap();
    std::string generateJSONData() const;
    void parseJSONData(const std::string& jsonData);
};

// Convenience functions for common visualization tasks
void createStandardVisualization(const std::string& title, const class Brain& brain);
void createNetworkVisualization(const class Brain& brain, const std::string& outputFile = "");
void createSpikeVisualization(const std::vector<SpikeVisualizationData>& spikes, const std::string& outputFile = "");
void createWeightMatrixVisualization(const class Brain& brain, const std::string& outputFile = "");
void createActivityHeatmap(const std::vector<float>& activityProfile, const std::string& outputFile = "");

// Console-based text visualization (fallback when graphical libraries not available)
class TextVisualizer {
public:
    TextVisualizer();
    void setBrain(const class Brain& brain);
    void renderNetworkGraph();
    void renderSpikeRaster();
    void renderWeightMatrix();
    void saveToFile(const std::string& filepath) const;
    std::string getOutput() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm