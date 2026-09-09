#include "PlasticityRule.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct TripletSTDP::Impl {
    float a_plus;       // Potentiation parameter A+
    float a_minus;      // Depression parameter A-
    float tau_plus;     // Time constant for potentiation
    float tau_minus;    // Time constant for depression
    float tau_x;        // Time constant for trace variable X
    float tau_y;        // Time constant for trace variable Y
    float minWeight;    // Minimum synaptic weight
    float maxWeight;    // Maximum synaptic weight
    
    Impl() : a_plus(0.01f), a_minus(0.012f), tau_plus(20.0f), tau_minus(20.0f),
             tau_x(100.0f), tau_y(100.0f), minWeight(-1.0f), maxWeight(1.0f) {}
};

TripletSTDP::TripletSTDP() : pImpl(new Impl) {}

TripletSTDP::~TripletSTDP() = default;

void TripletSTDP::update(Synapse* synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt) {
    if (!synapse || !synapse->isEnabled() || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    
    float traceX = synapse->getTraceX();
    float traceY = synapse->getTraceY();
    
    float deltaX = -traceX / pImpl->tau_x;
    float deltaY = -traceY / pImpl->tau_y;
    
    for (Timestamp preTime : preSpikes) {
        deltaX += 1.0f;
    }
    
    for (Timestamp postTime : postSpikes) {
        deltaY += 1.0f;
    }
    
    traceX += deltaX * dt;
    traceY += deltaY * dt;
    synapse->setTraceX(traceX);
    synapse->setTraceY(traceY);
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dtVal = static_cast<float>(postTime - preTime);
            if (dtVal > 0) {
                float delta = pImpl->a_plus * std::exp(-dtVal / pImpl->tau_plus);
                totalDelta += delta;
            } else if (dtVal < 0) {
                float delta = -pImpl->a_minus * std::exp(dtVal / pImpl->tau_minus);
                totalDelta += delta;
            }
        }
    }
    
    if (preSpikes.size() >= 2) {
        std::vector<Timestamp> sortedPre = preSpikes;
        std::sort(sortedPre.begin(), sortedPre.end());
        for (size_t i = 0; i < sortedPre.size() - 1; ++i) {
            float dtVal = static_cast<float>(sortedPre[i+1] - sortedPre[i]);
            float delta = -pImpl->a_plus * traceX * std::exp(-dtVal / pImpl->tau_x);
            totalDelta += delta;
        }
    }
    
    if (postSpikes.size() >= 2) {
        std::vector<Timestamp> sortedPost = postSpikes;
        std::sort(sortedPost.begin(), sortedPost.end());
        for (size_t i = 0; i < sortedPost.size() - 1; ++i) {
            float dtVal = static_cast<float>(sortedPost[i+1] - sortedPost[i]);
            float delta = -pImpl->a_minus * traceY * std::exp(-dtVal / pImpl->tau_y);
            totalDelta += delta;
        }
    }
    
    if (std::abs(totalDelta) > 1e-6f) {
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        synapse->addToWeight(totalDelta);
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
    }
}

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* TripletSTDP::getName() const {
    return "TripletSTDP";
}

void TripletSTDP::setAPlus(float weight) {
    pImpl->a_plus = std::clamp(weight, 0.0f, 1.0f);
}

float TripletSTDP::getAPlus() const {
    return pImpl->a_plus;
}

void TripletSTDP::setAMinus(float weight) {
    pImpl->a_minus = std::clamp(weight, 0.0f, 1.0f);
}

float TripletSTDP::getAMinus() const {
    return pImpl->a_minus;
}

void TripletSTDP::setTimeConstants(float tau_plus, float tau_minus) {
    pImpl->tau_plus = std::clamp(tau_plus, 1.0f, 100.0f);
    pImpl->tau_minus = std::clamp(tau_minus, 1.0f, 100.0f);
}

float TripletSTDP::getTauPlus() const {
    return pImpl->tau_plus;
}

float TripletSTDP::getTauMinus() const {
    return pImpl->tau_minus;
}

} // namespace nlm