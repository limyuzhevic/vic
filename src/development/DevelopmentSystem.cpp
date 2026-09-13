#include "DevelopmentSystem.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct DevelopmentSystem::Impl {
    DevelopmentalStage stage;
    SimulationStep stageStartStep;
    SimulationStep stepsInCurrentStage;
    double stageAge;  // Age since entering current stage;
    
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

void DevelopmentSystem::updateComprehensive(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    if (!brain) return;
    
    // First, call the original update method for stage management
    update(brain, rng, dt);
    
    // Apply all developmental effects
    affectNeuralExcitability(brain);
    affectAttentionSystem(brain);
    affectMemoryConsolidation(brain);
    affectPredictionSystem(brain);
    affectWorkingMemory(brain);
    affectNeuromodulation(brain);
    
    // Log developmental progress
    NLM_LOG_INFO("DevelopmentSystem comprehensive update - Stage: " + std::string(getStageName()) +
                ", Age: " + std::to_string(age_) + ", StageAge: " + std::to_string(pImpl->stageAge));
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

void DevelopmentSystem::affectNeuralExcitability(Brain* brain) {
    if (!brain) return;
    
    float excitabilityFactor = 0.0f;
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            excitabilityFactor = 1.0f;  // Maximum excitability in early development
            break;
        case DevelopmentalStage::CriticalPeriod:
            excitabilityFactor = 0.7f;  // High but decreasing excitability
            break;
        case DevelopmentalStage::Maturation:
            excitabilityFactor = 0.4f;  // Moderate excitability
            break;
        case DevelopmentalStage::Adult:
            excitabilityFactor = 0.2f;  // Low excitability in adulthood
            break;
        case DevelopmentalStage::Aging:
            excitabilityFactor = 0.1f;  // Minimal excitability in aging
            break;
    }
    
    // Modify neuron thresholds based on developmental stage
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                float currentThreshold = neuron->getThreshold();
                // Lower threshold = higher excitability
                float targetThreshold = -65.0f - excitabilityFactor * 40.0f;  // Range from -65 to -105
                
                // Gradually adapt threshold (not instant change)
                float adaptRate = 0.01f;
                if (targetThreshold < currentThreshold) {
                    neuron->setThreshold(std::max(targetThreshold, currentThreshold - adaptRate * 10.0f));
                }
            }
        }
    }
}

void DevelopmentSystem::affectAttentionSystem(Brain* brain) {
    if (!brain) return;
    
    float attentionFactor = 0.0f;
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            attentionFactor = 0.3f;  // Low selectivity, broad focus
            break;
        case DevelopmentalStage::CriticalPeriod:
            attentionFactor = 0.6f;  // Developing selectivity
            break;
        case DevelopmentalStage::Maturation:
            attentionFactor = 0.8f;  // High selectivity
            break;
        case DevelopmentalStage::Adult:
            attentionFactor = 0.9f;  // Fine-tuned selectivity
            break;
        case DevelopmentalStage::Aging:
            attentionFactor = 0.7f;  // Declining selectivity
            break;
    }
    
    auto* attention = brain->getAttention();
    if (attention) {
        // Adjust competition threshold
        float targetThreshold = 0.4f - attentionFactor * 0.3f;  // Range 0.1 to 0.4
        attention->setCompetitionThreshold(targetThreshold);
        
        // Adjust inhibition strength
        float inhibitionTarget = 0.3f + attentionFactor * 0.4f;  // Range 0.3 to 0.7
        attention->setInhibitionStrength(inhibitionTarget);
        
        // Adjust excitation strength
        float excitationTarget = 0.5f - attentionFactor * 0.2f;  // Range 0.3 to 0.5
        attention->setExcitationStrength(excitationTarget);
    }
}

void DevelopmentSystem::affectMemoryConsolidation(Brain* brain) {
    if (!brain) return;
    
    float memoryFactor = 0.0f;
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            memoryFactor = 0.3f;  // Low consolidation efficiency
            break;
        case DevelopmentalStage::CriticalPeriod:
            memoryFactor = 0.7f;  // High consolidation efficiency
            break;
        case DevelopmentalStage::Maturation:
            memoryFactor = 0.9f;  // Near-optimal consolidation
            break;
        case DevelopmentalStage::Adult:
            memoryFactor = 0.8f;  // Stable consolidation
            break;
        case DevelopmentalStage::Aging:
            memoryFactor = 0.5f;  // Declining consolidation
            break;
    }
    
    // Affect working memory
    auto* workingMemory = brain->getWorkingMemory();
    if (workingMemory) {
        // Adjust decay rate
        float decayTarget = 0.05f - memoryFactor * 0.03f;  // Range 0.02 to 0.05
        workingMemory->setDecayRate(std::max(0.01f, decayTarget));
        
        // Adjust capacity
        float capacityTarget = 50.0f * memoryFactor;  // Scale with memory efficiency
        workingMemory->setCapacity(static_cast<size_t>(capacityTarget));
        
        // Strengthen existing memory traces
        workingMemory->strengthenMemory(1.0f + memoryFactor * 0.5f);
    }
    
    // Affect episodic memory consolidation
    auto* episodicMemory = brain->getEpisodicMemory();
    if (episodicMemory) {
        // Set consolidation threshold based on memory factor
        float consolidationThreshold = 0.3f - memoryFactor * 0.2f;  // Range 0.1 to 0.3
        episodicMemory->consolidate(std::max(0.1f, consolidationThreshold));
    }
}

void DevelopmentSystem::affectPredictionSystem(Brain* brain) {
    if (!brain) return;
    
    float predictionFactor = 0.0f;
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            predictionFactor = 0.2f;  // Low prediction accuracy
            break;
        case DevelopmentalStage::CriticalPeriod:
            predictionFactor = 0.6f;  // Rapid learning of predictions
            break;
        case DevelopmentalStage::Maturation:
            predictionFactor = 0.8f;  // High prediction accuracy
            break;
        case DevelopmentalStage::Adult:
            predictionFactor = 0.9f;  // Stable prediction performance
            break;
        case DevelopmentalStage::Aging:
            predictionFactor = 0.6f;  // Declining prediction performance
            break;
    }
    
    auto* predictionSystem = brain->getPredictionSystem();
    if (predictionSystem) {
        // This would modify prediction system internal state if accessible
        // For now, we log the developmental effect
        NLM_LOG_INFO("Development affecting prediction system with factor: " + 
                    std::to_string(predictionFactor));
    }
    
    auto* neuralPrediction = static_cast<NeuralPrediction*>(predictionSystem);
    if (neuralPrediction) {
        // Adjust prediction confidence based on developmental stage
        // This would require access to NeuralPrediction internals
    }
}

void DevelopmentSystem::affectWorkingMemory(Brain* brain) {
    if (!brain) return;
    
    float workingMemoryFactor = 0.0f;
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            workingMemoryFactor = 0.3f;  // Limited working memory capacity
            break;
        case DevelopmentalStage::CriticalPeriod:
            workingMemoryFactor = 0.7f;  // Expanding working memory
            break;
        case DevelopmentalStage::Maturation:
            workingMemoryFactor = 0.9f;  // Near-optimal working memory
            break;
        case DevelopmentalStage::Adult:
            workingMemoryFactor = 0.8f;  // Stable working memory
            break;
        case DevelopmentalStage::Aging:
            workingMemoryFactor = 0.5f;  // Declining working memory
            break;
    }
    
    auto* workingMemory = brain->getWorkingMemory();
    if (workingMemory) {
        // These effects are already handled in affectMemoryConsolidation
        // Additional working memory-specific effects could go here
        NLM_LOG_INFO("Development affecting working memory with factor: " + 
                    std::to_string(workingMemoryFactor));
    }
}

void DevelopmentSystem::affectNeuromodulation(Brain* brain) {
    if (!brain) return;
    
    float neuromodulationFactor = 0.0f;
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            neuromodulationFactor = 0.5f;  // Baseline neuromodulation
            break;
        case DevelopmentalStage::CriticalPeriod:
            neuromodulationFactor = 0.8f;  // Enhanced neuromodulation
            break;
        case DevelopmentalStage::Maturation:
            neuromodulationFactor = 1.0f;  // Peak neuromodulation
            break;
        case DevelopmentalStage::Adult:
            neuromodulationFactor = 0.7f;  // Stable neuromodulation
            break;
        case DevelopmentalStage::Aging:
            neuromodulationFactor = 0.4f;  // Reduced neuromodulation
            break;
    }
    
    // Affect neuromodulators
    auto* acetylcholine = brain->getAcetylcholine();
    if (acetylcholine) {
        // ACh affects attention and memory consolidation
        float baselineACh = 0.5f + neuromodulationFactor * 0.5f;
        acetylcholine->setLevel(baselineACh);
    }
    
    auto* norepinephrine = brain->getNorepinephrine();
    if (norepinephrine) {
        // NE affects arousal and vigilance
        float baselineNE = 0.4f + neuromodulationFactor * 0.6f;
        norepinephrine->setLevel(baselineNE);
    }
    
    auto* serotonin = brain->getSerotonin();
    if (serotonin) {
        // 5-HT affects mood and impulsivity
        float baseline5HT = 0.5f - neuromodulationFactor * 0.1f;
        serotonin->setLevel(baseline5HT);
    }
    
    NLM_LOG_INFO("Development affecting neuromodulation with factor: " + 
                std::to_string(neuromodulationFactor));
}

} // namespace nlm
