#include "BrainStatistics.hpp"
#include "../core/Logger/Logger.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <stdexcept>

namespace nlm {

BrainStatistics::BrainStatistics(std::shared_ptr<Brain> brain)
    : brain(brain)
    , totalSpikeCount(0)
    , firingNeuronCount(0)
    , averageFiringRate(0.0f)
    , totalNeuronCount(0)
    , totalSynapseCount(0)
    , activeNeuronCount(0)
    , developmentalStage(DevelopmentalStage::Initial) {
    
    if (!brain) {
        throw std::invalid_argument("Brain pointer cannot be null");
    }
}

float BrainStatistics::calculateExcitationInhibitionRatio() const {
    if (!brain) {
        return 0.0f;
    }
    
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto& region : brain->getRegions()) {
        for (const auto& syn : region->getSynapses()) {
            float weight = syn->getWeight();
            if (weight > 0) {
                totalExcitatory += weight;
            } else {
                totalInhibitory += std::abs(weight);
            }
        }
    }
    
    if (totalInhibitory > 0.0f) {
        return totalExcitatory / totalInhibitory;
    }
    return totalExcitatory > 0.0f ? std::numeric_limits<float>::infinity() : 0.0f;
}

size_t BrainStatistics::calculateActiveNeuronCount() const {
    if (!brain) {
        return 0;
    }
    
    size_t activeCount = 0;
    for (const auto& region : brain->getRegions()) {
        activeCount += region->getActiveNeuronCount();
    }
    return activeCount;
}

float BrainStatistics::calculateAverageFiringRate() const {
    if (!brain) {
        return 0.0f;
    }
    
    size_t totalNeurons = brain->getTotalNeuronCount();
    if (totalNeurons == 0) {
        return 0.0f;
    }
    
    // Get total spikes from brain (simplified calculation)
    return static_cast<float>(brain->getTotalSpikeCount()) / static_cast<float>(totalNeurons);
}

std::string BrainStatistics::generateStatusReport() const {
    if (!brain) {
        return "Brain not initialized";
    }
    
    std::stringstream ss;
    ss << "=== NLM Brain Status ===\n";
    ss << "Regions: " << brain->getRegionCount() << "\n";
    ss << "Total neurons: " << brain->getTotalNeuronCount() << "\n";
    ss << "Total synapses: " << brain->getTotalSynapseCount() << "\n";
    ss << "Active neurons: " << brain->getActiveNeuronCount() << "\n";
    ss << "Firing neurons (this step): " << brain->getFiringNeuronCount() << "\n";
    ss << "Total spikes: " << brain->getTotalSpikeCount() << "\n";
    ss << "Average firing rate: " << std::fixed << std::setprecision(3) 
       << brain->getAverageFiringRate() << "\n";
    ss << "E/I ratio: " << std::fixed << std::setprecision(3) 
       << calculateExcitationInhibitionRatio() << "\n";
    
    ss << "\nMemory System Status:\n";
    if (brain->getWorkingMemory()) {
        ss << "  Working memory traces: " << brain->getWorkingMemory()->getActiveTraces() << "\n";
    }
    if (brain->getEpisodicMemory()) {
        ss << "  Episodic memory episodes: " << brain->getEpisodicMemory()->getEpisodeCount() << "\n";
    }
    
    ss << "\nNeuromodulation Status:\n";
    if (brain->getDopamine()) {
        ss << "  Dopamine level: " << std::fixed << std::setprecision(3) 
           << brain->getDopamine()->getLevel() << "\n";
    }
    
    ss << "\nDevelopment Status:\n";
    ss << "  Developmental stage: " << static_cast<int>(brain->getDevelopmentalStage()) << "\n";
    
    return ss.str();
}

} // namespace nlm