#include "VisualizationInterface.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <chrono>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    std::chrono::steady_clock::time_point lastUpdate;
    
    // Visualization data
    std::vector<std::pair<SimulationStep, float>> spikeHistory;
    std::vector<std::pair<SimulationStep, float>> firingRateHistory;
    std::vector<std::pair<SimulationStep, float>> weightHistory;
    std::vector<std::pair<SimulationStep, float>> activityHistory;
    
    // Network topology visualization data
    std::vector<RegionId> regionOrder;
    std::vector<std::string> regionNames;
    
    // Configuration
    bool showNetworkTopology;
    bool showSpikeRaster;
    bool showWeightMatrix;
    bool showActivityHeatmap;
    bool showMetrics;
    int maxHistoryPoints;
    
    Impl() : active(false), updateRate(30.0), lastUpdate(std::chrono::steady_clock::now()),
             showNetworkTopology(true), showSpikeRaster(true), showWeightMatrix(false),
             showActivityHeatmap(true), showMetrics(true), maxHistoryPoints(1000) {}
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    // Initialize real visualization
    pImpl->active = true;
    pImpl->lastUpdate = std::chrono::steady_clock::now();
    
    // Initialize visualization data
    pImpl->spikeHistory.clear();
    pImpl->firingRateHistory.clear();
    pImpl->weightHistory.clear();
    pImpl->activityHistory.clear();
    
    // Get brain regions for visualization
    if (auto brain = getBrainFromGlobal()) {
        auto regionIds = brain->getRegionIds();
        pImpl->regionOrder = regionIds;
        for (const auto& regionId : regionIds) {
            auto* region = brain->getRegion(regionId);
            if (region) {
                pImpl->regionNames.push_back(region->getName());
            }
        }
    }
    
    NLM_LOG_INFO("Visualization interface initialized successfully");
    return true;
}

void VisualizationInterface::update() {
    if (!pImpl->active) return;
    
    // Check if it's time to update visualization
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration<double>(now - pImpl->lastUpdate).count();
    
    if (elapsed >= 1.0 / pImpl->updateRate) {
        updateVisualizationData();
        pImpl->lastUpdate = now;
    }
}

void VisualizationInterface::render() {
    if (!pImpl->active) return;
    
    // Render different visualization modes based on user preference
    if (pImpl->showNetworkTopology) {
        renderNetworkTopology();
    }
    
    if (pImpl->showSpikeRaster) {
        renderSpikeRaster();
    }
    
    if (pImpl->showWeightMatrix) {
        renderWeightMatrix();
    }
    
    if (pImpl->showActivityHeatmap) {
        renderActivityHeatmap();
    }
    
    if (pImpl->showMetrics) {
        renderMetricsPanel();
    }
}

void VisualizationInterface::close() {
    pImpl->active = false;
    pImpl->spikeHistory.clear();
    pImpl->firingRateHistory.clear();
    pImpl->weightHistory.clear();
    pImpl->activityHistory.clear();
    NLM_LOG_INFO("Visualization interface closed");
}

bool VisualizationInterface::isActive() const {
    return pImpl->active;
}

void VisualizationInterface::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
    if (hz > 0) {
        NLM_LOG_INFO("Visualization update rate set to " + std::to_string(hz) + " Hz");
    }
}

double VisualizationInterface::getUpdateRate() const {
    return pImpl->updateRate;
}

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    if (!pImpl->active) return;
    
    // Update network visualization data
    auto regionIds = brain.getRegionIds();
    pImpl->regionOrder.clear();
    pImpl->regionNames.clear();
    
    for (const auto& regionId : regionIds) {
        pImpl->regionOrder.push_back(regionId);
        auto* region = brain.getRegion(regionId);
        if (region) {
            pImpl->regionNames.push_back(region->getName());
        }
    }
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    if (!pImpl->active) return;
    
    auto currentStep = brain.getCurrentStep();
    
    // Collect activity data
    float avgFiringRate = brain.getAverageFiringRate();
    size_t totalSpikes = brain.getTotalSpikeCount();
    size_t activeNeurons = brain.getActiveNeuronCount();
    float eiratio = brain.getExcitationInhibitionRatio();
    
    // Store in history
    pImpl->firingRateHistory.push_back({currentStep, avgFiringRate});
    pImpl->activityHistory.push_back({currentStep, static_cast<float>(activeNeurons)});
    
    // Trim history if it gets too large
    if (pImpl->firingRateHistory.size() > static_cast<size_t>(pImpl->maxHistoryPoints)) {
        pImpl->firingRateHistory.erase(pImpl->firingRateHistory.begin());
    }
    if (pImpl->activityHistory.size() > static_cast<size_t>(pImpl->maxHistoryPoints)) {
        pImpl->activityHistory.erase(pImpl->activityHistory.begin());
    }
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    if (!pImpl->active) return;
    
    // Update spike history
    for (const auto& spike : spikes) {
        pImpl->spikeHistory.push_back({spike.step, spike.weight});
    }
    
    // Trim history if it gets too large
    if (pImpl->spikeHistory.size() > static_cast<size_t>(pImpl->maxHistoryPoints)) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    if (!pImpl->active) return;
    
    // Get average synaptic weight
    float avgWeight = 0.0f;
    size_t synapseCount = 0;
    
    for (const auto& region : brain.getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            avgWeight += syn->getWeight();
            synapseCount++;
        }
    }
    
    if (synapseCount > 0) {
        avgWeight /= synapseCount;
        auto currentStep = brain.getCurrentStep();
        pImpl->weightHistory.push_back({currentStep, avgWeight});
        
        // Trim history
        if (pImpl->weightHistory.size() > static_cast<size_t>(pImpl->maxHistoryPoints)) {
            pImpl->weightHistory.erase(pImpl->weightHistory.begin());
        }
    }
}

void VisualizationInterface::updateVisualizationData() {
    // This would update the visualization data based on current brain state
    // For now, we just keep the existing data
}

void VisualizationInterface::renderNetworkTopology() {
    if (pImpl->regionNames.empty()) {
        return;
    }
    
    std::cout << "=== NLM Network Topology ===" << std::endl;
    std::cout << "Regions: " << pImpl->regionNames.size() << std::endl;
    
    for (size_t i = 0; i < pImpl->regionOrder.size(); ++i) {
        std::cout << "  " << pImpl->regionOrder[i].index() << ": " << pImpl->regionNames[i] << std::endl;
    }
    
    std::cout << "===============================" << std::endl;
}

void VisualizationInterface::renderSpikeRaster() {
    if (pImpl->spikeHistory.empty()) {
        std::cout << "=== Spike Raster Plot ===" << std::endl;
        std::cout << "No spike data available" << std::endl;
        std::cout << "=========================" << std::endl;
        return;
    }
    
    std::cout << "=== Spike Raster Plot ===" << std::endl;
    std::cout << "Total spikes: " << pImpl->spikeHistory.size() << std::endl;
    std::cout << "Spike history (last " << std::min(10, (int)pImpl->spikeHistory.size()) << "):" << std::endl;
    
    for (size_t i = std::max(0UL, pImpl->spikeHistory.size() - 10); i < pImpl->spikeHistory.size(); ++i) {
        std::cout << "  Step " << pImpl->spikeHistory[i].first 
                  << ", Weight: " << std::fixed << std::setprecision(3) << pImpl->spikeHistory[i].second << std::endl;
    }
    
    std::cout << "=========================" << std::endl;
}

void VisualizationInterface::renderWeightMatrix() {
    if (pImpl->weightHistory.empty()) {
        std::cout << "=== Weight Matrix ===" << std::endl;
        std::cout << "No weight data available" << std::endl;
        std::cout << "=====================" << std::endl;
        return;
    }
    
    std::cout << "=== Weight Matrix ===" << std::endl;
    std::cout << "Average weight over time:" << std::endl;
    
    float minWeight = std::numeric_limits<float>::max();
    float maxWeight = std::numeric_limits<float>::lowest();
    for (const auto& entry : pImpl->weightHistory) {
        minWeight = std::min(minWeight, entry.second);
        maxWeight = std::max(maxWeight, entry.second);
    }
    
    std::cout << "  Min: " << minWeight << std::endl;
    std::cout << "  Max: " << maxWeight << std::endl;
    std::cout << "  Current: " << pImpl->weightHistory.back().second << std::endl;
    
    // Simple heatmap representation
    std::cout << "  Heatmap:" << std::endl;
    for (size_t i = 0; i < pImpl->weightHistory.size(); ++i) {
        float weight = pImpl->weightHistory[i].second;
        int barLength = static_cast<int>((weight - minWeight) / (maxWeight - minWeight + 0.0001f) * 50);
        std::cout << "    [";
        for (int j = 0; j < barLength; ++j) std::cout << "*";
        std::cout << "] " << std::fixed << std::setprecision(2) << weight << std::endl;
    }
    
    std::cout << "=====================" << std::endl;
}

void VisualizationInterface::renderActivityHeatmap() {
    if (pImpl->activityHistory.empty()) {
        std::cout << "=== Activity Heatmap ===" << std::endl;
        std::cout << "No activity data available" << std::endl;
        std::cout << "=======================" << std::endl;
        return;
    }
    
    std::cout << "=== Activity Heatmap ===" << std::endl;
    std::cout << "Active neurons over time:" << std::endl;
    
    float minActivity = std::numeric_limits<float>::max();
    float maxActivity = std::numeric_limits<float>::lowest();
    for (const auto& entry : pImpl->activityHistory) {
        minActivity = std::min(minActivity, entry.second);
        maxActivity = std::max(maxActivity, entry.second);
    }
    
    // Show last 20 data points
    size_t startIdx = std::max(0UL, pImpl->activityHistory.size() - 20);
    for (size_t i = startIdx; i < pImpl->activityHistory.size(); ++i) {
        float activity = pImpl->activityHistory[i].second;
        int barLength = static_cast<int>((activity - minActivity) / (maxActivity - minActivity + 0.0001f) * 40);
        std::cout << "  Step " << pImpl->activityHistory[i].first << ": [";
        for (int j = 0; j < barLength; ++j) std::cout << "#";
        std::cout << "] " << static_cast<int>(activity) << std::endl;
    }
    
    std::cout << "=======================" << std::endl;
}

void VisualizationInterface::renderMetricsPanel() {
    std::cout << "=== NLM Metrics ===" << std::endl;
    std::cout << "Visualization System:" << std::endl;
    std::cout << "  Update rate: " << std::fixed << std::setprecision(1) << pImpl->updateRate << " Hz" << std::endl;
    std::cout << "  Active: " << (pImpl->active ? "Yes" : "No") << std::endl;
    std::cout << "  History points: " << pImpl->firingRateHistory.size() << std::endl;
    
    if (!pImpl->regionNames.empty()) {
        std::cout << "  Regions configured: " << pImpl->regionNames.size() << std::endl;
    }
    
    std::cout << "  Visualization modes:" << std::endl;
    std::cout << "    Network topology: " << (pImpl->showNetworkTopology ? "ON" : "OFF") << std::endl;
    std::cout << "    Spike raster: " << (pImpl->showSpikeRaster ? "ON" : "OFF") << std::endl;
    std::cout << "    Weight matrix: " << (pImpl->showWeightMatrix ? "ON" : "OFF") << std::endl;
    std::cout << "    Activity heatmap: " << (pImpl->showActivityHeatmap ? "ON" : "OFF") << std::endl;
    std::cout << "    Metrics panel: " << (pImpl->showMetrics ? "ON" : "OFF") << std::endl;
    
    std::cout << "====================" << std::endl;
}

// Global function to get brain reference for visualization
Brain* VisualizationInterface::getBrainFromGlobal() {
    // This would need to be implemented based on how the brain is stored globally
    // For now, return nullptr as this is just a placeholder implementation
    return nullptr;
}

} // namespace nlm