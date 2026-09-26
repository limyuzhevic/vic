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
    
    // Apply development effects on neural properties
    if (brain) {
        // 1. Effects on neural excitability
        // Higher development stages reduce baseline excitability
        float excitabilityModifier = 1.0f;
        switch (pImpl->stage) {
            case DevelopmentalStage::Initial: excitabilityModifier = 1.0f; break;
            case DevelopmentalStage::CriticalPeriod: excitabilityModifier = 0.9f; break;
            case DevelopmentalStage::Maturation: excitabilityModifier = 0.8f; break;
            case DevelopmentalStage::Adult: excitabilityModifier = 0.7f; break;
            case DevelopmentalStage::Aging: excitabilityModifier = 0.6f; break;
        }
        
        // Apply to all neurons - in a full implementation this would be more targeted
        for (const auto& region : brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Development reduces resting membrane potential over time
                    // This makes neurons less excitable in later stages
                    float currentResting = neuron->getState().restingPotential;
                    neuron->setRestingPotential(currentResting * excitabilityModifier);
                }
            }
        }
        
        // 2. Effects on plasticity rules
        // Development modifies plasticity parameters based on stage
        auto* stdp = brain->getSTDP();
        auto* hebbian = brain->getHebbian();
        auto* structuralPlasticity = brain->getStructuralPlasticity();
        
        if (stdp) {
            float stdpModifier = 1.0f;
            switch (pImpl->stage) {
                case DevelopmentalStage::Initial: stdpModifier = 1.0f; break;
                case DevelopmentalStage::CriticalPeriod: stdpModifier = 0.9f; break;
                case DevelopmentalStage::Maturation: stdpModifier = 0.7f; break;
                case DevelopmentalStage::Adult: stdpModifier = 0.5f; break;
                case DevelopmentalStage::Aging: stdpModifier = 0.3f; break;
            }
            // Apply to STDP parameters
            float ltpWeight = stdp->getLTPWeight() * stdpModifier;
            float ltdWeight = stdp->getLTDWeight() * stdpModifier;
            stdp->configure(ltpWeight, ltdWeight, stdp->getTau());
        }
        
        if (hebbian) {
            float hebbianModifier = 1.0f;
            switch (pImpl->stage) {
                case DevelopmentalStage::Initial: hebbianModifier = 1.0f; break;
                case DevelopmentalStage::CriticalPeriod: hebbianModifier = 0.9f; break;
                case DevelopmentalStage::Maturation: hebbianModifier = 0.7f; break;
                case DevelopmentalStage::Adult: hebbianModifier = 0.5f; break;
                case DevelopmentalStage::Aging: hebbianModifier = 0.3f; break;
            }
            // Apply to Hebbian parameters (conceptually)
            // In a full implementation, there would be Hebbian-specific parameters
        }
        
        if (structuralPlasticity) {
            // Structural plasticity is already modulated in Brain.cpp
            // But we can also adjust the rates based on development
            float synRate = 0.0001f;
            float pruneRate = 0.00001f;
            
            switch (pImpl->stage) {
                case DevelopmentalStage::Initial: 
                    synRate = 0.0002f; 
                    pruneRate = 0.000005f; 
                    break;
                case DevelopmentalStage::CriticalPeriod: 
                    synRate = 0.00015f; 
                    pruneRate = 0.000008f; 
                    break;
                case DevelopmentalStage::Maturation: 
                    synRate = 0.0001f; 
                    pruneRate = 0.00001f; 
                    break;
                case DevelopmentalStage::Adult: 
                    synRate = 0.00005f; 
                    pruneRate = 0.000015f; 
                    break;
                case DevelopmentalStage::Aging: 
                    synRate = 0.00002f; 
                    pruneRate = 0.00002f; 
                    break;
            }
            
            structuralPlasticity->setSynaptogenesisRate(synRate);
            structuralPlasticity->setPruningRate(pruneRate);
        }
        
        // 3. Effects on attention
        // Development affects attention parameters
        auto* attention = brain->getAttention();
        if (attention) {
            float attentionInhibition = 0.5f;
            float attentionExcitation = 1.5f;
            
            switch (pImpl->stage) {
                case DevelopmentalStage::Initial: 
                    attentionInhibition = 0.7f; 
                    attentionExcitation = 1.8f; 
                    break;
                case DevelopmentalStage::CriticalPeriod: 
                    attentionInhibition = 0.6f; 
                    attentionExcitation = 1.7f; 
                    break;
                case DevelopmentalStage::Maturation: 
                    attentionInhibition = 0.5f; 
                    attentionExcitation = 1.6f; 
                    break;
                case DevelopmentalStage::Adult: 
                    attentionInhibition = 0.4f; 
                    attentionExcitation = 1.5f; 
                    break;
                case DevelopmentalStage::Aging: 
                    attentionInhibition = 0.3f; 
                    attentionExcitation = 1.4f; 
                    break;
            }
            
            attention->setInhibitionStrength(attentionInhibition);
            attention->setExcitationStrength(attentionExcitation);
        }
        
        // 4. Effects on memory consolidation
        // Development affects memory consolidation parameters
        // In a full implementation, this would affect working memory decay rates,
        // episodic memory consolidation intervals, etc.
        
        // 5. Effects on neuromodulation
        // Development affects neuromodulator levels and effects
        auto* dopamine = brain->getDopamine();
        if (dopamine) {
            float dopamineBaseline = 0.1f;
            switch (pImpl->stage) {
                case DevelopmentalStage::Initial: dopamineBaseline = 0.1f; break;
                case DevelopmentalStage::CriticalPeriod: dopamineBaseline = 0.08f; break;
                case DevelopmentalStage::Maturation: dopamineBaseline = 0.06f; break;
                case DevelopmentalStage::Adult: dopamineBaseline = 0.05f; break;
                case DevelopmentalStage::Aging: dopamineBaseline = 0.04f; break;
            }
            dopamine->setLevel(dopamineBaseline);
        }
    }
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
    
    // Apply development effects on neural properties
    if (brain) {
        // 1. Effects on neural excitability
        // Higher development stages reduce baseline excitability
        float excitabilityModifier = 1.0f;
        switch (pImpl->stage) {
            case DevelopmentalStage::Initial: excitabilityModifier = 1.0f; break;
            case DevelopmentalStage::CriticalPeriod: excitabilityModifier = 0.9f; break;
            case DevelopmentalStage::Maturation: excitabilityModifier = 0.8f; break;
            case DevelopmentalStage::Adult: excitabilityModifier = 0.7f; break;
            case DevelopmentalStage::Aging: excitabilityModifier = 0.6f; break;
        }
        
        // Apply to all neurons - in a full implementation this would be more targeted
        for (const auto& region : brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Development reduces resting membrane potential over time
                    // This makes neurons less excitable in later stages
                    float currentResting = neuron->getState().restingPotential;
                    neuron->setRestingPotential(currentResting * excitabilityModifier);
                }
            }
        }
        
        // 2. Effects on plasticity rules
        // Development modifies plasticity parameters based on stage
        auto* stdp = brain->getSTDP();
        auto* hebbian = brain->getHebbian();
        auto* structuralPlasticity = brain->getStructuralPlasticity();
        
        if (stdp) {
            float stdpModifier = 1.0f;
            switch (pImpl->stage) {
                case DevelopmentalStage::Initial: stdpModifier = 1.0f; break;
                case DevelopmentalStage::CriticalPeriod: stdpModifier = 0.9f; break;
                case DevelopmentalStage::Maturation: stdpModifier = 0.7f; break;
                case DevelopmentalStage::Adult: stdpModifier = 0.5f; break;
                case DevelopmentalStage::Aging: stdpModifier = 0.3f; break;
            }
            // Apply to STDP parameters
            float ltpWeight = stdp->getLTPWeight() * stdpModifier;
            float ltdWeight = stdp->getLTDWeight() * stdpModifier;
            stdp->configure(ltpWeight, ltdWeight, stdp->getTau());
        }
        
        if (hebbian) {
            float hebbianModifier = 1.0f;
            switch (pImpl->stage) {
                case DevelopmentalStage::Initial: hebbianModifier = 1.0f; break;
                case DevelopmentalStage::CriticalPeriod: hebbianModifier = 0.9f; break;
                case DevelopmentalStage::Maturation: hebbianModifier = 0.7f; break;
                case DevelopmentalStage::Adult: hebbianModifier = 0.5f; break;
                case DevelopmentalStage::Aging: hebbianModifier = 0.3f; break;
            }
            // Apply to Hebbian parameters (conceptually)
            // In a full implementation, there would be Hebbian-specific parameters
        }
        
        if (structuralPlasticity) {
            // Structural plasticity is already modulated in Brain.cpp
            // But we can also adjust the rates based on development
            float synRate = 0.0001f;
            float pruneRate = 0.00001f;
            
            switch (pImpl->stage) {
                case DevelopmentalStage::Initial: 
                    synRate = 0.0002f; 
                    pruneRate = 0.000005f; 
                    break;
                case DevelopmentalStage::CriticalPeriod: 
                    synRate = 0.00015f; 
                    pruneRate = 0.000008f; 
                    break;
                case DevelopmentalStage::Maturation: 
                    synRate = 0.0001f; 
                    pruneRate = 0.00001f; 
                    break;
                case DevelopmentalStage::Adult: 
                    synRate = 0.00005f; 
                    pruneRate = 0.000015f; 
                    break;
                case DevelopmentalStage::Aging: 
                    synRate = 0.00002f; 
                    pruneRate = 0.00002f; 
                    break;
            }
            
            structuralPlasticity->setSynaptogenesisRate(synRate);
            structuralPlasticity->setPruningRate(pruneRate);
        }
        
        // 3. Effects on attention
        // Development affects attention parameters
        auto* attention = brain->getAttention();
        if (attention) {
            float attentionInhibition = 0.5f;
            float attentionExcitation = 1.5f;
            
            switch (pImpl->stage) {
                case DevelopmentalStage::Initial: 
                    attentionInhibition = 0.7f; 
                    attentionExcitation = 1.8f; 
                    break;
                case DevelopmentalStage::CriticalPeriod: 
                    attentionInhibition = 0.6f; 
                    attentionExcitation = 1.7f; 
                    break;
                case DevelopmentalStage::Maturation: 
                    attentionInhibition = 0.5f; 
                    attentionExcitation = 1.6f; 
                    break;
                case DevelopmentalStage::Adult: 
                    attentionInhibition = 0.4f; 
                    attentionExcitation = 1.5f; 
                    break;
                case DevelopmentalStage::Aging: 
                    attentionInhibition = 0.3f; 
                    attentionExcitation = 1.4f; 
                    break;
            }
            
            attention->setInhibitionStrength(attentionInhibition);
            attention->setExcitationStrength(attentionExcitation);
        }
        
        // 4. Effects on memory consolidation
        // Development affects memory consolidation parameters
        // In a full implementation, this would affect working memory decay rates,
        // episodic memory consolidation intervals, etc.
        
        // 5. Effects on neuromodulation
        // Development affects neuromodulator levels and effects
        auto* dopamine = brain->getDopamine();
        if (dopamine) {
            float dopamineBaseline = 0.1f;
            switch (pImpl->stage) {
                case DevelopmentalStage::Initial: dopamineBaseline = 0.1f; break;
                case DevelopmentalStage::CriticalPeriod: dopamineBaseline = 0.08f; break;
                case DevelopmentalStage::Maturation: dopamineBaseline = 0.06f; break;
                case DevelopmentalStage::Adult: dopamineBaseline = 0.05f; break;
                case DevelopmentalStage::Aging: dopamineBaseline = 0.04f; break;
            }
            dopamine->setLevel(dopamineBaseline);
        }
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
