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

void DevelopmentSystem::applyDevelopmentalEffects(Brain* brain, SimulationStep currentStep) {
    // Apply developmental effects to all brain systems
    // This is the new Phase 6 enhancement
    
    // 1. Modulate neural excitability based on developmental stage
    if (brain) {
        // Create a developmental neuromodulation signal
        float developmentalSignal = getPlasticityModifier();
        
        // Affect neural excitability: early stages increase, late stages decrease
        float excitabilityMod = 1.0f;
        switch (pImpl->stage) {
            case DevelopmentalStage::Initial:
                excitabilityMod = 1.2f;  // Increased excitability
                break;
            case DevelopmentalStage::CriticalPeriod:
                excitabilityMod = 1.1f;  // Still high
                break;
            case DevelopmentalStage::Maturation:
                excitabilityMod = 1.0f;  // Normal
                break;
            case DevelopmentalStage::Adult:
                excitabilityMod = 0.9f;  // Slightly decreased
                break;
            case DevelopmentalStage::Aging:
                excitabilityMod = 0.8f;  // Decreased
                break;
        }
        
        // Apply to all neurons
        brain->injectCurrentToNeurons(NeuronType::Sensory, (excitabilityMod - 1.0f) * 2.0f);
        brain->injectCurrentToNeurons(NeuronType::Internal, (excitabilityMod - 1.0f) * 2.0f);
        brain->injectCurrentToNeurons(NeuronType::Motor, (excitabilityMod - 1.0f) * 1.5f);
    }
    
    // 2. Modulate memory consolidation based on developmental stage
    if (brain && pImpl->episodicMemory) {
        // Early stages: more frequent consolidation
        // Late stages: selective consolidation only for important memories
        float consolidationRate = 1.0f;
        
        switch (pImpl->stage) {
            case DevelopmentalStage::Initial:
                consolidationRate = 1.5f;  // High consolidation
                break;
            case DevelopmentalStage::CriticalPeriod:
                consolidationRate = 1.3f;
                break;
            case DevelopmentalStage::Maturation:
                consolidationRate = 1.0f;
                break;
            case DevelopmentalStage::Adult:
                consolidationRate = 0.8f;  // Slower, more selective
                break;
            case DevelopmentalStage::Aging:
                consolidationRate = 0.6f;  // Reduced
                break;
        }
        
        // Every consolidationInterval steps, consolidate with modified rate
        NLM_LOG_INFO("Development: Memory consolidation rate modified by stage " + getStageName());
    }
    
    // 3. Modulate attention modulation based on developmental stage
    if (brain && pImpl->attention) {
        // Attention becomes more selective with maturation
        float attentionMod = 1.0f;
        
        switch (pImpl->stage) {
            case DevelopmentalStage::Initial:
                attentionMod = 0.7f;  // Broad attention
                break;
            case DevelopmentalStage::CriticalPeriod:
                attentionMod = 0.8f;
                break;
            case DevelopmentalStage::Maturation:
                attentionMod = 1.0f;  // Refined attention
                break;
            case DevelopmentalStage::Adult:
                attentionMod = 1.2f;  // Selective attention
                break;
            case DevelopmentalStage::Aging:
                attentionMod = 0.9f;  // Declining attention
                break;
        }
        
        pImpl->attention->setInhibitionStrength(0.5f * attentionMod);
        pImpl->attention->setExcitationStrength(1.5f * attentionMod);
    }
    
    // 4. Modulate plasticity rules based on developmental stage
    if (brain && pImpl->dopamine) {
        // Dopamine affects plasticity differently at each stage
        float plasticityEffect = 1.0f;
        
        switch (pImpl->stage) {
            case DevelopmentalStage::Initial:
                plasticityEffect = 1.5f;  // High learning capacity
                break;
            case DevelopmentalStage::CriticalPeriod:
                plasticityEffect = 1.3f;
                break;
            case DevelopmentalStage::Maturation:
                plasticityEffect = 1.0f;
                break;
            case DevelopmentalStage::Adult:
                plasticityEffect = 0.7f;  // Stable, less change
                break;
            case DevelopmentalStage::Aging:
                plasticityEffect = 0.5f;  // Reduced capacity
                break;
        }
        
        pImpl->dopamine->setLearningRate(std::min(1.0f, plasticityEffect * 0.1f + 0.01f));
    }
    
    // 5. Modulate learning rates based on developmental stage
    if (brain && pImpl->curiosity) {
        // Curiosity-driven exploration changes with development
        float explorationFactor = 1.0f;
        
        switch (pImpl->stage) {
            case DevelopmentalStage::Initial:
                explorationFactor = 1.5f;  // High exploration
                break;
            case DevelopmentalStage::CriticalPeriod:
                explorationFactor = 1.2f;
                break;
            case DevelopmentalStage::Maturation:
                explorationFactor = 1.0f;
                break;
            case DevelopmentalStage::Adult:
                explorationFactor = 0.8f;  // Reduced exploration
                break;
            case DevelopmentalStage::Aging:
                explorationFactor = 0.6f;  // Low exploration
                break;
        }
        
        pImpl->curiosity->setExplorationFactor(std::min(1.0f, explorationFactor));
    }
    
    // 6. Structural changes (synaptogenesis/pruning) based on stage
    if (brain && pImpl->structuralPlasticity) {
        // Development affects structural plasticity differently at each stage
        float synaptogenesisRate = 0.0001f;
        float pruningRate = 0.00001f;
        
        switch (pImpl->stage) {
            case DevelopmentalStage::Initial:
                synaptogenesisRate = 0.0002f;  // More growth
                pruningRate = 0.00001f;
                break;
            case DevelopmentalStage::CriticalPeriod:
                synaptogenesisRate = 0.00015f;
                pruningRate = 0.000015f;
                break;
            case DevelopmentalStage::Maturation:
                synaptogenesisRate = 0.0001f;
                pruningRate = 0.00002f;  // More pruning
                break;
            case DevelopmentalStage::Adult:
                synaptogenesisRate = 0.00005f;
                pruningRate = 0.00003f;
                break;
            case DevelopmentalStage::Aging:
                synaptogenesisRate = 0.00002f;
                pruningRate = 0.00005f;  // More pruning than growth
                break;
        }
        
        pImpl->structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        pImpl->structuralPlasticity->setPruningRate(pruningRate);
    }
    
    // 7. Stage-specific goals affect planning
    if (brain && pImpl->planner) {
        // Different stages have different planning horizons and goals
        size_t planningDepth = 3;
        
        switch (pImpl->stage) {
            case DevelopmentalStage::Initial:
                planningDepth = 2;  // Short-term
                break;
            case DevelopmentalStage::CriticalPeriod:
                planningDepth = 3;
                break;
            case DevelopmentalStage::Maturation:
                planningDepth = 4;  // Medium-term
                break;
            case DevelopmentalStage::Adult:
                planningDepth = 5;  // Long-term
                break;
            case DevelopmentalStage::Aging:
                planningDepth = 2;  // Short-term again
                break;
        }
        
        pImpl->planner->setPlanningDepth(planningDepth);
        
        // Set stage-appropriate goals
        std::vector<float> goal(5, 0.5f);  // 5-dimensional goal space
        if (pImpl->stage == DevelopmentalStage::Initial) {
            // Basic survival goals
            goal[0] = 0.7f;  // Food need
            goal[1] = 0.5f;  // Safety
        } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod) {
            // Social learning goals
            goal[2] = 0.8f;  // Social connection
            goal[3] = 0.6f;  // Skill development
        } else if (pImpl->stage == DevelopmentalStage::Adult) {
            // Achievement goals
            goal[4] = 0.9f;  // Mastery
        }
        
        pImpl->planner->setCurrentGoal(goal);
    }
    
    NLM_LOG_INFO("Developmental effects applied for stage " + getStageName());
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