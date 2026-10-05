#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    // Real dopamine neuron dynamics
    float level;                    // Current dopamine concentration (nM)
    float baseline;                 // Adaptive baseline concentration
    float recentBaseline;           // Recent baseline for tracking
    float burstLevel;               // Burst firing level
    float steadyState;              // Steady state level
    float recoveryRate;             // Rate of recovery towards baseline
    float potentiationRate;         // Rate of potentiation from rewards
    float depressionRate;           // Rate of depression from prediction errors
    
    // Time constants for concentration dynamics
    float concentrationTimeConstant; // Tau for concentration decay (seconds)
    float burstTimeConstant;         // Burst decay time constant
    float adaptationTimeConstant;    // Baseline adaptation time constant
    
    // Reward prediction error system
    float expectedReward;           // Expected reward prediction
    float predictionError;          // Current reward prediction error
    float predictionErrorHistory;   // Smoothed prediction error
    
    // Burst firing mechanism
    bool isBursting;                // Whether in burst firing mode
    float burstDuration;            // Remaining burst duration
    float burstRate;                // Burst firing rate
    float silentPeriod;             // Silent period between bursts
    float refractoryPeriod;         // Refractory period after burst
    
    // Concentration history for adaptive baseline
    std::vector<float> concentrationHistory;
    std::vector<float> rewardHistory;       // Recent reward history
    std::vector<float> predictionErrorHistory; // Recent prediction errors
    
    // Dopamine neuron conductance-based dynamics
    float membranePotential;        // Membrane potential (mV)
    float sodiumConductance;        // Sodium conductance (mS/cm²)
    float potassiumConductance;     // Potassium conductance (mS/cm²)
    float leakConductance;          // Leak conductance (mS/cm²)
    float inputConductance;         // Synaptic input conductance (mS/cm²)
    
    // Neuron state
    float threshold;                // Firing threshold (mV)
    float resetPotential;           // Reset potential (mV)
    float refractoryRemaining;      // Remaining refractory time
    float spikeCount;               // Total spike count
    float burstSpikeCount;          // Spike count during current burst
    
    // Random generator
    RandomGenerator rng;
    
    Impl() : 
        // Initial concentrations
        level(0.0f), baseline(0.0f), recentBaseline(0.0f), burstLevel(0.0f),
        steadyState(0.0f), recoveryRate(0.1f), potentiationRate(0.2f),
        depressionRate(0.15f), concentrationTimeConstant(2.0f),
        burstTimeConstant(0.5f), adaptationTimeConstant(10.0f),
        
        // Reward prediction error
        expectedReward(0.0f), predictionError(0.0f),
        predictionErrorHistory(0.0f),
        
        // Burst firing
        isBursting(false), burstDuration(0.0f), burstRate(20.0f), // Hz
        silentPeriod(1.0f), refractoryPeriod(0.1f),
        
        // History tracking
        concentrationHistory(), rewardHistory(), predictionErrorHistory(),
        
        // Neuron dynamics
        membranePotential(-70.0f), sodiumConductance(120.0f),
        potassiumConductance(36.0f), leakConductance(0.3f),
        inputConductance(0.0f), threshold(-50.0f),
        resetPotential(-65.0f), refractoryRemaining(0.0f),
        spikeCount(0.0f), burstSpikeCount(0.0f),
        
        // Initialize random generator
        rng(42) {}
    
    // Update adaptive baseline based on recent reward history
    void updateAdaptiveBaseline() {
        if (rewardHistory.size() > 0) {
            // Calculate recent mean reward (last 10 rewards)
            size_t historySize = std::min(rewardHistory.size(), static_cast<size_t>(10));
            float sumRewards = 0.0f;
            for (size_t i = 0; i < historySize; ++i) {
                sumRewards += rewardHistory[rewardHistory.size() - 1 - i];
            }
            float recentMean = sumRewards / static_cast<float>(historySize);
            
            // Slowly adapt baseline towards recent mean
            float targetBaseline = recentMean * 2.5f; // Scaling factor
            recentBaseline = recentBaseline + (targetBaseline - recentBaseline) * 
                            (1.0f / adaptationTimeConstant) * 0.01f;
        }
    }
    
    // Calculate burst firing based on prediction error
    void updateBurstFiring(float dt) {
        if (std::abs(predictionError) > 0.5f) { // Threshold for burst initiation
            if (!isBursting && refractoryRemaining <= 0.0f) {
                isBursting = true;
                burstDuration = 0.5f; // 500ms burst duration
                burstSpikeCount = 0;
                // Log burst initiation
            }
        }
        
        if (isBursting) {
            burstDuration -= dt;
            burstSpikeCount++;
            
            // Update concentration during burst
            float burstInput = predictionError * potentiationRate * 10.0f;
            level += burstInput * dt;
            
            if (burstDuration <= 0.0f) {
                isBursting = false;
                // Start silent period
                silentPeriod = 1.0f;
            }
        } else if (silentPeriod > 0.0f) {
            silentPeriod -= dt;
            refractoryRemaining = std::max(0.0f, refractoryRemaining - dt);
        }
    }
    
    // Update prediction error with smoothing
    void updatePredictionError(float dt) {
        // Simple exponential smoothing
        predictionErrorHistory = predictionErrorHistory * (1.0f - dt) + predictionError * dt;
    }
    
    // Update dopamine concentration with realistic dynamics
    void updateConcentration(float dt) {
        // Calculate target concentration based on inputs
        float targetLevel = steadyState;
        
        // Add reward-based potentiation
        if (predictionError > 0.0f) {
            targetLevel += predictionError * potentiationRate;
        }
        
        // Add prediction error-based depression
        if (predictionError < 0.0f) {
            targetLevel += predictionError * depressionRate;
        }
        
        // Adaptive baseline contribution
        targetLevel += baseline * 0.1f;
        
        // Exponential approach to target level
        float delta = targetLevel - level;
        level += delta * (1.0f - static_cast<float>(std::exp(-dt / concentrationTimeConstant)));
        
        // Natural decay
        level = std::max(0.0f, level * (1.0f - recoveryRate * dt * 0.1f));
        
        // Limit maximum concentration
        level = std::min(level, 1000.0f); // Physiological upper limit
        
        // Update history
        concentrationHistory.push_back(level);
        if (concentrationHistory.size() > 100) {
            concentrationHistory.erase(concentrationHistory.begin());
        }
    }
    
    // Conductance-based neuron update
    void updateNeuronDynamics(float dt) {
        // Hodgkin-Huxley style dynamics (simplified)
        float V = membranePotential;
        
        // Update conductances based on inputs
        float currentInput = inputConductance * (V - (-70.0f)); // Typical reversal potential
        
        // Sodium current
        float m = 0.1f * (1.0f / (1.0f + static_cast<float>(std::exp((V - (-40.0f)) / 10.0f))));
        float h = 0.7f * (1.0f / (1.0f + static_cast<float>(std::exp((V + 60.0f) / 10.0f))));
        float I_Na = sodiumConductance * m * m * m * h * (V - 50.0f);
        
        // Potassium current
        float n = 0.01f * (1.0f / (1.0f + static_cast<float>(std::exp((V - (-55.0f)) / 10.0f))));
        float I_K = potassiumConductance * n * n * n * n * (V - (-77.0f));
        
        // Leak current
        float I_leak = leakConductance * (V - (-65.0f));
        
        // Total membrane current
        float I_total = I_Na - I_K + I_leak + currentInput;
        
        // Update membrane potential (C = 1 for simplicity)
        membranePotential = V + (I_total * dt / 1.0f);
        
        // Check for spike threshold crossing
        if (membranePotential >= threshold && refractoryRemaining <= 0.0f && !isBursting) {
            // Spike detected
            spikeCount++;
            membranePotential = resetPotential;
            refractoryRemaining = refractoryPeriod;
            
            // Add to level based on burst or single spike
            float spikeValue = 1.0f;
            if (isBursting) {
                spikeValue = 5.0f; // Bursts produce larger dopamine release
            }
            level += spikeValue * 10.0f; // Scale factor for physiological realism
        }
        
        // Update refractory period
        if (refractoryRemaining > 0.0f) {
            refractoryRemaining -= dt;
        }
        
        // Update input conductance decay
        inputConductance *= (1.0f - dt * 0.1f);
    }
};  // End Impl struct

Dopamine::Dopamine() : pImpl(new Impl) {
    NLM_LOG_INFO("Dopamine system initialized with realistic dynamics");
}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1000.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity factor based on concentration
    // High dopamine enhances LTP, low dopamine enhances LTD
    if (pImpl->level < 100.0f) {
        // Low dopamine (below 100 nM)
        return 0.3f + pImpl->level / 1000.0f * 0.4f; // Enhances LTD
    } else {
        // High dopamine (above 100 nM)
        return 0.7f + (pImpl->level - 100.0f) / 900.0f * 0.3f; // Enhances LTP
    }
}

void Dopamine::update(TimestepDuration dt) {
    float deltaTime = static_cast<float>(dt);
    
    // Update adaptive baseline based on reward history
    pImpl->updateAdaptiveBaseline();
    
    // Update prediction error with smoothing
    pImpl->updatePredictionError(deltaTime);
    
    // Update burst firing mechanism
    pImpl->updateBurstFiring(deltaTime);
    
    // Update dopamine concentration
    pImpl->updateConcentration(deltaTime);
    
    // Update neuron dynamics (for spike generation)
    pImpl->updateNeuronDynamics(deltaTime);
    
    // Ensure concentration stays within physiological bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1000.0f);
}

void Dopamine::signalReward(float reward) {
    // Store reward in history
    pImpl->rewardHistory.push_back(reward);
    if (pImpl->rewardHistory.size() > 50) {
        pImpl->rewardHistory.erase(pImpl->rewardHistory.begin());
    }
    
    // Add to level with saturation
    pImpl->level = std::min(pImpl->level + reward * pImpl->potentiationRate * 100.0f, 1000.0f);
    
    // Set steady state based on reward
    pImpl->steadyState = reward * 2.5f;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Store prediction error in history
    pImpl->predictionError = error;
    pImpl->predictionErrorHistory = error;
    pImpl->predictionErrorHistory = error; // Use directly for simplicity
    
    // Prediction errors modulate dopamine level
    if (error > 0.0f) {
        // Positive RPE (better than expected) produces burst firing
        pImpl->level = std::min(pImpl->level + error * pImpl->potentiationRate * 50.0f, 1000.0f);
        
        // Initiate burst firing if error is significant
        if (error > 0.8f) {
            pImpl->isBursting = true;
            pImpl->burstDuration = 0.5f;
        }
    } else if (error < 0.0f) {
        // Negative RPE (worse than expected) reduces dopamine
        pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->depressionRate * 50.0f);
    }
    
    // Update steady state based on prediction error
    pImpl->steadyState += error * 0.1f;
}

void Dopamine::connectToPredictionError(PredictionError* predictionError) {
    if (predictionError) {
        // In a real implementation, we would update our prediction error from the system
        // For now, we just log the connection
        NLM_LOG_INFO("Dopamine system connected to PredictionError system");
    }
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

float Dopamine::getBurstLevel() const {
    return pImpl->isBursting ? pImpl->burstRate : 0.0f;
}

float Dopamine::getAdaptiveBaseline() const {
    return pImpl->baseline;
}

float Dopamine::getConcentrationHistory(size_t index) const {
    if (index < pImpl->concentrationHistory.size()) {
        return pImpl->concentrationHistory[index];
    }
    return 0.0f;
}

float Dopamine::getRecentPredictionError() const {
    return pImpl->predictionErrorHistory;
}

std::vector<float> Dopamine::getAllConcentrationHistory() const {
    return pImpl->concentrationHistory;
}

std::vector<float> Dopamine::getAllRewardHistory() const {
    return pImpl->rewardHistory;
}

std::vector<float> Dopamine::getAllPredictionErrorHistory() const {
    return pImpl->predictionErrorHistory;
}

} // namespace nlm
