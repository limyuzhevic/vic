#include "STDP.hpp"
#include "../brain/Neuron.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// ==================== RewardModulatedSTDP Implementation ====================

struct RewardModulatedSTDP::Impl {
    float learningRate;
    float predictionErrorGain;
    float eligibilityDecay;
    
    Impl() : learningRate(0.05f), predictionErrorGain(0.5f), eligibilityDecay(0.1f) {}
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
    
    // Get current prediction error from synapse
    float predictionError = synapse->getPredictionError();
    
    // Calculate STDP weight change
    float dtMs = static_cast<float>((postSpikes.back() - preSpikes.back()) * 1000.0);
    float stdpDelta = 0.0f;
    if (dtMs > 0) {
        stdpDelta = pImpl->learningRate * std::exp(-dtMs / 20.0f);
    } else if (dtMs < 0) {
        stdpDelta = -pImpl->learningRate * std::exp(dtMs / 20.0f);
    }
    
    // Modulate STDP by prediction error
    float modulatedDelta = stdpDelta * (1.0f + pImpl->predictionErrorGain * predictionError);
    
    // Update eligibility trace
    float currentEligibility = synapse->getEligibilityTrace();
    float newEligibility = currentEligibility + modulatedDelta;
    
    // Apply eligibility trace with decay
    float eligibilityAfterDecay = newEligibility * (1.0f - pImpl->eligibilityDecay * static_cast<float>(dt));
    
    // Store for actual weight application
    if (std::abs(eligibilityAfterDecay) > 1e-6f) {
        synapse->addToEligibilityTrace(eligibilityAfterDecay);
    }
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Use eligibility trace to modulate weight change
    float eligibility = synapse->getEligibilityTrace();
    float weightChange = delta * eligibility;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + weightChange;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
    
    // Decay eligibility trace
    synapse->decayEligibilityTrace(pImpl->eligibilityDecay);
}

void RewardModulatedSTDP::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getLearningRate() const {
    return pImpl->learningRate;
}

void RewardModulatedSTDP::setPredictionErrorGain(float gain) {
    pImpl->predictionErrorGain = std::clamp(gain, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getPredictionErrorGain() const {
    return pImpl->predictionErrorGain;
}

// ==================== TripletSTDP Implementation ====================

struct TripletSTDP::Impl {
    float tripletWeight;    // A1 presynaptic trace contribution
    float stdpWeight;       // A2 standard STDP weight
    float preTraceDecay;   // Presynaptic trace decay rate
    float postTraceDecay;  // Postsynaptic trace decay rate
    
    Impl() : tripletWeight(0.005f), stdpWeight(0.01f), 
             preTraceDecay(0.05f), postTraceDecay(0.05f) {}
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
    
    // Calculate spike timings
    float t_pre = preSpikes.back();
    float t_post = postSpikes.back();
    float dtMs = static_cast<float>((t_post - t_pre) * 1000.0);
    
    // Calculate contributions from different spike combinations
    float stdpContribution = 0.0f;
    float tripletContribution = 0.0f;
    
    if (dtMs > 0) {
        // Pre before post: standard STDP + presynaptic triplet
        stdpContribution = pImpl->stdpWeight * std::exp(-dtMs / 20.0f);
        
        // Presynaptic triplet (pre-pre-post): stronger LTP
        float dtPre = std::max(0.0f, dtMs - 10.0f);  // 10ms presynaptic window
        if (dtPre >= 0) {
            tripletContribution = pImpl->tripletWeight * std::exp(-dtPre / 15.0f);
        }
    } else {
        // Post before pre: standard STDP + postsynaptic triplet  
        stdpContribution = -pImpl->stdpWeight * std::exp(dtMs / 20.0f);
        
        // Postsynaptic triplet (pre-post-post): stronger LTD
        float dtPost = std::max(0.0f, -dtMs - 10.0f);  // 10ms postsynaptic window
        if (dtPost >= 0) {
            tripletContribution = -pImpl->tripletWeight * std::exp(-dtPost / 15.0f);
        }
    }
    
    // Total triplet STDP
    float totalDelta = stdpContribution + tripletContribution;
    
    // Apply with soft bounds for selective learning
    if (std::abs(totalDelta) > 1e-6f) {
        float w = synapse->getWeight();
        bool awayFromZero = std::abs(w + totalDelta) > std::abs(w);
        totalDelta *= awayFromZero ? (1.0f - std::abs(w)) : std::abs(w);
        
        // Update eligibility trace
        float currentEligibility = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentEligibility + totalDelta);
    }
}

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Use eligibility trace for weight application
    float eligibility = synapse->getEligibilityTrace();
    float weightChange = delta * eligibility;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + weightChange;
    newWeight = std::clamp(newWeight, -0.1f, 1.0f);
    synapse->setWeight(newWeight);
    
    // Maintain sign constraints
    if (synapse->getType() == SynapseType::Inhibitory && synapse->getWeight() > -0.1f) {
        synapse->setWeight(-0.1f);
    } else if (synapse->getType() == SynapseType::Excitatory && synapse->getWeight() < 0.1f) {
        synapse->setWeight(0.1f);
    }
    
    // Decay eligibility trace
    synapse->decayEligibilityTrace(0.05f);
}

void TripletSTDP::setTripletWeight(float weight) {
    pImpl->tripletWeight = std::clamp(weight, 0.0f, 0.1f);
}

float TripletSTDP::getTripletWeight() const {
    return pImpl->tripletWeight;
}

void TripletSTDP::setSTDPWeight(float weight) {
    pImpl->stdpWeight = std::clamp(weight, 0.0f, 0.1f);
}

float TripletSTDP::getSTDPWeight() const {
    return pImpl->stdpWeight;
}

// ==================== CompetitiveHebbianRule Implementation ====================

void CompetitiveHebbianRule::update(Synapse* synapse,
                                    const std::vector<Timestamp>& preSpikes,
                                    const std::vector<Timestamp>& postSpikes,
                                    TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate Hebbian contribution
    float hebbianDelta = 0.0f;
    if (!preSpikes.empty() && !postSpikes.empty()) {
        hebbianDelta = 0.02f * std::exp(-0.01f * 
            (std::max(preSpikes.back(), postSpikes.back()) - std::min(preSpikes.back(), postSpikes.back())));
    }
    
    // Get source and target neurons
    Neuron* sourceNeuron = synapse->getSourceNeuron();
    Neuron* targetNeuron = synapse->getDestinationNeuron();
    
    if (sourceNeuron && targetNeuron) {
        // Calculate neural similarity
        float similarity = calculateNeuralSimilarity(sourceNeuron, targetNeuron);
        
        // Apply competitive penalty based on similarity
        float competitivePenalty = 1.0f - similarity;  // 0 when identical, 1 when different
        
        // Modulate Hebbian learning by competition
        float selectiveDelta = hebbianDelta * (1.0f - pImpl->competitionStrength * similarity * competitivePenalty);
        
        // Ensure selectivity: similar neurons should have different weights
        if (similarity > 0.5f) {
            float currentWeight = synapse->getWeight();
            float targetWeight = std::min(0.5f, currentWeight + 0.02f);
            
            if (currentWeight < targetWeight - pImpl->minWeightDifference) {
                selectiveDelta *= 0.5f;  // Reduce learning for similar neurons
            }
        }
        
        // Apply soft bounds for selective learning
        bool awayFromZero = std::abs(currentWeight + selectiveDelta) > std::abs(currentWeight);
        selectiveDelta *= awayFromZero ? (1.0f - std::abs(currentWeight)) : std::abs(currentWeight);
        
        // Update eligibility trace
        float currentEligibility = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentEligibility + selectiveDelta);
    }
}

void CompetitiveHebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Use eligibility trace for weight application
    float eligibility = synapse->getEligibilityTrace();
    float weightChange = delta * eligibility;
    
    // Apply selective learning constraints
    float currentWeight = synapse->getWeight();
    float targetWeight = currentWeight + weightChange;
    
    // Ensure minimum weight difference between similar synapses
    if (std::abs(targetWeight - currentWeight) > pImpl->minWeightDifference) {
        // Clamp to maintain selectivity
        targetWeight = std::clamp(targetWeight, -0.1f, 0.5f);
        synapse->setWeight(targetWeight);
        
        // Update eligibility trace based on actual change
        float actualChange = targetWeight - currentWeight;
        synapse->setEligibilityTrace(eligibility - actualChange);
    }
}

void CompetitiveHebbianRule::setCompetitionStrength(float strength) {
    pImpl->competitionStrength = std::clamp(strength, 0.0f, 1.0f);
}

float CompetitiveHebbianRule::getCompetitionStrength() const {
    return pImpl->competitionStrength;
}

void CompetitiveHebbianRule::setSelectivityTarget(float target) {
    pImpl->selectivityTarget = std::clamp(target, 0.0f, 1.0f);
}

float CompetitiveHebbianRule::getSelectivityTarget() const {
    return pImpl->selectivityTarget;
}

float CompetitiveHebbianRule::calculateCompetitivePenalty(Neuron* source, Neuron* target) const {
    if (!source || !target) return 0.0f;
    
    // Calculate similarity based on neural characteristics
    float similarity = calculateNeuralSimilarity(source, target);
    
    // Higher similarity means higher penalty
    return 1.0f - similarity;  // 0 when identical, 1 when different
}

} // namespace nlm
