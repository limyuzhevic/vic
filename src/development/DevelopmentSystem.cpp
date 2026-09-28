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
    // Enhanced stage modulation with stronger effects
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // High plasticity for rapid learning and development
            return 1.2f;  // Increased from 1.0f
        case DevelopmentalStage::CriticalPeriod:
            // Critical period for sensory-motor coordination
            // Very high plasticity for key learning windows
            return 1.0f;  // Increased from 0.8f
        case DevelopmentalStage::Maturation:
            // Continued learning but with reduced plasticity
            // Strong pruning and stabilization
            return 0.7f;  // Decreased from 0.5f - more effective maturation
        case DevelopmentalStage::Adult:
            // Stable plasticity for maintaining skills
            // Focused on refinement and efficiency
            return 0.3f;  // Decreased from 0.2f - more effective learning possible
        case DevelopmentalStage::Aging:
            // Reduced plasticity with compensatory mechanisms
            return 0.15f;  // Decreased from 0.1f - minimal maintenance
        default: return 0.5f;
    }
}

void DevelopmentSystem::applyDevelopmentalModulation(Brain* brain, float plasticityRate) {
    if (!brain || !brain->getStructuralPlasticity()) return;
    
    auto* structuralPlasticity = brain->getStructuralPlasticity();
    
    // Get current stage
    DevelopmentalStage stage = getStage();
    
    // Enhanced developmental stage effects on plasticity
    switch (stage) {
        case DevelopmentalStage::Initial:
            // Rapid synaptogenesis during initial development
            // High neural growth and connection formation
            structuralPlasticity->setSynaptogenesisRate(0.0002f);  // Increased
            structuralPlasticity->setPruningRate(0.000005f);       // Reduced
            
            // Enhanced Hebbian learning during initial phase
            if (auto* hebbian = brain->getHebbian()) {
                hebbian->setLearningRate(0.02f);  // Increased
            }
            break;
            
        case DevelopmentalStage::CriticalPeriod:
            // Critical period for sensory refinement
            // Strong activity-dependent synaptic strengthening
            structuralPlasticity->setSynaptogenesisRate(0.00005f); // Peak synaptogenesis
            structuralPlasticity->setPruningRate(0.00002f);       // Increased pruning
            
            // Enhanced STDP during critical period
            if (auto* stdp = brain->getSTDP()) {
                float ltpWeight = 0.015f;  // Enhanced LTP
                float ltdWeight = 0.018f; // Enhanced LTD
                float tau = 15.0f;        // Longer time window
                stdp->configure(ltpWeight, ltdWeight, tau);
            }
            break;
            
        case DevelopmentalStage::Maturation:
            // Pruning and stabilization of neural circuits
            // Balance between synaptic strengthening and removal
            structuralPlasticity->setSynaptogenesisRate(0.00001f); // Low synaptogenesis
            structuralPlasticity->setPruningRate(0.00003f);       // Moderate pruning
            
            // Fine-tuning of synaptic weights
            if (auto* stdp = brain->getSTDP()) {
                float ltpWeight = 0.01f;
                float ltdWeight = 0.012f;
                float tau = 20.0f;
                stdp->configure(ltpWeight, ltdWeight, tau);
            }
            break;
            
        case DevelopmentalStage::Adult:
            // Maintenance and optimization of established circuits
            // Focused on efficiency and skill refinement
            structuralPlasticity->setSynaptogenesisRate(0.000001f); // Very low
            structuralPlasticity->setPruningRate(0.00001f);       // Low pruning
            
            // Refinement learning with reduced plasticity
            if (auto* stdp = brain->getSTDP()) {
                float ltpWeight = 0.005f;
                float ltdWeight = 0.006f;
                float tau = 25.0f;
                stdp->configure(ltpWeight, ltdWeight, tau);
            }
            break;
            
        case DevelopmentalStage::Aging:
            // Compensatory mechanisms and maintenance
            // Reduced but not absent plasticity
            structuralPlasticity->setSynaptogenesisRate(0.0000005f); // Minimal
            structuralPlasticity->setPruningRate(0.000005f);       // Low pruning
            
            // Basic maintenance with enhanced error correction
            if (auto* stdp = brain->getSTDP()) {
                float ltpWeight = 0.002f;
                float ltdWeight = 0.003f;
                float tau = 30.0f;
                stdp->configure(ltpWeight, ltdWeight, tau);
            }
            break;
    }
    
    // Apply development-modulated learning rates to neural plasticity
    if (auto* hebbian = brain->getHebbian()) {
        // Developmental stage affects Hebbian learning rate
        float hebbianRate = getHebbianLearningRate(stage);
        hebbian->setLearningRate(hebbianRate);
    }
    
    if (auto* stdp = brain->getSTDP()) {
        // Developmental stage affects STDP parameters
        float stdpRate = getSTDPlearningRate(stage);
        stdp->setLearningRate(stdpRate);
    }
}

float DevelopmentSystem::getHebbianLearningRate(DevelopmentalStage stage) const {
    // Stage-dependent Hebbian learning rates
    switch (stage) {
        case DevelopmentalStage::Initial: return 0.03f;  // High for initial learning
        case DevelopmentalStage::CriticalPeriod: return 0.02f;  // High for refinement
        case DevelopmentalStage::Maturation: return 0.015f;  // Moderate
        case DevelopmentalStage::Adult: return 0.01f;   // Lower for maintenance
        case DevelopmentalStage::Aging: return 0.005f;  // Minimal
        default: return 0.01f;
    }
}

float DevelopmentSystem::getSTDPlearningRate(DevelopmentalStage stage) const {
    // Stage-dependent STDP learning rates
    switch (stage) {
        case DevelopmentalStage::Initial: return 0.025f;  // High LTP/LTD
        case DevelopmentalStage::CriticalPeriod: return 0.02f;  // Very high
        case DevelopmentalStage::Maturation: return 0.015f;  // Strong refinement
        case DevelopmentalStage::Adult: return 0.008f;   // Reduced
        case DevelopmentalStage::Aging: return 0.003f;  // Minimal
        default: return 0.01f;
    }
}

void DevelopmentSystem::updateDevelopment(Brain* brain, float plasticityRate) {
    // Apply enhanced developmental modulation
    applyDevelopmentalModulation(brain, plasticityRate);
    
    // Record developmental effects on neural networks
    if (brain) {
        // In a full implementation, this would record synaptic changes
        // and network dynamics for analysis
    }
}

void DevelopmentSystem::enhancePlasticityBasedOnPrediction(Brain* brain, 
                                                        float predictionError,
                                                        float plasticityRate) {
    if (!brain) return;
    
    // Prediction errors trigger developmental plasticity
    // Stronger prediction errors cause more significant developmental changes
    float errorBasedModulation = std::min(1.5f, predictionError * 3.0f);
    
    // Get current stage
    DevelopmentalStage stage = getStage();
    
    // Apply developmental stage-specific plasticity enhancement
    if (errorBasedModulation > 0.5f) {
        switch (stage) {
            case DevelopmentalStage::Initial:
                // High prediction errors during initial phase drive rapid development
                // Enhanced synaptogenesis and learning
                if (auto* structuralPlasticity = brain->getStructuralPlasticity()) {
                    structuralPlasticity->setSynaptogenesisRate(
                        0.0002f * errorBasedModulation);
                }
                break;
                
            case DevelopmentalStage::CriticalPeriod:
                // Critical period is sensitive to prediction errors
                // Drives sensory refinement and correction
                if (auto* stdp = brain->getSTDP()) {
                    float ltpWeight = 0.015f * errorBasedModulation;
                    float ltdWeight = 0.018f * errorBasedModulation;
                    stdp->configure(ltpWeight, ltdWeight, 15.0f);
                }
                break;
                
            case DevelopmentalStage::Maturation:
                // Prediction errors drive pruning and circuit optimization
                if (auto* structuralPlasticity = brain->getStructuralPlasticity()) {
                    structuralPlasticity->setPruningRate(
                        0.00003f * errorBasedModulation);
                }
                break;
                
            default:
                // In adult stages, prediction errors primarily drive refinement
                // Not wholesale restructuring
                break;
        }
    }
}

float DevelopmentSystem::getStageSpecificPlasticityBoost() const {
    // Get stage-specific plasticity enhancement factor
    // This is used to boost overall plasticity based on developmental stage
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 1.5f;  // 50% boost for rapid growth
        case DevelopmentalStage::CriticalPeriod: return 1.2f;  // 20% boost for refinement
        case DevelopmentalStage::Maturation: return 1.1f;  // 10% boost for pruning
        case DevelopmentalStage::Adult: return 1.05f;  // 5% boost for maintenance
        case DevelopmentalStage::Aging: return 1.02f;  // 2% boost for minimal plasticity
        default: return 1.0f;
    }
}

bool DevelopmentSystem::shouldEnhanceSynapticConnections(DevelopmentalStage stage) const {
    // Determine if synaptic connections should be enhanced based on stage
    switch (stage) {
        case DevelopmentalStage::Initial:
        case DevelopmentalStage::CriticalPeriod:
            // Enhance connections during early development
            return true;
        case DevelopmentalStage::Maturation:
            // Selective enhancement - focus on strong connections
            return true;
        case DevelopmentalStage::Adult:
        case DevelopmentalStage::Aging:
            // Maintain existing connections, enhance efficiency
            return false;
        default:
            return false;
    }
}

bool DevelopmentSystem::shouldPruneWeakConnections(DevelopmentalStage stage) const {
    // Determine if weak connections should be pruned based on stage
    switch (stage) {
        case DevelopmentalStage::Initial:
            // Limited pruning initially
            return false;
        case DevelopmentalStage::CriticalPeriod:
            // Aggressive pruning during critical period
            return true;
        case DevelopmentalStage::Maturation:
            // Continued pruning for circuit optimization
            return true;
        case DevelopmentalStage::Adult:
            // Minimal pruning - mostly maintenance
            return false;
        case DevelopmentalStage::Aging:
            // Selective pruning for efficiency
            return true;
        default:
            return false;
    }
}
