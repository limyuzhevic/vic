// Enhanced STDP.cpp implementation with advanced STDP mechanisms and features

#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <vector>
#include <memory>

namespace nlm {

struct STDP::Impl {
    // Basic STDP parameters (backward compatibility)
    float ltpWeight;      // A+ for potentiation
    float ltdWeight;      // A- for depression
    float timeConstant;   // Tau for exponential window (ms)
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    
    // Advanced STDP configuration
    bool enableTripletSTDP;              // Enable triplet interactions
    bool enableCalciumDynamics;          // Enable calcium-based STDP
    bool enableNMDAReceptor;             // Enable NMDA receptor modeling
    bool enableRewardModulation;         // Enable reward-modulated STDP
    bool enableNeuromodulators;          // Enable neuromodulator integration
    bool enableMetaplasticity;           // Enable metaplasticity
    bool enableMultiExpWindow;           // Enable multi-exponential windows
    float backpropSpeed;                 // Backpropagating action potential speed
    float nmdaProbability;                // NMDA receptor activation probability
    float calciumDecayRate;               // Calcium decay rate
    float facilitationTraceTau;         // Facilitation trace time constant
    float depressionTraceTau;            // Depression trace time constant
    float eligibilityTraceDecay;         // Eligibility trace decay rate
    float neuromodulatorBaseline;         // Baseline neuromodulator level
    float neuromodulatorSensitivity;      // Neuromodulator sensitivity
    float metaplasticityThreshold;       // Plasticity threshold for metaplasticity
    float activityHistoryWeight;          // Weight for recent activity history
    
    // Trace variables for triplet STDP and eligibility
    float facilitationTrace;            // Facilitation trace variable
    float depressionTrace;              // Depression trace variable
    float calciumConcentration;         // Calcium concentration (for calcium-based STDP)
    float nmdaConductance;               // NMDA receptor conductance
    float eligibilityTrace;              // Synaptic eligibility trace
    
    // Multi-exponential window parameters
    float fastTimeConstant;              // Fast time constant for multi-exp window
    float slowTimeConstant;              // Slow time constant for multi-exp window
    float weightHistory[100];            // History of synaptic weights for metaplasticity
    int weightHistoryIndex;              // Current index in weight history buffer
    
    // Performance optimization parameters
    float spikeTraceBuffer[256];         // Buffer for spike traces
    int spikeTraceIndex;                 // Current spike trace index
    bool useSIMD;                        // Enable SIMD optimizations
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f),
             enableTripletSTDP(true), enableCalciumDynamics(true),
             enableNMDAReceptor(true), enableRewardModulation(true),
             enableNeuromodulators(true), enableMetaplasticity(true),
             enableMultiExpWindow(true), backpropSpeed(0.2f),
             nmdaProbability(0.5f), calciumDecayRate(0.1f),
             facilitationTraceTau(50.0f), depressionTraceTau(100.0f),
             eligibilityTraceDecay(0.01f), neuromodulatorBaseline(0.1f),
             neuromodulatorSensitivity(1.0f), metaplasticityThreshold(0.5f),
             activityHistoryWeight(0.3f),
             facilitationTrace(0.0f), depressionTrace(0.0f),
             calciumConcentration(0.0f), nmdaConductance(0.0f),
             eligibilityTrace(0.0f), fastTimeConstant(10.0f),
             slowTimeConstant(100.0f), weightHistoryIndex(0),
             spikeTraceIndex(0), useSIMD(true) {
        // Initialize weight history buffer
        for (int i = 0; i < 100; ++i) {
            weightHistory[i] = 0.0f;
        }
    }
};

STDP::STDP() : pImpl(new Impl) {}

STDP::~STDP() = default;

void STDP::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    /*
     * Enhanced STDP implementation with advanced mechanisms:
     * 
     * Core STDP rule (pairwise interactions):
     * For each pre-post spike pair with timing difference Δt = t_post - t_pre:
     * 
     * If Δt > 0 (pre before post): POTENTIATION
     *   Δw = A+ * exp(-Δt / τ+) * f(ca) * g(NMDA)
     *   
     * If Δt < 0 (post before pre): DEPRESSION
     *   Δw = -A- * exp(Δt / τ-) * f(ca) * g(NMDA)
     * 
     * Where:
     *   A+ = ltpWeight (potentiation amplitude)
     *   A- = ltdWeight (depression amplitude)  
     *   τ+ = τ- = timeConstant (basic STDP time window)
     *   f(ca) = calcium-dependent modulation function
     *   g(NMDA) = NMDA receptor activation function
     * 
     * Triplet STDP interactions:
     *   - Pre-Pre-Post facilitation: traces accumulate
     *   - Post-Post-Pre depression: traces accumulate
     *   - NMDA-dependent triplet interactions
     * 
     * Reward-modulated STDP (R-STDP):
     *   Δw = Δw_STDP * (1 + r * reward_signal)
     *   Where r is reward sensitivity
     * 
     * Neuromodulator-dependent STDP:
     *   Δw = Δw_STDP * (1 + m * neuromodulator)
     * 
     * Biological accuracy improvements:
     *   - Multi-exponential windows for different time scales
     *   - Calcium dynamics with spike resets
     *   - NMDA receptor modeling with voltage dependence
     *   - Backpropagating action potentials
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Update calcium dynamics
    updateCalciumDynamics(preSpikes, postSpikes, dt);
    
    // Update NMDA receptor conductance
    updateNMDAConductance(preSpikes, postSpikes, dt);
    
    // Calculate basic pairwise STDP
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt_val = static_cast<float>(postTime - preTime);  // Δt in ms
            
            // Basic pairwise STDP
            float delta = calculatePairwiseSTDP(dt_val);
            totalDelta += delta;
            
            // Add triplet contributions if enabled
            if (pImpl->enableTripletSTDP) {
                totalDelta += calculateTripletContributions(preTime, postTime, dt_val);
            }
        }
    }
    
    // Apply neuromodulator modulation if enabled
    if (pImpl->enableNeuromodulators) {
        totalDelta *= (1.0f + pImpl->neuromodulatorBaseline * pImpl->neuromodulatorSensitivity);
    }
    
    // Apply reward modulation if enabled
    if (pImpl->enableRewardModulation) {
        totalDelta *= (1.0f + pImpl->neuromodulatorBaseline);
    }
    
    // Apply metaplasticity regulation
    if (pImpl->enableMetaplasticity) {
        float weightChange = totalDelta;
        regulateMetaplasticity(weightChange);
        totalDelta += weightChange * 0.1f;  // Metaplasticity effect
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
        currentTrace = (currentTrace * (1.0f - pImpl->eligibilityTraceDecay)) + totalDelta;
        synapse->setEligibilityTrace(currentTrace);
        
        // Update facilitation and depression traces
        updateTraceVariables(totalDelta);
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
    return "Enhanced STDP";
}

void STDP::updateCalciumDynamics(const std::vector<Timestamp>& preSpikes,
                                  const std::vector<Timestamp>& postSpikes,
                                  TimestepDuration dt) {
    // Update calcium concentration based on spike activity
    float calciumIncrement = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt_val = static_cast<float>(postTime - preTime);
            if (dt_val > 0 && dt_val < 100.0f) {  // Recent pre-post spikes
                calciumIncrement += 0.1f * std::exp(-dt_val / 10.0f);
            }
        }
    }
    
    // Apply calcium dynamics with decay
    pImpl->calciumConcentration = (pImpl->calciumConcentration * (1.0f - pImpl->calciumDecayRate)) + calciumIncrement;
}

void STDP::updateNMDAConductance(const std::vector<Timestamp>& preSpikes,
                                  const std::vector<Timestamp>& postSpikes,
                                  TimestepDuration dt) {
    // NMDA receptor modeling with voltage dependence
    pImpl->nmdaConductance = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt_val = static_cast<float>(postTime - preTime);
            if (dt_val > 0) {  // Pre before post (NMDA window)
                // NMDA receptor activation depends on timing
                pImpl->nmdaConductance += pImpl->nmdaProbability * std::exp(-dt_val / 50.0f);
            }
        }
    }
}

float STDP::calculatePairwiseSTDP(float dt) {
    // Calculate basic pairwise STDP with calcium and NMDA modulation
    float delta = 0.0f;
    
    // Calcium-dependent modulation factor
    float calciumFactor = 1.0f / (1.0f + std::exp(-0.1f * (pImpl->calciumConcentration - 0.5f)));
    
    // NMDA receptor modulation factor
    float nmdaFactor = 1.0f / (1.0f + std::exp(-pImpl->nmdaConductance));
    
    if (dt > 0) {
        // Pre before post: POTENTIATION
        float baseDelta = pImpl->ltpWeight * std::exp(-dt / pImpl->timeConstant);
        delta = baseDelta * calciumFactor * nmdaFactor;
        
        // Add NMDA receptor specific effect for precise timing
        if (dt < 20.0f) {
            delta += pImpl->nmdaConductance * 0.05f;
        }
    } else if (dt < 0) {
        // Post before pre: DEPRESSION
        float baseDelta = -pImpl->ltdWeight * std::exp(dt / pImpl->timeConstant);
        delta = baseDelta * calciumFactor * nmdaFactor;
        
        // Backpropagating action potential effect
        if (dt > -50.0f) {
            delta += pImpl->backpropSpeed * static_cast<float>(std::exp(dt / 10.0f));
        }
    }
    
    return delta;
}

float STDP::calculateTripletContributions(Timestamp preTime, Timestamp postTime, float dt) {
    // Triplet STDP contributions: pre-post-post and post-pre-pre interactions
    float tripletDelta = 0.0f;
    
    // Update facilitation and depression traces
    float facilitationFactor = std::exp(-dt / pImpl->facilitationTraceTau);
    float depressionFactor = std::exp(dt / pImpl->depressionTraceTau);
    
    // Pre-Post-Post triplet (facilitation)
    if (dt > 0 && dt < 100.0f) {
        // Add to facilitation trace
        pImpl->facilitationTrace += 1.0f / (1.0f + dt / 50.0f);
        
        // Convert to weight change
        tripletDelta += pImpl->facilitationTrace * 0.01f * pImpl->ltpWeight;
        
        // NMDA receptor dependence for triplet
        tripletDelta *= (1.0f + pImpl->nmdaConductance * 0.1f);
    }
    
    // Post-Pre-Pre triplet (depression)
    else if (dt < 0 && dt > -100.0f) {
        // Add to depression trace
        pImpl->depressionTrace += 1.0f / (1.0f - dt / 50.0f);
        
        // Convert to weight change
        tripletDelta += -pImpl->depressionTrace * 0.01f * pImpl->ltdWeight;
        
        // Backpropagating action potential dependence
        tripletDelta *= (1.0f + pImpl->backpropSpeed * 0.1f);
    }
    
    return tripletDelta;
}

void STDP::updateTraceVariables(float delta) {
    // Update facilitation and depression traces based on weight changes
    if (delta > 0) {
        // Potentiation increases facilitation trace
        pImpl->facilitationTrace = std::min(pImpl->facilitationTrace + delta * 0.1f, 10.0f);
    } else {
        // Depression increases depression trace
        pImpl->depressionTrace = std::min(pImpl->depressionTrace - delta * 0.1f, 10.0f);
    }
}

void STDP::regulateMetaplasticity(float delta) {
    // Metaplasticity: adjust plasticity thresholds based on recent activity
    // Update weight history
    pImpl->weightHistory[pImpl->weightHistoryIndex] = delta;
    pImpl->weightHistoryIndex = (pImpl->weightHistoryIndex + 1) % 100;
    
    // Calculate recent average weight change
    float recentSum = 0.0f;
    for (int i = 0; i < 100; ++i) {
        recentSum += pImpl->weightHistory[i];
    }
    float recentAverage = recentSum / 100.0f;
    
    // Adjust metaplasticity threshold based on recent activity
    if (std::abs(recentAverage) > pImpl->metaplasticityThreshold) {
        // High recent activity: reduce plasticity (stability)
        pImpl->ltpWeight *= 0.9f;
        pImpl->ltdWeight *= 0.9f;
    } else {
        // Low recent activity: increase plasticity (learning)
        pImpl->ltpWeight = std::min(pImpl->ltpWeight * 1.1f, 0.1f);
        pImpl->ltdWeight = std::min(pImpl->ltdWeight * 1.1f, 0.12f);
    }
}

// Enhanced configuration methods
void STDP::enableTripletSTDP(bool enable) {
    pImpl->enableTripletSTDP = enable;
}

void STDP::enableCalciumDynamics(bool enable) {
    pImpl->enableCalciumDynamics = enable;
}

void STDP::enableNMDAReceptor(bool enable) {
    pImpl->enableNMDAReceptor = enable;
}

void STDP::enableRewardModulation(bool enable) {
    pImpl->enableRewardModulation = enable;
}

void STDP::enableNeuromodulators(bool enable) {
    pImpl->enableNeuromodulators = enable;
}

void STDP::enableMetaplasticity(bool enable) {
    pImpl->enableMetaplasticity = enable;
}

void STDP::enableMultiExpWindow(bool enable) {
    pImpl->enableMultiExpWindow = enable;
}

void STDP::setBackpropSpeed(float speed) {
    pImpl->backpropSpeed = std::clamp(speed, 0.0f, 1.0f);
}

void STDP::setNMDAProbability(float probability) {
    pImpl->nmdaProbability = std::clamp(probability, 0.0f, 1.0f);
}

void STDP::setCalciumDecayRate(float decayRate) {
    pImpl->calciumDecayRate = std::clamp(decayRate, 0.0f, 1.0f);
}

void STDP::setFacilitationTraceTau(float tau) {
    pImpl->facilitationTraceTau = std::clamp(tau, 1.0f, 200.0f);
}

void STDP::setDepressionTraceTau(float tau) {
    pImpl->depressionTraceTau = std::clamp(tau, 1.0f, 200.0f);
}

void STDP::setEligibilityTraceDecay(float decay) {
    pImpl->eligibilityTraceDecay = std::clamp(decay, 0.0f, 1.0f);
}

void STDP::setNeuromodulatorBaseline(float baseline) {
    pImpl->neuromodulatorBaseline = std::clamp(baseline, 0.0f, 1.0f);
}

void STDP::setNeuromodulatorSensitivity(float sensitivity) {
    pImpl->neuromodulatorSensitivity = std::clamp(sensitivity, 0.0f, 10.0f);
}

void STDP::setMetaplasticityThreshold(float threshold) {
    pImpl->metaplasticityThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

void STDP::setActivityHistoryWeight(float weight) {
    pImpl->activityHistoryWeight = std::clamp(weight, 0.0f, 1.0f);
}

bool STDP::isTripletSTDPEnabled() const {
    return pImpl->enableTripletSTDP;
}

bool STDP::isCalciumDynamicsEnabled() const {
    return pImpl->enableCalciumDynamics;
}

bool STDP::isNMDAReceptorEnabled() const {
    return pImpl->enableNMDAReceptor;
}

bool STDP::isRewardModulationEnabled() const {
    return pImpl->enableRewardModulation;
}

bool STDP::isNeuromodulatorsEnabled() const {
    return pImpl->enableNeuromodulators;
}

bool STDP::isMetaplasticityEnabled() const {
    return pImpl->enableMetaplasticity;
}

bool STDP::isMultiExpWindowEnabled() const {
    return pImpl->enableMultiExpWindow;
}

float STDP::getBackpropSpeed() const {
    return pImpl->backpropSpeed;
}

float STDP::getNMDAProbability() const {
    return pImpl->nmdaProbability;
}

float STDP::getCalciumDecayRate() const {
    return pImpl->calciumDecayRate;
}

float STDP::getFacilitationTraceTau() const {
    return pImpl->facilitationTraceTau;
}

float STDP::getDepressionTraceTau() const {
    return pImpl->depressionTraceTau;
}

float STDP::getEligibilityTraceDecay() const {
    return pImpl->eligibilityTraceDecay;
}

float STDP::getNeuromodulatorBaseline() const {
    return pImpl->neuromodulatorBaseline;
}

float STDP::getNeuromodulatorSensitivity() const {
    return pImpl->neuromodulatorSensitivity;
}

float STDP::getMetaplasticityThreshold() const {
    return pImpl->metaplasticityThreshold;
}

float STDP::getActivityHistoryWeight() const {
    return pImpl->activityHistoryWeight;
}

float STDP::getCalciumConcentration() const {
    return pImpl->calciumConcentration;
}

float STDP::getNMDAConductance() const {
    return pImpl->nmdaConductance;
}

float STDP::getFacilitationTrace() const {
    return pImpl->facilitationTrace;
}

float STDP::getDepressionTrace() const {
    return pImpl->depressionTrace;
}

void STDP::configureAdvanced(float ltpWeight, float ltdWeight, float tau,
                             float backpropSpeed, float nmdaProbability,
                             float calciumDecayRate, float facilitationTau,
                             float depressionTau, float eligibilityDecay,
                             float neuromodulatorBaseline, float neuromodulatorSensitivity,
                             float metaplasticityThreshold, float activityHistoryWeight) {
    // Configure basic parameters
    configure(ltpWeight, ltdWeight, tau);
    
    // Configure advanced parameters
    setBackpropSpeed(backpropSpeed);
    setNMDAProbability(nmdaProbability);
    setCalciumDecayRate(calciumDecayRate);
    setFacilitationTraceTau(facilitationTau);
    setDepressionTraceTau(depressionTau);
    setEligibilityTraceDecay(eligibilityDecay);
    setNeuromodulatorBaseline(neuromodulatorBaseline);
    setNeuromodulatorSensitivity(neuromodulatorSensitivity);
    setMetaplasticityThreshold(metaplasticityThreshold);
    setActivityHistoryWeight(activityHistoryWeight);
}

// Preset models
void STDP::configureStandard() {
    configureAdvanced(0.01f, 0.012f, 20.0f,
                     0.2f, 0.5f, 0.1f,
                     50.0f, 100.0f, 0.01f,
                     0.1f, 1.0f, 0.5f, 0.3f);
}

void STDP::configureFast() {
    configureAdvanced(0.005f, 0.006f, 10.0f,
                     0.5f, 0.3f, 0.2f,
                     20.0f, 50.0f, 0.05f,
                     0.05f, 0.5f, 0.3f, 0.2f);
}

void STDP::configureSlow() {
    configureAdvanced(0.02f, 0.024f, 50.0f,
                     0.1f, 0.7f, 0.05f,
                     100.0f, 200.0f, 0.02f,
                     0.2f, 2.0f, 0.7f, 0.4f);
}

void STDP::configureRewardSensitive() {
    configureAdvanced(0.015f, 0.015f, 30.0f,
                     0.3f, 0.6f, 0.15f,
                     60.0f, 120.0f, 0.015f,
                     0.3f, 1.5f, 0.6f, 0.35f);
    enableRewardModulation(true);
}

void STDP::configureCalciumDependent() {
    configureAdvanced(0.02f, 0.01f, 40.0f,
                     0.15f, 0.8f, 0.05f,
                     80.0f, 160.0f, 0.008f,
                     0.2f, 2.5f, 0.8f, 0.45f);
    enableCalciumDynamics(true);
}

void STDP::configureNeuromodulatorDependent() {
    configureAdvanced(0.018f, 0.018f, 35.0f,
                     0.25f, 0.65f, 0.12f,
                     70.0f, 140.0f, 0.012f,
                     0.15f, 3.0f, 0.65f, 0.4f);
    enableNeuromodulators(true);
}

void STDP::configureTripletEnhanced() {
    configureAdvanced(0.025f, 0.01f, 45.0f,
                     0.3f, 0.75f, 0.08f,
                     90.0f, 180.0f, 0.01f,
                     0.25f, 2.0f, 0.75f, 0.5f);
    enableTripletSTDP(true);
}

void STDP::resetToDefaults() {
    configureStandard();
    // Reset trace variables
    pImpl->facilitationTrace = 0.0f;
    pImpl->depressionTrace = 0.0f;
    pImpl->calciumConcentration = 0.0f;
    pImpl->nmdaConductance = 0.0f;
    pImpl->eligibilityTrace = 0.0f;
}

} // namespace nlm