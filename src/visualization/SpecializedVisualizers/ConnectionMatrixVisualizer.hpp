#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <array>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <unordered_map>
#include <mutex>
#include <iomanip>
#include <sstream>
#include <iostream>
#include "core/Types/Types.hpp"
#include "visualization/VisualizationInterface.hpp"
#include "visualization/VisualizationEngine.hpp"

namespace nlm {

// Connection matrix element
struct ConnectionMatrixElement {
    SynapseId synapseId;
    NeuronId preNeuronId;
    NeuronId postNeuronId;
    float weight;               // Synaptic weight
    float delay;                // Transmission delay
    SynapseType type;           // Connection type
    float strength;             // Visual strength (normalized 0-1)
    std::string color;          // Color for visualization
    float width;                // Line width for visualization
    bool isActive;              // Whether connection is active
    bool isPlastic;             // Whether connection is plastic
    float plasticityTrace;      // Plasticity eligibility trace
    float lastModified;         // Time of last modification
    
    ConnectionMatrixElement() : weight(0.0f), delay(0.0f), type(SynapseType::Excitatory),
                                strength(0.0f), width(1.0f), isActive(true),
                                isPlastic(false), plasticityTrace(0.0f), lastModified(0.0f) {}
};

// Network community (for graph analysis)
struct NetworkCommunity {
    uint32_t id;                       // Community identifier
    std::vector<SynapseId> synapses;    // Synapses belonging to this community
    std::vector<NeuronId> neurons;     // Neurons in this community
    float modularity;                  // Modularity score
    float density;                     // Community density
    float averageDegree;              // Average degree of nodes in community
    std::string color;                 // Color for visualization
    
    NetworkCommunity() : id(0), modularity(0.0f), density(0.0f), averageDegree(0.0f) {}
};

// Connection matrix configuration
struct ConnectionMatrixConfig {
    // Display options
    bool showMatrix;              // Whether to show the connection matrix
    bool showWeights;             // Whether to show connection weights
    bool showTypes;               // Whether to show connection types
    bool showDelays;              // Whether to show transmission delays
    bool showPlasticity;          // Whether to show plasticity state
    bool showCommunities;         // Whether to show network communities
    
    // Visual styling
    float minWeight;              // Minimum weight for scaling
    float maxWeight;              // Maximum weight for scaling
    float minDelay;               // Minimum delay for scaling
    float maxDelay;               // Maximum delay for scaling
    std::string weightColorMap;   // Color map for weights
    std::string typeColorMap;     // Color map for types
    std::string delayColorMap;    // Color map for delays
    bool useLogScale;             // Whether to use logarithmic scale for weights
    
    // Matrix layout
    bool showDiagonal;            // Whether to show diagonal (self-connections)
    bool showOffDiagonal;         // Whether to show off-diagonal elements
    float matrixScale;            // Scale factor for matrix display
    bool enableZoom;              // Whether to enable zooming
    bool enablePan;               // Whether to enable panning
    
    // Interaction
    bool enableSelection;         // Whether to enable element selection
    float selectionAlpha;         // Alpha for selected elements
    bool enableHover;             // Whether to enable hover effects
    
    // Analysis
    bool showStatistics;          // Whether to show matrix statistics
    bool showConnectivity;        // Whether to show connectivity statistics
    bool showPathAnalysis;        // Whether to show path analysis
    
    ConnectionMatrixConfig() : showMatrix(true), showWeights(true), showTypes(true),
                               showDelays(true), showPlasticity(true), showCommunities(true),
                               minWeight(0.0f), maxWeight(1.0f), minDelay(0.0f), maxDelay(100.0f),
                               weightColorMap("viridis"), typeColorMap("default"),
                               delayColorMap("plasma"), useLogScale(false), showDiagonal(true),
                               showOffDiagonal(true), matrixScale(1.0f), enableZoom(true),
                               enablePan(true), enableSelection(true), selectionAlpha(0.5f),
                               enableHover(true), showStatistics(true), showConnectivity(true),
                               showPathAnalysis(false) {}
};

// Connection matrix analyzer
struct ConnectionMatrixAnalyzer {
    float totalWeight;                    // Total synaptic weight
    float meanWeight;                     // Mean synaptic weight
    float stdWeight;                      // Standard deviation of weights
    float minWeight;                      // Minimum weight
    float maxWeight;                      // Maximum weight
    size_t connectionCount;               // Total number of connections
    float density;                        // Network density (0-1)
    float clusteringCoefficient;          // Clustering coefficient
    std::vector<float> degreeDistribution; // Degree distribution
    std::vector<size_t> degreeHistogram;   // Degree histogram
    std::map<std::string, float> metrics; // Additional metrics
    
    ConnectionMatrixAnalyzer() : totalWeight(0.0f), meanWeight(0.0f), stdWeight(0.0f),
                                minWeight(0.0f), maxWeight(0.0f), connectionCount(0),
                                density(0.0f), clusteringCoefficient(0.0f) {}
};

// Connection matrix visualizer class
class ConnectionMatrixVisualizer : public VisualizationInterface {
public:
    ConnectionMatrixVisualizer();
    ~ConnectionMatrixVisualizer();
    
    // VisualizationInterface interface
    bool initialize() override;
    void update() override;
    void render() override;
    void close() override;
    bool isActive() const override;
    void setUpdateRate(double hz) override;
    double getUpdateRate() const override;
    
    // Matrix visualization methods
    void visualizeConnectionMatrix(const class Brain& brain);
    void setMatrixData(const std::vector<std::vector<ConnectionMatrixElement>>& matrix);
    void updateMatrix(const class Brain& brain);
    
    // Analysis methods
    void computeNetworkAnalysis();
    void analyzeCommunityStructure();
    void computePathLengthDistribution();
    void analyzeHubStructure();
    
    // Configuration
    void setConfig(const ConnectionMatrixConfig& config);
    void setMatrixScale(float scale);
    void enableAdvancedAnalysis(bool enable);
    
    // Interaction
    void zoom(float factor);
    void pan(float dx, float dy);
    void selectElement(const ConnectionMatrixElement& element);
    void clearSelection();
    std::vector<ConnectionMatrixElement> getSelectedElements() const;
    
    // Export and analysis
    void exportMatrixData(const std::string& filename, const std::string& format = "csv");
    void exportAnalysisResults(const std::string& filename);
    std::string analyzeConnectionTopology() const;
    std::vector<std::string> analyzeHubStructure() const;
    
    // Performance optimization
    void enableLOD(bool enable);  // Level of detail
    void setMaxMatrixSize(size_t size);
    void enableSparseMatrixOptimization(bool enable);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal methods
    void updateMatrixElements(const class Brain& brain);
    void computeMatrixAnalysis();
    void renderMatrixBackground();
    void renderMatrixGrid();
    void renderMatrixElements();
    void renderSelection();
    void renderStatistics();
    void renderLegend();
    void handleInteraction();
    
    // Utility methods
    void computeMetrics();
    std::vector<std::vector<ConnectionMatrixElement>> createSparseMatrix(const class Brain& brain) const;
    float normalizeValue(float value, float min, float max) const;
    std::string getColorForWeight(float weight) const;
    std::string getColorForType(SynapseType type) const;
    std::string getColorForDelay(float delay) const;
    size_t findNeuronIndex(NeuronId neuronId) const;
    NeuronId getNeuronIdFromIndex(size_t index) const;
};

} // namespace nlm
