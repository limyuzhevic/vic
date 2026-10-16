// src/plasticity/AdvancedSTDP.cpp
#include "AdvancedSTDP.hpp"
#include "../brain/Synapse.hpp"
#include "../core/Types/Types.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct AdvancedSTDP::Impl {
    float tauPlus;
    float tauMinus;
    float learningRate;
    float A1;
    float A2;
    float B1;
    float B2;
    float rewardScale;
    float baselineWeight;
    float maxWeightChange;
    
    // Timestep integration
    float integrationStep;
    
    Impl() : tauPlus(20.0f), tauMinus(20.0f), learningRate(0.001f),
             A1(0.01f), A2(0.005f), B1(0.01f), B2(0.005f),
             rewardScale(1.0f), baselineWeight(0.0f),
             maxWeightChange(0.1f), integrationStep(0.1f) {}
};

AdvancedSTDP::AdvancedSTDP() {
    pImpl = new Impl();
    tripletSTDPEnabled_ = false;
    rewardModulatedSTDP_ = false;
}

AdvancedSTDP::~AdvancedSTDP() {
    delete pImpl;
}

void AdvancedSTDP::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    // Apply basic STDP plus advanced features
    PlasticityRule::update(synapse, preSpikes, postSpikes, dt);
    
    // Apply triplet STDP if enabled
    if (tripletSTDPEnabled_) {
        applyTripletSTDP(synapse, preSpikes, postSpikes, dt);
    }
    
    // Apply reward-modulated STDP if enabled
    if (rewardModulatedSTDP_) {
        applyRewardModulatedSTDP(synapse, preSpikes, postSpikes, dt);
    }
}

void AdvancedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    // Apply weight change with bounds and safety checks
    float newWeight = synapse->getWeight() + delta * pImpl->learningRate;
    
    // Clamp to reasonable bounds
    newWeight = std::max(0.0f, std::min(newWeight, 1.0f));
    
    // Apply with stability considerations
    synapse->setWeight(newWeight);
    
    // Log if significant change
    if (std::abs(delta) > 0.01f) {
        Logger::getGlobal().info("Significant STDP weight change: " + std::to_string(delta));
    }
}

void AdvancedSTDP::applyTripletSTDP(Synapse* synapse,
                                    const std::vector<Timestamp>& preSpikes,
                                    const std::vector<Timestamp>& postSpikes,
                                    TimestepDuration dt) {
    // Triplet STDP implementation
    // Uses accumulated pre- and post-synaptic spikes for higher-order timing
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float weight = synapse->getWeight();
    float delta = 0.0f;
    
    // Calculate triplet contributions
    for (size_t i = 0; i < preSpikes.size(); ++i) {
        Timestamp preTime = preSpikes[i];
        
        // Find corresponding post spike (within reasonable window)
        for (size_t j = 0; j < postSpikes.size(); ++j) {
            Timestamp postTime = postSpikes[j];
            float deltaT = static_cast<float>(postTime - preTime);
            
            if (deltaT > 0 && deltaT < 200.0f) { // Within 200ms
                // Standard STDP contribution
                if (deltaT < pImpl->tauPlus) {
                    delta += A1_ * std::exp(-deltaT / pImpl->tauPlus);
                }
                if (deltaT > -pImpl->tauMinus) {
                    delta -= A2_ * std::exp(deltaT / pImpl->tauMinus);
                }
                
                // Triplet contributions
                if (deltaT < pImpl->tauPlus + 50.0f) {
                    // B1 term: recent pre before recent post
                    for (auto laterPost : postSpikes) {
                        if (laterPost > postTime && laterPost - postTime < 50.0f) {
                            delta += B1_ * std::exp(-(laterPost - postTime) / pImpl->tauPlus);
                        }
                    }
                    
                    // B2 term: recent post before recent pre
                    for (auto earlierPre : preSpikes) {
                        if (earlierPre < preTime && preTime - earlierPre < 50.0f) {
                            delta -= B2_ * std::exp(-(preTime - earlierPre) / pImpl->tauMinus);
                        }
                    }
                }
            }
        }
    }
    
    // Apply weight change
    applyWeightChange(synapse, delta);
}

void AdvancedSTDP::applyRewardModulatedSTDP(Synapse* synapse,
                                            const std::vector<Timestamp>& preSpikes,
                                            const std::vector<Timestamp>& postSpikes,
                                            TimestepDuration dt) {
    // Reward-modulated STDP implementation
    // Scales STDP learning by global reward signal
    
    PlasticityRule::update(synapse, preSpikes, postSpikes, dt);
    
    // Get current reward modulation level
    float rewardSignal = Dopamine::getGlobalReward();
    
    // Scale the weight changes by reward
    float scaledWeightChange = calculateBaseWeightChange(preSpikes, postSpikes) * rewardSignal * pImpl->rewardScale;
    
    // Apply scaled weight change
    applyWeightChange(synapse, scaledWeightChange);
}

float AdvancedSTDP::calculateBaseWeightChange(const std::vector<Timestamp>& preSpikes,
                                            const std::vector<Timestamp>& postSpikes) const {
    // Calculate basic STDP weight change without reward modulation
    float delta = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float deltaT = static_cast<float>(postTime - preTime);
            
            if (deltaT < pImpl->tauPlus) {
                delta += A1_ * std::exp(-deltaT / pImpl->tauPlus);
            }
            if (deltaT > -pImpl->tauMinus) {
                delta -= A2_ * std::exp(deltaT / pImpl->tauMinus);
            }
        }
    }
    
    return delta;
}

} // namespace nlm