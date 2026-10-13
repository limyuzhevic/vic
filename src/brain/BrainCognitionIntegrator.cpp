#include "BrainCognitionIntegrator.hpp"
#include "../Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

BrainCognitionIntegrator::BrainCognitionIntegrator(std::shared_ptr<Config> config)
    : metaCognitionEnabled(false)
    , abstractReasoningEnabled(false)
    , socialCognitionEnabled(false)
    , planningDepth(5)
    , attentionInhibitionStrength(0.5f)
    , attentionExcitationStrength(1.5f)
{
}

void BrainCognitionIntegrator::initializePlanner(Brain* brain) {
    planner = std::make_unique<NeuralPlanner>();
    planner->initialize(brain);
    planner->setPlanningDepth(5);
}

void BrainCognitionIntegrator::initializeConceptFormation(Brain* brain) {
    conceptFormation = std::make_unique<ConceptFormation>();
    conceptFormation->initialize(brain);
}

void BrainCognitionIntegrator::initializeAttention(Brain* brain) {
    attention = std::make_unique<AttentionalSelection>();
    attention->initialize(brain);
    attention->setInhibitionStrength(0.5f);
    attention->setExcitationStrength(1.5f);
}

void BrainCognitionIntegrator::updatePlanner(Brain* brain) {
    if (planner) {
        // Update planning from memory and current state
        updatePlanningFromMemory(brain);
    }
}

void BrainCognitionIntegrator::updateConceptFormation(Brain* brain, SimulationStep step, Timestamp time) {
    if (conceptFormation) {
        formConceptsFromPatterns(brain, step, time);
    }
}

void BrainCognitionIntegrator::updateAttention(Brain* brain, TimestepDuration dt) {
    if (attention) {
        performAttentionalSelection(brain, dt);
    }
}

void BrainCognitionIntegrator::processCompetition(Brain* brain, std::vector<NeuronId> competitors) {
    if (attention) {
        attention->processCompetition(competitors);
    }
}

void BrainCognitionIntegrator::applyAttentionToWorkingMemory(Brain* brain) {
    if (attention) {
        // Apply attention to working memory winners
        auto* workingMemory = brain->pImpl->brainInitializer->getWorkingMemory();
        if (workingMemory && !workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = workingMemory->getMemoryNeurons();
            attention->processCompetition(competitors);
        }
    }
}

void BrainCognitionIntegrator::configurePlanner(Brain* brain, size_t planningDepth) {
    this->planningDepth = planningDepth;
    if (planner) {
        planner->setPlanningDepth(planningDepth);
    }
}

void BrainCognitionIntegrator::configureAttention(Brain* brain, float inhibitionStrength, float excitationStrength) {
    this->attentionInhibitionStrength = inhibitionStrength;
    this->attentionExcitationStrength = excitationStrength;
    if (attention) {
        attention->setInhibitionStrength(inhibitionStrength);
        attention->setExcitationStrength(excitationStrength);
    }
}

void BrainCognitionIntegrator::resetCognitionSystems(Brain* brain) {
    if (planner) planner->reset();
    if (conceptFormation) conceptFormation->reset();
    if (attention) attention->reset();
}

void BrainCognitionIntegrator::clearConceptHistory(Brain* brain) {
    if (conceptFormation) {
        // Clear concept formation history
    }
}

void BrainCognitionIntegrator::enableMetaCognition(Brain* brain, bool enabled) {
    metaCognitionEnabled = enabled;
    if (enabled) {
        enableAdvancedCognitionFeatures(brain);
    }
}

void BrainCognitionIntegrator::enableAbstractReasoning(Brain* brain, bool enabled) {
    abstractReasoningEnabled = enabled;
    if (enabled) {
        enableAdvancedCognitionFeatures(brain);
    }
}

void BrainCognitionIntegrator::enableSocialCognition(Brain* brain, bool enabled) {
    socialCognitionEnabled = enabled;
    if (enabled) {
        enableAdvancedCognitionFeatures(brain);
    }
}

void BrainCognitionIntegrator::setupCognitionConnections(Brain* brain) {
    // Setup connections between cognition systems
    if (planner && conceptFormation) {
        // Add cognition system interconnections
    }
}

void BrainCognitionIntegrator::updatePlanningFromMemory(Brain* brain) {
    if (planner && conceptFormation) {
        evaluateActionSequences(brain);
        selectOptimalAction(brain);
    }
}

void BrainCognitionIntegrator::formConceptsFromPatterns(Brain* brain, SimulationStep step, Timestamp time) {
    if (conceptFormation) {
        // Process current neural activity patterns to form concepts
        // This requires sensory state encoding and pattern analysis
    }
}

void BrainCognitionIntegrator::performAttentionalSelection(Brain* brain, TimestepDuration dt) {
    if (attention) {
        attention->update(dt);
    }
}

void BrainCognitionIntegrator::enableAdvancedCognitionFeatures(Brain* brain) {
    // Enable meta-cognition, abstract reasoning, and social cognition
}

void BrainCognitionIntegrator::integrateWithMemorySystems(Brain* brain) {
    if (planner && brain->pImpl->brainInitializer->getWorkingMemory()) {
        // Integrate planning with working memory
    }
    if (conceptFormation && brain->pImpl->brainInitializer->getEpisodicMemory()) {
        // Integrate concept formation with episodic memory
    }
}

void BrainCognitionIntegrator::integrateWithNeuromodulation(Brain* brain) {
    if (attention && brain->pImpl->brainInitializer->getDopamine()) {
        // Integrate attention with neuromodulation
    }
}

void BrainCognitionIntegrator::integrateWithDevelopment(Brain* brain) {
    if (planner && brain->pImpl->brainInitializer->getDevelopmentSystem()) {
        // Integrate planning with development
    }
}

void BrainCognitionIntegrator::evaluateActionSequences(Brain* brain) {
    if (planner) {
        // Evaluate possible action sequences based on current neural state
    }
}

void BrainCognitionIntegrator::selectOptimalAction(Brain* brain) {
    if (planner) {
        // Select optimal action from evaluated sequences
    }
}

void BrainCognitionIntegrator::applyLearningToCognition(Brain* brain, float learningRate) {
    if (conceptFormation) {
        // Apply learning to concept formation
    }
}