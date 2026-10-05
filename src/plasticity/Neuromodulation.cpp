#include "Neuromodulation.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

struct Neuromodulation::Impl {
    float learningRate;
    float modulationFactor;
    float eligibilityTraceDecay;
    float traceBoostFactor;
    float maxWeightChange;
    bool enableDopamine;
    bool enableACh;
    bool enableSerotonin;
    bool enableNorepinephrine;
    size_t totalUpdates;
    float totalDelta;
    
    Impl() : learningRate(0.01f), modulationFactor(1.0f),
             eligibilityTraceDecay(0.1f), traceBoostFactor(0.5f),
             maxWeightChange(0.1f),
             enableDopamine(true), enableACh(false), enableSerotonin(false), enableNorepinephrine(false),
             totalUpdates(0), totalDelta(0.0f) {}
};

Neuromodulation::Neuromodulation() : pImpl(new Impl) {}

Neuromodulation::~Neuromodulation() = default;

Neuromodulation::Neuromodulation(Neuromodulation&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

Neuromodulation& Neuromodulation::operator=(Neuromodulation&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

void Neuromodulation::update(Synapse* synapse,
                             const std::vector<Timestamp>& preSpikes,
                             const std::vector<Timestamp>& postSpikes,
                             float rewardPredictionError,
                             float neuromodulatorLevel,
                             float learningRate,
                             TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate base synaptic change from spike timing
    float baseDelta = 0.0f;
    
    // Use spike timing to determine if this was a successful or unsuccessful prediction
    Timestamp latestPre = preSpikes.back();
    Timestamp latestPost = postSpikes.back();
    float dt_ms = static_cast<float>(latestPost - latestPre);
    
    if (dt_ms > 0) {
        // Pre before post: Expected pattern -> positive prediction error reward
        baseDelta += 0.01f * std::exp(-dt_ms / 20.0f);
    } else if (dt_ms < 0) {
        // Post before pre: Unexpected pattern -> negative prediction error punishment
        baseDelta -= 0.012f * std::exp(dt_ms / 20.0f);
    }
    
    // Apply neuromodulation effects
    float modulatedDelta = baseDelta;
    
    if (pImpl->enableDopamine) {
        // Dopamine modulates learning based on reward prediction error
        // Phasic dopamine bursts (RPE > 0) enhance learning
        // Tonic dopamine (baseline) stabilizes weights
        if (rewardPredictionError > 0.0f) {
            modulatedDelta *= (1.0f + rewardPredictionError * pImpl->modulationFactor);
        }
        // Dopamine also affects eligibility traces
        float currentTrace = synapse->getEligibilityTrace();
        float traceBoost = std::min(rewardPredictionError, 1.0f) * pImpl->traceBoostFactor;
        synapse->setEligibilityTrace(currentTrace + traceBoost * baseDelta);
    }
    
    // Apply eligibility trace decay
    float currentTrace = synapse->getEligibilityTrace();
    currentTrace *= (1.0f - pImpl->eligibilityTraceDecay * dt);
    synapse->setEligibilityTrace(std::max(0.0f, currentTrace));
    
    // Apply weight change with neuromodulatory influence
    if (std::abs(modulatedDelta) > 1e-8f) {
        modulatedDelta *= pImpl->learningRate;
        
        // Apply eligibility trace modulation
        modulatedDelta *= (1.0f + currentTrace);
        
        // Clamp to prevent runaway changes
        modulatedDelta = std::clamp(modulatedDelta, -pImpl->maxWeightChange, pImpl->maxWeightChange);
        
        // Apply the change
        float newWeight = synapse->getWeight() + modulatedDelta;
        newWeight = std::clamp(newWeight, -1.0f, 1.0f);
        synapse->setWeight(newWeight);
        
        // Update statistics
        ++pImpl->totalUpdates;
        pImpl->totalDelta += modulatedDelta;
    }
}

void Neuromodulation::applyWeightChange(Synapse* synapse, SynapticWeight delta, float neuromodulatorLevel) {
    if (!synapse) return;
    
    // Apply neuromodulatory scaling
    float scaledDelta = delta;
    
    if (pImpl->enableDopamine && neuromodulatorLevel > 0.0f) {
        scaledDelta *= (1.0f + neuromodulatorLevel * pImpl->modulationFactor);
    }
    
    // Apply bounds
    scaledDelta = std::clamp(scaledDelta, -pImpl->maxWeightChange, pImpl->maxWeightChange);
    
    // Apply weight change
    float newWeight = synapse->getWeight() + scaledDelta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

void Neuromodulation::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

void Neuromodulation::setModulationFactor(float factor) {
    pImpl->modulationFactor = std::clamp(factor, 0.0f, 10.0f);
}

void Neuromodulation::setEligibilityTraceDecay(float decay) {
    pImpl->eligibilityTraceDecay = std::clamp(decay, 0.0f, 1.0f);
}

void Neuromodulation::setTraceBoostFactor(float factor) {
    pImpl->traceBoostFactor = std::clamp(factor, 0.0f, 1.0f);
}

void Neuromodulation::setWeightClipping(float max) {
    pImpl->maxWeightChange = std::clamp(max, 0.0f, 0.5f);
}

void Neuromodulation::enableDopamineModulation(bool enable) {
    pImpl->enableDopamine = enable;
}

void Neuromodulation::enableAChModulation(bool enable) {
    pImpl->enableACh = enable;
}

void Neuromodulation::enableSerotoninModulation(bool enable) {
    pImpl->enableSerotonin = enable;
}

void Neuromodulation::enableNorepinephrineModulation(bool enable) {
    pImpl->enableNorepinephrine = enable;
}

size_t Neuromodulation::getTotalNeuromodulatedUpdates() const {
    return pImpl->totalUpdates;
}

float Neuromodulation::getAverageNeuromodulatedChange() const {
    if (pImpl->totalUpdates == 0) return 0.0f;
    return pImpl->totalDelta / static_cast<float>(pImpl->totalUpdates);
}

} // namespace nlm
