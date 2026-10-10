#include "DevelopmentSystem.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/SpatialRepresentation.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct DevelopmentalStageInfo {
    float excitabilityMultiplier;      // Effect on neural excitability
    float plasticityMultiplier;        // Effect on plasticity rates
    float attentionMultiplier;         // Effect on attentional systems
    float memoryConsolidationMultiplier; // Effect on memory consolidation
    float neuromodulationMultiplier;    // Effect on neuromodulator baseline levels
    float criticalPeriodFactor;         // Special factor for critical period
    std::string description;
};

struct DevelopmentSystem::Impl {
    DevelopmentalStage stage;
    SimulationStep stageStartStep;
    SimulationStep stepsInCurrentStage;
    double stageAge;  // Age since entering current stage
    double globalDevelopmentalAge;  // Total age since birth
    
    // Enhanced developmental properties
    float neuralExcitability;          // Current neural excitability level (0-1)
    float synapticMaturation;           // Level of synaptic maturation (0-1)
    float myelinDevelopment;           // Level of myelin development (0-1)
    float connectivityDepth;            // Level of long-range connectivity (0-1)
    float cognitiveCapacity;            // Level of cognitive capabilities (0-1)
    
    // Developmental trajectories
    double excitabilityTrajectory;      // Rate of excitability change
    double plasticityTrajectory;        // Rate of plasticity change
    double attentionTrajectory;         // Rate of attentional system development
    double memoryTrajectory;            // Rate of memory system development
    
    // Critical period tracking
    float criticalPeriodIntensity;       // How active the current critical period is
    float criticalPeriodProgress;       // Progress through critical period (0-1)
    std::vector<SimulationStep> criticalPeriodEvents;
    
    // Memory system maturity
    float workingMemoryMaturity;         // Maturity of working memory system
    float episodicMemoryMaturity;        // Maturity of episodic memory system
    float associativeMemoryMaturity;     // Maturity of associative memory system
    
    // Attention system maturity
    float attentionalSelectionMaturity; // Maturity of attentional selection
    float spatialRepresentationMaturity; // Maturity of spatial representation
    float temporalRelationMaturity;     // Maturity of temporal relation system
    
    // Neuromodulator baseline levels by development
    float baselineDopamine;             // Developmental baseline for dopamine
    float baselineSerotonin;            // Developmental baseline for serotonin
    float baselineNorepinephrine;        // Developmental baseline for norepinephrine
    float baselineAcetylcholine;         // Developmental baseline for acetylcholine
    
    Impl() : stage(DevelopmentalStage::Initial), stageStartStep(0), stepsInCurrentStage(0), stageAge(0.0), 
             globalDevelopmentalAge(0.0), neuralExcitability(0.3f), synapticMaturation(0.0f), 
             myelinDevelopment(0.0f), connectivityDepth(0.0f), cognitiveCapacity(0.0f),
             excitabilityTrajectory(0.02f), plasticityTrajectory(0.01f), attentionTrajectory(0.015f),
             memoryTrajectory(0.01f), criticalPeriodIntensity(0.0f), criticalPeriodProgress(0.0f),
             workingMemoryMaturity(0.0f), episodicMemoryMaturity(0.0f), associativeMemoryMaturity(0.0f),
             attentionalSelectionMaturity(0.0f), spatialRepresentationMaturity(0.0f), 
             temporalRelationMaturity(0.0f), baselineDopamine(0.2f), baselineSerotonin(0.1f),
             baselineNorepinephrine(0.15f), baselineAcetylcholine(0.25f) {}
};

DevelopmentSystem::DevelopmentSystem() : pImpl(new Impl), age_(0.0) {}

DevelopmentSystem::~DevelopmentSystem() = default;

DevelopmentalStage DevelopmentSystem::getStage() const {
    return pImpl->stage;
}

void DevelopmentSystem::setStage(DevelopmentalStage stage) {
    pImpl->stage = stage;
    pImpl->stageAge = 0.0;
    pImpl->criticalPeriodProgress = 0.0f;
    
    // Reset critical period events when entering/exiting critical periods
    if (stage == DevelopmentalStage::CriticalPeriod) {
        pImpl->criticalPeriodIntensity = 1.0f;
        pImpl->criticalPeriodEvents.clear();
        NLM_LOG_INFO("Entering critical period - high plasticity and rapid learning");
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod && 
               stage != DevelopmentalStage::CriticalPeriod) {
        pImpl->criticalPeriodIntensity = 0.0f;
        NLM_LOG_INFO("Exiting critical period - plasticity decreasing");
    }
}

void DevelopmentSystem::advanceStage() {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            pImpl->stage = DevelopmentalStage::CriticalPeriod;
            break;
        case DevelopmentalStage::CriticalPeriod:
            pImpl->stage = DevelopmentalStage::Social;
            break;
        case DevelopmentalStage::Social:
            pImpl->stage = DevelopmentalStage::Maturation;
            break;
        case DevelopmentalStage::Maturation:
            pImpl->stage = DevelopmentalStage::Adult;
            break;
        case DevelopmentalStage::Adult:
            pImpl->stage = DevelopmentalStage::Cognitive;
            break;
        case DevelopmentalStage::Cognitive:
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
        case DevelopmentalStage::Social: return "Social";
        case DevelopmentalStage::Maturation: return "Maturation";
        case DevelopmentalStage::Adult: return "Adult";
        case DevelopmentalStage::Cognitive: return "Cognitive";
        case DevelopmentalStage::Aging: return "Aging";
        default: return "Unknown";
    }
}

void DevelopmentSystem::update(Brain* brain, SimulationStep currentStep) {
    ++pImpl->stepsInCurrentStage;
    age_ += 0.001;  // Approximate timestep
    pImpl->stageAge += 0.001;
    pImpl->globalDevelopmentalAge += 0.001;
    
    // Update developmental state based on current stage
    updateDevelopmentalState(brain);
    
    // Update critical period progress
    if (pImpl->stage == DevelopmentalStage::CriticalPeriod) {
        pImpl->criticalPeriodProgress = std::min(1.0f, 
                                               static_cast<float>(pImpl->stageAge / 300.0));
    }
    
    // Auto-advance stage based on time in stage
    // Initial: 60 steps, CriticalPeriod: 300 steps, Social: 180 steps, Maturation: 600 steps
    // Cognitive: 900 steps, Aging continues
    if (pImpl->stage == DevelopmentalStage::Initial && pImpl->stageAge > 60.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod && pImpl->stageAge > 300.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::Social && pImpl->stageAge > 180.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::Maturation && pImpl->stageAge > 600.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::Adult && pImpl->stageAge > 900.0) {
        advanceStage();
    }
}

void DevelopmentSystem::update(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    age_ += dt;
    pImpl->stageAge += dt;
    pImpl->globalDevelopmentalAge += dt;
    ++pImpl->stepsInCurrentStage;
    
    // Update developmental state with time-based dynamics
    updateDevelopmentalState(brain);
    
    // Update critical period progress
    if (pImpl->stage == DevelopmentalStage::CriticalPeriod) {
        pImpl->criticalPeriodProgress = std::min(1.0f, 
                                               static_cast<float>(pImpl->stageAge / 300.0));
    }
    
    // Auto-advance stage based on developmental age
    // These thresholds are in simulation seconds
    if (pImpl->stage == DevelopmentalStage::Initial && age_ > 60.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod && age_ > 360.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::Social && age_ > 540.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::Maturation && age_ > 960.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::Adult && age_ > 1800.0) {
        advanceStage();
    }
    
    // Occasionally add critical period events for recording
    if (pImpl->stage == DevelopmentalStage::CriticalPeriod && 
        rng.randFloat() < 0.01f) {
        pImpl->criticalPeriodEvents.push_back(currentStep);
    }
}

void DevelopmentSystem::updateDevelopmentalState(Brain* brain) {
    // Get developmental stage characteristics
    DevelopmentalStageInfo stageInfo = getStageCharacteristics(pImpl->stage, pImpl->globalDevelopmentalAge);
    
    // Update neural excitability
    pImpl->excitabilityTrajectory += (stageInfo.excitabilityMultiplier - pImpl->excitabilityTrajectory) * 0.1f;
    pImpl->neuralExcitability = std::clamp(pImpl->neuralExcitability + pImpl->excitabilityTrajectory * 0.001f, 0.0f, 1.0f);
    
    // Update plasticity rates
    float plasticityModifier = getPlasticityModifier();
    float targetPlasticity = stageInfo.plasticityMultiplier * (pImpl->criticalPeriodFactor * 1.5f + 0.5f);
    float currentPlasticity = plasticityModifier;
    if (brain && brain->getSTDP()) {
        brain->getSTDP()->setLTPWeight(currentPlasticity * 0.01f);
        brain->getSTDP()->setLTDWeight(currentPlasticity * 0.012f);
    }
    
    // Update attention system maturity
    if (brain && brain->getAttention()) {
        pImpl->attentionTrajectory += (stageInfo.attentionMultiplier - pImpl->attentionTrajectory) * 0.05f;
        pImpl->attentionalSelectionMaturity = std::min(1.0f, 
                                                     pImpl->attentionalSelectionMaturity + pImpl->attentionTrajectory * 0.005f);
    }
    
    // Update memory system maturity
    if (brain) {
        if (brain->getWorkingMemory()) {
            pImpl->memoryTrajectory += (stageInfo.memoryConsolidationMultiplier - pImpl->memoryTrajectory) * 0.05f;
            pImpl->workingMemoryMaturity = std::min(1.0f, 
                                                  pImpl->workingMemoryMaturity + pImpl->memoryTrajectory * 0.003f);
        }
        if (brain->getEpisodicMemory()) {
            pImpl->episodicMemoryMaturity = std::min(1.0f, 
                                                   pImpl->episodicMemoryMaturity + pImpl->memoryTrajectory * 0.004f);
        }
        if (brain->getAssociativeMemory()) {
            pImpl->associativeMemoryMaturity = std::min(1.0f, 
                                                      pImpl->associativeMemoryMaturity + pImpl->memoryTrajectory * 0.003f);
        }
    }
    
    // Update neuromodulator baseline levels
    updateNeuromodulatorBaselines();
    
    // Apply developmental effects on brain systems
    applyDevelopmentalEffects(brain);
}

DevelopmentalStageInfo DevelopmentSystem::getStageCharacteristics(DevelopmentalStage stage, double age) {
    DevelopmentalStageInfo info;
    
    // Default values for unknown stages
    info.excitabilityMultiplier = 1.0f;
    info.plasticityMultiplier = 1.0f;
    info.attentionMultiplier = 1.0f;
    info.memoryConsolidationMultiplier = 1.0f;
    info.neuromodulationMultiplier = 1.0f;
    info.criticalPeriodFactor = 1.0f;
    info.description = "Default";
    
    switch (stage) {
        case DevelopmentalStage::Initial:
            info.excitabilityMultiplier = 0.8f;
            info.plasticityMultiplier = 1.5f;
            info.attentionMultiplier = 0.7f;
            info.memoryConsolidationMultiplier = 0.5f;
            info.neuromodulationMultiplier = 0.8f;
            info.criticalPeriodFactor = 0.0f;
            info.description = "Early development - high plasticity, low attention";
            break;
            
        case DevelopmentalStage::CriticalPeriod:
            info.excitabilityMultiplier = 1.2f;
            info.plasticityMultiplier = 2.0f;
            info.attentionMultiplier = 0.8f;
            info.memoryConsolidationMultiplier = 0.8f;
            info.neuromodulationMultiplier = 1.2f;
            info.criticalPeriodFactor = 1.0f;
            info.description = "Critical period - high plasticity, rapid learning";
            break;
            
        case DevelopmentalStage::Social:
            info.excitabilityMultiplier = 1.0f;
            info.plasticityMultiplier = 1.2f;
            info.attentionMultiplier = 1.2f;
            info.memoryConsolidationMultiplier = 1.0f;
            info.neuromodulationMultiplier = 1.0f;
            info.criticalPeriodFactor = 0.3f;
            info.description = "Social development - attention to social stimuli";
            break;
            
        case DevelopmentalStage::Maturation:
            info.excitabilityMultiplier = 0.9f;
            info.plasticityMultiplier = 1.0f;
            info.attentionMultiplier = 1.5f;
            info.memoryConsolidationMultiplier = 1.2f;
            info.neuromodulationMultiplier = 1.0f;
            info.criticalPeriodFactor = 0.1f;
            info.description = "Maturation - refining cognitive functions";
            break;
            
        case DevelopmentalStage::Adult:
            info.excitabilityMultiplier = 1.0f;
            info.plasticityMultiplier = 0.8f;
            info.attentionMultiplier = 1.3f;
            info.memoryConsolidationMultiplier = 1.5f;
            info.neuromodulationMultiplier = 1.0f;
            info.criticalPeriodFactor = 0.0f;
            info.description = "Adulthood - stable neural function";
            break;
            
        case DevelopmentalStage::Cognitive:
            info.excitabilityMultiplier = 1.1f;
            info.plasticityMultiplier = 0.7f;
            info.attentionMultiplier = 1.8f;
            info.memoryConsolidationMultiplier = 1.8f;
            info.neuromodulationMultiplier = 1.2f;
            info.criticalPeriodFactor = 0.0f;
            info.description = "Cognitive development - advanced functions";
            break;
            
        case DevelopmentalStage::Aging:
            info.excitabilityMultiplier = 0.7f;
            info.plasticityMultiplier = 0.6f;
            info.attentionMultiplier = 0.8f;
            info.memoryConsolidationMultiplier = 0.7f;
            info.neuromodulationMultiplier = 0.9f;
            info.criticalPeriodFactor = 0.0f;
            info.description = "Aging - declining function";
            break;
    }
    
    return info;
}

void DevelopmentSystem::updateNeuromodulatorBaselines() {
    // Adjust neuromodulator baselines based on developmental age
    pImpl->baselineDopamine = std::max(0.1f, 0.5f - pImpl->globalDevelopmentalAge * 0.0001f);
    pImpl->baselineSerotonin = std::max(0.05f, 0.2f - pImpl->globalDevelopmentalAge * 0.00008f);
    pImpl->baselineNorepinephrine = std::max(0.05f, 0.3f - pImpl->globalDevelopmentalAge * 0.00007f);
    pImpl->baselineAcetylcholine = std::max(0.1f, 0.4f - pImpl->globalDevelopmentalAge * 0.00009f);
}

void DevelopmentSystem::applyDevelopmentalEffects(Brain* brain) {
    if (!brain) return;
    
    DevelopmentalStageInfo stageInfo = getStageCharacteristics(pImpl->stage, pImpl->globalDevelopmentalAge);
    
    // Apply developmental effects on neural regions
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Adjust neuronal properties based on developmental stage
                if (pImpl->stage == DevelopmentalStage::CriticalPeriod) {
                    // During critical period, enhance synaptic strength for frequently used connections
                    float activityLevel = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
                    if (activityLevel > 5.0f) {
                        // Strengthen frequently active synapses
                        neuron->injectCurrent(0.01f * pImpl->criticalPeriodProgress);
                    }
                } else if (pImpl->stage == DevelopmentalStage::Social) {
                    // Social stage: enhance responses to social-relevant cues
                    neuron->injectCurrent(0.005f * stageInfo.attentionMultiplier);
                } else if (pImpl->stage == DevelopmentalStage::Cognitive) {
                    // Cognitive stage: enhance complex processing
                    neuron->injectCurrent(0.002f * stageInfo.attentionMultiplier);
                } else if (pImpl->stage == DevelopmentalStage::Aging) {
                    // Aging: reduce overall excitability
                    neuron->injectCurrent(-0.01f * (1.0f - stageInfo.excitabilityMultiplier));
                }
            }
        }
    }
    
    // Apply developmental effects on cognitive systems
    if (brain->getAttention()) {
        // Attention system maturity affects its effectiveness
        float attentionMaturation = pImpl->attentionalSelectionMaturity;
        brain->getAttention()->setInhibitionStrength(0.5f * attentionMaturation);
        brain->getAttention()->setExcitationStrength(1.5f * attentionMaturation);
    }
    
    if (brain->getPlanner()) {
        // Planner maturity affects planning depth
        float planningDepth = 5 * pImpl->cognitiveCapacity;
        brain->getPlanner()->setPlanningDepth(static_cast<size_t>(planningDepth));
    }
    
    // Apply developmental effects on memory systems
    if (brain->getWorkingMemory() && pImpl->workingMemoryMaturity > 0.0f) {
        // Working memory capacity increases with maturity
        brain->getWorkingMemory()->setCapacity(brain->getTotalNeuronCount() / 10 * pImpl->workingMemoryMaturity);
    }
}

float DevelopmentSystem::getPlasticityModifier() const {
    // Higher plasticity in early stages, lower in later stages
    // Enhanced version that also considers critical period
    float basePlasticity = 0.5f;
    float stageFactor = 1.0f;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: basePlasticity = 1.0f; stageFactor = 1.2f; break;
        case DevelopmentalStage::CriticalPeriod: basePlasticity = 0.8f; stageFactor = 2.0f; break;
        case DevelopmentalStage::Social: basePlasticity = 0.6f; stageFactor = 1.3f; break;
        case DevelopmentalStage::Maturation: basePlasticity = 0.5f; stageFactor = 1.0f; break;
        case DevelopmentalStage::Adult: basePlasticity = 0.3f; stageFactor = 0.9f; break;
        case DevelopmentalStage::Cognitive: basePlasticity = 0.2f; stageFactor = 0.8f; break;
        case DevelopmentalStage::Aging: basePlasticity = 0.1f; stageFactor = 0.6f; break;
        default: basePlasticity = 0.5f; stageFactor = 1.0f; break;
    }
    
    // Apply critical period enhancement
    float criticalPeriodEnhancement = 1.0f + (pImpl->stage == DevelopmentalStage::CriticalPeriod ? 1.0f : 0.0f);
    
    return basePlasticity * stageFactor * criticalPeriodEnhancement;
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
