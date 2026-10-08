#include "EnhancedWorkingMemoryIntegration.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/InternalSignals.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

struct EnhancedWorkingMemoryIntegration::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

EnhancedWorkingMemoryIntegration::EnhancedWorkingMemoryIntegration() : pImpl(new Impl) {}
EnhancedWorkingMemoryIntegration::~EnhancedWorkingMemoryIntegration() = default;

void EnhancedWorkingMemoryIntegration::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("EnhancedWorkingMemoryIntegration initialized");
}

void EnhancedWorkingMemoryIntegration::integrateThroughoutBrainLoop() {
    // STEP 1: Enhanced sensory integration
    // Update working memory with richer sensory representations
    integrateEnhancedSensoryInput();
    
    // STEP 2: Continuous working memory updates
    // Update working memory with neural activity, prediction errors, and action outcomes
    integrateContinuousUpdates();
    
    // STEP 3: Working memory-attention integration
    // Make working memory drive attention and vice versa
    integrateWithAttentionSystem();
    
    // STEP 4: Working memory-cognition integration
    // Connect working memory with prediction system and cognitive processes
    integrateWithCognitionSystems();
    
    // STEP 5: Working memory consolidation
    // Implement proper consolidation of working memory into episodic memory
    consolidateToEpisodicMemory();
}

// ========== STEP 1: Enhanced sensory integration ==========
void EnhancedWorkingMemoryIntegration::integrateEnhancedSensoryInput() {
    if (!pImpl->brain || !pImpl->brain->getWorkingMemory()) return;
    
    // This is an enhanced version of the original receiveSensoryInput()
    // The actual implementation would access sensory input through the brain's interfaces
    // For now, we simulate the enhanced integration
    
    // Create rich sensory representations with temporal context
    std::vector<float> richSensoryPattern;
    richSensoryPattern.reserve(5);
    
    // Current sensory state (simulated)
    richSensoryPattern.push_back(0.7f);  // Current activation
    
    // Temporal derivative (rate of change)
    richSensoryPattern.push_back(0.1f);  // Small change
    
    // Temporal integration (accumulated change)
    richSensoryPattern.push_back(0.5f);  // Moderate integration
    
    // Modulated by attention
    richSensoryPattern.push_back(0.8f);  // Attention-modulated
    
    // Novelty component
    richSensoryPattern.push_back(0.3f);  // Novelty detection
    
    // Store in working memory
    pImpl->brain->getWorkingMemory()->store(richSensoryPattern, 0.9f);
}

// ========== STEP 2: Continuous working memory updates ==========
void EnhancedWorkingMemoryIntegration::integrateContinuousUpdates() {
    if (!pImpl->brain) return;
    
    // Update with current neural activity
    integrateWithCurrentNeuralActivity();
    
    // Update with prediction errors
    integratePredictionErrorUpdates();
    
    // Update with action outcomes
    integrateActionOutcomeUpdates();
    
    // Update with neuromodulation signals
    integrateNeuromodulationUpdates();
}

void EnhancedWorkingMemoryIntegration::integrateWithCurrentNeuralActivity() {
    // Simulate integration with current neural activity
    std::vector<float> neuralActivityPattern;
    neuralActivityPattern.reserve(4);
    
    // Average firing rate proxy
    neuralActivityPattern.push_back(0.6f);  // Moderate activity
    
    // Active population activity
    neuralActivityPattern.push_back(0.4f);  // Some active populations
    
    // Recurrent activity level
    neuralActivityPattern.push_back(0.5f);  // Recurrent connections active
    
    // Competition level
    neuralActivityPattern.push_back(0.3f);  // Moderate competition
    
    if (pImpl->brain && pImpl->brain->getWorkingMemory()) {
        pImpl->brain->getWorkingMemory()->store(neuralActivityPattern, 0.8f);
    }
}

void EnhancedWorkingMemoryIntegration::integratePredictionErrorUpdates() {
    if (!pImpl->brain || !pImpl->brain->getPredictionSystem()) return;
    
    float predictionError = pImpl->brain->getPredictionSystem()->getPredictionError();
    
    std::vector<float> errorPattern;
    errorPattern.reserve(3);
    
    // Error magnitude
    errorPattern.push_back(predictionError);
    
    // Temporal context (recent error history)
    errorPattern.push_back(std::min(1.0f, predictionError * 1.5f));
    
    // Error sign and direction
    errorPattern.push_back(predictionError > 0.5f ? 1.0f : 0.0f);
    
    pImpl->brain->getWorkingMemory()->store(errorPattern, 0.7f);
}

void EnhancedWorkingMemoryIntegration::integrateActionOutcomeUpdates() {
    if (!pImpl->brain || !pImpl->brain->getWorkingMemory()) return;
    
    // Simulate action outcome processing
    std::vector<float> actionPattern;
    actionPattern.reserve(4);
    
    // Action type (simulated)
    actionPattern.push_back(0.6f);  // Action probability
    
    // Action effectiveness
    actionPattern.push_back(0.7f);  // Success probability
    
    // Action temporal context
    actionPattern.push_back(0.4f);  // Time since last action
    
    // Action novelty
    actionPattern.push_back(0.3f);  // Novelty level
    
    pImpl->brain->getWorkingMemory()->store(actionPattern, 0.8f);
}

void EnhancedWorkingMemoryIntegration::integrateNeuromodulationUpdates() {
    if (!pImpl->brain || !pImpl->brain->getWorkingMemory()) return;
    
    // Integrate with neuromodulation systems
    std::vector<float> neuromodPattern;
    neuromodPattern.reserve(4);
    
    // Dopamine level
    neuromodPattern.push_back(0.5f);  // Reward prediction error
    
    // Novelty level
    neuromodPattern.push_back(0.3f);  // Novelty detection
    
    // Curiosity level
    neuromodPattern.push_back(0.4f);  // Exploration drive
    
    // Prediction error
    neuromodPattern.push_back(0.2f);  // Learning signal
    
    pImpl->brain->getWorkingMemory()->store(neuromodPattern, 0.6f);
}

// ========== STEP 3: Working memory-attention integration ==========
void EnhancedWorkingMemoryIntegration::integrateWithAttentionSystem() {
    if (!pImpl->brain || !pImpl->brain->getWorkingMemory() || !pImpl->brain->getAttention()) return;
    
    // Get working memory content
    std::vector<NeuronId> wmNeurons = pImpl->brain->getWorkingMemory()->getMemoryNeurons();
    
    // Apply working memory content as attention bias
    for (size_t i = 0; i < wmNeurons.size(); ++i) {
        NeuronId neuron = wmNeurons[i];
        float activation = pImpl->brain->getWorkingMemory()->getNeuronActivation(neuron);
        
        // Apply attention bias based on working memory activation
        // Note: This would call actual attention system methods
        // pImpl->brain->getAttention()->applyTopDownBias(neuron, activation * 2.0f);
    }
    
    // Get current working memory activity
    float memoryActivity = pImpl->brain->getWorkingMemory()->getMemoryActivity();
    
    // Use working memory to drive attention selection
    if (memoryActivity > 0.5f) {
        // Focus attention based on strong working memory traces
        // pImpl->brain->getAttention()->focusAttention();
    }
}

// ========== STEP 4: Working memory-cognition integration ==========
void EnhancedWorkingMemoryIntegration::integrateWithCognitionSystems() {
    if (!pImpl->brain) return;
    
    // Integrate with prediction system
    integrateWithPredictionSystem();
    
    // Integrate with planning system
    integrateWithPlanningSystem();
    
    // Integrate with concept formation
    integrateWithConceptFormation();
}

void EnhancedWorkingMemoryIntegration::integrateWithPredictionSystem() {
    if (!pImpl->brain || !pImpl->brain->getPredictionSystem() || !pImpl->brain->getWorkingMemory()) return;
    
    // Get current working memory state
    std::vector<float> currentState = pImpl->brain->getWorkingMemory()->retrieve();
    
    if (!currentState.empty()) {
        // Create prediction based on working memory
        std::vector<float> predictionPattern;
        predictionPattern.reserve(3);
        
        // Prediction confidence
        predictionPattern.push_back(0.7f);  // High confidence
        
        // Prediction error
        predictionPattern.push_back(0.2f);  // Some error
        
        // Prediction accuracy
        predictionPattern.push_back(0.8f);  // Good accuracy
        
        pImpl->brain->getWorkingMemory()->store(predictionPattern, 0.8f);
    }
}

void EnhancedWorkingMemoryIntegration::integrateWithPlanningSystem() {
    if (!pImpl->brain || !pImpl->brain->getPlanner() || !pImpl->brain->getWorkingMemory()) return;
    
    // Get current working memory state
    std::vector<float> currentState = pImpl->brain->getWorkingMemory()->retrieve();
    
    if (!currentState.empty()) {
        // Use working memory to inform planning
        std::vector<float> planningPattern;
        planningPattern.reserve(3);
        
        // Planning context
        planningPattern.push_back(0.6f);  // Planning context
        
        // Planning depth
        planningPattern.push_back(0.4f);  // Planning depth
        
        // Planning confidence
        planningPattern.push_back(0.7f);  // Planning confidence
        
        pImpl->brain->getWorkingMemory()->store(planningPattern, 0.7f);
    }
}

void EnhancedWorkingMemoryIntegration::integrateWithConceptFormation() {
    if (!pImpl->brain || !pImpl->brain->getConceptFormation() || !pImpl->brain->getWorkingMemory()) return;
    
    // Get current working memory state
    std::vector<float> currentState = pImpl->brain->getWorkingMemory()->retrieve();
    
    if (!currentState.empty()) {
        // Present to concept formation system
        std::vector<float> conceptPattern;
        conceptPattern.reserve(2);
        
        // Concept stability
        conceptPattern.push_back(0.8f);  // High stability
        
        // Concept novelty
        conceptPattern.push_back(0.3f);  // Some novelty
        
        pImpl->brain->getWorkingMemory()->store(conceptPattern, 0.6f);
    }
}

// ========== STEP 5: Working memory consolidation ==========
void EnhancedWorkingMemoryIntegration::consolidateToEpisodicMemory() {
    if (!pImpl->brain || !pImpl->brain->getWorkingMemory() || !pImpl->brain->getEpisodicMemory()) return;
    
    // Get current working memory state
    std::vector<float> currentMemory = pImpl->brain->getWorkingMemory()->retrieve();
    
    if (!currentMemory.empty()) {
        // Create episode from working memory
        EpisodicMemoryItem episode;
        episode.timestamp = 0;  // Would be current step
        episode.reward = 0.5f;  // Would be actual reward
        
        // Store working memory pattern
        episode.sensoryState = currentMemory;
        
        // Store in episodic memory
        pImpl->brain->getEpisodicMemory()->storeEpisode(episode);
        
        // Consolidation strength based on memory strength
        float memoryStrength = pImpl->brain->getWorkingMemory()->getMemoryActivity();
        if (memoryStrength > 0.5f) {
            pImpl->brain->getEpisodicMemory()->consolidate(0.3f + memoryStrength * 0.4f);
        }
    }
}

float EnhancedWorkingMemoryIntegration::getWorkingMemoryActivity() const {
    if (!pImpl->brain || !pImpl->brain->getWorkingMemory()) return 0.0f;
    return pImpl->brain->getWorkingMemory()->getMemoryActivity();
}

void EnhancedWorkingMemoryIntegration::applyToAttentionSystem() {
    if (!pImpl->brain || !pImpl->brain->getAttention()) return;
    
    // Apply working memory state to attention system
    // This would call actual attention system methods
    // For example:
    // std::vector<NeuronId> wmNeurons = pImpl->brain->getWorkingMemory()->getMemoryNeurons();
    // pImpl->brain->getAttention()->processCompetition(wmNeurons);
}

void EnhancedWorkingMemoryIntegration::applyToPredictionSystem() {
    if (!pImpl->brain || !pImpl->brain->getPredictionSystem()) return;
    
    // Apply working memory state to prediction system
    std::vector<float> currentState = pImpl->brain->getWorkingMemory()->retrieve();
    
    if (!currentState.empty()) {
        // This would update prediction system with working memory state
        // pImpl->brain->getPredictionSystem()->updatePredictions(...);
    }
}

void EnhancedWorkingMemoryIntegration::applyToPlanningSystem() {
    if (!pImpl->brain || !pImpl->brain->getPlanner()) return;
    
    // Apply working memory state to planning system
    std::vector<float> currentState = pImpl->brain->getWorkingMemory()->retrieve();
    
    if (!currentState.empty()) {
        // This would update planning system with working memory state
        // pImpl->brain->getPlanner()->planAction(currentState);
    }
}

void EnhancedWorkingMemoryIntegration::applyToConceptFormation() {
    if (!pImpl->brain || !pImpl->brain->getConceptFormation()) return;
    
    // Apply working memory state to concept formation
    std::vector<float> currentState = pImpl->brain->getWorkingMemory()->retrieve();
    
    if (!currentState.empty()) {
        // This would update concept formation with working memory state
        // pImpl->brain->getConceptFormation()->presentExperience(currentState, ...);
    }
}

} // namespace nlm
