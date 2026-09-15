#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <map>

namespace nlm {

struct RewardModulatedSTDP::Impl {
    float ltpWeight;              // A+ for potentiation
    float ltdWeight;              // A- for depression
    float tauPlus;                // τ+ for potentiation time constant
    float tauMinus;               // τ- for depression time constant
    float learningRate;           // Base learning rate
    float dopamineLevel;          // Current dopamine concentration
    float baselineDopamine;       // Baseline dopamine level
    float dopamineModulation;     // How strongly dopamine modulates plasticity
    float rewardPredictionError;  // Current reward prediction error (RPE)
    float expectedValue;          // Expected value for prediction
    float temporalDifference;     // Temporal difference signal
    float creditAssignmentWindow; // Time window for credit assignment
    float eligibilityTraceDecay;  // How fast eligibility traces decay
    
    // Eligibility traces for reward-modulated learning
    std::map<SynapseId, float> eligibilityTraces;
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f),
             tauPlus(20.0f), tauMinus(30.0f),
             learningRate(0.01f), dopamineLevel(0.0f), baselineDopamine(1.0f),
             dopamineModulation(1.0f), rewardPredictionError(0.0f),
             expectedValue(0.0f), temporalDifference(0.0f),
             creditAssignmentWindow(50.0f), eligibilityTraceDecay(0.01f) {}
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
    
    float totalDelta = 0.0f;
    
    // Compute spike timing contributions (same as STDP)
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float deltaT = static_cast<float>(postTime - preTime);
            
            if (deltaT > 0) {
                totalDelta += pImpl->ltpWeight * std::exp(-deltaT / pImpl->tauPlus);
            } else if (deltaT < 0) {
                totalDelta -= pImpl->ltdWeight * std::exp(deltaT / pImpl->tauMinus);
            }
        }
    }
    
    // Compute dopamine modulation factor
    float dopamineFactor = 1.0f;
    if (std::abs(pImpl->dopamineLevel - pImpl->baselineDopamine) > 0.01f) {
        // Dopamine modulates STDP based on reward prediction error
        float deviation = pImpl->dopamineLevel - pImpl->baselineDopamine;
        float rpeEffect = pImpl->dopamineModulation * pImpl->rewardPredictionError;
        
        // Positive RPE (reward prediction error > 0) enhances LTP
        // Negative RPE (reward prediction error < 0) enhances LTD
        dopamineFactor = 1.0f + rpeEffect * (deltaT > 0 ? 1.0f : -1.0f);
    }
    
    // Apply neuromodulation
    totalDelta *= dopamineFactor;
    
    // Apply learning rate scaling
    totalDelta *= pImpl->learningRate;
    
    // Apply temporal difference learning (TD) modulation
    if (std::abs(pImpl->temporalDifference) > 0.01f) {
        totalDelta *= (1.0f + pImpl->dopamineModulation * pImpl->temporalDifference);
    }
    
    // Apply eligibility trace dynamics
    float traceFactor = 1.0f;
    if (pImpl->creditAssignmentWindow > 0.0f) {
        // Longer credit assignment window = more integration
        traceFactor = 1.0f + pImpl->creditAssignmentWindow / 100.0f;
    }
    
    totalDelta *= traceFactor;
    
    // Apply synaptic normalization
    float totalDeltasq = totalDelta * totalDelta;
    if (totalDeltasq > 0.001f) {
        float weightMagnitude = std::abs(synapse->getWeight());
        if (weightMagnitude > 0.1f) {
            totalDelta *= 0.1f / weightMagnitude;
        }
    }
    
    // Update eligibility trace
    updateEligibilityTrace(synapse, totalDelta, pImpl->eligibilityTraceDecay);
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        applyWeightChange(synapse, totalDelta);
    }
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    
    // Apply weight bounds (synapse handles clamping internally)
    synapse->addToWeight(delta);
}

const char* RewardModulatedSTDP::getName() const {
    return "RewardModulatedSTDP";
}

void RewardModulatedSTDP::setDopamineLevel(float level) {
    pImpl->dopamineLevel = std::max(0.0f, level);
}

float RewardModulatedSTDP::getDopamineLevel() const {
    return pImpl->dopamineLevel;
}

void RewardModulatedSTDP::setBaselineDopamine(float baseline) {
    pImpl->baselineDopamine = std::max(0.0f, baseline);
}

float RewardModulatedSTDP::getBaselineDopamine() const {
    return pImpl->baselineDopamine;
}

void RewardModulatedSTDP::setDopamineModulation(float mod) {
    pImpl->dopamineModulation = std::max(0.0f, mod);
}

float RewardModulatedSTDP::getDopamineModulation() const {
    return pImpl->dopamineModulation;
}

void RewardModulatedSTDP::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getLearningRate() const {
    return pImpl->learningRate;
}

void RewardModulatedSTDP::setRewardPredictionError(float rpe) {
    pImpl->rewardPredictionError = std::clamp(rpe, -10.0f, 10.0f);
}

float RewardModulatedSTDP::getRewardPredictionError() const {
    return pImpl->rewardPredictionError;
}

void RewardModulatedSTDP::setExpectedValue(float value) {
    pImpl->expectedValue = value;
}

float RewardModulatedSTDP::getExpectedValue() const {
    return pImpl->expectedValue;
}

void RewardModulatedSTDP::setTemporalDifference(float td) {
    pImpl->temporalDifference = td;
}

float RewardModulatedSTDP::getTemporalDifference() const {
    return pImpl->temporalDifference;
}

void RewardModulatedSTDP::setCreditAssignmentWindow(float window) {
    pImpl->creditAssignmentWindow = std::max(0.0f, window);
}

float RewardModulatedSTDP::getCreditAssignmentWindow() const {
    return pImpl->creditAssignmentWindow;
}

void RewardModulatedSTDP::setEligibilityTraceDecay(float decay) {
    pImpl->eligibilityTraceDecay = std::clamp(decay, 0.0f, 0.5f);
}

float RewardModulatedSTDP::getEligibilityTraceDecay() const {
    return pImpl->eligibilityTraceDecay;
}

void RewardModulatedSTDP::updateEligibilityTrace(Synapse* synapse, float delta, float dt) {
    if (!synapse) return;
    
    // Update eligibility trace using exponential decay
    float currentTrace = pImpl->eligibilityTraces[synapse->getId()];
    currentTrace = currentTrace * (1.0f - pImpl->eligibilityTraceDecay * dt) + delta;
    pImpl->eligibilityTraces[synapse->getId()] = currentTrace;
}

void RewardModulatedSTDP::decayAllTraces(float decayRate) {
    for (auto& trace : pImpl->eligibilityTraces) {
        trace.second *= (1.0f - decayRate);
    }
}

float RewardModulatedSTDP::getTraceValue(const Synapse* synapse) const {
    if (!synapse) return 0.0f;
    auto it = pImpl->eligibilityTraces.find(synapse->getId());
    if (it != pImpl->eligibilityTraces.end()) {
        return it->second;
    }
    return 0.0f;
}

float RewardModulatedSTDP::computeDopamineSignal(float reward, float predictionError) const {
    // Compute dopamine signal based on reward prediction error
    return predictionError > 0.0f ? 
        pImpl->baselineDopamine * (1.0f + reward) : 
        pImpl->baselineDopamine * (1.0f - std::abs(reward));
}

float RewardModulatedSTDP::computeRewardPredictionError(float reward, float value) const {
    // Simple RPE computation: reward - value
    return reward - value;
}

void RewardModulatedSTDP::configure(float ltpWeight, float ltdWeight, float tauPlus, float tauMinus,
                                   float dopamineLevel, float rpe) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstantPlus(tauPlus);
    setTimeConstantMinus(tauMinus);
    setDopamineLevel(dopamineLevel);
    setRewardPredictionError(rpe);
}

} // namespace nlm