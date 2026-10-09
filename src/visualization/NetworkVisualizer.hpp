// Network visualization implementation
#include "NetworkVisualizer.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <queue>

namespace nlm {

NetworkVisualizer::NetworkVisualizer() 
    : active_(false), initialized_(false), updateRate_(30.0), brain_(nullptr) {
}

NetworkVisualizer::~NetworkVisualizer() {
    shutdown();
}

bool NetworkVisualizer::initialize(std::shared_ptr<Brain> brain) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize NetworkVisualizer: null brain pointer");
        return false;
    }
    
    brain_ = brain;
    
    // Initialize graph data structures
    initializeGraphData();
    
    // Initialize spike raster data
    initializeSpikeData();
    
    // Initialize connectivity map
    initializeConnectivityMap();
    
    // Initialize development stage colors
    initializeDevelopmentColors();
    
    initialized_ = true;
    NLM_LOG_INFO("NetworkVisualizer initialized successfully");
    return true;
}

void NetworkVisualizer::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!initialized_) return;
    
    // Clear all data structures
    neuronGraph_.clear();
    spikeRaster_.clear();
    connectivityMap_.clear();
    developmentColors_.clear();
    neuronRegions_.clear();
    synapseWeights_.clear();
    
    brain_.reset();
    initialized_ = false;
    NLM_LOG_INFO("NetworkVisualizer shutdown complete");
}

void NetworkVisualizer::update(const VisualizationData& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!initialized_ || !brain_) return;
    
    // Update neuron graph
    updateNeuronGraph(data);
    
    // Update spike raster
    updateSpikeRaster(data);
    
    // Update connectivity map
    updateConnectivityMap(data);
    
    // Update development colors
    updateDevelopmentColors(data);
}

void NetworkVisualizer::render() {
    // Network visualization would be implemented here
    // This would typically use a graphics library (OpenGL, Qt, SFML, etc.)
    // For now, we'll provide placeholder implementations
    
    if (visualizationMode_ == VisualizationMode::Graph) {
        renderGraphMode();
    } else if (visualizationMode_ == VisualizationMode::SpikeRaster) {
        renderSpikeRasterMode();
    } else if (visualizationMode_ == VisualizationMode::Connectivity) {
        renderConnectivityMode();
    } else if (visualizationMode_ == VisualizationMode::3D) {
        render3DMode();
    }
}

void NetworkVisualizer::setVisualizationMode(VisualizationMode mode) {
    std::lock_guard<std::mutex> lock(mutex_);
    visualizationMode_ = mode;
}

VisualizationMode NetworkVisualizer::getVisualizationMode() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return visualizationMode_;
}

void NetworkVisualizer::addNeuron(const NeuronId& id, const NeuronInfo& info) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    NeuronGraphNode node;
    node.id = id;
    node.info = info;
    node.x = 0.0f; // Will be computed during layout
    node.y = 0.0f;
    node.z = 0.0f;
    node.radius = computeNodeRadius(info);
    node.color = getNeuronColor(info);
    node.regionId = info.regionId;
    
    neuronGraph_[id] = node;
}

void NetworkVisualizer::removeNeuron(const NeuronId& id) {
    std::lock_guard<std::mutex> lock(mutex_);
    neuronGraph_.erase(id);
}

void NetworkVisualizer::addConnection(const SynapseId& id, const SynapseInfo& info) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    SynapseGraphEdge edge;
    edge.id = id;
    edge.info = info;
    edge.sourceId = info.sourceNeuron;
    edge.targetId = info.destinationNeuron;
    edge.weight = info.weight;
    edge.type = info.type;
    edge.color = getConnectionColor(info);
    edge.opacity = computeConnectionOpacity(info.weight);
    
    connectivityMap_[id] = edge;
}

void NetworkVisualizer::removeConnection(const SynapseId& id) {
    std::lock_guard<std::mutex> lock(mutex_);
    connectivityMap_.erase(id);
}

void NetworkVisualizer::setLayoutAlgorithm(LayoutAlgorithm algorithm) {
    std::lock_guard<std::mutex> lock(mutex_);
    layoutAlgorithm_ = algorithm;
}

LayoutAlgorithm NetworkVisualizer::getLayoutAlgorithm() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return layoutAlgorithm_;
}

void NetworkVisualizer::setStyle(const VisualizationStyle& style) {
    std::lock_guard<std::mutex> lock(mutex_);
    style_ = style;
}

VisualizationStyle NetworkVisualizer::getStyle() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return style_;
}

void NetworkVisualizer::enableAnimation(bool enable) {
    std::lock_guard<std::mutex> lock(mutex_);
    animationEnabled_ = enable;
}

bool NetworkVisualizer::isAnimationEnabled() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return animationEnabled_;
}

void NetworkVisualizer::addSpikeEvent(const SpikeEvent& event) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    SpikeRasterPoint point;
    point.spikeTime = event.timestamp;
    point.neuronId = event.neuronId;
    point.regionId = getRegionIdForNeuron(event.neuronId);
    
    spikeRaster_.push_back(point);
    
    // Keep only recent spikes for performance
    if (spikeRaster_.size() > maxSpikeHistory_) {
        spikeRaster_.erase(spikeRaster_.begin());
    }
}

void NetworkVisualizer::clearSpikeEvents() {
    std::lock_guard<std::mutex> lock(mutex_);
    spikeRaster_.clear();
}

std::vector<NeuronId> NetworkVisualizer::getActiveNeurons() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<NeuronId> activeNeurons;
    
    for (const auto& pair : neuronGraph_) {
        if (pair.second.info.isActive) {
            activeNeurons.push_back(pair.first);
        }
    }
    
    return activeNeurons;
}

std::vector<SynapseId> NetworkVisualizer::getActiveConnections() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<SynapseId> activeConnections;
    
    for (const auto& pair : connectivityMap_) {
        if (pair.second.info.isActive) {
            activeConnections.push_back(pair.first);
        }
    }
    
    return activeConnections;
}

void NetworkVisualizer::exportGraph(const std::string& filepath, ExportFormat format) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (format == ExportFormat::GraphML) {
        exportGraphML(filepath);
    } else if (format == ExportFormat::DOT) {
        exportDOT(filepath);
    } else if (format == ExportFormat::JSON) {
        exportJSON(filepath);
    } else {
        NLM_LOG_WARNING("Unsupported export format for graph visualization");
    }
}

void NetworkVisualizer::exportSpikeRaster(const std::string& filepath, ExportFormat format) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (format == ExportFormat::CSV) {
        exportCSV(filepath);
    } else if (format == ExportFormat::JSON) {
        exportJSON(filepath);
    } else {
        NLM_LOG_WARNING("Unsupported export format for spike raster visualization");
    }
}

void NetworkVisualizer::initializeGraphData() {
    // Initialize graph layout and positioning
    // This is a placeholder - real implementation would compute optimal layout
    layoutWidth_ = 1000.0f;
    layoutHeight_ = 1000.0f;
    layoutDepth_ = 100.0f;
    
    // Initialize region positions (placeholder)
    regionPositions_[RegionId(1)] = {500.0f, 500.0f, 0.0f};
    regionPositions_[RegionId(2)] = {300.0f, 300.0f, 0.0f};
    regionPositions_[RegionId(3)] = {700.0f, 300.0f, 0.0f};
}

void NetworkVisualizer::initializeSpikeData() {
    maxSpikeHistory_ = 1000;
    spikeRaster_.reserve(maxSpikeHistory_);
}

void NetworkVisualizer::initializeConnectivityMap() {
    // Initialize connectivity visualization parameters
    minConnectionOpacity_ = 0.1f;
    maxConnectionOpacity_ = 1.0f;
    minConnectionWidth_ = 0.5f;
    maxConnectionWidth_ = 5.0f;
}

void NetworkVisualizer::initializeDevelopmentColors() {
    // Map development stages to colors
    developmentColors_[DevelopmentalStage::Initial] = {0.3f, 0.3f, 0.3f, 1.0f}; // Gray
    developmentColors_[DevelopmentalStage::CriticalPeriod] = {0.8f, 0.2f, 0.2f, 1.0f}; // Red
    developmentColors_[DevelopmentalStage::Maturation] = {1.0f, 0.6f, 0.2f, 1.0f}; // Orange
    developmentColors_[DevelopmentalStage::Adult] = {0.2f, 0.8f, 0.2f, 1.0f}; // Green
}

void NetworkVisualizer::updateNeuronGraph(const VisualizationData& data) {
    if (!brain_) return;
    
    // Update neuron positions and properties based on current data
    // This is a placeholder - real implementation would update layout algorithm
    
    size_t neuronCount = brain_->getTotalNeuronCount();
    
    // Update active neuron states
    for (size_t i = 0; i < neuronCount && i < data.neuronActivity.size(); ++i) {
        NeuronId id(i + 1);
        
        if (neuronGraph_.find(id) != neuronGraph_.end()) {
            auto& node = neuronGraph_[id];
            
            // Update activity level
            float activity = data.neuronActivity[i];
            node.info.isActive = (activity > 0.1f);
            
            // Update membrane potential visualization
            if (i < data.membranePotentials.size()) {
                float potential = data.membranePotentials[i];
                node.info.membranePotential = potential;
            }
            
            // Update position based on network dynamics
            if (layoutAlgorithm_ == LayoutAlgorithm::ForceDirected) {
                updateForceDirectedLayout(node, data);
            } else if (layoutAlgorithm_ == LayoutAlgorithm::Radial) {
                updateRadialLayout(node, data);
            } else if (layoutAlgorithm_ == LayoutAlgorithm::Grid) {
                updateGridLayout(node, data);
            }
        }
    }
}

void NetworkVisualizer::updateSpikeRaster(const VisualizationData& data) {
    if (!brain_) return;
    
    // Get current step information for spike timing
    static SimulationStep lastStep = 0;
    SimulationStep currentStep = brain_->getCurrentStep();
    
    if (currentStep != lastStep) {
        // This is a placeholder - real implementation would get actual spike events
        // For now, we'll simulate spike events based on activity levels
        for (size_t i = 0; i < data.neuronActivity.size() && i < data.spikeCounts.size(); ++i) {
            if (data.spikeCounts[i] > 0) {
                SpikeEvent event;
                event.neuronId = NeuronId(i + 1);
                event.timestamp = static_cast<Timestamp>(currentStep * 1.0); // 1ms per step
                event.weight = data.neuronActivity[i];
                event.isExcitatory = true;
                
                addSpikeEvent(event);
            }
        }
        
        lastStep = currentStep;
    }
}

void NetworkVisualizer::updateConnectivityMap(const VisualizationData& data) {
    if (!brain_) return;
    
    // Update connection weights and visualization properties
    size_t synapseCount = brain_->getTotalSynapseCount();
    
    // This is a placeholder - real implementation would update actual synapse data
    // For now, we'll simulate connection updates based on weight matrix
    
    if (!data.synapticWeights.empty() && data.synapticWeights.size() > 0) {
        size_t matrixSize = static_cast<size_t>(std::sqrt(data.synapticWeights.size()));
        
        for (size_t i = 0; i < matrixSize && i < neuronGraph_.size(); ++i) {
            NeuronId sourceId(i + 1);
            
            if (neuronGraph_.find(sourceId) != neuronGraph_.end()) {
                for (size_t j = 0; j < matrixSize && j < neuronGraph_.size(); ++j) {
                    NeuronId targetId(j + 1);
                    
                    if (i < data.synapticWeights.size() && 
                        j * matrixSize + i < data.synapticWeights.size()) {
                        
                        float weight = data.synapticWeights[j * matrixSize + i];
                        
                        // Find or create connection
                        for (auto& pair : connectivityMap_) {
                            auto& edge = pair.second;
                            if (edge.sourceId == sourceId && edge.targetId == targetId) {
                                edge.weight = weight;
                                edge.info.weight = weight;
                                edge.color = getConnectionColor(edge.info);
                                edge.opacity = computeConnectionOpacity(weight);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}

void NetworkVisualizer::updateDevelopmentColors(const VisualizationData& data) {
    // Update development stage colors based on current data
    for (size_t i = 0; i < data.developmentStage.size() && i < neuronGraph_.size(); ++i) {
        NeuronId id(i + 1);
        
        if (neuronGraph_.find(id) != neuronGraph_.end()) {
            auto& node = neuronGraph_[id];
            node.color = developmentColors_[static_cast<DevelopmentalStage>(data.developmentStage[i])];
        }
    }
}

float NetworkVisualizer::computeNodeRadius(const NeuronInfo& info) const {
    // Compute node radius based on neuron properties
    float baseRadius = 2.0f;
    float activityRadius = info.isActive ? 4.0f : 2.0f;
    float typeRadius = (info.type == NeuronType::Sensory || info.type == NeuronType::Motor) ? 5.0f : 3.0f;
    
    return baseRadius + activityRadius + typeRadius;
}

Color NetworkVisualizer::getNeuronColor(const NeuronInfo& info) const {
    // Get color based on neuron type and activity
    if (info.isActive) {
        return {1.0f, 0.5f, 0.5f, 1.0f}; // Bright red for active neurons
    }
    
    switch (info.type) {
        case NeuronType::Sensory:
            return {0.2f, 0.6f, 1.0f, 1.0f}; // Blue for sensory neurons
        case NeuronType::Motor:
            return {1.0f, 0.2f, 0.2f, 1.0f}; // Red for motor neurons
        case NeuronType::Internal:
            return {0.3f, 0.3f, 0.8f, 1.0f}; // Purple for internal neurons
        default:
            return {0.5f, 0.5f, 0.5f, 1.0f}; // Gray for other types
    }
}

Color NetworkVisualizer::getConnectionColor(const SynapseInfo& info) const {
    // Get color based on synapse type and weight
    float normalizedWeight = (info.weight - (-1.0f)) / 2.0f; // Normalize from [-1, 1] to [0, 1]
    
    if (info.type == SynapseType::Excitatory) {
        return {std::max(0.0f, 1.0f - normalizedWeight), 0.0f, std::min(1.0f, normalizedWeight * 2.0f), 1.0f};
    } else {
        return {0.0f, std::max(0.0f, normalizedWeight), std::max(0.0f, 1.0f - normalizedWeight), 1.0f};
    }
}

float NetworkVisualizer::computeConnectionOpacity(float weight) const {
    // Compute connection opacity based on weight (absolute value)
    float normalizedWeight = std::abs(weight);
    return minConnectionOpacity_ + (maxConnectionOpacity_ - minConnectionOpacity_) * normalizedWeight;
}

RegionId NetworkVisualizer::getRegionIdForNeuron(const NeuronId& neuronId) const {
    // This is a placeholder - real implementation would map neuron to region
    // For now, we'll use a simple modulo operation
    return RegionId((neuronId.index() % 3) + 1);
}

void NetworkVisualizer::updateForceDirectedLayout(NeuronGraphNode& node, const VisualizationData& data) {
    // Placeholder for force-directed layout update
    // This would compute forces between connected neurons and update positions
}

void NetworkVisualizer::updateRadialLayout(NeuronGraphNode& node, const VisualizationData& data) {
    // Placeholder for radial layout update
    // This would position neurons in concentric circles based on activity
}

void NetworkVisualizer::updateGridLayout(NeuronGraphNode& node, const VisualizationData& data) {
    // Placeholder for grid layout update
    // This would position neurons in a grid pattern
}

void NetworkVisualizer::renderGraphMode() {
    // Placeholder for graph rendering
    // This would render neurons and connections using a graphics library
}

void NetworkVisualizer::renderSpikeRasterMode() {
    // Placeholder for spike raster rendering
    // This would render spike times as dots or lines
}

void NetworkVisualizer::renderConnectivityMode() {
    // Placeholder for connectivity map rendering
    // This would render connections as an overlay
}

void NetworkVisualizer::render3DMode() {
    // Placeholder for 3D rendering
    // This would render the network in 3D space
}

void NetworkVisualizer::exportGraphML(const std::string& filepath) {
    // Placeholder for GraphML export
    // GraphML is a standard graph file format
}

void NetworkVisualizer::exportDOT(const std::string& filepath) {
    // Placeholder for DOT export
    // DOT is a graph description language from Graphviz
}

void NetworkVisualizer::exportJSON(const std::string& filepath) {
    // Placeholder for JSON export
    // Convert graph data to JSON format
}

void NetworkVisualizer::exportCSV(const std::string& filepath) {
    // Placeholder for CSV export
    // Convert spike raster data to CSV format
}

} // namespace nlm
