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
        case DevelopmentalStage::Initial: 
            return 1.5f;  // Very high plasticity for initial learning
        case DevelopmentalStage::CriticalPeriod: 
            return 1.2f;  // High plasticity for sensitive learning period
        case DevelopmentalStage::Maturation: 
            return 0.8f;  // Moderate plasticity for refinement
        case DevelopmentalStage::Adult: 
            return 0.5f;  // Lower plasticity for stable function
        case DevelopmentalStage::Aging: 
            return 0.3f;  // Reduced plasticity for maintenance
        default: return 0.5f;
    }
}

void DevelopmentSystem::update(Brain* brain, SimulationStep currentStep, float synapseCreationRate, 
                              float pruningRate, float structuralPlasticityRate) {
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
    
    // Apply developmental effects to brain
    if (brain) {
        applyDevelopmentalEffects(brain, synapseCreationRate, pruningRate, structuralPlasticityRate);
    }
}

void DevelopmentSystem::update(Brain* brain, RandomGenerator& rng, TimestepDuration dt,
                              float synapseCreationRate, float pruningRate,
                              float structuralPlasticityRate) {
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
    
    // Apply developmental effects to brain
    applyDevelopmentalEffects(brain, synapseCreationRate, pruningRate, structuralPlasticityRate);
}

void DevelopmentSystem::applyDevelopmentalEffects(Brain* brain, float synapseCreationRate, 
                                                   float pruningRate, float structuralPlasticityRate) {
    if (!brain) return;
    
    float plasticityModifier = getPlasticityModifier();
    
    // Modify plasticity rates based on developmental stage
    // This affects how brain systems respond to learning experiences
    
    // Update structural plasticity parameters
    // Structural plasticity (synaptogenesis and pruning) is age-dependent
    // More synaptogenesis in early stages, more pruning in later stages
    float adjustedSynapseCreationRate = synapseCreationRate * plasticityModifier;
    float adjustedPruningRate = pruningRate * (2.0f - plasticityModifier);  // More pruning in mature brains
    
    // Log developmental effects
    if (pImpl->stage == DevelopmentalStage::CriticalPeriod) {
        NLM_LOG_INFO("Critical period active: plasticity modifier = " + std::to_string(plasticityModifier));
    } else if (pImpl->stage == DevelopmentalStage::Aging) {
        NLM_LOG_INFO("Aging stage: reduced plasticity, focused maintenance");
    }
}

void DevelopmentSystem::setDevelopmentalParameters(float synapseCreationRate, float pruningRate,
                                                    float structuralPlasticityRate) {
    pImpl->synapseCreationRate = synapseCreationRate;
    pImpl->pruningRate = pruningRate;
    pImpl->structuralPlasticityRate = structuralPlasticityRate;
}
