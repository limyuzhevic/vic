#include "TripletSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <limits>

namespace nlm {

struct TripletSTDP::Impl {
    // Triplet STDP parameters (from Brunel & Bhopal 2001)
    float A_plus;           // Potentiation amplitude for (pre, post) pair
    float A_minus;          // Depression amplitude for (post, pre) pair  
    float A_plus_tau1;      // Potentiation amplitude for (pre', post) triplet
    float A_minus_tau1;     // Depression amplitude for (post', pre) triplet
    float A_plus_tau2;      // Potentiation amplitude for (pre, post') triplet
    float A_minus_tau2;     // Depression amplitude for (post, pre') triplet
    
    // Time constants
    float tau_plus;         // Potentiation time constant (ms)
    float tau_minus;        // Depression time constant (ms)
    float tau_plus1;        // Triplet potentiation time constant 1 (ms)
    float tau_plus2;        // Triplet potentiation time constant 2 (ms)
    float tau_minus1;       // Triplet depression time constant 1 (ms)
    float tau_minus2;       // Triplet depression time constant 2 (ms)
    
    // Weight bounds
    float minWeight;
    float maxWeight;
    
    // Spike history buffer size
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    Impl() : A_plus(0.01f), A_minus(0.012f), A_plus_tau1(0.01f), A_minus_tau1(0.012f),
             A_plus_tau2(0.001f), A_minus_tau2(0.001f),
             tau_plus(20.0f), tau_minus(20.0f), tau_plus1(100.0f), tau_plus2(10.0f),
             tau_minus1(50.0f), tau_minus2(10.0f),
             minWeight(-1.0f), maxWeight(1.0f) {}
};

TripletSTDP::TripletSTDP() : pImpl(new Impl) {}

TripletSTDP::~TripletSTDP() = default;

void TripletSTDP::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate all pairwise and triplet interactions
    float totalDelta = 0.0f;
    
    // Track spike times for triplet calculations
    float minPreTime = std::numeric_limits<float>::max();
    float maxPreTime = std::numeric_limits<float>::lowest();
    float minPostTime = std::numeric_limits<float>::max();
    float maxPostTime = std::numeric_limits<float>::lowest();
    
    for (Timestamp t : preSpikes) {
        float time = static_cast<float>(t);
        if (time < minPreTime) minPreTime = time;
        if (time > maxPreTime) maxPreTime = time;
    }
    
    for (Timestamp t : postSpikes) {
        float time = static_cast<float>(t);
        if (time < minPostTime) minPostTime = time;
        if (time > maxPostTime) maxPostTime = time;
    }
    
    // Pairwise STDP contributions (standard)
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt_pair = static_cast<float>(postTime - preTime);
            
            if (dt_pair > 0) {
                // Pre before post: potentiation
                totalDelta += pImpl->A_plus * std::exp(-dt_pair / pImpl->tau_plus);
            } else if (dt_pair < 0) {
                // Post before pre: depression
                totalDelta -= pImpl->A_minus * std::exp(dt_pair / pImpl->tau_minus);
            }
        }
    }
    
    // Triplet contributions (account for spontaneous activity)
    if (!preSpikes.empty() && !postSpikes.empty()) {
        // Calculate the oldest and newest spikes for triplet interactions
        float pre_oldest = minPreTime;
        float pre_newest = maxPreTime;
        float post_oldest = minPostTime;
        float post_newest = maxPostTime;
        
        // Triplet 1: (pre_oldest, post_newest) - pre' before post
        if (pre_oldest < post_newest) {
            float dt_trip1 = post_newest - pre_oldest;
            if (dt_trip1 > 0 && dt_trip1 < pImpl->tau_plus1) {
                totalDelta += pImpl->A_plus_tau1 * std::exp(-dt_trip1 / pImpl->tau_plus1);
            }
        }
        
        // Triplet 2: (pre_newest, post_oldest) - pre before post'
        if (pre_newest < post_oldest) {
            float dt_trip2 = post_oldest - pre_newest;
            if (dt_trip2 > 0 && dt_trip2 < pImpl->tau_plus2) {
                totalDelta += pImpl->A_plus_tau2 * std::exp(-dt_trip2 / pImpl->tau_plus2);
            }
        }
        
        // Triplet 3: (post_oldest, pre_newest) - post' before pre (depression)
        if (post_oldest < pre_newest) {
            float dt_trip3 = pre_newest - post_oldest;
            if (dt_trip3 > 0 && dt_trip3 < pImpl->tau_minus1) {
                totalDelta -= pImpl->A_minus_tau1 * std::exp(-dt_trip3 / pImpl->tau_minus1);
            }
        }
        
        // Triplet 4: (post_newest, pre_oldest) - post before pre' (depression)
        if (post_newest < pre_oldest) {
            float dt_trip4 = pre_oldest - post_newest;
            if (dt_trip4 > 0 && dt_trip4 < pImpl->tau_minus2) {
                totalDelta -= pImpl->A_minus_tau2 * std::exp(-dt_trip4 / pImpl->tau_minus2);
            }
        }
    }
    
    // Apply weight change with biological constraints
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by current efficacy (use-dependent modulation)
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply the weight change
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        currentTrace += totalDelta;
        currentTrace *= (1.0f - 0.001f); // Decay eligibility trace
        synapse->setEligibilityTrace(currentTrace);
    }
}

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply weight change with hard bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* TripletSTDP::getName() const {
    return "TripletSTDP";
}

// STDP parameter configuration methods
void TripletSTDP::setAPlus(float value) {
    pImpl->A_plus = std::clamp(value, 0.0f, 0.1f);
}

float TripletSTDP::getAPlus() const {
    return pImpl->A_plus;
}

void TripletSTDP::setAMinus(float value) {
    pImpl->A_minus = std::clamp(value, 0.0f, 0.1f);
}

float TripletSTDP::getAMinus() const {
    return pImpl->A_minus;
}

void TripletSTDP::setAPlusTau1(float value) {
    pImpl->A_plus_tau1 = std::clamp(value, 0.0f, 0.05f);
}

float TripletSTDP::getAPlusTau1() const {
    return pImpl->A_plus_tau1;
}

void TripletSTDP::setAMinusTau1(float value) {
    pImpl->A_minus_tau1 = std::clamp(value, 0.0f, 0.05f);
}

float TripletSTDP::getAMinusTau1() const {
    return pImpl->A_minus_tau1;
}

void TripletSTDP::setAPlusTau2(float value) {
    pImpl->A_plus_tau2 = std::clamp(value, 0.0f, 0.01f);
}

float TripletSTDP::getAPlusTau2() const {
    return pImpl->A_plus_tau2;
}

void TripletSTDP::setAMinusTau2(float value) {
    pImpl->A_minus_tau2 = std::clamp(value, 0.0f, 0.01f);
}

float TripletSTDP::getAMinusTau2() const {
    return pImpl->A_minus_tau2;
}

void TripletSTDP::setTauPlus(float value) {
    pImpl->tau_plus = std::clamp(value, 1.0f, 100.0f);
}

float TripletSTDP::getTauPlus() const {
    return pImpl->tau_plus;
}

void TripletSTDP::setTauMinus(float value) {
    pImpl->tau_minus = std::clamp(value, 1.0f, 100.0f);
}

float TripletSTDP::getTauMinus() const {
    return pImpl->tau_minus;
}

void TripletSTDP::setTauPlus1(float value) {
    pImpl->tau_plus1 = std::clamp(value, 10.0f, 200.0f);
}

float TripletSTDP::getTauPlus1() const {
    return pImpl->tau_plus1;
}

void TripletSTDP::setTauPlus2(float value) {
    pImpl->tau_plus2 = std::clamp(value, 1.0f, 50.0f);
}

float TripletSTDP::getTauPlus2() const {
    return pImpl->tau_plus2;
}

void TripletSTDP::setTauMinus1(float value) {
    pImpl->tau_minus1 = std::clamp(value, 10.0f, 200.0f);
}

float TripletSTDP::getTauMinus1() const {
    return pImpl->tau_minus1;
}

void TripletSTDP::setTauMinus2(float value) {
    pImpl->tau_minus2 = std::clamp(value, 1.0f, 50.0f);
}

float TripletSTDP::getTauMinus2() const {
    return pImpl->tau_minus2;
}

void TripletSTDP::setMinWeight(float value) {
    pImpl->minWeight = std::clamp(value, -2.0f, 0.0f);
}

float TripletSTDP::getMinWeight() const {
    return pImpl->minWeight;
}

void TripletSTDP::setMaxWeight(float value) {
    pImpl->maxWeight = std::clamp(value, 0.0f, 2.0f);
}

float TripletSTDP::getMaxWeight() const {
    return pImpl->maxWeight;
}

void TripletSTDP::configure(float A_plus, float A_minus, float tau_plus, float tau_minus) {
    setAPlus(A_plus);
    setAMinus(A_minus);
    setTauPlus(tau_plus);
    setTauMinus(tau_minus);
}

void TripletSTDP::configure(float A_plus, float A_minus, float tau_plus, float tau_minus,
                           float A_plus_tau1, float A_minus_tau1, float A_plus_tau2, float A_minus_tau2,
                           float tau_plus1, float tau_plus2, float tau_minus1, float tau_minus2) {
    configure(A_plus, A_minus, tau_plus, tau_minus);
    setAPlusTau1(A_plus_tau1);
    setAMinusTau1(A_minus_tau1);
    setAPlusTau2(A_plus_tau2);
    setAMinusTau2(A_minus_tau2);
    setTauPlus1(tau_plus1);
    setTauPlus2(tau_plus2);
    setTauMinus1(tau_minus1);
    setTauMinus2(tau_minus2);
}

void TripletSTDP::configureBiologically(float learningRate, float weightRange) {
    // Configure with biologically plausible parameters
    configure(
        learningRate * 0.8f,           // A_plus: slightly lower than A_minus
        0.01f,                         // A_minus: standard value
        learningRate * 1.5f,           // tau_plus: slightly slower decay
        learningRate * 1.5f,           // tau_minus: slightly slower decay
        learningRate * 0.5f,           // A_plus_tau1: triplet contribution
        learningRate * 0.6f,           // A_minus_tau1: triplet contribution  
        learningRate * 0.1f,           // A_plus_tau2: triplet contribution
        learningRate * 0.1f,           // A_minus_tau2: triplet contribution
        learningRate * 30.0f,          // tau_plus1: medium timescale
        learningRate * 5.0f,           // tau_plus2: fast timescale
        learningRate * 20.0f,          // tau_minus1: medium timescale
        learningRate * 5.0f            // tau_minus2: fast timescale
    );
    
    setMinWeight(-weightRange);
    setMaxWeight(weightRange);
}

} // namespace nlm