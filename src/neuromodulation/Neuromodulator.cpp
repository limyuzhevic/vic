#include "Neuromodulator.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <unordered_map>

namespace nlm {

struct Dopamine::Impl {
    // Dopamine state variables
    float level;              // Current dopamine concentration (normalized 0-1)
    float baseline;            // Baseline dopamine level (tonic firing)
    float predictionError;     // Current TD prediction error
    
    // Kinetic parameters
    float synthesisRate;       // Tyrosine hydroxylase activity rate
    float releaseProbability;  // Burst release probability
    float clearanceRate;       // DAT reuptake clearance rate
    float diffusionRate;       // Extracellular diffusion rate
    
    // Autoreceptor dynamics
    float autoreceptorActivation; // D2 autoreceptor sensitivity
    float autoreceptorTimeConstant; // Autoreceptor feedback time
    
    // Receptor pathways
    float d1Activation;       // D1 receptor activation (appetitive learning)
    float d2Activation;       // D2 receptor activation (aversive learning)
    float d1TimeConstant;     // D1 receptor dynamics
    float d2TimeConstant;     // D2 receptor dynamics
    
    // Time constants for different processes
    float burstTimeConstant;  // Dopamine burst transient duration
    float phasicTimeConstant; // Phasic signal duration
    float tonicTimeConstant;  // Tonic baseline duration
    
    // Eligibility traces for reward association
    float globalEligibilityTrace; // Global trace for delayed rewards
    float eligibilityDecayRate;   // Trace decay rate
    
    // Stochasticity for biological realism
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    
    // Pre-computed spike history for plasticity
    struct SpikeHistoryEntry {
        float time;
        float weight;
        float eligibilityTrace;
    };
    
    std::vector<SpikeHistoryEntry> preSpikeHistory;
    std::vector<SpikeHistoryEntry> postSpikeHistory;
    
    // Synaptic mapping for modulated plasticity
    std::unordered_map<Synapse*, float> synapseDopamineModulation;
    
    Impl() : 
        level(0.0f), 
        baseline(0.05f),
        predictionError(0.0f),
        synthesisRate(0.8f),
        releaseProbability(0.3f),
        clearanceRate(0.5f),
        diffusionRate(0.3f),
        autoreceptorActivation(0.2f),
        autoreceptorTimeConstant(50.0f),
        d1Activation(0.0f),
        d2Activation(0.0f),
        d1TimeConstant(100.0f),
        d2TimeConstant(200.0f),
        burstTimeConstant(100.0f),
        phasicTimeConstant(500.0f),
        tonicTimeConstant(2000.0f),
        globalEligibilityTrace(0.0f),
        eligibilityDecayRate(0.001f),
        rng(std::random_device{}()),
        uniformDist(0.0f, 1.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 2.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Dopamine-modulated plasticity factor based on D1/D2 balance
    // D1 enhances LTP (positive weights), D2 enhances LTD (negative weights)
    // Net effect determines overall plasticity magnitude
    float d1d2Balance = pImpl->d1Activation - pImpl->d2Activation;
    float predictionModulation = std::tanh(pImpl->predictionError);
    
    // Plasticity is maximal when D1 and D2 have opposing effects
    // and scales with prediction error magnitude
    float basePlasticity = 0.5f + 0.5f * std::abs(d1d2Balance);
    float errorModulation = 1.0f + std::abs(predictionModulation);
    
    return basePlasticity * errorModulation;
}

void Dopamine::update(TimestepDuration dt) {
    TimestepDuration dtMs = static_cast<TimestepDuration>(dt * 1000.0); // Convert to ms
    
    // Update baseline (tonic firing)
    float tonicRelease = pImpl->baseline * pImpl->synthesisRate;
    
    // Update prediction error with decay
    if (std::abs(pImpl->predictionError) > 1e-6f) {
        pImpl->predictionError *= std::exp(-dtMs / pImpl->phasicTimeConstant);
    }
    
    // Autoreceptor feedback (D2 autoreceptors inhibit dopamine release)
    float autoreceptorEffect = pImpl->autoreceptorActivation * pImpl->level;
    
    // Stochastic burst firing for phasic signals
    float burstTrigger = pImpl->uniformDist(pImpl->rng);
    float burstMagnitude = 0.0f;
    
    if (burstTrigger < pImpl->releaseProbability) {
        // Simulated burst firing based on presynaptic activity
        float spikeRate = 20.0f; // Hz - typical burst firing rate
        burstMagnitude = spikeRate * 0.1f * std::exp(-dtMs / pImpl->burstTimeConstant);
    }
    
    // Combine all sources and apply autoreceptor inhibition
    float totalInput = tonicRelease + burstMagnitude;
    float autoreceptorInhibition = autoreceptorEffect;
    
    // Update dopamine level with clearance and diffusion
    float clearance = pImpl->level * pImpl->clearanceRate;
    float diffusion = pImpl->level * pImpl->diffusionRate;
    
    // Euler integration
    pImpl->level += (totalInput - autoreceptorInhibition - clearance - diffusion) * dtMs;
    
    // Clamp to reasonable bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, 2.0f);
    
    // Update D1/D2 receptor activations
    if (pImpl->level > 0.0f) {
        // D1 receptors respond to high dopamine (phasic)
        pImpl->d1Activation += (pImpl->level * 0.8f - pImpl->d1Activation) * dtMs / pImpl->d1TimeConstant;
        
        // D2 receptors respond to moderate dopamine
        pImpl->d2Activation += (pImpl->level * 0.3f - pImpl->d2Activation) * dtMs / pImpl->d2TimeConstant;
    }
    
    // Decay eligibility trace
    pImpl->globalEligibilityTrace *= std::exp(-dtMs * pImpl->eligibilityDecayRate);
}

void Dopamine::signalReward(float reward) {
    // True reward prediction error computation
    // For simplicity, assume current prediction is 0, so error = reward
    // In a full implementation, this would compare reward to predicted value
    signalRewardPredictionError(reward);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Implement true reward prediction error (TD error)
    // δ = r + γV(s') - V(s)
    
    // Store the prediction error
    pImpl->predictionError = error;
    
    // Generate a biologically realistic dopamine burst
    // Phasic dopamine encodes positive prediction errors (reward prediction)
    if (error > 0) {
        // Burst firing for positive prediction error
        float burstMagnitude = error * 2.0f * (1.0f + 0.5f * pImpl->uniformDist(pImpl->rng));
        float burstDuration = 100.0f + 50.0f * error; // ms
        
        // Simulate exponential burst profile
        float burstIntegral = 0.0f;
        for (float t = 0.0f; t < burstDuration; t += 1.0f) {
            float instantaneous = burstMagnitude * std::exp(-t / 50.0f);
            burstIntegral += instantaneous * 0.1f;
        }
        
        // Apply to dopamine level with saturation
        pImpl->level += burstIntegral;
        pImpl->level = std::min(pImpl->level, 2.0f);
        
        // Boost D1 receptor activation more for positive prediction error
        pImpl->d1Activation += error * 0.5f;
        pImpl->d2Activation *= 0.9f; // Slight suppression of D2
        
    } else if (error < 0) {
        // Phasic suppression for negative prediction error (punishment)
        float suppressionMagnitude = std::abs(error) * 1.5f;
        float suppressionDuration = 150.0f;
        
        // Dopamine dips for negative prediction error
        pImpl->level -= suppressionMagnitude;
        pImpl->level = std::max(pImpl->level, 0.0f);
        
        // Boost D2 receptor activation more for negative prediction error
        pImpl->d2Activation += std::abs(error) * 0.5f;
        pImpl->d1Activation *= 0.9f;
    }
    
    // Update autoreceptor activation in response to level changes
    pImpl->autoreceptorActivation = std::min(1.0f, pImpl->level * 0.5f);
}

// Additional methods for dopamine-modulated plasticity

void Dopamine::recordPreSpike(Synapse* synapse, float time, float weight) {
    if (!synapse) return;
    
    // Store pre-synaptic spike with eligibility trace
    Impl::SpikeHistoryEntry entry{time, weight, 0.0f};
    pImpl->preSpikeHistory.push_back(entry);
    
    // Update eligibility trace for this synapse based on dopamine level
    if (pImpl->level > 0.0f) {
        float d1Effect = pImpl->d1Activation * 0.5f; // D1 enhances trace
        float d2Effect = pImpl->d2Activation * 0.3f; // D2 modulates trace
        float traceModulation = 1.0f + d1Effect - d2Effect;
        
        // Scale eligibility trace by dopamine level and receptor effects
        float eligibilityChange = pImpl->level * traceModulation * 0.1f;
        
        // Update synapse-specific modulation
        auto it = pImpl->synapseDopamineModulation.find(synapse);
        if (it != pImpl->synapseDopamineModulation.end()) {
            it->second = eligibilityChange;
        }
    }
}

void Dopamine::recordPostSpike(Synapse* synapse, float time) {
    if (!synapse) return;
    
    // Store post-synaptic spike
    Impl::SpikeHistoryEntry entry{time, 0.0f, 0.0f};
    pImpl->postSpikeHistory.push_back(entry);
    
    // Apply post-synaptic spike effects on plasticity
    if (pImpl->level > 0.0f) {
        float d1Effect = pImpl->d1Activation * 0.3f;
        float d2Effect = pImpl->d2Activation * 0.2f;
        float modulation = 1.0f + d1Effect - d2Effect;
        
        // Update global eligibility trace
        pImpl->globalEligibilityTrace += pImpl->level * modulation * 0.05f;
        
        // Apply to all synapses
        for (auto& pair : pImpl->synapseDopamineModulation) {
            pair.second *= modulation;
        }
    }
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

void Dopamine::resetPredictionError() {
    pImpl->predictionError = 0.0f;
}

float Dopamine::getD1Modulation() const {
    return pImpl->d1Activation;
}

float Dopamine::getD2Modulation() const {
    return pImpl->d2Activation;
}

float Dopamine::getEligibilityTrace() const {
    return pImpl->globalEligibilityTrace;
}

void Dopamine::updateEligibilityTrace(float delta, float decayRate) {
    pImpl->globalEligibilityTrace += delta * (1.0f + pImpl->d1Activation - pImpl->d2Activation);
    pImpl->globalEligibilityTrace *= std::exp(-decayRate);
}

float Dopamine::computeTDUpdate(float predictionError, float learningRate, float eligibilityTrace) {
    // TD learning: ΔW = δ * e(t) * f(W)
    // Where δ is prediction error, e(t) is eligibility trace, f(W) is weight-dependent function
    
    // Weight-dependent function prevents runaway potentiation/depression
    float weightDependence = 1.0f / (1.0f + std::abs(predictionError) * 10.0f);
    
    // Dopamine modulation scales the update
    float d1Scale = 1.0f + pImpl->d1Activation * 0.5f;
    float d2Scale = 1.0f - pImpl->d2Activation * 0.5f;
    float receptorModulation = d1Scale * d2Scale;
    
    // Combine all factors
    float tdUpdate = predictionError * eligibilityTrace * learningRate * 
                    weightDependence * receptorModulation;
    
    // Add biological noise for realism
    tdUpdate *= (1.0f + (pImpl->uniformDist(pImpl->rng) - 0.5f) * 0.1f);
    
    return tdUpdate;
}

float Dopamine::getLearningRate(float predictionError, float baselineRate) {
    // Dopamine-dependent learning rate modulation
    // Higher prediction error -> higher learning rate
    // But bounded to prevent instability
    
    float errorMagnitude = std::abs(predictionError);
    
    // Synaptic scaling factor
    float synapticFactor = 1.0f / (1.0f + errorMagnitude * 5.0f);
    
    // D1 enhances learning rate, D2 reduces it
    float d1Effect = 1.0f + pImpl->d1Activation * 0.8f;
    float d2Effect = 1.0f - pImpl->d2Activation * 0.3f;
    
    float rate = baselineRate * errorMagnitude * d1Effect * d2Effect * synapticFactor;
    
    // Clip to reasonable range
    return std::clamp(rate, 0.001f, 0.5f);
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->predictionError = 0.0f;
    pImpl->d1Activation = 0.0f;
    pImpl->d2Activation = 0.0f;
    pImpl->globalEligibilityTrace = 0.0f;
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->synapseDopamineModulation.clear();
}

} // namespace nlm
