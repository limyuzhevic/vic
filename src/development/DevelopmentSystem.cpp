#include "DevelopmentSystem.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <vector>

namespace nlm {

struct DevelopmentSystem::Impl {
    DevelopmentalStage stage;
    SimulationStep stageStartStep;
    SimulationStep stepsInCurrentStage;
    double stageAge;  // Age since entering current stage
    
    // Development tracking
    float overallMaturationScore;
    std::unordered_map<std::string, float> systemMaturationScores;
    
    Impl() : stage(DevelopmentalStage::Initial), stageStartStep(0), stepsInCurrentStage(0), stageAge(0.0),
             overallMaturationScore(0.0f) {}
};

DevelopmentSystem::DevelopmentSystem() : pImpl(new Impl), age_(0.0) {}

DevelopmentSystem::~DevelopmentSystem() = default;

DevelopmentalStage DevelopmentSystem::getStage() const {
    return pImpl->stage;
}

void DevelopmentSystem::setStage(DevelopmentalStage stage) {
    pImpl->stage = stage;
    pImpl->stageAge = 0.0;
}

void DevelopmentSystem::advanceStage() {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            pImpl->stage = DevelopmentalStage::CriticalPeriod;
            break;
        case DevelopmentalStage::CriticalPeriod:
            pImpl->stage = DevelopmentalStage::Maturation;
            break;
        case DevelopmentalStage::Maturation:
            pImpl->stage = DevelopmentalStage::Adult;
            break;
        case DevelopmentalStage::Adult:
            pImpl->stage = DevelopmentalStage::Aging;
            break;
        case DevelopmentalStage::Aging:
            // Already at final stage
            break;
    }
    pImpl->stepsInCurrentStage = 0;
    pImpl->stageAge = 0.0;
    NLM_LOG_INFO("Development advanced to stage: " + std::string(getStageName()));
}

const char* DevelopmentSystem::getStageName() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return "Initial";
        case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
        case DevelopmentalStage::Maturation: return "Maturation";
        case DevelopmentalStage::Adult: return "Adult";
        case DevelopmentalStage::Aging: return "Aging";
        default: return "Unknown";
    }
}

void DevelopmentSystem::update(Brain* brain, SimulationStep currentStep) {
    ++pImpl->stepsInCurrentStage;
    age_ += 0.001;  // Approximate timestep
    pImpl->stageAge += 0.001;
    
    // Auto-advance stage based on time in stage
    // Initial: 60 steps, CriticalPeriod: 300 steps, Maturation: 600 steps
    if (pImpl->stage == DevelopmentalStage::Initial && pImpl->stageAge > 60.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod && pImpl->stageAge > 300.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::Maturation && pImpl->stageAge > 600.0) {
        advanceStage();
    }
}

void DevelopmentSystem::update(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    age_ += dt;
    pImpl->stageAge += dt;
    ++pImpl->stepsInCurrentStage;
    
    // Auto-advance stage based on developmental age
    // These thresholds are in simulation seconds
    if (pImpl->stage == DevelopmentalStage::Initial && age_ > 60.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod && age_ > 360.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::Maturation && age_ > 960.0) {
        advanceStage();
    }
}

float DevelopmentSystem::getPlasticityModifier() const {
    // Higher plasticity in early stages, lower in later stages
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 1.0f;
        case DevelopmentalStage::CriticalPeriod: return 0.8f;
        case DevelopmentalStage::Maturation: return 0.5f;
        case DevelopmentalStage::Adult: return 0.2f;
        case DevelopmentalStage::Aging: return 0.1f;
        default: return 0.5f;
    }
}

bool DevelopmentSystem::isCriticalPeriod() const {
    return pImpl->stage == DevelopmentalStage::CriticalPeriod;
}

float DevelopmentSystem::getCriticalPeriodProgress() const {
    if (pImpl->stage != DevelopmentalStage::CriticalPeriod) {
        return 0.0f;
    }
    // Progress through critical period (0 to 1)
    return std::min(1.0f, static_cast<float>(pImpl->stageAge / 300.0));
}

// === MEMORY SYSTEM INTEGRATION ===

void DevelopmentSystem::applyMemoryDevelopment(NeuralWorkingMemory* workingMemory, Brain* brain) const {
    if (!workingMemory) return;
    
    float plasticity = getPlasticityModifier();
    float developmentalAgeFactor = static_cast<float>(std::min(age_, 180.0) / 180.0);
    
    // Memory capacity increases with development
    size_t baseCapacity = 10;
    size_t capacityIncrease = static_cast<size_t>(baseCapacity * developmentalAgeFactor * plasticity);
    workingMemory->setCapacity(workingMemory->getCapacity() + capacityIncrease);
    
    // Decay rate decreases (memories last longer) with development
    float baseDecayRate = 0.1f;
    float newDecayRate = baseDecayRate * (1.0f - developmentalAgeFactor * plasticity * 0.7f);
    workingMemory->setDecayRate(newDecayRate);
    
    // Age-dependent working memory enhancement
    if (age_ > 90.0) { // Late initial stage
        workingMemory->strengthenMemory(1.2f);
    }
    
    pImpl->systemMaturationScores["workingMemory"] = newDecayRate;
}

void DevelopmentSystem::applyEpisodicMemoryDevelopment(NeuralEpisodicMemory* episodicMemory, Brain* brain, SimulationStep currentStep) const {
    if (!episodicMemory) return;
    
    float plasticity = getPlasticityModifier();
    float developmentalAgeFactor = static_cast<float>(std::min(age_, 180.0) / 180.0);
    
    // Enhanced storage during critical period
    if (isCriticalPeriod()) {
        episodicMemory->storeMetaInfo(999, currentStep); // Use special concept for development
    }
    
    // Age-dependent consolidation
    float consolidationStrength = 0.5f + (developmentalAgeFactor * plasticity * 0.5f);
    episodicMemory->consolidate(consolidationStrength, {}, currentStep);
    
    // Replay frequency increases with development
    if (age_ > 120.0) { // Middle maturation
        episodicMemory->enableReplay(true);
        episodicMemory->applySleepConsolidation(currentStep);
    }
    
    pImpl->systemMaturationScores["episodicMemory"] = consolidationStrength;
}

void DevelopmentSystem::applyAssociativeMemoryDevelopment(NeuralAssociativeMemory* associativeMemory, Brain* brain) const {
    if (!associativeMemory) return;
    
    float plasticity = getPlasticityModifier();
    float developmentalAgeFactor = static_cast<float>(std::min(age_, 180.0) / 180.0);
    
    // Association strength development
    float associationStrength = 1.0f + (developmentalAgeFactor * plasticity * 0.8f);
    
    // Clear old associations and create new developmentally appropriate ones
    associativeMemory->clear();
    
    // Create basic associations based on developmental stage
    if (isCriticalPeriod()) {
        // Strong associations during critical period
        std::vector<float> patternA(10, 0.5f);
        std::vector<float> patternB(10, 0.6f);
        associativeMemory->associate(patternA, patternB, associationStrength);
    } else if (age_ > 60.0 && age_ < 180.0) {
        // Moderate associations during maturation
        std::vector<float> patternA(10, 0.3f);
        std::vector<float> patternB(10, 0.4f);
        associativeMemory->associate(patternA, patternB, associationStrength * 0.6f);
    }
    
    pImpl->systemMaturationScores["associativeMemory"] = associationStrength;
}

void DevelopmentSystem::applySleepMemoryConsolidationDevelopment(NeuralEpisodicMemory* episodicMemory, SleepManager* sleepManager, Brain* brain) const {
    if (!episodicMemory || !sleepManager) return;
    
    float plasticity = getPlasticityModifier();
    
    // Development-dependent sleep consolidation parameters
    if (isCriticalPeriod()) {
        // Focus on stress-guided consolidation during critical period
        // Would apply stress signals to prioritize emotionally salient memories
    } else if (age_ > 180.0 && age_ < 360.0) {
        // Deep sleep consolidation during maturation
        // Enhanced systems consolidation for integrating new memories
        episodicMemory->applySleepConsolidation(0); // Let sleep manager call it
    } else if (age_ > 360.0) {
        // REM consolidation for adult learning
        // Emotional memory processing and procedural learning
    }
    
    pImpl->systemMaturationScores["sleepMemoryConsolidation"] = plasticity;
}

// === PREDICTION SYSTEM INTEGRATION ===

void DevelopmentSystem::applyPredictionDevelopment(PredictionSystem* predictionSystem, Brain* brain) const {
    if (!predictionSystem) return;
    
    float plasticity = getPlasticityModifier();
    float developmentalAgeFactor = static_cast<float>(std::min(age_, 180.0) / 180.0);
    
    // Prediction accuracy improves with development
    float predictionAccuracy = 0.5f + (developmentalAgeFactor * plasticity * 0.5f);
    
    // Prediction confidence increases
    float predictionConfidence = predictionAccuracy * (1.0f + plasticity * 0.3f);
    
    // Would update prediction system parameters here
    // predictionSystem->setPredictionAccuracy(predictionAccuracy);
    
    pImpl->systemMaturationScores["predictionAccuracy"] = predictionAccuracy;
    pImpl->systemMaturationScores["predictionConfidence"] = predictionConfidence;
}

void DevelopmentSystem::applyPredictionHorizonDevelopment(PredictionSystem* predictionSystem, Brain* brain) const {
    if (!predictionSystem) return;
    
    float plasticity = getPlasticityModifier();
    float developmentalAgeFactor = static_cast<float>(std::min(age_, 180.0) / 180.0);
    
    // Prediction horizon expands with development
    size_t baseHorizon = 1;
    size_t horizonIncrease = static_cast<size_t>(baseHorizon + (developmentalAgeFactor * plasticity * 4.0f));
    
    // predictionSystem->setPredictionHorizon(horizonIncrease);
    
    pImpl->systemMaturationScores["predictionHorizon"] = static_cast<float>(horizonIncrease);
}

void DevelopmentSystem::applyPredictionErrorDevelopment(PredictionSystem* predictionSystem, Brain* brain) const {
    if (!predictionSystem) return;
    
    float plasticity = getPlasticityModifier();
    
    // Prediction error sensitivity decreases with development (more stable predictions)
    float errorSensitivity = 1.0f - (plasticity * 0.4f);
    
    // errorSensitivity would affect prediction error processing
    // Would make error signals more subtle and less reactive with age
    
    pImpl->systemMaturationScores["predictionErrorSensitivity"] = errorSensitivity;
}

void DevelopmentSystem::applyDevelopmentalLearningPace(PredictionSystem* predictionSystem, Brain* brain) const {
    if (!predictionSystem) return;
    
    // Learning pace varies by developmental stage
    float learningPace = 1.0f;
    
    if (isCriticalPeriod()) {
        learningPace = 1.5f; // Accelerated learning during critical period
    } else if (age_ > 180.0 && age_ < 360.0) {
        learningPace = 1.2f; // Moderate learning during maturation
    } else if (age_ > 360.0) {
        learningPace = 0.8f; // Slower, more refined learning in adulthood
    }
    
    pImpl->systemMaturationScores["learningPace"] = learningPace;
}

// === COGNITIVE SYSTEM INTEGRATION ===

void DevelopmentSystem::applyNeuralPlannerDevelopment(NeuralPlanner* neuralPlanner, Brain* brain) const {
    if (!neuralPlanner) return;
    
    float plasticity = getPlasticityModifier();
    float developmentalAgeFactor = static_cast<float>(std::min(age_, 180.0) / 180.0);
    
    // Planning depth increases with development
    size_t basePlanningDepth = 2;
    size_t planningDepthIncrease = static_cast<size_t>(basePlanningDepth + (developmentalAgeFactor * plasticity * 3.0f));
    neuralPlanner->setPlanningDepth(planningDepthIncrease);
    
    // Planning confidence increases with development
    float planningConfidence = 0.5f + (developmentalAgeFactor * plasticity * 0.5f);
    neuralPlanner->setPlanningConfidence(planningConfidence);
    
    // Set goal based on developmental stage
    std::vector<float> developmentalGoal(5, 0.0f);
    if (isCriticalPeriod()) {
        // Explore goals during critical period
        for (int i = 0; i < 5; ++i) {
            developmentalGoal[i] = 0.3f + (static_cast<float>(i) * 0.1f);
        }
    } else if (age_ > 360.0) {
        // More focused goals in adulthood
        for (int i = 0; i < 5; ++i) {
            developmentalGoal[i] = 0.7f + (static_cast<float>(i) * 0.06f);
        }
    }
    neuralPlanner->setCurrentGoal(developmentalGoal);
    
    pImpl->systemMaturationScores["neuralPlannerDepth"] = static_cast<float>(planningDepthIncrease);
    pImpl->systemMaturationScores["neuralPlannerConfidence"] = planningConfidence;
}

void DevelopmentSystem::applyConceptFormationDevelopment(ConceptFormation* conceptFormation, Brain* brain) const {
    if (!conceptFormation) return;
    
    float plasticity = getPlasticityModifier();
    float developmentalAgeFactor = static_cast<float>(std::min(age_, 180.0) / 180.0);
    
    // Concept formation threshold varies with development
    float formationThreshold = 0.7f - (developmentalAgeFactor * plasticity * 0.3f); // Lower threshold = easier concept formation
    
    // Concept stability requirements decrease with age (more generalization)
    float stabilityRequirement = 0.8f - (developmentalAgeFactor * 0.2f);
    
    // Stability window increases with development (need more observations)
    size_t stabilityWindow = 5 + static_cast<size_t>(developmentalAgeFactor * plasticity * 5.0f);
    
    conceptFormation->setFormationThreshold(formationThreshold);
    conceptFormation->setStabilityWindow(stabilityWindow);
    
    // Enhanced concept development during critical period
    if (isCriticalPeriod()) {
        // Rapid concept formation with lower thresholds
        conceptFormation->setFormationThreshold(0.5f);
    }
    
    pImpl->systemMaturationScores["conceptFormationThreshold"] = formationThreshold;
    pImpl->systemMaturationScores["conceptStabilityWindow"] = static_cast<float>(stabilityWindow);
}

void DevelopmentSystem::applyAttentionalSelectionDevelopment(AttentionalSelection* attentionalSelection, Brain* brain) const {
    if (!attentionalSelection) return;
    
    float plasticity = getPlasticityModifier();
    float developmentalAgeFactor = static_cast<float>(std::min(age_, 180.0) / 180.0);
    
    // Inhibition/excitation balance develops with age
    float inhibitionStrength = 0.5f + (developmentalAgeFactor * plasticity * 0.2f); // More inhibition with age
    float excitationStrength = 1.0f - (inhibitionStrength - 0.5f); // Balance excitation
    
    attentionalSelection->setInhibitionStrength(inhibitionStrength);
    attentionalSelection->setExcitationStrength(excitationStrength);
    
    // Competition threshold varies with development
    float competitionThreshold = 0.6f - (developmentalAgeFactor * plasticity * 0.2f); // Lower threshold = more competition
    attentionalSelection->setCompetitionThreshold(competitionThreshold);
    
    // Enhanced attentional development during critical period
    if (isCriticalPeriod()) {
        attentionalSelection->setInhibitionStrength(0.7f);
        attentionalSelection->setExcitationStrength(0.3f);
    }
    
    pImpl->systemMaturationScores["attentionalInhibition"] = inhibitionStrength;
    pImpl->systemMaturationScores["attentionalExcitation"] = excitationStrength;
}

void DevelopmentSystem::applyCognitiveFlexibilityDevelopment(Brain* brain) const {
    if (!brain) return;
    
    float plasticity = getPlasticityModifier();
    float developmentalAgeFactor = static_cast<float>(std::min(age_, 180.0) / 180.0);
    
    // Cognitive flexibility (ability to switch between strategies)
    float cognitiveFlexibility = 0.5f + (developmentalAgeFactor * plasticity * 0.4f);
    
    // Adaptation to change
    float adaptationRate = 0.3f + (developmentalAgeFactor * plasticity * 0.5f);
    
    // Would integrate with brain systems to affect cognitive flexibility
    // Enhanced in adulthood for complex problem solving
    // Reduced during critical period for focused learning
    
    pImpl->systemMaturationScores["cognitiveFlexibility"] = cognitiveFlexibility;
    pImpl->systemMaturationScores["adaptationRate"] = adaptationRate;
}

// === NEUROMODULATION INTEGRATION ===

void DevelopmentSystem::applyNeuromodulationDevelopment(Dopamine* dopamine, Acetylcholine* acetylcholine,
                                                         Norepinephrine* norepinephrine, Serotonin* serotonin,
                                                         Curiosity* curiosity, Novelty* novelty,
                                                         PredictionError* predictionError, Brain* brain) const {
    if (!dopamine || !acetylcholine || !norepinephrine || !serotonin) return;
    
    float plasticity = getPlasticityModifier();
    float developmentalAgeFactor = static_cast<float>(std::min(age_, 180.0) / 180.0);
    
    // Baseline neuromodulator levels develop with age
    float baseLevel = 0.5f;
    
    // Dopamine development (reward sensitivity)
    float dopamineLevel = baseLevel * (0.5f + developmentalAgeFactor * plasticity * 0.8f);
    dopamine->setLevel(dopamineLevel);
    
    // Acetylcholine development (attention)
    float acetylcholineLevel = baseLevel * (0.6f + developmentalAgeFactor * 0.5f);
    acetylcholine->setLevel(acetylcholineLevel);
    
    // Norepinephrine development (arousal)
    float norepinephrineLevel = baseLevel * (0.4f + developmentalAgeFactor * plasticity * 0.6f);
    norepinephrine->setLevel(norepinephrineLevel);
    
    // Serotonin development (mood)
    float serotoninLevel = baseLevel * (0.5f + developmentalAgeFactor * 0.4f);
    serotonin->setLevel(serotoninLevel);
    
    // Novelty and curiosity development
    if (curiosity) curiosity->setLevel(0.3f + developmentalAgeFactor * 0.4f);
    if (novelty) novelty->setLevel(0.4f + developmentalAgeFactor * 0.3f);
    if (predictionError) predictionError->setLevel(0.2f + developmentalAgeFactor * 0.3f);
    
    pImpl->systemMaturationScores["dopamineLevel"] = dopamineLevel;
    pImpl->systemMaturationScores["acetylcholineLevel"] = acetylcholineLevel;
    pImpl->systemMaturationScores["norepinephrineLevel"] = norepinephrineLevel;
    pImpl->systemMaturationScores["serotoninLevel"] = serotoninLevel;
}

void DevelopmentSystem::applyNeuromodulatorSpecificDevelopment(Dopamine* dopamine, Acetylcholine* acetylcholine,
                                                                 Norepinephrine* norepinephrine, Serotonin* serotonin,
                                                                 Brain* brain) const {
    if (!dopamine || !acetylcholine || !norepinephrine || !serotonin) return;
    
    float developmentalAgeFactor = static_cast<float>(std::min(age_, 180.0) / 180.0);
    
    // Age-dependent sensitivity changes
    if (isCriticalPeriod()) {
        // Enhanced reward sensitivity during critical period
        dopamine->setLevel(dopamine->getLevel() * 1.2f);
        
        // Stronger attention signals during critical period
        acetylcholine->setLevel(acetylcholine->getLevel() * 1.1f);
    }
    
    if (age_ > 180.0) { // Later maturation
        // More stable neuromodulation in adulthood
        float stabilityFactor = 0.9f - (static_cast<float>(age_ - 180.0) * 0.001f);
        dopamine->setLevel(dopamine->getLevel() * stabilityFactor);
        acetylcholine->setLevel(acetylcholine->getLevel() * stabilityFactor);
    }
}

void DevelopmentSystem::applyStressRewardDevelopment(Brain* brain) const {
    if (!brain) return;
    
    float developmentalAgeFactor = static_cast<float>(std::min(age_, 180.0) / 180.0);
    
    // Stress response develops with age
    // Young animals have stronger stress responses for survival
    // Adults have more regulated stress responses
    
    pImpl->systemMaturationScores["stressResponse"] = 1.0f - developmentalAgeFactor * 0.3f;
    pImpl->systemMaturationScores["rewardSensitivity"] = 0.8f + developmentalAgeFactor * 0.2f;
}

void DevelopmentSystem::applyNeuromodulationBalanceDevelopment(Brain* brain) const {
    if (!brain) return;
    
    // Balance between different neuromodulators changes with development
    float daAchBalance = 1.0f; // Dopamine/Acetylcholine balance
    
    if (isCriticalPeriod()) {
        // Reward-dominant during critical period
        daAchBalance = 1.5f;
    } else if (age_ > 180.0) {
        // More balanced in adulthood
        daAchBalance = 1.0f;
    }
    
    pImpl->systemMaturationScores["daAchBalance"] = daAchBalance;
}

// === REPLAY INTEGRATION ===

void DevelopmentSystem::applyReplayDevelopment(NeuralEpisodicMemory* episodicMemory, Brain* brain, SimulationStep currentStep) const {
    if (!episodicMemory) return;
    
    float plasticity = getPlasticityModifier();
    float developmentalAgeFactor = static_cast<float>(std::min(age_, 180.0) / 180.0);
    
    // Replay frequency and duration development
    if (isCriticalPeriod()) {
        // Brief, frequent replays during critical period for fast learning
        episodicMemory->enableReplay(true);
    } else if (age_ > 180.0 && age_ < 360.0) {
        // Longer, more focused replays during maturation
        episodicMemory->enableReplay(true);
        episodicMemory->replayWithAttention(nullptr, {}); // Would need attention system
    } else if (age_ > 360.0) {
        // Strategic replays in adulthood
        episodicMemory->enableReplay(true);
    }
    
    pImpl->systemMaturationScores["replayFrequency"] = developmentalAgeFactor * 0.8f + 0.2f;
    pImpl->systemMaturationScores["replayDuration"] = plasticity * 0.7f + 0.3f;
}

void DevelopmentSystem::applyReplayTargetSelectionDevelopment(NeuralEpisodicMemory* episodicMemory, Brain* brain) const {
    if (!episodicMemory) return;
    
    float developmentalAgeFactor = static_cast<float>(std::min(age_, 180.0) / 180.0);
    
    // Replay target selection becomes more sophisticated with development
    // Would integrate with attention and prediction systems for target selection
    
    pImpl->systemMaturationScores["replayTargetSelection"] = developmentalAgeFactor * 0.9f + 0.1f;
}

void DevelopmentSystem::applyReplayBasedLearningDevelopment(NeuralEpisodicMemory* episodicMemory, Brain* brain) const {
    if (!episodicMemory) return;
    
    float developmentalAgeFactor = static_cast<float>(std::min(age_, 180.0) / 180.0);
    
    // Replay-based learning effectiveness develops
    // Early replays are more about pattern completion
    // Later replays integrate with concepts and planning
    
    pImpl->systemMaturationScores["replayLearningEffectiveness"] = developmentalAgeFactor * 0.8f + 0.2f;
}

void DevelopmentSystem::applySleepReplayDevelopment(NeuralEpisodicMemory* episodicMemory, SleepManager* sleepManager, Brain* brain) const {
    if (!episodicMemory || !sleepManager) return;
    
    float developmentalAgeFactor = static_cast<float>(std::min(age_, 180.0) / 180.0);
    
    // Sleep replay development varies by stage
    if (isCriticalPeriod()) {
        // Light sleep replay dominates during critical period
        // Essential for rapid encoding of new experiences
    } else if (age_ > 180.0 && age_ < 360.0) {
        // Deep sleep replay integrates memories during maturation
        // Systems consolidation phase
    } else if (age_ > 360.0) {
        // REM replay refines concepts and skills during adulthood
        // Procedural and emotional memory processing
    }
    
    pImpl->systemMaturationScores["sleepReplayEffectiveness"] = developmentalAgeFactor * 0.7f + 0.3f;
}

// === CHECKPOINT INTEGRATION ===

void DevelopmentSystem::applyCheckpointDevelopment(Brain* brain) const {
    if (!brain) return;
    
    float developmentalAgeFactor = static_cast<float>(std::min(age_, 180.0) / 180.0);
    
    // Checkpoint saving/loading integration
    // Would affect serialization/deserialization of brain state
    
    pImpl->systemMaturationScores["checkpointReliability"] = developmentalAgeFactor * 0.8f + 0.2f;
}

void DevelopmentSystem::applyCheckpointPrecisionDevelopment(Brain* brain) const {
    if (!brain) return;
    
    float developmentalAgeFactor = static_cast<float>(std::min(age_, 180.0) / 180.0);
    
    // Checkpoint precision (how accurately state is preserved)
    // Increases with development
    
    pImpl->systemMaturationScores["checkpointPrecision"] = developmentalAgeFactor * 0.9f + 0.1f;
}

void DevelopmentSystem::applyPersistenceDevelopment(Brain* brain) const {
    if (!brain) return;
    
    float developmentalAgeFactor = static_cast<float>(std::min(age_, 180.0) / 180.0);
    
    // Persistence across sessions (memory consolidation)
    // Better in older individuals
    
    pImpl->systemMaturationScores["sessionPersistence"] = developmentalAgeFactor * 0.7f + 0.3f;
}

// === OVERALL DEVELOPMENT COORDINATION ===

void DevelopmentSystem::applyAllDevelopmentEffects(Brain* brain, SimulationStep currentStep, RandomGenerator& rng, TimestepDuration dt) {
    // Apply all developmental effects based on current stage
    
    if (!brain) return;
    
    float plasticity = getPlasticityModifier();
    float developmentalAgeFactor = static_cast<float>(std::min(age_, 180.0) / 180.0);
    
    // Reset system maturation scores
    pImpl->systemMaturationScores.clear();
    
    // Apply memory system development
    if (brain->getWorkingMemory()) {
        applyMemoryDevelopment(brain->getWorkingMemory(), brain);
    }
    
    if (brain->getEpisodicMemory()) {
        applyEpisodicMemoryDevelopment(brain->getEpisodicMemory(), brain, currentStep);
    }
    
    if (brain->getAssociativeMemory()) {
        applyAssociativeMemoryDevelopment(brain->getAssociativeMemory(), brain);
    }
    
    // Apply prediction system development
    if (brain->getPredictionSystem()) {
        applyPredictionDevelopment(brain->getPredictionSystem(), brain);
        applyPredictionHorizonDevelopment(brain->getPredictionSystem(), brain);
        applyPredictionErrorDevelopment(brain->getPredictionSystem(), brain);
        applyDevelopmentalLearningPace(brain->getPredictionSystem(), brain);
    }
    
    // Apply cognitive system development
    if (brain->getPlanner()) {
        applyNeuralPlannerDevelopment(brain->getPlanner(), brain);
    }
    
    if (brain->getConceptFormation()) {
        applyConceptFormationDevelopment(brain->getConceptFormation(), brain);
    }
    
    if (brain->getAttention()) {
        applyAttentionalSelectionDevelopment(brain->getAttention(), brain);
    }
    
    applyCognitiveFlexibilityDevelopment(brain);
    
    // Apply neuromodulation system development
    if (brain->getDopamine()) {
        applyNeuromodulationDevelopment(brain->getDopamine(), brain->getAcetylcholine(),
                                       brain->getNorepinephrine(), brain->getSerotonin(),
                                       brain->getCuriosity(), brain->getNovelty(),
                                       brain->getPredictionError(), brain);
        applyNeuromodulatorSpecificDevelopment(brain->getDopamine(), brain->getAcetylcholine(),
                                              brain->getNorepinephrine(), brain->getSerotonin(), brain);
    }
    
    applyStressRewardDevelopment(brain);
    applyNeuromodulationBalanceDevelopment(brain);
    
    // Apply replay system development
    if (brain->getEpisodicMemory() && brain->getSleepManager()) {
        applyReplayDevelopment(brain->getEpisodicMemory(), brain, currentStep);
        applyReplayTargetSelectionDevelopment(brain->getEpisodicMemory(), brain);
        applyReplayBasedLearningDevelopment(brain->getEpisodicMemory(), brain);
        applySleepReplayDevelopment(brain->getEpisodicMemory(), brain->getSleepManager(), brain);
    }
    
    // Apply checkpoint system development
    applyCheckpointDevelopment(brain);
    applyCheckpointPrecisionDevelopment(brain);
    applyPersistenceDevelopment(brain);
    
    // Calculate overall development score
    calculateOverallDevelopmentScore();
    
    // Log developmental progress
    if (static_cast<int>(age_) % 30 == 0) {
        NLM_LOG_INFO("Development System: Stage " + std::string(getStageName()) +
                    ", Age: " + std::to_string(age_) +
                    ", Maturation: " + std::to_string(pImpl->overallMaturationScore));
    }
}

float DevelopmentSystem::getDevelopmentalMaturationScore() const {
    return pImpl->overallMaturationScore;
}

void DevelopmentSystem::calculateOverallDevelopmentScore() {
    if (pImpl->systemMaturationScores.empty()) {
        pImpl->overallMaturationScore = 0.0f;
        return;
    }
    
    float totalScore = 0.0f;
    int scoreCount = 0;
    
    // Weight different systems
    float weights[] = {0.15f, 0.15f, 0.15f, 0.10f, 0.10f, 0.10f, 0.05f, 0.05f, 0.05f};
    std::string systemNames[] = {"workingMemory", "episodicMemory", "associativeMemory", 
                                 "predictionAccuracy", "predictionHorizon", "predictionErrorSensitivity",
                                 "learningPace", "neuralPlannerDepth", "neuralPlannerConfidence"};
    
    for (int i = 0; i < 9 && i < static_cast<int>(pImpl->systemMaturationScores.size()); ++i) {
        totalScore += pImpl->systemMaturationScores[systemNames[i]] * weights[i];
        scoreCount++;
    }
    
    if (scoreCount > 0) {
        pImpl->overallMaturationScore = totalScore / scoreCount;
    } else {
        pImpl->overallMaturationScore = getPlasticityModifier(); // Fallback
    }
}

void DevelopmentSystem::resetToInitial() {
    setStage(DevelopmentalStage::Initial);
    age_ = 0.0;
    pImpl->stageAge = 0.0;
    pImpl->stageStartStep = 0;
    pImpl->stepsInCurrentStage = 0;
    pImpl->systemMaturationScores.clear();
    pImpl->overallMaturationScore = 0.0f;
    
    NLM_LOG_INFO("Development System reset to Initial stage");
}

} // namespace nlm
