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
    
    // Apply developmental stage effects to brain systems
    applyDevelopmentalEffects(brain, dt);
    
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
    if (!brain) return;
    
    // Get plasticity modifier for current developmental stage
    float plasticityModifier = getPlasticityModifier();
    
    // Apply to structural plasticity rates
    if (auto* structural = brain->getStructuralPlasticity()) {
        // Synaptogenesis decreases with development (more stable)
        float synaptogenesisRate = 0.0001f * plasticityModifier;
        structural->setSynaptogenesisRate(synaptogenesisRate);
        
        // Pruning may increase with development (pruning old connections)
        float pruningRate = 0.00001f * (2.0f - plasticityModifier);
        structural->setPruningRate(pruningRate);
    }
    
    // Apply developmental effects to neuromodulation
    applyNeuromodulationDevelopmentEffects(brain, dt, plasticityModifier);
    
    // Apply developmental effects to memory systems
    applyMemoryDevelopmentEffects(brain, dt, plasticityModifier);
    
    // Apply developmental effects to prediction system
    applyPredictionDevelopmentEffects(brain, dt, plasticityModifier);
    
    // Apply developmental effects to cognition systems
    applyCognitionDevelopmentEffects(brain, dt, plasticityModifier);
}

void DevelopmentSystem::applyNeuromodulationDevelopmentEffects(Brain* brain, TimestepDuration dt, float plasticityModifier) {
    if (!brain) return;
    
    // Dopamine effects: increases with development (reward learning matures)
    if (auto* dopamine = brain->getDopamine()) {
        float dopamineLevel = dopamine->getLevel();
        
        // Modulate dopamine based on developmental stage
        switch (getStage()) {
            case DevelopmentalStage::Initial:
                dopamineLevel *= 0.8f;  // Lower initial reward sensitivity
                break;
            case DevelopmentalStage::CriticalPeriod:
                dopamineLevel *= 1.0f;  // Normal reward processing
                break;
            case DevelopmentalStage::Maturation:
                dopamineLevel *= 1.2f;  // Increased reward learning
                break;
            case DevelopmentalStage::Adult:
                dopamineLevel *= 1.0f;  // Stable reward processing
                break;
            case DevelopmentalStage::Aging:
                dopamineLevel *= 0.8f;  // Reduced reward sensitivity
                break;
        }
        
        // Apply dopamine effects on neural excitability
        for (size_t i = 0; i < 10 && i < brain->getTotalNeuronCount(); ++i) {
            brain->injectCurrentToNeurons(NeuronType::Internal, dopamineLevel * 0.01f);
        }
    }
    
    // Novelty detection may decrease with development (less exploration needed)
    if (auto* novelty = brain->getNovelty()) {
        float noveltyLevel = novelty->getLevel();
        noveltyLevel *= (1.5f - plasticityModifier);  // Less novelty in mature brains
        novelty->setLevel(std::max(0.0f, noveltyLevel));
    }
}

void DevelopmentSystem::applyMemoryDevelopmentEffects(Brain* brain, TimestepDuration dt, float plasticityModifier) {
    if (!brain) return;
    
    // Working memory capacity may increase with development
    if (auto* workingMemory = brain->getWorkingMemory()) {
        // Mature brains can hold more active traces
        size_t baseCapacity = 100;
        size_t capacityIncrease = static_cast<size_t>(baseCapacity * (1.0f - plasticityModifier));
        workingMemory->setCapacity(baseCapacity + capacityIncrease);
    }
    
    // Episodic memory consolidation may increase with development
    if (auto* episodicMemory = brain->getEpisodicMemory()) {
        // Older brains consolidate better
        float consolidationStrength = 0.5f + (0.5f * (1.0f - plasticityModifier));
        // Would need a method to set consolidation strength if available
    }
    
    // Development affects attention to memory
    if (auto* attention = brain->getAttention()) {
        // Attention becomes more selective with development
        float inhibitionStrength = 0.5f + (0.3f * (1.0f - plasticityModifier));
        attention->setInhibitionStrength(inhibitionStrength);
    }
}

void DevelopmentSystem::applyPredictionDevelopmentEffects(Brain* brain, TimestepDuration dt, float plasticityModifier) {
    if (!brain) return;
    
    // Prediction system accuracy improves with development
    if (auto* predictionSystem = brain->getPredictionSystem()) {
        // Would need prediction system state update based on development
        // For now, we just note that prediction learning accelerates with age
        float predictionLearningRate = 0.01f * (1.5f - plasticityModifier);
        // Apply to prediction system if it has such a method
    }
}

void DevelopmentSystem::applyCognitionDevelopmentEffects(Brain* brain, TimestepDuration dt, float plasticityModifier) {
    if (!brain) return;
    
    // Neural planner develops more sophisticated planning with age
    if (auto* planner = brain->getPlanner()) {
        // Would apply planning sophistication changes
    }
    
    // Concept formation improves with development
    if (auto* conceptFormation = brain->getConceptFormation()) {
        // Would apply concept abstraction improvements
    }
    
    // Development affects attention parameters
    if (auto* attention = brain->getAttention()) {
        // Attention becomes more stable and selective with development
        float competitionThreshold = 0.3f + (0.4f * (1.0f - plasticityModifier));
        attention->setCompetitionThreshold(competitionThreshold);
    }
}

} // namespace nlm
