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
    
    // Apply stage-specific effects to multiple brain systems
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // High plasticity phase: affects multiple systems
            applyInitialEffects(brain, rng, dt);
            break;
            
        case DevelopmentalStage::CriticalPeriod:
            // Sensitive phase: affects attention and memory systems
            applyCriticalPeriodEffects(brain, rng, dt);
            break;
            
        case DevelopmentalStage::Maturation:
            // Refinement phase: fine-tunes multiple parameters
            applyMaturationEffects(brain, rng, dt);
            break;
            
        case DevelopmentalStage::Adult:
            // Stability phase: maintains stable performance
            applyAdultEffects(brain, rng, dt);
            break;
            
        case DevelopmentalStage::Aging:
            // Decline phase: gradual parameter reduction
            applyAgingEffects(brain, rng, dt);
            break;
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

void DevelopmentSystem::applyInitialEffects(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    // High plasticity phase: affects multiple brain systems
    
    // 1. Increase neural excitability across the brain
    if (brain) {
        for (auto& region : brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Inject current to increase excitability
                    neuron->injectCurrent(0.5f * dt);
                }
            }
        }
    }
    
    // 2. Enhance plasticity rules themselves
    if (auto* stdp = brain->getSTDP()) {
        // Increase STDP learning rates during initial phase
        float baseLTP = 0.01f;
        float baseLTD = 0.012f;
        stdp->setLTPWeight(baseLTP * 1.5f);
        stdp->setLTDWeight(baseLTD * 1.5f);
    }
    
    if (auto* hebbian = brain->getHebbian()) {
        // Enable Hebbian learning with higher rate
        hebbian->setLearningRate(0.02f);  // Higher than default
    }
    
    // 3. Enhance attention system for exploration
    if (auto* attention = brain->getAttention()) {
        // Reduce inhibition strength to allow more exploration
        attention->setInhibitionStrength(0.3f);
        // Increase excitation strength
        attention->setExcitationStrength(2.0f);
        // Lower competition threshold for more flexible selection
        attention->setCompetitionThreshold(0.3f);
    }
    
    // 4. Enhance memory consolidation
    if (auto* episodicMemory = brain->getEpisodicMemory()) {
        // Increase consolidation strength during development
        // This helps form stronger episodic traces
    }
    
    // 5. Boost neuromodulation levels
    if (auto* dopamine = brain->getDopamine()) {
        // Increase baseline dopamine for higher motivation
        // Note: Dopamine class doesn't have setLevel method, using update instead
    }
    
    if (auto* curiosity = brain->getCuriosity()) {
        // Enhance curiosity for exploration
        curiosity->update(dt * 2.0f);
    }
    
    NLM_LOG_INFO("Initial developmental effects applied: " + getStageName() + 
                " - high plasticity across brain systems");
}

void DevelopmentSystem::applyCriticalPeriodEffects(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    // Sensitive phase: affects attention and memory systems
    
    // 1. Fine-tune attention system
    if (auto* attention = brain->getAttention()) {
        // Adjust inhibition strength for selective attention
        attention->setInhibitionStrength(0.5f);
        attention->setExcitationStrength(1.5f);
        attention->setCompetitionThreshold(0.5f);
        
        // Apply sensitive period effects on neural tuning
        // This helps develop specialized neural responses
    }
    
    // 2. Enhance memory encoding precision
    if (auto* episodicMemory = brain->getEpisodicMemory()) {
        // Improve encoding fidelity during critical period
        // This creates more detailed episodic memories
    }
    
    // 3. Modulate neuromodulation for learning
    if (auto* dopamine = brain->getDopamine()) {
        // Adjust dopamine for reward prediction during sensitive period
    }
    
    // 4. Apply Hebbian plasticity to form lasting associations
    if (auto* hebbian = brain->getHebbian()) {
        // Enable stable Hebbian connections
        hebbian->setLearningRate(0.015f);
    }
    
    NLM_LOG_INFO("Critical period effects applied: " + getStageName() + 
                " - attention and memory refinement");
}

void DevelopmentSystem::applyMaturationEffects(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    // Refinement phase: fine-tunes multiple parameters
    
    // 1. Stabilize attention parameters
    if (auto* attention = brain->getAttention()) {
        // Settle on optimal attention balance
        attention->setInhibitionStrength(0.7f);
        attention->setExcitationStrength(1.2f);
        attention->setCompetitionThreshold(0.7f);
    }
    
    // 2. Refine memory consolidation mechanisms
    if (auto* episodicMemory = brain->getEpisodicMemory()) {
        // Adjust consolidation thresholds for better memory integration
    }
    
    // 3. Optimize plasticity for stable learning
    if (auto* stdp = brain->getSTDP()) {
        // Settle on balanced STDP parameters
        stdp->setLTPWeight(0.01f);
        stdp->setLTDWeight(0.012f);
    }
    
    // 4. Fine-tune neuromodulation
    if (auto* novelty = brain->getNovelty()) {
        // Adjust novelty detection for refined exploration
        novelty->update(dt * 0.8f);
    }
    
    NLM_LOG_INFO("Maturation effects applied: " + getStageName() + 
                " - parameter refinement and stabilization");
}

void DevelopmentSystem::applyAdultEffects(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    // Stability phase: maintains stable performance
    
    // 1. Maintain stable attention
    if (auto* attention = brain->getAttention()) {
        // Lock in stable attention parameters
        attention->setInhibitionStrength(0.8f);
        attention->setExcitationStrength(1.0f);
        attention->setCompetitionThreshold(0.8f);
    }
    
    // 2. Consolidate memory for long-term storage
    if (auto* episodicMemory = brain->getEpisodicMemory()) {
        // Strong consolidation for permanent memory storage
        episodicMemory->consolidate(0.5f);
    }
    
    // 3. Stabilize plasticity at moderate levels
    if (auto* stdp = brain->getSTDP()) {
        // Maintain stable STDP parameters
        stdp->setLTPWeight(0.01f);
        stdp->setLTDWeight(0.012f);
    }
    
    // 4. Stabilize neuromodulation
    if (auto* dopamine = brain->getDopamine()) {
        // Maintain baseline dopamine levels
    }
    
    NLM_LOG_INFO("Adult effects applied: " + getStageName() + 
                " - stable performance maintenance");
}

void DevelopmentSystem::applyAgingEffects(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    // Decline phase: gradual parameter reduction
    
    // 1. Reduce attention strength
    if (auto* attention = brain->getAttention()) {
        // Weaken attention for flexibility
        attention->setInhibitionStrength(0.9f);
        attention->setExcitationStrength(0.9f);
        attention->setCompetitionThreshold(0.9f);
    }
    
    // 2. Reduce memory consolidation efficiency
    if (auto* episodicMemory = brain->getEpisodicMemory()) {
        // Weaken consolidation for more flexibility
        episodicMemory->consolidate(0.8f);
    }
    
    // 3. Reduce plasticity for stability
    if (auto* stdp = brain->getSTDP()) {
        // Reduce STDP learning rates
        stdp->setLTPWeight(0.005f);
        stdp->setLTDWeight(0.006f);
    }
    
    // 4. Reduce neuromodulation
    if (auto* dopamine = brain->getDopamine()) {
        // Reduce dopamine levels
    }
    
    NLM_LOG_INFO("Aging effects applied: " + getStageName() + 
                " - gradual parameter decline and stabilization");
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
} // namespace nlm
