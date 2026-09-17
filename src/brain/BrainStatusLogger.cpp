// Brain Status Logger Implementation
// Extracted from Brain.cpp to reduce file complexity

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <sstream>

namespace nlm {

std::string Brain::StatusLogger::formatMemoryStatus(const Brain* brain) {
    std::stringstream ss;
    
    // Memory system status
    ss << "Memory Systems:\n";
    if (brain->getWorkingMemory()) {
        ss << "  Working memory: " << brain->getWorkingMemory()->getActiveTraces() 
           << " traces" << std::endl;
    }
    if (brain->getEpisodicMemory()) {
        ss << "  Episodic memory: " << brain->getEpisodicMemory()->getEpisodeCount()
           << " episodes" << std::endl;
    }
    
    return ss.str();
}

std::string Brain::StatusLogger::formatNeuromodulationStatus(const Brain* brain) {
    std::stringstream ss;
    
    // Neuromodulation status
    ss << "Neuromodulation:\n";
    if (brain->getDopamine()) {
        ss << "  Dopamine: " << brain->getDopamine()->getLevel() << std::endl;
    }
    if (brain->getCuriosity()) {
        ss << "  Curiosity: " << brain->getCuriosity()->getLevel() << std::endl;
    }
    
    return ss.str();
}

std::string Brain::StatusLogger::formatDevelopmentStatus(const Brain* brain) {
    std::stringstream ss;
    
    // Development status
    ss << "Development:\n";
    ss << "  Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
    
    return ss.str();
}

std::string Brain::StatusLogger::formatRegionDetails(const Brain* brain) {
    std::stringstream ss;
    
    ss << "Region Details:\n";
    for (const auto& region : brain->getRegions()) {
        ss << "  Region " << region->getId().index() << " (" 
           << region->getName() << "):" << std::endl;
        ss << "    Neurons: " << region->getTotalNeuronCount() << std::endl;
        ss << "    Synapses: " << region->getSynapseCount() << std::endl;
        ss << "    Avg weight: " << region->getAverageSynapticWeight() << std::endl;
    }
    
    return ss.str();
}

void Brain::StatusLogger::logStatus(const Brain* brain) {
    NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Integrated) ===");
    
    // Basic statistics
    NLM_LOG_INFO("Regions: " + std::to_string(brain->getRegionCount()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(brain->getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(brain->getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(brain->getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons (this step): " + std::to_string(brain->getFiringNeuronCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("Pending spike events: " + std::to_string(brain->getPendingSpikeEventCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(brain->getAverageFiringRate()));
    NLM_LOG_INFO("E/I ratio: " + std::to_string(brain->getExcitationInhibitionRatio()));
    
    // System status (using formatted strings)
    NLM_LOG_INFO("%s", StatusLogger::formatMemoryStatus(brain).c_str());
    NLM_LOG_INFO("%s", StatusLogger::formatNeuromodulationStatus(brain).c_str());
    NLM_LOG_INFO("%s", StatusLogger::formatDevelopmentStatus(brain).c_str());
    NLM_LOG_INFO("%s", StatusLogger::formatRegionDetails(brain).c_str());
}

} // namespace nlm