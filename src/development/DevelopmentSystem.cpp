#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../development/Synaptogenesis.hpp"
#include "../development/Pruning.hpp"
#include "../development/Maturation.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <memory>

namespace nlm {

struct DevelopmentSystem::Impl {
    // Developmental stage information
    DevelopmentalStage stage;
    SimulationStep stageStartStep;
    SimulationStep stepsInCurrentStage;
    double stageAge;  // Age since entering current stage
    
    // Critical period information
    double criticalPeriodEndAge;
    bool criticalPeriodActive;
    
    // Experience-dependent plasticity components
    std::vector<float> synapticHistory;     // Recent synaptic activity
    std::vector<float> neuromodulationHistory; // Recent neuromodulator levels
    float experienceStrength;               // Accumulated experience strength
    
    // Connection-specific timing
    std::unordered_map<SynapseId, double> connectionMaturationTimes;
    std::unordered_map<NeuronId, double> neuronMaturationTimes;
    
    // Developmental plasticity modulation
    float plasticityModulation;             // Current plasticity level (0-1)
    float activityDependency;               // How much development depends on activity
    
    // Developmental learning with reward
    double rewardPredictionError;           // RPE signal for development
    double developmentalLearningRate;       // Learning rate for developmental changes
    
    // Age-dependent parameters
    double initialStageDuration;            // Duration of initial stage
    double criticalPeriodDuration;          // Duration of critical period
    double maturationDuration;              // Duration of maturation stage
    
    Impl() : stage(DevelopmentalStage::Initial),
             stageStartStep(0), stepsInCurrentStage(0), stageAge(0.0),
             criticalPeriodEndAge(360.0), criticalPeriodActive(true),
             experienceStrength(0.0f), plasticityModulation(1.0f),
             activityDependency(0.8f), rewardPredictionError(0.0),
             developmentalLearningRate(0.01),
             initialStageDuration(60.0),
             criticalPeriodDuration(300.0),
             maturationDuration(600.0) {}
};

DevelopmentSystem::DevelopmentSystem() : pImpl(new Impl()), age_(0.0) {}

DevelopmentSystem::~DevelopmentSystem() = default;

DevelopmentalStage DevelopmentSystem::getStage() const {
    return pImpl->stage;
}

void DevelopmentSystem::setStage(DevelopmentalStage stage) {
    pImpl->stage = stage;
    pImpl->stageAge = 0.0;
    pImpl->stepsInCurrentStage = 0;
    
    NLM_LOG_INFO("DevelopmentSystem: Stage changed to " + std::string(getStageName()));
}

void DevelopmentSystem::advanceStage() {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            pImpl->stage = DevelopmentalStage::CriticalPeriod;
            pImpl->criticalPeriodActive = true;
            pImpl->criticalPeriodEndAge = age_ + pImpl->criticalPeriodDuration;
            break;
        case DevelopmentalStage::CriticalPeriod:
            pImpl->stage = DevelopmentalStage::Maturation;
            pImpl->criticalPeriodActive = false;
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
    pImpl->stageStartStep = 0;
    pImpl->stepsInCurrentStage = 0;
    pImpl->stageAge = 0.0;
    
    NLM_LOG_INFO("DevelopmentSystem: Advanced to stage: " + std::string(getStageName()));
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
    if (!brain) return;
    
    ++pImpl->stepsInCurrentStage;
    age_ += 0.001;  // Approximate timestep
    pImpl->stageAge += 0.001;
    
    // Update critical period status
    pImpl->criticalPeriodActive = (age_ < pImpl->criticalPeriodEndAge);
    
    // Get neuromodulator ensemble for plasticity control
    auto* neuromodulators = brain->getNeuromodulatorEnsemble();
    float neuromodulationFactor = 1.0f;
    if (neuromodulators) {
        neuromodulationFactor = neuromodulators->getCombinedPlasticityFactor();
    }
    
    // Get random generator
    auto* rng = brain->getRandomGenerator();
    if (!rng) {
        NLM_LOG_WARNING("DevelopmentSystem: No random generator available");
        return;
    }
    
    // Update with full parameters for comprehensive development
    update(brain, *rng, 0.001); // Using small timestep
}

void DevelopmentSystem::update(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    if (!brain) return;
    
    age_ += dt;
    pImpl->stageAge += dt;
    ++pImpl->stepsInCurrentStage;
    
    // Update critical period status
    pImpl->criticalPeriodActive = (age_ < pImpl->criticalPeriodEndAge);
    
    // Get neuromodulator ensemble for plasticity control
    auto* neuromodulators = brain->getNeuromodulatorEnsemble();
    float neuromodulationFactor = 1.0f;
    if (neuromodulators) {
        neuromodulationFactor = neuromodulators->getCombinedPlasticityFactor();
    }
    
    // Auto-advance stage based on developmental age
    // These thresholds are in simulation seconds
    if (pImpl->stage == DevelopmentalStage::Initial && age_ > pImpl->initialStageDuration) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod && age_ > pImpl->criticalPeriodEndAge) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::Maturation && 
               age_ > (pImpl->criticalPeriodEndAge + pImpl->maturationDuration)) {
        advanceStage();
    }
    
    // Coordinate developmental processes
    coordinateDevelopment(brain, rng, dt, neuromodulationFactor);
}

void DevelopmentSystem::coordinateDevelopment(Brain* brain, RandomGenerator& rng, 
                                             TimestepDuration dt, float neuromodulationFactor) {
    if (!brain) return;
    
    // Update development-specific processes based on stage
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // Initial stage: basic synaptogenesis, limited pruning
            handleInitialDevelopment(brain, rng, dt, neuromodulationFactor);
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Critical period: high plasticity, activity-dependent refinement
            handleCriticalPeriodDevelopment(brain, rng, dt, neuromodulationFactor);
            break;
        case DevelopmentalStage::Maturation:
            // Maturation stage: synaptic pruning, stabilization
            handleMaturationDevelopment(brain, rng, dt, neuromodulationFactor);
            break;
        case DevelopmentalStage::Adult:
            // Adult stage: maintenance, limited plasticity
            handleAdultDevelopment(brain, rng, dt, neuromodulationFactor);
            break;
        case DevelopmentalStage::Aging:
            // Aging stage: degeneration, reduced plasticity
            handleAgingDevelopment(brain, rng, dt, neuromodulationFactor);
            break;
    }
}

void DevelopmentSystem::handleInitialDevelopment(Brain* brain, RandomGenerator& rng, 
                                                TimestepDuration dt, float neuromodulationFactor) {
    if (!brain) return;
    
    // Initial stage: high synaptogenesis rate
    float initialSynaptogenesisRate = 0.1f * neuromodulationFactor;
    float initialPruningRate = 0.01f * neuromodulationFactor;
    
    // Update synaptogenesis system
    // Note: In real implementation, we would access the actual Synaptogenesis instance
    // For now, we coordinate through the brain's development system
    
    // Update pruning system
    // Note: In real implementation, we would access the actual Pruning instance
    
    // Update experience-dependent plasticity
    pImpl->experienceStrength *= (1.0f - 0.01f * dt * 1000.0f); // Decay
    // Add new experience
    pImpl->experienceStrength += neuromodulationFactor * 0.001f * dt * 1000.0f;
}

void DevelopmentSystem::handleCriticalPeriodDevelopment(Brain* brain, RandomGenerator& rng, 
                                                       TimestepDuration dt, float neuromodulationFactor) {
    if (!brain) return;
    
    // Critical period: very high plasticity, strong activity dependence
    float criticalPeriodFactor = 2.0f; // Double plasticity during critical period
    
    // Enhanced synaptogenesis with high neuromodulation sensitivity
    float criticalPeriodSynaptogenesisRate = 0.15f * neuromodulationFactor * criticalPeriodFactor;
    
    // Activity-dependent pruning with competition
    float criticalPeriodPruningRate = 0.05f * neuromodulationFactor * criticalPeriodFactor;
    
    // Update experience integration
    pImpl->activityDependency = 0.9f; // Very activity-dependent during critical period
    pImpl->experienceStrength *= (1.0f - 0.01f * dt * 1000.0f);
    pImpl->experienceStrength += neuromodulationFactor * 0.002f * dt * 1000.0f; // Higher accumulation
    
    // Update reward prediction error for developmental learning
    updateDevelopmentalLearning(brain, rng, dt);
}

void DevelopmentSystem::handleMaturationDevelopment(Brain* brain, RandomGenerator& rng, 
                                                   TimestepDuration dt, float neuromodulationFactor) {
    if (!brain) return;
    
    // Maturation stage: stabilization, pruning, reduced plasticity
    float maturationFactor = 0.6f; // Reduced plasticity
    
    // Activity-dependent synaptic stabilization
    float stabilizationRate = 0.08f * neuromodulationFactor * maturationFactor;
    
    // Competitive synaptic pruning
    float maturationPruningRate = 0.03f * neuromodulationFactor * maturationFactor;
    
    // Update connectivity patterns based on accumulated experience
    pImpl->activityDependency = 0.6f; // Moderate activity dependence
    pImpl->experienceStrength *= (1.0f - 0.005f * dt * 1000.0f); // Slower decay
    pImpl->experienceStrength += neuromodulationFactor * 0.0005f * dt * 1000.0f; // Lower accumulation
    
    // Optimize connectivity based on experience
    optimizeConnectivityBasedOnExperience(brain, rng, dt);
}

void DevelopmentSystem::handleAdultDevelopment(Brain* brain, RandomGenerator& rng, 
                                              TimestepDuration dt, float neuromodulationFactor) {
    if (!brain) return;
    
    // Adult stage: maintenance, minimal changes
    float adultFactor = 0.3f; // Low plasticity
    
    // Limited synaptic turnover
    float adultSynaptogenesisRate = 0.02f * neuromodulationFactor * adultFactor;
    float adultPruningRate = 0.005f * neuromodulationFactor * adultFactor;
    
    // Maintain experienced-based connectivity patterns
    pImpl->activityDependency = 0.3f; // Low activity dependence
    pImpl->experienceStrength *= (1.0f - 0.001f * dt * 1000.0f); // Very slow decay
    pImpl->experienceStrength += neuromodulationFactor * 0.0001f * dt * 1000.0f; // Minimal accumulation
    
    // Fine-tune existing connections
    fineTuneConnections(brain, rng, dt, neuromodulationFactor);
}

void DevelopmentSystem::handleAgingDevelopment(Brain* brain, RandomGenerator& rng, 
                                              TimestepDuration dt, float neuromodulationFactor) {
    if (!brain) return;
    
    // Aging stage: degeneration, reduced plasticity and function
    float agingFactor = 0.1f; // Very low plasticity
    
    // Minimal synaptic turnover with increased pruning
    float agingSynaptogenesisRate = 0.01f * neuromodulationFactor * agingFactor;
    float agingPruningRate = 0.015f * neuromodulationFactor * agingFactor;
    
    // Reduce plasticity significantly
    pImpl->activityDependency = 0.1f; // Very low activity dependence
    pImpl->experienceStrength *= (1.0f - 0.0005f * dt * 1000.0f); // Slow decay
    pImpl->experienceStrength += neuromodulationFactor * 0.00005f * dt * 1000.0f; // Minimal accumulation
    
    // Allow some degenerative changes for realism
    if (rng.bernoulli(0.001f * dt * 1000.0f)) {
        // Simulate age-related degeneration (simplified)
        // In real implementation would affect neuron/synapse health
    }
}

void DevelopmentSystem::updateDevelopmentalLearning(Brain* brain, RandomGenerator& rng, 
                                                     TimestepDuration dt) {
    if (!brain) return;
    
    // Get reward prediction error from neuromodulators
    auto* neuromodulators = brain->getNeuromodulatorEnsemble();
    if (!neuromodulators) return;
    
    // Use dopamine's reward prediction error for developmental learning
    float rewardPredictionError = neuromodulators->getRewardPredictionError();
    
    // Update developmental learning based on RPE
    double learningUpdate = rewardPredictionError * pImpl->developmentalLearningRate * dt * 1000.0f;
    
    // Update experience strength based on developmental learning
    pImpl->experienceStrength += learningUpdate;
    
    // Update connection-specific maturation times
    updateConnectionMaturationTimes(brain, rng, dt);
}

void DevelopmentSystem::optimizeConnectivityBasedOnExperience(Brain* brain, RandomGenerator& rng, 
                                                              TimestepDuration dt) {
    if (!brain) return;
    
    // Calculate connectivity statistics
    size_t totalNeurons = brain->getTotalNeuronCount();
    size_t totalSynapses = brain->getTotalSynapseCount();
    
    if (totalNeurons == 0 || totalSynapses == 0) return;
    
    // Calculate current connectivity efficiency
    float currentEfficiency = calculateConnectivityEfficiency(brain);
    
    // Target efficiency based on experience strength
    float targetEfficiency = 0.7f + pImpl->experienceStrength * 0.3f;
    targetEfficiency = std::min(1.0f, std::max(0.5f, targetEfficiency));
    
    // Adjust connectivity if needed
    if (currentEfficiency < targetEfficiency) {
        // Need to strengthen connections
        strengthenWeakConnections(brain, rng, dt, (targetEfficiency - currentEfficiency));
    } else if (currentEfficiency > targetEfficiency * 1.2f) {
        // Need to prune excess connections
        pruneExcessiveConnections(brain, rng, dt, (currentEfficiency - targetEfficiency * 1.2f));
    }
}

void DevelopmentSystem::fineTuneConnections(Brain* brain, RandomGenerator& rng, 
                                           TimestepDuration dt, float neuromodulationFactor) {
    if (!brain) return;
    
    // Fine-tune existing connections based on experience
    size_t totalSynapses = brain->getTotalSynapseCount();
    
    // Sample synapses for fine-tuning
    size_t tuningSample = std::min(totalSynapses, static_cast<size_t>(100));
    
    for (size_t i = 0; i < tuningSample; ++i) {
        // Get random synapse
        size_t index = rng.pickRandomIndex(static_cast<uint64_t>(totalSynapses));
        
        // Apply small weight adjustment based on experience
        float experienceInfluence = pImpl->experienceStrength * neuromodulationFactor * 0.01f * dt * 1000.0f;
        
        // Get current synapse weight (placeholder)
        float currentWeight = 0.5f; // Would get actual weight
        
        // Apply fine-tuning adjustment
        float adjustment = (rng.randomBool() ? 1.0f : -1.0f) * experienceInfluence;
        float newWeight = currentWeight + adjustment;
        newWeight = std::max(0.0f, std::min(newWeight, 2.0f));
        
        // Would apply adjustment to synapse
    }
}

void DevelopmentSystem::updateConnectionMaturationTimes(Brain* brain, RandomGenerator& rng, 
                                                       TimestepDuration dt) {
    if (!brain) return;
    
    // Update connection-specific maturation times based on usage
    size_t totalSynapses = brain->getTotalSynapseCount();
    
    for (size_t i = 0; i < totalSynapses; ++i) {
        // Get synapse connection
        // Would access actual synapse
        
        // Update maturation time based on recent activity
        double maturationTime = pImpl->connectionMaturationTimes[synapseId];
        
        // Increase maturation time based on usage
        // In real implementation would track actual usage
        pImpl->connectionMaturationTimes[synapseId] = maturationTime + dt * 1000.0f;
    }
}

float DevelopmentSystem::calculateConnectivityEfficiency(Brain* brain) {
    if (!brain) return 0.0f;
    
    // Calculate simplified connectivity efficiency
    size_t totalNeurons = brain->getTotalNeuronCount();
    size_t totalSynapses = brain->getTotalSynapseCount();
    
    if (totalNeurons <= 1) return 0.0f;
    
    // Ideal connectivity density
    float idealDensity = 1.8f; // synapses per neuron
    float currentDensity = static_cast<float>(totalSynapses) / totalNeurons;
    
    // Calculate efficiency as deviation from ideal
    float efficiency = 1.0f - std::abs(currentDensity - idealDensity) / idealDensity;
    efficiency = std::max(0.0f, efficiency);
    
    // Boost efficiency based on experience
    efficiency *= (1.0f + pImpl->experienceStrength);
    efficiency = std::min(1.0f, efficiency);
    
    return efficiency;
}

void DevelopmentSystem::strengthenWeakConnections(Brain* brain, RandomGenerator& rng, 
                                                 TimestepDuration dt, float deficiency) {
    if (!brain) return;
    
    // Strengthen a subset of connections based on deficiency
    size_t totalSynapses = brain->getTotalSynapseCount();
    size_t synapsesToStrengthen = static_cast<size_t>(totalSynapses * deficiency * 0.1f);
    
    for (size_t i = 0; i < synapsesToStrengthen && i < totalSynapses; ++i) {
        // Get random synapse
        size_t index = rng.pickRandomIndex(static_cast<uint64_t>(totalSynapses));
        
        // Strengthen synapse (simplified)
        // Would increase synapse weight based on deficiency
    }
}

void DevelopmentSystem::pruneExcessiveConnections(Brain* brain, RandomGenerator& rng, 
                                                  TimestepDuration dt, float excess) {
    if (!brain) return;
    
    // Prune a subset of connections based on excess
    size_t totalSynapses = brain->getTotalSynapseCount();
    size_t synapsesToPrune = static_cast<size_t>(totalSynapses * excess * 0.05f);
    
    for (size_t i = 0; i < synapsesToPrune && i < totalSynapses; ++i) {
        // Get random synapse
        size_t index = rng.pickRandomIndex(static_cast<uint64_t>(totalSynapses));
        
        // Prune synapse (simplified)
        // Would remove synapse based on excess
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
    return pImpl->criticalPeriodActive;
}

float DevelopmentSystem::getCriticalPeriodProgress() const {
    if (!pImpl->criticalPeriodActive) {
        return 0.0f;
    }
    // Progress through critical period (0 to 1)
    double totalCriticalPeriodDuration = pImpl->criticalPeriodDuration;
    double progress = pImpl->stageAge / totalCriticalPeriodDuration;
    return std::min(1.0f, static_cast<float>(progress));
}

} // namespace nlm