#include "DevelopmentSystem.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>

namespace nlm {

struct DevelopmentSystem::Impl {
    DevelopmentalStage stage;
    SimulationStep stageStartStep;
    SimulationStep stepsInCurrentStage;
    double stageAge;  // Age since entering current stage
    
    // Developmental effects on different systems
    float neuralExcitability;
    float plasticityRules;
    float attentionGain;
    float memoryConsolidation;
    float neuromodulationDopamine;
    float neuromodulationCuriosity;
    float conceptFormationThreshold;
    float predictionConfidence;
    
    Impl() : stage(DevelopmentalStage::Initial), stageStartStep(0), stepsInCurrentStage(0), stageAge(0.0)
        , neuralExcitability(1.0f), plasticityRules(1.0f), attentionGain(1.0f), memoryConsolidation(1.0f)
        , neuromodulationDopamine(1.0f), neuromodulationCuriosity(1.0f), conceptFormationThreshold(0.5f)
        , predictionConfidence(1.0f) {}
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
        default: return 1.0f;
    }
}

bool DevelopmentSystem::isCriticalPeriod() const {
    return pImpl->stage == DevelopmentalStage::CriticalPeriod;
}

float DevelopmentSystem::getCriticalPeriodProgress() const {
    if (!isCriticalPeriod()) return 1.0f;  // Not in critical period
    
    // Calculate progress through critical period
    float stageAgeSeconds = static_cast<float>(pImpl->stageAge);
    return std::min(1.0f, stageAgeSeconds / 300.0f);
}

// === ENHANCED DEVELOPMENT EFFECTS ===

void DevelopmentSystem::setNeuralExcitability(Brain* brain, float excitabilityMod) {
    pImpl->neuralExcitability = excitabilityMod;
    
    if (brain) {
        // Apply excitability modifier to all neurons
        for (auto& region : brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Scale leak conductance based on developmental excitability
                    float currentLeak = neuron->getLeakConductance();
                    neuron->setLeakConductance(currentLeak * pImpl->neuralExcitability);
                    
                    // Adjust threshold based on developmental excitability
                    float currentThreshold = neuron->getThreshold();
                    neuron->setThreshold(currentThreshold * pImpl->neuralExcitability);
                }
            }
        }
    }
}

void DevelopmentSystem::setPlasticityRules(Brain* brain, float stdpMod, float hebbianMod) {
    pImpl->plasticityRules = (stdpMod + hebbianMod) * 0.5f;
    
    if (brain) {
        // Apply plasticity modifier to both STDP and Hebbian systems
        if (auto* stdp = brain->getSTDP()) {
            float currentLTP = stdp->getLTPWeight();
            float currentLTD = stdp->getLTDWeight();
            stdp->setLTPWeight(currentLTP * pImpl->plasticityRules);
            stdp->setLTDWeight(currentLTD * pImpl->plasticityRules);
        }
        
        if (auto* hebbian = brain->getHebbian()) {
            // Hebbian has different modulation mechanism
            float hebbianMod = pImpl->plasticityRules * 0.8f;
            // Apply to Hebbian learning rates (need to check what methods are available)
        }
    }
}

void DevelopmentSystem::setAttention(Brain* brain, float attentionGain) {
    pImpl->attentionGain = attentionGain;
    
    if (brain && brain->getAttention()) {
        brain->getAttention()->setGain(pImpl->attentionGain);
    }
}

void DevelopmentSystem::setMemoryConsolidation(Brain* brain, float consolidationBoost) {
    pImpl->memoryConsolidation = consolidationBoost;
    
    if (brain && brain->getEpisodicMemory()) {
        brain->getEpisodicMemory()->setConsolidationBoost(pImpl->memoryConsolidation);
    }
}

void DevelopmentSystem::setNeuromodulation(Brain* brain, float dopamineMod, float curiosityMod) {
    pImpl->neuromodulationDopamine = dopamineMod;
    pImpl->neuromodulationCuriosity = curiosityMod;
    
    if (brain) {
        if (auto* dopamine = brain->getDopamine()) {
            float currentLevel = dopamine->getLevel();
            // Scale dopamine level based on developmental stage
            float modulatedLevel = currentLevel * pImpl->neuromodulationDopamine;
            dopamine->setLevel(modulatedLevel);
        }
        
        if (auto* curiosity = brain->getCuriosity()) {
            float currentLevel = curiosity->getLevel();
            float modulatedLevel = currentLevel * pImpl->neuromodulationCuriosity;
            curiosity->setLevel(modulatedLevel);
        }
    }
}

void DevelopmentSystem::setConceptFormation(Brain* brain, float conceptThreshold) {
    pImpl->conceptFormationThreshold = conceptThreshold;
    
    if (brain && brain->getConceptFormation()) {
        brain->getConceptFormation()->setFormationThreshold(pImpl->conceptFormationThreshold);
    }
}

void DevelopmentSystem::setPredictionSystem(Brain* brain, float predictionConfidence) {
    pImpl->predictionConfidence = predictionConfidence;
    
    if (brain && brain->getPredictionSystem()) {
        // PredictionSystem doesn't currently have setConfidence method
        // For now, just store the value for future use
    }
}

float DevelopmentSystem::getDevelopmentalEffect(DevelopmentalEffectType type) const {
    switch (type) {
        case DevelopmentalEffectType::NeuralExcitability: return pImpl->neuralExcitability;
        case DevelopmentalEffectType::PlasticityRules: return pImpl->plasticityRules;
        case DevelopmentalEffectType::Attention: return pImpl->attentionGain;
        case DevelopmentalEffectType::MemoryConsolidation: return pImpl->memoryConsolidation;
        case DevelopmentalEffectType::Neuromodulation: return (pImpl->neuromodulationDopamine + pImpl->neuromodulationCuriosity) * 0.5f;
        case DevelopmentalEffectType::ConceptFormation: return pImpl->conceptFormationThreshold;
        case DevelopmentalEffectType::PredictionSystem: return pImpl->predictionConfidence;
        case DevelopmentalEffectType::WorkingMemory: return pImpl->attentionGain;  // Placeholder
        case DevelopmentalEffectType::EpisodicMemory: return pImpl->memoryConsolidation;  // Placeholder
        default: return 1.0f;
    }
}

void DevelopmentSystem::setDevelopmentalEffect(DevelopmentalEffectType type, float strength) {
    switch (type) {
        case DevelopmentalEffectType::NeuralExcitability: pImpl->neuralExcitability = strength; break;
        case DevelopmentalEffectType::PlasticityRules: pImpl->plasticityRules = strength; break;
        case DevelopmentalEffectType::Attention: pImpl->attentionGain = strength; break;
        case DevelopmentalEffectType::MemoryConsolidation: pImpl->memoryConsolidation = strength; break;
        case DevelopmentalEffectType::Neuromodulation: 
            pImpl->neuromodulationDopamine = strength;
            pImpl->neuromodulationCuriosity = strength;
            break;
        case DevelopmentalEffectType::ConceptFormation: pImpl->conceptFormationThreshold = strength; break;
        case DevelopmentalEffectType::PredictionSystem: pImpl->predictionConfidence = strength; break;
        case DevelopmentalEffectType::WorkingMemory: /* placeholder */ break;
        case DevelopmentalEffectType::EpisodicMemory: /* placeholder */ break;
    }
}

void DevelopmentSystem::calculateNeuralDevelopment(Brain* brain, float& leakConductance, float& threshold) {
    if (!brain) return;
    
    // Calculate developmental influence on neural parameters
    float devStage = static_cast<float>(getDevelopmentalAge()) / 1000.0f;  // Normalize to [0, 1]
    
    // Neural excitability decreases with age (neurons become more stable)
    float ageFactor = std::max(0.1f, 1.0f - devStage * 0.9f);
    
    // Leak conductance decreases with age (less leak, more stable)
    leakConductance *= ageFactor;
    
    // Threshold increases with age (more stable, higher threshold)
    threshold *= ageFactor;
    
    // Apply developmental neuromodulation
    float neuromodMod = getDevelopmentalEffect(DevelopmentalEffectType::Neuromodulation);
    leakConductance *= neuromodMod;
    threshold *= neuromodMod;
}

} // namespace nlm
