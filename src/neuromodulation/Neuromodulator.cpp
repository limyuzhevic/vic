#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Integration with brain state
    Brain* brain;
    
    // Plasticity modulation factors
    float plasticityFactor;
    float learningRate;
    
    // State tracking for learning
    float predictionError;
    float rewardPredictionError;
    float expectedReward;
    float actualReward;
    
    // Eligibility traces for credit assignment
    std::vector<float> eligibilityTraces;
    
    // Dopamine neuron dynamics
    float membranePotential;
    float firingThreshold;
    float membraneTimeConstant;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             brain(nullptr), plasticityFactor(1.0f), learningRate(0.1f),
             predictionError(0.0f), rewardPredictionError(0.0f), expectedReward(0.0f),
             actualReward(0.0f), membranePotential(-70.0f), firingThreshold(-55.0f),
             membraneTimeConstant(20.0f) {
        eligibilityTraces.reserve(1000);
    }
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Enhanced dopamine-modulated plasticity factor
    // Combines baseline plasticity with dopamine modulation
    float basePlasticity = 1.0f;
    
    // Reward prediction error effect
    float rpeEffect = 1.0f + pImpl->rewardPredictionError * 0.5f;
    
    // Concentration-dependent effect (phasic vs tonic)
    float concentrationEffect;
    if (pImpl->level > 0.5f) {
        // Phasic response: high level increases plasticity up to a point
        concentrationEffect = 1.0f + (pImpl->level - 0.5f) * 1.0f;
    } else {
        // Tonic response: low level maintains baseline plasticity
        concentrationEffect = 1.0f - pImpl->level * 0.5f;
    }
    
    // Temporal dynamics (recent history)
    float temporalFactor = 1.0f + pImpl->predictionError * 0.3f;
    
    // Combined factor
    float factor = basePlasticity * rpeEffect * concentrationEffect * temporalFactor;
    
    // Clamp to reasonable range
    return std::clamp(factor, 0.1f, 3.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // Update dopamine neuron membrane potential
    if (pImpl->brain) {
        // Decay membrane potential
        float decay = std::exp(-static_cast<float>(dt) / pImpl->membraneTimeConstant);
        pImpl->membranePotential = pImpl->membranePotential * decay + (-70.0f) * (1.0f - decay);
        
        // Integrate incoming signals
        integrateSignals(dt);
        
        // Update level based on membrane potential
        float normalizedPotential = std::clamp((pImpl->membranePotential + 70.0f) / 110.0f, 0.0f, 1.0f);
        pImpl->level = normalizedPotential;
        
        // Apply baseline
        pImpl->level = std::max(pImpl->baseline, pImpl->level);
    }
}

void Dopamine::integrateSignals(TimestepDuration dt) {
    if (!pImpl->brain) return;
    
    float input = 0.0f;
    
    // Reward signal
    input += actualReward * 0.3f;
    
    // Reward prediction error
    input += rewardPredictionError * 0.4f;
    
    // Novelty/curiosity signal from curiosity neuromodulator
    auto* curiosity = pImpl->brain->getCuriosity();
    if (curiosity) {
        // Curiosity generates prediction error-like signals
        input += curiosity->getLevel() * 0.2f;
    }
    
    // Novelty detection
    auto* novelty = pImpl->brain->getNovelty();
    if (novelty) {
        input += novelty->getLevel() * 0.1f;
    }
    
    // Temporal difference error from prediction system
    auto* predictionSystem = pImpl->brain->getPredictionSystem();
    if (predictionSystem) {
        float predictionError = predictionSystem->getPredictionError();
        input += predictionError * 0.3f;
    }
    
    // Update membrane potential with integrated input
    float deltaV = (input - pImpl->membranePotential) * (1.0f - std::exp(-static_cast<float>(dt) / pImpl->membraneTimeConstant));
    pImpl->membranePotential += deltaV;
    
    // Apply eligibility trace modulation
    updateEligibilityTraces(dt);
}

void Dopamine::updateEligibilityTraces(TimestepDuration dt) {
    // Update eligibility traces for reward-modulated plasticity
    float decay = std::exp(-static_cast<float>(dt) / (pImpl->learningRate * 100.0f));
    
    for (float& trace : pImpl->eligibilityTraces) {
        trace *= decay;
    }
}

void Dopamine::recordEligibilityTrace(float trace) {
    // Record eligibility trace for a synapse
    if (pImpl->eligibilityTraces.size() < 1000) {
        pImpl->eligibilityTraces.push_back(trace);
    }
}

void Dopamine::applyToSynapse(int synapseIndex, float weight, float learningRate) {
    // Apply dopamine signal to modify synaptic weight
    if (synapseIndex < 0 || synapseIndex >= pImpl->eligibilityTraces.size()) return;
    
    float eligibility = pImpl->eligibilityTraces[synapseIndex];
    
    // Dopamine modulates weight change
    float weightChange = eligibility * level_ * learningRate * getPlasticityFactor();
    
    // Apply weight change
    float newWeight = weight + weightChange;
    
    // Clamp weight
    if (newWeight < 0.0f) newWeight = 0.0f;
    if (newWeight > 1.0f) newWeight = 1.0f;
    
    // Update eligibility trace (decay)
    pImpl->eligibilityTraces[synapseIndex] *= (1.0f - pImpl->learningRate * 0.1f);
}

void Dopamine::signalReward(float reward) {
    actualReward = reward;
    
    // Burst response for unexpected reward
    float burst = std::max(0.0f, reward - expectedReward);
    if (burst > 0.0f) {
        // Large dopamine burst
        pImpl->membranePotential += burst * 50.0f;  // Large depolarization
    }
    
    // Prediction error signal
    rewardPredictionError = reward - expectedReward;
}

void Dopamine::signalRewardPredictionError(float error) {
    rewardPredictionError = error;
    
    // Dopamine responds to prediction error
    if (error > 0.0f) {
        // Positive prediction error (better than expected) -> dopamine burst
        pImpl->membranePotential += error * 30.0f;
    } else {
        // Negative prediction error (worse than expected) -> dopamine dip
        pImpl->membranePotential += error * 20.0f;
    }
}

void Dopamine::updateLearningState() {
    if (!pImpl->brain) return;
    
    // Update expected reward based on actual experience
    // This implements temporal difference learning
    float predictionError = actualReward - expectedReward;
    
    // Update expected reward
    expectedReward += predictionError * pImpl->learningRate;
    
    // Store for next update
    pImpl->predictionError = predictionError;
}

void Dopamine::configure(float baseline, float peak, float decayRate, float releaseRate) {
    pImpl->baseline = baseline;
    pImpl->peak = peak;
    pImpl->decayRate = decayRate;
    pImpl->releaseRate = releaseRate;
}

void Dopamine::setBrain(Brain* brain) {
    pImpl->brain = brain;
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->membranePotential = -70.0f;
    pImpl->expectedReward = 0.0f;
    pImpl->actualReward = 0.0f;
    pImpl->rewardPredictionError = 0.0f;
    pImpl->predictionError = 0.0f;
    pImpl->eligibilityTraces.clear();
}

} // namespace nlm
