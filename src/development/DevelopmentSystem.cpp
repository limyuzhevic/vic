#include "DevelopmentSystem.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct DevelopmentSystem::Impl {
    DevelopmentalStage stage;
    SimulationStep stageStartStep;
    SimulationStep stepsInCurrentStage;
    double stageAge;  // Age since entering current stage
    float plasticityModifier;  // Plasticity modifier for current stage
    
    Impl() : stage(DevelopmentalStage::Initial), stageStartStep(0), stepsInCurrentStage(0), stageAge(0.0), plasticityModifier(1.0f) {}
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
    
    // Apply development effects to brain systems
    if (brain) {
        applyDevelopmentEffects(brain);
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
    
    // Apply development effects to brain systems
    if (brain) {
        applyDevelopmentEffects(brain);
    }
}

// Apply development effects to various brain systems
void DevelopmentSystem::applyDevelopmentEffects(Brain* brain) {
    if (!brain || !pImpl) return;
    
    DevelopmentalStage stage = pImpl->stage;
    
    // Neural excitability effects
    // Development stages affect baseline neural excitability
    float excitabilityMod = 1.0f;
    switch (stage) {
        case DevelopmentalStage::Initial:
            excitabilityMod = 1.5f;  // High excitability during initial development
            break;
        case DevelopmentalStage::CriticalPeriod:
            excitabilityMod = 1.2f;  // Moderately high excitability
            break;
        case DevelopmentalStage::Maturation:
            excitabilityMod = 1.0f;  // Normal excitability
            break;
        case DevelopmentalStage::Adult:
            excitabilityMod = 0.9f;  // Slightly reduced excitability (mature brain)
            break;
        case DevelopmentalStage::Aging:
            excitabilityMod = 0.8f;  // Reduced excitability (aging brain)
            break;
        default:
            excitabilityMod = 1.0f;
    }
    
    // Apply excitability modulation to neurons
    // This would affect the base membrane potentials and thresholds
    // For now, just log the effect
    NLM_LOG_INFO(std::string(getStageName()) + " stage affecting neural excitability: " + 
                 std::to_string(excitabilityMod));
    
    // Attention system effects
    // Development stages affect attention parameters
    if (auto* attention = brain->getAttention()) {
        float attentionMod = 1.0f;
        switch (stage) {
            case DevelopmentalStage::Initial:
                attentionMod = 0.6f;  // Narrow attention focus during development
                break;
            case DevelopmentalStage::CriticalPeriod:
                attentionMod = 0.8f;  // Moderate attention control
                break;
            case DevelopmentalStage::Maturation:
                attentionMod = 1.0f;  // Normal attention
                break;
            case DevelopmentalStage::Adult:
                attentionMod = 1.2f;  // Enhanced attention control in adults
                break;
            case DevelopmentalStage::Aging:
                attentionMod = 1.0f;  // Stable attention in aging
                break;
            default:
                attentionMod = 1.0f;
        }
        
        // Apply attention modulation
        // This would affect attention inhibition/excitation balance
        NLM_LOG_INFO(std::string(getStageName()) + " stage affecting attention: " + 
                     std::to_string(attentionMod));
    }
    
    // Memory consolidation effects
    // Development stages affect memory consolidation rates
    if (auto* episodicMemory = brain->getEpisodicMemory()) {
        float consolidationMod = 1.0f;
        switch (stage) {
            case DevelopmentalStage::Initial:
                consolidationMod = 0.7f;  // Lower consolidation during development
                break;
            case DevelopmentalStage::CriticalPeriod:
                consolidationMod = 0.8f;  // Moderate consolidation
                break;
            case DevelopmentalStage::Maturation:
                consolidationMod = 1.0f;  // Normal consolidation
                break;
            case DevelopmentalStage::Adult:
                consolidationMod = 1.1f;  // Enhanced consolidation in adults
                break;
            case DevelopmentalStage::Aging:
                consolidationMod = 1.0f;  // Stable consolidation in aging
                break;
            default:
                consolidationMod = 1.0f;
        }
        
        // This would affect memory consolidation parameters
        NLM_LOG_INFO(std::string(getStageName()) + " stage affecting memory consolidation: " + 
                     std::to_string(consolidationMod));
    }
    
    // Neuromodulation baseline effects
    // Development stages affect baseline neuromodulator levels
    if (auto* dopamine = brain->getDopamine()) {
        float dopamineMod = 1.0f;
        switch (stage) {
            case DevelopmentalStage::Initial:
                dopamineMod = 1.2f;  // Higher baseline dopamine during development
                break;
            case DevelopmentalStage::CriticalPeriod:
                dopamineMod = 1.1f;  // Elevated baseline
                break;
            case DevelopmentalStage::Maturation:
                dopamineMod = 1.0f;  // Normal baseline
                break;
            case DevelopmentalStage::Adult:
                dopamineMod = 0.9f;  // Slightly reduced baseline in adults
                break;
            case DevelopmentalStage::Aging:
                dopamineMod = 0.8f;  // Reduced baseline in aging
                break;
            default:
                dopamineMod = 1.0f;
        }
        
        // This would affect neuromodulator baseline levels
        NLM_LOG_INFO(std::string(getStageName()) + " stage affecting dopamine baseline: " + 
                     std::to_string(dopamineMod));
    }
    
    // Plasticity rule effects
    // Development stages affect which plasticity rules are active
    float plasticityMod = 1.0f;
    switch (stage) {
        case DevelopmentalStage::Initial:
            plasticityMod = 1.5f;  // High plasticity during initial stage
            break;
        case DevelopmentalStage::CriticalPeriod:
            plasticityMod = 1.2f;  // Elevated plasticity
            break;
        case DevelopmentalStage::Maturation:
            plasticityMod = 1.0f;  // Normal plasticity
            break;
        case DevelopmentalStage::Adult:
            plasticityMod = 0.8f;  // Reduced plasticity (stability)
            break;
        case DevelopmentalStage::Aging:
            plasticityMod = 0.6f;  // Reduced plasticity (rigidity)
            break;
        default:
            plasticityMod = 1.0f;
    }
    
    // Store plasticity modifier for other systems to use
    pImpl->plasticityModifier = plasticityMod;
    
    NLM_LOG_INFO(std::string(getStageName()) + " stage affecting plasticity: " + 
                 std::to_string(plasticityMod));
}

float DevelopmentSystem::getPlasticityModifier() const {
    return pImpl->plasticityModifier;
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
