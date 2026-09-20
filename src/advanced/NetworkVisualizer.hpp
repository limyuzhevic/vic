#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <map>
#include <array>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

// Visualization types
enum class VisualizationType {
    NetworkTopology,
    SpikeRaster,
    Heatmap,
    ConnectivityMatrix,
    PopulationActivity,
    SynapticWeightDistribution,
    SpikePatternAnalysis,
    3DNetworkView,
    DynamicConnectivity,
    CustomVisualization
};

// Color schemes for visualization
enum class ColorScheme {
    Default,
    Scientific,
    Rainbow,
    Grayscale,
    Viridis,
    Plasma,
    Hot,
    Cool,
    Custom
};

// Visualization layout types
enum class LayoutType {
    Circular,
    Radial,
    Grid,
    ForceDirected,
    Hierarchical,
    Geodesic,
    3DSpring,
    Custom
};

// Quality levels for rendering
enum class QualityLevel {
    Low,
    Medium,
    High,
    Ultra,
    Custom
};

// Rendering modes
enum class RenderMode {
    Points,
    Lines,
    Filled,
    Outlined,
    Transparent,
    Billboards,
    Wireframe,
    Custom
};

// Color mapping strategies
enum class ColorMapping {
    Identity,
    Linear,
    Logarithmic,
    Exponential,
    Categorical,
    Sequential,
    Diverging,
    Custom
};

// Coordinate system for 2D/3D visualization
struct Coordinate {
    float x, y, z;
    Coordinate() : x(0), y(0), z(0) {}
    Coordinate(float xx, float yy, float zz) : x(xx), y(yy), z(zz) {}
};

// Neuron visualization properties
struct NeuronVisualization {
    NeuronId id;
    Coordinate position;
    float radius;
    std::string color;
    float opacity;
    bool highlight;
    std::string label;
    std::vector<std::string> properties;  // Additional properties for tooltips
};

// Edge visualization properties  
struct EdgeVisualization {
    SynapseId id;
    NeuronId sourceId;
    NeuronId destinationId;
    float weight;
    std::string color;
    float opacity;
    float width;
    std::string style;  // solid, dashed, dotted
    bool highlight;
    float flowAnimation;  // 0-1 for flow animation
};

// Time series data for activity visualization
struct TimeSeriesData {
    double timestamp;  // ms
    std::vector<float> values;  // Activity values for each neuron/region
    std::string label;
};

// Spike event visualization
struct SpikeVisualization {
    NeuronId neuronId;
    SimulationStep step;
    Timestamp time;
    float amplitude;
    std::string color;
    float size;
};

// Heatmap data
struct HeatmapData {
    std::vector<std::vector<float>> values;  // 2D grid of activity
    float minValue;
    float maxValue;
    std::string title;
    std::string units;
};

// Network clustering information
struct ClusterInfo {
    size_t id;
    std::vector<NeuronId> members;
    float density;
    float modularity;
    float averageDegree;
    std::string color;
};

// NetworkVisualizer: Real-time network visualization and analysis
class NetworkVisualizer {
public:
    NetworkVisualizer(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config);
    ~NetworkVisualizer();
    
    // Core visualization functionality
    bool initialize();
    void shutdown();
    bool isInitialized() const;
    void update();
    void render();
    
    // Visualization control
    void setVisualizationType(VisualizationType type);
    void setColorScheme(ColorScheme scheme);
    void setUpdateRate(double hz);
    void setMaxHistorySize(size_t size);
    void toggleAnimation(bool enable);
    bool isAnimationEnabled() const;
    
    // Data collection and processing
    void collectBrainData(const Brain& brain);
    void addTimeSeriesData(const TimeSeriesData& data);
    void addSpikeVisualization(const SpikeVisualization& spike);
    void clearTimeSeriesData();
    void clearSpikeData();
    
    // Advanced visualization features
    void visualizeNetworkTopology(const Brain& brain);
    void visualizeSpikeRaster(const std::vector<SpikeVisualization>& spikes);
    void visualizeHeatmap(const HeatmapData& heatmap);
    void visualizeConnectivityMatrix(const Brain& brain);
    void visualizePopulationActivity(const Brain& brain);
    void visualizeSynapticWeights(const Brain& brain);
    void analyzeSpikePatterns(const Brain& brain);
    void create3DNetworkView(const Brain& brain);
    void visualizeDynamicConnectivity(const Brain& brain);
    
    // Network analysis
    std::vector<ClusterInfo> performClustering(const Brain& brain);
    std::vector<std::vector<NeuronId>> findCommunities(const Brain& brain);
    float calculateClusteringCoefficient(const Brain& brain, NeuronId neuronId);
    float calculatePathLength(const Brain& brain);
    float calculateBetweennessCentrality(const Brain& brain, NeuronId neuronId);
    std::vector<NeuronId> identifyHubNeurons(const Brain& brain, float threshold);
    
    // Export and import
    bool saveVisualization(const std::string& filepath, const std::string& format = "png");
    bool loadVisualization(const std::string& filepath);
    bool exportData(const std::string& filepath, const std::string& dataType = "csv");
    
    // Configuration
    void configureLayout(LayoutType layout);
    void setNodeSizeScale(float scale);
    void setEdgeWidthScale(float scale);
    void setColorMapping(const std::string& metric, ColorMapping mapping);
    void addCustomVisualization(const std::string& name, std::function<void()> renderFunc);
    
    // Event handling
    void onWindowResize(int width, int height);
    void onMouseClick(int x, int y);
    void onMouseWheel(float delta);
    void onKeyPress(int key, int action);
    
    // Analysis and reporting
    std::string generateTopologyReport() const;
    std::string generateSpikeAnalysisReport() const;
    std::string generateConnectivityReport() const;
    std::string generateCommunityReport() const;
    std::string getVisualizationStatus() const;
    
    // Power user controls
    void enableAdvancedFeatures(bool enable);
    void setQualityLevel(QualityLevel level);
    void setRenderMode(RenderMode mode);
    void addCustomFilter(std::function<bool(const NeuronVisualization&)> filter);
    
    // Integration
    void attachToBrain(std::shared_ptr<Brain> brain);
    void detachFromBrain();
    bool isAttached() const;
    
    // Cleanup
    void cleanup() noexcept;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
