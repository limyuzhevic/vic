#include "Dopamine.hpp"
#include "Neuromodulator.hpp"
#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"
#include "../plasticity/STDP.hpp"
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <chrono>
#include <random>
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Dopamine::Impl {
    // Dopamine concentration dynamics
    float totalLevel;                    // Total dopamine level (phasic + tonic)
    float phasicComponent;              // Fast phasic burst component
    float tonicComponent;                // Slow tonic baseline component
    float predictionError;               // Current reward prediction error
    float valueFunction;                 // Estimated value of current state
    
    // Temporal dynamics parameters
    float phasicGain;                    // Scaling factor for phasic bursts
    float tonicBaseline;                 // Tonic baseline level
    float phasicTimeConstant;            // Time constant for phasic decay (seconds)
    float tonicTimeConstant;             // Time constant for tonic decay (seconds)
    float adaptationRate;                // Rate of adaptation to environment changes
    float decayRate;                     // General decay rate
    
    // Reinforcement learning parameters
    float learningRate;                  // TD learning rate (α)
    float discountFactor;                // Discount factor (γ)
    float explorationBonus;              // Exploration-exploitation balance
    float rewardPredictionHorizon;       // How far ahead predictions extend
    
    // Plasticity modulation parameters
    float ltpModulation;                 // Modulation of LTP by dopamine
    float ltdModulation;                 // Modulation of LTD by dopamine
    float plasticityThreshold;           // Minimum prediction error for plasticity
    float eligibilityTraceDecay;         // Decay rate of eligibility traces
    
    // State tracking
    float accumulatedRewards;            // Total rewards received
    float accumulatedPredictionErrors;   // Total prediction errors
    float lastObservedReward;            // Last observed reward
    float lastPredictedReward;           // Last predicted reward
    SimulationStep lastUpdateStep;       // Last update step
    
    // History and statistics
    std::vector<float> rewardHistory;    // Recent reward history
    std::vector<float> predictionHistory; // Recent prediction history
    std::vector<float> errorHistory;     // Recent error history
    
    // Learning statistics
    LearningStats stats;
    
    // Configuration parameters
    std::unordered_map<std::string, float> parameters;
    
    // Random number generation for exploration
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    
    // Timing and temporal dynamics
    Timestamp currentTime;
    Timestamp lastRPETime;
    std::vector<Timestamp> rpeTimestamps;
    
    // Initialize default parameters
    Impl() : rng(std::chrono::system_clock::now().time_since_epoch().count()),
             uniformDist(0.0f, 1.0f) {
        
        // Initialize levels
        totalLevel = 0.05f;              // Baseline dopamine level
        phasicComponent = 0.0f;          // Start with no phasic component
        tonicComponent = 0.05f;          // Tonic baseline
        predictionError = 0.0f;          // No prediction error initially
        valueFunction = 0.0f;            // Zero value estimate initially
        
        // Temporal dynamics parameters (biologically plausible values)
        phasicGain = 2.0f;               // Strong phasic response amplification
        tonicBaseline = 0.05f;           // Tonic baseline (~50 nM)
        phasicTimeConstant = 0.5f;       // Fast phasic decay (~500ms)
        tonicTimeConstant = 300.0f;      // Slow tonic decay (~5 minutes)
        adaptationRate = 0.1f;           // Moderate adaptation rate
        decayRate = 0.05f;               // General decay rate
        
        // Reinforcement learning parameters
        learningRate = 0.1f;             // Standard TD learning rate
        discountFactor = 0.9f;           // Discount factor for future rewards
        explorationBonus = 0.1f;         // Exploration bonus weight
        rewardPredictionHorizon = 10.0f;  // 10 second prediction horizon
        
        // Plasticity modulation parameters
        ltpModulation = 1.2f;             // 20% enhancement of LTP
        ltdModulation = 0.8f;             // 20% reduction of LTD
        plasticityThreshold = 0.01f;      // Minimum error for plasticity
        eligibilityTraceDecay = 0.95f;    // Slow eligibility trace decay
        
        // State tracking
        accumulatedRewards = 0.0f;
        accumulatedPredictionErrors = 0.0f;
        lastObservedReward = 0.0f;
        lastPredictedReward = 0.0f;
        lastUpdateStep = 0;
        
        // Initialize statistics
        stats.totalRewards = 0.0f;
        stats.totalPredictionErrors = 0.0f;
        stats.learningRate = learningRate;
        stats.predictionAccuracy = 0.0f;
        stats.explorationBonus = explorationBonus;
        stats.exploitationValue = 0.0f;
        
        // Set up default parameters
        parameters["phasic_gain"] = phasicGain;
        parameters["tonic_baseline"] = tonicBaseline;
        parameters["phasic_tau"] = phasicTimeConstant;
        parameters["tonic_tau"] = tonicTimeConstant;
        parameters["learning_rate"] = learningRate;
        parameters["discount_factor"] = discountFactor;
        parameters["plasticity_threshold"] = plasticityThreshold;
        parameters["eligibility_decay"] = eligibilityTraceDecay;
        
        // Record initial state
        currentTime = 0.0;
        lastRPETime = 0.0;
        rpeTimestamps.push_back(0.0);
        
        // Add initial state to history
        rewardHistory.push_back(0.0f);
        predictionHistory.push_back(0.0f);
        errorHistory.push_back(0.0f);
        
        NLM_LOG_INFO("Dopamine system initialized with biologically realistic parameters");
    }
};

Dopamine::Dopamine() : pImpl(std::make_unique<Impl>()) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->totalLevel;
}

void Dopamine::setLevel(float level) {
    pImpl->totalLevel = std::clamp(level, 0.0f, 10.0f);
    // Update components proportionally
    pImpl->phasicComponent = pImpl->totalLevel * 0.2f; // 20% phasic
    pImpl->tonicComponent = pImpl->totalLevel * 0.8f;  // 80% tonic
}

float Dopamine::getPlasticityFactor() const {
    // Dopamine modulates plasticity based on prediction error
    // High RPE (surprise) increases plasticity, low RPE decreases it
    float rpe = std::abs(pImpl->predictionError);
    float basePlasticity = 1.0f;
    
    // RPE-dependent modulation: high surprise increases plasticity
    if (rpe > pImpl->plasticityThreshold) {
        return basePlasticity * (1.0f + pImpl->learningRate * rpe);
    }
    return basePlasticity * (1.0f - pImpl->learningRate * rpe * 0.5f);
}

void Dopamine::update(TimestepDuration dt) {
    if (dt <= 0.0) return;
    
    pImpl->currentTime += static_cast<Timestamp>(dt);
    
    // Update phasic component (fast dynamics)
    if (pImpl->phasicComponent > 0.0f) {
        float phasicDecay = std::exp(-static_cast<float>(dt) / pImpl->phasicTimeConstant);
        pImpl->phasicComponent *= phasicDecay;
    }
    
    // Update tonic component (slow dynamics)
    float tonicDecay = std::exp(-static_cast<float>(dt) / pImpl->tonicTimeConstant);
    pImpl->tonicComponent *= tonicDecay;
    
    // Restore tonic baseline
    pImpl->tonicComponent += pImpl->tonicBaseline * (1.0f - tonicDecay);
    
    // Adapt baseline based on accumulated rewards/errors
    pImpl->tonicBaseline += pImpl->adaptationRate * 
        (pImpl->accumulatedRewards / (pImpl->accumulatedPredictionErrors + 1e-6f) - pImpl->tonicBaseline);
    
    // Update total level
    pImpl->totalLevel = pImpl->phasicComponent + pImpl->tonicComponent;
    
    // Decay eligibility traces in connected synapses
    // Note: This would need access to the synapse system
    // For now, just log the update
    NLM_LOG_DEBUG("Dopamine updated: phasic=" + std::to_string(pImpl->phasicComponent) + 
                  " tonic=" + std::to_string(pImpl->tonicComponent) + 
                  " total=" + std::to_string(pImpl->totalLevel));
}

void Dopamine::signalReward(float reward, float predictedReward) {
    // Compute reward prediction error (RPE)
    float rpe = reward - predictedReward;
    
    // Update value prediction using temporal difference learning
    if (pImpl->valueFunction > 0.0f) {
        pImpl->valueFunction += pImpl->learningRate * rpe;
    } else {
        pImpl->valueFunction = reward * pImpl->learningRate;
    }
    
    // Update statistics
    pImpl->stats.totalRewards += reward;
    pImpl->stats.totalPredictionErrors += std::abs(rpe);
    pImpl->lastObservedReward = reward;
    pImpl->lastPredictedReward = predictedReward;
    
    // Record history
    pImpl->rewardHistory.push_back(reward);
    pImpl->predictionHistory.push_back(predictedReward);
    pImpl->errorHistory.push_back(rpe);
    
    // Trim history to maintain size
    const size_t maxHistorySize = 1000;
    if (pImpl->rewardHistory.size() > maxHistorySize) {
        pImpl->rewardHistory.erase(pImpl->rewardHistory.begin());
        pImpl->predictionHistory.erase(pImpl->predictionHistory.begin());
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    
    // Signal prediction error
    signalPredictionError(rpe);
    
    // Update accumulated values
    pImpl->accumulatedRewards += reward;
    pImpl->accumulatedPredictionErrors += std::abs(rpe);
    
    // Compute exploration-exploitation balance
    float predictionAccuracy = 1.0f - std::abs(rpe) / std::max(std::abs(predictedReward), 1.0f);
    pImpl->stats.predictionAccuracy = predictionAccuracy;
    
    NLM_LOG_DEBUG("Dopamine reward signaling: reward=" + std::to_string(reward) + 
                  " predicted=" + std::to_string(predictedReward) + 
                  " RPE=" + std::to_string(rpe));
}

void Dopamine::signalImmediateReward(float reward) {
    // Simple phasic response to immediate reward
    float burstStrength = std::min(reward * pImpl->phasicGain, 1.0f);
    pImpl->phasicComponent = burstStrength;
    
    // Update value function
    pImpl->valueFunction = reward;
    
    // Update statistics
    pImpl->stats.totalRewards += reward;
    pImpl->lastObservedReward = reward;
    
    NLM_LOG_DEBUG("Dopamine immediate reward: reward=" + std::to_string(reward) + 
                  " burst_strength=" + std::to_string(burstStrength));
}

void Dopamine::signalPredictionError(float error) {
    pImpl->predictionError = error;
    
    // Record RPE timestamp
    pImpl->lastRPETime = pImpl->currentTime;
    pImpl->rpeTimestamps.push_back(pImpl->lastRPETime);
    
    // Maintain RPE history size
    const size_t maxRPEHistory = 100;
    if (pImpl->rpeTimestamps.size() > maxRPEHistory) {
        pImpl->rpeTimestamps.erase(pImpl->rpeTimestamps.begin());
    }
    
    NLM_LOG_DEBUG("Dopamine prediction error: error=" + std::to_string(error));
}

void Dopamine::updateValuePrediction(float currentValue) {
    // Temporal difference learning update
    float tdError = currentValue - pImpl->valueFunction;
    pImpl->valueFunction += pImpl->learningRate * tdError;
    
    // Update prediction error
    pImpl->predictionError = tdError;
}

void Dopamine::learnFromOutcome(float actualOutcome) {
    // Learn from actual outcomes using prediction error
    float rpe = actualOutcome - pImpl->valueFunction;
    
    // Apply learning rate
    pImpl->valueFunction += pImpl->learningRate * rpe;
    pImpl->predictionError = rpe;
    
    // Update accumulated statistics
    pImpl->accumulatedPredictionErrors += std::abs(rpe);
    pImpl->stats.totalPredictionErrors += std::abs(rpe);
    
    NLM_LOG_DEBUG("Dopamine learned from outcome: actual=" + std::to_string(actualOutcome) + 
                  " predicted=" + std::to_string(pImpl->valueFunction) + 
                  " RPE=" + std::to_string(rpe));
}

void Dopamine::setPhasicGain(float gain) {
    pImpl->phasicGain = std::max(0.0f, gain);
    pImpl->parameters["phasic_gain"] = pImpl->phasicGain;
}

void Dopamine::setTonicBaseline(float baseline) {
    pImpl->tonicBaseline = std::clamp(baseline, 0.0f, 1.0f);
    pImpl->parameters["tonic_baseline"] = pImpl->tonicBaseline;
}

void Dopamine::setTimeConstants(float phasicTau, float tonicTau) {
    pImpl->phasicTimeConstant = std::max(0.1f, phasicTau);
    pImpl->tonicTimeConstant = std::max(10.0f, tonicTau);
    pImpl->parameters["phasic_tau"] = pImpl->phasicTimeConstant;
    pImpl->parameters["tonic_tau"] = pImpl->tonicTimeConstant;
}

void Dopamine::setAdaptationParameters(float adaptationRate, float decayRate) {
    pImpl->adaptationRate = std::max(0.0f, adaptationRate);
    pImpl->decayRate = std::max(0.0f, decayRate);
}

void Dopamine::modulateSTDP(float rpe, const Synapse* synapse, float eligibilityTrace) {
    if (!synapse || std::abs(rpe) < pImpl->plasticityThreshold) {
        return;
    }
    
    // Compute modulation factor based on RPE
    float modulationFactor = 1.0f;
    if (rpe > 0.0f) {
        // Positive RPE (better than expected): enhance LTP
        modulationFactor = pImpl->ltpModulation;
    } else {
        // Negative RPE (worse than expected): suppress LTD
        modulationFactor = pImpl->ltdModulation;
    }
    
    // Apply modulation to synaptic eligibility
    float modulatedEligibility = eligibilityTrace * modulationFactor;
    
    // Log the modulation
    NLM_LOG_DEBUG("Dopamine STDP modulation: RPE=" + std::to_string(rpe) + 
                  " modulation=" + std::to_string(modulationFactor) + 
                  " eligibility=" + std::to_string(modulatedEligibility));
}

void Dopamine::setSTDPModulationParameters(float ltpMod, float ltdMod, float threshold) {
    pImpl->ltpModulation = std::max(0.1f, ltpMod);
    pImpl->ltdModulation = std::max(0.1f, ltdMod);
    pImpl->plasticityThreshold = std::max(0.001f, threshold);
    
    pImpl->parameters["ltp_modulation"] = pImpl->ltpModulation;
    pImpl->parameters["ltd_modulation"] = pImpl->ltdModulation;
    pImpl->parameters["plasticity_threshold"] = pImpl->plasticityThreshold;
}

void Dopamine::reset() {
    pImpl->totalLevel = pImpl->tonicBaseline;
    pImpl->phasicComponent = 0.0f;
    pImpl->tonicComponent = pImpl->tonicBaseline;
    pImpl->predictionError = 0.0f;
    pImpl->valueFunction = 0.0f;
    pImpl->accumulatedRewards = 0.0f;
    pImpl->accumulatedPredictionErrors = 0.0f;
    pImpl->lastObservedReward = 0.0f;
    pImpl->lastPredictedReward = 0.0f;
    
    pImpl->rewardHistory.clear();
    pImpl->predictionHistory.clear();
    pImpl->errorHistory.clear();
    
    pImpl->rpeTimestamps.clear();
    pImpl->rpeTimestamps.push_back(pImpl->currentTime);
    
    // Reset statistics
    pImpl->stats.totalRewards = 0.0f;
    pImpl->stats.totalPredictionErrors = 0.0f;
    pImpl->stats.predictionAccuracy = 0.0f;
    pImpl->stats.explorationBonus = pImpl->explorationBonus;
    pImpl->stats.exploitationValue = 0.0f;
    
    NLM_LOG_INFO("Dopamine system reset");
}

void Dopamine::initialize(float initialLevel) {
    setLevel(initialLevel);
    pImpl->valueFunction = initialLevel;
    
    NLM_LOG_INFO("Dopamine system initialized with level=" + std::to_string(initialLevel));
}

float Dopamine::getPhasicComponent() const {
    return pImpl->phasicComponent;
}

float Dopamine::getTonicComponent() const {
    return pImpl->tonicComponent;
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

float Dopamine::getValueFunction() const {
    return pImpl->valueFunction;
}

const std::unordered_map<std::string, float>& Dopamine::getParameters() const {
    return pImpl->parameters;
}

void Dopamine::setParameter(const std::string& name, float value) {
    if (pImpl->parameters.find(name) != pImpl->parameters.end()) {
        pImpl->parameters[name] = value;
        
        // Update internal state based on parameter changes
        if (name == "phasic_gain") {
            setPhasicGain(value);
        } else if (name == "tonic_baseline") {
            setTonicBaseline(value);
        } else if (name == "phasic_tau") {
            setTimeConstants(value, pImpl->parameters["tonic_tau"]);
        } else if (name == "tonic_tau") {
            setTimeConstants(pImpl->parameters["phasic_tau"], value);
        } else if (name == "learning_rate") {
            pImpl->learningRate = std::max(0.0f, std::min(1.0f, value));
            pImpl->stats.learningRate = pImpl->learningRate;
        } else if (name == "plasticity_threshold") {
            setSTDPModulationParameters(pImpl->ltpModulation, pImpl->ltdModulation, value);
        }
    }
}

const Dopamine::LearningStats& Dopamine::getLearningStats() const {
    return pImpl->stats;
}

void Dopamine::applyNeuralPlasticity(const std::vector<std::shared_ptr<Synapse>>& synapses,
                                   const std::vector<float>& eligibilityTraces,
                                   float learningRateMultiplier) {
    if (synapses.size() != eligibilityTraces.size()) {
        NLM_LOG_ERROR("Dopamine plasticity error: synapse and eligibility trace sizes mismatch");
        return;
    }
    
    // Compute modulated learning rate
    float modulatedLearningRate = pImpl->learningRate * learningRateMultiplier;
    
    // Apply dopamine-modulated plasticity to each synapse
    for (size_t i = 0; i < synapses.size(); ++i) {
        if (!synapses[i]) continue;
        
        float eligibilityTrace = eligibilityTraces[i];
        
        // Apply dopamine modulation based on prediction error
        modulateSTDP(pImpl->predictionError, synapses[i].get(), eligibilityTrace);
        
        // Apply weight change proportional to eligibility trace and learning rate
        float weightChange = eligibilityTrace * modulatedLearningRate * getPlasticityFactor();
        
        if (std::abs(weightChange) > 1e-6f) {
            synapses[i]->applyWeightChange(weightChange);
            
            // Log significant weight changes
            if (std::abs(weightChange) > 0.01f) {
                NLM_LOG_INFO("Dopamine-mediated synaptic plasticity: synapse=" + 
                           std::to_string(synapses[i]->getId().index()) +
                           " delta=" + std::to_string(weightChange) +
                           " RPE=" + std::to_string(pImpl->predictionError));
            }
        }
    }
    
    NLM_LOG_DEBUG("Dopamine applied plasticity to " + std::to_string(synapses.size()) + 
                  " synapses with learning rate multiplier=" + std::to_string(learningRateMultiplier));
}

void Dopamine::integrateWithPredictionSystem(float predictedValue, float actualValue) {
    // Update prediction using TD error
    updateValuePrediction(predictedValue);
    
    // Learn from actual outcome
    learnFromOutcome(actualValue);
    
    // Compute integrated prediction error
    float tdError = actualValue - predictedValue;
    
    // Adapt prediction parameters based on error
    if (std::abs(tdError) > pImpl->plasticityThreshold) {
        // Adjust learning rate based on prediction accuracy
        pImpl->learningRate *= (1.0f - std::abs(tdError));
        pImpl->learningRate = std::clamp(pImpl->learningRate, 0.01f, 0.5f);
        
        // Update exploration bonus based on uncertainty
        pImpl->explorationBonus = std::abs(tdError);
        pImpl->stats.explorationBonus = pImpl->explorationBonus;
        
        // Update exploitation value
        pImpl->stats.exploitationValue = pImpl->valueFunction;
        
        NLM_LOG_DEBUG("Dopamine integrated with prediction system: predicted=" + 
                     std::to_string(predictedValue) + 
                     " actual=" + std::to_string(actualValue) + 
                     " TD_error=" + std::to_string(tdError));
    }
}

void Dopamine::temporalAdaptation(float environmentComplexity) {
    // Adapt dopamine dynamics based on environmental complexity
    float complexityFactor = std::clamp(environmentComplexity, 0.0f, 1.0f);
    
    // Increase phasic gain in complex environments
    pImpl->phasicGain = 1.0f + complexityFactor * 3.0f;
    
    // Adjust time constants based on complexity
    pImpl->phasicTimeConstant = 0.5f * (1.0f + complexityFactor);
    pImpl->tonicTimeConstant = 300.0f * (1.0f + complexityFactor * 0.5f);
    
    // Update parameters
    pImpl->parameters["phasic_gain"] = pImpl->phasicGain;
    pImpl->parameters["phasic_tau"] = pImpl->phasicTimeConstant;
    pImpl->parameters["tonic_tau"] = pImpl->tonicTimeConstant;
    
    // Adjust learning rate for complex environments
    pImpl->learningRate = 0.15f * (1.0f + complexityFactor * 0.5f);
    pImpl->stats.learningRate = pImpl->learningRate;
    
    NLM_LOG_INFO("Dopamine temporal adaptation: complexity=" + std::to_string(environmentComplexity) +
                 " phasic_gain=" + std::to_string(pImpl->phasicGain) +
                 " learning_rate=" + std::to_string(pImpl->learningRate));
}

} // namespace nlm