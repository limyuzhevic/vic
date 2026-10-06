#include "Development.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <numeric>

namespace nlm {

struct Development::Impl {
    double age;
    DevelopmentalStage currentStage;
    double stageStartAge;
    double stageTransitionTimer;
    
    // Neural excitability parameters
    float baselineExcitability;
    float peakExcitabilityAge;
    float lateExcitabilityFactor;
    
    // Attention capacity parameters
    float baselineAttentionCapacity;
    float criticalPeriodAttentionBoost;
    float maturationAttentionOptimization;
    
    // Memory consolidation parameters
    float baselineConsolidationStrength;
    float criticalPeriodReplayIntensity;
    float adultConsolidationEfficiency;
    
    // Neuromodulator release parameters
    std::unordered_map<NeuromodulatorType, float> baselineReleaseLevels;
    std::unordered_map<NeuromodulatorType, float> ageModifiers;
    
    // Plasticity rule parameters
    std::unordered_map<PlasticityType, float> baselinePlasticityRules;
    std::unordered_map<PlasticityType, float> ageBasedModifications;
    
    // Stage timing
    double initialStageDuration;
    double criticalPeriodDuration;
    double maturationDuration;
    double adultStageDuration;
    double agingStageDuration;
    
    // Learning and behavior parameters
    float learningRateBaseline;
    float learningRateCriticalPeriod;
    float learningRateMaturation;
    float learningRateAdult;
    
    float explorationBiasBaseline;
    float explorationBiasCriticalPeriod;
    float explorationBiasMaturation;
    float explorationBiasAdult;
    
    float memoryRetentionBaseline;
    float memoryRetentionCriticalPeriod;
    float memoryRetentionMaturation;
    float memoryRetentionAdult;
    
    // Developmental window tracking
    bool inCriticalPeriod;
    bool inSensitivePeriod;
    bool inMaturationWindow;
    
    // Neural changes tracking
    double totalSynapticChanges;
    double totalPruningEvents;
    double totalGrowthEvents;
    double adaptationMemory;
    
    Impl() 
        : age(0.0), currentStage(DevelopmentalStage::Initial), stageStartAge(0.0), stageTransitionTimer(0.0)
        , baselineExcitability(1.0f), peakExcitabilityAge(30.0), lateExcitabilityFactor(0.7f)
        , baselineAttentionCapacity(1.0f), criticalPeriodAttentionBoost(2.0f), maturationAttentionOptimization(1.5f)
        , baselineConsolidationStrength(1.0f), criticalPeriodReplayIntensity(3.0f), adultConsolidationEfficiency(1.2f)
        , initialStageDuration(60.0), criticalPeriodDuration(300.0), maturationDuration(600.0), 
          adultStageDuration(3600.0), agingStageDuration(1800.0)
        , learningRateBaseline(0.1f), learningRateCriticalPeriod(0.5f), learningRateMaturation(0.3f), learningRateAdult(0.15f)
        , explorationBiasBaseline(0.5f), explorationBiasCriticalPeriod(0.8f), explorationBiasMaturation(0.7f), explorationBiasAdult(0.3f)
        , memoryRetentionBaseline(0.5f), memoryRetentionCriticalPeriod(0.7f), memoryRetentionMaturation(0.8f), memoryRetentionAdult(0.9f)
        , inCriticalPeriod(false), inSensitivePeriod(false), inMaturationWindow(false)
        , totalSynapticChanges(0.0), totalPruningEvents(0.0), totalGrowthEvents(0.0), adaptationMemory(0.0) {
        
        // Initialize baseline neuromodulator release levels
        baselineReleaseLevels[NeuromodulatorType::Dopamine] = 0.5f;
        baselineReleaseLevels[NeuromodulatorType::Acetylcholine] = 0.3f;
        baselineReleaseLevels[NeuromodulatorType::Norepinephrine] = 0.2f;
        baselineReleaseLevels[NeuromodulatorType::Serotonin] = 0.4f;
        
        // Initialize age modifiers
        ageModifiers[NeuromodulatorType::Dopamine] = 1.0f;
        ageModifiers[NeuromodulatorType::Acetylcholine] = 1.0f;
        ageModifiers[NeuromodulatorType::Norepinephrine] = 1.0f;
        ageModifiers[NeuromodulatorType::Serotonin] = 1.0f;
        
        // Initialize baseline plasticity rules
        baselinePlasticityRules[PlasticityType::STDP] = 1.0f;
        baselinePlasticityRules[PlasticityType::Hebbian] = 1.0f;
        baselinePlasticityRules[PlasticityType::Structural] = 1.0f;
        
        // Initialize age-based modifications
        ageBasedModifications[PlasticityType::STDP] = 1.0f;
        ageBasedModifications[PlasticityType::Hebbian] = 1.0f;
        ageBasedModifications[PlasticityType::Structural] = 1.0f;
    }
};

Development::Development() 
    : pImpl(new Impl()), age(0.0), currentStage(DevelopmentalStage::Initial), 
      stageStartAge(0.0), stageTransitionTimer(0.0), brain(nullptr) {
}

Development::~Development() {
    delete pImpl;
}

void Development::update(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    if (!brain) return;
    this->brain = brain;
    
    // Update age
    age += dt;
    pImpl->age = age;
    
    // Update developmental stage if needed
    updateDevelopmentalStage(brain, rng);
    
    // Apply neural excitability changes
    applyExcitabilityEffects(brain);
    
    // Apply attention effects
    applyAttentionEffects(brain);
    
    // Apply memory consolidation effects
    applyMemoryConsolidationEffects(brain);
    
    // Apply neuromodulation effects
    applyNeuromodulationEffects(brain);
    
    // Apply plasticity rule effects
    applyPlasticityRuleEffects(brain);
    
    // Track developmental changes
    pImpl->totalSynapticChanges += std::abs(getNeuralExcitabilityModifier() - 1.0f);
}

void Development::updateDevelopmentalStage(Brain* brain, RandomGenerator& rng) {
    double timeInStage = age - stageStartAge;
    double stageDuration = calculateStageDuration(currentStage);
    
    if (timeInStage >= stageDuration) {
        // Advance to next stage
        advanceStage(brain);
    } else {
        // Update stage transition timer
        stageTransitionTimer = timeInStage / stageDuration;
        
        // Update critical period and sensitive period flags
        pImpl->inCriticalPeriod = (currentStage == DevelopmentalStage::CriticalPeriod);
        pImpl->inSensitivePeriod = (currentStage == DevelopmentalStage::CriticalPeriod) || 
                                  (currentStage == DevelopmentalStage::Maturation);
        pImpl->inMaturationWindow = (currentStage == DevelopmentalStage::Maturation);
    }
}

void Development::applyExcitabilityEffects(Brain* brain) {
    if (!brain) return;
    
    float excitabilityModifier = getNeuralExcitabilityModifier();
    
    // Calculate excitability based on age (Gaussian curve peaking around peak age)
    double ageDistance = std::abs(age - pImpl->peakExcitabilityAge);
    double ageGaussian = std::exp(-(ageDistance * ageDistance) / (2.0 * 100.0)); // sigma = 10
    
    float ageBasedExcitability = static_cast<float>(ageGaussian);
    
    // Combine with developmental stage modifier
    float finalExcitability = excitabilityModifier * ageBasedExcitability;
    
    // Apply excitability to neurons based on stage
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Apply excitability effects
                if (currentStage == DevelopmentalStage::CriticalPeriod) {
                    // Critical period: enhanced excitability and plasticity
                    neuron->setThreshold(neuron->getThreshold() * (1.0f - excitabilityModifier * 0.3f));
                    neuron->injectCurrent(0.2f * excitabilityModifier);
                } else if (currentStage == DevelopmentalStage::Maturation) {
                    // Maturation: optimized excitability
                    neuron->setThreshold(neuron->getThreshold() * (1.0f - excitabilityModifier * 0.15f));
                } else if (currentStage == DevelopmentalStage::Adult) {
                    // Adult: stable excitability with late-life decline
                    neuron->setThreshold(neuron->getThreshold() * (1.0f - excitabilityModifier * 0.1f));
                    if (age > pImpl->peakExcitabilityAge + 180.0) {
                        // Late-life decline
                        float lateLifeFactor = pImpl->lateExcitabilityFactor * 
                                             (1.0f - static_cast<float>((age - (pImpl->peakExcitabilityAge + 180.0)) / 360.0));
                        neuron->setThreshold(neuron->getThreshold() * lateLifeFactor);
                    }
                }
                
                // Apply age-based excitability
                if (ageBasedExcitability > 0.8f) {
                    // Peak excitability period
                    neuron->injectCurrent(finalExcitability * 0.1f);
                }
            }
        }
    }
}

void Development::applyAttentionEffects(Brain* brain) {
    if (!brain) return;
    
    float attentionCapacityModifier = getAttentionCapacityModifier();
    
    // Get attention system
    auto* attention = brain->getAttention();
    if (!attention) return;
    
    // Apply developmental attention capacity
    float baselineInhibition = attention->getInhibitionStrength();
    float baselineExcitation = attention->getExcitationStrength();
    
    if (currentStage == DevelopmentalStage::CriticalPeriod) {
        // Critical period: enhanced attentional flexibility and capacity
        float criticalPeriodBoost = std::min(2.0f, pImpl->criticalPeriodAttentionBoost * attentionCapacityModifier);
        attention->setInhibitionStrength(baselineInhibition * criticalPeriodBoost);
        attention->setExcitationStrength(baselineExcitation * criticalPeriodBoost);
        
        // Enhance attentional selection
        attention->setAttentionGain(attention->getAttentionGain() * attentionCapacityModifier);
        attention->setVigilanceGain(attention->getVigilanceGain() * attentionCapacityModifier);
    } else if (currentStage == DevelopmentalStage::Maturation) {
        // Maturation: optimized attentional control
        float maturationOptimization = pImpl->maturationAttentionOptimization * attentionCapacityModifier;
        attention->setInhibitionStrength(baselineInhibition * maturationOptimization);
        attention->setExcitationStrength(baselineExcitation * maturationOptimization);
    } else if (currentStage == DevelopmentalStage::Adult) {
        // Adult: stable attentional control with potential decline in late adulthood
        float adultFactor = 1.0f;
        if (age > pImpl->peakExcitabilityAge + 180.0) {
            adultFactor = 0.8f + 0.2f * static_cast<float>((pImpl->peakExcitabilityAge + 360.0 - age) / 180.0);
        }
        attention->setInhibitionStrength(baselineInhibition * adultFactor);
        attention->setExcitationStrength(baselineExcitation * adultFactor);
    }
}

void Development::applyMemoryConsolidationEffects(Brain* brain) {
    if (!brain) return;
    
    float consolidationStrength = getMemoryConsolidationStrength();
    
    // Get memory systems
    auto* workingMemory = brain->getWorkingMemory();
    auto* episodicMemory = brain->getEpisodicMemory();
    
    if (!workingMemory || !episodicMemory) return;
    
    if (currentStage == DevelopmentalStage::CriticalPeriod) {
        // Critical period: enhanced memory replay and consolidation
        float replayIntensity = pImpl->criticalPeriodReplayIntensity * consolidationStrength;
        
        // Enhance working memory capacity
        workingMemory->setCapacity(static_cast<size_t>(workingMemory->getCapacity() * replayIntensity));
        
        // Enhance episodic memory consolidation
        for (int i = 0; i < 3; ++i) { // Simulate enhanced replay
            auto episodesToReplay = episodicMemory->getEpisodesForReplay(1);
            for (const auto* episode : episodesToReplay) {
                episodicMemory->replayEpisode(episode);
                episodicMemory->consolidate(0.5f * consolidationStrength);
            }
        }
    } else if (currentStage == DevelopmentalStage::Maturation) {
        // Maturation: optimized memory consolidation
        float consolidationEfficiency = pImpl->adultConsolidationEfficiency * consolidationStrength;
        episodicMemory->consolidate(0.3f * consolidationEfficiency);
        
        // Enhance memory pruning for efficiency
        workingMemory->setCapacity(std::max(10, static_cast<int>(workingMemory->getCapacity() * 0.9f)));
    } else if (currentStage == DevelopmentalStage::Adult) {
        // Adult: stable memory consolidation with age-related changes
        float consolidationEfficiency = pImpl->adultConsolidationEfficiency * consolidationStrength;
        episodicMemory->consolidate(0.2f * consolidationEfficiency);
        
        // Enhance late-life memory consolidation
        if (age > pImpl->peakExcitabilityAge + 180.0) {
            episodicMemory->consolidate(0.3f * consolidationStrength);
        }
    }
}

void Development::applyNeuromodulationEffects(Brain* brain) {
    if (!brain) return;
    
    // Get neuromodulators
    auto* dopamine = brain->getDopamine();
    auto* acetylcholine = brain->getAcetylcholine();
    auto* norepinephrine = brain->getNorepinephrine();
    auto* serotonin = brain->getSerotonin();
    
    if (!dopamine || !acetylcholine || !norepinephrine || !serotonin) return;
    
    // Calculate age-based release modifiers
    float dopamineModifier = getNeuromodulatorReleaseModifier(NeuromodulatorType::Dopamine);
    float acetylcholineModifier = getNeuromodulatorReleaseModifier(NeuromodulatorType::Acetylcholine);
    float norepinephrineModifier = getNeuromodulatorReleaseModifier(NeuromodulatorType::Norepinephrine);
    float serotoninModifier = getNeuromodulatorReleaseModifier(NeuromodulatorType::Serotonin);
    
    // Apply developmental modulation to neuromodulator levels
    if (currentStage == DevelopmentalStage::CriticalPeriod) {
        // Critical period: enhanced neuromodulator release for learning
        dopamine->setLevel(dopamine->getLevel() * dopamineModifier * 1.5f);
        acetylcholine->setLevel(acetylcholine->getLevel() * acetylcholineModifier * 1.5f);
        norepinephrine->setLevel(norepinephrine->getLevel() * norepinephrineModifier * 1.5f);
        serotonin->setLevel(serotonin->getLevel() * serotoninModifier * 1.2f);
    } else if (currentStage == DevelopmentalStage::Maturation) {
        // Maturation: optimized neuromodulator balance
        dopamine->setLevel(dopamine->getLevel() * dopamineModifier * 1.2f);
        acetylcholine->setLevel(acetylcholine->getLevel() * acetylcholineModifier * 1.1f);
        norepinephrine->setLevel(norepinephrine->getLevel() * norepinephrineModifier * 1.0f);
        serotonin->setLevel(serotonin->getLevel() * serotoninModifier * 1.1f);
    } else if (currentStage == DevelopmentalStage::Adult) {
        // Adult: stable neuromodulator levels with age-related changes
        float adultAgeFactor = 1.0f;
        if (age > pImpl->peakExcitabilityAge + 180.0) {
            adultAgeFactor = 0.8f + 0.2f * static_cast<float>((pImpl->peakExcitabilityAge + 360.0 - age) / 180.0);
        }
        
        dopamine->setLevel(dopamine->getLevel() * dopamineModifier * adultAgeFactor);
        acetylcholine->setLevel(acetylcholine->getLevel() * acetylcholineModifier * adultAgeFactor);
        norepinephrine->setLevel(norepinephrine->getLevel() * norepinephrineModifier * adultAgeFactor);
        serotonin->setLevel(serotonin->getLevel() * serotoninModifier * adultAgeFactor);
    }
}

void Development::applyPlasticityRuleEffects(Brain* brain) {
    if (!brain) return;
    
    // Get plasticity systems
    auto* stdp = brain->getSTDP();
    auto* hebbian = brain->getHebbian();
    auto* structuralPlasticity = brain->getStructuralPlasticity();
    
    if (!stdp || !hebbian || !structuralPlasticity) return;
    
    // Calculate age-dependent plasticity rule modifications
    float stdpModifier = getPlasticityRuleModifier(PlasticityType::STDP);
    float hebbianModifier = getPlasticityRuleModifier(PlasticityType::Hebbian);
    float structuralModifier = getPlasticityRuleModifier(PlasticityType::Structural);
    
    if (currentStage == DevelopmentalStage::CriticalPeriod) {
        // Critical period: enhanced STDP for synaptic strengthening
        float stdpLTP = stdp->getLTPWeight() * stdpModifier * 1.5f;
        float stdpLTD = stdp->getLTDWeight() * stdpModifier * 1.5f;
        stdp->configure(stdpLTP, stdpLTD, stdp->getTau());
        
        // Enhanced Hebbian learning for synaptic formation
        hebbian->setLearningRate(hebbian->getLearningRate() * hebbianModifier * 1.5f);
        
        // Enhanced structural plasticity for synaptogenesis
        structuralPlasticity->setSynaptogenesisRate(structuralPlasticity->getSynaptogenesisRate() * structuralModifier * 2.0f);
        structuralPlasticity->setPruningRate(structuralPlasticity->getPruningRate() * structuralModifier * 0.5f);
        
    } else if (currentStage == DevelopmentalStage::Maturation) {
        // Maturation: optimized plasticity balance
        float stdpLTP = stdp->getLTPWeight() * stdpModifier * 1.2f;
        float stdpLTD = stdp->getLTDWeight() * stdpModifier * 1.2f;
        stdp->configure(stdpLTP, stdpLTD, stdp->getTau());
        
        hebbian->setLearningRate(hebbian->getLearningRate() * hebbianModifier * 1.2f);
        
        structuralPlasticity->setSynaptogenesisRate(structuralPlasticity->getSynaptogenesisRate() * structuralModifier * 1.1f);
        structuralPlasticity->setPruningRate(structuralPlasticity->getPruningRate() * structuralModifier * 1.1f);
        
    } else if (currentStage == DevelopmentalStage::Adult) {
        // Adult: stable plasticity with age-related adjustments
        float adultFactor = 1.0f;
        if (age > pImpl->peakExcitabilityAge + 180.0) {
            adultFactor = 0.8f + 0.2f * static_cast<float>((pImpl->peakExcitabilityAge + 360.0 - age) / 180.0);
        }
        
        float stdpLTP = stdp->getLTPWeight() * stdpModifier * adultFactor;
        float stdpLTD = stdp->getLTDWeight() * stdpModifier * adultFactor;
        stdp->configure(stdpLTP, stdpLTD, stdp->getTau());
        
        hebbian->setLearningRate(hebbian->getLearningRate() * hebbianModifier * adultFactor);
        
        structuralPlasticity->setSynaptogenesisRate(structuralPlasticity->getSynaptogenesisRate() * structuralModifier * adultFactor);
        structuralPlasticity->setPruningRate(structuralPlasticity->getPruningRate() * structuralModifier * adultFactor);
    }
}

void Development::advanceStage(Brain* brain) {
    // Transition to next developmental stage
    switch (currentStage) {
        case DevelopmentalStage::Initial:
            setStage(DevelopmentalStage::CriticalPeriod);
            break;
        case DevelopmentalStage::CriticalPeriod:
            setStage(DevelopmentalStage::Maturation);
            break;
        case DevelopmentalStage::Maturation:
            setStage(DevelopmentalStage::Adult);
            break;
        case DevelopmentalStage::Adult:
            setStage(DevelopmentalStage::Aging);
            break;
        case DevelopmentalStage::Aging:
            // Already at final stage
            break;
    }
    
    stageStartAge = age;
    stageTransitionTimer = 0.0;
    
    // Log stage transition
    NLM_LOG_INFO("Development advanced to stage: " + std::string(getStageName()));
    
    // Re-initialize stage parameters
    initializeStageParameters();
}

void Development::setStage(DevelopmentalStage stage) {
    currentStage = stage;
    stageStartAge = age;
    stageTransitionTimer = 0.0;
    
    // Update stage-specific flags
    pImpl->inCriticalPeriod = (currentStage == DevelopmentalStage::CriticalPeriod);
    pImpl->inSensitivePeriod = (currentStage == DevelopmentalStage::CriticalPeriod) || 
                              (currentStage == DevelopmentalStage::Maturation);
    pImpl->inMaturationWindow = (currentStage == DevelopmentalStage::Maturation);
}

void Development::initializeStageParameters() {
    // Initialize stage-specific parameters
    switch (currentStage) {
        case DevelopmentalStage::Initial:
            pImpl->baselineExcitability = 0.8f;
            pImpl->baselineAttentionCapacity = 0.5f;
            pImpl->baselineConsolidationStrength = 0.3f;
            break;
        case DevelopmentalStage::CriticalPeriod:
            pImpl->baselineExcitability = 1.5f;
            pImpl->baselineAttentionCapacity = 1.5f;
            pImpl->baselineConsolidationStrength = 2.0f;
            break;
        case DevelopmentalStage::Maturation:
            pImpl->baselineExcitability = 1.2f;
            pImpl->baselineAttentionCapacity = 1.2f;
            pImpl->baselineConsolidationStrength = 1.5f;
            break;
        case DevelopmentalStage::Adult:
            pImpl->baselineExcitability = 1.0f;
            pImpl->baselineAttentionCapacity = 1.0f;
            pImpl->baselineConsolidationStrength = 1.0f;
            break;
        case DevelopmentalStage::Aging:
            pImpl->baselineExcitability = 0.8f;
            pImpl->baselineAttentionCapacity = 0.8f;
            pImpl->baselineConsolidationStrength = 0.8f;
            break;
    }
}

double Development::calculateStageDuration(DevelopmentalStage stage) const {
    switch (stage) {
        case DevelopmentalStage::Initial: return pImpl->initialStageDuration;
        case DevelopmentalStage::CriticalPeriod: return pImpl->criticalPeriodDuration;
        case DevelopmentalStage::Maturation: return pImpl->maturationDuration;
        case DevelopmentalStage::Adult: return pImpl->adultStageDuration;
        case DevelopmentalStage::Aging: return pImpl->agingStageDuration;
        default: return 0.0;
    }
}

float Development::getNeuralExcitabilityModifier() const {
    if (currentStage == DevelopmentalStage::CriticalPeriod) return 1.5f;
    if (currentStage == DevelopmentalStage::Maturation) return 1.2f;
    if (currentStage == DevelopmentalStage::Adult) return 1.0f;
    if (currentStage == DevelopmentalStage::Aging) return 0.8f;
    return 1.0f;
}

float Development::getAttentionCapacityModifier() const {
    if (currentStage == DevelopmentalStage::CriticalPeriod) return 1.5f;
    if (currentStage == DevelopmentalStage::Maturation) return 1.2f;
    if (currentStage == DevelopmentalStage::Adult) return 1.0f;
    if (currentStage == DevelopmentalStage::Aging) return 0.8f;
    return 0.5f;
}

float Development::getMemoryConsolidationStrength() const {
    if (currentStage == DevelopmentalStage::CriticalPeriod) return 2.0f;
    if (currentStage == DevelopmentalStage::Maturation) return 1.5f;
    if (currentStage == DevelopmentalStage::Adult) return 1.0f;
    if (currentStage == DevelopmentalStage::Aging) return 0.8f;
    return 0.3f;
}

float Development::getNeuromodulatorReleaseModifier(NeuromodulatorType type) const {
    float baseLevel = 1.0f;
    
    if (currentStage == DevelopmentalStage::CriticalPeriod) {
        if (type == NeuromodulatorType::Dopamine || type == NeuromodulatorType::Acetylcholine) {
            baseLevel = 1.5f;
        }
    } else if (currentStage == DevelopmentalStage::Maturation) {
        if (type == NeuromodulatorType::Acetylcholine) {
            baseLevel = 1.2f;
        }
    } else if (currentStage == DevelopmentalStage::Adult) {
        baseLevel = 1.0f;
    } else if (currentStage == DevelopmentalStage::Aging) {
        baseLevel = 0.8f;
    }
    
    return baseLevel;
}

float Development::getPlasticityRuleModifier(PlasticityType type) const {
    float baseLevel = 1.0f;
    
    if (currentStage == DevelopmentalStage::CriticalPeriod) {
        if (type == PlasticityType::STDP) baseLevel = 1.5f;
    } else if (currentStage == DevelopmentalStage::Maturation) {
        baseLevel = 1.2f;
    } else if (currentStage == DevelopmentalStage::Adult) {
        baseLevel = 1.0f;
    } else if (currentStage == DevelopmentalStage::Aging) {
        baseLevel = 0.8f;
    }
    
    return baseLevel;
}

float Development::getCriticalPeriodProgress() const {
    if (!isCriticalPeriod()) return 0.0f;
    
    double stageDuration = calculateStageDuration(currentStage);
    if (stageDuration <= 0.0) return 0.0f;
    
    return static_cast<float>(stageTransitionTimer / stageDuration);
}

bool Development::isCriticalPeriod() const {
    return currentStage == DevelopmentalStage::CriticalPeriod;
}

bool Development::isSensitivePeriod() const {
    return currentStage == DevelopmentalStage::CriticalPeriod || 
           currentStage == DevelopmentalStage::Maturation;
}

float Development::getLearningRate() const {
    if (currentStage == DevelopmentalStage::CriticalPeriod) return pImpl->learningRateCriticalPeriod;
    if (currentStage == DevelopmentalStage::Maturation) return pImpl->learningRateMaturation;
    if (currentStage == DevelopmentalStage::Adult) return pImpl->learningRateAdult;
    return pImpl->learningRateBaseline;
}

float Development::getMemoryRetentionFactor() const {
    if (currentStage == DevelopmentalStage::CriticalPeriod) return pImpl->memoryRetentionCriticalPeriod;
    if (currentStage == DevelopmentalStage::Maturation) return pImpl->memoryRetentionMaturation;
    if (currentStage == DevelopmentalStage::Adult) return pImpl->memoryRetentionAdult;
    return pImpl->memoryRetentionBaseline;
}

float Development::getExplorationBias() const {
    if (currentStage == DevelopmentalStage::CriticalPeriod) return pImpl->explorationBiasCriticalPeriod;
    if (currentStage == DevelopmentalStage::Maturation) return pImpl->explorationBiasMaturation;
    if (currentStage == DevelopmentalStage::Adult) return pImpl->explorationBiasAdult;
    return pImpl->explorationBiasBaseline;
}

float Development::getStageTransitionProgress() const {
    return static_cast<float>(stageTransitionTimer);
}

const char* Development::getStageName() const {
    switch (currentStage) {
        case DevelopmentalStage::Initial: return "Initial";
        case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
        case DevelopmentalStage::Maturation: return "Maturation";
        case DevelopmentalStage::Adult: return "Adult";
        case DevelopmentalStage::Aging: return "Aging";
        default: return "Unknown";
    }
}

Development::DevelopmentalMetrics Development::getMetrics() const {
    DevelopmentalMetrics metrics;
    
    metrics.age = age;
    metrics.stage = currentStage;
    metrics.neuralExcitability = getNeuralExcitabilityModifier();
    metrics.attentionCapacity = getAttentionCapacityModifier();
    metrics.memoryConsolidation = getMemoryConsolidationStrength();
    metrics.dopamineRelease = getNeuromodulatorReleaseModifier(NeuromodulatorType::Dopamine);
    metrics.acetylcholineRelease = getNeuromodulatorReleaseModifier(NeuromodulatorType::Acetylcholine);
    metrics.norepinephrineRelease = getNeuromodulatorReleaseModifier(NeuromodulatorType::Norepinephrine);
    metrics.serotoninRelease = getNeuromodulatorReleaseModifier(NeuromodulatorType::Serotonin);
    metrics.plasticityStrength = getPlasticityRuleModifier(PlasticityType::STDP);
    metrics.learningRate = getLearningRate();
    metrics.explorationBias = getExplorationBias();
    metrics.memoryRetention = getMemoryRetentionFactor();
    
    return metrics;
}

} // namespace nlm
