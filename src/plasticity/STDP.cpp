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

// Enhanced TD learning with eligibility traces
void STDP::temporalDifferenceLearning(Synapse* synapse,
                                     float predictionError,
                                     float reward,
                                     TimestepDuration dt,
                                     float learningRate) {
    /*
     * Temporal Difference Learning with Eligibility Traces
     * 
     * TD(λ) learning combines advantages of Monte Carlo (MC) and
     * Temporal Difference (TD) learning by using eligibility traces
     * to distribute credit across multiple previous states and actions.
     * 
     * Mathematical formulation:
     *   Δw = α * δ * e(t)  (for synaptic weights)
     *   e(t) = γ * λ * e(t-1) + φ (eligibility trace update)
     *   
     * Where:
     *   α = learning rate
     *   δ = prediction error (reward - predicted value)
     *   e(t) = eligibility trace
     *   γ = discount factor
     *   λ = trace decay parameter
     *   φ = spike-triggered eligibility increment
     */
    
    if (!synapse || std::abs(predictionError) < 1e-6f) {
        return;
    }
    
    float dtf = static_cast<float>(dt);
    
    // TD learning update rule
    // Weight change proportional to prediction error and eligibility trace
    float weightChange = learningRate * predictionError * synapse->getEligibilityTrace();
    
    // Apply the weight change with biological constraints
    applyWeightChange(synapse, weightChange);
    
    // Update eligibility trace with decay
    float currentTrace = synapse->getEligibilityTrace();
    float decayFactor = 0.95f * dtf * 10.0f; // Decay over time
    float traceUpdate = currentTrace * decayFactor;
    
    // Add eligibility trace for recent spikes (simplified)
    float spikeActivity = synapse->getRecentSpikeActivity();
    traceUpdate += spikeActivity * 0.1f;
    
    // Clamp eligibility trace to reasonable bounds
    traceUpdate = std::clamp(traceUpdate, 0.0f, 1.0f);
    synapse->setEligibilityTrace(traceUpdate);
}

// Bayesian synaptic learning with uncertainty quantification
void STDP::bayesianLearning(Synapse* synapse,
                           float predictionError,
                           float uncertainty,
                           float learningRate,
                           float priorStrength) {
    /*
     * Bayesian Synaptic Learning
     * 
     * Implements Bayesian inference for synaptic weight updates,
     * explicitly accounting for prediction uncertainty.
     * 
     * Mathematical formulation:
     *   w_new ~ N(w_old + Δw, Σ)  where Σ incorporates uncertainty
     *   Δw = (learningRate / (uncertainty + ε)) * predictionError
     *   
     * Where:
     *   w_new = new synaptic weight
     *   w_old = current synaptic weight
     *   Σ = uncertainty covariance matrix
     *   ε = small regularization to prevent division by zero
     */
    
    if (!synapse || std::abs(predictionError) < 1e-6f) {
        return;
    }
    
    // Bayesian update rule
    // Weight change inversely proportional to uncertainty
    // High uncertainty = smaller, more cautious updates
    // Low uncertainty = larger, more confident updates
    
    float uncertaintyGain = learningRate / (uncertainty + 0.01f);
    float weightChange = uncertaintyGain * predictionError;
    
    // Apply Bayesian weight change with additional prior influence
    weightChange *= (1.0f + priorStrength * 0.1f);
    
    // Apply the weight change with biological constraints
    applyWeightChange(synapse, weightChange);
    
    // Update uncertainty estimate (learning reduces uncertainty)
    float currentUncertainty = synapse->getUncertainty();
    float uncertaintyUpdate = currentUncertainty * (1.0f - learningRate * 0.05f);
    synapse->setUncertainty(uncertaintyUpdate);
}

// Meta-learning: adaptive learning rate adjustment
void STDP::metaLearning(Synapse* synapse,
                       float predictionError,
                       float learningRate,
                       float targetError,
                       float adaptationRate) {
    /*
     * Meta-Learning for Adaptive Learning Rates
     * 
     * Implements meta-learning where the learning rate itself
     * is learned based on prediction error patterns.
     * 
     * Mathematical formulation:
     *   α_new = α_old + η * (δ² - target²) * sign(δ)
     *   
     * Where:
     *   α_new = new learning rate
     *   α_old = current learning rate
     *   η = meta-learning rate
     *   δ = prediction error
     *   target = desired error magnitude
     */
    
    if (!synapse) {
        return;
    }
    
    // Meta-learning update for learning rate
    float errorSquared = predictionError * predictionError;
    float errorChange = (errorSquared - targetError * targetError) * 
                       (predictionError >= 0.0f ? 1.0f : -1.0f);
    
    float newLearningRate = learningRate + adaptationRate * errorChange;
    
    // Clamp learning rate to reasonable bounds
    newLearningRate = std::clamp(newLearningRate, 0.01f, 0.5f);
    
    // Apply meta-learning to synapse
    synapse->setLearningRate(newLearningRate);
}

// Online learning with streaming data
void STDP::onlineLearning(Synapse* synapse,
                         float streamingPredictionError,
                         TimestepDuration dt,
                         float momentum) {
    /*
     * Online Learning with Streaming Data
     * 
     * Implements online learning capabilities for continuous
     * adaptation to streaming data patterns.
     * 
     * Mathematical formulation:
     *   Δw = α * δ * (1 - |w|) * momentum
     *   
     * Where:
     *   Δw = online weight change
     *   α = online learning rate
     *   δ = streaming prediction error
     *   w = current synaptic weight
     *   momentum = adaptive momentum term
     */
    
    if (!synapse || std::abs(streamingPredictionError) < 1e-6f) {
        return;
    }
    
    float dtf = static_cast<float>(dt);
    
    // Online learning update rule
    float weightChange = synapse->getLearningRate() * streamingPredictionError *
                        (1.0f - std::abs(synapse->getWeight())) *
                        momentum * dtf * 0.1f;
    
    // Apply online weight change
    applyWeightChange(synapse, weightChange);
    
    // Update momentum based on recent changes
    float currentMomentum = synapse->getMomentum();
    float momentumUpdate = currentMomentum * 0.99f + 
                          std::abs(weightChange) * 0.01f;
    synapse->setMomentum(momentumUpdate);
}

// Configure advanced learning algorithms
void STDP::configureAdvancedLearning(float tdLearningRate,
                                    float bayesianUncertaintyGain,
                                    float metaLearningRate,
                                    float onlineMomentum) {
    // Store advanced learning parameters
    advancedLearningParams.tdLearningRate = tdLearningRate;
    advancedLearningParams.bayesianUncertaintyGain = bayesianUncertaintyGain;
    advancedLearningParams.metaLearningRate = metaLearningRate;
    advancedLearningParams.onlineMomentum = onlineMomentum;
    
    // Enable advanced learning modes
    enableTDLearning = true;
    enableBayesianLearning = true;
    enableMetaLearning = true;
    enableOnlineLearning = true;
}

// Get advanced learning parameters
void STDP::getAdvancedLearningParams(AdvancedLearningParams& params) const {
    params = advancedLearningParams;
}

// Enable/disable specific learning modes
void STDP::setTDLearningEnabled(bool enabled) {
    enableTDLearning = enabled;
}

void STDP::setBayesianLearningEnabled(bool enabled) {
    enableBayesianLearning = enabled;
}

void STDP::setMetaLearningEnabled(bool enabled) {
    enableMetaLearning = enabled;
}

void STDP::setOnlineLearningEnabled(bool enabled) {
    enableOnlineLearning = enabled;
}

// Get learning mode status
bool STDP::isTDLearningEnabled() const {
    return enableTDLearning;
}

bool STDP::isBayesianLearningEnabled() const {
    return enableBayesianLearning;
}

bool STDP::isMetaLearningEnabled() const {
    return enableMetaLearning;
}

bool STDP::isOnlineLearningEnabled() const {
    return enableOnlineLearning;
}

} // namespace nlm
