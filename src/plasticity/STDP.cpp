#include "STDP.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

// STDP implementation - begin

// Helper functions for STDP calculations
namespace {

float timeDifference(float t1, float t2, float axonalDelay = 1.0f) {
    return t2 - t1 + axonalDelay;
}

float exponentialDecay(float delta_t, float tau) {
    if (tau <= 0.0f) return 0.0f;
    return std::exp(-std::abs(delta_t) / tau);
}

float tripletLTPContribution(float delta1, float delta2, float tauPre, float ltpWeight, float tripletStrength) {
    float ltp1 = exponentialDecay(delta1, tauPre) * ltpWeight;
    float ltp2 = exponentialDecay(delta2, tauPre) * tripletStrength;
    return ltp1 + ltp2;
}

float tripletLTDContribution(float delta1, float delta2, float tauPost, float ltdWeight, float tripletStrength) {
    float ltd1 = exponentialDecay(delta1, tauPost) * ltdWeight;
    float ltd2 = exponentialDecay(delta2, tauPost) * tripletStrength;
    return ltd1 + ltd2;
}

} // anonymous namespace

// STDP implementation - end

} // namespace nlm
