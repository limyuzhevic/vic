#include "DevelopmentSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/Synapse.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct DevelopmentSystem::Impl {
    DevelopmentalStage stage;
    SimulationStep stageStartStep;
    SimulationStep stepsInCurrentStage;
    double stageAge;  // Age since entering current stage
    
    // Developmental learning history
    std::vector<float> rewardHistory_;
    double explorationRate_;
    double exploitationRate_;
    
    // Development system state
    double baseExcitability_;
    double baseThreshold_;
    double baseRestingPotential_;
    double baseTimeConstant_;
    double baseRefractoryPeriod_;
    double baseInputResistance_;
    
    // Memory system state
    size_t baseWorkingMemoryCapacity_;
    double memoryConsolidationBase_;
    double patternCompletionBase_;
    
    // Neuromodulator system state
    double baseDopamineProduction_;
    double baseAcetylcholineProduction_;
    double baseNorepinephrineProduction_;
    double baseSerotoninProduction_;
    
    // Receptor densities
    double baseDopamineReceptors_;
    double baseAcetylcholineReceptors_;
    double baseNorepinephrineReceptors_;
    double baseSerotoninReceptors_;
    
    // Cognitive system state
    double baseAttentionStrength_;
    double baseConceptFormationRate_;
    double basePlanningCapability_;
    
    // Prediction system state
    double basePredictionAccuracy_;
    double basePredictionErrorSensitivity_;
    double baseTemporalPredictionAccuracy_;
    TimestepDuration basePredictionTimeHorizon_;
    float basePredictionUpdateRate_;
    float basePredictionErrorTolerance_;
    
    Impl() : stage(DevelopmentalStage::Initial), stageStartStep(0), stepsInCurrentStage(0), stageAge(0.0)
           , explorationRate_(0.3), exploitationRate_(0.7)
           // Initialize developmental system state
           , baseExcitability_(1.0), baseThreshold_(1.0), baseRestingPotential_(-70.0), baseTimeConstant_(10.0), baseRefractoryPeriod_(5.0), baseInputResistance_(100.0)
           // Initialize memory system state
           , baseWorkingMemoryCapacity_(50), memoryConsolidationBase_(1.0), patternCompletionBase_(0.5)
           // Initialize neuromodulator system state
           , baseDopamineProduction_(1.0), baseAcetylcholineProduction_(1.0), baseNorepinephrineProduction_(1.0), baseSerotoninProduction_(1.0)
           , baseDopamineReceptors_(1.0), baseAcetylcholineReceptors_(1.0), baseNorepinephrineReceptors_(1.0), baseSerotoninReceptors_(1.0)
           // Initialize cognitive system state
           , baseAttentionStrength_(1.0), baseConceptFormationRate_(1.0), basePlanningCapability_(1.0)
           // Initialize prediction system state
           , basePredictionAccuracy_(0.8), basePredictionErrorSensitivity_(1.0), baseTemporalPredictionAccuracy_(0.7)
           , basePredictionTimeHorizon_(100.0), basePredictionUpdateRate_(1.0), basePredictionErrorTolerance_(0.5) {}
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
            // Reset developmental system state for next stage
            resetToStageDefaults();
            break;
        case DevelopmentalStage::CriticalPeriod:
            pImpl->stage = DevelopmentalStage::Maturation;
            resetToStageDefaults();
            break;
        case DevelopmentalStage::Maturation:
            pImpl->stage = DevelopmentalStage::Adult;
            resetToStageDefaults();
            break;
        case DevelopmentalStage::Adult:
            pImpl->stage = DevelopmentalStage::Aging;
            resetToStageDefaults();
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
        default: return 0.5f;
    }
}

void DevelopmentSystem::setPlasticityFactor(float factor) {
    // Store the neuromodulator-enhanced plasticity factor
    pImpl->explorationRate_ = factor * 0.3f;  // Base exploration rate scaled by factor
    pImpl->exploitationRate_ = (1.0f - factor) * 0.7f;  // Base exploitation rate
}

bool DevelopmentSystem::isCriticalPeriod() const {
    return pImpl->stage == DevelopmentalStage::CriticalPeriod;
}

float DevelopmentSystem::getCriticalPeriodProgress() const {
    if (pImpl->stage != DevelopmentalStage::CriticalPeriod) {
        return 0.0f;
    }
    // Progress through critical period (0 to 1)
    // Calculate based on stage age
    if (pImpl->stageAge > 0.0f) {
        return std::min(1.0f, static_cast<float>(pImpl->stageAge) / 300.0f);
    }
    return 0.0f;
}

void DevelopmentSystem::resetToStageDefaults() {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // Initialize high plasticity, high learning capacity
            pImpl->baseExcitability_ = 1.2f;
            pImpl->baseThreshold_ = 0.8f;
            pImpl->baseRestingPotential_ = -65.0f;
            pImpl->baseTimeConstant_ = 8.0f;
            pImpl->baseRefractoryPeriod_ = 2.0f;
            pImpl->baseInputResistance_ = 120.0f;
            
            pImpl->baseWorkingMemoryCapacity_ = 30;
            pImpl->memoryConsolidationBase_ = 1.2f;
            pImpl->patternCompletionBase_ = 0.3f;
            
            pImpl->baseDopamineProduction_ = 0.8f;
            pImpl->baseAcetylcholineProduction_ = 0.8f;
            pImpl->baseNorepinephrineProduction_ = 0.8f;
            pImpl->baseSerotoninProduction_ = 0.8f;
            
            pImpl->baseDopamineReceptors_ = 1.2f;
            pImpl->baseAcetylcholineReceptors_ = 1.2f;
            pImpl->baseNorepinephrineReceptors_ = 1.2f;
            pImpl->baseSerotoninReceptors_ = 1.2f;
            
            pImpl->baseAttentionStrength_ = 1.0f;
            pImpl->baseConceptFormationRate_ = 1.0f;
            pImpl->basePlanningCapability_ = 0.5f;
            
            pImpl->basePredictionAccuracy_ = 0.6f;
            pImpl->basePredictionErrorSensitivity_ = 1.5f;
            pImpl->baseTemporalPredictionAccuracy_ = 0.5f;
            pImpl->basePredictionTimeHorizon_ = 50.0f;
            pImpl->basePredictionUpdateRate_ = 0.8f;
            pImpl->basePredictionErrorTolerance_ = 0.7f;
            break;
            
        case DevelopmentalStage::CriticalPeriod:
            // Enhanced plasticity for rapid learning
            pImpl->baseExcitability_ = 1.1f;
            pImpl->baseThreshold_ = 0.9f;
            pImpl->baseRestingPotential_ = -68.0f;
            pImpl->baseTimeConstant_ = 9.0f;
            pImpl->baseRefractoryPeriod_ = 3.0f;
            pImpl->baseInputResistance_ = 110.0f;
            
            pImpl->baseWorkingMemoryCapacity_ = 40;
            pImpl->memoryConsolidationBase_ = 1.1f;
            pImpl->patternCompletionBase_ = 0.4f;
            
            pImpl->baseDopamineProduction_ = 1.0f;
            pImpl->baseAcetylcholineProduction_ = 1.0f;
            pImpl->baseNorepinephrineProduction_ = 1.0f;
            pImpl->baseSerotoninProduction_ = 1.0f;
            
            pImpl->baseDopamineReceptors_ = 1.1f;
            pImpl->baseAcetylcholineReceptors_ = 1.1f;
            pImpl->baseNorepinephrineReceptors_ = 1.1f;
            pImpl->baseSerotoninReceptors_ = 1.1f;
            
            pImpl->baseAttentionStrength_ = 1.2f;
            pImpl->baseConceptFormationRate_ = 1.1f;
            pImpl->basePlanningCapability_ = 0.7f;
            
            pImpl->basePredictionAccuracy_ = 0.7f;
            pImpl->basePredictionErrorSensitivity_ = 1.2f;
            pImpl->baseTemporalPredictionAccuracy_ = 0.6f;
            pImpl->basePredictionTimeHorizon_ = 75.0f;
            pImpl->basePredictionUpdateRate_ = 0.9f;
            pImpl->basePredictionErrorTolerance_ = 0.6f;
            break;
            
        case DevelopmentalStage::Maturation:
            // Balanced development approaching adulthood
            pImpl->baseExcitability_ = 1.0f;
            pImpl->baseThreshold_ = 1.0f;
            pImpl->baseRestingPotential_ = -70.0f;
            pImpl->baseTimeConstant_ = 10.0f;
            pImpl->baseRefractoryPeriod_ = 5.0f;
            pImpl->baseInputResistance_ = 100.0f;
            
            pImpl->baseWorkingMemoryCapacity_ = 50;
            pImpl->memoryConsolidationBase_ = 1.0f;
            pImpl->patternCompletionBase_ = 0.5f;
            
            pImpl->baseDopamineProduction_ = 1.0f;
            pImpl->baseAcetylcholineProduction_ = 1.0f;
            pImpl->baseNorepinephrineProduction_ = 1.0f;
            pImpl->baseSerotoninProduction_ = 1.0f;
            
            pImpl->baseDopamineReceptors_ = 1.0f;
            pImpl->baseAcetylcholineReceptors_ = 1.0f;
            pImpl->baseNorepinephrineReceptors_ = 1.0f;
            pImpl->baseSerotoninReceptors_ = 1.0f;
            
            pImpl->baseAttentionStrength_ = 1.0f;
            pImpl->baseConceptFormationRate_ = 1.0f;
            pImpl->basePlanningCapability_ = 1.0f;
            
            pImpl->basePredictionAccuracy_ = 0.8f;
            pImpl->basePredictionErrorSensitivity_ = 1.0f;
            pImpl->baseTemporalPredictionAccuracy_ = 0.8f;
            pImpl->basePredictionTimeHorizon_ = 100.0f;
            pImpl->basePredictionUpdateRate_ = 1.0f;
            pImpl->basePredictionErrorTolerance_ = 0.5f;
            break;
            
        case DevelopmentalStage::Adult:
            // Stable adult performance
            pImpl->baseExcitability_ = 0.9f;
            pImpl->baseThreshold_ = 1.1f;
            pImpl->baseRestingPotential_ = -72.0f;
            pImpl->baseTimeConstant_ = 12.0f;
            pImpl->baseRefractoryPeriod_ = 7.0f;
            pImpl->baseInputResistance_ = 90.0f;
            
            pImpl->baseWorkingMemoryCapacity_ = 60;
            pImpl->memoryConsolidationBase_ = 0.9f;
            pImpl->patternCompletionBase_ = 0.7f;
            
            pImpl->baseDopamineProduction_ = 0.9f;
            pImpl->baseAcetylcholineProduction_ = 0.9f;
            pImpl->baseNorepinephrineProduction_ = 0.9f;
            pImpl->baseSerotoninProduction_ = 0.9f;
            
            pImpl->baseDopamineReceptors_ = 0.9f;
            pImpl->baseAcetylcholineReceptors_ = 0.9f;
            pImpl->baseNorepinephrineReceptors_ = 0.9f;
            pImpl->baseSerotoninReceptors_ = 0.9f;
            
            pImpl->baseAttentionStrength_ = 0.9f;
            pImpl->baseConceptFormationRate_ = 0.9f;
            pImpl->basePlanningCapability_ = 1.2f;
            
            pImpl->basePredictionAccuracy_ = 0.9f;
            pImpl->basePredictionErrorSensitivity_ = 0.8f;
            pImpl->baseTemporalPredictionAccuracy_ = 0.9f;
            pImpl->basePredictionTimeHorizon_ = 120.0f;
            pImpl->basePredictionUpdateRate_ = 1.1f;
            pImpl->basePredictionErrorTolerance_ = 0.4f;
            break;
            
        case DevelopmentalStage::Aging:
            // Reduced performance with stability
            pImpl->baseExcitability_ = 0.8f;
            pImpl->baseThreshold_ = 1.2f;
            pImpl->baseRestingPotential_ = -75.0f;
            pImpl->baseTimeConstant_ = 15.0f;
            pImpl->baseRefractoryPeriod_ = 10.0f;
            pImpl->baseInputResistance_ = 80.0f;
            
            pImpl->baseWorkingMemoryCapacity_ = 40;
            pImpl->memoryConsolidationBase_ = 0.7f;
            pImpl->patternCompletionBase_ = 0.8f;
            
            pImpl->baseDopamineProduction_ = 0.7f;
            pImpl->baseAcetylcholineProduction_ = 0.7f;
            pImpl->baseNorepinephrineProduction_ = 0.7f;
            pImpl->baseSerotoninProduction_ = 0.7f;
            
            pImpl->baseDopamineReceptors_ = 0.7f;
            pImpl->baseAcetylcholineReceptors_ = 0.7f;
            pImpl->baseNorepinephrineReceptors_ = 0.7f;
            pImpl->baseSerotoninReceptors_ = 0.7f;
            
            pImpl->baseAttentionStrength_ = 0.7f;
            pImpl->baseConceptFormationRate_ = 0.8f;
            pImpl->basePlanningCapability_ = 1.0f;
            
            pImpl->basePredictionAccuracy_ = 0.6f;
            pImpl->basePredictionErrorSensitivity_ = 0.6f;
            pImpl->baseTemporalPredictionAccuracy_ = 0.7f;
            pImpl->basePredictionTimeHorizon_ = 80.0f;
            pImpl->basePredictionUpdateRate_ = 0.8f;
            pImpl->basePredictionErrorTolerance_ = 0.6f;
            break;
    }
}

void DevelopmentSystem::applyPlasticityModulation(STDP* stdp, Hebbian* hebbian, float plasticityFactor) const {
    if (!stdp || !hebbian) return;
    
    // Stage-specific STDP learning rate modulation
    float stdpMod = getSTDPModulationFactor();
    
    // Stage-specific Hebbian learning rate modulation  
    float hebbianMod = getHebbianModulationFactor();
    
    // Configure STDP with developmental modulation
    float currentLTP = stdp->getLTPWeight();
    float currentLTD = stdp->getLTDWeight();
    float currentTau = stdp->getTau();
    
    // Apply stage-based modulation to STDP parameters
    float enhancedLTP = currentLTP * (1.0f + (stdpMod - 1.0f) * plasticityFactor * 0.5f);
    float enhancedLTD = currentLTD * (1.0f + (stdpMod - 1.0f) * plasticityFactor * 0.5f);
    float enhancedTau = currentTau * (1.0f + (stdpMod - 1.0f) * plasticityFactor * 0.3f);
    
    stdp->configure(enhancedLTP, enhancedLTD, enhancedTau);
    
    // Apply developmental modulation to Hebbian learning
    // (Hebbian learning would be configured separately in the brain)
    // This represents developmental enhancement/reduction of Hebbian learning strength
    if (hebbian) {
        // In a real implementation, this would configure Hebbian learning parameters
        // based on developmental stage (higher in early stages, lower in adult)
        // The hebbian module would use this information internally
    }
}

void DevelopmentSystem::applyEligibilityTraceModulation(Synapse* synapse, float& eligibilityTrace, float plasticityFactor) const {
    if (!synapse) return;
    
    // Stage-specific eligibility trace time constant
    TimestepDuration eligibilityTimeConstant = getEligibilityTraceTimeConstant();
    
    // Apply developmental modulation to eligibility trace dynamics
    float stageMod = 1.0f;
    if (pImpl->stage == DevelopmentalStage::Initial) {
        stageMod = 1.5f;  // Longer eligibility traces for initial stage
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod) {
        stageMod = 1.2f;  // Extended critical period eligibility
    } else if (pImpl->stage == DevelopmentalStage::Adult) {
        stageMod = 0.8f;  // Shorter eligibility traces in adult
    }
    
    // Apply plasticity factor
    stageMod *= plasticityFactor;
    
    // Modulate eligibility trace
    eligibilityTrace *= stageMod;
}

void DevelopmentSystem::applyStageBasedPlasticity(Synapse* synapse, float plasticityAmount, float& totalPlasticity) const {
    if (!synapse) return;
    
    // Calculate stage-based plasticity multiplier
    float plasticityMultiplier = 1.0f;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            plasticityMultiplier = 1.5f;  // High plasticity in initial stage
            break;
        case DevelopmentalStage::CriticalPeriod:
            plasticityMultiplier = 1.3f;  // Enhanced critical period plasticity
            break;
        case DevelopmentalStage::Maturation:
            plasticityMultiplier = 1.0f;  // Normal plasticity
            break;
        case DevelopmentalStage::Adult:
            plasticityMultiplier = 0.7f;  // Reduced plasticity for stability
            break;
        case DevelopmentalStage::Aging:
            plasticityMultiplier = 0.5f;  // Further reduced plasticity
            break;
    }
    
    // Apply to plasticity amount
    totalPlasticity = plasticityAmount * plasticityMultiplier;
}

float DevelopmentSystem::getSTDPModulationFactor() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 1.5f;
        case DevelopmentalStage::CriticalPeriod: return 1.3f;
        case DevelopmentalStage::Maturation: return 1.1f;
        case DevelopmentalStage::Adult: return 1.0f;
        case DevelopmentalStage::Aging: return 0.8f;
        default: return 1.0f;
    }
}

float DevelopmentSystem::getHebbianModulationFactor() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 1.4f;
        case DevelopmentalStage::CriticalPeriod: return 1.2f;
        case DevelopmentalStage::Maturation: return 1.0f;
        case DevelopmentalStage::Adult: return 0.8f;
        case DevelopmentalStage::Aging: return 0.6f;
        default: return 1.0f;
    }
}

TimestepDuration DevelopmentSystem::getEligibilityTraceTimeConstant() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 30.0f;
        case DevelopmentalStage::CriticalPeriod: return 25.0f;
        case DevelopmentalStage::Maturation: return 20.0f;
        case DevelopmentalStage::Adult: return 15.0f;
        case DevelopmentalStage::Aging: return 12.0f;
        default: return 20.0f;
    }
}

void DevelopmentSystem::applyExcitabilityModulation(Neuron* neuron, float& excitability) const {
    if (!neuron) return;
    
    // Calculate stage-based excitability multiplier
    float excitabilityMultiplier = 1.0f;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            excitabilityMultiplier = pImpl->baseExcitability_;
            break;
        case DevelopmentalStage::CriticalPeriod:
            excitabilityMultiplier = pImpl->baseExcitability_;
            break;
        case DevelopmentalStage::Maturation:
            excitabilityMultiplier = pImpl->baseExcitability_;
            break;
        case DevelopmentalStage::Adult:
            excitabilityMultiplier = pImpl->baseExcitability_;
            break;
        case DevelopmentalStage::Aging:
            excitabilityMultiplier = pImpl->baseExcitability_;
            break;
    }
    
    // Apply to excitability
    excitability *= excitabilityMultiplier;
}

void DevelopmentSystem::applyThresholdModulation(Neuron* neuron, float& threshold, float& resetPotential) const {
    if (!neuron) return;
    
    // Apply stage-based threshold and reset potential modulation
    float thresholdMultiplier = 1.0f;
    float resetMultiplier = 1.0f;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            thresholdMultiplier = pImpl->baseThreshold_;
            resetMultiplier = pImpl->baseRestingPotential_ / (-70.0f);
            break;
        case DevelopmentalStage::CriticalPeriod:
            thresholdMultiplier = pImpl->baseThreshold_;
            resetMultiplier = pImpl->baseRestingPotential_ / (-70.0f);
            break;
        case DevelopmentalStage::Maturation:
            thresholdMultiplier = pImpl->baseThreshold_;
            resetMultiplier = pImpl->baseRestingPotential_ / (-70.0f);
            break;
        case DevelopmentalStage::Adult:
            thresholdMultiplier = pImpl->baseThreshold_;
            resetMultiplier = pImpl->baseRestingPotential_ / (-70.0f);
            break;
        case DevelopmentalStage::Aging:
            thresholdMultiplier = pImpl->baseThreshold_;
            resetMultiplier = pImpl->baseRestingPotential_ / (-70.0f);
            break;
    }
    
    threshold *= thresholdMultiplier;
    resetPotential = -70.0f * resetMultiplier;
}

void DevelopmentSystem::applySpikeReliabilityModulation(Neuron* neuron, float& reliability) const {
    if (!neuron) return;
    
    // Stage-based spike reliability multiplier
    float reliabilityMultiplier = 1.0f;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            reliabilityMultiplier = 0.9f;  // Less reliable spiking in early development
            break;
        case DevelopmentalStage::CriticalPeriod:
            reliabilityMultiplier = 0.95f;  // Improving reliability
            break;
        case DevelopmentalStage::Maturation:
            reliabilityMultiplier = 1.0f;  // Optimal reliability
            break;
        case DevelopmentalStage::Adult:
            reliabilityMultiplier = 1.05f;  // Slightly enhanced reliability
            break;
        case DevelopmentalStage::Aging:
            reliabilityMultiplier = 0.85f;  // Decreased reliability
            break;
    }
    
    reliability *= reliabilityMultiplier;
}

float DevelopmentSystem::getTimeConstantMultiplier() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return pImpl->baseTimeConstant_ / 10.0f;
        case DevelopmentalStage::CriticalPeriod: return pImpl->baseTimeConstant_ / 10.0f;
        case DevelopmentalStage::Maturation: return pImpl->baseTimeConstant_ / 10.0f;
        case DevelopmentalStage::Adult: return pImpl->baseTimeConstant_ / 10.0f;
        case DevelopmentalStage::Aging: return pImpl->baseTimeConstant_ / 10.0f;
        default: return 1.0f;
    }
}

float DevelopmentSystem::getRefractoryMultiplier() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return pImpl->baseRefractoryPeriod_ / 5.0f;
        case DevelopmentalStage::CriticalPeriod: return pImpl->baseRefractoryPeriod_ / 5.0f;
        case DevelopmentalStage::Maturation: return pImpl->baseRefractoryPeriod_ / 5.0f;
        case DevelopmentalStage::Adult: return pImpl->baseRefractoryPeriod_ / 5.0f;
        case DevelopmentalStage::Aging: return pImpl->baseRefractoryPeriod_ / 5.0f;
        default: return 1.0f;
    }
}

float DevelopmentSystem::getInputResistanceMultiplier() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return pImpl->baseInputResistance_ / 100.0f;
        case DevelopmentalStage::CriticalPeriod: return pImpl->baseInputResistance_ / 100.0f;
        case DevelopmentalStage::Maturation: return pImpl->baseInputResistance_ / 100.0f;
        case DevelopmentalStage::Adult: return pImpl->baseInputResistance_ / 100.0f;
        case DevelopmentalStage::Aging: return pImpl->baseInputResistance_ / 100.0f;
        default: return 1.0f;
    }
}

void DevelopmentSystem::applyMemoryConsolidationModulation(float& consolidationFactor) const {
    // Apply stage-based memory consolidation modulation
    float consolidationMultiplier = 1.0f;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            consolidationMultiplier = pImpl->memoryConsolidationBase_;
            break;
        case DevelopmentalStage::CriticalPeriod:
            consolidationMultiplier = pImpl->memoryConsolidationBase_;
            break;
        case DevelopmentalStage::Maturation:
            consolidationMultiplier = pImpl->memoryConsolidationBase_;
            break;
        case DevelopmentalStage::Adult:
            consolidationMultiplier = pImpl->memoryConsolidationBase_;
            break;
        case DevelopmentalStage::Aging:
            consolidationMultiplier = pImpl->memoryConsolidationBase_;
            break;
    }
    
    consolidationFactor *= consolidationMultiplier;
}

size_t DevelopmentSystem::getWorkingMemoryCapacity() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 30;
        case DevelopmentalStage::CriticalPeriod: return 40;
        case DevelopmentalStage::Maturation: return 50;
        case DevelopmentalStage::Adult: return 60;
        case DevelopmentalStage::Aging: return 40;
        default: return 50;
    }
}

void DevelopmentSystem::applyEpisodicMemoryEncodingModulation(float& encodingStrength) const {
    // Stage-based encoding strength modulation
    float encodingMultiplier = 1.0f;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            encodingMultiplier = 0.8f;  // Weaker encoding in early stages
            break;
        case DevelopmentalStage::CriticalPeriod:
            encodingMultiplier = 0.9f;  // Improving encoding
            break;
        case DevelopmentalStage::Maturation:
            encodingMultiplier = 1.0f;  // Optimal encoding
            break;
        case DevelopmentalStage::Adult:
            encodingMultiplier = 1.1f;  // Enhanced encoding
            break;
        case DevelopmentalStage::Aging:
            encodingMultiplier = 0.7f;  // Reduced encoding
            break;
    }
    
    encodingStrength *= encodingMultiplier;
}

float DevelopmentSystem::getMemoryConsolidationModulation() const {
    // Get current memory consolidation modulation factor based on stage
    float consolidationFactor = 1.0f;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            consolidationFactor = pImpl->memoryConsolidationBase_;
            break;
        case DevelopmentalStage::CriticalPeriod:
            consolidationFactor = pImpl->memoryConsolidationBase_;
            break;
        case DevelopmentalStage::Maturation:
            consolidationFactor = pImpl->memoryConsolidationBase_;
            break;
        case DevelopmentalStage::Adult:
            consolidationFactor = pImpl->memoryConsolidationBase_;
            break;
        case DevelopmentalStage::Aging:
            consolidationFactor = pImpl->memoryConsolidationBase_;
            break;
    }
    
    return consolidationFactor;
}

float DevelopmentSystem::getPatternCompletionStrength() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 0.2f;
        case DevelopmentalStage::CriticalPeriod: return 0.3f;
        case DevelopmentalStage::Maturation: return 0.5f;
        case DevelopmentalStage::Adult: return 0.7f;
        case DevelopmentalStage::Aging: return 0.6f;
        default: return 0.5f;
    }
}

void DevelopmentSystem::applyNeuromodulatorProductionModulation(float& dopamineProduction, float& acetylcholineProduction, 
                                                                float& norepinephrineProduction, float& serotoninProduction) const {
    // Apply stage-based neuromodulator production modulation
    float dopamineMultiplier = 1.0f;
    float acetylcholineMultiplier = 1.0f;
    float norepinephrineMultiplier = 1.0f;
    float serotoninMultiplier = 1.0f;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            dopamineMultiplier = pImpl->baseDopamineProduction_;
            acetylcholineMultiplier = pImpl->baseAcetylcholineProduction_;
            norepinephrineMultiplier = pImpl->baseNorepinephrineProduction_;
            serotoninMultiplier = pImpl->baseSerotoninProduction_;
            break;
        case DevelopmentalStage::CriticalPeriod:
            dopamineMultiplier = pImpl->baseDopamineProduction_;
            acetylcholineMultiplier = pImpl->baseAcetylcholineProduction_;
            norepinephrineMultiplier = pImpl->baseNorepinephrineProduction_;
            serotoninMultiplier = pImpl->baseSerotoninProduction_;
            break;
        case DevelopmentalStage::Maturation:
            dopamineMultiplier = pImpl->baseDopamineProduction_;
            acetylcholineMultiplier = pImpl->baseAcetylcholineProduction_;
            norepinephrineMultiplier = pImpl->baseNorepinephrineProduction_;
            serotoninMultiplier = pImpl->baseSerotoninProduction_;
            break;
        case DevelopmentalStage::Adult:
            dopamineMultiplier = pImpl->baseDopamineProduction_;
            acetylcholineMultiplier = pImpl->baseAcetylcholineProduction_;
            norepinephrineMultiplier = pImpl->baseNorepinephrineProduction_;
            serotoninMultiplier = pImpl->baseSerotoninProduction_;
            break;
        case DevelopmentalStage::Aging:
            dopamineMultiplier = pImpl->baseDopamineProduction_;
            acetylcholineMultiplier = pImpl->baseAcetylcholineProduction_;
            norepinephrineMultiplier = pImpl->baseNorepinephrineProduction_;
            serotoninMultiplier = pImpl->baseSerotoninProduction_;
            break;
    }
    
    dopamineProduction *= dopamineMultiplier;
    acetylcholineProduction *= acetylcholineMultiplier;
    norepinephrineProduction *= norepinephrineMultiplier;
    serotoninProduction *= serotoninMultiplier;
}

void DevelopmentSystem::applyNeuromodulatorSensitivityModulation(float& dopamineSensitivity, float& acetylcholineSensitivity, 
                                                                   float& norepinephrineSensitivity, float& serotoninSensitivity) const {
    // Similar to production but for sensitivity
    applyNeuromodulatorProductionModulation(dopamineSensitivity, acetylcholineSensitivity, norepinephrineSensitivity, serotoninSensitivity);
}

void DevelopmentSystem::applyReceptorDensityModulation(float& dopamineReceptors, float& acetylcholineReceptors, 
                                                       float& norepinephrineReceptors, float& serotoninReceptors) const {
    // Apply stage-based receptor density modulation
    float dopamineMultiplier = 1.0f;
    float acetylcholineMultiplier = 1.0f;
    float norepinephrineMultiplier = 1.0f;
    float serotoninMultiplier = 1.0f;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            dopamineMultiplier = pImpl->baseDopamineReceptors_;
            acetylcholineMultiplier = pImpl->baseAcetylcholineReceptors_;
            norepinephrineMultiplier = pImpl->baseNorepinephrineReceptors_;
            serotoninMultiplier = pImpl->baseSerotoninReceptors_;
            break;
        case DevelopmentalStage::CriticalPeriod:
            dopamineMultiplier = pImpl->baseDopamineReceptors_;
            acetylcholineMultiplier = pImpl->baseAcetylcholineReceptors_;
            norepinephrineMultiplier = pImpl->baseNorepinephrineReceptors_;
            serotoninMultiplier = pImpl->baseSerotoninReceptors_;
            break;
        case DevelopmentalStage::Maturation:
            dopamineMultiplier = pImpl->baseDopamineReceptors_;
            acetylcholineMultiplier = pImpl->baseAcetylcholineReceptors_;
            norepinephrineMultiplier = pImpl->baseNorepinephrineReceptors_;
            serotoninMultiplier = pImpl->baseSerotoninReceptors_;
            break;
        case DevelopmentalStage::Adult:
            dopamineMultiplier = pImpl->baseDopamineReceptors_;
            acetylcholineMultiplier = pImpl->baseAcetylcholineReceptors_;
            norepinephrineMultiplier = pImpl->baseNorepinephrineReceptors_;
            serotoninMultiplier = pImpl->baseSerotoninReceptors_;
            break;
        case DevelopmentalStage::Aging:
            dopamineMultiplier = pImpl->baseDopamineReceptors_;
            acetylcholineMultiplier = pImpl->baseAcetylcholineReceptors_;
            norepinephrineMultiplier = pImpl->baseNorepinephrineReceptors_;
            serotoninMultiplier = pImpl->baseSerotoninReceptors_;
            break;
    }
    
    dopamineReceptors *= dopamineMultiplier;
    acetylcholineReceptors *= acetylcholineMultiplier;
    norepinephrineReceptors *= norepinephrineMultiplier;
    serotoninReceptors *= serotoninMultiplier;
}

float DevelopmentSystem::getDopamineProductionBaseline() const {
    return pImpl->baseDopamineProduction_;
}

float DevelopmentSystem::getAcetylcholineProductionBaseline() const {
    return pImpl->baseAcetylcholineProduction_;
}

float DevelopmentSystem::getNorepinephrineProductionBaseline() const {
    return pImpl->baseNorepinephrineProduction_;
}

float DevelopmentSystem::getSerotoninProductionBaseline() const {
    return pImpl->baseSerotoninProduction_;
}

void DevelopmentSystem::applyAttentionModulationModulation(float& attentionStrength) const {
    // Apply stage-based attention modulation
    float attentionMultiplier = 1.0f;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            attentionMultiplier = 0.8f;  // Limited attentional control early
            break;
        case DevelopmentalStage::CriticalPeriod:
            attentionMultiplier = 0.9f;  // Developing attentional control
            break;
        case DevelopmentalStage::Maturation:
            attentionMultiplier = 1.0f;  // Optimal attentional control
            break;
        case DevelopmentalStage::Adult:
            attentionMultiplier = 1.1f;  // Enhanced attentional control
            break;
        case DevelopmentalStage::Aging:
            attentionMultiplier = 0.7f;  // Declining attentional control
            break;
    }
    
    attentionStrength *= attentionMultiplier;
}

void DevelopmentSystem::applyConceptFormationModulation(float& formationRate) const {
    // Apply stage-based concept formation modulation
    float conceptMultiplier = 1.0f;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            conceptMultiplier = 0.5f;  // Limited concept formation
            break;
        case DevelopmentalStage::CriticalPeriod:
            conceptMultiplier = 0.7f;  // Rapid concept development
            break;
        case DevelopmentalStage::Maturation:
            conceptMultiplier = 1.0f;  // Optimal concept formation
            break;
        case DevelopmentalStage::Adult:
            conceptMultiplier = 1.2f;  // Enhanced concept refinement
            break;
        case DevelopmentalStage::Aging:
            conceptMultiplier = 0.8f;  // Slower concept formation
            break;
    }
    
    formationRate *= conceptMultiplier;
}

void DevelopmentSystem::applyPlanningCapabilityModulation(float& planningCapability) const {
    // Apply stage-based planning capability modulation
    float planningMultiplier = 1.0f;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            planningMultiplier = 0.3f;  // Basic planning abilities
            break;
        case DevelopmentalStage::CriticalPeriod:
            planningMultiplier = 0.5f;  // Developing planning skills
            break;
        case DevelopmentalStage::Maturation:
            planningMultiplier = 0.8f;  // Developing long-term planning
            break;
        case DevelopmentalStage::Adult:
            planningMultiplier = 1.0f;  // Full planning capabilities
            break;
        case DevelopmentalStage::Aging:
            planningMultiplier = 0.7f;  // Some decline in planning
            break;
    }
    
    planningCapability *= planningMultiplier;
}

float DevelopmentSystem::getAttentionGainFactor() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 0.8f;
        case DevelopmentalStage::CriticalPeriod: return 0.9f;
        case DevelopmentalStage::Maturation: return 1.0f;
        case DevelopmentalStage::Adult: return 1.1f;
        case DevelopmentalStage::Aging: return 0.7f;
        default: return 1.0f;
    }
}

float DevelopmentSystem::getConceptFormationEfficiency() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 0.5f;
        case DevelopmentalStage::CriticalPeriod: return 0.7f;
        case DevelopmentalStage::Maturation: return 1.0f;
        case DevelopmentalStage::Adult: return 1.2f;
        case DevelopmentalStage::Aging: return 0.8f;
        default: return 1.0f;
    }
}

size_t DevelopmentSystem::getPlanningDepthCapability() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 2;
        case DevelopmentalStage::CriticalPeriod: return 3;
        case DevelopmentalStage::Maturation: return 4;
        case DevelopmentalStage::Adult: return 5;
        case DevelopmentalStage::Aging: return 4;
        default: return 3;
    }
}

void DevelopmentSystem::applyPredictionAccuracyModulation(float& accuracy) const {
    // Apply stage-based prediction accuracy modulation
    float accuracyMultiplier = 1.0f;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            accuracyMultiplier = pImpl->basePredictionAccuracy_;
            break;
        case DevelopmentalStage::CriticalPeriod:
            accuracyMultiplier = pImpl->basePredictionAccuracy_;
            break;
        case DevelopmentalStage::Maturation:
            accuracyMultiplier = pImpl->basePredictionAccuracy_;
            break;
        case DevelopmentalStage::Adult:
            accuracyMultiplier = pImpl->basePredictionAccuracy_;
            break;
        case DevelopmentalStage::Aging:
            accuracyMultiplier = pImpl->basePredictionAccuracy_;
            break;
    }
    
    accuracy *= accuracyMultiplier;
}

void DevelopmentSystem::applyPredictionErrorSensitivityModulation(float& errorSensitivity) const {
    // Apply stage-based prediction error sensitivity modulation
    float sensitivityMultiplier = 1.0f;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            sensitivityMultiplier = pImpl->basePredictionErrorSensitivity_;
            break;
        case DevelopmentalStage::CriticalPeriod:
            sensitivityMultiplier = pImpl->basePredictionErrorSensitivity_;
            break;
        case DevelopmentalStage::Maturation:
            sensitivityMultiplier = pImpl->basePredictionErrorSensitivity_;
            break;
        case DevelopmentalStage::Adult:
            sensitivityMultiplier = pImpl->basePredictionErrorSensitivity_;
            break;
        case DevelopmentalStage::Aging:
            sensitivityMultiplier = pImpl->basePredictionErrorSensitivity_;
            break;
    }
    
    errorSensitivity *= sensitivityMultiplier;
}

void DevelopmentSystem::applyTemporalPredictionModulation(float& temporalAccuracy) const {
    // Apply stage-based temporal prediction modulation
    float temporalMultiplier = 1.0f;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            temporalMultiplier = pImpl->baseTemporalPredictionAccuracy_;
            break;
        case DevelopmentalStage::CriticalPeriod:
            temporalMultiplier = pImpl->baseTemporalPredictionAccuracy_;
            break;
        case DevelopmentalStage::Maturation:
            temporalMultiplier = pImpl->baseTemporalPredictionAccuracy_;
            break;
        case DevelopmentalStage::Adult:
            temporalMultiplier = pImpl->baseTemporalPredictionAccuracy_;
            break;
        case DevelopmentalStage::Aging:
            temporalMultiplier = pImpl->baseTemporalPredictionAccuracy_;
            break;
    }
    
    temporalAccuracy *= temporalMultiplier;
}

TimestepDuration DevelopmentSystem::getPredictionTimeHorizon() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return pImpl->basePredictionTimeHorizon_;
        case DevelopmentalStage::CriticalPeriod: return pImpl->basePredictionTimeHorizon_;
        case DevelopmentalStage::Maturation: return pImpl->basePredictionTimeHorizon_;
        case DevelopmentalStage::Adult: return pImpl->basePredictionTimeHorizon_;
        case DevelopmentalStage::Aging: return pImpl->basePredictionTimeHorizon_;
        default: return 100.0f;
    }
}

float DevelopmentSystem::getPredictionUpdateRate() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return pImpl->basePredictionUpdateRate_;
        case DevelopmentalStage::CriticalPeriod: return pImpl->basePredictionUpdateRate_;
        case DevelopmentalStage::Maturation: return pImpl->basePredictionUpdateRate_;
        case DevelopmentalStage::Adult: return pImpl->basePredictionUpdateRate_;
        case DevelopmentalStage::Aging: return pImpl->basePredictionUpdateRate_;
        default: return 1.0f;
    }
}

float DevelopmentSystem::getPredictionErrorTolerance() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return pImpl->basePredictionErrorTolerance_;
        case DevelopmentalStage::CriticalPeriod: return pImpl->basePredictionErrorTolerance_;
        case DevelopmentalStage::Maturation: return pImpl->basePredictionErrorTolerance_;
        case DevelopmentalStage::Adult: return pImpl->basePredictionErrorTolerance_;
        case DevelopmentalStage::Aging: return pImpl->basePredictionErrorTolerance_;
        default: return 0.5f;
    }
}

void DevelopmentSystem::integrateWithBrainSystems(Brain* brain) const {
    if (!brain) return;
    
    // Apply development system to all brain systems
    applyDevelopmentEffectsToAllSystems(brain);
    
    // Create feedback loops from development to other systems
    createFeedbackLoops(brain);
}

void DevelopmentSystem::applyDevelopmentEffectsToAllSystems(Brain* brain, float plasticityFactor) const {
    if (!brain) return;
    
    // 1. Apply plasticity effects
    if (brain->getSTDP()) {
        applyPlasticityModulation(brain->getSTDP(), brain->getHebbian(), plasticityFactor);
    }
    
    // 2. Apply neural dynamics effects
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                float excitability = 1.0f;
                applyExcitabilityModulation(neuron, excitability);
                
                // Update excitability on neuron
                neuron->injectCurrent(excitability * 0.5f);
                
                float threshold = 1.0f;
                float resetPotential = -70.0f;
                applyThresholdModulation(neuron, threshold, resetPotential);
                
                // Note: Threshold and reset potential are set at neuron initialization
                // so they can't be changed after neuron creation in this model
                
                float reliability = 1.0f;
                applySpikeReliabilityModulation(neuron, reliability);
            }
        }
    }
    
    // 3. Apply memory system effects
    if (brain->getWorkingMemory()) {
        // This would affect working memory internally
        // The brain would read developmental parameters
    }
    
    if (brain->getEpisodicMemory()) {
        float consolidationFactor = 1.0f;
        applyMemoryConsolidationModulation(consolidationFactor);
        // This would affect episodic memory consolidation
    }
    
    // 4. Apply neuromodulation effects
    if (brain->getDopamine()) {
        float dopamineProduction = 1.0f;
        float acetylcholineProduction = 1.0f;
        float norepinephrineProduction = 1.0f;
        float serotoninProduction = 1.0f;
        applyNeuromodulatorProductionModulation(dopamineProduction, acetylcholineProduction, 
                                               norepinephrineProduction, serotoninProduction);
    }
    
    // 5. Apply cognitive effects
    if (brain->getConceptFormation()) {
        float conceptFormationRate = 1.0f;
        applyConceptFormationModulation(conceptFormationRate);
    }
    
    if (brain->getPlanner()) {
        float planningCapability = 1.0f;
        applyPlanningCapabilityModulation(planningCapability);
    }
    
    if (brain->getAttention()) {
        float attentionStrength = 1.0f;
        applyAttentionModulationModulation(attentionStrength);
    }
    
    // 6. Apply prediction effects
    if (brain->getPredictionSystem()) {
        float predictionAccuracy = 1.0f;
        applyPredictionAccuracyModulation(predictionAccuracy);
        
        float predictionErrorSensitivity = 1.0f;
        applyPredictionErrorSensitivityModulation(predictionErrorSensitivity);
        
        float temporalAccuracy = 1.0f;
        applyTemporalPredictionModulation(temporalAccuracy);
    }
}

void DevelopmentSystem::createFeedbackLoops(Brain* brain) const {
    if (!brain) return;
    
    // 1. Create feedback from development to neuromodulation
    if (brain->getDopamine()) {
        // Development enhances dopamine sensitivity
        float dopamineSensitivity = 1.0f;
        applyNeuromodulatorSensitivityModulation(dopamineSensitivity, dopamineSensitivity, dopamineSensitivity, dopamineSensitivity);
    }
    
    // 2. Create feedback from development to plasticity
    // This creates a feedback loop where developmental stage modulates plasticity
    // and plasticity in turn influences development through learning history
    if (brain->getSTDP() && brain->getHebbian()) {
        applyPlasticityModulation(brain->getSTDP(), brain->getHebbian());
    }
    
    // 3. Create feedback from development to memory systems
    if (brain->getWorkingMemory()) {
        // Development affects working memory capacity
        size_t capacity = getWorkingMemoryCapacity();
        // This would update working memory capacity in the brain
    }
    
    if (brain->getEpisodicMemory()) {
        // Development affects episodic memory encoding
        float encodingStrength = 1.0f;
        applyEpisodicMemoryEncodingModulation(encodingStrength);
    }
    
    // 4. Create feedback from development to cognition
    if (brain->getConceptFormation()) {
        // Development affects concept formation rate
        float formationRate = 1.0f;
        applyConceptFormationModulation(formationRate);
    }
    
    if (brain->getAttentionalSelection()) {
        // Development affects attention modulation
        float attentionStrength = 1.0f;
        applyAttentionModulationModulation(attentionStrength);
    }
    
    // 5. Create feedback from development to prediction
    if (brain->getPredictionSystem()) {
        // Development affects prediction accuracy and error sensitivity
        float accuracy = 1.0f;
        applyPredictionAccuracyModulation(accuracy);
        
        float errorSensitivity = 1.0f;
        applyPredictionErrorSensitivityModulation(errorSensitivity);
    }
}

DevelopmentSystem::DevelopmentalProfile DevelopmentSystem::getDevelopmentalProfile() const {
    DevelopmentSystem::DevelopmentalProfile profile;
    profile.stage = pImpl->stage;
    profile.plasticity = getPlasticityModifier();
    profile.excitability = pImpl->baseExcitability_;
    profile.workingMemoryCapacity = getWorkingMemoryCapacity();
    profile.memoryConsolidation = pImpl->memoryConsolidationBase_;
    profile.attentionStrength = pImpl->baseAttentionStrength_;
    profile.predictionAccuracy = pImpl->basePredictionAccuracy_;
    profile.planningDepth = getPlanningDepthCapability();
    profile.age = age_;
    profile.stageProgress = getCriticalPeriodProgress();  // Use as general stage progress indicator
    return profile;
}

} // namespace nlm