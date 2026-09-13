#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

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

// RewardModulatedSTDP implementation
struct RewardModulatedSTDP::Impl {
    float baseLtpWeight;
    float baseLtdWeight;
    float rewardGain;
    float baselineDopamine;
    float currentDopamine;
    
    Impl() : baseLtpWeight(0.01f), baseLtdWeight(0.012f), 
             rewardGain(1.0f), baselineDopamine(0.0f), currentDopamine(0.0f) {}
};

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(new Impl) {}

RewardModulatedSTDP::~RewardModulatedSTDP() = default;

void RewardModulatedSTDP::update(Synapse* synapse,
                                const std::vector<Timestamp>& preSpikes,
                                const std::vector<Timestamp>& postSpikes,
                                TimestepDuration dt) {
    if (!synapse) return;
    
    // Update STDP based on current dopamine level
    float effectiveLtp = pImpl->baseLtpWeight * (1.0f + pImpl->rewardGain * pImpl->currentDopamine);
    float effectiveLtd = pImpl->baseLtdWeight * (1.0f - pImpl->rewardGain * pImpl->currentDopamine);
    
    // Apply standard STDP with modulated weights
    // For simplicity, we'll use the same update logic as STDP but with modulated parameters
    float totalDelta = 0.0f;
    float tau = 20.0f;  // Default time constant
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                float delta = effectiveLtp * std::exp(-dt / tau);
                totalDelta += delta;
            } else if (dt < 0) {
                // Post before pre: DEPRESSION
                float delta = -effectiveLtd * std::exp(dt / tau);
                totalDelta += delta;
            }
        }
    }
    
    // Apply weight change
    if (std::abs(totalDelta) > 1e-6f) {
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta * pImpl->rewardGain);
    }
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply with bounds based on reward context
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);  // Typical weight range
    synapse->setWeight(newWeight);
}

void RewardModulatedSTDP::setBaseLTPWeight(float weight) {
    pImpl->baseLtpWeight = std::clamp(weight, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getBaseLTPWeight() const {
    return pImpl->baseLtpWeight;
}

void RewardModulatedSTDP::setBaseLTDWeight(float weight) {
    pImpl->baseLtdWeight = std::clamp(weight, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getBaseLTDWeight() const {
    return pImpl->baseLtdWeight;
}

void RewardModulatedSTDP::setRewardGain(float gain) {
    pImpl->rewardGain = std::clamp(gain, 0.0f, 10.0f);
}

float RewardModulatedSTDP::getRewardGain() const {
    return pImpl->rewardGain;
}

void RewardModulatedSTDP::setBaselineDopamine(float baseline) {
    pImpl->baselineDopamine = std::clamp(baseline, -1.0f, 1.0f);
}

float RewardModulatedSTDP::getBaselineDopamine() const {
    return pImpl->baselineDopamine;
}

void RewardModulatedSTDP::updateWithReward(float rewardPredictionError) {
    // Update dopamine level based on reward prediction error
    pImpl->currentDopamine = pImpl->baselineDopamine + rewardPredictionError;
    pImpl->currentDopamine = std::clamp(pImpl->currentDopamine, -1.0f, 1.0f);
}

// TripletSTDP implementation
struct TripletSTDP::Impl {
    float uPlus;
    float uMinus;
    float tauPlus;
    float tauMinus;
    float w;  // Synaptic modification rate
    
    Impl() : uPlus(0.005f), uMinus(0.004f), tauPlus(20.0f), tauMinus(20.0f), w(0.01f) {}
};

TripletSTDP::TripletSTDP() : pImpl(new Impl) {}

TripletSTDP::~TripletSTDP() = default;

void TripletSTDP::update(Synapse* synapse,
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) return;
    
    // Triplet STDP implementation based on Pfister & Gerstner (2006)
    // This model includes contributions from spike bursts (triples)
    
    float totalDelta = 0.0f;
    
    // Pairwise contributions (standard STDP component)
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                // LTP contribution
                totalDelta += pImpl->uPlus * std::exp(-dt / pImpl->tauPlus);
            } else if (dt < 0) {
                // LTD contribution
                totalDelta += -pImpl->uMinus * std::exp(dt / pImpl->tauMinus);
            }
        }
    }
    
    // For triplet STDP, we would also need to consider:
    // - Pre-prestrike -> pre-spike -> post-spike sequences
    // - Pre-spike -> post-prestrike -> post-spike sequences
    // However, this requires more complex spike history tracking
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        totalDelta *= pImpl->w;  // Scale by synaptic modification rate
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
    }
}

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply with biological constraints
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

void TripletSTDP::setUPlus(float u) {
    pImpl->uPlus = std::clamp(u, 0.0f, 1.0f);
}

float TripletSTDP::getUPlus() const {
    return pImpl->uPlus;
}

void TripletSTDP::setUMinus(float u) {
    pImpl->uMinus = std::clamp(u, 0.0f, 1.0f);
}

float TripletSTDP::getUMinus() const {
    return pImpl->uMinus;
}

void TripletSTDP::setTaup(float tau) {
    pImpl->tauPlus = std::clamp(tau, 1.0f, 100.0f);
}

float TripletSTDP::getTaup() const {
    return pImpl->tauPlus;
}

void TripletSTDP::setTauMinus(float tau) {
    pImpl->tauMinus = std::clamp(tau, 1.0f, 100.0f);
}

float TripletSTDP::getTauMinus() const {
    return pImpl->tauMinus;
}

void TripletSTDP::setBCMWeight(float w) {
    pImpl->w = std::clamp(w, 0.0f, 1.0f);
}

float TripletSTDP::getBCMWeight() const {
    return pImpl->w;
}

// AntiHebbianRule implementation
struct AntiHebbianRule::Impl {
    float learningRate;
    
    Impl() : learningRate(0.001f) {}
};

AntiHebbianRule::AntiHebbianRule() : pImpl(new Impl) {}

AntiHebbianRule::~AntiHebbianRule() = default;

void AntiHebbianRule::update(Synapse* synapse,
                            const std::vector<Timestamp>& preSpikes,
                            const std::vector<Timestamp>& postSpikes,
                            TimestepDuration dt) {
    if (!synapse) return;
    
    // Anti-Hebbian plasticity: weaken synapse when neurons fire together
    // This is the opposite of standard Hebbian learning
    
    float totalDelta = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            // Anti-Hebbian: weaken connection when pre and post fire close in time
            // This is similar to LTD in standard STDP but with different rules
            float dt = static_cast<float>(postTime - preTime);
            
            if (std::abs(dt) < 50.0f) {  // Within 50ms of each other
                // Weaken the connection
                totalDelta -= pImpl->learningRate * std::exp(-std::abs(dt) / 10.0f);
            }
        }
    }
    
    // Apply weight change
    if (std::abs(totalDelta) > 1e-6f) {
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
    }
}

void AntiHebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply with bounds (typically anti-Hebbian weights are negative or zero)
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 0.5f);  // Anti-Hebbian typically doesn't go positive
    synapse->setWeight(newWeight);
}

void AntiHebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float AntiHebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

// BCMRule implementation
struct BCMRule::Impl {
    float theta;  // Sliding threshold
    float tau;    // Threshold dynamics time constant
    float w;      // Synaptic modification rate
    float eta;    // Learning rate
    float currentTheta;  // Current threshold value
    
    Impl() : theta(25.0f), tau(1000.0f), w(0.01f), eta(0.01f), currentTheta(theta) {}
};

BCMRule::BCMRule() : pImpl(new Impl) {}

BCMRule::~BCMRule() = default;

void BCMRule::update(Synapse* synapse,
                    const std::vector<Timestamp>& preSpikes,
                    const std::vector<Timestamp>& postSpikes,
                    TimestepDuration dt) {
    if (!synapse) return;
    
    // BCM (Bienenstock-Cooper-Munro) rule: sliding threshold theory
    // Learning occurs when post-synaptic activity exceeds a sliding threshold (theta)
    
    float postActivity = 0.0f;
    for (Timestamp postTime : postSpikes) {
        // Simple approximation of post-synaptic activity
        postActivity += 1.0f;
    }
    
    // BCM learning rule: Δw = η * post * (post - θ)
    float delta = pImpl->eta * postActivity * (postActivity - pImpl->currentTheta);
    
    // Update sliding threshold
    pImpl->currentTheta += dt * (postActivity - pImpl->currentTheta) / pImpl->tau;
    
    // Apply weight change if learning occurs
    if (std::abs(delta) > 1e-6f) {
        // Apply synaptic weight change
        applyWeightChange(synapse, delta);
        
        // Update eligibility trace
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + delta);
    }
}

void BCMRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply with biological constraints
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

void BCMRule::setTheta(float theta) {
    pImpl->theta = std::clamp(theta, 0.0f, 100.0f);
    pImpl->currentTheta = theta;
}

float BCMRule::getTheta() const {
    return pImpl->theta;
}

void BCMRule::setTau(float tau) {
    pImpl->tau = std::clamp(tau, 10.0f, 10000.0f);
}

float BCMRule::getTau() const {
    return pImpl->tau;
}

void BCMRule::setBCMWeight(float w) {
    pImpl->w = std::clamp(w, 0.0f, 1.0f);
}

float BCMRule::getBCMWeight() const {
    return pImpl->w;
}

} // namespace nlm