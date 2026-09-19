#include "DevelopmentSystem.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct DevelopmentSystem::Impl {
    DevelopmentalStage stage;
    SimulationStep stageStartStep;
    SimulationStep stepsInCurrentStage;
    double stageAge;  // Age since entering current stage
    double basePlasticity;  // Base plasticity level
    double criticalPeriodEndAge;
    double maturationEndAge;
    double maxDevelopmentAge;
    
    Impl() 
        : stage(DevelopmentalStage::Initial)
        , stageStartStep(0)
        , stepsInCurrentStage(0)
        , stageAge(0.0)
        , basePlasticity(1.0)
        , criticalPeriodEndAge(360.0)
        , maturationEndAge(960.0)
        , maxDevelopmentAge(2880.0) {}  // 48 hours max
};

DevelopmentSystem::DevelopmentSystem() : pImpl(new Impl), age_(0.0) {}

DevelopmentSystem::~DevelopmentSystem() = default;

DevelopmentalStage DevelopmentSystem::getStage() const {
    return pImpl->stage;
}

void DevelopmentSystem::setStage(DevelopmentalStage stage) {
    pImpl->stage = stage;
    pImpl->stageAge = 0.0;
    pImpl->stageStartStep = 0;
    NLM_LOG_INFO("Development system entered stage: " + std::string(getStageName()));
}

void DevelopmentSystem::advanceStage() {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            pImpl->stage = DevelopmentalStage::CriticalPeriod;
            NLM_LOG_INFO("Entering Critical Period - synaptic refinement phase");
            break;
        case DevelopmentalStage::CriticalPeriod:
            pImpl->stage = DevelopmentalStage::Maturation;
            NLM_LOG_INFO("Entering Maturation - myelination and circuit optimization phase");
            break;
        case DevelopmentalStage::Maturation:
            pImpl->stage = DevelopmentalStage::Adult;
            NLM_LOG_INFO("Entering Adult - mature neural function phase");
            break;
        case DevelopmentalStage::Adult:
            pImpl->stage = DevelopmentalStage::Aging;
            NLM_LOG_INFO("Entering Aging - gradual decline and maintenance phase");
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
    
    // Apply developmental effects based on current stage
    applyDevelopmentEffects(brain, 0.001);
    
    // Auto-advance stage based on time in stage
    // These thresholds are in simulated steps
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
    
    // Apply developmental effects
    applyDevelopmentEffects(brain, dt);
    
    // Auto-advance stage based on developmental age
    // These thresholds are in simulation seconds
    if (pImpl->stage == DevelopmentalStage::Initial && age_ > 60.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod && age_ > pImpl->criticalPeriodEndAge) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::Maturation && age_ > pImpl->maturationEndAge) {
        advanceStage();
    }
}

float DevelopmentSystem::getPlasticityModifier() const {
    // Higher plasticity in early stages, lower in later stages
    float modifier = 1.0f;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: 
            modifier = 1.2f;  // Enhanced initial growth
            break;
        case DevelopmentalStage::CriticalPeriod:
            modifier = 0.8f;  // Refinement phase
            break;
        case DevelopmentalStage::Maturation:
            modifier = 0.5f;  // Optimization
            break;
        case DevelopmentalStage::Adult:
            modifier = 0.2f;  // Stable maintenance
            break;
        case DevelopmentalStage::Aging:
            modifier = 0.1f;  // Reduced plasticity
            break;
        default: modifier = 0.5f;
    }
    
    // Age-related modulation
    if (age_ > pImpl->criticalPeriodEndAge) {
        float ageFactor = std::max(0.0f, 1.0f - (age_ - pImpl->criticalPeriodEndAge) / (pImpl->maxDevelopmentAge - pImpl->criticalPeriodEndAge) * 0.5f);
        modifier *= ageFactor;
    }
    
    return modifier;
}

bool DevelopmentSystem::isCriticalPeriod() const {
    return pImpl->stage == DevelopmentalStage::CriticalPeriod;
}

float DevelopmentSystem::getCriticalPeriodProgress() const {
    if (pImpl->stage != DevelopmentalStage::CriticalPeriod) {
        return 0.0f;
    }
    // Progress through critical period (0 to 1)
    return std::min(1.0f, static_cast<float>(pImpl->stageAge / pImpl->criticalPeriodEndAge));
}

void DevelopmentSystem::applyDevelopmentEffects(Brain* brain, TimestepDuration dt) {
    if (!brain) return;
    
    // Stage-specific effects
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            applyInitialEffects(brain, dt);
            break;
        case DevelopmentalStage::CriticalPeriod:
            applyCriticalPeriodEffects(brain, dt);
            break;
        case DevelopmentalStage::Maturation:
            applyMaturationEffects(brain, dt);
            break;
        case DevelopmentalStage::Adult:
            applyAdultEffects(brain, dt);
            break;
        case DevelopmentalStage::Aging:
            applyAgingEffects(brain, dt);
            break;
    }
}

void DevelopmentSystem::applyInitialEffects(Brain* brain, TimestepDuration dt) {
    // Initial phase: high synaptogenesis, rapid growth
    auto* sp = brain->getStructuralPlasticity();
    if (sp) {
        // Boost synaptogenesis during initial phase
        float rate = 0.0002f * (1.0f + 0.5f * std::sin(age_ / 100.0));  // Oscillatory growth
        sp->setSynaptogenesisRate(rate);
        sp->setPruningRate(0.00001f * 0.5f);  // Low pruning initially
    }
}

void DevelopmentSystem::applyCriticalPeriodEffects(Brain* brain, TimestepDuration dt) {
    // Critical period: activity-dependent synaptic refinement
    auto* sp = brain->getStructuralPlasticity();
    if (sp) {
        // Activity-dependent refinement
        float refinementRate = 0.0001f * (1.0f + 0.3f * std::sin(age_ / 50.0));
        sp->setSynaptogenesisRate(refinementRate * 0.5f);
        sp->setPruningRate(0.00002f * (1.0f - getCriticalPeriodProgress() * 0.5f));  // High pruning early in critical period
        
        // This is when STDP is most effective
        if (brain->getSTDP()) {
            NLM_LOG_DEBUG("Critical period: STDP is highly effective for synaptic refinement");
        }
    }
    
    // Increase Hebbian learning during critical period
    if (brain->getHebbian()) {
        // Simulate increased learning rate through neuromodulation
        // In reality, acetylcholine and other neuromodulators would be elevated
        NLM_LOG_DEBUG("Critical period: Enhanced Hebbian learning for pattern formation");
    }
}

void DevelopmentSystem::applyMaturationEffects(Brain* brain, TimestepDuration dt) {
    // Maturation phase: myelination, pruning, circuit optimization
    auto* sp = brain->getStructuralPlasticity();
    if (sp) {
        // Gradual reduction in synaptogenesis
        float synaptogenesisRate = 0.00005f * (1.0f - std::min(1.0f, (age_ - pImpl->criticalPeriodEndAge) / 600.0));
        sp->setSynaptogenesisRate(synaptogenesisRate);
        
        // Controlled pruning for efficiency
        float pruningRate = 0.00003f * (0.5f + 0.5f * std::sin((age_ - pImpl->criticalPeriodEndAge) / 200.0));
        sp->setPruningRate(pruningRate);
    }
    
    // Stabilization of synaptic weights
    if (brain->getSTDP()) {
        // STDP becomes more stable
        NLM_LOG_DEBUG("Maturation: Synaptic weights stabilizing");
    }
}

void DevelopmentSystem::applyAdultEffects(Brain* brain, TimestepDuration dt) {
    // Adult phase: maintenance, skill learning, limited plasticity
    auto* sp = brain->getStructuralPlasticity();
    if (sp) {
        // Very low structural plasticity
        sp->setSynaptogenesisRate(0.00001f);
        sp->setPruningRate(0.000001f);
    }
    
    // Emphasis on Hebbian learning for skill acquisition
    if (brain->getHebbian()) {
        // Skill-based learning continues but with reduced overall plasticity
        NLM_LOG_DEBUG("Adult: Maintaining existing circuits, enabling skill learning");
    }
    
    // Neuromodulatory tone stabilizes
    if (brain->getDopamine()) {
        // Dopamine levels stabilize
        NLM_LOG_DEBUG("Adult: Neuromodulatory systems at steady state");
    }
}

void DevelopmentSystem::applyAgingEffects(Brain* brain, TimestepDuration dt) {
    // Aging phase: decline and maintenance
    auto* sp = brain->getStructuralPlasticity();
    if (sp) {
        // Reduced connectivity maintenance
        sp->setSynaptogenesisRate(0.000005f);
        sp->setPruningRate(0.0000005f);
    }
    
    // Cognitive decline mitigation through compensation
    if (brain->getSTDP()) {
        // Some adaptive plasticity remains
        NLM_LOG_DEBUG("Aging: Limited adaptive plasticity for compensation");
    }
}

} // namespace nlm
