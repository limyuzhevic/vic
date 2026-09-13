#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct RewardModulatedSTDP::Impl {
    float dopamineGain;     // Dopamine modulation factor for plasticity
    float daWeight;         // Dopamine weight for eligibility trace
    float eligibilityDecay;  // Eligibility trace decay rate
    
    Impl() : dopamineGain(1.0f), daWeight(0.5f), eligibilityDecay(0.01f) {}
};

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(new Impl) {}

RewardModulatedSTDP::~RewardModulatedSTDP() = default;

void RewardModulatedSTDP::update(Synapse* synapse,
                                  const std::vector<Timestamp>& preSpikes,
                                  const std::vector<Timestamp>& postSpikes,
                                  TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate base STDP change
    float baseDelta = 0.0f;
    float tau = 20.0f;  // Standard STDP time constant
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                // Potentiation: pre before post
                baseDelta += 0.01f * std::exp(-dt / tau);
            } else if (dt < 0) {
                // Depression: post before pre
                baseDelta -= 0.012f * std::exp(dt / tau);
            }
        }
    }
    
    // Apply dopamine modulation
    float dopamineLevel = 1.0f;  // Would get from neuromodulator in real implementation
    float modulation = 1.0f + (dopamineLevel - 1.0f) * pImpl->dopamineGain;
    
    float modulatedDelta = baseDelta * modulation;
    
    // Update eligibility trace with modulated change
    if (std::abs(modulatedDelta) > 1e-6f) {
        float currentTrace = synapse->getEligibilityTrace();
        currentTrace += modulatedDelta * pImpl->daWeight;
        
        // Decay eligibility trace
        currentTrace *= (1.0f - pImpl->eligibilityDecay);
        
        synapse->setEligibilityTrace(currentTrace);
    }
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply modulated weight change with bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* RewardModulatedSTDP::getName() const {
    return "R-STDP";
}

void RewardModulatedSTDP::setDopamineGain(float gain) {
    pImpl->dopamineGain = std::clamp(gain, 0.0f, 2.0f);
}

float RewardModulatedSTDP::getDopamineGain() const {
    return pImpl->dopamineGain;
}

void RewardModulatedSTDP::setDAWeight(float weight) {
    pImpl->daWeight = std::clamp(weight, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getDAWeight() const {
    return pImpl->daWeight;
}

} // namespace nlm
