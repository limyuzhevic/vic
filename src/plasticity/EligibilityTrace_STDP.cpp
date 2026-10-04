#include "EligibilityTrace_STDP.hpp"
#include "../../brain/Synapse.hpp"
#include "../../neuromodulation/Neuromodulator.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct EligibilityTraceSTDP::Impl {
    float ltpWeight;      // A+ for potentiation
    float ltdWeight;      // A- for depression
    float timeConstant;   // Tau for exponential window (ms)
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    
    // Eligibility trace parameters
    float traceDecayRate;  // Trace decay rate
    float maxTraceValue;   // Maximum trace value
    
    // Delayed reward parameters
    float delayWindow;    // Time window for delayed reward
    
    // Neuromodulation
    Neuromodulator* rewardNeuromodulator; // Reward neuromodulator (dopamine)
    
    // Eligibility trace storage
    float currentTrace;   // Current eligibility trace value
    
    // Reward history for delayed effects
    struct RewardHistory {
        float reward;
        float time;
    };
    std::vector<RewardHistory> recentRewards; // Recent rewards for delayed learning
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f), traceDecayRate(0.01f),
             maxTraceValue(1.0f), delayWindow(100.0f), 
             rewardNeuromodulator(nullptr), currentTrace(0.0f) {}
};

EligibilityTraceSTDP::EligibilityTraceSTDP() : pImpl(new Impl) {}

EligibilityTraceSTDP::~EligibilityTraceSTDP() = default;

void EligibilityTraceSTDP::update(Synapse* synapse,
                                 const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes,
                                 TimestepDuration dt) {
    if (!synapse) {
        return;
    }
    
    // Update eligibility trace based on spike activity
    updateEligibilityTrace(synapse, preSpikes, postSpikes, dt);
    
    // Calculate standard STDP weight change
    float totalDelta = calculateSTDPDelta(synapse, preSpikes, postSpikes);
    
    // Apply delayed reward modulation
    float rewardModulation = applyDelayedReward(synapse->getId());
    
    // Combine with reward modulation
    float modulatedDelta = totalDelta * (1.0f + rewardModulation);
    
    // Apply weight change with bounds
    if (std::abs(modulatedDelta) > 1e-6f) {
        float efficacy = synapse->getEfficacy();
        modulatedDelta *= efficacy;
        
        // Apply weight change
        synapse->addToWeight(modulatedDelta);
        
        // Update eligibility trace with new weight change
        float currentTrace = synapse->getEligibilityTrace();
        float newTrace = currentTrace + modulatedDelta;
        newTrace = std::clamp(newTrace, -pImpl->maxTraceValue, pImpl->maxTraceValue);
        synapse->setEligibilityTrace(newTrace);
    }
}

void EligibilityTraceSTDP::setTraceDecayRate(float rate) {
    pImpl->traceDecayRate = std::clamp(rate, 0.0f, 1.0f);
}

float EligibilityTraceSTDP::getTraceDecayRate() const {
    return pImpl->traceDecayRate;
}

void EligibilityTraceSTDP::setMaxTraceValue(float max) {
    pImpl->maxTraceValue = std::clamp(max, 0.0f, 10.0f);
}

float EligibilityTraceSTDP::getMaxTraceValue() const {
    return pImpl->maxTraceValue;
}

void EligibilityTraceSTDP::modulateWithReward(float reward, float time) {
    RewardHistory rh;
    rh.reward = reward;
    rh.time = time;
    
    pImpl->recentRewards.push_back(rh);
    
    // Keep only recent rewards within delay window
    float currentTime = time;
    pImpl->recentRewards.erase(
        std::remove_if(pImpl->recentRewards.begin(), pImpl->recentRewards.end(),
                      [currentTime, this](const RewardHistory& rh) {
                          return (currentTime - rh.time) > pImpl->delayWindow;
                      }),
        pImpl->recentRewards.end()
    );
}

void EligibilityTraceSTDP::setRewardModulator(Neuromodulator* neuromodulator) {
    pImpl->rewardNeuromodulator = neuromodulator;
}

void EligibilityTraceSTDP::setDelayWindow(float window) {
    pImpl->delayWindow = std::clamp(window, 10.0f, 1000.0f);
}

float EligibilityTraceSTDP::getDelayWindow() const {
    return pImpl->delayWindow;
}

void EligibilityTraceSTDP::updateEligibilityTrace(
    Synapse* synapse,
    const std::vector<Timestamp>& preSpikes,
    const std::vector<Timestamp>& postSpikes,
    TimestepDuration dt) {
    
    float trace = synapse->getEligibilityTrace();
    
    // Decay trace over time
    trace *= (1.0f - pImpl->traceDecayRate * dt);
    
    // Add contributions from spikes
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                // Pre before post: add positive eligibility
                float contribution = std::exp(-dt / pImpl->timeConstant);
                trace += contribution;
            } else if (dt < 0) {
                // Post before pre: add negative eligibility
                float contribution = -std::exp(dt / pImpl->timeConstant);
                trace += contribution;
            }
        }
    }
    
    // Clamp trace
    trace = std::clamp(trace, -pImpl->maxTraceValue, pImpl->maxTraceValue);
    synapse->setEligibilityTrace(trace);
}

float EligibilityTraceSTDP::calculateSTDPDelta(
    Synapse* synapse,
    const std::vector<Timestamp>& preSpikes,
    const std::vector<Timestamp>& postSpikes) {
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                // Pre before post: potentiation
                float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                totalDelta += delta;
            } else if (dt < 0) {
                // Post before pre: depression
                float delta = -pImpl->ltdWeight * std::exp(dt / tau);
                totalDelta += delta;
            }
        }
    }
    
    return totalDelta;
}

float EligibilityTraceSTDP::applyDelayedReward(SynapseId synapseId) {
    float currentTime = 0.0f; // Simplified - in real system would use simulation time
    
    float totalModulation = 0.0f;
    
    for (const auto& reward : pImpl->recentRewards) {
        float timeDiff = currentTime - reward.time;
        
        if (timeDiff >= 0.0f && timeDiff <= pImpl->delayWindow) {
            // Apply reward modulation based on time since reward
            float temporalFactor = 1.0f - (timeDiff / pImpl->delayWindow);
            totalModulation += reward.reward * temporalFactor * 0.1f; // Scaling factor
        }
    }
    
    return totalModulation;
}

} // namespace nlm
