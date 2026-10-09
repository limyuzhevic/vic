#include "CognitionSystem.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Neuron.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

CognitionSystem::CognitionSystem(NeuralPlanner* planner,
                                 ConceptFormation* conceptFormation,
                                 AttentionalSelection* attention)
    : planner(planner), conceptFormation(conceptFormation),
      attention(attention), attentionEnabled(true), 
      conceptFormationEnabled(true), planningEnabled(true) {
}

void CognitionSystem::updateCognition(TimestepDuration dt) {
    // Update attentional selection with current neural activity
    updateAttention(std::vector<NeuronId>()); // Would need to pass active neurons
    
    // Form concepts from neural activity patterns
    updateConceptFormation(std::vector<NeuronId>()); // Would need to pass active neurons
    
    // Generate action plans
    updatePlanning(std::vector<NeuronId>()); // Would need to pass active neurons
}

void CognitionSystem::updateAttention(const std::vector<NeuronId>& competitors) {
    if (attention) {
        // Process competition for attentional focus
        // In real implementation, would process with actual neural activity patterns
        attention->update(0.001); // Using default timestep
        
        if (!competitors.empty()) {
            // Would normally process the competitors list
            attention->processCompetition(competitors);
        }
    }
}

void CognitionSystem::updateConceptFormation(const std::vector<NeuronId>& activeNeurons) {
    if (conceptFormation) {
        // Form new concepts from neural activity patterns
        // Would process the active neurons to identify patterns
        conceptFormation->update(0.001); // Using default timestep
    }
}

void CognitionSystem::updatePlanning(const std::vector<NeuronId>& activeNeurons) {
    if (planner) {
        // Generate action plans based on current neural state
        planner->update(0.001); // Using default timestep
        
        // Would process active neurons for planning context
    }
}

float CognitionSystem::getAttentionStrength() const {
    return attention ? attention->getAttentionStrength() : 0.0f;
}

float CognitionSystem::getPlanningDepth() const {
    return planner ? static_cast<float>(planner->getPlanningDepth()) : 0.0f;
}

float CognitionSystem::getConceptActivation() const {
    return conceptFormation ? conceptFormation->getActivationLevel() : 0.0f;
}

void CognitionSystem::setAttentionStrength(float strength) {
    if (attention) {
        attention->setInhibitionStrength(1.0f - strength);
        attention->setExcitationStrength(strength);
    }
}

void CognitionSystem::setPlanningDepth(size_t depth) {
    if (planner) {
        planner->setPlanningDepth(depth);
    }
}

void CognitionSystem::enableAttention(bool enable) {
    attentionEnabled = enable;
}

} // namespace nlm