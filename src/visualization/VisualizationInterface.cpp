#include "VisualizationInterface.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    int lastNeuronsShown;
    int lastStepsShown;
    
    Impl() : active(false), updateRate(30.0), lastNeuronsShown(0), lastStepsShown(0) {}
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    // TODO PHASE 2: Initialize real visualization (e.g., with GLFW, SDL, or custom)
    // PLACEHOLDER: No visualization in Phase 1
    pImpl->active = false;
    std::cout << "[Visualization] Console visualization initialized (basic mode)" << std::endl;
    return true;
}

void VisualizationInterface::update() {
    // PLACEHOLDER: No visualization in Phase 1
    pImpl->active = true;
}

void VisualizationInterface::render() {
    if (!pImpl->active) return;
    
    // Clear screen and show basic status
    std::cout << "\033[2J\033[H"; // Clear screen (ANSI escape codes)
    std::cout << "===========================================================" << std::endl;
    std::cout << "                     NLM NEURAL NETWORK VISUALIZATION" << std::endl;
    std::cout << "===========================================================" << std::endl;
}

void VisualizationInterface::close() {
    pImpl->active = false;
    std::cout << "\033[0m" << std::endl; // Reset terminal
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

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    std::cout << "[Visualization] Network visualization:") << std::endl;
    std::cout << "  Total neurons: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "  Total synapses: " << brain.getTotalSynapseCount() << std::endl;
    std::cout << "  Active neurons: " << brain.getFiringNeuronCount() << std::endl;
    std::cout << "  Average firing rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
    
    // Show region information
    auto regions = brain.getRegions();
    std::cout << "  Regions: " << regions.size() << std::endl;
    for (size_t i = 0; i < regions.size(); ++i) {
        const auto& region = regions[i];
        std::cout << "    Region " << i << ": " << region->getNeuronCount() << " neurons, " 
                  << region->getSynapseCount() << " synapses" << std::endl;
    }
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    std::cout << "[Visualization] Activity visualization:") << std::endl;
    
    double avgFiringRate = brain.getAverageFiringRate();
    int totalFiring = brain.getFiringNeuronCount();
    int totalNeurons = brain.getTotalNeuronCount();
    double firingPercentage = (totalNeurons > 0) ? (totalFiring * 100.0 / totalNeurons) : 0.0;
    
    std::cout << "  Firing neurons: " << totalFiring << " (" << std::fixed << std::setprecision(1) 
              << firingPercentage << "%)" << std::endl;
    std::cout << "  Average firing rate: " << std::fixed << std::setprecision(2) << avgFiringRate << " Hz" << std::endl;
    
    // Show activity histogram (simple text-based)
    if (totalNeurons > 0) {
        std::cout << "  Activity histogram (last 10 neurons):" << std::endl;
        // This would require access to individual neuron firing rates
        // For now, just show placeholder
        std::cout << "    [=== placeholder histogram ===]" << std::endl;
    }
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    std::cout << "[Visualization] Spike raster plot:") << std::endl;
    std::cout << "  Total spikes recorded: " << spikes.size() << std::endl;
    
    if (spikes.size() > 0) {
        // Show last 10 spikes
        int showCount = std::min(10, (int)spikes.size());
        std::cout << "  Recent spikes:" << std::endl;
        for (int i = 0; i < showCount; ++i) {
            const auto& spike = spikes[spikes.size() - 1 - i];
            std::cout << "    Neuron " << spike.neuronId << " at time " 
                      << std::fixed << std::setprecision(3) << spike.time << "s" << std::endl;
        }
    }
    
    // Simple ASCII spike representation (placeholder)
    std::cout << "  Spike timeline (placeholder):" << std::endl;
    std::cout << "    [===== spike activity =====]" << std::endl;
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    std::cout << "[Visualization] Weight matrix visualization:") << std::endl;
    
    // Get synapse information if available
    auto synapses = brain.getAllSynapses();
    if (synapses.size() > 0) {
        float minWeight = synapses[0]->getWeight();
        float maxWeight = synapses[0]->getWeight();
        float totalWeight = 0.0f;
        
        for (const auto& synapse : synapses) {
            float weight = synapse->getWeight();
            minWeight = std::min(minWeight, weight);
            maxWeight = std::max(maxWeight, weight);
            totalWeight += weight;
        }
        
        std::cout << "  Synapses: " << synapses.size() << std::endl;
        std::cout << "  Weight range: [" << std::fixed << std::setprecision(4) << minWeight 
                  << ", " << maxWeight << "]" << std::endl;
        std::cout << "  Average weight: " << std::fixed << std::setprecision(4) << (totalWeight / synapses.size()) << std::endl;
        
        // Show weight distribution (placeholder)
        std::cout << "  Weight distribution (placeholder):" << std::endl;
        std::cout << "    [low] [=== medium ===] [high]" << std::endl;
    }
}

} // namespace nlm
