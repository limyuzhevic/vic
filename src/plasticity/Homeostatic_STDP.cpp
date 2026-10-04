#include "Homeostatic_STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct HomeostaticSTDP::Impl {
    float ltpWeight;      // A+ for potentiation
    float ltdWeight;      // A- for depression
    float timeConstant;   // Tau for exponential window (ms)
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    
    // Homeostatic parameters
    float targetActivity;        // Target firing rate
    float homeostaticRate;       // Homeostatic learning rate
    float homeostaticTimeConstant; // Homeostatic time constant
    float currentActivity;       // Current firing rate (estimated)
    
    // Metaplasticity
    float metaplasticityFactor;  // Modulates plasticity itself
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f), targetActivity(0.1f),
             homeostaticRate(0.001f), homeostaticTimeConstant(1000.0f),
             currentActivity(0.0f), metaplasticityFactor(1.0f) {}
};

HomeostaticSTDP::HomeostaticSTDP() : pImpl(new Impl) {}

HomeostaticSTDP::~HomeostaticSTDP() = default;

void HomeostaticSTDP::update(Synapse* synapse,
                             const std::vector<Timestamp>& preSpikes,
                             const std::vector<Timestamp>& postSpikes,
                             TimestepDuration dt) {
    if (!synapse) {
        return;
    }
    
    // Estimate current activity from spike counts
    float activity = static_cast<float>(preSpikes.size() + postSpikes.size()) / 
                    (pImpl->currentActivity + 1e-6f);
    pImpl->currentActivity = activity;
    
    // First, apply standard STDP
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                // Standard potentiation
                float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                totalDelta += delta;
            } else if (dt < 0) {
                // Standard depression
                float delta = -pImpl->ltdWeight * std::exp(dt / tau);
                totalDelta += delta;
            }
        }
    }
    
    // Apply homeostatic correction
    float homeostaticCorrection = pImpl->homeostaticRate * 
                                 (pImpl->targetActivity - pImpl->currentActivity) *
                                 pImpl->metaplasticityFactor;
    
    totalDelta += homeostaticCorrection;
    
    if (std::abs(totalDelta) > 1e-6f) {
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
    }
}

void HomeostaticSTDP::setTargetActivity(float target) {
    pImpl->targetActivity = std::clamp(target, 0.0f, 1.0f);
}

float HomeostaticSTDP::getTargetActivity() const {
    return pImpl->targetActivity;
}

void HomeostaticSTDP::setHomeostaticRate(float rate) {
    pImpl->homeostaticRate = std::clamp(rate, 0.0f, 0.01f);
}

float HomeostaticSTDP::getHomeostaticRate() const {
    return pImpl->homeostaticRate;
}

void HomeostaticSTDP::setHomeostaticTimeConstant(float tau) {
    pImpl->homeostaticTimeConstant = std::clamp(tau, 10.0f, 5000.0f);
}

float HomeostaticSTDP::getHomeostaticTimeConstant() const {
    return pImpl->homeostaticTimeConstant;
}

void HomeostaticSTDP::setMetaplasticityFactor(float factor) {
    pImpl->metaplasticityFactor = std::clamp(factor, 0.1f, 10.0f);
}

float HomeostaticSTDP::getMetaplasticityFactor() const {
    return pImpl->metaplasticityFactor;
}

} // namespace nlm
