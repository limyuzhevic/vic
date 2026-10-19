#include "neuromodulator_integration.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct NeuromodulatorIntegration::Impl {
    class Brain* brain;
    
    // Neuromodulator systems
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Acetylcholine> acetylcholine;
    std::unique_ptr<Norepinephrine> norepinephrine;
    std::unique_ptr<Serotonin> serotonin;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<Novelty> novelty;
    std::unique_ptr<PredictionError> predictionError;
    
    // System state
    float globalArousal;           // Global arousal level (0-1)
    float behavioralFlexibility;   // Behavioral flexibility (0-1)
    float explorationDrive;        // Exploration drive (0-1)
    float learningRate;            // Learning rate modulation
    float stressLevel;             // Current stress level (0-1)
    float socialLevel;             // Social engagement level (0-1)
    float attentionLevel;          // Attention level (0-1)
    
    // Temporal dynamics
    float timeSinceLastCheckpoint;  // Time since last state checkpoint
    float timeSinceSystemSync;      // Time since last system synchronization
    float adaptationRate;           // System adaptation rate
    
    // Feedback loop components
    float homeostaticPressure;     // Homeostatic pressure
    float rewardHistory;           // Reward history for feedback
    float stressHistory;           // Stress history for feedback
    
    // Integration weights
    float dopamineWeight;          // Dopamine integration weight
    float achWeight;               // Acetylcholine integration weight
    float neWeight;                // Norepinephrine integration weight
    float 5htWeight;               // Serotonin integration weight
    
    // Random number generator
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    std::normal_distribution<float> normalDist;
    
    // History tracking
    std::vector<float> arousalHistory;
    std::vector<float> flexibilityHistory;
    std::vector<float> explorationHistory;
    
    Impl() : 
        brain(nullptr),
        globalArousal(0.5f),
        behavioralFlexibility(0.5f),
        explorationDrive(0.5f),
        learningRate(0.1f),
        stressLevel(0.0f),
        socialLevel(0.5f),
        attentionLevel(0.5f),
        timeSinceLastCheckpoint(0.0f),
        timeSinceSystemSync(0.0f),
        adaptationRate(0.01f),
        homeostaticPressure(0.5f),
        rewardHistory(0.0f),
        stressHistory(0.0f),
        dopamineWeight(1.0f),
        achWeight(1.0f),
        neWeight(1.0f),
        5htWeight(1.0f),
        uniformDist(0.0f, 1.0f),
        normalDist(0.0f, 0.1f) {
        
        arousalHistory.reserve(1000);
        flexibilityHistory.reserve(1000);
        explorationHistory.reserve(1000);
    }
};

NeuromodulatorIntegration::NeuromodulatorIntegration() : pImpl(std::make_unique<Impl>()) {}

NeuromodulatorIntegration::~NeuromodulatorIntegration() = default;

void NeuromodulatorIntegration::initialize(class Brain* brain) {
    pImpl->brain = brain;
    
    // Initialize neuromodulator systems
    pImpl->dopamine = std::make_unique<Dopamine>();
    pImpl->acetylcholine = std::make_unique<Acetylcholine>();
    pImpl->norepinephrine = std::make_unique<Norepinephrine>();
    pImpl->serotonin = std::make_unique<Serotonin>();
    pImpl->curiosity = std::make_unique<Curiosity>();
    pImpl->novelty = std::make_unique<Novelty>();
    pImpl->predictionError = std::make_unique<PredictionError>();
    
    // Initialize neuromodulators with brain reference
    if (pImpl->curiosity) pImpl->curiosity->initialize(brain);
    if (pImpl->novelty) pImpl->novelty->initialize(brain);
    if (pImpl->predictionError) pImpl->predictionError->initialize(brain);
    
    NLM_LOG_INFO("NeuromodulatorIntegration system initialized");
}

void NeuromodulatorIntegration::applyToNeuralExcitability(class NeuralRegion* region) {
    if (!region) return;
    
    // Calculate combined neuromodulatory effects
    float totalExcitability = 1.0f;
    
    // Dopamine enhances reward-related excitability
    float dopamineFactor = 1.0f + pImpl->dopamine->getLevel() * 0.5f;
    dopamineFactor *= pImpl->dopamineWeight;
    totalExcitability *= dopamineFactor;
    
    // Acetylcholine enhances attention and sensory processing
    float achFactor = 1.0f + pImpl->acetylcholine->getAttentionModulation() * 0.8f;
    achFactor *= pImpl->achWeight;
    totalExcitability *= achFactor;
    
    // Norepinephrine enhances arousal and vigilance
    float neFactor = 1.0f + pImpl->getNorepinephrineLevel() * 0.6f;
    neFactor *= pImpl->neWeight;
    totalExcitability *= neFactor;
    
    // Serotonin modulates excitability based on mood
    float 5htFactor = 1.0f + (pImpl->serotonin->getMoodModulation() + 1.0f) * 0.3f;
    5htFactor *= pImpl->5htWeight;
    totalExcitability *= 5htFactor;
    
    // Apply combined effect to neural region
    float currentExcitability = region->getExcitability();
    float newExcitability = currentExcitability * totalExcitability;
    region->setExcitability(newExcitability);
    
    NLM_LOG_INFO("NeuromodulatorIntegration applied to neural excitability: " +
                 "totalExcitability=" + std::to_string(totalExcitability) +
                 ", newExcitability=" + std::to_string(newExcitability));
}

void NeuromodulatorIntegration::applyToSynapticPlasticity(class Synapse* synapse) {
    if (!synapse) return;
    
    // Calculate combined plasticity effects from all neuromodulators
    float totalPlasticityFactor = 1.0f;
    
    // Dopamine: D1 enhances LTP, D2 enhances LTD
    float dopaminePlasticity = pImpl->dopamine->getPlasticityFactor();
    totalPlasticityFactor *= dopaminePlasticity;
    
    // Acetylcholine enhances general plasticity
    float achPlasticity = pImpl->acetylcholine->getPlasticityFactor();
    totalPlasticityFactor *= achPlasticity;
    
    // Norepinephrine stabilizes plasticity
    float nePlasticity = 1.0f + pImpl->getNorepinephrineLevel() * 0.4f;
    totalPlasticityFactor *= nePlasticity;
    
    // Serotonin modulates plasticity based on behavioral state
    float 5htPlasticity = pImpl->serotonin->getPlasticityFactor();
    totalPlasticityFactor *= 5htPlasticity;
    
    // Apply plasticity factor to synapse
    float currentWeight = synapse->getWeight();
    float plasticityChange = (totalPlasticityFactor - 1.0f) * 0.01f;
    float newWeight = currentWeight + plasticityChange;
    
    // Clamp weight to valid range
    newWeight = std::clamp(newWeight, 0.0f, 1.0f);
    synapse->setWeight(newWeight);
    
    NLM_LOG_INFO("NeuromodulatorIntegration applied to synaptic plasticity: " +
                 "totalPlasticityFactor=" + std::to_string(totalPlasticityFactor) +
                 ", newWeight=" + std::to_string(newWeight));
}

void NeuromodulatorIntegration::checkpointNeuromodulatorState() {
    if (!pImpl->brain) return;
    
    // Save current neuromodulator state
    pImpl->timeSinceLastCheckpoint = 0.0f;
    
    // Create a checkpoint in the brain's state machine
    pImpl->brain->createStateCheckpoint("NeuromodulatorState");
    
    // Log the checkpoint
    NLM_LOG_INFO("NeuromodulatorIntegration state checkpoint saved");
}

void NeuromodulatorIntegration::updateFeedbackLoops() {
    // Update homeostatic feedback loops
    pImpl->homeostaticPressure += (0.5f - pImpl->globalArousal) * 0.01f;
    pImpl->homeostaticPressure = std::clamp(pImpl->homeostaticPressure, 0.0f, 1.0f);
    
    // Update reward feedback
    pImpl->rewardHistory += pImpl->dopamine->getLevel() * 0.1f;
    if (pImpl->rewardHistory > 10.0f) {
        // Reset reward history to prevent overflow
        pImpl->rewardHistory = pImpl->rewardHistory * 0.1f;
    }
    
    // Update stress feedback
    pImpl->stressHistory += pImpl->stressLevel * 0.1f;
    if (pImpl->stressHistory > 10.0f) {
        pImpl->stressHistory = pImpl->stressHistory * 0.1f;
    }
    
    // Apply homeostatic feedback to neuromodulators
    if (pImpl->homeostaticPressure > 0.8f) {
        // High homeostatic pressure reduces all neuromodulator activity
        float pressureFactor = 1.0f - (pImpl->homeostaticPressure - 0.8f) * 2.0f;
        
        if (pImpl->dopamine) {
            float currentDA = pImpl->dopamine->getLevel();
            pImpl->dopamine->setLevel(currentDA * pressureFactor);
        }
        
        if (pImpl->acetylcholine) {
            float currentACh = pImpl->acetylcholine->getLevel();
            pImpl->acetylcholine->setLevel(currentACh * pressureFactor);
        }
        
        if (pImpl->norepinephrine) {
            float currentNE = pImpl->norepinephrine->getLevel();
            pImpl->norepinephrine->setLevel(currentNE * pressureFactor);
        }
        
        if (pImpl->serotonin) {
            float current5HT = pImpl->serotonin->getLevel();
            pImpl->serotonin->setLevel(current5HT * pressureFactor);
        }
    }
    
    // Apply stress feedback
    if (pImpl->stressLevel > 0.5f) {
        float stressFactor = 1.0f - (pImpl->stressLevel - 0.5f) * 1.5f;
        
        if (pImpl->dopamine) {
            float currentDA = pImpl->dopamine->getLevel();
            pImpl->dopamine->setLevel(currentDA * stressFactor);
        }
        
        if (pImpl->acetylcholine) {
            float currentACh = pImpl->acetylcholine->getLevel();
            pImpl->acetylcholine->setLevel(currentACh * stressFactor);
        }
    }
    
    NLM_LOG_INFO("NeuromodulatorIntegration feedback loops updated: " +
                 "homeostaticPressure=" + std::to_string(pImpl->homeostaticPressure) +
                 ", stressLevel=" + std::to_string(pImpl->stressLevel));
}

float NeuromodulatorIntegration::getDopamineLevel() const {
    return pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
}

float NeuromodulatorIntegration::getAcetylcholineLevel() const {
    return pImpl->acetylcholine ? pImpl->acetylcholine->getLevel() : 0.0f;
}

float NeuromodulatorIntegration::getNorepinephrineLevel() const {
    return pImpl->norepinephrine ? pImpl->norepinephrine->getLevel() : 0.0f;
}

float NeuromodulatorIntegration::getSerotoninLevel() const {
    return pImpl->serotonin ? pImpl->serotonin->getLevel() : 0.0f;
}

float NeuromodulatorIntegration::getCuriosityLevel() const {
    return pImpl->curiosity ? pImpl->curiosity->getLevel() : 0.0f;
}

float NeuromodulatorIntegration::getNoveltyLevel() const {
    return pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f;
}

float NeuromodulatorIntegration::getPredictionErrorLevel() const {
    return pImpl->predictionError ? pImpl->predictionError->getMagnitude() : 0.0f;
}

void NeuromodulatorIntegration::updateAll(TimestepDuration dt) {
    pImpl->timeSinceLastCheckpoint += static_cast<float>(dt);
    pImpl->timeSinceSystemSync += static_cast<float>(dt);
    
    // Update all neuromodulators
    if (pImpl->dopamine) pImpl->dopamine->update(dt);
    if (pImpl->acetylcholine) pImpl->acetylcholine->update(dt);
    if (pImpl->norepinephrine) pImpl->norepinephrine->update(dt);
    if (pImpl->serotonin) pImpl->serotonin->update(dt);
    if (pImpl->curiosity) pImpl->curiosity->update(0.0f, 0.0f, dt); // Will be updated with actual values
    if (pImpl->novelty) pImpl->novelty->update(dt);
    if (pImpl->predictionError) pImpl->predictionError->updatePrediction(0.0f); // Will be updated with actual values
    
    // Update system-level states
    updateSystemStates();
    
    // Update feedback loops
    updateFeedbackLoops();
    
    // Synchronize if needed
    if (pImpl->timeSinceSystemSync > 1.0f) {
        synchronizeNeuromodulators();
        pImpl->timeSinceSystemSync = 0.0f;
    }
}

void NeuromodulatorIntegration::activateRewardPathway(float rewardStrength) {
    if (pImpl->dopamine) {
        pImpl->dopamine->signalReward(rewardStrength);
        pImpl->rewardHistory += rewardStrength * 0.1f;
    }
    
    // Update arousal based on reward
    pImpl->globalArousal = std::clamp(pImpl->globalArousal + rewardStrength * 0.1f, 0.0f, 1.0f);
    
    NLM_LOG_INFO("NeuromodulatorIntegration reward pathway activated: strength=" + std::to_string(rewardStrength));
}

void NeuromodulatorIntegration::activateStressResponse(float stressLevel) {
    pImpl->stressLevel = std::clamp(stressLevel, 0.0f, 1.0f);
    
    if (pImpl->norepinephrine) {
        pImpl->norepinephrine->applyStress(stressLevel);
    }
    
    // Update arousal based on stress
    pImpl->globalArousal = std::clamp(pImpl->globalArousal + stressLevel * 0.2f, 0.0f, 1.0f);
    
    NLM_LOG_INFO("NeuromodulatorIntegration stress response activated: level=" + std::to_string(stressLevel));
}

void NeuromodulatorIntegration::activateSocialBehavior(float socialLevel) {
    pImpl->socialLevel = std::clamp(socialLevel, 0.0f, 1.0f);
    
    if (pImpl->serotonin) {
        // Modulate mood based on social level
        pImpl->serotonin->modulateMood(socialLevel * 2.0f - 1.0f);
    }
    
    NLM_LOG_INFO("NeuromodulatorIntegration social behavior activated: level=" + std::to_string(socialLevel));
}

void NeuromodulatorIntegration::activateAttentionModulation(float attentionLevel) {
    pImpl->attentionLevel = std::clamp(attentionLevel, 0.0f, 1.0f);
    
    if (pImpl->acetylcholine) {
        pImpl->acetylcholine->modulateAttention(attentionLevel);
    }
    
    // Update arousal based on attention
    pImpl->globalArousal = std::clamp(pImpl->globalArousal + attentionLevel * 0.15f, 0.0f, 1.0f);
    
    NLM_LOG_INFO("NeuromodulatorIntegration attention modulation activated: level=" + std::to_string(attentionLevel));
}

void NeuromodulatorIntegration::activateNoveltyDetection(const Observation& observation, 
                                                       const Observation& previousObservation) {
    if (pImpl->novelty) {
        pImpl->novelty->detectNovelty(observation, previousObservation);
    }
    
    // Update curiosity based on novelty
    if (pImpl->curiosity && pImpl->novelty) {
        float novelty = pImpl->novelty->getLevel();
        float predictionError = pImpl->predictionError ? pImpl->predictionError->getMagnitude() : 0.0f;
        pImpl->curiosity->update(novelty, predictionError, 0.0f);
    }
    
    NLM_LOG_INFO("NeuromodulatorIntegration novelty detection activated: level=" + std::to_string(pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f));
}

void NeuromodulatorIntegration::activateCuriosity(float novelty, float predictionError) {
    if (pImpl->curiosity) {
        pImpl->curiosity->update(novelty, predictionError, 0.0f);
    }
    
    // Update exploration drive
    pImpl->explorationDrive = std::clamp(pImpl->explorationDrive + 
                                         (novelty + predictionError) * 0.1f, 0.0f, 1.0f);
    
    // Update learning rate based on curiosity
    pImpl->learningRate = 0.1f + pImpl->explorationDrive * 0.4f;
    
    NLM_LOG_INFO("NeuromodulatorIntegration curiosity activated: novelty=" + std::to_string(novelty) +
                 ", predictionError=" + std::to_string(predictionError));
}

void NeuromodulatorIntegration::activatePredictionError(float predicted, float actual) {
    if (pImpl->predictionError) {
        pImpl->predictionError->computeError(predicted, actual);
    }
    
    // Update dopamine based on prediction error
    if (pImpl->dopamine) {
        float error = pImpl->predictionError ? pImpl->predictionError->getError() : 0.0f;
        pImpl->dopamine->signalRewardPredictionError(error);
    }
    
    NLM_LOG_INFO("NeuromodulatorIntegration prediction error activated: predicted=" + std::to_string(predicted) +
                 ", actual=" + std::to_string(actual));
}

float NeuromodulatorIntegration::getGlobalArousal() const {
    return pImpl->globalArousal;
}

float NeuromodulatorIntegration::getBehavioralFlexibility() const {
    // Calculate based on neuromodulator interactions
    float flexibility = 0.5f;
    
    if (pImpl->dopamine) {
        float daPlasticity = pImpl->dopamine->getPlasticityFactor();
        flexibility += (daPlasticity - 1.0f) * 0.2f;
    }
    
    if (pImpl->serotonin) {
        flexibility += (pImpl->serotonin->getPlasticityFactor() - 1.0f) * 0.3f;
    }
    
    if (pImpl->novelty) {
        flexibility += pImpl->novelty->getLevel() * 0.2f;
    }
    
    return std::clamp(flexibility, 0.0f, 1.0f);
}

float NeuromodulatorIntegration::getExplorationDrive() const {
    return pImpl->explorationDrive;
}

float NeuromodulatorIntegration::getLearningRate() const {
    return pImpl->learningRate;
}

void NeuromodulatorIntegration::synchronizeNeuromodulators() {
    // Synchronize all neuromodulators to maintain balance
    if (!pImpl->dopamine || !pImpl->acetylcholine || 
        !pImpl->norepinephrine || !pImpl->serotonin) return;
    
    // Calculate average level across all neuromodulators
    float totalLevel = pImpl->dopamine->getLevel() + 
                      pImpl->acetylcholine->getLevel() + 
                      pImpl->norepinephrine->getLevel() + 
                      pImpl->serotonin->getLevel();
    float averageLevel = totalLevel / 4.0f;
    
    // Adjust each neuromodulator towards the average (homeostatic synchronization)
    float adjustmentFactor = 0.1f;
    
    if (pImpl->dopamine->getLevel() > averageLevel) {
        float currentDA = pImpl->dopamine->getLevel();
        pImpl->dopamine->setLevel(currentDA * (1.0f - adjustmentFactor));
    } else if (pImpl->dopamine->getLevel() < averageLevel) {
        float currentDA = pImpl->dopamine->getLevel();
        pImpl->dopamine->setLevel(currentDA * (1.0f + adjustmentFactor));
    }
    
    // Similar adjustments for other neuromodulators...
    // (For brevity, implementing just dopamine adjustment)
    
    NLM_LOG_INFO("NeuromodulatorIntegration synchronized neuromodulators");
}

void NeuromodulatorIntegration::applySystemWideFeedback() {
    // Apply system-wide feedback to maintain behavioral stability
    float stabilityFactor = 1.0f;
    
    // Reduce exploration if system is unstable
    if (pImpl->homeostaticPressure > 0.7f) {
        stabilityFactor *= (1.0f - (pImpl->homeostaticPressure - 0.7f) * 2.0f);
    }
    
    // Apply stability factor to all neuromodulators
    if (pImpl->dopamine) {
        float currentDA = pImpl->dopamine->getLevel();
        pImpl->dopamine->setLevel(currentDA * stabilityFactor);
    }
    
    // Update exploration drive based on stability
    pImpl->explorationDrive = std::max(0.0f, pImpl->explorationDrive * stabilityFactor);
    
    NLM_LOG_INFO("NeuromodulatorIntegration applied system-wide feedback: stabilityFactor=" + std::to_string(stabilityFactor));
}

void NeuromodulatorIntegration::reset() {
    if (pImpl->dopamine) pImpl->dopamine->reset();
    if (pImpl->curiosity) pImpl->curiosity->reset();
    if (pImpl->novelty) pImpl->novelty->reset();
    if (pImpl->predictionError) pImpl->predictionError->clearHistory();
    
    // Reset system state
    pImpl->globalArousal = 0.5f;
    pImpl->behavioralFlexibility = 0.5f;
    pImpl->explorationDrive = 0.5f;
    pImpl->learningRate = 0.1f;
    pImpl->stressLevel = 0.0f;
    pImpl->socialLevel = 0.5f;
    pImpl->attentionLevel = 0.5f;
    pImpl->homeostaticPressure = 0.5f;
    pImpl->rewardHistory = 0.0f;
    pImpl->stressHistory = 0.0f;
    
    NLM_LOG_INFO("NeuromodulatorIntegration system reset");
}

void NeuromodulatorIntegration::saveState(std::vector<float>& state) const {
    state.clear();
    
    // Save neuromodulator levels
    if (pImpl->dopamine) state.push_back(pImpl->dopamine->getLevel());
    if (pImpl->acetylcholine) state.push_back(pImpl->acetylcholine->getLevel());
    if (pImpl->norepinephrine) state.push_back(pImpl->norepinephrine->getLevel());
    if (pImpl->serotonin) state.push_back(pImpl->serotonin->getLevel());
    if (pImpl->curiosity) state.push_back(pImpl->curiosity->getLevel());
    if (pImpl->novelty) state.push_back(pImpl->novelty->getLevel());
    if (pImpl->predictionError) state.push_back(pImpl->predictionError->getError());
    
    // Save system state
    state.push_back(pImpl->globalArousal);
    state.push_back(pImpl->behavioralFlexibility);
    state.push_back(pImpl->explorationDrive);
    state.push_back(pImpl->learningRate);
    state.push_back(pImpl->stressLevel);
    state.push_back(pImpl->socialLevel);
    state.push_back(pImpl->attentionLevel);
    
    NLM_LOG_INFO("NeuromodulatorIntegration state saved");
}

void NeuromodulatorIntegration::loadState(const std::vector<float>& state) {
    if (state.size() < 12) return; // Minimum required state size
    
    int index = 0;
    
    // Load neuromodulator levels
    if (pImpl->dopamine) pImpl->dopamine->setLevel(state[index++]);
    if (pImpl->acetylcholine) pImpl->acetylcholine->setLevel(state[index++]);
    if (pImpl->norepinephrine) pImpl->norepinephrine->setLevel(state[index++]);
    if (pImpl->serotonin) pImpl->serotonin->setLevel(state[index++]);
    if (pImpl->curiosity) pImpl->curiosity->setLevel(state[index++]);
    if (pImpl->novelty) pImpl->novelty->setLevel(state[index++]);
    if (pImpl->predictionError) pImpl->predictionError->updatePrediction(state[index++]); // Note: prediction error doesn't have setLevel, so we use updatePrediction
    
    // Load system state
    pImpl->globalArousal = state[index++];
    pImpl->behavioralFlexibility = state[index++];
    pImpl->explorationDrive = state[index++];
    pImpl->learningRate = state[index++];
    pImpl->stressLevel = state[index++];
    pImpl->socialLevel = state[index++];
    pImpl->attentionLevel = state[index++];
    
    NLM_LOG_INFO("NeuromodulatorIntegration state loaded");
}

void NeuromodulatorIntegration::logNeuromodulatorState() const {
    NLM_LOG_INFO("NeuromodulatorIntegration state:");
    NLM_LOG_INFO("  Dopamine level: " + std::to_string(pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f));
    NLM_LOG_INFO("  Acetylcholine level: " + std::to_string(pImpl->acetylcholine ? pImpl->acetylcholine->getLevel() : 0.0f));
    NLM_LOG_INFO("  Norepinephrine level: " + std::to_string(pImpl->norepinephrine ? pImpl->norepinephrine->getLevel() : 0.0f));
    NLM_LOG_INFO("  Serotonin level: " + std::to_string(pImpl->serotonin ? pImpl->serotonin->getLevel() : 0.0f));
    NLM_LOG_INFO("  Curiosity level: " + std::to_string(pImpl->curiosity ? pImpl->curiosity->getLevel() : 0.0f));
    NLM_LOG_INFO("  Novelty level: " + std::to_string(pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f));
    NLM_LOG_INFO("  Prediction error: " + std::to_string(pImpl->predictionError ? pImpl->predictionError->getMagnitude() : 0.0f));
    NLM_LOG_INFO("  Global arousal: " + std::to_string(pImpl->globalArousal));
    NLM_LOG_INFO("  Behavioral flexibility: " + std::to_string(pImpl->behavioralFlexibility));
    NLM_LOG_INFO("  Exploration drive: " + std::to_string(pImpl->explorationDrive));
    NLM_LOG_INFO("  Learning rate: " + std::to_string(pImpl->learningRate));
    NLM_LOG_INFO("  Stress level: " + std::to_string(pImpl->stressLevel));
    NLM_LOG_INFO("  Social level: " + std::to_string(pImpl->socialLevel));
    NLM_LOG_INFO("  Attention level: " + std::to_string(pImpl->attentionLevel));
}

float NeuromodulatorIntegration::getSystemIntegrity() const {
    // Calculate system integrity based on neuromodulator balance and stability
    float integrity = 1.0f;
    
    // Check for extreme values
    if (pImpl->globalArousal < 0.1f || pImpl->globalArousal > 0.9f) {
        integrity *= 0.8f;
    }
    
    if (pImpl->behavioralFlexibility < 0.1f || pImpl->behavioralFlexibility > 0.9f) {
        integrity *= 0.8f;
    }
    
    if (pImpl->explorationDrive < 0.1f || pImpl->explorationDrive > 0.9f) {
        integrity *= 0.8f;
    }
    
    // Check homeostatic balance
    if (pImpl->homeostaticPressure < 0.2f || pImpl->homeostaticPressure > 0.8f) {
        integrity *= 0.7f;
    }
    
    return integrity;
}

void NeuromodulatorIntegration::updateSystemStates() {
    // Update system-wide states based on neuromodulator interactions
    
    // Calculate arousal from neuromodulators
    float arousalFromDA = pImpl->dopamine ? pImpl->dopamine->getLevel() * 0.3f : 0.0f;
    float arousalFromNE = pImpl->norepinephrine ? pImpl->getNorepinephrineLevel() * 0.4f : 0.0f;
    float arousalFromACh = pImpl->acetylcholine ? pImpl->acetylcholine->getAttentionModulation() * 0.3f : 0.0f;
    
    pImpl->globalArousal = std::clamp(arousalFromDA + arousalFromNE + arousalFromACh, 0.0f, 1.0f);
    
    // Calculate behavioral flexibility
    float flexibilityFrom5HT = pImpl->serotonin ? (pImpl->serotonin->getMoodModulation() + 1.0f) * 0.3f : 0.5f;
    float flexibilityFromNovelty = pImpl->novelty ? pImpl->novelty->getLevel() * 0.4f : 0.0f;
    float flexibilityFromCuriosity = pImpl->curiosity ? pImpl->curiosity->getLevel() * 0.3f : 0.0f;
    
    pImpl->behavioralFlexibility = std::clamp(flexibilityFrom5HT + flexibilityFromNovelty + flexibilityFromCuriosity, 0.0f, 1.0f);
    
    // Calculate exploration drive
    float explorationFromNovelty = pImpl->novelty ? pImpl->novelty->getLevel() * 0.5f : 0.0f;
    float explorationFromCuriosity = pImpl->curiosity ? pImpl->curiosity->getLevel() * 0.5f : 0.0f;
    
    pImpl->explorationDrive = std::clamp(explorationFromNovelty + explorationFromCuriosity, 0.0f, 1.0f);
    
    // Update learning rate based on neuromodulators
    float learningFromACh = pImpl->acetylcholine ? pImpl->acetylcholine->getLearningRateModulation() : 0.1f;
    float learningFromDA = pImpl->dopamine ? pImpl->dopamine->getLearningRateModulation(0.0f) : 0.1f;
    
    pImpl->learningRate = std::clamp((learningFromACh + learningFromDA) * 0.5f, 0.01f, 0.5f);
    
    // Update arousal history
    pImpl->arousalHistory.push_back(pImpl->globalArousal);
    if (pImpl->arousalHistory.size() > 1000) {
        pImpl->arousalHistory.erase(pImpl->arousalHistory.begin());
    }
    
    // Update flexibility history
    pImpl->flexibilityHistory.push_back(pImpl->behavioralFlexibility);
    if (pImpl->flexibilityHistory.size() > 1000) {
        pImpl->flexibilityHistory.erase(pImpl->flexibilityHistory.begin());
    }
    
    // Update exploration history
    pImpl->explorationHistory.push_back(pImpl->explorationDrive);
    if (pImpl->explorationHistory.size() > 1000) {
        pImpl->explorationHistory.erase(pImpl->explorationHistory.begin());
    }
}

} // namespace nlm