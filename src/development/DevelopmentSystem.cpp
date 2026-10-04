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
    
    // Apply developmental effects to brain systems
    if (brain) {
        applyToBrain(brain, dt);
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

void DevelopmentSystem::applyToBrain(Brain* brain, TimestepDuration dt) {
    if (!brain) return;
    
    // Get developmental stage effects
    float plasticityMod = getPlasticityModifier();
    float excitabilityMod = getExcitabilityModifier();
    float attentionMod = getAttentionModifier();
    float memoryMod = getMemoryConsolidationModifier();
    
    // Apply to plasticity systems
    if (auto* stdp = brain->getSTDP()) {
        // Modulate STDP parameters based on developmental stage
        float currentLtp = stdp->getLTPWeight();
        float currentLtd = stdp->getLTDWeight();
        
        // In early development, enhance plasticity
        stdp->setLTPWeight(currentLtp * plasticityMod);
        stdp->setLTDWeight(currentLtd * plasticityMod);
    }
    
    if (auto* hebbian = brain->getHebbian()) {
        // Hebbian learning also modulated by development
        // (Hebbian implementation would need to be enhanced)
    }
    
    if (auto* structural = brain->getStructuralPlasticity()) {
        // Structural plasticity rate affected by development
        float synaptogenesisRate = 0.0001f * plasticityMod;
        float pruningRate = 0.00001f * (2.0f - plasticityMod);
        structural->setSynaptogenesisRate(synaptogenesisRate);
        structural->setPruningRate(pruningRate);
    }
    
    // Apply to neuromodulation
    if (auto* dopamine = brain->getDopamine()) {
        // Dopamine level affected by development
        float currentDA = dopamine->getLevel();
        dopamine->setLevel(currentDA * (1.0f + (1.0f - plasticityMod) * 0.2f));
    }
    
    if (auto* acetylcholine = brain->getAcetylcholine()) {
        // ACh affects attention development
        float currentACh = acetylcholine->getLevel();
        acetylcholine->setLevel(currentACh * attentionMod);
    }
    
    if (auto* norepinephrine = brain->getNorepinephrine()) {
        // NE affects arousal and stress response
        float currentNE = norepinephrine->getLevel();
        norepinephrine->setLevel(currentNE * excitabilityMod);
    }
    
    if (auto* serotonin = brain->getSerotonin()) {
        // Serotonin affects mood and social development
        float current5HT = serotonin->getLevel();
        serotonin->setLevel(current5HT * moodMod);
    }
    
    // Apply to neural excitability (like neuromodulators)
    float totalExcitability = excitabilityMod;
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Developmental excitability modulation
                if (totalExcitability > 1.0f) {
                    neuron->injectCurrent(totalExcitability * 0.1f);
                }
            }
        }
    }
    
    // Apply to memory systems
    if (auto* workingMem = brain->getWorkingMemory()) {
        // Working memory capacity affected by development
        int capacity = static_cast<int>(300 * plasticityMod);
        workingMem->setCapacity(capacity);
    }
    
    if (auto* episodicMem = brain->getEpisodicMemory()) {
        // Episodic memory consolidation affected by development
        float consolidationRate = memoryMod * 0.2f;
        episodicMem->setConsolidationRate(consolidationRate);
    }
    
    // Apply to attention system
    if (auto* attention = brain->getAttention()) {
        // Attention modulation through development
        attention->setInhibitionStrength(0.5f * (1.0f - attentionMod));
        attention->setExcitationStrength(1.5f * attentionMod);
    }
    
    // Apply to prediction system
    if (auto* prediction = brain->getPredictionSystem()) {
        // Prediction system affects developmental learning
        // (Implementation would depend on prediction system interface)
    }
    
    // Apply to cognition systems
    if (auto* planner = brain->getPlanner()) {
        // Planning complexity affected by development
        planner->setPlanningDepth(static_cast<int>(5 * plasticityMod));
    }
    
    if (auto* conceptFormation = brain->getConceptFormation()) {
        // Concept formation affected by development
        // (Implementation would depend on concept formation interface)
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
