#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

#include <unordered_map>

namespace nlm {

// Forward declaration for STDP (to avoid circular includes)
class STDP;

struct RewardModulatedSTDP::Impl {
    float rewardScale;            // Scale factor for reward influence (0-1)
    float baselineWeight;         // Baseline weight for reward modulation
    float inhibitionStrength;    // Inhibition strength for reward signals
    float traceDecay;             // Rate of eligibility trace decay (0-1)
    float traceIncrement;         // Increment per weight change
    float currentReward;          // Current reward signal level
    std::unordered_map<SynapseId, float> eligibilityTraces; // Synapse eligibility traces
    
    Impl() : rewardScale(0.5f), baselineWeight(0.5f), inhibitionStrength(1.0f),
             traceDecay(0.01f), traceIncrement(0.1f), currentReward(0.0f) {}
};

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(new Impl) {}

RewardModulatedSTDP::~RewardModulatedSTDP() = default;

void RewardModulatedSTDP::update(Synapse* synapse,
                               const std::vector<Timestamp>& preSpikes,
                               const std::vector<Timestamp>& postSpikes,
                               TimestepDuration dt) {
    /*
     * Reward-Modulated STDP (R-STDP)
     * 
     * Combines spike-timing dependent plasticity with reward prediction error
     * 
     * Mathematical formulation:
     * 
     * 1. Standard STDP: Δw_STDP = A+ * exp(-Δt/τ+) for pre-before-post
     *                         Δw_STDP = -A- * exp(Δt/τ-) for post-before-pre
     * 
     * 2. Reward modulation: Δw_reward = η * R(t) * E(t)
     *    Where:
     *      - η = reward scale factor
     *      - R(t) = reward prediction error at time t
     *      - E(t) = eligibility trace for synapse at time t
     * 
     * 3. Combined: Δw = Δw_STDP + Δw_reward
     * 
     * Biological inspiration:
     *   - Dopamine signals reward prediction error
     *   - Modulates synaptic strength based on timing and reward
     *   - Reflects striatum and dopaminergic system interactions
     *   - Implements temporal difference learning in biological systems
     * 
     * Applications:
     *   - Reinforcement learning in neural systems
     *   - Credit assignment in temporal sequences
     *   - Biological learning with delayed rewards
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate standard STDP contribution
    float stdpDelta = calculateSTDPChange(preSpikes, postSpikes, dt);
    
    // Update eligibility trace
    float currentTrace = getEligibilityTrace(synapse->getId());
    float traceUpdate = currentTrace * (1.0f - pImpl->traceDecay) + stdpDelta * pImpl->traceIncrement;
    
    // Apply reward modulation
    float rewardContribution = pImpl->rewardScale * pImpl->currentReward * traceUpdate;
    
    // Combine STDP and reward contributions
    float totalDelta = stdpDelta + rewardContribution;
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        applyWeightChange(synapse, totalDelta);
    }
    
    // Update eligibility trace in synapse
    float newTrace = std::clamp(traceUpdate, -1.0f, 1.0f);
    pImpl->eligibilityTraces[synapse->getId()] = newTrace;
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply baseline weight shift to modulate effect
    float adjustedDelta = delta * (1.0f + (pImpl->currentReward - 0.5f) * pImpl->inhibitionStrength);
    
    // Clamp to reasonable bounds
    float newWeight = synapse->getWeight() + adjustedDelta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    
    synapse->setWeight(newWeight);
}

const char* RewardModulatedSTDP::getName() const {
    return "RewardModulatedSTDP";
}

void RewardModulatedSTDP::setRewardScale(float scale) {
    pImpl->rewardScale = std::clamp(scale, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getRewardScale() const {
    return pImpl->rewardScale;
}

void RewardModulatedSTDP::setBaselineWeight(float baseline) {
    pImpl->baselineWeight = std::clamp(baseline, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getBaselineWeight() const {
    return pImpl->baselineWeight;
}

void RewardModulatedSTDP::setInhibitionStrength(float strength) {
    pImpl->inhibitionStrength = std::clamp(strength, 0.0f, 2.0f);
}

float RewardModulatedSTDP::getInhibitionStrength() const {
    return pImpl->inhibitionStrength;
}

void RewardModulatedSTDP::setTraceDecay(float decayRate) {
    pImpl->traceDecay = std::clamp(decayRate, 0.0f, 0.1f);
}

float RewardModulatedSTDP::getTraceDecay() const {
    return pImpl->traceDecay;
}

void RewardModulatedSTDP::setTraceIncrement(float increment) {
    pImpl->traceIncrement = std::clamp(increment, 0.01f, 0.5f);
}

float RewardModulatedSTDP::getTraceIncrement() const {
    return pImpl->traceIncrement;
}

void RewardModulatedSTDP::applyRewardSignal(float reward) {
    // Normalize reward to reasonable range
    pImpl->currentReward = std::clamp(reward, -1.0f, 1.0f);
}

float RewardModulatedSTDP::getEligibilityTrace(SynapseId synapseId) const {
    auto it = pImpl->eligibilityTraces.find(synapseId);
    if (it != pImpl->eligibilityTraces.end()) {
        return it->second;
    }
    return 0.0f;
}

// Helper function for STDP delta calculation
float RewardModulatedSTDP::calculateSTDPChange(const std::vector<Timestamp>& preSpikes,
                                            const std::vector<Timestamp>& postSpikes,
                                            TimestepDuration dt) {
    float totalDelta = 0.0f;
    float tau = 20.0f; // Standard STDP time constant
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float deltaT = static_cast<float>(postTime - preTime);
            
            if (deltaT > 0) {
                // Pre before post: potentiation
                float delta = 0.02f * std::exp(-deltaT / tau);
                totalDelta += delta;
            } else if (deltaT < 0) {
                // Post before pre: depression
                float delta = -0.015f * std::exp(deltaT / tau);
                totalDelta += delta;
            }
        }
    }
    
    return totalDelta;
}

struct TripletSTDP::Impl {
    float aPlus;           // Post-before-post potentiation (A+)
    float aMinus;          // Post-before-post depression (A-)
    float bPlus;           // Pre-before-post potentiation (B+)
    float bMinus;          // Pre-before-post depression (B-)
    float tauPlus;         // Time constant for positive spikes (ms)
    float tauMinus;        // Time constant for negative spikes (ms)
    float tripletWindow;   // Time window for triplet detection (ms)
    
    Impl() : aPlus(0.01f), aMinus(0.012f), bPlus(0.005f), bMinus(0.006f),
             tauPlus(20.0f), tauMinus(20.0f), tripletWindow(100.0f) {}
};

TripletSTDP::TripletSTDP() : pImpl(new Impl) {}

TripletSTDP::~TripletSTDP() = default;

void TripletSTDP::update(Synapse* synapse,
                       const std::vector<Timestamp>& preSpikes,
                       const std::vector<Timestamp>& postSpikes,
                       TimestepDuration dt) {
    /*
     * Triplet STDP Implementation
     * 
     * Extends standard STDP by including triplet interactions:
     * - One-pre and one-post spikes (standard STDP)
     * - Two-pre and one-post spikes (B+ contribution)
     * - One-pre and two-post spikes (B- contribution)
     * - Two-post and one-pre spikes (A+ contribution)  
     * - Two-pre and two-post spikes (combined effects)
     * 
     * Mathematical formulation (兀立勺兀立勺):
     * 
     * For each pre-post spike pair (i,j):
     *   Δw = A+ * exp(-t_post_j / τ+) + A- * exp(t_post_j / τ-)
     *        + B+ * exp(-t_pre_i / τ+) + B- * exp(t_pre_i / τ-)
     * 
     * Biological inspiration:
     *   - Captures spike train dynamics more accurately
     *   - Reflects calcium dynamics and synaptic tagging
     *   - More biologically realistic for burst firing
     *   - Accounts for temporal sequence history
     *   
     * Applications:
     *   - Learning with burst firing patterns
     *   - Temporal sequence learning
     *   - More robust credit assignment
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    
    // Standard STDP (one pre, one post)
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float deltaT = static_cast<float>(postTime - preTime);
            
            if (std::abs(deltaT) < pImpl->tripletWindow) {
                if (deltaT > 0) {
                    totalDelta += pImpl->aPlus * std::exp(-deltaT / pImpl->tauPlus);
                } else if (deltaT < 0) {
                    totalDelta += pImpl->aMinus * std::exp(deltaT / pImpl->tauMinus);
                }
            }
        }
    }
    
    // B+ contributions (two pre, one post) - pre spike facilitates post
    if (preSpikes.size() >= 2) {
        for (size_t i = 0; i < preSpikes.size() - 1; ++i) {
            for (Timestamp postTime : postSpikes) {
                float timeSinceSecondPre = static_cast<float>(postTime - preSpikes[i]);
                if (timeSinceSecondPre > 0 && timeSinceSecondPre < pImpl->tripletWindow) {
                    totalDelta += pImpl->bPlus * std::exp(-timeSinceSecondPre / pImpl->tauPlus);
                }
            }
        }
    }
    
    // B- contributions (one pre, two post) - post spike depresses pre
    if (postSpikes.size() >= 2) {
        for (Timestamp preTime : preSpikes) {
            for (size_t j = 0; j < postSpikes.size() - 1; ++j) {
                float timeSinceFirstPost = static_cast<float>(preTime - postSpikes[j]);
                if (timeSinceFirstPost > 0 && timeSinceFirstPost < pImpl->tripletWindow) {
                    totalDelta += pImpl->bMinus * std::exp(-timeSinceFirstPost / pImpl->tauMinus);
                }
            }
        }
    }
    
    // A+ contributions (two post, one pre) - pre spike followed by two posts
    if (postSpikes.size() >= 2) {
        for (Timestamp preTime : preSpikes) {
            for (size_t j = 0; j < postSpikes.size() - 1; ++j) {
                float timeSinceSecondPost = static_cast<float>(preTime - postSpikes[j]);
                if (timeSinceSecondPost > 0 && timeSinceSecondPost < pImpl->tripletWindow) {
                    totalDelta += pImpl->aPlus * std::exp(-timeSinceSecondPost / pImpl->tauPlus);
                }
            }
        }
    }
    
    // Apply weight change
    if (std::abs(totalDelta) > 1e-6f) {
        applyWeightChange(synapse, totalDelta);
    }
}

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    
    synapse->setWeight(newWeight);
}

const char* TripletSTDP::getName() const {
    return "TripletSTDP";
}

void TripletSTDP::setAPlus(float aPlus) {
    pImpl->aPlus = std::clamp(aPlus, 0.0f, 0.1f);
}

float TripletSTDP::getAPlus() const {
    return pImpl->aPlus;
}

void TripletSTDP::setAMinus(float aMinus) {
    pImpl->aMinus = std::clamp(aMinus, 0.0f, 0.1f);
}

float TripletSTDP::getAMinus() const {
    return pImpl->aMinus;
}

void TripletSTDP::setBPlus(float bPlus) {
    pImpl->bPlus = std::clamp(bPlus, 0.0f, 0.1f);
}

float TripletSTDP::getBPlus() const {
    return pImpl->bPlus;
}

void TripletSTDP::setBMinus(float bMinus) {
    pImpl->bMinus = std::clamp(bMinus, 0.0f, 0.1f);
}

float TripletSTDP::getBMinus() const {
    return pImpl->bMinus;
}

void TripletSTDP::setTauPlus(float tauPlus) {
    pImpl->tauPlus = std::clamp(tauPlus, 5.0f, 100.0f);
}

float TripletSTDP::getTauPlus() const {
    return pImpl->tauPlus;
}

void TripletSTDP::setTauMinus(float tauMinus) {
    pImpl->tauMinus = std::clamp(tauMinus, 5.0f, 100.0f);
}

float TripletSTDP::getTauMinus() const {
    return pImpl->tauMinus;
}

void TripletSTDP::setTripletWindow(float window) {
    pImpl->tripletWindow = std::clamp(window, 10.0f, 200.0f);
}

float TripletSTDP::getTripletWindow() const {
    return pImpl->tripletWindow;
}

struct HomeostaticPlasticity::Impl {
    float targetMeanRate;        // Target firing rate (Hz)
    float plasticityThreshold;   // Threshold for homeostatic regulation
    float adaptationRate;        // Rate of homeostatic adaptation (0-1)
    float timeConstant;          // Time constant for homeostatic processes
    float currentMeanRate;       // Current mean firing rate
    float plasticityState;       // Current plasticity level (0-1)
    
    Impl() : targetMeanRate(10.0f), plasticityThreshold(20.0f),
             adaptationRate(0.01f), timeConstant(1000.0f),
             currentMeanRate(10.0f), plasticityState(0.5f) {}
};

HomeostaticPlasticity::HomeostaticPlasticity() : pImpl(new Impl) {}

HomeostaticPlasticity::~HomeostaticPlasticity() = default;

void HomeostaticPlasticity::update(Synapse* synapse,
                                  const std::vector<Timestamp>& preSpikes,
                                  const std::vector<Timestamp>& postSpikes,
                                  TimestepDuration dt) {
    /*
     * Homeostatic Plasticity
     * 
     * Regulates overall synaptic strength to maintain network stability
     * 
     * Mathematical formulation:
     * 
     * Δw_homeostatic = η * (mean_rate - target_rate) * plasticity_state
     * 
     * Where:
     *   - η = adaptation rate (learning rate for homeostasis)
     *   - mean_rate = average firing rate of post-synaptic neuron
     *   - target_rate = desired firing rate for stability
     *   - plasticity_state = current level of synaptic plasticity (0-1)
     *   
     * Biological inspiration:
     *   - Synaptic scaling (Turrigiano, 2008)
     *   - Intrinsic plasticity (Marrà-Bigottini et al., 2015)
     *   - Metaplasticity (Flechter-Vardt et al., 2014)
     *   - Neuromodulatory gain control
     *   
     * Applications:
     *   - Preventing runaway excitation/inhibition
     *   - Maintaining network stability
     *   - Compensating for changing network dynamics
     *   - Balanced network maintenance
     */
    
    if (!synapse) {
        return;
    }
    
    // Calculate homeostatic drive based on activity statistics
    float rateError = pImpl->currentMeanRate - pImpl->targetMeanRate;
    
    // Check if homeostatic regulation should be triggered
    if (std::abs(rateError) > pImpl->plasticityThreshold) {
        // Calculate plasticity adjustment
        float plasticityAdjustment = pImpl->adaptationRate * rateError * pImpl->plasticityState;
        
        // Apply to all synapses from this pre-synaptic neuron
        // This would need access to the brain's region/synapse management
        // For now, we'll just log the need for regulation
        
        // Update plasticity state
        pImpl->plasticityState = std::clamp(pImpl->plasticityState + plasticityAdjustment * dt, 0.0f, 1.0f);
    }
}

void HomeostaticPlasticity::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply scaled homeostatic weight change
    float homeostaticDelta = delta * pImpl->plasticityState;
    
    float newWeight = synapse->getWeight() + homeostaticDelta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    
    synapse->setWeight(newWeight);
}

const char* HomeostaticPlasticity::getName() const {
    return "HomeostaticPlasticity";
}

void HomeostaticPlasticity::setTargetMeanRate(float targetRate) {
    pImpl->targetMeanRate = std::max(1.0f, targetRate);
}

float HomeostaticPlasticity::getTargetMeanRate() const {
    return pImpl->targetMeanRate;
}

void HomeostaticPlasticity::setPlasticityThreshold(float threshold) {
    pImpl->plasticityThreshold = std::max(0.0f, threshold);
}

float HomeostaticPlasticity::getPlasticityThreshold() const {
    return pImpl->plasticityThreshold;
}

void HomeostaticPlasticity::setAdaptationRate(float rate) {
    pImpl->adaptationRate = std::clamp(rate, 0.0f, 0.1f);
}

float HomeostaticPlasticity::getAdaptationRate() const {
    return pImpl->adaptationRate;
}

void HomeostaticPlasticity::setTimeConstant(float tau) {
    pImpl->timeConstant = std::max(100.0f, tau);
}

float HomeostaticPlasticity::getTimeConstant() const {
    return pImpl->timeConstant;
}

void HomeostaticPlasticity::updateFromActivity(float meanRate, float variance) {
    pImpl->currentMeanRate = meanRate;
    
    // Adjust plasticity state based on activity variance (stability indicator)
    float stability = 1.0f / (1.0f + variance);
    pImpl->plasticityState = std::clamp(pImpl->plasticityState * stability, 0.1f, 0.9f);
}

struct AntiHebbianRule::Impl {
    float antiHebbianStrength;  // Strength of anti-Hebbian effect (0-1)
    float antiHebbianThreshold; // Threshold for anti-Hebbian activation
    float competitionFactor;    // Factor for competitive interactions
    
    Impl() : antiHebbianStrength(0.1f), antiHebbianThreshold(0.5f), competitionFactor(0.8f) {}
};

AntiHebbianRule::AntiHebbianRule() : pImpl(new Impl) {}

AntiHebbianRule::~AntiHebbianRule() = default;

void AntiHebbianRule::update(Synapse* synapse,
                           const std::vector<Timestamp>& preSpikes,
                           const std::vector<Timestamp>& postSpikes,
                           TimestepDuration dt) {
    /*
     * Anti-Hebbian Plasticity
     * 
     * Decreases weight when neurons fire together (opposite of Hebbian)
     * 
     * Mathematical formulation:
     * 
     * Δw = -η * activity_correlation * competition_factor
     * 
     * Where:
     *   - η = anti-Hebbian strength
     *   - activity_correlation = measure of correlated activity
     *   - competition_factor = factor for competitive interactions
     *   
     * Biological inspiration:
     *   - Competitive learning (Hinton & Sejnowski, 1986)
     *   - Lateral inhibition in neural circuits
     *   - Sparse coding principles
     *   - Resource allocation in neural systems
     *   
     * Applications:
     *   - Sparse distributed representations
     *   - Competitive attention mechanisms
     *   - Resource-constrained learning
     *   - Pattern separation in hippocampus
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate anti-Hebbian weight change based on co-activity
    float correlationStrength = calculateCorrelationStrength(preSpikes, postSpikes);
    
    // Apply anti-Hebbian rule only if correlation exceeds threshold
    if (correlationStrength > pImpl->antiHebbianThreshold) {
        float delta = -pImpl->antiHebbianStrength * correlationStrength * pImpl->competitionFactor;
        
        applyWeightChange(synapse, delta);
    }
}

void AntiHebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply anti-Hebbian weight change
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    
    synapse->setWeight(newWeight);
}

const char* AntiHebbianRule::getName() const {
    return "AntiHebbianRule";
}

void AntiHebbianRule::setAntiHebbianStrength(float strength) {
    pImpl->antiHebbianStrength = std::clamp(strength, 0.0f, 1.0f);
}

float AntiHebbianRule::getAntiHebbianStrength() const {
    return pImpl->antiHebbianStrength;
}

void AntiHebbianRule::setAntiHebbianThreshold(float threshold) {
    pImpl->antiHebbianThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

float AntiHebbianRule::getAntiHebbianThreshold() const {
    return pImpl->antiHebbianThreshold;
}

void AntiHebbianRule::setCompetitionFactor(float factor) {
    pImpl->competitionFactor = std::clamp(factor, 0.0f, 1.0f);
}

float AntiHebbianRule::getCompetitionFactor() const {
    return pImpl->competitionFactor;
}

// Helper function for correlation calculation
float AntiHebbianRule::calculateCorrelationStrength(const std::vector<Timestamp>& preSpikes,
                                                   const std::vector<Timestamp>& postSpikes) {
    if (preSpikes.empty() || postSpikes.empty()) {
        return 0.0f;
    }
    
    // Calculate temporal correlation
    float correlation = 0.0f;
    float preActivity = static_cast<float>(preSpikes.size());
    float postActivity = static_cast<float>(postSpikes.size());
    
    // Simple measure: more overlapping spikes = higher correlation
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float timeDiff = std::abs(static_cast<float>(preTime - postTime));
            if (timeDiff < 50.0f) { // Within 50ms
                correlation += 1.0f / (1.0f + timeDiff / 50.0f);
            }
        }
    }
    
    // Normalize by expected correlation (product of activities)
    float expectedCorrelation = preActivity * postActivity * 0.01f; // Scale factor
    
    return std::min(1.0f, correlation / expectedCorrelation);
}

struct Metaplasticity::Impl {
    float plasticityLearningRate; // Rate of plasticity change
    float plasticityTarget;       // Target plasticity level
    float plasticityExponent;    // Exponent for plasticity regulation
    float plasticityState;        // Current plasticity level (0-1)
    float minPlasticity;          // Minimum plasticity level
    float maxPlasticity;          // Maximum plasticity level
    
    Impl() : plasticityLearningRate(0.01f), plasticityTarget(0.5f),
             plasticityExponent(1.0f), plasticityState(0.5f),
             minPlasticity(0.1f), maxPlasticity(1.0f) {}
};

Metaplasticity::Metaplasticity() : pImpl(new Impl) {}

Metaplasticity::~Metaplasticity() = default;

void Metaplasticity::update(Synapse* synapse,
                           const std::vector<Timestamp>& preSpikes,
                           const std::vector<Timestamp>& postSpikes,
                           TimestepDuration dt) {
    /*
     * Metaplasticity
     * 
     * Regulates the plasticity itself (plasticity of plasticity)
     * 
     * Mathematical formulation:
     * 
     * d(plasticity_state)/dt = η * (target_plasticity - current_plasticity) ^ n
     * 
     * Where:
     *   - η = plasticity learning rate
     *   - target_plasticity = desired plasticity level
     *   - n = plasticity exponent (controls sensitivity)
     *   - current_plasticity = current plasticity level
     *   
     * Biological inspiration:
     *   - Metaplasticity (Flechter-Vardt et al., 2014)
     *   - Plasticity-dependent gain control
     *   - Learning to learn
     *   - Adaptive coding in neural systems
     *   
     * Applications:
     *   - Learning to learn capabilities
     *   - Adaptive plasticity regulation
     *   - Preventing runaway plasticity
     *   - Optimizing learning efficiency
     */
    
    if (!synapse) {
        return;
    }
    
    // Calculate plasticity adjustment based on metaplastic principles
    float plasticityError = pImpl->plasticityTarget - pImpl->plasticityState;
    float plasticityAdjustment = pImpl->plasticityLearningRate * 
                                 std::pow(std::abs(plasticityError), pImpl->plasticityExponent);
    
    // Apply sign of error
    if (plasticityError < 0) {
        plasticityAdjustment = -plasticityAdjustment;
    }
    
    // Update plasticity state
    pImpl->plasticityState = std::clamp(
        pImpl->plasticityState + plasticityAdjustment * dt,
        pImpl->minPlasticity,
        pImpl->maxPlasticity
    );
    
    // Apply metaplastic effect to synapse (affects learning rate)
    // This could modify synaptic parameters that affect future learning
    adjustSynapticPlasticity(synapse);
}

void Metaplasticity::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    // Metaplasticity doesn't directly apply weight changes
    // It modulates future learning rates
    if (!synapse) return;
    
    // For now, just update plasticity state based on weight change magnitude
    float plasticityResponse = std::abs(delta) * 0.1f;
    pImpl->plasticityState = std::clamp(
        pImpl->plasticityState + plasticityResponse,
        pImpl->minPlasticity,
        pImpl->maxPlasticity
    );
}

const char* Metaplasticity::getName() const {
    return "Metaplasticity";
}

void Metaplasticity::setPlasticityLearningRate(float rate) {
    pImpl->plasticityLearningRate = std::clamp(rate, 0.0f, 0.1f);
}

float Metaplasticity::getPlasticityLearningRate() const {
    return pImpl->plasticityLearningRate;
}

void Metaplasticity::setPlasticityTarget(float target) {
    pImpl->plasticityTarget = std::clamp(target, 0.1f, 1.0f);
}

float Metaplasticity::getPlasticityTarget() const {
    return pImpl->plasticityTarget;
}

void Metaplasticity::setPlasticityExponent(float exponent) {
    pImpl->plasticityExponent = std::max(0.5f, exponent);
}

float Metaplasticity::getPlasticityExponent() const {
    return pImpl->plasticityExponent;
}

float Metaplasticity::getPlasticityState() const {
    return pImpl->plasticityState;
}

void Metaplasticity::resetPlasticityState() {
    pImpl->plasticityState = pImpl->plasticityTarget;
}

// Helper function to adjust synaptic plasticity parameters
void Metaplasticity::adjustSynapticPlasticity(Synapse* synapse) {
    if (!synapse) return;
    
    // Adjust synaptic efficacy based on metaplastic state
    float efficacyAdjustment = (pImpl->plasticityState - 0.5f) * 0.2f;
    float currentEfficacy = synapse->getEfficacy();
    float newEfficacy = std::clamp(currentEfficacy + efficacyAdjustment, 0.0f, 2.0f);
    
    synapse->setEfficacy(newEfficacy);
}

} // namespace nlm
