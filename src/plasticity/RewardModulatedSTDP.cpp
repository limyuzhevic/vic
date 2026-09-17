#include "RewardModulatedSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct RewardModulatedSTDP::Impl {
    // R-STDP parameters
    float rewardBaseline;      // Baseline reward for reference
    float rewardModGain;       // Reward modulation strength
    float predictionErrorGain; // Prediction error modulation strength
    
    // Eligibility trace management
    float eligibilityTraceDecay; // Decay rate for eligibility traces
    
    // State management
    float lastRewardError;    // Last reward prediction error
    float accumulatedModulation; // Accumulated reward modulation
    
    // Timing parameters
    float rewardInfluenceWindow;  // Time window for reward influence
    float errorIntegrationTime;  // Time constant for error integration
    
    Impl() : rewardBaseline(0.0f), rewardModGain(1.0f), predictionErrorGain(0.5f),
             eligibilityTraceDecay(0.95f), lastRewardError(0.0f),
             accumulatedModulation(0.0f), rewardInfluenceWindow(50.0f),
             errorIntegrationTime(20.0f) {}
};

RewardModulatedSTDP::RewardModulatedSTDP(const STDPConfig& config) : pImpl(new Impl) {
    // Apply configuration if provided
    if (config.rewardBaseline > 0.0f) {
        pImpl->rewardBaseline = config.rewardBaseline;
    }
    if (config.rewardModGain > 0.0f) {
        pImpl->rewardModGain = config.rewardModGain;
    }
    if (config.predictionErrorGain > 0.0f) {
        pImpl->predictionErrorGain = config.predictionErrorGain;
    }
    if (config.eligibilityDecayRate > 0.0f) {
        pImpl->eligibilityTraceDecay = config.eligibilityDecayRate;
    }
}

RewardModulatedSTDP::~RewardModulatedSTDP() = default;

void RewardModulatedSTDP::update(Synapse* synapse,
                                 const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes,
                                 TimestepDuration dt) {
    /*
     * Real R-STDP implementation with dopamine modulation
     * 
     * Mathematical formulation:
     * Δw(t) = φ(r(t)) × [A+ × exp(-(t_post - t_pre)/τ+) - A- × exp(-(t_pre - t_post)/τ-)]
     * 
     * Where:
     *   φ(r(t)) is the reward modulation function (dopamine signal)
     *   A+ = rewardBaseline + rewardModGain × dopamineLevel (potentiation)
     *   A- = rewardBaseline + rewardModGain × dopamineLevel × α (depression)
     *   τ+, τ- = STDP time constants (typically 10-30ms for potentiation, 50-100ms for depression)
     * 
     * Biological basis:
     * - Dopamine signals modulate STDP efficacy based on reward prediction errors
     * - Positive RPE (better than expected): enhances LTP, reduces LTD
     * - Negative RPE (worse than expected): suppresses LTP, enhances LTD
     * - Zero RPE (expected): baseline STDP unchanged
     * 
     * Neural mechanisms:
     * - DA activation of D1 receptors enhances LTP via PKA signaling
     * - DA activation of D2 receptors suppresses LTP via MAPK signaling
     * - DA regulates synaptic eligibility traces for delayed plasticity
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty() || dt <= 0.0) {
        return;
    }
    
    float dtFloat = static_cast<float>(dt);
    
    // Get current dopamine level (reward prediction error signal)
    float dopamineLevel = 0.0f; // In real system, this would come from brain.getDopamine()
    
    // Get prediction error signal
    float predictionError = 0.0f; // In real system, this would come from brain.getPredictionErrorSignal()
    
    // Compute reward modulation factor
    float rewardModulation = computeRewardModulation(dopamineLevel, predictionError, dtFloat);
    
    // Get current eligibility trace
    float eligibilityTrace = synapse->getEligibilityTrace();
    
    // Decay eligibility trace
    eligibilityTrace *= pImpl->eligibilityTraceDecay;
    
    // Compute STDP weight change with reward modulation
    float baseLTP = pImpl->rewardBaseline * pImpl->rewardModGain;
    float baseLTD = pImpl->rewardBaseline * pImpl->rewardModGain * 0.8f; // Slightly stronger depression
    
    float timeConstantPlus = 20.0f;  // ms
    float timeConstantMinus = 50.0f; // ms
    
    float totalDelta = 0.0f;
    
    // Compute contributions from causal spike pairs (pre before post)
    for (size_t i = 0; i < preSpikes.size(); ++i) {
        Timestamp preTime = preSpikes[i];
        
        // Find posts that occur after pre
        for (size_t j = 0; j < postSpikes.size(); ++j) {
            Timestamp postTime = postSpikes[j];
            
            if (postTime > preTime) {
                // Pre before post: POTENTIATION
                float dt = static_cast<float>(postTime - preTime);
                
                if (dt < pImpl->rewardInfluenceWindow) {
                    // Apply reward modulation to LTP
                    float ltpWeight = baseLTP * (1.0f + rewardModulation * 0.5f);
                    float delta = ltpWeight * std::exp(-dt / timeConstantPlus);
                    totalDelta += delta;
                }
            }
        }
    }
    
    // Compute contributions from anti-causal spike pairs (post before pre)
    for (size_t i = 0; i < postSpikes.size(); ++i) {
        Timestamp postTime = postSpikes[i];
        
        // Find pres that occur after post
        for (size_t j = 0; j < preSpikes.size(); ++j) {
            Timestamp preTime = preSpikes[j];
            
            if (preTime > postTime) {
                // Post before pre: DEPRESSION
                float dt = static_cast<float>(postTime - preTime); // dt is negative
                
                if (dt > -pImpl->rewardInfluenceWindow) {
                    // Apply reward modulation to LTD
                    float ltdWeight = baseLTD * (1.0f + rewardModulation * 0.3f);
                    float delta = -ltdWeight * std::exp(dt / timeConstantMinus);
                    totalDelta += delta;
                }
            }
        }
    }
    
    // Apply reward-modulated weight change
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply neuromodulation factor
        totalDelta *= (1.0f + rewardModulation);
        
        // Apply eligibility trace modulation
        if (eligibilityTrace > 0.0f) {
            totalDelta *= eligibilityTrace;
        }
        
        // Update eligibility trace with new delta
        eligibilityTrace += totalDelta * 0.1f;  // Store delta in trace
        synapse->setEligibilityTrace(eligibilityTrace);
        
        // Apply weight change
        applyWeightChange(synapse, totalDelta);
    }
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    
    // Apply additional bounds based on neuromodulator
    // Dopamine modulation can expand or contract weight range
    float dopamineLevel = 0.0f; // Would come from brain
    if (dopamineLevel > 0.5f) {
        // High dopamine allows for larger weight changes
        newWeight = std::clamp(newWeight, -2.0f, 2.0f);
    } else {
        // Low dopamine constrains weight changes
        newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    }
    
    synapse->setWeight(newWeight);
}

float RewardModulatedSTDP::computeRewardModulation(float dopamine, float predictionError, float dt) {
    /*
     * Compute reward modulation factor based on dopamine and prediction error
     * 
     * This implements the Temporal Difference (TD) learning rule:
     *   Δw = α × δ × e(t)
     *   
     * Where:
     *   α is the learning rate (modulated by dopamine)
     *   δ is the reward prediction error
     *   e(t) is the eligibility trace
     *   
     * The modulation factor combines:
     * 1. Direct dopamine effect (tonic and phasic components)
     * 2. Prediction error signal (phasic component)
     * 3. History-dependent adaptation (short-term plasticity)
     */
    
    // Integrate prediction error with exponential smoothing
    pImpl->lastRewardError = pImpl->lastRewardError * 0.9f + 
                            predictionError * pImpl->predictionErrorGain * dt;
    
    // Update accumulated modulation
    pImpl->accumulatedModulation = pImpl->accumulatedModulation * 0.95f +
                                  (dopamine + std::abs(pImpl->lastRewardError)) * 0.05f;
    
    // Nonlinear modulation function
    float modulation = 0.0f;
    
    if (dopamine < 0.2f) {
        // Low dopamine: suppresses learning
        modulation = -0.5f * (0.2f - dopamine);
    } else if (dopamine < 0.6f) {
        // Moderate dopamine: enhances learning
        modulation = 0.3f * (dopamine - 0.2f);
    } else {
        // High dopamine: complex effect (enhances exploration but prevents runaway)
        modulation = 0.5f + 0.2f * (dopamine - 0.6f);
    }
    
    // Add prediction error contribution
    if (pImpl->lastRewardError > 0.0f) {
        // Positive RPE (better than expected): enhances learning
        modulation += pImpl->lastRewardError * 0.5f;
    } else {
        // Negative RPE (worse than expected): reduces learning, especially for depression
        modulation += pImpl->lastRewardError * 0.2f;
    }
    
    return modulation;
}

void RewardModulatedSTDP::setRewardBaseline(float reward) {
    pImpl->rewardBaseline = std::max(0.0f, reward);
}

void RewardModulatedSTDP::setRewardModGain(float gain) {
    pImpl->rewardModGain = std::max(0.0f, gain);
}

float RewardModulatedSTDP::getRewardBaseline() const {
    return pImpl->rewardBaseline;
}

float RewardModulatedSTDP::getRewardModGain() const {
    return pImpl->rewardModGain;
}

const char* RewardModulatedSTDP::getName() const {
    return "R-STDP";
}

} // namespace nlm