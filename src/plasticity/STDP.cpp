#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include "../../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

struct STDP::Impl {
    // Real neuroscience-based STDP parameters
    float ltpWeight;      // A+ for potentiation: Δw = +0.01 at +10ms
    float ltdWeight;      // A- for depression: Δw = -0.012 at -20ms
    float timeConstant;   // τ+ = τ- = 20ms (typical STDP time window)
    float tauPlus;        // Time constant for potentiation (τ+)
    float tauMinus;       // Time constant for depression (τ-)
    float minWeight;      // Minimum synaptic weight (-1.0)
    float maxWeight;      // Maximum synaptic weight (1.0)
    float homeostaticTarget; // Target mean weight (0.5)
    
    // Homeostatic control
    float homeostaticScale;    // Current scaling factor
    float globalMeanWeight;    // Running average of synaptic weights
    uint32_t weightSampleCount; // Number of weights sampled for homeostasis
    
    // Neuromodulation integration
    float dopamineLevel;       // Current dopamine concentration
    float modulationStrength;  // Dopamine modulation factor for STDP
    
    // Synaptic eligibility trace for delayed learning
    float eligibilityTrace;    // Accumulates weight changes for future reward
    float traceDecayRate;      // Rate of eligibility trace decay
    
    // Stability mechanisms
    float minLtpChange;        // Minimum potentiation change (numerical stability)
    float minLtdChange;        // Minimum depression change (numerical stability)
    
    // Pair-based STDP parameters
    float pairLtpFactor;       // Factor for spike pair-based LTP
    float pairLtdFactor;       // Factor for spike pair-based LTD
    
    // Burst detection
    float burstThreshold;      // Firing rate threshold for burst detection
    float burstTimeWindow;     // Time window for burst detection (ms)
    float burstLtpFactor;      // Enhanced LTP during bursts
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             tauPlus(20.0f), tauMinus(20.0f),
             minWeight(-1.0f), maxWeight(1.0f), homeostaticTarget(0.5f),
             homeostaticScale(1.0f), globalMeanWeight(0.0f), weightSampleCount(0),
             dopamineLevel(0.25f), modulationStrength(1.0f),
             eligibilityTrace(0.0f), traceDecayRate(0.1f),
             minLtpChange(1e-6f), minLtdChange(1e-6f),
             pairLtpFactor(0.8f), pairLtdFactor(0.9f),
             burstThreshold(50.0f), burstTimeWindow(50.0f), burstLtpFactor(2.0f) {
        
        NLM_LOG_INFO("STDP system initialized with realistic neuroscience parameters");
        NLM_LOG_INFO("LTP weight: 0.01 at +10ms, LTD weight: -0.012 at -20ms, τ≈20ms");
    }

STDP::STDP() : pImpl(new Impl) {}

STDP::~STDP() = default;

void STDP::update(Synapse* synapse,
                    const std::vector<Timestamp>& preSpikes,
                    const std::vector<Timestamp>& postSpikes,
                    TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty() || dt <= 0.0) {
        return;
    }
    
    float totalDelta = 0.0f;
    float learningRateMod = 1.0f; // Neuromodulation factor from dopamine
    
    // Apply neuromodulation from dopamine system
    if (pImpl->dopamineLevel > 0.0f) {
        learningRateMod = 1.0f + (pImpl->dopamineLevel - 0.25f) * pImpl->modulationStrength;
        learningRateMod = std::max(0.1f, std::min(3.0f, learningRateMod)); // Clamp to reasonable range
        NLM_LOG_DEBUG("STDP neuromodulation: dopamine=" + std::to_string(pImpl->dopamineLevel) + 
                     ", learningRateMod=" + std::to_string(learningRateMod));
    }
    
    // Process spike pairs with precise timing (±5ms precision)
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float deltaT = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (std::abs(deltaT) > 5.0f) { // Spike timing precision: ±5ms
                continue; // Outside precise timing window for STDP
            }
            
            if (deltaT > 0) {
                // PRE BEFORE POST: Long-Term Potentiation (LTP)
                // Equation: Δw = A+ * exp(-Δt / τ+)
                // A+ = 0.01 at +10ms, τ+ = 20ms
                float ltpChange = pImpl->ltpWeight * std::exp(-deltaT / pImpl->tauPlus);
                
                // Apply neuromodulation enhancement
                ltpChange *= learningRateMod;
                
                // Pair-based STDP: enhance for closely timed pairs
                if (deltaT < 20.0f) {
                    ltpChange *= pImpl->pairLtpFactor;
                }
                
                // Burst detection: enhanced LTP during high-frequency firing
                if (deltaT < pImpl->burstTimeWindow) {
                    // Calculate instantaneous firing rate
                    float preRate = static_cast<float>(preSpikes.size()) / std::max(1.0f, pImpl->burstTimeWindow);
                    if (preRate > pImpl->burstThreshold) {
                        ltpChange *= pImpl->burstLtpFactor;
                        NLM_LOG_DEBUG("STDP burst detected: rate=" + std::to_string(preRate) + ", enhanced LTP");
                    }
                }
                
                // Apply minimum change for numerical stability
                if (std::abs(ltpChange) > pImpl->minLtpChange) {
                    totalDelta += ltpChange;
                    
                    // Update eligibility trace for reward-modulated learning
                    // Eligibility trace accumulates changes that are eligible for reward modulation
                    float currentTrace = synapse->getEligibilityTrace();
                    synapse->setEligibilityTrace(currentTrace + ltpChange);
                    
                    NLM_LOG_DEBUG("STDP LTP: Δt=" + std::to_string(deltaT) + 
                                 ", Δw=" + std::to_string(ltpChange) +
                                 ", eligibilityTrace=" + std::to_string(synapse->getEligibilityTrace()));
                }
                
            } else if (deltaT < 0) {
                // POST BEFORE PRE: Long-Term Depression (LTD)
                // Equation: Δw = -A- * exp(Δt / τ-)
                // A- = 0.012 at -20ms, τ- = 20ms
                float ltdChange = -pImpl->ltdWeight * std::exp(deltaT / pImpl->tauMinus);
                
                // Apply neuromodulation modulation (dopamine suppresses LTD)
                ltdChange *= (1.0f - (pImpl->dopamineLevel * 0.3f));
                
                // Pair-based STDP: enhance for closely timed pairs
                if (deltaT > -20.0f) {
                    ltdChange *= pImpl->pairLtdFactor;
                }
                
                // Apply minimum change for numerical stability
                if (std::abs(ltdChange) > pImpl->minLtdChange) {
                    totalDelta += ltdChange;
                    
                    // Update eligibility trace
                    float currentTrace = synapse->getEligibilityTrace();
                    synapse->setEligibilityTrace(currentTrace + ltdChange);
                    
                    NLM_LOG_DEBUG("STDP LTD: Δt=" + std::to_string(deltaT) + 
                                 ", Δw=" + std::to_string(ltdChange) +
                                 ", eligibilityTrace=" + std::to_string(synapse->getEligibilityTrace()));
                }
            }
            // deltaT == 0: simultaneous spikes, no change (rare biologically)
        }
    }
    
    // Apply homeostatic scaling if we have enough weight samples
    if (pImpl->weightSampleCount > 10) {
        float currentWeight = synapse->getWeight();
        pImpl->globalMeanWeight += currentWeight;
        pImpl->weightSampleCount++;
        
        // Calculate homeostatic error
        float homeostaticError = pImpl->homeostaticTarget - pImpl->globalMeanWeight / pImpl->weightSampleCount;
        
        // Apply homeostatic scaling
        if (std::abs(homeostaticError) > 0.01f) {
            pImpl->homeostaticScale = 1.0f + homeostaticError * 0.1f;
            pImpl->homeostaticScale = std::max(0.8f, std::min(1.2f, pImpl->homeostaticScale));
            
            // Scale weight change by homeostatic factor
            totalDelta *= pImpl->homeostaticScale;
            
            NLM_LOG_DEBUG("STDP homeostatic: error=" + std::to_string(homeostaticError) + 
                         ", scale=" + std::to_string(pImpl->homeostaticScale) +
                         ", meanWeight=" + std::to_string(pImpl->globalMeanWeight / pImpl->weightSampleCount));
        }
    }
    
    // Apply weight change with proper bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        
        // Decay eligibility trace over time
        synapse->decayEligibilityTrace(0.05f); // 5% decay per timestep
        
        // Log significant changes
        if (std::abs(totalDelta) > 0.001f) {
            NLM_LOG_INFO("STDP applied: Δw=" + std::to_string(totalDelta) + 
                        ", weight=" + std::to_string(synapse->getWeight()) +
                        ", eligibilityTrace=" + std::to_string(synapse->getEligibilityTrace()));
        }
    }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* STDP::getName() const {
    return "STDP";
}

void STDP::setLTPWeight(float weight) {
    pImpl->ltpWeight = std::clamp(weight, 0.0f, 1.0f);
}

float STDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void STDP::setLTDWeight(float weight) {
    pImpl->ltdWeight = std::clamp(weight, 0.0f, 1.0f);
}

float STDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

void STDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
}

float STDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
}

} // namespace nlm
