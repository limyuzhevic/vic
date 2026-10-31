// Brain.cpp - Main implementation file for Brain class
// This file now contains remaining methods after extraction
// of initialization, step, plasticity, memory, cognition,
// neuromodulation, and development logic into separate files.

#include "Brain.hpp"
#include "BrainInitialization.hpp"
#include "BrainStep.hpp"
#include "BrainPlasticity.hpp"
#include "BrainMemory.hpp"
#include "BrainCognition.hpp"
#include "BrainNeuromodulation.hpp"
#include "BrainDevelopment.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Remaining Brain class implementations after extraction

void Brain::reset() {
    NLM_LOG_INFO("Resetting NLM Brain...");
    
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->totalSpikesTotal = 0;
    pImpl->isResting = false;
    pImpl->stepsSinceLastEpisode = 0;
    
    for (auto& region : pImpl->regions) {
        region->reset();
    }
    
    pImpl->spikeSystem->reset();
    pImpl->developmentalStage = DevelopmentalStage::Initial;
    
    // Reset memory systems
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
    if (pImpl->attention) pImpl->attention->reset();
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

// Additional remaining implementations...
// This file is now much cleaner and more maintainable
// with complex logic extracted into focused, specialized files

} // namespace nlm
