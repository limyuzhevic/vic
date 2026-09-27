// ==============================================================================
// Neural Network Visualizer
// ==============================================================================
#ifndef NEURALNETWORKVISUALIZER_HPP
#define NEURALNETWORKVISUALIZER_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <graphviz/cgraph.h>
#include "../src/brain/NeuralPopulation.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"

namespace nlm {
namespace debug {

/**
 * @class NeuralNetworkVisualizer
 * @brief Neural network visualization tool for NLM
 * 
 * Provides tools for visualizing neural network structures including neurons,
 * synapses, populations, and connectivity patterns.
 */
class NeuralNetworkVisualizer {
public:
    /**
     * @enum VisualizationStyle
     * @brief Style of visualization
     */
    enum class VisualizationStyle {
        GRAPHVIZ_DOT,
        GRAPHVIZ_NEATO,
        GRAPHVIZ_FDP,
        GRAPHVIZ_CIRCO,
        GRAPHVIZ_TWO,
        HEATMAP,
        CIRCUIT,
        FUNCTIONAL
    };

    /**
     * @enum HighlightType
     * @brief Type of highlighting
     */
    enum class HighlightType {
        NONE,
        ACTIVE,
        HIGH_ACTIVITY,
        HIGH_CONNECTIONS,
        HIGH_EXCITABILITY,
        HIGH_INHIBITORY,
        RECENTLY_ACTIVE,
        PLACIDITY_HOTSPOTS,
        ANOMALIES
    };

    /**
     * @struct NodeInfo
     * @brief Information about a node
     */
    struct NodeInfo {
        std::string id;
        std::string type;
        std::string label;
        double size;
        double x;
        double y;
        double z;
        std::map<std::string, std::string> attributes;
    };

    /**
     * @struct EdgeInfo
     * @brief Information about an edge
     */
    struct EdgeInfo {
        std::string source;
        std::string target;
        double weight;
        double capacity;
        double delay;
        std::map<std::string, std::string> attributes;
    };

    NeuralNetworkVisualizer();
    ~NeuralNetworkVisualizer();

    /**
     * @brief Visualize neural network
     * @param brain Neural network to visualize
     * @param filename Output file path
     * @param style Visualization style
     * @return true if visualization was created
     */
    bool visualize(const Brain& brain, const std::string& filename, VisualizationStyle style = VisualizationStyle::GRAPHVIZ_DOT);

    /**
     * @brief Visualize neural population
     * @param population Neural population to visualize
     * @param filename Output file path
     * @param style Visualization style
     * @return true if visualization was created
     */
    bool visualize(const NeuralPopulation& population, const std::string& filename, VisualizationStyle style = VisualizationStyle::GRAPHVIZ_DOT);

    /**
     * @brief Visualize neuron
     * @param neuron Neuron to visualize
     * @param filename Output file path
     * @param style Visualization style
     * @return true if visualization was created
     */
    bool visualize(const Neuron& neuron, const std::string& filename, VisualizationStyle style = VisualizationStyle::GRAPHVIZ_DOT);

    /**
     * @brief Visualize synapse
     * @param synapse Synapse to visualize
     * @param filename Output file path
     * @param style Visualization style
     * @return true if visualization was created
     */
    bool visualize(const Synapse& synapse, const std::string& filename, VisualizationStyle style = VisualizationStyle::GRAPHVIZ_DOT);

    /**
     * @brief Highlight neurons
     * @param brain Neural network reference
     * @param neuronIds Vector of neuron IDs to highlight
     * @param highlightType Highlight type
     */
    void highlightNeurons(const Brain& brain, const std::vector<std::string>& neuronIds, HighlightType highlightType = HighlightType::ACTIVE);

    /**
     * @brief Highlight synapses
     * @param brain Neural network reference
     * @param synapseIds Vector of synapse IDs to highlight
     * @param highlightType Highlight type
     */
    void highlightSynapses(const Brain& brain, const std::vector<std::string>& synapseIds, HighlightType highlightType = HighlightType::HIGH_CONNECTIONS);

    /**
     * @brief Get visualization options
     * @return Visualization options
     */
    struct VisualizationOptions {
        bool showNeurons;
        bool showSynapses;
        bool showPopulations;
        bool showWeights;
        bool showActivity;
        bool showConnections;
        double nodeSizeScale;
        double edgeWidthScale;
        std::string colorScheme;
        bool animated;
    };

    VisualizationOptions getVisualizationOptions() const;

    /**
     * @brief Set visualization options
     * @param options Visualization options
     */
    void setVisualizationOptions(const VisualizationOptions& options);

    /**
     * @brief Get node information
     * @param brain Neural network reference
     * @param neuronId Neuron ID
     * @return Node information
     */
    NodeInfo getNodeInfo(const Brain& brain, const std::string& neuronId) const;

    /**
     * @brief Get edge information
     * @param brain Neural network reference
     * @param synapseId Synapse ID
     * @return Edge information
     */
    EdgeInfo getEdgeInfo(const Brain& brain, const std::string& synapseId) const;

    /**
     * @brief Generate connectivity matrix
     * @param brain Neural network reference
     * @param filename Output file path
     * @return true if connectivity matrix was generated
     */
    bool generateConnectivityMatrix(const Brain& brain, const std::string& filename);

    /**
     * @brief Generate network statistics
     * @param brain Neural network reference
     * @return Network statistics
     */
    struct NetworkStatistics {
        uint64_t totalNeurons;
        uint64_t totalSynapses;
        uint64_t totalPopulations;
        double averageConnectionsPerNeuron;
        double averageFiringRate;
        double averageExcitatoryInhibitionRatio;
        double averageSynapticStrength;
        double clusteringCoefficient;
        double pathLength;
        double modularity;
    };

    NetworkStatistics getNetworkStatistics(const Brain& brain) const;

    /**
     * @brief Export visualization to SVG
     * @param brain Neural network reference
     * @param filename Output file path
     * @return true if export was successful
     */
    bool exportToSVG(const Brain& brain, const std::string& filename);

    /**
     * @brief Export visualization to PDF
     * @param brain Neural network reference
     * @param filename Output file path
     * @return true if export was successful
     */
    bool exportToPDF(const Brain& brain, const std::string& filename);

protected:
    /**
     * @brief Create graphviz graph
     * @param brain Neural network reference
     * @return Graphviz graph
     */
    Agraph_t* createGraphvizGraph(const Brain& brain);

    /**
     * @brief Add neurons to graph
     * @param graph Graphviz graph
     * @param brain Neural network reference
     */
    void addNeuronsToGraph(Agraph_t* graph, const Brain& brain);

    /**
     * @brief Add synapses to graph
     * @param graph Graphviz graph
     * @param brain Neural network reference
     */
    void addSynapsesToGraph(Agraph_t* graph, const Brain& brain);

    /**
     * @brief Add populations to graph
     * @param graph Graphviz graph
     * @param brain Neural network reference
     */
    void addPopulationsToGraph(Agraph_t* graph, const Brain& brain);

    /**
     * @brief Apply highlighting to graph
     * @param graph Graphviz graph
     * @param highlightType Highlight type
     */
    void applyHighlighting(Agraph_t* graph, HighlightType highlightType);

    /**
     * @brief Calculate neuron position
     * @param neuron Neuron reference
     * @param populationIndex Population index
     * @param totalPopulations Total populations
     * @return Position
     */
    std::pair<double, double> calculateNeuronPosition(const Neuron& neuron, size_t populationIndex, size_t totalPopulations);

    /**
     * @brief Get neuron color based on activity
     * @param neuron Neuron reference
     * @param highlightType Highlight type
     * @return Color
     */
    std::string getNeuronColor(const Neuron& neuron, HighlightType highlightType);

    /**
     * @brief Get synapse color based on weight
     * @param synapse Synapse reference
     * @param highlightType Highlight type
     * @return Color
     */
    std::string getSynapseColor(const Synapse& synapse, HighlightType highlightType);

    /**
     * @brief Get population color
     * @param populationIndex Population index
     * @param totalPopulations Total populations
     * @return Color
     */
    std::string getPopulationColor(size_t populationIndex, size_t totalPopulations);

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace debug
} // namespace nlm

#endif // NEURALNETWORKVISUALIZER_HPP
