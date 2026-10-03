// Step 14: Periodic memory consolidation
// This module consolidates important memories and removes weak ones
// Maintains memory efficiency and removes noise

#include "step14_memory_consolidation.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

namespace Step14 {

void ConsolidateMemoriesWithThreshold(NeuralEpisodicMemory* episodicMemory,
                                      float strengthThreshold) {
    if (!episodicMemory) {
        return;
    }
    
    episodicMemory->consolidate(strengthThreshold);
}

void ConsolidateMemories(Brain::Impl* impl,
                         SimulationStep currentStep,
                         Timestamp currentTime) {
    if (!impl) {
        NLM_LOG_ERROR("ConsolidateMemories called with null impl pointer");
        return;
    }
    
    NLM_LOG_DEBUG("Consolidating memories (Step " + std::to_string(currentStep) + ")");
    
    // Periodic memory consolidation
    if (currentStep % impl->consolidationInterval == 0 && impl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        ConsolidateMemoriesWithThreshold(impl->episodicMemory, 0.3f);
    }
}

} // namespace Step14

} // namespace nlm