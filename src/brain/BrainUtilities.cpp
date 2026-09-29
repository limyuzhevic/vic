#include "BrainUtilities.hpp"

namespace nlm {

void BrainUtilities::logStatus(const Brain& brain) {
    NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Integrated) ===");
    NLM_LOG_INFO("Regions: " + std::to_string(brain.getRegionCount()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(brain.getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(brain.getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(brain.getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons (this step): " + std::to_string(brain.getFiringNeuronCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(brain.getTotalSpikeCount()));
    NLM_LOG_INFO("Pending spike events: " + std::to_string(brain.getPendingSpikeEventCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(brain.getAverageFiringRate()));
    NLM_LOG_INFO("E/I ratio: " + std::to_string(brain.getExcitationInhibitionRatio()));
    
    // Memory system status
    if (brain.getWorkingMemory()) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(brain.getWorkingMemory()->getActiveTraces()));
    }
    if (brain.getEpisodicMemory()) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(brain.getEpisodicMemory()->getEpisodeCount()));
    }
    
    // Neuromodulation status
    if (brain.getDopamine()) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(brain.getDopamine()->getLevel()));
    }
    
    // Development status
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(brain.getDevelopmentalStage())));
    
    for (const auto& region : brain.getRegions()) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                    " (" + region->getName() + "): " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                    std::to_string(region->getSynapseCount()) + " synapses, " +
                    "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
}

void BrainUtilities::reset(Brain& brain) {
    NLM_LOG_INFO("Resetting NLM Brain...");
    
    brain.pImpl->currentStep = 0;
    brain.pImpl->currentTime = 0.0;
    brain.pImpl->totalSpikesThisStep = 0;
    brain.pImpl->totalSpikesTotal = 0;
    brain.pImpl->isResting = false;
    brain.pImpl->stepsSinceLastEpisode = 0;
    
    for (auto& region : brain.pImpl->regions) {
        region->reset();
    }
    
    brain.getSpikeSystem()->reset();
    brain.pImpl->developmentalStage = DevelopmentalStage::Initial;
    
    // Reset memory systems
    if (brain.getWorkingMemory()) brain.getWorkingMemory()->clear();
    if (brain.getEpisodicMemory()) brain.getEpisodicMemory()->clear();
    if (brain.getAssociativeMemory()) brain.getAssociativeMemory()->clear();
    if (brain.getAttention()) brain.getAttention()->reset();
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

float BrainUtilities::getExcitationInhibitionRatio(const Brain& brain) {
    return brain.getExcitationInhibitionRatio();
}

} // namespace nlm