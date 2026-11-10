#include "STDP.hpp"
#include "../brain/Synapse.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct STDP::Impl {
    // Basic STDP parameters
    float ltpWeight;           // Long-term potentiation weight
    float ltdWeight;           // Long-term depression weight
    float tau;                 // STDP time constant (ms)
    
    // Enhanced STDP parameters
    float learningRate;        // Overall learning rate
    float selectivityFactor;  // Pattern selectivity factor
    float temporalWindow;     // Temporal window for STDP
    
    // Spike traces for triplet STDP
    float preTrace;            // Pre-synaptic trace
    float postTrace;           // Post-synaptic trace
    float preTraceTau;         // Pre-synaptic trace time constant
    float postTraceTau;        // Post-synaptic trace time constant
    
    // Statistics for learning monitoring
    float totalLTP;            // Total LTP applied
    float totalLTD;            // Total LTD applied
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), tau(20.0f),
             learningRate(1.0f), selectivityFactor(1.0f), temporalWindow(50.0f),
             preTrace(0.0f), postTrace(0.0f), preTraceTau(10.0f), postTraceTau(10.0f),
             totalLTP(0.0f), totalLTD(0.0f) {}
};

STDP::STDP() : pImpl(new Impl) {}

STDP::~STDP() = default;

const char* STDP::getName() const {
    return "STDP";
}

void STDP::setLTPWeight(float weight) {
    pImpl->ltpWeight = weight;
}

float STDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void STDP::setLTDWeight(float weight) {
    pImpl->ltdWeight = weight;
}

float STDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

void STDP::setTimeConstant(float tau) {
    pImpl->tau = tau;
}

float STDP::getTimeConstant() const {
    return pImpl->tau;
}

void STDP::setSTDPLearningRate(float rate) {
    pImpl->learningRate = rate;
}

float STDP::getSTDPLearningRate() const {
    return pImpl->learningRate;
}

void STDP::setSelectivityFactor(float factor) {
    pImpl->selectivityFactor = factor;
}

float STDP::getSelectivityFactor() const {
    return pImpl->selectivityFactor;
}

void STDP::setTemporalWindow(float window) {
    pImpl->temporalWindow = window;
}

float STDP::getTemporalWindow() const {
    return pImpl->temporalWindow;
}

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
}

void STDP::configureAdvanced(float ltpWeight, float ltdWeight, float tau,
                            float learningRate, float selectivityFactor, float temporalWindow) {
    configure(ltpWeight, ltdWeight, tau);
    setSTDPLearningRate(learningRate);
    setSelectivityFactor(selectivityFactor);
    setTemporalWindow(temporalWindow);
}

void STDP::update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) {
    if (!synapse) return;
    
    TimestepDuration dtMs = dt * 1000.0f;  // Convert to milliseconds
    
    float totalDelta = 0.0f;
    
    // Enhanced STDP with temporal asymmetry and selectivity
    for (size_t i = 0; i < preSpikes.size(); ++i) {
        for (size_t j = 0; j < postSpikes.size(); ++j) {
            float spikePairTime = postSpikes[j] - preSpikes[i];
            
            if (spikePairTime < -pImpl->temporalWindow) continue;  // Outside temporal window
            if (spikePairTime > pImpl->temporalWindow) break;     // Beyond range
            
            float weightChange;
            if (spikePairTime < 0.0f) {
                // Pre before post (LTP)
                // Enhanced LTP with selectivity
                float temporalFactor = std::exp(spikePairTime / pImpl->tau);
                weightChange = pImpl->ltpWeight * temporalFactor * pImpl->selectivityFactor;
            } else {
                // Post before pre (LTD)
                // Enhanced LTD with temporal asymmetry
                float temporalFactor = std::exp(-spikePairTime / pImpl->tau);
                weightChange = -pImpl->ltdWeight * temporalFactor * pImpl->selectivityFactor;
            }
            
            // Apply learning rate and neuromodulation
            weightChange *= pImpl->learningRate;
            
            // Check if synapse can potentiate/depress
            float currentWeight = synapse->getWeight();
            float minWeight = -1.0f;  // STDP_MIN_WEIGHT
            float maxWeight = 1.0f;   // STDP_MAX_WEIGHT
            
            // Enhanced bounds check with asymmetry
            if (weightChange > 0.0f && currentWeight >= maxWeight * 0.9f) {
                // Limit LTP to prevent saturation
                weightChange = std::min(weightChange, (maxWeight - currentWeight) * 0.2f);
            } else if (weightChange < 0.0f && currentWeight <= minWeight * 0.9f) {
                // Limit LTD to prevent over-depression
                weightChange = std::max(weightChange, (minWeight - currentWeight) * 0.2f);
            }
            
            totalDelta += weightChange;
            pImpl->totalLTP += (weightChange > 0.0f) ? weightChange : 0.0f;
            pImpl->totalLTD += (weightChange < 0.0f) ? weightChange : 0.0f;
        }
    }
    
    if (totalDelta > 0.0f || totalDelta < 0.0f) {
        applyWeightChange(synapse, totalDelta);
        NLM_LOG_INFO("STDP: Applied delta " + std::to_string(totalDelta) +
                     " to synapse, LTP total: " + std::to_string(pImpl->totalLTP) +
                     ", LTD total: " + std::to_string(pImpl->totalLTD));
    }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + delta;
    
    // Apply bounds with selectivity
    float minWeight = -1.0f;  // STDP_MIN_WEIGHT
    float maxWeight = 1.0f;   // STDP_MAX_WEIGHT
    
    // Enhanced bounds check with learning history
    float selectivityAdjustment = std::min(1.0f, pImpl->totalLTP / 1000.0f);  // Prevent saturation
    minWeight = -1.0f * (1.0f - selectivityAdjustment);
    maxWeight = 1.0f * (1.0f - selectivityAdjustment);
    
    synapse->setWeight(std::clamp(newWeight, minWeight, maxWeight));
    
    NLM_LOG_INFO("STDP: Changed synapse weight from " + std::to_string(currentWeight) +
                 " to " + std::to_string(synapse->getWeight()) +
                 " (delta: " + std::to_string(delta) + ")");
}

} // namespace nlm