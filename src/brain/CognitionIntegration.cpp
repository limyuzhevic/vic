#include "Integration.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void CognitionIntegration::initialize(NeuralPlanner* plannerPtr, ConceptFormation* conceptFormationPtr,
                                    AttentionalSelection* attentionPtr, Brain* brainPtr) {
    planner = plannerPtr;
    conceptFormation = conceptFormationPtr;
    attention = attentionPtr;
    brain = brainPtr;
}

void CognitionIntegration::update(CognitionIntegration* cognPtr, float timestep,
                                 NeuralWorkingMemory* workingMemoryPtr,
                                 NeuralPlanner* plannerPtr, ConceptFormation* conceptFormationPtr,
                                 AttentionalSelection* attentionPtr) {
    if (!cognPtr) return;
    
    // Step 9: Update attention system
    if (cognPtr->attention) {
        cognPtr->attention->update(timestep);
        
        // Apply attention to working memory winners
        if (workingMemoryPtr && !workingMemoryPtr->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = workingMemoryPtr->getMemoryNeurons();
            cognPtr->attention->processCompetition(competitors);
        }
    }
    
    // Step 10: Update concept formation
    if (cognPtr->conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
}

} // namespace nlm