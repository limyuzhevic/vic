#include "BrainCognition.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// ===========================================
// NEURAL PLANNER COGNITION
// ===========================================

void BrainCognition::initializeNeuralPlanner(NeuralPlanner* planner, Brain* brain) {
    if (!planner || !brain) return;

    planner->initialize(brain);
}

void BrainCognition::updateNeuralPlanner(NeuralPlanner* planner, size_t planningDepth) {
    if (!planner) return;

    planner->setPlanningDepth(planningDepth);
}

void BrainCognition::executePlan(NeuralPlanner* planner, const std::vector<Neuron*>& neurons) {
    if (!planner) return;

    // Would execute planned action sequences based on neural activity
    // For now, this is a placeholder for real neural planning
    planner->processNeuralActivity(neurons);
}

// ===========================================
// CONCEPT FORMATION COGNITION
// ===========================================

void BrainCognition::initializeConceptFormation(ConceptFormation* conceptFormation,
                                                Brain* brain) {
    if (!conceptFormation || !brain) return;

    conceptFormation->initialize(brain);
}

void BrainCognition::updateConceptFormation(ConceptFormation* conceptFormation,
                                           const std::vector<Neuron*>& neurons) {
    if (!conceptFormation) return;

    // Would process current neural activity patterns to form concepts
    // This requires sensory state encoding
    conceptFormation->processNeuralPatterns(neurons);
}

void BrainCognition::retrieveConcepts(ConceptFormation* conceptFormation,
                                      std::vector<ConceptId>& concepts) {
    if (!conceptFormation) return;

    concepts = conceptFormation->getActiveConcepts();
}

// ===========================================
// ATTENTION COGNITION
// ===========================================

void BrainCognition::initializeAttentionalSelection(AttentionalSelection* attention,
                                                   Brain* brain) {
    if (!attention || !brain) return;

    attention->initialize(brain);
}

void BrainCognition::updateAttentionalSelection(AttentionalSelection* attention,
                                               const std::vector<Neuron*>& neurons) {
    if (!attention) return;

    attention->update(0.001);  // Default timestep

    // Apply attention to neural activity
    attention->processNeuralActivity(neurons);
}

void BrainCognition::applyAttentionToNeurons(AttentionalSelection* attention,
                                            std::vector<Neuron*>& neurons) {
    if (!attention) return;

    attention->applyFocus(neurons);
}

// ===========================================
// COGNITION STATISTICS AND ACCESSORS
// ===========================================

void BrainCognition::logCognitionStatus(NeuralPlanner* planner,
                                        ConceptFormation* conceptFormation,
                                        AttentionalSelection* attention) {
    if (planner) {
        NLM_LOG_INFO("Neural planner active: " + std::to_string(planner->isActive()));
    }
    if (conceptFormation) {
        NLM_LOG_INFO("Concepts formed: " + std::to_string(conceptFormation->getConceptCount()));
    }
    if (attention) {
        NLM_LOG_INFO("Attention focus: " + std::to_string(attention->getFocusStrength()));
    }
}

} // namespace nlm
