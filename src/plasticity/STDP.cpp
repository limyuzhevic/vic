#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

struct STDP::Impl {
    float ltpWeight;          // A+ for potentiation
    float ltdWeight;          // A- for depression
    float tauPlus;            // τ+ time constant (ms)
    float tauMinus;           // τ- time constant (ms)
    float minWeight;          // Minimum synaptic weight
    float maxWeight;          // Maximum synaptic weight
    bool tripletLearning;     // Enable triplet STDP
    bool weightNormalization; // Enable weight normalization
    size_t totalUpdates;      // Total STDP updates
    float totalDelta;         // Sum of all weight changes
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), 
             tauPlus(20.0f), tauMinus(20.0f),
             minWeight(-1.0f), maxWeight(1.0f),
             tripletLearning(false), weightNormalization(true),
             totalUpdates(0), totalDelta(0.0f) {}
};

STDP::STDP() : pImpl(new Impl) {}

STDP::~STDP() = default;

STDP::STDP(STDP&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

STDP& STDP::operator=(STDP&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

void STDP::update(Synapse* synapse,
                  const std::vector<Timestamp>& preSpikes,
                  const std::vector<Timestamp>& postSpikes,
                  TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Optimized implementation: use last spike only for efficiency
    // For more precision, we could track all spikes, but biological
    // learning primarily depends on recent spike timing
    
    Timestamp latestPre = preSpikes.back();
    Timestamp latestPost = postSpikes.back();
    float dt_ms = static_cast<float>(latestPost - latestPre);  // Δt in ms
    
    float totalDelta = 0.0f;
    
    if (dt_ms > 0) {
        // Pre before post: POTENTIATION (LTP)
        // Weight increases when pre fires before post
        // Reflects NMDA receptor activation and calcium influx
        float delta = pImpl->ltpWeight * std::exp(-dt_ms / pImpl->tauPlus);
        totalDelta += delta;
    } else if (dt_ms < 0) {
        // Post before pre: DEPRESSION (LTD)
        // Weight decreases when post fires before pre
        // Reflects backpropagating action potentials
        float delta = -pImpl->ltdWeight * std::exp(dt_ms / pImpl->tauMinus);
        totalDelta += delta;
    }
    
    // Apply weight change with bounds and biological constraints
    if (std::abs(totalDelta) > 1e-8f) {
        // Scale by synaptic efficacy and use-dependent modulation
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Weight normalization if enabled (prevents runaway potentiation)
        if (pImpl->weightNormalization) {
            float currentWeight = synapse->getWeight();
            float normFactor = 1.0f - 0.5f * currentWeight / pImpl->maxWeight;
            totalDelta *= normFactor;
        }
        
        // Apply weight change with biological clamping
        float newWeight = synapse->getWeight() + totalDelta;
        newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
        float clampedDelta = newWeight - synapse->getWeight();
        
        synapse->addToWeight(clampedDelta);
        
        // Update eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta * 0.5f);
        
        // Update statistics
        ++pImpl->totalUpdates;
        pImpl->totalDelta += totalDelta;
    }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to biological bounds
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
    pImpl->tauPlus = pImpl->tauMinus = std::clamp(tau, 1.0f, 100.0f);
}

float STDP::getTimeConstant() const {
    return pImpl->tauPlus;
}

void STDP::setSTDPTimeWindows(float tau_plus, float tau_minus) {
    pImpl->tauPlus = std::clamp(tau_plus, 1.0f, 100.0f);
    pImpl->tauMinus = std::clamp(tau_minus, 1.0f, 100.0f);
}

float STDP::getSTDPTimeWindowPlus() const {
    return pImpl->tauPlus;
}

float STDP::getSTDPTimeWindowMinus() const {
    return pImpl->tauMinus;
}

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setSTDPTimeWindows(tau, tau);
}

void STDP::enableTripletLearning(bool enable) {
    pImpl->tripletLearning = enable;
}

bool STDP::isTripletLearningEnabled() const {
    return pImpl->tripletLearning;
}

void STDP::setWeightNormalization(bool enable) {
    pImpl->weightNormalization = enable;
}

bool STDP::isWeightNormalizationEnabled() const {
    return pImpl->weightNormalization;
}

size_t STDP::getTotalSTDPUpdates() const {
    return pImpl->totalUpdates;
}

float STDP::getAverageSTDPChange() const {
    if (pImpl->totalUpdates == 0) return 0.0f;
    return pImpl->totalDelta / static_cast<float>(pImpl->totalUpdates);
}

} // namespace nlm
