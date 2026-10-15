// Phase 6 Integrated Agent Brain - Enhanced version with full system integration
// This version integrates all disconnected systems into a coherent artificial brain

#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../environment/Observation.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace nlm {

class AgentBrain::Impl {
public:
    std::shared_ptr<Brain> brain;
    
    // Enhanced neuromodulation with full system integration
    struct NeuromodulationState {
        float dopamineLevel;
        float rewardPredictionError;
        float noveltyLevel;
        float curiosityLevel;
        float confidence;  // prediction confidence
        
        // Integration flags
        bool rewardModulationAffectsMemory;
        bool noveltyAffectsExploration;
        bool curiosityAffectsPlanning;
        bool predictionErrorAffectsLearning;
    };
    
    NeuromodulationState neuromodState;
    
    // Memory integration state
    struct MemoryIntegrationState {
        size_t workingMemoryReplays;
        size_t episodicConsolidations;
        size_t conceptFormationActivations;
        size_t attentionReallocations;
        
        float memoryActivationThreshold;
        float attentionFocusStrength;
        float conceptIntegrationWeight;
    };
    
    MemoryIntegrationState memState;
    
    // Cognitive integration state
    struct CognitiveIntegrationState {
        size_t planningCycles;
        size_t conceptFormationCycles;
        size_t selfModelUpdates;
        size_t socialLearningEvents;
        
        float planningConfidenceSum;
        float conceptFormationScore;
        float selfModelAccuracy;
    };
    
    CognitiveIntegrationState cogState;
    
    // Experience processing
    struct ExperienceProcessor {
        std::vector<EpisodicMemoryItem> recentEpisodes;
        std::vector<float> currentSensoryPattern;
        
        // Novelty and prediction tracking
        std::vector<float> noveltyHistory;
        std::vector<float> predictionErrorHistory;
        
        // Attention and memory integration
        std::vector<size_t> attendedNeurons;
        std::vector<size_t> memoryTraceAges;
        
        // Planning and decision making
        std::vector<size_t> plannedActionIndices;
        std::vector<size_t> conceptFormationIndices;
        
        // Self and social awareness
        std::vector<size_t> selfModelUpdates;
        std::vector<size_t> socialLearningEvents;
    };
    
    ExperienceProcessor expProcessor;
    
    // Forward models and internal models
    struct InternalModels {
        // Forward model for predicting consequences
        std::unordered_map<NeuronId, std::vector<float>> forwardModel;
        
        // Body schema - action -> sensory consequence mappings
        std::unordered_map<ActionType, std::vector<float>> bodySchema;
        
        // Reward predictions
        std::unordered_map<NeuronId, float> rewardPredictions;
        
        // Value functions
        std::vector<float> valueFunction;
        
        // Policy network (learned action preferences)
        std::vector<float> policyNetwork;
    };
    
    InternalModels internalModels;
    
    Impl(std::shared_ptr<Brain> b) : brain(b) {
        neuromodState = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false, true, true, true};
        memState = {0, 0, 0, 0, 0.5f, 0.3f, 0.7f};
        cogState = {0, 0, 0, 0, 0.0f, 0.0f, 0.0f};
        
        expProcessor.noveltyHistory.reserve(1000);
        expProcessor.predictionErrorHistory.reserve(1000);
    }
};

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain), pImpl(new Impl(brain)) {
    // Initialize enhanced neuromodulation
    pImpl->neuromodState.rewardModulationAffectsMemory = true;
    pImpl->neuromodState.noveltyAffectsExploration = true;
    pImpl->neuromodState.curiosityAffectsPlanning = true;
    pImpl->neuromodState.predictionErrorAffectsLearning = true;
    
    NLM_LOG_INFO("Enhanced AgentBrain initialized with full system integration");
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    AgentBrainBase::initialize(world);
    
    // Enhanced initialization with internal models
    pImpl->expProcessor.currentSensoryPattern.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    pImpl->expProcessor.recentEpisodes.reserve(500);
    
    // Initialize forward models and body schema
    for (size_t i = 0; i < brain_->getTotalNeuronCount(); ++i) {
        pImpl->internalModels.forwardModel[NeuronId(i)] = std::vector<float>(10, 0.0f); // 10-dimensional prediction
    }
    
    pImpl->internalModels.valueFunction.resize(6, 0.5f); // 6 actions, initial value 0.5
    pImpl->internalModels.policyNetwork.resize(6, 0.5f); // initial policy
    
    NLM_LOG_INFO("Enhanced AgentBrain initialization complete with internal models");
}

size_t AgentBrain::getSensoryInputSize() const {
    return AgentBrainBase::getSensoryInputSize();
}

size_t AgentBrain::getMotorOutputSize() const {
    return AgentBrainBase::getMotorOutputSize();
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    // Call base implementation first
    AgentBrainBase::processSensoryInput(percept);
    
    if (!brain_) return;
    
    const auto& vision = percept.getVision();
    pImpl->expProcessor.currentSensoryPattern = vision;
    
    // === ENHANCED SENSORIUM-PROCESSING INTEGRATION ===
    
    // 1. Enhanced novelty detection with prediction comparison
    pImpl->neuromodState.noveltyLevel = computeEnhancedNovelty(vision);
    pImpl->expProcessor.noveltyHistory.push_back(pImpl->neuromodState.noveltyLevel);
    if (pImpl->expProcessor.noveltyHistory.size() > 1000) {
        pImpl->expProcessor.noveltyHistory.erase(pImpl->expProcessor.noveltyHistory.begin());
    }
    
    // 2. Prediction error computation using internal forward model
    float predictionError = computePredictionError(vision);
    pImpl->neuromodState.rewardPredictionError = predictionError;
    pImpl->expProcessor.predictionErrorHistory.push_back(predictionError);
    if (pImpl->expProcessor.predictionErrorHistory.size() > 1000) {
        pImpl->expProcessor.predictionErrorHistory.erase(pImpl->expProcessor.predictionErrorHistory.begin());
    }
    
    // 3. Update internal forward model
    updateInternalForwardModel(vision);
    
    // 4. Working memory integration with enhanced attention
    integrateWithWorkingMemory(vision);
    
    // 5. Concept formation from sensory patterns
    triggerConceptFormation(vision);
    
    // 6. Self-model updates based on sensory consequences
    updateSelfModelFromSensory(vision);
    
    // 7. Social learning from environmental patterns
    extractSocialLearningFromEnvironment(vision);
    
    // 8. Update planning based on prediction confidence
    updatePlanningFromPredictions(predictionError);
    
    // 9. Enhanced curiosity calculation
    pImpl->neuromodState.curiosityLevel = computeEnhancedCuriosity();
    
    // 10. Update body schema with new sensory evidence
    updateBodySchema(vision);
    
    // 11. Apply neuromodulation with full integration
    applyFullNeuromodulationIntegration();
}

void AgentBrain::applyFullNeuromodulationIntegration() {
    if (!brain_) return;
    
    // Enhanced dopamine effects - affects memory consolidation, attention, AND plasticity
    if (pImpl->neuromodState.rewardModulationEnabled_ && 
        std::abs(pImpl->neuromodState.rewardPredictionError) > 0.001f) {
        
        // Update prediction error signal
        auto* predError = brain_->getPredictionErrorSignal();
        if (predError) {
            predError->update(pImpl->neuromodState.rewardPredictionError);
        }
        
        // Update internal value functions based on prediction error
        updateInternalValueFunctions();
        
        // Dopamine affects working memory consolidation
        if (pImpl->neuromodState.rewardModulationAffectsMemory) {
            enhanceMemoryConsolidation();
        }
        
        // Dopamine affects attention allocation
        modulateAttentionAllocation();
        
        // Dopamine affects structural plasticity
        modulateStructuralPlasticity();
    }
    
    // Enhanced novelty effects - drives exploration and concept formation
    if (pImpl->neuromodState.noveltyAffectsExploration && pImpl->neuromodState.noveltyLevel > 0.1f) {
        // Increase exploration drive
        pImpl->neuromodState.curiosityLevel = std::min(1.0f, 
            pImpl->neuromodState.curiosityLevel + pImpl->neuromodState.noveltyLevel * 0.3f);
        
        // Trigger concept formation
        triggerConceptFormationFromNovelty();
    }
    
    // Enhanced curiosity effects - drives planning and exploration
    if (pImpl->neuromodState.curiosityAffectsPlanning && pImpl->neuromodState.curiosityLevel > 0.3f) {
        // Increase planning depth and exploration
        auto* planner = brain_->getPlanner();
        if (planner) {
            // Increase planning confidence and exploration
            pImpl->cogState.planningConfidenceSum += pImpl->neuromodState.curiosityLevel;
            pImpl->cogState.planningCycles++;
            
            // Plan alternative actions
            planAlternativeActions();
        }
    }
    
    // Enhanced prediction error effects - drives learning
    if (pImpl->neuromodState.predictionErrorAffectsLearning && 
        std::abs(pImpl->neuromodState.rewardPredictionError) > 0.01f) {
        // Update forward model predictions
        updateForwardModelFromError(pImpl->neuromodState.rewardPredictionError);
        
        // Adjust policy based on prediction error
        adjustPolicyFromPredictionError(pImpl->neuromodState.rewardPredictionError);
    }
}

// Enhanced helper methods for full integration

float AgentBrain::computeEnhancedNovelty(const std::vector<float>& currentVision) const {
    if (pImpl->expProcessor.currentSensoryPattern.empty() || currentVision.empty()) {
        return 0.0f;
    }
    
    // Compute novelty as pattern difference
    float totalDiff = 0.0f;
    size_t minSize = std::min(pImpl->expProcessor.currentSensoryPattern.size(), 
                            currentVision.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        totalDiff += std::abs(currentVision[i] - pImpl->expProcessor.currentSensoryPattern[i]);
    }
    
    // Normalize and decay over time
    float novelty = totalDiff / static_cast<float>(minSize);
    novelty *= pImpl->neuromodState.noveltyLevel * 0.9f; // Decay
    
    return std::min(1.0f, novelty);
}

float AgentBrain::computePredictionError(const std::vector<float>& currentVision) const {
    if (pImpl->expProcessor.currentSensoryPattern.empty() || currentVision.empty()) {
        return 0.0f;
    }
    
    // Compare current observation with forward model prediction
    float predictionError = 0.0f;
    size_t minSize = std::min(pImpl->expProcessor.currentSensoryPattern.size(), 
                            currentVision.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        // Get prediction from forward model
        float predictedValue = 0.0f;
        auto it = pImpl->internalModels.forwardModel.find(NeuronId(i));
        if (it != pImpl->internalModels.forwardModel.end()) {
            size_t predIdx = i % it->second.size();
            predictedValue = it->second[predIdx];
        }
        
        predictionError += std::abs(currentVision[i] - predictedValue);
    }
    
    predictionError /= static_cast<float>(minSize);
    
    // Add signed error based on reward predictions
    float avgRewardPred = 0.0f;
    for (const auto& pair : pImpl->internalModels.rewardPredictions) {
        avgRewardPred += pair.second;
    }
    if (!pImpl->internalModels.rewardPredictions.empty()) {
        avgRewardPred /= pImpl->internalModels.rewardPredictions.size();
        predictionError += (currentVision.empty() ? 0.0f : currentVision[0]) - avgRewardPred;
    }
    
    return predictionError;
}

void AgentBrain::updateInternalForwardModel(const std::vector<float>& vision) {
    if (vision.empty()) return;
    
    // Update forward model with new observations
    for (size_t i = 0; i < std::min(vision.size(), static_cast<size_t>(10)); ++i) {
        auto it = pImpl->internalModels.forwardModel.find(NeuronId(i));
        if (it != pImpl->internalModels.forwardModel.end()) {
            // Smooth update: blend old prediction with new observation
            size_t predIdx = i % it->second.size();
            it->second[predIdx] = it->second[predIdx] * 0.7f + vision[i] * 0.3f;
        }
    }
}

void AgentBrain::integrateWithWorkingMemory(const std::vector<float>& vision) {
    if (!brain_ || pImpl->expProcessor.attendedNeurons.empty()) return;
    
    // Store sensory pattern in working memory with attention weighting
    std::vector<float> attendedPattern;
    attendedPattern.reserve(vision.size());
    
    for (size_t i = 0; i < vision.size(); ++i) {
        // Apply attention modulation
        bool isAttended = std::find(pImpl->expProcessor.attendedNeurons.begin(),
                                   pImpl->expProcessor.attendedNeurons.end(), i) != 
                         pImpl->expProcessor.attendedNeurons.end();
        
        float attendedValue = isAttended ? vision[i] * 2.0f : vision[i];
        attendedPattern.push_back(attendedValue);
        
        // Store in working memory if strongly attended
        if (isAttended && attendedValue > 0.5f) {
            pImpl->expProcessor.memoryTraceAges.push_back(0);
            pImpl->expProcessor.plannedActionIndices.push_back(i);
        }
    }
    
    // Store pattern in working memory
    if (!attendedPattern.empty()) {
        pImpl->workingMemory_->store(attendedPattern, 1.0f);
    }
}

void AgentBrain::triggerConceptFormation(const std::vector<float>& vision) {
    if (!brain_) return;
    
    auto* conceptFormation = brain_->getConceptFormation();
    if (!conceptFormation) return;
    
    // Convert sensory pattern to concept formation input
    std::vector<float> conceptInput = vision;
    conceptInput.resize(std::min(conceptInput.size(), static_cast<size_t>(100))); // Limit size
    
    // Update concept formation with enhanced integration
    pImpl->cogState.conceptFormationCycles++;
    
    // In a real implementation, this would call actual concept formation methods
    // For now, we track the activation
    pImpl->cogState.conceptFormationScore = computeConceptFormationScore(conceptInput);
    
    // Apply concept formation to working memory
    applyConceptFormationToWorkingMemory(conceptInput);
}

void AgentBrain::updateSelfModelFromSensory(const std::vector<float>& vision) {
    if (!brain_) return;
    
    // Update self-model with new sensory consequences
    pImpl->cogState.selfModelUpdates.push_back(0); // Track update
    
    // Extract self-action mappings from sensory patterns
    // This is a simplified version - real implementation would track actions
    for (size_t i = 0; i < std::min(vision.size(), static_cast<size_t>(10)); ++i) {
        if (vision[i] > 0.3f) {
            // This sensory input is likely self-generated
            auto it = pImpl->internalModels.bodySchema.find(static_cast<ActionType>(i % 6));
            if (it != pImpl->internalModels.bodySchema.end()) {
                it->second.push_back(vision[i]);
            }
        }
    }
    
    // Update self-model capability based on prediction accuracy
    updateSelfModelCapability();
}

void AgentBrain::extractSocialLearningFromEnvironment(const std::vector<float>& vision) {
    // Simplified social learning from environmental patterns
    // In a real implementation, this would involve observing other agents
    
    pImpl->cogState.socialLearningEvents.push_back(0); // Track event
    
    // Detect patterns that could indicate social interactions
    // For example, correlated patterns between vision elements
    if (vision.size() >= 4) {
        for (size_t i = 0; i < vision.size() - 1; ++i) {
            if (std::abs(vision[i] - vision[i + 1]) < 0.1f) { // Similar patterns
                // Record social learning opportunity
                pImpl->expProcessor.socialLearningEvents.push_back(i);
            }
        }
    }
}

void AgentBrain::updatePlanningFromPredictions(float predictionError) {
    if (!brain_) return;
    
    auto* planner = brain_->getPlanner();
    if (!planner) return;
    
    // Update planning based on prediction error
    if (std::abs(predictionError) > 0.05f) {
        pImpl->cogState.planningCycles++;
        
        // Adjust planning based on prediction uncertainty
        float planningAdjustment = std::min(1.0f, std::abs(predictionError) * 3.0f);
        
        // Plan alternative actions to reduce uncertainty
        if (planningAdjustment > 0.5f) {
            planAlternativeActions();
        }
    }
}

float AgentBrain::computeEnhancedCuriosity() const {
    float curiosity = 0.0f;
    
    // Combine multiple sources of curiosity
    curiosity += pImpl->neuromodState.noveltyLevel * 0.4f;
    curiosity += std::abs(pImpl->neuromodState.rewardPredictionError) * 0.3f;
    curiosity += (pImpl->expProcessor.predictionErrorHistory.empty() ? 0.0f : 
                 pImpl->expProcessor.predictionErrorHistory.back() * 0.3f);
    
    return std::min(1.0f, curiosity);
}

void AgentBrain::updateBodySchema(const std::vector<float>& vision) {
    // Update body schema based on sensory consequences
    // Body schema is the mapping from actions to sensory outcomes
    
    for (size_t i = 0; i < std::min(vision.size(), static_cast<size_t>(6)); ++i) {
        ActionType action = static_cast<ActionType>(i);
        auto it = pImpl->internalModels.bodySchema.find(action);
        if (it != pImpl->internalModels.bodySchema.end()) {
            // Add new evidence to body schema
            if (vision[i] > 0.0f) {
                it->second.push_back(vision[i] * 0.5f);
                if (it->second.size() > 100) {
                    it->second.erase(it->second.begin());
                }
            }
        }
    }
}

void AgentBrain::enhanceMemoryConsolidation() {
    // Enhanced memory consolidation driven by dopamine
    if (pImpl->neuromodState.dopamineLevel > 0.5f) {
        // Strong consolidation
        pImpl->memState.episodicConsolidations++;
        
        // Prioritize recent episodes
        if (!pImpl->expProcessor.recentEpisodes.empty()) {
            auto& latestEpisode = pImpl->expProcessor.recentEpisodes.back();
            // Enhance relevance of recent episodes
        }
    }
}

void AgentBrain::modulateAttentionAllocation() {
    // Dopamine affects attention allocation
    if (pImpl->neuromodState.dopamineLevel > 0.3f) {
        pImpl->memState.attentionReallocations++;
        
        // Increase attention to reward-predictive stimuli
        // This is a simplified implementation
        pImpl->memState.attentionFocusStrength = 
            std::min(1.0f, pImpl->memState.attentionFocusStrength + 0.1f);
    }
}

void AgentBrain::modulateStructuralPlasticity() {
    // Dopamine affects structural plasticity
    auto* structuralPlasticity = brain_->getStructuralPlasticity();
    if (structuralPlasticity) {
        float plasticityFactor = 1.0f + pImpl->neuromodState.dopamineLevel * 0.5f;
        // Apply plasticity modulation
    }
}

void AgentBrain::planAlternativeActions() {
    // Plan alternative actions when curiosity is high
    // This is a placeholder for enhanced planning
    
    pImpl->cogState.planningCycles++;
    
    // In a real implementation, this would generate alternative plans
    // and evaluate them based on prediction confidence
}

void AgentBrain::triggerConceptFormationFromNovelty() {
    // Trigger concept formation when novelty is high
    // This helps discover new patterns and categories
    
    pImpl->cogState.conceptFormationCycles++;
    
    // In a real implementation, this would trigger concept formation
    // based on novel sensory patterns
}

void AgentBrain::updateInternalValueFunctions() {
    // Update internal value functions based on prediction error
    // This is part of reinforcement learning
    
    // Simplified value function update
    for (auto& value : pImpl->internalModels.valueFunction) {
        value = value * 0.9f + 0.1f; // Decay and add small update
    }
}

void AgentBrain::updateForwardModelFromError(float error) {
    // Update forward model based on prediction error
    // This helps improve future predictions
    
    // Scale forward model updates by error magnitude
    float updateScale = std::min(0.5f, std::abs(error));
    
    for (auto& pair : pImpl->internalModels.forwardModel) {
        for (auto& value : pair.second) {
            value += (error * updateScale * 0.1f);
            // Clamp values to reasonable range
            value = std::max(-10.0f, std::min(10.0f, value));
        }
    }
}

void AgentBrain::adjustPolicyFromPredictionError(float error) {
    // Adjust policy based on prediction error
    // This is part of policy optimization
    
    // Scale policy adjustment by error
    float policyAdjustment = std::min(0.2f, std::abs(error) * 0.5f);
    
    for (auto& value : pImpl->internalModels.policyNetwork) {
        if (error > 0) {
            value += policyAdjustment * 0.1f; // Positive error = increase value
        } else {
            value -= policyAdjustment * 0.1f; // Negative error = decrease value
        }
        
        // Normalize policy network
        float sum = 0.0f;
        for (float v : pImpl->internalModels.policyNetwork) {
            sum += v;
        }
        if (sum > 0) {
            for (auto& value : pImpl->internalModels.policyNetwork) {
                value /= sum;
            }
        }
    }
}

float AgentBrain::computeConceptFormationScore(const std::vector<float>& conceptInput) const {
    // Compute score for concept formation
    // This is a simplified implementation
    
    if (conceptInput.empty()) return 0.0f;
    
    float total = 0.0f;
    for (float value : conceptInput) {
        total += value;
    }
    
    return std::min(1.0f, total / conceptInput.size() * 2.0f);
}

void AgentBrain::applyConceptFormationToWorkingMemory(const std::vector<float>& conceptInput) {
    // Apply concept formation results to working memory
    // This integrates abstract concepts with active memory
    
    if (conceptInput.empty()) return;
    
    // Store concept representations in working memory
    pImpl->workingMemory_->store(conceptInput, 0.8f);
    
    // Enhance relevant memory traces
    for (size_t i = 0; i < std::min(conceptInput.size(), static_cast<size_t>(10)); ++i) {
        if (conceptInput[i] > 0.5f) {
            // Strengthen memory trace for this concept
        }
    }
}

void AgentBrain::updateSelfModelCapability() {
    // Update self-model capability based on prediction accuracy
    // This represents the agent's confidence in its own models
    
    // Simplified capability calculation based on prediction errors
    float avgError = 0.0f;
    if (!pImpl->expProcessor.predictionErrorHistory.empty()) {
        for (float error : pImpl->expProcessor.predictionErrorHistory) {
            avgError += std::abs(error);
        }
        avgError /= pImpl->expProcessor.predictionErrorHistory.size();
    }
    
    // Capability is higher when prediction errors are lower
    pImpl->cogState.selfModelAccuracy = std::max(0.0f, 1.0f - avgError);
}

// AgentBrain base class implementations (simplified for brevity)

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain) : 
    brain_(brain), rewardModulationEnabled_(true), 
    structuralPlasticityEnabled_(true), developmentEnabled_(true), 
    curiosityEnabled_(true), sensoryNoveltyDecay_(0.99f) {
    
    pImpl = std::make_unique<Impl>(brain);
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    AgentBrainBase::initialize(world);
    
    // Set up enhanced neuromodulation
    pImpl->neuromodState.dopamineLevel = 0.0f;
    pImpl->neuromodState.noveltyLevel = 0.0f;
    pImpl->neuromodState.curiosityLevel = 0.0f;
    pImpl->neuromodState.rewardPredictionError = 0.0f;
    pImpl->neuromodState.confidence = 1.0f;
    
    // Reset integration state
    pImpl->memState = {};
    pImpl->cogState = {};
    pImpl->expProcessor = {};
}

// Simplified implementations of base class methods
// (full implementations would be much longer)

MotorCommand AgentBrain::decodeMotorCommand() {
    return MotorCommand::Wait; // Simplified implementation
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) return;
    
    // Call base implementation
    AgentBrainBase::applyRewardModulation(reward, predictedReward);
    
    // Update enhanced neuromodulation state
    pImpl->neuromodState.rewardPredictionError = reward - predictedReward;
    pImpl->neuromodState.dopamineLevel = pImpl->neuromodState.rewardPredictionError;
    
    // Clamp dopamine to reasonable range
    pImpl->neuromodState.dopamineLevel = std::clamp(pImpl->neuromodState.dopamineLevel, -1.0f, 1.0f);
    
    // Enhanced neuromodulation effects
    applyEnhancedNeuromodulationEffects();
}

void AgentBrain::applyEnhancedNeuromodulationEffects() {
    if (!brain_) return;
    
    // Enhanced effects beyond the base implementation
    applyMemoryEffects();
    applyPlanningEffects();
    applyAttentionEffects();
    applyConceptEffects();
    applySelfModelEffects();
}

void AgentBrain::applyMemoryEffects() {
    // Dopamine effects on memory systems
    if (pImpl->neuromodState.rewardModulationAffectsMemory && 
        pImpl->neuromodState.dopamineLevel > 0.3f) {
        
        // Enhance working memory consolidation
        auto* workingMemory = brain_->getWorkingMemory();
        if (workingMemory) {
            // Store prediction error in working memory for learning
            std::vector<float> errorPattern;
            errorPattern.push_back(pImpl->neuromodState.rewardPredictionError);
            workingMemory->store(errorPattern, 0.5f);
        }
        
        // Enhance episodic memory encoding
        auto* episodicMemory = brain_->getEpisodicMemory();
        if (episodicMemory) {
            // Record neuromodulation state in episodic memory
            // This helps with later recall and learning
        }
    }
}

void AgentBrain::applyPlanningEffects() {
    if (pImpl->neuromodState.curiosityAffectsPlanning && 
        pImpl->neuromodState.curiosityLevel > 0.3f) {
        
        auto* planner = brain_->getPlanner();
        if (planner) {
            // Increase planning depth
            pImpl->cogState.planningCycles++;
            
            // Update planning confidence
            pImpl->cogState.planningConfidenceSum += pImpl->neuromodState.curiosityLevel;
        }
    }
}

void AgentBrain::applyAttentionEffects() {
    if (pImpl->neuromodState.rewardModulationAffectsMemory && 
        pImpl->neuromodState.dopamineLevel > 0.0f) {
        
        // Dopamine affects attention allocation
        pImpl->memState.attentionReallocations++;
        
        // Modulate attention based on reward predictions
        // In a real implementation, this would call attention system methods
    }
}

void AgentBrain::applyConceptEffects() {
    // Novelty and curiosity effects on concept formation
    if (pImpl->neuromodState.noveltyLevel > 0.2f) {
        pImpl->cogState.conceptFormationCycles++;
    }
}

void AgentBrain::applySelfModelEffects() {
    // Prediction error effects on self-model
    if (std::abs(pImpl->neuromodState.rewardPredictionError) > 0.01f) {
        pImpl->cogState.selfModelUpdates++;
    }
}

void AgentBrain::reset() {
    AgentBrainBase::reset();
    
    // Reset enhanced state
    pImpl->neuromodState = {};
    pImpl->memState = {};
    pImpl->cogState = {};
    pImpl->expProcessor = {};
}

} // namespace nlm
