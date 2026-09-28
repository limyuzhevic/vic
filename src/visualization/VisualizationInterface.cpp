#include "VisualizationInterface.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>

namespace nlm {

VisualizationInterface::VisualizationInterface()
    : pImpl(new Impl())
    , windowWidth_(800)
    , windowHeight_(600)
    , showNetworkGraph_(true)
    , showSpikeRaster_(true)
    , showWeightMatrix_(false)
    , showActivityHeatmap_(false)
    , autoRefresh_(true)
    , refreshInterval_(100)
    , lastUpdateStep_(0)
{
}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    // Initialize visualization with external libraries
    // For now, provide a comprehensive placeholder that shows what's possible
    NLM_LOG_INFO("Visualization Interface initialized");
    NLM_LOG_INFO("Available visualization modes:");
    NLM_LOG_INFO("  - Network Graph: Shows neural topology and connectivity");
    NLM_LOG_INFO("  - Spike Raster: Displays spike timing patterns");
    NLM_LOG_INFO("  - Weight Matrix: Shows synaptic weight distributions");
    NLM_LOG_INFO("  - Activity Heatmap: Displays regional activity levels");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Real implementation requires external library (OpenGL/SFML/SDL2)");
    
    // Enable visualization by default
    pImpl->active = true;
    return true;
}

void VisualizationInterface::update(const Brain& brain, SimulationStep step) {
    // Update visualization data with current brain state
    if (!pImpl->active) return;
    
    updateNetworkData(brain);
    updateSpikeData(brain, step);
    updateWeightData(brain);
    
    lastUpdateStep_ = step;
    
    // Update display if auto-refresh is enabled
    if (autoRefresh_ && step % refreshInterval_ == 0) {
        render(brain, step);
    }
}

void VisualizationInterface::render(const Brain& brain, SimulationStep step) {
    // Render visualization based on current settings
    if (!pImpl->active) return;
    
    NLM_LOG_DEBUG("Rendering visualization at step " + std::to_string(step));
    
    if (showNetworkGraph_) {
        renderNetworkGraph(brain);
    }
    
    if (showSpikeRaster_) {
        renderSpikeRaster(brain, step);
    }
    
    if (showWeightMatrix_) {
        renderWeightMatrix(brain);
    }
    
    if (showActivityHeatmap_) {
        renderActivityHeatmap(brain);
    }
}

void VisualizationInterface::renderNetworkGraph(const Brain& brain) {
    // Render the neural network topology
    NLM_LOG_DEBUG("Rendering network graph with " + std::to_string(brain.getRegionCount()) + " regions");
    
    for (const auto& regionId : brain.getRegionIds()) {
        const auto* region = brain.getRegion(regionId);
        if (region) {
            NLM_LOG_DEBUG("  Region " + std::to_string(regionId.index()) + ": " +
                         std::to_string(region->getName()) + ", " +
                         std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                         std::to_string(region->getSynapseCount()) + " synapses, " +
                         "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
        }
    }
}

void VisualizationInterface::renderSpikeRaster(const Brain& brain, SimulationStep step) {
    // Render spike raster plot showing spike times
    NLM_LOG_DEBUG("Rendering spike raster plot (step " + std::to_string(step) + ")");
    
    size_t totalSpikes = brain.getTotalSpikeCount();
    NLM_LOG_DEBUG("  Total spikes: " + std::to_string(totalSpikes));
    NLM_LOG_DEBUG("  Firing neurons this step: " + std::to_string(brain.getFiringNeuronCount()));
    
    if (totalSpikes > 0) {
        NLM_LOG_DEBUG("  Spike activity detected across " + std::to_string(brain.getActiveNeuronCount()) + " neurons");
    }
}

void VisualizationInterface::renderWeightMatrix(const Brain& brain) {
    // Render synaptic weight matrix visualization
    NLM_LOG_DEBUG("Rendering weight matrix visualization");
    
    float avgWeight = brain.getExcitationInhibitionRatio();
    NLM_LOG_DEBUG("  Average synaptic weight ratio: " + std::to_string(avgWeight));
    
    size_t totalSynapses = brain.getTotalSynapseCount();
    if (totalSynapses > 0) {
        NLM_LOG_DEBUG("  Total connections: " + std::to_string(totalSynapses));
    }
}

void VisualizationInterface::renderActivityHeatmap(const Brain& brain) {
    // Render activity heatmap of brain regions
    NLM_LOG_DEBUG("Rendering activity heatmap");
    
    for (const auto* region : brain.getRegions()) {
        float avgActivity = region->getAverageFiringRate();
        float activityPercent = avgActivity * 100.0f;
        NLM_LOG_DEBUG("  Region " + std::to_string(region->getId().index()) + ": " +
                     std::to_string(activityPercent) + "% average activity");
    }
}

void VisualizationInterface::updateNetworkData(const Brain& brain) {
    // Update network topology data for visualization
    networkData_.clear();
    
    for (const auto& regionId : brain.getRegionIds()) {
        const auto* region = brain.getRegion(regionId);
        if (region) {
            NetworkNode node;
            node.id = regionId.index();
            node.name = region->getName();
            node.neuronCount = region->getTotalNeuronCount();
            node.synapseCount = region->getSynapseCount();
            node.averageWeight = region->getAverageSynapticWeight();
            
            networkData_.push_back(node);
        }
    }
}

void VisualizationInterface::updateSpikeData(const Brain& brain, SimulationStep step) {
    // Update spike data for raster plots
    spikeData_.clear();
    
    SpikePlotData plotData;
    plotData.step = step;
    plotData.totalSpikes = brain.getTotalSpikeCount();
    plotData.firingNeurons = brain.getFiringNeuronCount();
    
    spikeData_.push_back(plotData);
}

void VisualizationInterface::updateWeightData(const Brain& brain) {
    // Update synaptic weight data for visualization
    weightData_.clear();
    
    float totalWeight = 0.0f;
    size_t totalSynapses = brain.getTotalSynapseCount();
    
    if (totalSynapses > 0) {
        for (const auto* region : brain.getRegions()) {
            for (const auto* synapse : region->getSynapses()) {
                totalWeight += std::abs(synapse->getWeight());
            }
        }
        
        float avgWeight = totalWeight / static_cast<float>(totalSynapses);
        
        WeightMatrixData matrixData;
        matrixData.step = lastUpdateStep_;
        matrixData.averageWeight = avgWeight;
        matrixData.minWeight = -1.0f;  // Would track actual min
        matrixData.maxWeight = 1.0f;   // Would track actual max
        matrixData.totalConnections = totalSynapses;
        
        weightData_.push_back(matrixData);
    }
}

void VisualizationInterface::configure(int width, int height) {
    // Configure visualization window settings
    windowWidth_ = width;
    windowHeight_ = height;
    
    NLM_LOG_INFO("Visualization configured: " + std::to_string(width) + "x" + std::to_string(height));
}

void VisualizationInterface::toggleDisplayMode(VisualizationInterface::DisplayMode mode) {
    // Toggle specific visualization mode
    switch (mode) {
        case DisplayMode::NetworkGraph:
            showNetworkGraph_ = !showNetworkGraph_;
            NLM_LOG_INFO("Network graph display: " + std::string(showNetworkGraph_ ? "ON" : "OFF"));
            break;
        case DisplayMode::SpikeRaster:
            showSpikeRaster_ = !showSpikeRaster_;
            NLM_LOG_INFO("Spike raster display: " + std::string(showSpikeRaster_ ? "ON" : "OFF"));
            break;
        case DisplayMode::WeightMatrix:
            showWeightMatrix_ = !showWeightMatrix_;
            NLM_LOG_INFO("Weight matrix display: " + std::string(showWeightMatrix_ ? "ON" : "OFF"));
            break;
        case DisplayMode::ActivityHeatmap:
            showActivityHeatmap_ = !showActivityHeatmap_;
            NLM_LOG_INFO("Activity heatmap display: " + std::string(showActivityHeatmap_ ? "ON" : "OFF"));
            break;
        default:
            break;
    }
}

void VisualizationInterface::setAutoRefresh(bool enabled, int interval) {
    // Configure automatic refresh behavior
    autoRefresh_ = enabled;
    refreshInterval_ = interval;
    
    NLM_LOG_INFO("Auto refresh: " + std::string(enabled ? "ON" : "OFF") + 
                 " (interval: " + std::to_string(interval) + " steps)");
}

void VisualizationInterface::close() {
    // Clean up visualization resources
    pImpl->active = false;
    NLM_LOG_INFO("Visualization interface closed");
}

bool VisualizationInterface::isActive() const {
    return pImpl->active;
}

void VisualizationInterface::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
}

double VisualizationInterface::getUpdateRate() const {
    return pImpl->updateRate;
}

// Getters for visualization properties
int VisualizationInterface::getWindowWidth() const {
    return windowWidth_;
}

int VisualizationInterface::getWindowHeight() const {
    return windowHeight_;
}

bool VisualizationInterface::isNetworkGraphEnabled() const {
    return showNetworkGraph_;
}

bool VisualizationInterface::isSpikeRasterEnabled() const {
    return showSpikeRaster_;
}

bool VisualizationInterface::isWeightMatrixEnabled() const {
    return showWeightMatrix_;
}

bool VisualizationInterface::isActivityHeatmapEnabled() const {
    return showActivityHeatmap_;
}

bool VisualizationInterface::isAutoRefreshEnabled() const {
    return autoRefresh_;
}

// Implementation of NetworkNode::NetworkNode()
VisualizationInterface::NetworkNode::NetworkNode() : id(0), neuronCount(0), synapseCount(0), averageWeight(0.0f) {}

// Implementation of SpikePlotData::SpikePlotData()
VisualizationInterface::SpikePlotData::SpikePlotData() : step(0), totalSpikes(0), firingNeurons(0) {}

// Implementation of WeightMatrixData::WeightMatrixData()
VisualizationInterface::WeightMatrixData::WeightMatrixData() : step(0), averageWeight(0.0f), minWeight(0.0f), maxWeight(0.0f), totalConnections(0) {}

} // namespace nlm

// Helper function for visualization
void printVisualizationInfo(const VisualizationInterface& viz) {
    NLM_LOG_INFO("=== Visualization Interface Status ===");
    NLM_LOG_INFO("Window size: " + std::to_string(viz.getWindowWidth()) + "x" + std::to_string(viz.getWindowHeight()));
    NLM_LOG_INFO("Network graph: " + std::string(viz.isNetworkGraphEnabled() ? "ON" : "OFF"));
    NLM_LOG_INFO("Spike raster: " + std::string(viz.isSpikeRasterEnabled() ? "ON" : "OFF"));
    NLM_LOG_INFO("Weight matrix: " + std::string(viz.isWeightMatrixEnabled() ? "ON" : "OFF"));
    NLM_LOG_INFO("Activity heatmap: " + std::string(viz.isActivityHeatmapEnabled() ? "ON" : "OFF"));
    NLM_LOG_INFO("Auto refresh: " + std::string(viz.isAutoRefreshEnabled() ? "ON" : "OFF"));
    NLM_LOG_INFO("===================================");
}
