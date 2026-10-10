#pragma once

#include "Brain.hpp"
#include "ColorManager.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <cmath>

namespace nlm {

class BrainStructureRenderer {
public:
    enum class LayoutMode {
        Grid,
        Radial,
        Hierarchical,
        ForceDirected,
        Geographic,
        Custom
    };
    
    enum class ConnectionStyle {
        Straight,
        Curved,
        Directed,
        Weighted,
        ColorCoded,
        Transparency
    };
    
    struct Node {
        std::string id;
        std::string name;
        int x;
        int y;
        int radius;
        float activityLevel;
        size_t neuronCount;
        NeuralRegion* region;
        NeuralPopulation* population;
        Neuron* neuron;
        Synapse* synapse;
        
        Node() : x(0), y(0), radius(0), activityLevel(0.0f), neuronCount(0), 
                 region(nullptr), population(nullptr), neuron(nullptr), synapse(nullptr) {}
    };
    
    struct Connection {
        std::string sourceId;
        std::string targetId;
        float weight;
        bool directed;
        std::string style;
        float transparency;
        
        Connection() : weight(0.0f), directed(false), transparency(1.0f) {}
    };
    
    BrainStructureRenderer();
    ~BrainStructureRenderer() = default;
    
    void setLayoutMode(LayoutMode mode) { layoutMode_ = mode; }
    void setConnectionStyle(ConnectionStyle style) { connectionStyle_ = style; }
    void setShowNeuronLabels(bool show) { showNeuronLabels_ = show; }
    void setShowConnectionWeights(bool show) { showConnectionWeights_ = show; }
    void setShowRegionBoundaries(bool show) { showRegionBoundaries_ = show; }
    void setZoomLevel(float zoom) { zoomLevel_ = std::max(0.1f, std::min(5.0f, zoom)); }
    void setPanOffset(int x, int y) { panOffsetX_ = x; panOffsetY_ = y; }
    
    void render(const Brain& brain);
    void updateLayout(const Brain& brain);
    void renderNodes(const Brain& brain);
    void renderConnections(const Brain& brain);
    void renderLabels(const Brain& brain);
    
    // Node rendering
    void renderNeuron(const Node& neuron, const ColorManager& colorManager);
    void renderPopulation(const Node& population, const ColorManager& colorManager);
    void renderRegion(const Node& region, const ColorManager& colorManager);
    
    // Connection rendering
    void renderSynapse(const Connection& synapse, const ColorManager& colorManager);
    void renderInterRegionConnection(const Connection& connection, const ColorManager& colorManager);
    
    // Layout algorithms
    void layoutGrid(const Brain& brain);
    void layoutRadial(const Brain& brain);
    void layoutHierarchical(const Brain& brain);
    void layoutForceDirected(const Brain& brain);
    void layoutGeographic(const Brain& brain);
    
    // Interactive features
    Node* getNodeAt(int x, int y);
    Connection* getConnectionAt(int x, int y);
    void handleNodeSelection(const std::string& nodeId);
    void handleConnectionHighlight(const std::string& sourceId, const std::string& targetId);
    
    // Data extraction
    void extractBrainData(const Brain& brain);
    void calculateNodePositions();
    void calculateConnectionPaths();
    
    // Export functionality
    std::string exportToASCII() const;
    std::string exportToJSON() const;
    
    // Statistics and analysis
    void calculateStatistics(const Brain& brain);
    void showStatistics() const;
    
    // Configuration
    void loadConfiguration(const std::string& configFile);
    void saveConfiguration() const;
    
    // Color schemes for different visualization modes
    void setColorScheme(const std::string& scheme);
    std::string getNodeColor(const Node& node, const ColorManager& colorManager) const;
    std::string getConnectionColor(const Connection& connection, const ColorManager& colorManager) const;
    
private:
    LayoutMode layoutMode_;
    ConnectionStyle connectionStyle_;
    bool showNeuronLabels_;
    bool showConnectionWeights_;
    bool showRegionBoundaries_;
    float zoomLevel_;
    int panOffsetX_, panOffsetY_;
    
    std::vector<Node> nodes_;
    std::vector<Connection> connections_;
    
    // Mapping from brain entities to visualization nodes
    std::map<std::string, Node> nodeMap_;
    std::map<std::pair<std::string, std::string>, Connection> connectionMap_;
    
    // Layout algorithm state
    std::vector<std::vector<float>> adjacencyMatrix_;
    std::vector<float> nodeForcesX_;
    std::vector<float> nodeForcesY_;
    
    // Statistics
    size_t totalNeurons_;
    size_t totalSynapses_;
    size_t totalPopulations_;
    size_t totalRegions_;
    float avgFiringRate_;
    float avgSynapticWeight_;
    
    // Configuration
    std::string colorScheme_;
    int gridColumns_;
    int gridRows_;
    float forceRepulsion_;
    float forceAttraction_;
    float layoutIterations_;
    
    void initializeDefaultConfiguration();
    void updateNodeSizes(const Brain& brain);
    void updateConnectionWeights(const Brain& brain);
    int applyPanOffset(int x) const { return x + panOffsetX_; }
    int applyPanOffset(int y) const { return y + panOffsetY_; }
    float applyZoom(float value) const { return value * zoomLevel_; }
    
    // Geometry and math utilities
    float calculateDistance(const Node& a, const Node& b) const;
    float calculateAngle(const Node& a, const Node& b) const;
    std::pair<float, float> calculateBezierCurve(
        const Node& start, const Node& control1, const Node& control2, const Node& end) const;
};

} // namespace nlm
