#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <array>
#include <algorithm>
#include <unordered_map>
#include "core/Types/Types.hpp"
#include "visualization/VisualizationEngine.hpp"

namespace nlm {

// Network node representation for visualization
struct NetworkNode {
    NeuronId id;
    NeuronType type;
    RegionId regionId;
    PopulationId populationId;
    float x, y, z;  // Position in 3D space
    float radius;    // Visual size
    std::string color;
    std::string style;
    DevelopmentalStage developmentalStage;
    FiringRate firingRate;
    uint32_t synapseCount;
    std::vector<SynapseId> incomingSynapses;
    std::vector<SynapseId> outgoingSynapses;
    
    NetworkNode() : type(NeuronType::Internal), x(0.0f), y(0.0f), z(0.0f), 
                    radius(3.0f), developmentalStage(DevelopmentalStage::Adult),
                    firingRate(0.0f), synapseCount(0) {}
};

// Network connection representation for visualization
struct NetworkConnection {
    SynapseId id;
    NeuronId sourceId;
    NeuronId targetId;
    float weight;           // Synaptic weight
    float delay;            // Transmission delay
    SynapseType type;       // Connection type
    float strength;         // Visual strength (normalized weight)
    std::string color;      // Visual color
    float width;            // Visual line width
    bool isPlastic;         // Whether plastic (subject to change)
    float plasticityTrace;  // Plasticity eligibility trace
    
    NetworkConnection() : weight(0.0f), delay(0.0f), type(SynapseType::Excitatory),
                         strength(0.0f), width(1.0f), isPlastic(false), plasticityTrace(0.0f) {}
};

// Network layout configuration
struct NetworkLayout {
    enum class LayoutType {
        ForceDirected,
        Hierarchical,
        Circular,
        SpringEmbedded,
        Grid,
        Radial,
        Geodesic,
        Custom
    };
    
    LayoutType type;
    float repulsionStrength;      // For force-directed layouts
    float attractionStrength;     // For force-directed layouts
    float dampingFactor;           // For physics simulation
    float convergenceThreshold;   // Layout convergence criterion
    float timeStep;                // Integration time step
    bool showConnections;          // Whether to show connections
    bool showRegions;              // Whether to show region boundaries
    bool showPopulations;          // Whether to show population boundaries
    float nodeSizeFactor;          // Scaling factor for node sizes
    float connectionWidthFactor;   // Scaling factor for connection widths
    
    NetworkLayout() : type(LayoutType::ForceDirected), repulsionStrength(1.0f),
                     attractionStrength(0.5f), dampingFactor(0.1f),
                     convergenceThreshold(0.01f), timeStep(0.016f),
                     showConnections(true), showRegions(false), showPopulations(false),
                     nodeSizeFactor(1.0f), connectionWidthFactor(1.0f) {}
};

// Network visualization configuration
struct NetworkVisualizationConfig {
    bool showNeuronLabels;         // Whether to show neuron IDs/labels
    bool showConnectionWeights;    // Whether to show connection weights
    bool showConnectionTypes;      // Whether to show connection types
    bool showDevelopmentalState;   // Whether to show developmental stage
    bool showFiringRate;           // Whether to show firing rate
    bool showPlasticityState;      // Whether to show plasticity status
    bool highlightActive;          // Whether to highlight active neurons
    bool showRegionBoundaries;      // Whether to show region overlays
    bool showPopulationBoundaries;  // Whether to show population overlays
    float labelFontSize;           // Font size for labels
    float highlightThreshold;      // Activity level for highlighting
    std::string activeColor;       // Color for active neurons
    std::string inactiveColor;     // Color for inactive neurons
    std::string developmentalColorPrefix; // Color scheme for development
    bool animated;                 // Whether to animate changes
    
    NetworkVisualizationConfig() : showNeuronLabels(false), showConnectionWeights(true),
                                  showConnectionTypes(true), showDevelopmentalState(false),
                                  showFiringRate(false), showPlasticityState(false),
                                  highlightActive(true), showRegionBoundaries(false),
                                  showPopulationBoundaries(false), labelFontSize(10.0f),
                                  highlightThreshold(0.1f), activeColor("#FF4444"),
                                  inactiveColor("#4A90E2"), developmentalColorPrefix("stage_"),
                                  animated(true) {}
};

// NetworkVisualizer class
class NetworkVisualizer : public VisualizationInterface {
public:
    NetworkVisualizer();
    ~NetworkVisualizer();
    
    // VisualizationInterface interface
    bool initialize() override;
    void update() override;
    void render() override;
    void close() override;
    bool isActive() const override;
    void setUpdateRate(double hz) override;
    double getUpdateRate() const override;
    
    // Network visualization methods
    void visualizeNetwork(const class Brain& brain);
    void setNetworkData(const std::vector<NetworkNode>& nodes, const std::vector<NetworkConnection>& connections);
    void updateNetwork(const class Brain& brain);
    
    // Layout and configuration
    void setLayout(const NetworkLayout& layout);
    void setVisualizationConfig(const NetworkVisualizationConfig& config);
    void resetLayout();
    
    // Interaction and navigation
    void zoom(float factor);
    void pan(float dx, float dy);
    void rotate(float angle);
    void setFocus(const NetworkNode& node);
    void resetView();
    
    // Export and analysis
    void exportNetworkGraph(const std::string& filename, const std::string& format = "png");
    std::vector<std::string> analyzeNetworkTopology() const;
    float calculateNetworkDensity() const;
    float calculateClusteringCoefficient() const;
    
    // Performance optimization
    void enableLOD(bool enable);  // Level of detail
    void setMaxNodes(size_t maxNodes);
    void enableConnectionCulling(bool enable);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal methods
    void computeLayout();
    void renderNodes();
    void renderConnections();
    void renderRegions();
    void renderPopulations();
    void renderLabels();
    void renderDebugInfo();
    void updateNodePositions();
    void applyPhysicsForces();
    void handleInteraction();
    
    // Utility methods
    float calculateRepulsionForce(const NetworkNode& node1, const NetworkNode& node2) const;
    float calculateAttractionForce(const NetworkNode& node1, const NetworkNode& node2) const;
    std::string getNeuronColor(const NetworkNode& node) const;
    float getConnectionWidth(float weight) const;
    std::string getConnectionColor(const NetworkConnection& connection) const;
};

} // namespace nlm
