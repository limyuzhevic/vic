#pragma once

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void Brain::Impl::reset() {
    NLM_LOG_INFO("Resetting NLM Brain...");
    
    currentStep = 0;
    currentTime = 0.0;
    totalSpikesThisStep = 0;
    totalSpikesTotal = 0;
    isResting = false;
    stepsSinceLastEpisode = 0;
    
    for (auto& region : regions) {
        region->reset();
    }
    
    if (spikeSystem) {
        spikeSystem->reset();
    }
    
    developmentalStage = DevelopmentalStage::Initial;
    
    // Reset memory systems
    if (workingMemory) workingMemory->clear();
    if (episodicMemory) episodicMemory->clear();
    if (associativeMemory) associativeMemory->clear();
    if (attention) attention->reset();
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

} // namespace nlm