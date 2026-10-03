// Step 9: Update attention system
// This module updates the attentional selection system
// Manages focus allocation and competition among memory traces

#include "step9_update_attention.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

namespace Step9 {

void ApplyAttentionToWorkingMemory(Brain::Impl* impl,
                                   const std::vector<NeuronId>& competitors) {
    if (!impl || !impl->attention) {
        return;
    }
    
    // Apply attention to working memory winners
    impl->attention->processCompetition(competitors);
}

void ProcessAttentionalCompetition(Brain::Impl* impl,
                                  const std::vector<NeuronId>& competitors) {
    if (!impl || !impl->attention) {
        return;
    }
    
    // Process competition among neuronal representations
    impl->attention->processCompetition(competitors);
}

void UpdateAttention(Brain::Impl* impl,
                    SimulationStep currentStep,
                    Timestamp currentTime) {
    if (!impl) {
        NLM_LOG_ERROR("UpdateAttention called with null impl pointer");
        return;
    }
    
    if (!impl->attention) {
        NLM_LOG_DEBUG("Attention system not initialized, skipping update");
        return;
    }
    
    NLM_LOG_DEBUG("Updating attention system (Step " + std::to_string(currentStep) + ")");
    
    impl->attention->update(impl->timestep);
    
    // Apply attention to working memory winners
    if (impl->workingMemory && !impl->workingMemory->getMemoryNeurons().empty()) {
        std::vector<NeuronId> competitors = impl->workingMemory->getMemoryNeurons();
        ProcessAttentionalCompetition(impl, competitors);
    }
}

} // namespace Step9

} // namespace nlm