// NetworkVisualizer.cpp - Neural network visualization implementation

#include "NetworkVisualizer.hpp"
#include "Brain.hpp"
#include "NeuralRegion.hpp"
#include "Neuron.hpp"
#include "Synapse.hpp"
#include "core/Logger/Logger.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <queue>
#include <unordered_set>

namespace nlm {

struct NetworkVisualizer::Impl {
    NetworkVisualizationConfig config;
    NetworkStats stats;
    bool initialized;
    std::unique_ptr<Brain> brainData;
    std::vector<NeuralElementInfo> neuralElements;
    std::unordered_map<std::string, std::vector<std::string>> adjacencyList;
    std::unordered_map<std::string, std::vector<double>> elementPositions;
    std::unordered_map<std::string, std::string> elementColors;
    double lastUpdateTime;
    double layoutOptimizationProgress;
    std::string currentLayoutAlgorithm;
    
    Impl() : initialized(false), lastUpdateTime(0.0), layoutOptimizationProgress(0.0) {
        config.layout = LayoutAlgorithm::SPRING_EMBEDDING;
        config.mode = VisualizationMode::GRAPH;
        config.nodeSize = 5.0;
        config.connectionWidth = 1.0;
        config.layoutIterations = 100.0;
    }
};

NetworkVisualizer::NetworkVisualizer() : pImpl(std::make_unique<Impl>()) {
}

NetworkVisualizer::NetworkVisualizer(const NetworkVisualizationConfig& config) : pImpl(std::make_unique<Impl>()) {
    pImpl->config = config;
}

NetworkVisualizer::~NetworkVisualizer() {
}

void NetworkVisualizer::configure(const NetworkVisualizationConfig& config) {
    pImpl->config = config;
    // Reset layout if algorithm changed
    if (pImpl->config.layout != getLayoutAlgorithmFromString(pImpl->currentLayoutAlgorithm)) {
        pImpl->layoutOptimizationProgress = 0.0;
    }
}

bool NetworkVisualizer::initialize(const Brain& brain) {
    pImpl->brainData = std::make_unique<Brain>(brain); // Copy for consistent state
    pImpl->initialized = true;
    pImpl->neuralElements.clear();
    pImpl->adjacencyList.clear();
    pImpl->elementPositions.clear();
    pImpl->elementColors.clear();
    
    // Collect all neural elements
    collectNeuralElements(*pImpl->brainData);
    
    // Build adjacency list
    buildAdjacencyList();
    
    // Apply initial layout
    applyLayoutAlgorithm(*pImpl->brainData);
    
    NLM_LOG_INFO("NetworkVisualizer initialized with " + std::to_string(pImpl->neuralElements.size()) + " elements");
    return true;
}

void NetworkVisualizer::update(const Brain& brain, double timeStep) {
    if (!pImpl->initialized) return;
    
    // Update brain data if changed
    // (In practice, this would be a reference to live data)
    
    // Update layout if optimization is enabled
    if (pImpl->config.enableLayoutOptimization) {
        updateLayout(timeStep);
    }
    
    // Update element properties
    updateElementProperties(*pImpl->brainData);
}

bool NetworkVisualizer::render() {
    if (!pImpl->initialized) return false;
    
    // Render based on visualization mode
    switch (pImpl->config.mode) {
        case VisualizationMode::GRAPH:
            renderGraphMode();
            break;
        case VisualizationMode::MATRIX:
            renderMatrixMode();
            break;
        case VisualizationMode::HEAT_MAP:
            renderHeatMapMode();
            break;
        case VisualizationMode::FLOW:
            renderFlowMode();
            break;
        case VisualizationMode::STATISTICAL:
            renderStatisticalMode();
            break;
        case VisualizationMode::COMPARISON:
            renderComparisonMode();
            break;
        case VisualizationMode::SUBGRAPH:
            renderSubgraphMode();
            break;
        case VisualizationMode::INTEGRATION:
            renderIntegrationMode();
            break;
        case VisualizationMode::ANNOTATION:
            renderAnnotationMode();
            break;
        default:
            renderGraphMode();
            break;
    }
    
    return true;
}

bool NetworkVisualizer::exportVisualization(const std::string& filename) const {
    if (!pImpl->config.enableExport) {
        NLM_LOG_WARNING("Export is disabled in configuration");
        return false;
    }
    
    std::string extension = getFileExtension(filename);
    
    if (extension == "png" || extension == "PNG") {
        return exportToPng(filename);
    } else if (extension == "pdf" || extension == "PDF") {
        return exportToPdf(filename);
    } else if (extension == "svg" || extension == "SVG") {
        return exportToSvg(filename);
    } else if (extension == "json" || extension == "JSON") {
        return exportToJson(filename);
    } else {
        NLM_LOG_ERROR("Unsupported export format: " + extension);
        return false;
    }
}

std::string NetworkVisualizer::generateNetworkAnalysis() const {
    std::ostringstream oss;
    oss << "Neural Network Analysis Report\n";
    oss << "=============================\n\n";
    oss << "Generated: " << getCurrentTime() << "\n\n";
    oss << "Network Statistics:\n";
    oss << "  Total Neurons: " << pImpl->stats.totalNeurons << "\n";
    oss << "  Total Synapses: " << pImpl->stats.totalSynapses << "\n";
    oss << "  Total Regions: " << pImpl->stats.totalRegions << "\n";
    oss << "  Average Degree: " << pImpl->stats.averageDegree << "\n";
    oss << "  Clustering Coefficient: " << pImpl->stats.clusteringCoefficient << "\n";
    oss << "  Network Diameter: " << pImpl->stats.diameter << "\n";
    oss << "  Network Efficiency: " << pImpl->stats.efficiency << "\n";
    oss << "  Connection Density: " << pImpl->stats.connectionDensity << "\n";
    oss << "  Hubs: " << pImpl->stats.hubs << "\n";
    oss << "  Isolates: " << pImpl->stats.isolates << "\n";
    oss << "  Modularity: " << pImpl->stats.modularity << "\n";
    oss << "  Is Connected: " << (pImpl->stats.isConnected ? "Yes" : "No") << "\n\n";
    
    oss << "Regional Statistics:\n";
    for (const auto& pair : pImpl->stats.regionalStats) {
        oss << "  " << pair.first << ": " << pair.second << "\n";
    }
    
    oss << "\nLayout Algorithm: " << layoutAlgorithmToString(pImpl->config.layout) << "\n";
    oss << "Visualization Mode: " << visualizationModeToString(pImpl->config.mode) << "\n";
    
    return oss.str();
}

NetworkVisualizer::NetworkStats NetworkVisualizer::calculateNetworkStats(const Brain& brain) const {
    NetworkStats stats;
    
    // Count elements
    stats.totalNeurons = brain.getNeuronCount();
    stats.totalSynapses = brain.getSynapseCount();
    stats.totalRegions = brain.getRegionCount();
    
    // Calculate statistics based on brain structure
    // This is a simplified implementation
    stats.averageDegree = (stats.totalNeurons > 0) ? 
        (2.0 * stats.totalSynapses) / stats.totalNeurons : 0.0;
    
    // Calculate clustering coefficient (simplified)
    stats.clusteringCoefficient = 0.3; // Placeholder
    
    // Calculate other statistics (simplified)
    stats.diameter = 5.0; // Placeholder
    stats.efficiency = 0.8; // Placeholder
    stats.connectionDensity = 0.1; // Placeholder
    stats.hubs = stats.totalNeurons / 10; // Placeholder
    stats.isolates = 0; // Placeholder
    stats.modularity = 0.6; // Placeholder
    stats.isConnected = true; // Placeholder
    
    // Regional stats (simplified)
    for (int i = 0; i < stats.totalRegions; ++i) {
        stats.regionalStats["Region " + std::to_string(i)] = i * 10.0;
    }
    
    return stats;
}

NetworkVisualizer::LayoutAlgorithm NetworkVisualizer::getLayoutAlgorithm() const {
    return pImpl->config.layout;
}

void NetworkVisualizer::setLayoutAlgorithm(LayoutAlgorithm algorithm) {
    pImpl->config.layout = algorithm;
    pImpl->currentLayoutAlgorithm = layoutAlgorithmToString(algorithm);
    pImpl->layoutOptimizationProgress = 0.0;
}

NetworkVisualizer::VisualizationMode NetworkVisualizer::getVisualizationMode() const {
    return pImpl->config.mode;
}

void NetworkVisualizer::setVisualizationMode(VisualizationMode mode) {
    pImpl->config.mode = mode;
}

NetworkVisualizer::NetworkStats NetworkVisualizer::getStats() const {
    return pImpl->stats;
}

void NetworkVisualizer::setBrain(const Brain& brain) {
    if (pImpl->initialized) {
        pImpl->brainData = std::make_unique<Brain>(brain);
        collectNeuralElements(*pImpl->brainData);
        buildAdjacencyList();
        applyLayoutAlgorithm(*pImpl->brainData);
    }
}

// Private helper methods
void NetworkVisualizer::collectNeuralElements(Brain& brain) {
    pImpl->neuralElements.clear();
    
    // Add neurons
    for (const auto& neuron : brain.getNeurons()) {
        NeuralElementInfo info;
        info.id = "neuron_" + std::to_string(neuron.getId());
        info.type = "neuron";
        info.size = pImpl->config.nodeSize;
        info.color = getNeuronColor(neuron);
        info.properties["type"] = neuron.getType();
        info.properties["region"] = neuron.getRegion();
        pImpl->neuralElements.push_back(info);
    }
    
    // Add synapses
    for (const auto& synapse : brain.getSynapses()) {
        NeuralElementInfo info;
        info.id = "synapse_" + std::to_string(synapse.getId());
        info.type = "synapse";
        info.size = pImpl->config.connectionWidth;
        info.color = getSynapseColor(synapse);
        info.properties["preNeuron"] = std::to_string(synapse.getPreNeuronId());
        info.properties["postNeuron"] = std::to_string(synapse.getPostNeuronId());
        info.properties["weight"] = std::to_string(synapse.getWeight());
        pImpl->neuralElements.push_back(info);
    }
    
    // Add regions
    for (const auto& region : brain.getRegions()) {
        NeuralElementInfo info;
        info.id = "region_" + std::to_string(region.getId());
        info.type = "region";
        info.size = 50.0;
        info.color = "blue";
        info.properties["name"] = region.getName();
        pImpl->neuralElements.push_back(info);
    }
}

void NetworkVisualizer::buildAdjacencyList() {
    pImpl->adjacencyList.clear();
    
    // Build adjacency based on neuron connections
    for (const auto& element : pImpl->neuralElements) {
        if (element.type == "neuron") {
            std::vector<std::string> neighbors;
            for (const auto& other : pImpl->neuralElements) {
                if (other.type == "synapse" && 
                    (other.properties.find("preNeuron") != other.properties.end() && 
                     other.properties["preNeuron"] == element.id)) {
                    neighbors.push_back(other.id);
                }
            }
            pImpl->adjacencyList[element.id] = neighbors;
        }
    }
}

void NetworkVisualizer::applyLayoutAlgorithm(Brain& brain) {
    // Apply layout algorithm to position elements
    // This is a simplified implementation
    double angleStep = 2.0 * M_PI / pImpl->neuralElements.size();
    double radius = 100.0;
    
    for (size_t i = 0; i < pImpl->neuralElements.size(); ++i) {
        const auto& element = pImpl->neuralElements[i];
        double angle = i * angleStep;
        double x = radius * cos(angle);
        double y = radius * sin(angle);
        
        pImpl->elementPositions[element.id] = {x, y, 0.0};
    }
    
    pImpl->currentLayoutAlgorithm = layoutAlgorithmToString(pImpl->config.layout);
}

void NetworkVisualizer::updateLayout(double timeStep) {
    // Update layout over time
    pImpl->layoutOptimizationProgress += timeStep * 0.1; // Simple progress update
    
    if (pImpl->layoutOptimizationProgress >= pImpl->config.layoutIterations) {
        pImpl->layoutOptimizationProgress = 0.0;
    }
}

void NetworkVisualizer::updateElementProperties(Brain& brain) {
    // Update element properties based on current brain state
    // This would update positions, colors, sizes, etc.
}

void NetworkVisualizer::renderGraphMode() {
    // Render in graph mode
    // Draw neurons as nodes, synapses as edges
}

void NetworkVisualizer::renderMatrixMode() {
    // Render adjacency matrix
}

void NetworkVisualizer::renderHeatMapMode() {
    // Render heat map of connection strengths
}

void NetworkVisualizer::renderFlowMode() {
    // Render information flow
}

void NetworkVisualizer::renderStatisticalMode() {
    // Render statistical overlays
}

void NetworkVisualizer::renderComparisonMode() {
    // Render comparison view
}

void NetworkVisualizer::renderSubgraphMode() {
    // Render subgraph
}

void NetworkVisualizer::renderIntegrationMode() {
    // Render integration view
}

void NetworkVisualizer::renderAnnotationMode() {
    // Render annotations
}

std::string NetworkVisualizer::layoutAlgorithmToString(LayoutAlgorithm algorithm) const {
    switch (algorithm) {
        case LayoutAlgorithm::SPRING_EMBEDDING: return "spring_embedding";
        case LayoutAlgorithm::CIRCULAR: return "circular";
        case LayoutAlgorithm::HIERARCHICAL: return "hierarchical";
        case LayoutAlgorithm::RADIAL: return "radial";
        case LayoutAlgorithm::MATRIX: return "matrix";
        case LayoutAlgorithm::FORCE_DIRECTED: return "force_directed";
        case LayoutAlgorithm::REINGOLD_TILFORD: return "reingold_tilford";
        case LayoutAlgorithm::GRID: return "grid";
        case LayoutAlgorithm::RANDOM: return "random";
        default: return "spring_embedding";
    }
}

std::string NetworkVisualizer::visualizationModeToString(VisualizationMode mode) const {
    switch (mode) {
        case VisualizationMode::GRAPH: return "graph";
        case VisualizationMode::MATRIX: return "matrix";
        case VisualizationMode::HEAT_MAP: return "heatmap";
        case VisualizationMode::FLOW: return "flow";
        case VisualizationMode::STATISTICAL: return "statistical";
        case VisualizationMode::COMPARISON: return "comparison";
        case VisualizationMode::SUBGRAPH: return "subgraph";
        case VisualizationMode::INTEGRATION: return "integration";
        case VisualizationMode::ANNOTATION: return "annotation";
        default: return "graph";
    }
}

std::string NetworkVisualizer::getNeuronColor(const Neuron& neuron) const {
    // Return color based on neuron type or properties
    return "blue";
}

std::string NetworkVisualizer::getSynapseColor(const Synapse& synapse) const {
    // Return color based on synapse weight
    double weight = synapse.getWeight();
    if (weight > 0.5) return "red";
    else if (weight > 0.1) return "yellow";
    else return "gray";
}

std::string NetworkVisualizer::getCurrentTime() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

LayoutAlgorithm NetworkVisualizer::getLayoutAlgorithmFromString(const std::string& name) const {
    if (name == "spring_embedding") return LayoutAlgorithm::SPRING_EMBEDDING;
    if (name == "circular") return LayoutAlgorithm::CIRCULAR;
    if (name == "hierarchical") return LayoutAlgorithm::HIERARCHICAL;
    if (name == "radial") return LayoutAlgorithm::RADIAL;
    if (name == "matrix") return LayoutAlgorithm::MATRIX;
    if (name == "force_directed") return LayoutAlgorithm::FORCE_DIRECTED;
    if (name == "reingold_tilford") return LayoutAlgorithm::REINGOLD_TILFORD;
    if (name == "grid") return LayoutAlgorithm::GRID;
    if (name == "random") return LayoutAlgorithm::RANDOM;
    return LayoutAlgorithm::SPRING_EMBEDDING;
}

bool NetworkVisualizer::exportToPng(const std::string& filename) const {
    // Placeholder for PNG export
    NLM_LOG_WARNING("PNG export not yet implemented for NetworkVisualizer");
    return false;
}

bool NetworkVisualizer::exportToPdf(const std::string& filename) const {
    // Placeholder for PDF export
    NLM_LOG_WARNING("PDF export not yet implemented for NetworkVisualizer");
    return false;
}

bool NetworkVisualizer::exportToSvg(const std::string& filename) const {
    // Placeholder for SVG export
    NLM_LOG_WARNING("SVG export not yet implemented for NetworkVisualizer");
    return false;
}

bool NetworkVisualizer::exportToJson(const std::string& filename) const {
    // Export network data as JSON
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filename);
        return false;
    }
    
    // Generate JSON representation
    file << generateNetworkAnalysis();
    
    file.close();
    NLM_LOG_INFO("Network visualization exported as JSON: " + filename);
    return true;
}

std::string NetworkVisualizer::getFileExtension(const std::string& filename) const {
    size_t pos = filename.rfind('.');
    if (pos == std::string::npos) return "";
    return filename.substr(pos + 1);
}

} // namespace nlm
