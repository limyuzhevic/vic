#include "VisualizationInterface.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <algorithm>
#include <cmath>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    bool consoleMode;
    
    Impl() : active(false), updateRate(30.0), consoleMode(true) {}
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    // Try to initialize with console mode first (no external dependencies)
    NLM_LOG_INFO("Initializing visualization interface (console mode)");
    pImpl->active = true;
    pImpl->consoleMode = true;
    
    return true;
}

void VisualizationInterface::update() {
    // Update visualization data - for console mode, this is a no-op
    // since we render on demand
}

void VisualizationInterface::render() {
    // Render current state to console
    if (!pImpl->active) return;
    
    // Clear screen (works in most terminals)
    std::cout << "\033[2J\033[1;1H" << std::endl;
    
    std::cout << "=== NLM Visualization Interface ===" << std::endl;
    std::cout << "Update rate: " << pImpl->updateRate << " Hz" << std::endl;
    std::cout << std::endl;
}

void VisualizationInterface::close() {
    pImpl->active = false;
    std::cout << "Visualization interface closed." << std::endl;
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
    if (!pImpl->active) return;
    
    std::cout << "=== Network Structure ===" << std::endl;
    
    std::cout << "Regions:" << std::endl;
    auto regions = brain.getRegions();
    for (size_t i = 0; i < regions.size(); ++i) {
        const auto& region = regions[i];
        std::cout << "  Region " << i << ": " << region->getName() << std::endl;
        std::cout << "    Neurons: " << region->getTotalNeuronCount() << std::endl;
        std::cout << "    Populations: " << region->getPopulationCount() << std::endl;
        
        auto populations = region->getPopulations();
        for (size_t j = 0; j < populations.size(); ++j) {
            const auto& pop = populations[j];
            std::cout << "      Population " << j << ": " << pop->getType() << std::endl;
            std::cout << "        Size: " << pop->getNeuronCount() << std::endl;
            std::cout << "        Firing rate: " << std::fixed << std::setprecision(2) 
                      << pop->getAverageFiringRate() << " Hz" << std::endl;
        }
    }
    
    std::cout << std::endl;
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    if (!pImpl->active) return;
    
    std::cout << "=== Brain Activity ===" << std::endl;
    
    std::cout << "Global Statistics:" << std::endl;
    std::cout << "  Total spikes: " << brain.getTotalSpikeCount() << std::endl;
    std::cout << "  Firing neuron count: " << brain.getFiringNeuronCount() << std::endl;
    std::cout << "  Average firing rate: " << std::fixed << std::setprecision(2) 
              << brain.getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "  Working memory traces: " << (brain.getWorkingMemory() ? 
              brain.getWorkingMemory()->getActiveTraces() : 0) << std::endl;
    std::cout << "  Episodic memory episodes: " << (brain.getEpisodicMemory() ? 
              brain.getEpisodicMemory()->getEpisodeCount() : 0) << std::endl;
    
    // Show activity by region
    auto regions = brain.getRegions();
    std::cout << std::endl << "Activity by Region:" << std::endl;
    for (size_t i = 0; i < regions.size(); ++i) {
        const auto& region = regions[i];
        std::cout << "  Region " << i << ":" << std::endl;
        
        auto populations = region->getPopulations();
        for (size_t j = 0; j < populations.size(); ++j) {
            const auto& pop = populations[j];
            std::cout << "    Population " << j << " (" << pop->getType() << "):" << std::endl;
            std::cout << "      Neurons: " << pop->getNeuronCount() << std::endl;
            std::cout << "      Active neurons: " << pop->getFiringNeuronCount() << std::endl;
            std::cout << "      Firing rate: " << std::fixed << std::setprecision(2) 
                      << pop->getAverageFiringRate() << " Hz" << std::endl;
        }
    }
    
    std::cout << std::endl;
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    if (!pImpl->active) return;
    
    std::cout << "=== Spike Raster Plot ===" << std::endl;
    
    if (spikes.empty()) {
        std::cout << "No spikes recorded." << std::endl;
    } else {
        std::cout << "Spike count: " << spikes.size() << std::endl;
        
        // Show spike times (last 10 spikes for brevity)
        auto recentSpikes = spikes;
        if (spikes.size() > 10) {
            recentSpikes = std::vector<SpikeEvent>(spikes.end() - 10, spikes.end());
        }
        
        std::cout << "Recent spike times (last " << recentSpikes.size() << " spikes):" << std::endl;
        for (size применяется to the spike: 0; spike <= recentSpikes.size(); ++spike) {
            const SpikeEvent& spike = recentSpikes[spike];
            std::cout << "  Step " << spike.step << ", Neuron ID " << spike.neuronId.index() 
                      << ", Time " << std::fixed << std::setprecision(3) << spike.time << std::endl;
        }
    }
    
    std::cout << std::endl;
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    if (!pImpl->active) return;
    
    std::cout << "=== Weight Matrix Statistics ===" << std::endl;
    
    double totalWeight = 0.0;
    size_t synapseCount = 0;
    double maxWeight = -1000.0f;
    double minWeight = 1000.0f;
    
    // Iterate through all synapses to compute statistics
    auto regions = brain.getRegions();
    for (const auto& region : regions) {
        auto synapses = region->getSynapses();
        for (const auto& syn : synapses) {
            double weight = syn->getWeight();
            totalWeight += weight;
            ++synapseCount;
            if (weight > maxWeight) maxWeight = weight;
            if (weight < minWeight) minWeight = weight;
        }
    }
    
    if (synapseCount > 0) {
        std::cout << "Total synapses: " << synapseCount << std::endl;
        std::cout << "Total weight sum: " << std::fixed << std::setprecision(4) << totalWeight << std::endl;
        std::cout << "Average weight: " << std::fixed << std::setprecision(6) 
                  << (totalWeight / synapseCount) << std::endl;
        std::cout << "Maximum weight: " << std::fixed << std::setprecision(4) << maxWeight << std::endl;
        std::cout << "Minimum weight: " << std::fixed << std::setprecision(4) << minWeight << std::endl;
        
        // Show synaptic weight distribution by type
        std::cout << std::endl << "Weight Distribution by Synapse Type:" << std::endl;
        
        auto regions = brain.getRegions();
        for (const auto& region : regions) {
            auto synapses = region->getSynapses();
            for (const auto& syn : synapses) {
                auto synType = syn->getType();
                double weight = syn->getWeight();
                std::cout << "  Synapse type " << static_cast<int>(synType) << ": " 
                          << weight << std::endl;
            }
        }
    } else {
        std::cout << "No synapses found." << std::endl;
    }
    
    std::cout << std::endl;
}

} // namespace nlm

