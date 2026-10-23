#include "DevelopmentSystem.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct DevelopmentSystem::Impl {
    DevelopmentalStage stage;
    SimulationStep stageStartStep;
    SimulationStep stepsInCurrentStage;
    double stageAge;  // Age since entering current stage
    
    Impl() : stage(DevelopmentalStage::Initial), stageStartStep(0), stepsInCurrentStage(0), stageAge(0.0) {}
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

// Enhanced development effects
float DevelopmentSystem::getMemoryConsolidationModifier() const {
    // Memory consolidation improves with development
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 0.1f;
        case DevelopmentalStage::CriticalPeriod: return 0.4f;
        case DevelopmentalStage::Maturation: return 0.7f;
        case DevelopmentalStage::Adult: return 1.0f;
        case DevelopmentalStage::Aging: return 0.8f;
        default: return 0.5f;
    }
}

float DevelopmentSystem::getAttentionModulator() const {
    // Attention system matures through development
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 0.3f;  // Limited attention
        case DevelopmentalStage::CriticalPeriod: return 0.6f;
        case DevelopmentalStage::Maturation: return 0.9f;
        case DevelopmentalStage::Adult: return 1.2f;  // Enhanced attention
        case DevelopmentalStage::Aging: return 0.7f;
        default: return 0.5f;
    }
}

float DevelopmentSystem::getNeuralExcitabilityModifier() const {
    // Neural excitability changes through development
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 1.5f;  // High excitability
        case DevelopmentalStage::CriticalPeriod: return 1.2f;
        case DevelopmentalStage::Maturation: return 1.0f;
        case DevelopmentalStage::Adult: return 0.8f;  // Reduced excitability
        case DevelopmentalStage::Aging: return 0.6f;
        default: return 1.0f;
    }
}

float DevelopmentSystem::getLearningRateModifier() const {
    // Learning rate follows an inverted U-shaped curve
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 1.5f;  // High learning rate
        case DevelopmentalStage::CriticalPeriod: return 1.3f;
        case DevelopmentalStage::Maturation: return 1.0f;
        case DevelopmentalStage::Adult: return 0.7f;
        case DevelopmentalStage::Aging: return 0.5f;
        default: return 1.0f;
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

void DevelopmentSystem::update(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    age_ += dt;
    pImpl->stageAge += dt;
    ++pImpl->stepsInCurrentStage;
    
    // Apply developmental effects to brain systems
    if (brain) {
        applyDevelopmentalEffects(brain, dt);
    }
    
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

void DevelopmentSystem::recordReplayEvent(const EpisodicMemoryItem& episode) {
    // Record that a replay event occurred - this affects development
    // In a full implementation, this would update developmental statistics
    // For now, we track the fact that replay happened
    pImpl->stageAge += 10.0;  // Replay accelerates development
    NLM_LOG_INFO("Development accelerated by replay event");
}

void DevelopmentSystem::applyDevelopmentalEffects(Brain* brain, TimestepDuration dt) {
    if (!brain) return;
    
    // Get developmental stage modifiers
    float memoryConsolidationMod = getMemoryConsolidationModifier();
    float attentionMod = getAttentionModulator();
    float neuralExcitabilityMod = getNeuralExcitabilityModifier();
    float learningRateMod = getLearningRateModifier();
    
    // Apply memory consolidation effects
    if (brain->getEpisodicMemory() && pImpl->stage != DevelopmentalStage::Initial) {
        // Development stages affect memory consolidation rates
        if (isCriticalPeriod()) {
            // Critical period: enhanced consolidation
            brain->getEpisodicMemory()->setRelevancyThreshold(0.1f);
        } else if (pImpl->stage == DevelopmentalStage::Maturation) {
            // Maturation: balanced consolidation
            brain->getEpisodicMemory()->setRelevancyThreshold(0.3f);
        } else {
            // Adult: normal consolidation
            brain->getEpisodicMemory()->setRelevancyThreshold(0.5f);
        }
    }
    
    // Apply attention system effects
    if (brain->getAttention()) {
        // Development affects attention parameters
        float inhibitionStrength = 0.5f / attentionMod;
        float excitationStrength = 1.0f * attentionMod;
        
        brain->getAttention()->setInhibitionStrength(inhibitionStrength);
        brain->getAttention()->setExcitationStrength(excitationStrength);
    }
    
    // Apply neural excitability effects
    if (pImpl->stage != DevelopmentalStage::Aging) {
        // Development stages affect neural excitability
        // In a full implementation, this would modify brain parameters
        // For now, we log the effects
        NLM_LOG_INFO(std::string("Developmental stage: ") + getStageName() + 
                     std::string(", Neural excitability modifier: ") + std::to_string(neuralExcitabilityMod));
    }
    
    // Apply learning rate effects
    if (pImpl->stage != DevelopmentalStage::Aging) {
        // Development affects learning rates
        // In a full implementation, this would modify plasticity rules
        // For now, we log the effects
        NLM_LOG_INFO(std::string("Developmental stage: ") + getStageName() + 
                     std::string(", Learning rate modifier: ") + std::to_string(learningRateMod));
    }
    
    // Special effects for critical period
    if (isCriticalPeriod()) {
        // Critical period has enhanced plasticity and learning
        float criticalPeriodProgress = getCriticalPeriodProgress();
        
        // Boost plasticity during critical period
        float enhancedPlasticity = 1.0f + criticalPeriodProgress * 0.5f;
        NLM_LOG_INFO(std::string("Critical period progress: ") + std::to_string(criticalPeriodProgress) + 
                     std::string(", Enhanced plasticity: ") + std::to_string(enhancedPlasticity));
        
        // Enhanced memory encoding during critical period
        if (brain->getWorkingMemory()) {
            brain->getWorkingMemory()->setDecayRate(0.005f * (1.0f - criticalPeriodProgress));
        }
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

} // namespace nlm
