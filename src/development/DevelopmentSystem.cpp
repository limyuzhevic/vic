#include "DevelopmentSystem.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../neuromodulation/Acetylcholine.hpp"
#include "../neuromodulation/Norepinephrine.hpp"
#include "../neuromodulation/Serotonin.hpp"

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
    
    // Apply developmental effects on brain systems
    applyDevelopmentalEffects(brain, rng, dt);
    
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

void DevelopmentSystem::applyDevelopmentalEffects(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    if (!brain) return;
    
    // Neural excitability: adjust neuron firing thresholds based on developmental stage
    applyNeuralExcitabilityEffects(brain, dt);
    
    // Attention system: enhance attentional selection capabilities
    applyAttentionEffects(brain);
    
    // Memory consolidation: increase consolidation strength based on age
    applyMemoryConsolidationEffects(brain);
    
            break;
        case DevelopmentalStage::CriticalPeriod:
            excitabilityModifier = 1.1f;  // Elevated excitability during critical period
            break;
        case DevelopmentalStage::Maturation:
            excitabilityModifier = 1.0f;  // Normal excitability
            break;
        case DevelopmentalStage::Adult:
            excitabilityModifier = 0.9f;  // Slightly reduced excitability
            break;
        case DevelopmentalStage::Aging:
            excitabilityModifier = 0.8f;  // Reduced excitability in aging
            break;
        default:
            excitabilityModifier = 1.0f;
    }
    
    // Apply to all neurons in the brain
    for (const auto& region : brain->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (Neuron* neuron : pop->getNeurons()) {
                // Adjust neuron threshold based on developmental excitability
                float originalThreshold = neuron->getThreshold();
                float newThreshold = originalThreshold / excitabilityModifier;
                
                // Age-dependent threshold tuning (neurons become less excitable with age)
                newThreshold *= (1.0f - (static_cast<float>(age_) / 300.0f) * 0.1f);
                
                neuron->setThreshold(newThreshold);
            }
        }
    }
}

void DevelopmentSystem::applyAttentionEffects(Brain* brain) {
    // Enhance attentional selection based on developmental stage
    if (auto* attention = brain->getAttention()) {
        float attentionEnhancement = 1.0f;
        
        switch (pImpl->stage) {
            case DevelopmentalStage::Initial:
                attentionEnhancement = 0.7f;  // Initial stage: selective attention development
                break;
            case DevelopmentalStage::CriticalPeriod:
                attentionEnhancement = 1.5f;  // Critical period: enhanced attentional tuning
                break;
            case DevelopmentalStage::Maturation:
                attentionEnhancement = 1.2f;  // Maturation: refined attention
                break;
            case DevelopmentalStage::Adult:
                attentionEnhancement = 1.0f;  // Stable adult attention
                break;
            case DevelopmentalStage::Aging:
                attentionEnhancement = 0.8f;  // Age-related attentional decline
                break;
            default:
                attentionEnhancement = 1.0f;
        }
        
        // Apply attentional enhancement
        attention->enhanceAttention(attentionEnhancement);
        
        // Critical period bonus: stronger attentional modulation
        if (isCriticalPeriod()) {
            float criticalProgress = getCriticalPeriodProgress();
            attention->enhanceAttention(1.0f + (criticalProgress * 0.5f));
        }
    }
}

void DevelopmentSystem::applyMemoryConsolidationEffects(Brain* brain) {
    // Enhance memory consolidation based on developmental age
    if (auto* episodicMemory = brain->getEpisodicMemory()) {
        float consolidationStrength = 1.0f;
        
        // Younger brains consolidate more rapidly
        consolidationStrength *= (1.0f - std::min(1.0f, static_cast<float>(age_) / 200.0f));
        
        // Critical period enhances memory integration
        if (isCriticalPeriod()) {
            consolidationStrength *= 1.3f;
        }
        
        // Apply consolidation effects
        episodicMemory->enhanceConsolidation(conservationStrength);
        
        // Working memory updates based on developmental stage
        if (auto* workingMemory = brain->getWorkingMemory()) {
            workingMemory->updateCapacity(getPlasticityModifier());
        }
    }
}

void DevelopmentSystem::applyNeuromodulationEffects(Brain* brain) {
    // Adjust neuromodulation levels based on developmental stage
    float baseModulation = 1.0f;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            baseModulation = 0.8f;  // Lower neuromodulation in initial stage
            break;
        case DevelopmentalStage::CriticalPeriod:
            baseModulation = 1.2f;  // Elevated neuromodulation during critical period
            break;
        case DevelopmentalStage::Maturation:
            baseModulation = 1.0f;  // Normal neuromodulation
            break;
        case DevelopmentalStage::Adult:
            baseModulation = 0.9f;  // Slight reduction in adulthood
            break;
        case DevelopmentalStage::Aging:
            baseModulation = 0.7f;  // Reduced neuromodulation in aging
            break;
        default:
            baseModulation = 1.0f;
    }
    
    // Apply to neuromodulator systems
    if (auto* acetylcholine = brain->getAcetylcholine()) {
        acetylcholine->adjustLevel(baseModulation);
    }
    
    if (auto* norepinephrine = brain->getNorepinephrine()) {
        norepinephrine->adjustLevel(baseModulation);
    }
    
    if (auto* serotonin = brain->getSerotonin()) {
        serotonin->adjustLevel(baseModulation);
    }
    
    // Dopamine levels adapt to developmental stage
    if (auto* dopamine = brain->getDopamine()) {
        float dopamineLevel = baseModulation;
        if (isCriticalPeriod()) {
            float progress = getCriticalPeriodProgress();
            dopamineLevel *= (1.0f + progress * 0.4f);  // Higher dopamine during critical period
        }
        dopamine->setLevel(dopamineLevel);
    }
}

} // namespace nlm

// ========== SLEEP/REST STAGE MANAGEMENT ==========

bool DevelopmentSystem::isResting() const {
    // Note: This would typically check the brain's resting state
    // For now, we assume development can occur during both awake and sleep states
    return false;
}

SimulationStep DevelopmentSystem::getSleepStartStep() const {
    // Note: This would typically check the brain's sleep start step
    return 0;
}

int DevelopmentSystem::getSleepStage() const {
    // Note: This would typically check the brain's sleep stage
    return 0;
}

void DevelopmentSystem::applySleepEffects(float sleepModulation) {
    // Apply sleep-related developmental effects
    // This modifies developmental processes based on sleep state
    NLM_LOG_INFO("Development system: sleep modulation applied (factor: " + 
                 std::to_string(sleepModulation) + ")");
    
    // Sleep enhances synaptic plasticity and memory integration
    // Sleep-stage specific effects are handled by the brain system
    if (sleepModulation > 1.0f) {
        // Enhanced plasticity during sleep
        // This would affect synaptic refinement and memory consolidation
        NLM_LOG_INFO("Sleep-enhanced developmental plasticity active");
    }
}

void DevelopmentSystem::enhanceMemoryConsolidationDuringSleep() {
    // Enhanced memory consolidation during sleep
    if (pImpl->episodicMemory) {
        // Sleep-stage dependent consolidation
        float consolidationStrength = 1.5f;  // 50% enhancement during sleep
        
        // During deep sleep (stage 2), focus on declarative memory consolidation
        // During REM sleep (stage 3), focus on procedural memory integration
        
        pImpl->episodicMemory->enhanceConsolidation(conservationStrength);
        NLM_LOG_INFO("Sleep-enhanced memory consolidation (development)");
    }
}

void DevelopmentSystem::adjustPlasticityForSleep(int sleepStage) {
    // Adjust plasticity based on sleep stage
    switch (sleepStage) {
        case 1: // Light sleep
            // Moderate plasticity reduction for energy conservation
            // but maintain basic refinement processes
            break;
        case 2: // Deep sleep
            // High plasticity for synaptic downscaling and refinement
            // Critical for developmental processes
            NLM_LOG_INFO("Deep sleep: enhanced synaptic refinement and pruning");
            break;
        case 3: // REM sleep
            // Focused plasticity for memory integration and procedural learning
            NLM_LOG_INFO("REM sleep: enhanced procedural memory integration");
            break;
        default:
            // Normal plasticity rate
            break;
    }
}

void DevelopmentSystem::processDuringSleep(Brain* brain) {
    // Process developmental state during sleep
    if (!brain) return;
    
    // During sleep, development processes continue but at reduced rate
    double sleepDevelopmentalRate = 0.3;  // 30% of normal developmental rate
    
    // Sleep-stage specific development effects
    if (auto* brainSleepStage = getSleepStage(nullptr)) {
        // Sleep stage influences developmental timing
        switch (*brainSleepStage) {
            case 1: // Light sleep (N1)
                // Consolidation of early developmental changes
                if (age_ < 60.0) {  // Initial developmental phase
                    age_ += 0.1 * sleepDevelopmentalRate;
                }
                break;
            case 2: // Deep sleep (N3)
                // Critical period processing enhanced during deep sleep
                if (isCriticalPeriod()) {
                    double criticalProgress = getCriticalPeriodProgress();
                    age_ += 0.5 * sleepDevelopmentalRate * (1.0 + criticalProgress * 0.5);
                }
                break;
            case 3: // REM sleep
                // Integration of learned experiences
                age_ += 0.4 * sleepDevelopmentalRate;
                break;
            default:
                age_ += 0.3 * sleepDevelopmentalRate;
        }
    }
    
    // Sleep enhances synaptic pruning and myelination
    if (brain) {
        applySleepPlasticityEffects(brain);
    }
}

// Enhanced developmental processing during sleep periods
void DevelopmentSystem::applySleepPlasticityEffects(Brain* brain) {
    if (!brain) return;
    
    // Sleep-dependent plasticity enhancement
    float sleepPlasticityBoost = 1.5f;  // 50% increase in plasticity during sleep
    
    // Synaptic downscaling during deep sleep
    if (getSleepStage(nullptr) == 2) {  // Deep sleep (N3)
        // Enhance pruning during deep sleep
        if (auto* brainDevStage = getDevelopmentalStage(nullptr)) {
            if (*brainDevStage <= DevelopmentalStage::Adult) {
                // Critical for normal development
                // Synaptic refinement occurs during deep sleep
                // This is naturally handled by brain-level sleep processing
            }
        }
    }
    
    // Memory integration during REM sleep
    if (getSleepStage(nullptr) == 3) {  // REM sleep
        // Enhance concept formation during REM
        if (auto* conceptForm = brain->getConceptFormation()) {
            conceptForm->enhanceDuringSleep(sleepPlasticityBoost);
        }
        
        // Strengthen procedural memory traces
        if (auto* proceduralMemory = brain->getProceduralMemory()) {
            proceduralMemory->strengthenDuringSleep(sleepPlasticityBoost);
        }
    }
    
    // Overall sleep enhancement for all developmental processes
    // (This is handled at the brain level in the Brain.cpp implementation)
}

// Get current developmental stage from brain
DevelopmentalStage DevelopmentSystem::getDevelopmentalStage(Brain* brain) const {
    if (brain) {
        return brain->getDevelopmentalStage();
    }
    return pImpl->stage;
}

// Get sleep stage from brain (helper function)
int DevelopmentSystem::getSleepStage(Brain* brain) {
    if (brain) {
        return brain->getSleepStage();
    }
    return 0;
}

} // namespace nlm
