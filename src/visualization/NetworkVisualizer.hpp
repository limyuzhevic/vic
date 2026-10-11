#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <array>
#include "Visualizer.hpp"

namespace nlm {

class Brain;
class NeuralRegion;
class Neuron;
class Synapse;

/**
 * Neural network visualization system for NLM
 * 
 * Provides comprehensive visualization of neural network structures,
 * connections, and dynamics including:
 * - Network topology and architecture
 * - Neural connectivity patterns
 * - Regional organization and segregation
 * - Synaptic strength and direction
 * - Network efficiency measures
 * - Small-world and scale-free properties
 * - Community detection visualization
 * - Layered and hierarchical representations
 * 
 * Features:
 * - Multiple rendering modes (graph, matrix, radial, tree)
 * - Interactive element selection and highlighting
 * - Heat map visualization of synaptic strengths
 * - Network flow and information propagation
 * - Statistical analysis overlays
 * - Export in multiple formats
 * 
 * Integration with:
 * - Brain and neural data structures
 * - Core visualization system
 * - Real-time simulation updates
 * - Export systems
 * 
 * @see Visualizer
 * @see InteractiveVisualizer
 * @see DataExporter
 * @see SpikeVisualizer
 */
class NetworkVisualizer {
public:
    /**
     * Network layout algorithm enumeration
     */
    enum class LayoutAlgorithm {
        SPRING_EMBEDDING,  // Force-directed layout
        CIRCULAR,          // Circular layout
        HIERARCHICAL,      // Tree/hierarchical layout
        RADIAL,            // Radial layout
        MATRIX,            // Adjacency matrix layout
        FORCE_DIRECTED,    // Modern force-directed
        REINGOLD_TILFORD,  // Tree layout algorithm
        GRID,              // Grid-based layout
        RANDOM             // Random placement
    };

    /**
     * Visualization mode enumeration
     */
    enum class VisualizationMode {
        GRAPH,             // Graph visualization
        MATRIX,            // Adjacency matrix
        HEAT_MAP,          // Synaptic strength heat map
        FLOW,              // Information flow
        STATISTICAL,       // Statistical overlays
        COMPARISON,        // Multi-network comparison
        SUBGRAPH,          // Focus on specific subnetworks
        INTEGRATION,       // Network integration view
        ANNOTATION         // Annotation and labeling
    };

    /**
     * Network visualization configuration
     */
    struct NetworkVisualizationConfig {
        LayoutAlgorithm layout = LayoutAlgorithm::SPRING_EMBEDDING;
        VisualizationMode mode = VisualizationMode::GRAPH;
        double nodeSize = 5.0;
        double connectionWidth = 1.0;
        double minConnectionStrength = 0.0;
        bool showNeuronTypes = true;
        bool showSynapticWeights = true;
        bool showConnectionDirection = true;
        bool showRegionBoundaries = true;
        bool animateConnections = false;
        double animationSpeed = 1.0;
        std::string nodeColorScheme = "type"; // type, strength, region, custom
        std::string edgeColorScheme = "weight"; // weight, direction, strength
        bool enableLayoutOptimization = true;
        double layoutIterations = 100.0;
        bool enableClustering = false;
        double clusteringThreshold = 0.5;
        std::vector<std::string> selectedRegions;
        std::vector<std::string> highlightedNeurons;
        std::vector<std::pair<std::string, std::string>> highlightedConnections;
        double scale = 1.0;
        double offsetX = 0.0;
        double offsetY = 0.0;
        bool enableExport = true;
    };

    /**
     * Neural element information
     */
    struct NeuralElementInfo {
        std::string id;
        std::string type; // neuron, synapse, region
        double x;         // Position X
        double y;         // Position Y
        double z;         // Position Z (for 3D)
        double size;      // Display size
        std::string color; // Display color
        std::unordered_map<std::string, std::string> properties;
        std::vector<std::string> connectedTo;
        double weight;    // Connection strength (for synapses)
        std::string region; // Brain region
    };

    /**
     * Network statistics
     */
    struct NetworkStats {
        size_t totalNeurons = 0;
        size_t totalSynapses = 0;
        size_t totalRegions = 0;
        double averageDegree = 0.0;
        double clusteringCoefficient = 0.0;
        double diameter = 0.0;
        double efficiency = 0.0;
        std::unordered_map<std::string, double> regionalStats;
        double connectionDensity = 0.0;
        size_t hubs = 0;           // High-degree nodes
        size_t isolates = 0;       // No connections
        double modularity = 0.0;
        bool isConnected = true;
    };

    /**
     * NetworkVisualizer constructor
     */
    NetworkVisualizer();
    
    /**
     * NetworkVisualizer constructor with configuration
     */
    explicit NetworkVisualizer(const NetworkVisualizationConfig& config);
    
    /**
     * NetworkVisualizer destructor
     */
    ~NetworkVisualizer();

    /**
     * Configure network visualization
     */
    void configure(const NetworkVisualizationConfig& config);
    
    /**
     * Initialize network visualization
     */
    bool initialize(const Brain& brain);
    
    /**
     * Update visualization with new brain data
     */
    void update(const Brain& brain, double timeStep);
    
    /**
     * Render network visualization
     */
    bool render();
    
    /**
     * Export network visualization
     */
    bool exportVisualization(const std::string& filename) const;
    
    /**
     * Generate network analysis report
     */
    std::string generateNetworkAnalysis() const;
    
    /**
     * Calculate network statistics
     */
    NetworkStats calculateNetworkStats(const Brain& brain) const;
    
    /**
     * Get layout algorithm
     */
    LayoutAlgorithm getLayoutAlgorithm() const;
    
    /**
     * Set layout algorithm
     */
    void setLayoutAlgorithm(LayoutAlgorithm algorithm);
    
    /**
     * Get visualization mode
     */
    VisualizationMode getVisualizationMode() const;
    
    /**
     * Set visualization mode
     */
    void setVisualizationMode(VisualizationMode mode);
    
    /**
     * Get network statistics
     */
    NetworkStats getStats() const;
    
    /**
     * Set brain data source
     */
    void setBrain(const Brain& brain);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Suppress warnings about unused private members
    NetworkVisualizer(const NetworkVisualizer&) = delete;
    NetworkVisualizer& operator=(const NetworkVisualizer&) = delete;
};

} // namespace nlm
