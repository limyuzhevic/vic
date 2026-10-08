#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <chrono>

namespace nlm {

struct STDP::Impl {
    float ltpWeight;      // A+ for potentiation
    float ltdWeight;      // A- for depression
    float timeConstant;   // Tau for exponential window (ms)
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f) {}
};

STDP::STDP() : pImpl(new Impl) {}

STDP::~STDP() = default;

void STDP::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    /*
     * Real STDP implementation based on spike-timing correlation
     * 
     * Mathematical formulation:
     * For each pre-post spike pair with timing difference Δt = t_post - t_pre:
     * 
     * If Δt > 0 (pre before post): POTENTIATION
     *   Δw = A+ * exp(-Δt / τ+)
     *   
     * If Δt < 0 (post before pre): DEPRESSION
     *   Δw = A- * exp(Δt / τ-)
     * 
     * Where:
     *   A+ = ltpWeight (potentiation amplitude)
     *   A- = ltdWeight (depression amplitude)  
     *   τ+ = τ- = timeConstant (STDP time window)
     * 
     * Biological inspiration:
     *   - Reflects NMDA receptor-mediated calcium signaling
     *   - Pre-before-post activates NMDA receptors when postsynaptic spikes
     *   - Post-before-pre causes backpropagating action potentials
     *   
     * Limitations:
     *   - Simplified pairwise rule (doesn't capture triplet interactions)
     *   - Assumes single exponential window (more complex in biology)
     *   - Doesn't account for synaptic eligibility traces
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                // "Cells that fire together, wire together" - but only if pre fires before post
                float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                totalDelta += delta;
            } else if (dt < 0) {
                // Post before pre: DEPRESSION
                // "Anti-Hebbian" - connection weakens if post fires without pre
                float delta = -pImpl->ltdWeight * std::exp(dt / tau);  // dt is negative, so this subtracts
                totalDelta += delta;
            }
            // dt == 0: no change (simultaneous spikes - rare in practice)
        }
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
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

struct RewardModulatedSTDP::Impl {
    float ltpWeight;
    float ltdWeight;
    float timeConstant;
    float rewardRate;
    float minWeight;
    float maxWeight;
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             rewardRate(0.1f), minWeight(-1.0f), maxWeight(1.0f) {}
};

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(new Impl) {}

RewardModulatedSTDP::~RewardModulatedSTDP() = default;

void RewardModulatedSTDP::update(Synapse* synapse,
                                 const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes,
                                 TimestepDuration dt) {
    /*
     * Reward-modulated STDP (R-STDP)
     * Combines STDP with reward signals for model-based learning
     * Based on: Wu et al. (2010), "Reward-modulated spike-timing-dependent plasticity"
     * 
     * Mathematical formulation:
     * Δw = (STDP_weight * reward_signal) * eligibility_trace
     * 
     * Where:
     *   STDP_weight = LTP weight for pre-post spikes, LTD weight for post-pre spikes
     *   reward_signal = prediction error (reward - expected_reward)
     *   eligibility_trace = synaptic eligibility for reward-based plasticity
     * 
     * Biological inspiration:
     *   - Dopamine modulates synaptic eligibility traces
     *   - Dopamine release signals prediction errors
     *   - DA D1 receptor strengthens synapses for predicted rewards
     *   - DA D2 receptor weakens synapses for unexpected outcomes
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Compute standard STDP weight change
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                totalDelta += delta;
            } else if (dt < 0) {
                // Post before pre: DEPRESSION
                float delta = -pImpl->ltdWeight * std::exp(dt / tau);
                totalDelta += delta;
            }
        }
    }
    
    // Apply reward modulation
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by eligibility trace and reward signal
        float eligibility = synapse->getEligibilityTrace();
        totalDelta *= eligibility * pImpl->rewardRate;
        
        applyWeightChange(synapse, totalDelta);
        
        // Decay eligibility trace after use
        synapse->decayEligibilityTrace(0.1f);
    }
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* RewardModulatedSTDP::getName() const {
    return "R-STDP";
}

void RewardModulatedSTDP::setLTPWeight(float weight) {
    pImpl->ltpWeight = std::clamp(weight, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void RewardModulatedSTDP::setLTDWeight(float weight) {
    pImpl->ltdWeight = std::clamp(weight, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

void RewardModulatedSTDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
}

float RewardModulatedSTDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void RewardModulatedSTDP::setRewardRate(float rate) {
    pImpl->rewardRate = std::clamp(rate, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getRewardRate() const {
    return pImpl->rewardRate;
}

void RewardModulatedSTDP::configure(float ltpWeight, float ltdWeight, float tau, float rewardRate) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
    setRewardRate(rewardRate);
}

struct AntiHebbianRule::Impl {
    float learningRate;
    float minWeight;
    float maxWeight;
    
    Impl() : learningRate(0.01f), minWeight(-1.0f), maxWeight(1.0f) {}
};

AntiHebbianRule::AntiHebbianRule() : pImpl(new Impl) {}

AntiHebbianRule::~AntiHebbianRule() = default;

void AntiHebbianRule::update(Synapse* synapse,
                            const std::vector<Timestamp>& preSpikes,
                            const std::vector<Timestamp>& postSpikes,
                            TimestepDuration dt) {
    /*
     * Anti-Hebbian learning implementation
     * 
     * Mathematical formulation:
     * Δw = -η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
     * 
     * Where:
     *   η = learningRate (positive parameter)
     *   coactivity = number of correlated pre/post spikes
     *   baseline = learningRate * mean activity
     * 
     * This implements "neurons that fire together, divide together" - promoting competition
     * 
     * Biological inspiration:
     *   - Reflects synaptic depression in motor learning
     *   - Competitive plasticity in developing sensory maps
     *   - Weight normalization in neural circuits
     *   
     * Use cases:
     *   - Competitive learning between parallel pathways
     *   - Contrast enhancement in sensory processing
     *   - Preventing runaway excitation
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Count correlated spike pairs (simplified covariance)
    size_t correlationCount = 0;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            // Count spikes within a broad time window as correlated
            if (std::abs(dt) < 100.0f) {  // 100ms correlation window
                ++correlationCount;
            }
        }
    }
    
    // Compute weight change (negative of Hebbian)
    // Higher correlation = stronger depression
    float delta = -pImpl->learningRate * static_cast<float>(correlationCount);
    
    // Apply with bounds (anti-Hebbian only depresses/excites within bounds)
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void AntiHebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* AntiHebbianRule::getName() const {
    return "AntiHebbian";
}

void AntiHebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float AntiHebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

void AntiHebbianRule::setMinMaxWeights(float minWeight, float maxWeight) {
    pImpl->minWeight = std::clamp(minWeight, -10.0f, 0.0f);
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

void AntiHebbianRule::setMinWeight(float minWeight) {
    pImpl->minWeight = std::clamp(minWeight, -10.0f, 0.0f);
}

void AntiHebbianRule::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

float AntiHebbianRule::getMinWeight() const {
    return pImpl->minWeight;
}

float AntiHebbianRule::getMaxWeight() const {
    return pImpl->maxWeight;
}

struct TripletSTDP::Impl {
    float ltpWeight;
    float ltdWeight;
    float tauPre;      // Presynaptic time constant
    float tauPost;     // Postsynaptic time constant
    float tau;         // STD time constant
    float aPlus;       // LTP amplitude
    float aMinus;      // LTD amplitude
    float bPlus;       // Plus component (presynaptic contribution)
    float bMinus;      // Minus component (postsynaptic contribution)
    float minWeight;
    float maxWeight;
    
    // Triplet interaction variables
    std::unordered_map<Timestamp, float> preSpikeHistory;  // Timestamp -> presynaptic influence
    std::unordered_map<Timestamp, float> postSpikeHistory; // Timestamp -> postsynaptic influence
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), tauPre(20.0f), tauPost(20.0f), 
             tau(20.0f), aPlus(0.01f), aMinus(0.012f), bPlus(0.005f), bMinus(0.005f),
             minWeight(-1.0f), maxWeight(1.0f) {}
};

TripletSTDP::TripletSTDP() : pImpl(new Impl) {}

TripletSTDP::~TripletSTDP() = default;

void TripletSTDP::update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) {
    /*
     * Triplet STDP implementation
     * Advanced STDP incorporating triplet interactions (Gerstner et al., 1996)
     * Captures presynaptic spike history interactions that are not captured by pairwise STDP
     * 
     * Mathematical formulation (based on Gerstner et al., 1996):
     * For each pre-post spike pair with timing difference Δt = t_post - t_pre:
     * 
     * If Δt > 0 (pre before post): POTENTIATION
     *   Δw = a+ * exp(-Δt / τ) + b+ * Σ_{k} exp(-(t_post - t_post_k) / τ_post)
     * 
     * If Δt < 0 (post before pre): DEPRESSION
     *   Δw = -a- * exp(Δt / τ) - b- * Σ_{k} exp(-(t_pre - t_pre_k) / τ_pre)
     * 
     * Where:
     *   a+, a- = LTP/LTD amplitudes
     *   b+, b- = triplet interaction strengths
     *   τ, τ_pre, τ_post = time constants
     *   k = sum over all previous presynaptic/postsynaptic spikes
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    
    // Update presynaptic and postsynaptic spike history
    for (Timestamp preTime : preSpikes) {
        pImpl->preSpikeHistory[preTime] = 1.0f;
    }
    for (Timestamp postTime : postSpikes) {
        pImpl->postSpikeHistory[postTime] = 1.0f;
    }
    
    // Compute total weight change from all spike pairs
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                // Pairwise component
                float delta = pImpl->aPlus * std::exp(-dt / pImpl->tau);
                
                // Presynaptic triplet component (b+)
                for (const auto& prePair : pImpl->preSpikeHistory) {
                    if (prePair.first < preTime) {
                        float tSincePrePre = static_cast<float>(preTime - prePair.first);
                        delta += pImpl->bPlus * std::exp(-tSincePrePre / pImpl->tauPost);
                    }
                }
                
                totalDelta += delta;
            } else if (dt < 0) {
                // Post before pre: DEPRESSION
                // Pairwise component
                float delta = -pImpl->aMinus * std::exp(dt / pImpl->tau);
                
                // Postsynaptic triplet component (b-)
                for (const auto& postPair : pImpl->postSpikeHistory) {
                    if (postPair.first > postTime) {
                        float tSincePostPost = static_cast<float>(postPair.first - postTime);
                        delta -= pImpl->bMinus * std::exp(-tSincePostPost / pImpl->tauPre);
                    }
                }
                
                totalDelta += delta;
            }
        }
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        applyWeightChange(synapse, totalDelta);
    }
}

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* TripletSTDP::getName() const {
    return "TripletSTDP";
}

void TripletSTDP::setLTPWeight(float weight) {
    pImpl->ltpWeight = std::clamp(weight, 0.0f, 1.0f);
}

float TripletSTDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void TripletSTDP::setLTDWeight(float weight) {
    pImpl->ltdWeight = std::clamp(weight, 0.0f, 1.0f);
}

float TripletSTDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

void TripletSTDP::setTimeConstants(float tauPre, float tauPost, float tau) {
    pImpl->tauPre = std::clamp(tauPre, 1.0f, 100.0f);
    pImpl->tauPost = std::clamp(tauPost, 1.0f, 100.0f);
    pImpl->tau = std::clamp(tau, 1.0f, 100.0f);
}

void TripletSTDP::setTripletWeights(float aPlus, float aMinus, float bPlus, float bMinus) {
    pImpl->aPlus = std::clamp(aPlus, 0.0f, 0.1f);
    pImpl->aMinus = std::clamp(aMinus, 0.0f, 0.1f);
    pImpl->bPlus = std::clamp(bPlus, 0.0f, 0.05f);
    pImpl->bMinus = std::clamp(bMinus, 0.0f, 0.05f);
}

void TripletSTDP::configure(float ltpWeight, float ltdWeight, float tauPre, float tauPost, float tau, float aPlus, float aMinus, float bPlus, float bMinus) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstants(tauPre, tauPost, tau);
    setTripletWeights(aPlus, aMinus, bPlus, bMinus);
}

struct BCMRule::Impl {
    float learningRate;
    float beta;           // Stability parameter
    float slidingThreshold;
    float minWeight;
    float maxWeight;
    
    // Weight history for BCM rule
    std::unordered_map<Timestamp, float> weightHistory;
    
    Impl() : learningRate(0.01f), beta(0.01f), slidingThreshold(0.5f),
             minWeight(-1.0f), maxWeight(1.0f) {}
};

BCMRule::BCMRule() : pImpl(new Impl) {}

BCMRule::~BCMRule() = default;

void BCMRule::update(Synapse* synapse,
                    const std::vector<Timestamp>& preSpikes,
                    const std::vector<Timestamp>& postSpikes,
                    TimestepDuration dt) {
    /*
     * BCM (Bienenstock-Cooper-Munro) learning rule
     * 
     * Mathematical formulation:
     * 
     * For each pre-post spike pair with timing difference Δt = t_post - t_pre:
     *   If Δt > 0 (pre before post): Δw = Ω * (w_post - θ) if w_post > θ
     *   If Δt < 0 (post before pre): Δw = -γ * (w_post - θ) if w_post > θ
     *   
     * Where:
     *   Ω = learningRate * LTP weight
     *   γ = learningRate * LTD weight
     *   θ = sliding threshold (average postsynaptic activity)
     *   w_post = synaptic weight
     *   η = stability parameter
     * 
     * Biological inspiration:
     *   - Local synaptic competition
     *   - Homeostatic plasticity
     *   - Preventing runaway excitation
     *   - Stable memory traces
     *   
     * Use cases:
     *   - Cortical map development
     *   - Memory consolidation
     *   - Maintaining stable neural representations
     */
    
    if (!synapse || postSpikes.empty()) {
        return;
    }
    
    // Update sliding threshold based on recent postsynaptic activity
    // θ = (1-η) * θ + η * avg_post_activity
    float currentActivity = 0.0f;
    for (Timestamp spikeTime : postSpikes) {
        currentActivity += static_cast<float>(spikeTime);
    }
    currentActivity /= postSpikes.size();
    
    pImpl->slidingThreshold = (1.0f - pImpl->beta) * pImpl->slidingThreshold + 
                             pImpl->beta * currentActivity;
    
    // For each presynaptic spike, update synapse
    for (Timestamp preTime : preSpikes) {
        // Find most recent post spike after this pre spike
        float maxPostActivity = -1000.0f;
        for (Timestamp postTime : postSpikes) {
            if (postTime > preTime) {
                float postActivity = pImpl->weightHistory.find(postTime) != pImpl->weightHistory.end() ? 
                    pImpl->weightHistory.at(postTime) : 0.0f;
                if (postActivity > maxPostActivity) {
                    maxPostActivity = postActivity;
                }
            }
        }
        
        if (maxPostActivity < 0.0f) {
            continue;
        }
        
        float weightDifference = maxPostActivity - pImpl->slidingThreshold;
        
        // Store weight for potential LTD later
        pImpl->weightHistory[preTime] = maxPostActivity;
        
        if (weightDifference > 0.0f) {
            // LTP: positive weight change when post firing > threshold
            float delta = pImpl->learningRate * pImpl->learningRate * weightDifference;
            applyWeightChange(synapse, delta);
        } else if (weightDifference < 0.0f) {
            // LTD: negative weight change when post firing < threshold
            float delta = -pImpl->learningRate * pImpl->learningRate * std::abs(weightDifference);
            applyWeightChange(synapse, delta);
        }
    }
    
    // Clear history to prevent memory leaks
    pImpl->weightHistory.clear();
}

void BCMRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* BCMRule::getName() const {
    return "BCM";
}

void BCMRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 0.1f);
}

float BCMRule::getLearningRate() const {
    return pImpl->learningRate;
}

void BCMRule::setBeta(float beta) {
    pImpl->beta = std::clamp(beta, 0.0f, 0.1f);
}

float BCMRule::getBeta() const {
    return pImpl->beta;
}

void BCMRule::setTheta(float theta) {
    pImpl->slidingThreshold = std::clamp(theta, 0.0f, 1.0f);
}

float BCMRule::getTheta() const {
    return pImpl->slidingThreshold;
}

void BCMRule::setMinMaxWeights(float minWeight, float maxWeight) {
    pImpl->minWeight = std::clamp(minWeight, -10.0f, 0.0f);
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

void BCMRule::setMinWeight(float minWeight) {
    pImpl->minWeight = std::clamp(minWeight, -10.0f, 0.0f);
}

void BCMRule::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

float BCMRule::getMinWeight() const {
    return pImpl->minWeight;
}

float BCMRule::getMaxWeight() const {
    return pImpl->maxWeight;
}

void BCMRule::configure(float beta, float theta, float learningRate, float minWeight, float maxWeight) {
    setLearningRate(learningRate);
    setBeta(beta);
    setTheta(theta);
    setMinMaxWeights(minWeight, maxWeight);
}

} // namespace nlm