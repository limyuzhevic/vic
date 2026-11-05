#include "BrainCognition.hpp"
#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

void BrainCognition::configureFromConfig(const Config* config) {
    if (!config) return;
    
    planningDepth_ = config->getOr<float>("cognition.planner.depth", 5.0f);
    conceptFormThreshold_ = config->getOr<float>("cognition.concept.threshold", 0.5f);
    attentionStrength_ = config->getOr<float>("cognition.attention.strength", 1.0f);
    
    planningSteps_ = static_cast<size_t>(config->getOr<int64_t>("cognition.planner.steps", 3));
    conceptFormMinActivation_ = config->getOr<float>("cognition.concept.min_activation", 0.3f);
    attentionInhibitionStrength_ = config->getOr<float>("cognition.attention.inhibition", 0.5f);
    attentionExcitationStrength_ = config->getOr<float>("cognition.attention.excitation", 1.5f);
    
    // Initialize cognition systems
    planner_ = std::make_unique<NeuralPlanner>();
    conceptFormation_ = std::make_unique<ConceptFormation>();
    attention_ = std::make_unique<AttentionalSelection>();
    
    NLM_LOG_INFO("BrainCognition configured from config:");
    NLM_LOG_INFO("  Planning depth: " + std::to_string(planningDepth_));
    NLM_LOG_INFO("  Concept formation threshold: " + std::to_string(conceptFormThreshold_));
    NLM_LOG_INFO("  Attention strength: " + std::to_string(attentionStrength_));
}

void BrainCognition::setCognitiveParameters(float planningDepth, float conceptFormThreshold, float attentionStrength) {
    planningDepth_ = std::clamp(planningDepth, 1.0f, 10.0f);
    conceptFormThreshold_ = std::clamp(conceptFormThreshold, 0.1f, 1.0f);
    attentionStrength_ = std::clamp(attentionStrength, 0.1f, 2.0f);
}

void BrainCognition::updateAllCognition(Brain* brain, TimestepDuration timestep) {
    if (!brain) return;
    
    // Get active neurons from working memory
    std::vector<NeuronId> activeNeurons;
    if (auto* workingMemory = brain->getWorkingMemory()) {
        activeNeurons = workingMemory->getMemoryNeurons();
    }
    
    if (!activeNeurons.empty()) {
        // Update cognition systems
        updatePlanning(brain, activeNeurons);
        updateConceptFormation(brain, activeNeurons);
        updateAttention(brain, activeNeurons, timestep);
    }
    
    // Collect statistics
    collectCognitionStats(brain);
}

void BrainCognition::updatePlanning(Brain* brain, const std::vector<NeuronId>& activeNeurons) {
    if (!planner_) return;
    
    planner_->setPlanningDepth(planningDepth_);
    planner_->setPlanningSteps(planningSteps_);
    planner_->update(brain, activeNeurons);
}

void BrainCognition::updateConceptFormation(Brain* brain, const std::vector<NeuronId>& activeNeurons) {
    if (!conceptFormation_) return;
    
    conceptFormation_->setActivationThreshold(conceptFormThreshold_);
    conceptFormation_->setMinActivation(conceptFormMinActivation_);
    conceptFormation_->update(brain, activeNeurons);
    
    // Track number of active concepts
    activeConcepts_ = conceptFormation_->getActiveConceptCount();
}

void BrainCognition::updateAttention(Brain* brain, const std::vector<NeuronId>& activeNeurons, TimestepDuration timestep) {
    if (!attention_) return;
    
    attention_->setInhibitionStrength(attentionInhibitionStrength_);
    attention_->setExcitationStrength(attentionExcitationStrength_);
    attention_->update(brain, activeNeurons, timestep);
}

void BrainCognition::resetCognition(Brain* brain) {
    if (planner_) planner_->reset();
    if (conceptFormation_) conceptFormation_->reset();
    if (attention_) attention_->reset();
    
    activeConcepts_ = 0;
}

void BrainCognition::enableCognitiveFunctions(Brain* brain, bool enablePlanning, bool enableConcepts, bool enableAttention) {
    if (enablePlanning && planner_) planner_->enable(true);
    if (enableConcepts && conceptFormation_) conceptFormation_->enable(true);
    if (enableAttention && attention_) attention_->enable(true);
}

void BrainCognition::processCognitionSignals(Brain* brain, const std::vector<NeuronId>& neuralActivity) {
    if (!brain) return;
    
    // Process cognitive signals from neural activity
    // This would involve converting neural activity into cognitive representations
    
    // For example, convert firing rates to attention weights
    for (const NeuronId& neuronId : neuralActivity) {
        // Would need to get the actual neuron to read its firing rate
        // For now, this is a placeholder implementation
    }
}

void BrainCognition::planActions(Brain* brain, const std::vector<NeuronId>& motorNeurons) {
    if (!planner_) return;
    
    // Use the planner to generate action sequences based on motor neuron activity
    planner_->planActions(brain, motorNeurons);
}

void BrainCognition::formConcepts(Brain* brain, const std::vector<NeuronId>& sensoryNeurons) {
    if (!conceptFormation_) return;
    
    // Form new concepts based on sensory neuron patterns
    conceptFormation_->processSensoryPatterns(brain, sensoryNeurons);
}

void BrainCognition::selectAttention(Brain* brain, const std::vector<NeuronId>& competitors, TimestepDuration timestep) {
    if (!attention_) return;
    
    // Use attention system to select focus among competitors
    attention_->processCompetition(brain, competitors);
}

void BrainCognition::initializeCognitionSystems(Brain* brain) {
    if (!brain) return;
    
    // Establish connections between cognition systems
    if (planner_ && conceptFormation_) {
        planner_->setConceptFormationReference(conceptFormation_.get());
    }
    
    if (conceptFormation_ && attention_) {
        conceptFormation_->setAttentionReference(attention_.get());
    }
    
    if (attention_ && planner_) {
        attention_->setPlannerReference(planner_.get());
    }
}

void BrainCognition::validateCognitionState(Brain* brain) const {
    // Validate cognition system state
    if (planner_ && !planner_->isEnabled()) {
        NLM_LOG_WARNING("Planner system is disabled");
    }
    
    if (conceptFormation_ && conceptFormation_->getActiveConceptCount() > 100) {
        NLM_LOG_WARNING("High number of active concepts: " + std::to_string(conceptFormation_->getActiveConceptCount()));
    }
    
    if (attention_ && !attention_->isActive()) {
        NLM_LOG_WARNING("Attention system is not active");
    }
}

void BrainCognition::updateCognitiveConnections(Brain* brain) {
    if (!brain) return;
    
    // Update connections between cognitive systems
    // This would maintain consistency across planning, concept formation, and attention
}

void BrainCognition::processCognitionLearning(Brain* brain, TimestepDuration timestep) {
    if (!brain) return;
    
    // Process learning in cognition systems
    // This would adapt cognitive parameters based on experience
}

} // namespace nlm