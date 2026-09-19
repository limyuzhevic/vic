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

void DevelopmentSystem::integrateWithBrain(Brain* brain) {
    if (!brain) return;
    
    NLM_LOG_INFO("DevelopmentSystem: Integrating with brain (Stage: " + std::string(getStageName()) + ")");
    
    // Update attention system based on developmental stage
    updateAttentionDevelopment(brain);
    
    // Update memory system based on developmental stage
    updateMemoryDevelopment(brain);
    
    // Update neuromodulation sensitivity based on developmental stage
    updateNeuromodulationDevelopment(brain);
    
    // Update plasticity rules based on developmental stage
    updatePlasticityDevelopment(brain);
    
    // Check for structural changes based on developmental stage
    if (shouldTriggerStructuralChanges()) {
        NLM_LOG_INFO("DevelopmentSystem: Triggering structural changes");
        auto* sp = brain->getStructuralPlasticity();
        if (sp) {
            // Stage-appropriate structural plasticity rates
            float synaptogenesisRate = getSynaptogenesisRate();
            float pruningRate = getPruningRate();
            sp->setSynaptogenesisRate(synaptogenesisRate);
            sp->setPruningRate(pruningRate);
        }
    }
}

void DevelopmentSystem::updateAttentionDevelopment(Brain* brain) {
    if (!brain) return;
    
    auto* attention = brain->getAttention();
    if (!attention) return;
    
    // Stage-appropriate attention modulation
    float attentionStrength = getAttentionStrengthModifier();
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // High attention focus, easily distracted
            attention->setInhibitionStrength(0.3f * attentionStrength);
            attention->setExcitationStrength(0.8f * attentionStrength);
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Developing selective attention
            attention->setInhibitionStrength(0.5f * attentionStrength);
            attention->setExcitationStrength(0.7f * attentionStrength);
            break;
        case DevelopmentalStage::Maturation:
            // Balanced attention control
            attention->setInhibitionStrength(0.6f * attentionStrength);
            attention->setExcitationStrength(0.6f * attentionStrength);
            break;
        case DevelopmentalStage::Adult:
            // Mature attention control
            attention->setInhibitionStrength(0.8f * attentionStrength);
            attention->setExcitationStrength(0.5f * attentionStrength);
            break;
        case DevelopmentalStage::Aging:
            // Declining attention control
            attention->setInhibitionStrength(0.9f * attentionStrength);
            attention->setExcitationStrength(0.4f * attentionStrength);
            break;
    }
}

void DevelopmentSystem::updateMemoryDevelopment(Brain* brain) {
    if (!brain) return;
    
    auto* workingMemory = brain->getWorkingMemory();
    auto* episodicMemory = brain->getEpisodicMemory();
    
    if (!workingMemory || !episodicMemory) return;
    
    // Stage-appropriate memory parameters
    float memoryConsolidation = getMemoryConsolidationModifier();
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // High memory encoding, low consolidation
            workingMemory->setCapacity(workingMemory->getCapacity() * (1.5f * memoryConsolidation));
            workingMemory->setDecayRate(0.1f / memoryConsolidation);  // Slower decay
            episodicMemory->setMaxEpisodes(episodicMemory->getMaxEpisodes() * 2);
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Rapid memory development, moderate consolidation
            workingMemory->setDecayRate(0.05f / memoryConsolidation);
            episodicMemory->setMaxEpisodes(episodicMemory->getMaxEpisodes() * 1.5);
            break;
        case DevelopmentalStage::Maturation:
            // Stable memory capacity
            workingMemory->setCapacity(workingMemory->getCapacity() * 1.0f);
            workingMemory->setDecayRate(0.02f / memoryConsolidation);
            episodicMemory->setMaxEpisodes(episodicMemory->getMaxEpisodes() * 1.2f);
            break;
        case DevelopmentalStage::Adult:
            // Optimized memory efficiency
            workingMemory->setCapacity(workingMemory->getCapacity() * 0.9f);
            workingMemory->setDecayRate(0.01f / memoryConsolidation);
            episodicMemory->setMaxEpisodes(episodicMemory->getMaxEpisodes());
            break;
        case DevelopmentalStage::Aging:
            // Declining memory capacity
            workingMemory->setCapacity(workingMemory->getCapacity() * 0.7f);
            workingMemory->setDecayRate(0.05f / memoryConsolidation);  // Faster decay
            episodicMemory->setMaxEpisodes(episodicMemory->getMaxEpisodes() * 0.7f);
            break;
    }
}

void DevelopmentSystem::updateNeuromodulationDevelopment(Brain* brain) {
    if (!brain) return;
    
    // Stage-appropriate neuromodulation sensitivity
    float neuromodulationSensitivity = getNeuromodulationSensitivity();
    
    // Update neuromodulation system gains based on developmental stage
    auto* dopamine = brain->getDopamine();
    auto* acetylcholine = brain->getAcetylcholine();
    auto* norepinephrine = brain->getNorepinephrine();
    auto* serotonin = brain->getSerotonin();
    auto* curiosity = brain->getCuriosity();
    auto* novelty = brain->getNovelty();
    
    if (dopamine) {
        // Dopamine sensitivity changes with development
        switch (pImpl->stage) {
            case DevelopmentalStage::Initial:
                // High reward sensitivity
                dopamine->setLevel(dopamine->getLevel() * (0.5f + neuromodulationSensitivity * 0.5f));
                break;
            case DevelopmentalStage::CriticalPeriod:
                // Developing reward sensitivity
                dopamine->setLevel(dopamine->getLevel() * (0.7f + neuromodulationSensitivity * 0.3f));
                break;
            default:
                // Stable dopamine levels
                break;
        }
    }
    
    if (acetylcholine) {
        // ACh sensitivity affects attention and memory
        acetylcholine->setLevel(acetylcholine->getLevel() * neuromodulationSensitivity);
    }
    
    if (norepinephrine) {
        // NE sensitivity affects arousal and stress response
        norepinephrine->setLevel(norepinephrine->getLevel() * (0.5f + neuromodulationSensitivity * 0.5f));
    }
    
    if (serotonin) {
        // 5-HT sensitivity affects mood and motivation
        serotonin->setLevel(serotonin->getLevel() * neuromodulationSensitivity);
    }
    
    if (curiosity) {
        // Curiosity sensitivity changes with development
        float curiosityLevel = curiosity->getCuriosityLevel();
        switch (pImpl->stage) {
            case DevelopmentalStage::Initial:
                // High curiosity, exploratory
                curiosityLevel = curiosityLevel * (0.8f + neuromodulationSensitivity * 0.4f);
                break;
            case DevelopmentalStage::CriticalPeriod:
                // Developing curiosity
                curiosityLevel = curiosityLevel * (0.6f + neuromodulationSensitivity * 0.3f);
                break;
            default:
                // Stabilized curiosity
                break;
        }
        // Note: Assuming curiosity has a setLevel method
    }
}

void DevelopmentSystem::updatePlasticityDevelopment(Brain* brain) {
    if (!brain) return;
    
    // Stage-appropriate plasticity rule modulation
    float plasticityStageModifier = getPlasticityModifier();
    
    // Update STDP parameters based on developmental stage
    auto* stdp = brain->getSTDP();
    if (stdp) {
        // STDP learning rate modulation
        float stdpLearningRate = getSTDPLearningRate() * plasticityStageModifier;
        // Note: Assuming STDP has a configure method for learning rate
        // stdp->configure(stdpLearningRate, stdp->getLTDWeight(), stdp->getTau());
    }
    
    // Update Hebbian parameters
    auto* hebbian = brain->getHebbian();
    if (hebbian) {
        // Hebbian learning strength modulation
        float hebbianStrength = getHebbianStrength() * plasticityStageModifier;
        // Note: Assuming Hebbian has methods to set strength
        // hebbian->setStrength(hebbianStrength);
    }
    
    // Update neuromodulation effects on plasticity
    auto* dopamine = brain->getDopamine();
    if (dopamine) {
        // Dopamine modulates plasticity via plasticity factor
        float dopaminePlasticityMod = dopamine->getPlasticityFactor();
        // Combine with developmental stage modifier
        float combinedPlasticityMod = plasticityStageModifier * dopaminePlasticityMod;
        // Apply combined modulation to plasticity systems
        // Note: Assuming plasticity systems have methods to set modulation
    }
}

bool DevelopmentSystem::shouldTriggerStructuralChanges() const {
    // Check if developmental stage should trigger structural changes
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // High structural plasticity in initial stage
            return pImpl->stageAge > 30.0;  // Trigger after 30 simulation time units
        case DevelopmentalStage::CriticalPeriod:
            // Moderate structural changes during critical period
            return pImpl->stageAge > 150.0;
        case DevelopmentalStage::Maturation:
            // Low-level structural refinement
            return pImpl->stageAge > 400.0;
        case DevelopmentalStage::Adult:
            // Minimal structural changes in adulthood
            return pImpl->stageAge > 600.0;
        case DevelopmentalStage::Aging:
            // Some structural changes in aging
            return pImpl->stageAge > 800.0;
        default:
            return false;
    }
}

float DevelopmentSystem::getAttentionStrengthModifier() const {
    // Return developmental stage multiplier for attention strength
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 0.6f;
        case DevelopmentalStage::CriticalPeriod: return 0.8f;
        case DevelopmentalStage::Maturation: return 1.0f;
        case DevelopmentalStage::Adult: return 1.2f;
        case DevelopmentalStage::Aging: return 0.8f;
        default: return 1.0f;
    }
}

float DevelopmentSystem::getMemoryConsolidationModifier() const {
    // Return developmental stage multiplier for memory consolidation
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 0.7f;
        case DevelopmentalStage::CriticalPeriod: return 0.85f;
        case DevelopmentalStage::Maturation: return 1.0f;
        case DevelopmentalStage::Adult: return 1.1f;
        case DevelopmentalStage::Aging: return 0.9f;
        default: return 1.0f;
    }
}

float DevelopmentSystem::getNeuromodulationSensitivity() const {
    // Return developmental stage multiplier for neuromodulation sensitivity
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 1.2f;
        case DevelopmentalStage::CriticalPeriod: return 1.0f;
        case DevelopmentalStage::Maturation: return 0.9f;
        case DevelopmentalStage::Adult: return 0.8f;
        case DevelopmentalStage::Aging: return 0.6f;
        default: return 1.0f;
    }
}

float DevelopmentSystem::getSynaptogenesisRate() const {
    // Stage-appropriate synaptogenesis rates
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 0.0002f;
        case DevelopmentalStage::CriticalPeriod: return 0.0001f;
        case DevelopmentalStage::Maturation: return 0.00005f;
        case DevelopmentalStage::Adult: return 0.00002f;
        case DevelopmentalStage::Aging: return 0.00001f;
        default: return 0.0001f;
    }
}

float DevelopmentSystem::getPruningRate() const {
    // Stage-appropriate pruning rates
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 0.000005f;
        case DevelopmentalStage::CriticalPeriod: return 0.00001f;
        case DevelopmentalStage::Maturation: return 0.00002f;
        case DevelopmentalStage::Adult: return 0.00003f;
        case DevelopmentalStage::Aging: return 0.00004f;
        default: return 0.00001f;
    }
}

float DevelopmentSystem::getSTDPLearningRate() const {
    // Stage-appropriate STDP learning rates
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 0.015f;
        case DevelopmentalStage::CriticalPeriod: return 0.012f;
        case DevelopmentalStage::Maturation: return 0.01f;
        case DevelopmentalStage::Adult: return 0.008f;
        case DevelopmentalStage::Aging: return 0.006f;
        default: return 0.01f;
    }
}

float DevelopmentSystem::getHebbianStrength() const {
    // Stage-appropriate Hebbian strength
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 1.2f;
        case DevelopmentalStage::CriticalPeriod: return 1.0f;
        case DevelopmentalStage::Maturation: return 0.9f;
        case DevelopmentalStage::Adult: return 0.8f;
        case DevelopmentalStage::Aging: return 0.7f;
        default: return 1.0f;
    }
}

} // namespace nlm
