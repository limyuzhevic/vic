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
    
    // Apply developmental effects to brain based on stage
    applyDevelopmentalEffects(brain, currentStep);
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
    
    // Apply developmental effects to brain based on stage
    applyDevelopmentalEffects(brain, currentStep);
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
    
    // Apply developmental effects to brain based on stage
    applyDevelopmentalEffects(brain, dt);
}

void DevelopmentSystem::applyDevelopmentalEffects(Brain* brain, double dt) {
    if (!brain) return;
    
    // Get plasticity modifier for current developmental stage
    float plasticityModifier = getPlasticityModifier();
    
    // Get development system access
    auto* devSystem = brain->getDevelopmentSystem();
    if (!devSystem) return;
    
    // Apply age-dependent plasticity effects
    applyPlasticityModulation(brain, plasticityModifier, dt);
    
    // Apply structural changes based on development stage
    applyStructuralPlasticity(brain, plasticityModifier, dt);
    
    // Update learning capabilities
    updateLearningCapabilities(brain, plasticityModifier, dt);
}

void DevelopmentSystem::applyPlasticityModulation(Brain* brain, float plasticityModifier, double dt) {
    // Age-dependent plasticity modulation
    // Early stages: high plasticity, rapid learning
    // Later stages: lower plasticity, stable networks
    
    if (!brain) return;
    
    // Get access to plasticity systems
    auto* stdp = brain->getSTDP();
    auto* hebbian = brain->getHebbian();
    auto* structuralPlasticity = brain->getStructuralPlasticity();
    
    if (!stdp || !hebbian || !structuralPlasticity) return;
    
    // Modulate STDP learning rates based on developmental stage
    float baseLTP = 0.01f;
    float baseLTD = 0.012f;
    float modulatedLTP = baseLTP * plasticityModifier;
    float modulatedLTD = baseLTD * plasticityModifier;
    
    stdp->setLTPWeight(modulatedLTP);
    stdp->setLTDWeight(modulatedLTD);
    
    // Modulate Hebbian learning rate
    // Hebbian strength scales with developmental plasticity
    float hebbianWeight = 0.01f * plasticityModifier;
    hebbian->setLearningRate(hebbianWeight);
    
    // Structural plasticity rates vary by stage
    // Early development: more synaptogenesis, more pruning
    // Later stages: stable connectivity, minimal changes
    float synaptogenesisRate = 0.0001f * plasticityModifier;
    float pruningRate = 0.00001f * (2.0f - plasticityModifier);
    
    structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
    structuralPlasticity->setPruningRate(pruningRate);
}

void DevelopmentSystem::applyStructuralPlasticity(Brain* brain, float plasticityModifier, double dt) {
    if (!brain) return;
    
    auto* structuralPlasticity = brain->getStructuralPlasticity();
    if (!structuralPlasticity) return;
    
    // Development stage-specific structural changes
    DevelopmentalStage stage = getStage();
    
    switch (stage) {
        case DevelopmentalStage::Initial:
            // Rapid synaptogenesis during initial stage
            // High connectivity establishment
            structuralPlasticity->update(brain, *brain->getRandomGenerator());
            break;
            
        case DevelopmentalStage::CriticalPeriod:
            // Refinement phase - selective strengthening and pruning
            // Critical for sensory system development
            structuralPlasticity->update(brain, *brain->getRandomGenerator());
            
            // Enhanced pruning during critical period
            float enhancedPruningRate = 0.00002f * (2.0f - plasticityModifier);
            structuralPlasticity->setPruningRate(enhancedPruningRate);
            break;
            
        case DevelopmentalStage::Maturation:
            // Continued refinement with decreasing plasticity
            // More stable network architecture
            structuralPlasticity->update(brain, *brain->getRandomGenerator());
            break;
            
        case DevelopmentalStage::Adult:
            // Minimal structural changes, mostly maintenance
            // Synaptic homeostasis
            structuralPlasticity->update(brain, *brain->getRandomGenerator());
            break;
            
        case DevelopmentalStage::Aging:
            // Age-related decline in structural maintenance
            // Pathological changes may begin
            break;
            
        default:
            // Default maintenance behavior
            structuralPlasticity->update(brain, *brain->getRandomGenerator());
            break;
    }
}

void DevelopmentSystem::updateLearningCapabilities(Brain* brain, float plasticityModifier, double dt) {
    if (!brain) return;
    
    // Update learning system capabilities based on development stage
    DevelopmentalStage stage = getStage();
    
    switch (stage) {
        case DevelopmentalStage::Initial:
            // High learning capacity, rapid adaptation
            // Enhanced memory formation and recall
            if (brain->getWorkingMemory()) {
                brain->getWorkingMemory()->setCapacity(brain->getTotalNeuronCount() / 10);
            }
            break;
            
        case DevelopmentalStage::CriticalPeriod:
            // Sensitive period for sensory learning
            // Enhanced sensory processing and refinement
            if (brain->getWorkingMemory()) {
                // Critical period plasticity enhancement
                float learningRate = 1.0f / (1.0f + getCriticalPeriodProgress() * 2.0f);
                brain->getWorkingMemory()->setLearningRate(learningRate);
            }
            break;
            
        case DevelopmentalStage::Maturation:
            // Consolidation phase - transfer to long-term storage
            // Enhanced episodic memory formation
            if (brain->getEpisodicMemory()) {
                brain->getEpisodicMemory()->setMaxEpisodes(1000);
            }
            break;
            
        case DevelopmentalStage::Adult:
            // Stable learning capabilities
            // Efficient retrieval and application
            break;
            
        default:
            // Default behavior
            break;
    }
}
