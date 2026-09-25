#include "DevelopmentSystem.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct DevelopmentSystem::Impl {
    DevelopmentalStage stage;
    SimulationStep stageStartStep;
    SimulationStep stepsInCurrentStage;
    double stageAge;  // Age since entering current stage
    
    // Memory system effects
    float memoryCapacityModifier;
    float memoryStabilityModifier;
    float workingMemoryModifier;
    float replayStrengthModifier;
    float schemaFormationModifier;
    
    // Neuromodulation system effects
    float dopamineModulation;
    float curiosityModulation;
    float predictionErrorModulation;
    float rewardPredictionModulation;
    
    // Cognitive system effects
    float planningModifier;
    float attentionModifier;
    float conceptFormationModifier;
    float predictionModifier;
    
    // Age-dependent development
    float synapticStrengthFactor;
    float neuralEfficiency;
    float learningRate;
    
    // Experience-dependent development
    std::vector<std::vector<float>> recordedExperiences;
    double lastExperienceTime;
    
    Impl() : stage(DevelopmentalStage::Initial), stageStartStep(0), stepsInCurrentStage(0), stageAge(0.0)
        , memoryCapacityModifier(1.0f), memoryStabilityModifier(1.0f), workingMemoryModifier(1.0f)
        , replayStrengthModifier(1.0f), schemaFormationModifier(1.0f)
        , dopamineModulation(1.0f), curiosityModulation(1.0f), predictionErrorModulation(1.0f), rewardPredictionModulation(1.0f)
        , planningModifier(1.0f), attentionModifier(1.0f), conceptFormationModifier(1.0f), predictionModifier(1.0f)
        , synapticStrengthFactor(1.0f), neuralEfficiency(1.0f), learningRate(1.0f)
        , lastExperienceTime(0.0) {}
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

// ========== DEVELOPMENTAL EFFECTS ON MEMORY SYSTEMS ==========

void DevelopmentSystem::applyMemoryEffects(Brain* brain) {
    // Apply memory system effects based on developmental stage
    float stageMultiplier = getPlasticityModifier();
    
    // Update memory system modifiers based on stage
    pImpl->memoryCapacityModifier = calculateMemoryCapacityModifier();
    pImpl->memoryStabilityModifier = calculateMemoryStabilityModifier();
    pImpl->workingMemoryModifier = calculateWorkingMemoryModifier();
    pImpl->replayStrengthModifier = calculateReplayStrengthModifier();
    pImpl->schemaFormationModifier = calculateSchemaFormationModifier();
    
    // Apply effects to integrated memory systems if available
    if (brain) {
        // Memory systems integration (to be implemented based on Brain::Impl structure)
        // Memory systems can be accessed via brain->pImpl->workingMemory, etc.
    }
}

float DevelopmentSystem::calculateMemoryCapacityModifier() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 0.3f;  // Limited capacity initially
        case DevelopmentalStage::CriticalPeriod: return 0.6f;  // Rapid expansion
        case DevelopmentalStage::Maturation: return 0.9f;  // Near adult capacity
        case DevelopmentalStage::Adult: return 1.0f;  // Full capacity
        case DevelopmentalStage::Aging: return 0.8f;  // Slight decline with age
        default: return 1.0f;
    }
}

float DevelopmentSystem::calculateMemoryStabilityModifier() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 0.5f;  // Unstable memories
        case DevelopmentalStage::CriticalPeriod: return 0.7f;  // Increasing stability
        case DevelopmentalStage::Maturation: return 0.9f;  // High stability
        case DevelopmentalStage::Adult: return 1.0f;  // Maximum stability
        case DevelopmentalStage::Aging: return 0.6f;  // Some decline in stability
        default: return 1.0f;
    }
}

float DevelopmentSystem::calculateWorkingMemoryModifier() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 0.4f;  // Limited working memory
        case DevelopmentalStage::CriticalPeriod: return 0.8f;  // Rapid development
        case DevelopmentalStage::Maturation: return 1.0f;  // Mature working memory
        case DevelopmentalStage::Adult: return 1.0f;  // Stable
        case DevelopmentalStage::Aging: return 0.7f;  // Some decline
        default: return 1.0f;
    }
}

float DevelopmentSystem::calculateReplayStrengthModifier() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 0.3f;  // Minimal replay initially
        case DevelopmentalStage::CriticalPeriod: return 0.7f;  // Increased replay during critical period
        case DevelopmentalStage::Maturation: return 1.2f;  // Enhanced replay for integration
        case DevelopmentalStage::Adult: return 1.0f;  // Normal replay
        case DevelopmentalStage::Aging: return 0.9f;  // Moderate replay
        default: return 1.0f;
    }
}

float DevelopmentSystem::calculateSchemaFormationModifier() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 0.2f;  // Minimal schema formation
        case DevelopmentalStage::CriticalPeriod: return 0.6f;  // Beginning schema formation
        case DevelopmentalStage::Maturation: return 1.0f;  // Full schema formation
        case DevelopmentalStage::Adult: return 1.0f;  // Stable schema formation
        case DevelopmentalStage::Aging: return 0.8f;  // Reduced schema formation
        default: return 1.0f;
    }
}

float DevelopmentSystem::getMemoryCapacityModifier() const {
    return pImpl->memoryCapacityModifier;
}

float DevelopmentSystem::getMemoryStabilityModifier() const {
    return pImpl->memoryStabilityModifier;
}

float DevelopmentSystem::getWorkingMemoryModifier() const {
    return pImpl->workingMemoryModifier;
}

float DevelopmentSystem::getReplayStrengthModifier() const {
    return pImpl->replayStrengthModifier;
}

float DevelopmentSystem::getSchemaFormationModifier() const {
    return pImpl->schemaFormationModifier;
}

// ========== DEVELOPMENTAL EFFECTS ON NEUROMODULATION ==========

void DevelopmentSystem::applyNeuromodulationEffects(Brain* brain) {
    // Apply neuromodulation system effects based on developmental stage
    float stageMultiplier = getPlasticityModifier();
    
    // Update neuromodulation modifiers based on stage
    pImpl->dopamineModulation = calculateDopamineModulation();
    pImpl->curiosityModulation = calculateCuriosityModulation();
    pImpl->predictionErrorModulation = calculatePredictionErrorModulation();
    pImpl->rewardPredictionModulation = calculateRewardPredictionModulation();
    
    // Apply effects to integrated neuromodulation systems if available
    if (brain) {
        // Neuromodulation systems can be accessed via brain->pImpl->dopamine, etc.
    }
}

float DevelopmentSystem::calculateDopamineModulation() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 0.5f;  // Developing dopamine system
        case DevelopmentalStage::CriticalPeriod: return 0.8f;  // Strong modulation
        case DevelopmentalStage::Maturation: return 1.0f;  // Mature dopamine
        case DevelopmentalStage::Adult: return 1.0f;  // Stable
        case DevelopmentalStage::Aging: return 0.7f;  // Reduced modulation
        default: return 1.0f;
    }
}

float DevelopmentSystem::calculateCuriosityModulation() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 0.6f;  // High curiosity for exploration
        case DevelopmentalStage::CriticalPeriod: return 0.9f;  // Peak curiosity
        case DevelopmentalStage::Maturation: return 0.8f;  // Still high curiosity
        case DevelopmentalStage::Adult: return 0.6f;  // Moderate curiosity
        case DevelopmentalStage::Aging: return 0.4f;  // Declining curiosity
        default: return 1.0f;
    }
}

float DevelopmentSystem::calculatePredictionErrorModulation() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 0.7f;  // Developing error sensing
        case DevelopmentalStage::CriticalPeriod: return 0.9f;  // Enhanced error signaling
        case DevelopmentalStage::Maturation: return 1.0f;  // Mature error detection
        case DevelopmentalStage::Adult: return 1.0f;  // Stable
        case DevelopmentalStage::Aging: return 0.8f;  // Reduced sensitivity
        default: return 1.0f;
    }
}

float DevelopmentSystem::calculateRewardPredictionModulation() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 0.4f;  // Developing reward learning
        case DevelopmentalStage::CriticalPeriod: return 0.8f;  // Enhanced reward processing
        case DevelopmentalStage::Maturation: return 1.0f;  // Mature reward system
        case DevelopmentalStage::Adult: return 1.0f;  // Stable
        case DevelopmentalStage::Aging: return 0.7f;  // Reduced reward sensitivity
        default: return 1.0f;
    }
}

float DevelopmentSystem::getDopamineModulation() const {
    return pImpl->dopamineModulation;
}

float DevelopmentSystem::getCuriosityModulation() const {
    return pImpl->curiosityModulation;
}

float DevelopmentSystem::getPredictionErrorModulation() const {
    return pImpl->predictionErrorModulation;
}

float DevelopmentSystem::getRewardPredictionModulation() const {
    return pImpl->rewardPredictionModulation;
}

// ========== DEVELOPMENTAL EFFECTS ON COGNITION ==========

void DevelopmentSystem::applyCognitiveEffects(Brain* brain) {
    // Apply cognitive system effects based on developmental stage
    float stageMultiplier = getPlasticityModifier();
    
    // Update cognitive system modifiers based on stage
    pImpl->planningModifier = calculatePlanningModifier();
    pImpl->attentionModifier = calculateAttentionModifier();
    pImpl->conceptFormationModifier = calculateConceptFormationModifier();
    pImpl->predictionModifier = calculatePredictionModifier();
    
    // Apply effects to integrated cognitive systems if available
    if (brain) {
        // Cognitive systems can be accessed via brain->pImpl->planner, etc.
    }
}

float DevelopmentSystem::calculatePlanningModifier() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 0.3f;  // Limited planning ability
        case DevelopmentalStage::CriticalPeriod: return 0.7f;  // Developing planning
        case DevelopmentalStage::Maturation: return 1.0f;  // Mature planning
        case DevelopmentalStage::Adult: return 1.0f;  // Stable
        case DevelopmentalStage::Aging: return 0.8f;  // Slight decline
        default: return 1.0f;
    }
}

float DevelopmentSystem::calculateAttentionModifier() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 0.4f;  // Limited attention
        case DevelopmentalStage::CriticalPeriod: return 0.9f;  // Enhanced attention
        case DevelopmentalStage::Maturation: return 1.0f;  // Mature attention
        case DevelopmentalStage::Adult: return 1.0f;  // Stable
        case DevelopmentalStage::Aging: return 0.7f;  // Reduced attention control
        default: return 1.0f;
    }
}

float DevelopmentSystem::calculateConceptFormationModifier() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 0.5f;  // Early concept formation
        case DevelopmentalStage::CriticalPeriod: return 0.9f;  // Rapid concept development
        case DevelopmentalStage::Maturation: return 1.2f;  // Enhanced concept formation
        case DevelopmentalStage::Adult: return 1.0f;  // Stable
        case DevelopmentalStage::Aging: return 0.8f;  // Reduced flexibility
        default: return 1.0f;
    }
}

float DevelopmentSystem::calculatePredictionModifier() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 0.4f;  // Basic prediction
        case DevelopmentalStage::CriticalPeriod: return 0.8f;  // Developing prediction
        case DevelopmentalStage::Maturation: return 1.0f;  // Mature prediction
        case DevelopmentalStage::Adult: return 1.0f;  // Stable
        case DevelopmentalStage::Aging: return 0.7f;  // Reduced prediction accuracy
        default: return 1.0f;
    }
}

float DevelopmentSystem::getPlanningModifier() const {
    return pImpl->planningModifier;
}

float DevelopmentSystem::getAttentionModifier() const {
    return pImpl->attentionModifier;
}

float DevelopmentSystem::getConceptFormationModifier() const {
    return pImpl->conceptFormationModifier;
}

float DevelopmentSystem::getPredictionModifier() const {
    return pImpl->predictionModifier;
}

// ========== AGE-DEPENDENT DEVELOPMENT ==========

void DevelopmentSystem::applyAgeDependentEffects(Brain* brain, double age) {
    // Apply age-dependent developmental effects
    
    // Update age-dependent factors
    pImpl->synapticStrengthFactor = calculateSynapticStrengthFactor(age);
    pImpl->neuralEfficiency = calculateNeuralEfficiency(age);
    pImpl->learningRate = calculateLearningRate(age);
    
    // Apply effects based on age
    double ageFactor = calculateAgeFactor(age);
    
    // Memory system age effects
    pImpl->memoryCapacityModifier *= ageFactor;
    pImpl->memoryStabilityModifier *= ageFactor;
    
    // Neuromodulation age effects
    pImpl->dopamineModulation *= ageFactor;
    pImpl->curiosityModulation *= ageFactor;
    
    // Cognitive system age effects
    pImpl->planningModifier *= ageFactor;
    pImpl->attentionModifier *= ageFactor;
}

float DevelopmentSystem::calculateSynapticStrengthFactor(double age) const {
    // Synaptic strength follows an inverted U curve with age
    if (age < 1.0) return age;  // Growth phase
    else if (age < 10.0) return 1.0f;  // Peak strength
    else return std::max(0.1f, 20.0f / age);  // Declining strength
}

float DevelopmentSystem::calculateNeuralEfficiency(double age) const {
    // Neural efficiency increases with age but plateaus
    return std::min(1.0f, age / 10.0f);
}

float DevelopmentSystem::calculateLearningRate(double age) const {
    // Learning rate decreases with age
    return std::max(0.1f, 2.0f / age);
}

bool DevelopmentSystem::isAgeAppropriateCriticalPeriod(double age) const {
    // Critical period is only appropriate for young brains
    return age < 2.0;
}

float DevelopmentSystem::getNeuralEfficiency(double age) const {
    return calculateNeuralEfficiency(age);
}

float DevelopmentSystem::getLearningRate(double age) const {
    return calculateLearningRate(age);
}

// ========== EXPERIENCE-DEPENDENT DEVELOPMENT ==========

void DevelopmentSystem::applyExperienceDependentEffects(Brain* brain, const std::vector<float>& experiencePattern) {
    // Apply experience-dependent developmental effects
    
    // Record the experience
    recordExperience(experiencePattern);
    
    // Calculate experience-based plasticity enhancement
    float experienceEnhancement = calculateExperienceEnhancement(experiencePattern);
    
    // Update developmental modifiers based on experience
    pImpl->memoryCapacityModifier *= (1.0f + experienceEnhancement * 0.2f);
    pImpl->planningModifier *= (1.0f + experienceEnhancement * 0.15f);
    pImpl->conceptFormationModifier *= (1.0f + experienceEnhancement * 0.25f);
    
    // Apply experience effects to integrated systems if available
    if (brain) {
        // Experience-dependent effects can be applied to brain systems
    }
}

void DevelopmentSystem::recordExperience(const std::vector<float>& experiencePattern) {
    pImpl->recordedExperiences.push_back(experiencePattern);
    pImpl->lastExperienceTime = age_;
    
    // Keep only recent experiences
    if (pImpl->recordedExperiences.size() > 100) {
        pImpl->recordedExperiences.erase(pImpl->recordedExperiences.begin());
    }
}

float DevelopmentSystem::calculateExperienceEnhancement(const std::vector<float>& experiencePattern) const {
    if (experiencePattern.empty()) return 0.0f;
    
    // Calculate diversity of experience pattern
    float sum = 0.0f;
    for (float val : experiencePattern) {
        sum += val;
    }
    float avg = sum / experiencePattern.size();
    
    // Calculate variance (diversity)
    float variance = 0.0f;
    for (float val : experiencePattern) {
        variance += (val - avg) * (val - avg);
    }
    variance /= experiencePattern.size();
    
    // Experience enhancement is higher for diverse experiences
    return std::min(1.0f, variance * 2.0f);
}

float DevelopmentSystem::getExperiencePlasticityEnhancement(const std::vector<float>& experiencePattern) const {
    return calculateExperienceEnhancement(experiencePattern);
}

DevelopmentalStage DevelopmentSystem::getExperienceStage(const std::vector<float>& experiencePattern) const {
    float experienceValue = calculateExperienceEnhancement(experiencePattern);
    
    if (experienceValue > 0.7f) {
        return DevelopmentalStage::Maturation;
    } else if (experienceValue > 0.4f) {
        return DevelopmentalStage::CriticalPeriod;
    } else if (experienceValue > 0.2f) {
        return DevelopmentalStage::Adult;
    } else {
        return DevelopmentalStage::Initial;
    }
}

float DevelopmentSystem::getExperienceAdaptation(const std::vector<float>& experiencePattern) const {
    return calculateExperienceEnhancement(experiencePattern);
}

// ========== MEMORY CONSOLIDATION AND SCHEMA FORMATION ==========

float DevelopmentSystem::getSleepConsolidationStrength() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 0.6f;  // Moderate consolidation
        case DevelopmentalStage::CriticalPeriod: return 1.2f;  // Enhanced consolidation
        case DevelopmentalStage::Maturation: return 1.5f;  // Strong consolidation
        case DevelopmentalStage::Adult: return 1.0f;  // Normal consolidation
        case DevelopmentalStage::Aging: return 0.8f;  // Reduced consolidation
        default: return 1.0f;
    }
}

SimulationStep DevelopmentSystem::getReplayInterval() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 50;  // Frequent replay for learning
        case DevelopmentalStage::CriticalPeriod: return 100;  // Regular replay
        case DevelopmentalStage::Maturation: return 200;  // Moderate replay
        case DevelopmentalStage::Adult: return 300;  // Standard replay
        case DevelopmentalStage::Aging: return 400;  // Less frequent replay
        default: return 300;
    }
}

bool DevelopmentSystem::shouldFormSchemas() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return false;  // No schemas initially
        case DevelopmentalStage::CriticalPeriod: return pImpl->stageAge > 200.0;  // Begin schema formation
        case DevelopmentalStage::Maturation: return true;  // Full schema formation
        case DevelopmentalStage::Adult: return true;  // Continue schema formation
        case DevelopmentalStage::Aging: return pImpl->stageAge > 500.0;  // Reduced schema formation
        default: return false;
    }
}

float DevelopmentSystem::getMemoryPrioritization(const EpisodicMemoryItem* episode) const {
    if (!episode) return 0.5f;
    
    // Factor in episode age for prioritization
    float ageFactor = std::min(1.0f, static_cast<float>(episode->age) / 1000.0f);
    
    // Factor in reward for prioritization
    float rewardFactor = std::max(0.0f, episode->reward) / 10.0f;
    
    // Combine factors with developmental stage influence
    return (ageFactor * 0.5f + rewardFactor * 0.5f) * getPlasticityModifier();
}

// Helper function for calculating age factor
float DevelopmentSystem::calculateAgeFactor(double age) const {
    // Age factor modifies developmental effects
    if (age < 1.0) return 1.0f + age * 0.5f;  // Growth phase enhancement
    else if (age < 10.0) return 1.0f;  // Stable phase
    else return 1.0f - (age - 10.0) * 0.05f;  // Decline phase
}

} // namespace nlm
