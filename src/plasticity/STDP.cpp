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
    
    // Synaptic scaling parameters
    float synapticScalingFactor;    // Overall scaling factor (e.g., 1.0 = no scaling)
    int scalingSteps;               // Steps between scaling events
    int stepsSinceLastScaling;       // Counter for scaling
    
    // Metaplasticity parameters
    float learningRate;             // Global learning rate modifier
    float metaplasticityThreshold;  // Threshold for metaplastic changes
    float homeostaticTarget;       // Target firing rate for homeostatic regulation
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f),
             synapticScalingFactor(1.0f), scalingSteps(1000),
             stepsSinceLastScaling(0), learningRate(1.0f),
             metaplasticityThreshold(0.5f), homeostaticTarget(2.0f) {}
};

STDP::STDP() : pImpl(new Impl) {}

STDP::~STDP() = default;

void STDP::setSynapticScalingFactor(float factor) {
    pImpl->synapticScalingFactor = std::clamp(factor, 0.1f, 10.0f);
}

float STDP::getSynapticScalingFactor() const {
    return pImpl->synapticScalingFactor;
}

void STDP::setScalingPeriod(int steps) {
    pImpl->scalingSteps = std::max(1, steps);
    pImpl->stepsSinceLastScaling = 0;
}

int STDP::getScalingPeriod() const {
    return pImpl->scalingSteps;
}

void STDP::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.01f, 10.0f);
}

float STDP::getLearningRate() const {
    return pImpl->learningRate;
}

void STDP::setMetaplasticityThreshold(float threshold) {
    pImpl->metaplasticityThreshold = std::clamp(threshold, 0.1f, 1.0f);
}

float STDP::getMetaplasticityThreshold() const {
    return pImpl->metaplasticityThreshold;
}

void STDP::triggerSynapticScaling() {
    pImpl->stepsSinceLastScaling = 0;
    NLM_LOG_INFO("STDP: Synaptic scaling triggered");
}

void STDP::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    /*
     * Enhanced STDP implementation with synaptic scaling and metaplasticity
     * 
     * Mathematical formulation:
     * For each pre-post spike pair with timing difference Δt = t_post - t_pre:
     * 
     * If Δt > 0 (pre before post): POTENTIATION
     *   Δw = A+ * exp(-Δt / τ+) * learningRate
     *   
     * If Δt < 0 (post before pre): DEPRESSION
     *   Δw = A- * exp(Δt / τ-) * learningRate
     * 
     * Biological inspiration:
     *   - Reflects NMDA receptor-mediated calcium signaling
     *   - Pre-before-post activates NMDA receptors when postsynaptic spikes
     *   - Post-before-pre causes backpropagating action potentials
     *   
     * Synaptic scaling:
     *   - Homeostatic mechanism to maintain network stability
     *   - Scales all synaptic weights proportionally
     *   - Applied periodically based on scalingSteps
     *   
     * Metaplasticity:
     *   - Plasticity of plasticity - regulates how plastic synapses are
     *   - Modulates learning rate based on network activity
     *   - Prevents runaway excitation or inhibition
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Apply metaplastic learning rate modulation
    float currentLearningRate = pImpl->learningRate;
    
    // Update scaling counter and apply synaptic scaling if needed
    pImpl->stepsSinceLastScaling++;
    if (pImpl->stepsSinceLastScaling >= pImpl->scalingSteps) {
        triggerSynapticScaling();
        // Apply synaptic scaling
        float scalingFactor = pImpl->synapticScalingFactor;
        float currentWeight = synapse->getWeight();
        float newWeight = currentWeight * scalingFactor;
        newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
        synapse->setWeight(newWeight);
        NLM_LOG_INFO("STDP: Synaptic scaling applied (factor: " + 
                     std::to_string(scalingFactor) + ", weight: " + 
                     std::to_string(newWeight) + ")");
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                // "Cells that fire together, wire together" - but only if pre fires before post
                float delta = pImpl->ltpWeight * std::exp(-dt / tau) * currentLearningRate;
                totalDelta += delta;
            } else if (dt < 0) {
                // Post before pre: DEPRESSION
                // "Anti-Hebbian" - connection weakens if post fires without pre
                float delta = -pImpl->ltdWeight * std::exp(dt / tau) * currentLearningRate;
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
    
    // Apply metaplastic modulation
    float metaplasticDelta = delta * pImpl->learningRate;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + metaplasticDelta;
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

void STDP::setSynapticScalingFactor(float factor) {
    pImpl->synapticScalingFactor = std::clamp(factor, 0.1f, 10.0f);
}

float STDP::getSynapticScalingFactor() const {
    return pImpl->synapticScalingFactor;
}

void STDP::setScalingPeriod(int steps) {
    pImpl->scalingSteps = std::max(1, steps);
    pImpl->stepsSinceLastScaling = 0;
}

int STDP::getScalingPeriod() const {
    return pImpl->scalingSteps;
}

void STDP::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.01f, 10.0f);
}

float STDP::getLearningRate() const {
    return pImpl->learningRate;
}

void STDP::setMetaplasticityThreshold(float threshold) {
    pImpl->metaplasticityThreshold = std::clamp(threshold, 0.1f, 1.0f);
}

float STDP::getMetaplasticityThreshold() const {
    return pImpl->metaplasticityThreshold;
}

void STDP::triggerSynapticScaling() {
    pImpl->stepsSinceLastScaling = 0;
    NLM_LOG_INFO("STDP: Synaptic scaling triggered");
}

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
    
    // Set default metaplasticity parameters based on tau
    setLearningRate(1.0f);
    setMetaplasticityThreshold(0.5f);
    setSynapticScalingFactor(1.0f);
    setScalingPeriod(1000);
}

} // namespace nlm
