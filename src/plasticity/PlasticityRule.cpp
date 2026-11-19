#include "PlasticityRule.hpp"
#include "../brain/Synapse.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace nlm {

bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

struct AdvancedPlasticityRule::Impl {
    float learningRate;
    float metaplasticityRate;
    float homeostaticTarget;
    float covarianceThreshold;
    float rewardInfluence;
    
    // STDP components
    float ltpWeight;
    float ltdWeight;
    float stdpTimeConstant;
    
    // Hebbian components
    float hebbianRate;
    float hebbianCovariance;
    
    // Reward modulation
    float rewardPredictionError;
    float eligibilityTrace;
    float rewardHistory[10];
    
    // Metaplasticity state
    float synapticStrength;
    float homeostaticPressure;
    float previousWeightChange;
    
    // Learning statistics
    float totalLearningEvent;
    float totalReward;
    float averageLearningRate;
    
    Impl() : learningRate(0.01f), metaplasticityRate(0.005f), homeostaticTarget(0.5f),
             covarianceThreshold(0.1f), rewardInfluence(0.5f), ltpWeight(0.01f),
             ltdWeight(0.012f), stdpTimeConstant(20.0f), hebbianRate(0.005f),
             hebbianCovariance(0.0f), rewardPredictionError(0.0f), eligibilityTrace(0.0f),
             synapticStrength(0.5f), homeostaticPressure(0.0f), previousWeightChange(0.0f),
             totalLearningEvent(0.0f), totalReward(0.0f), averageLearningRate(0.01f) {
        
        // Initialize reward history
        for (int i = 0; i < 10; ++i) {
            rewardHistory[i] = 0.0f;
        }
    }
    
    // Enhanced plasticity update combining STDP and Hebbian
    void update(Synapse* synapse, const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes, double dt) {
        if (!synapse || preSpikes.empty() || postSpikes.empty()) {
            return;
        }
        
        // Calculate STDP contribution
        float stdpDelta = calculateSTDPContribution(preSpikes, postSpikes, synapse);
        
        // Calculate Hebbian contribution
        float hebbianDelta = calculateHebbianContribution(preSpikes, postSpikes, synapse);
        
        // Calculate reward-modulated contribution
        float rewardDelta = calculateRewardModulatedContribution(synapse);
        
        // Combine all contributions with metaplasticity modulation
        float totalDelta = (stdpDelta + hebbianDelta + rewardDelta) * calculateMetaplasticityFactor(synapse);
        
        // Apply homeostatic regulation
        totalDelta *= applyHomeostaticRegulation(synapse, totalDelta);
        
        // Apply weight change
        applyWeightChange(synapse, totalDelta);
        
        // Update learning statistics
        updateLearningStatistics(totalDelta, rewardDelta);
        
        // Update eligibility trace
        eligibilityTrace = std::max(0.0f, eligibilityTrace - metaplasticityRate * static_cast<float>(dt));
    }
    
    // Calculate STDP contribution with enhanced timing
    float calculateSTDPContribution(const std::vector<Timestamp>& preSpikes,
                                   const std::vector<Timestamp>& postSpikes,
                                   Synapse* synapse) const {
        if (preSpikes.size() < 2 || postSpikes.size() < 2) {
            return 0.0f;
        }
        
        float totalDelta = 0.0f;
        float tau = stdpTimeConstant;
        
        // Use multiple spike pairs for better temporal learning
        size_t maxPairs = std::min<size_t>(preSpikes.size(), postSpikes.size());
        
        for (size_t i = 0; i < maxPairs; ++i) {
            float dtMs = (postSpikes[i] - preSpikes[i]) * 1000.0f; // Convert to ms
            
            if (dtMs > 0) {
                // Pre before post: potentiation
                float ltp = ltpWeight * std::exp(-dtMs / tau);
                totalDelta += ltp;
            } else if (dtMs < 0) {
                // Post before pre: depression
                float ltd = -ltdWeight * std::exp(dtMs / tau);
                totalDelta += ltd;
            }
        }
        
        // Account for synaptic type
        if (synapse->getType() == SynapseType::Inhibitory) {
            totalDelta = -totalDelta; // Invert for inhibitory synapses
        }
        
        // Normalize by number of spike pairs
        if (maxPairs > 0) {
            totalDelta /= static_cast<float>(maxPairs);
        }
        
        return totalDelta;
    }
    
    // Calculate Hebbian contribution with covariance learning
    float calculateHebbianContribution(const std::vector<Timestamp>& preSpikes,
                                      const std::vector<Timestamp>& postSpikes,
                                      Synapse* synapse) const {
        if (preSpikes.size() < 2 || postSpikes.size() < 2) {
            return 0.0f;
        }
        
        // Calculate correlation between pre and post activity
        float preActivity = calculateAverageActivity(preSpikes);
        float postActivity = calculateAverageActivity(postSpikes);
        
        // Calculate covariance
        float covariance = calculateCovariance(preSpikes, postSpikes);
        
        // Hebbian learning: Δw = η * (covariance - threshold)
        float delta = hebbianRate * (covariance - covarianceThreshold);
        
        // Scale by synaptic strength
        delta *= synapticStrength;
        
        return delta;
    }
    
    // Calculate reward-modulated contribution
    float calculateRewardModulatedContribution(Synapse* synapse) {
        if (eligibilityTrace <= 0.0f) {
            return 0.0f;
        }
        
        // Reward prediction error modulation
        float rpe = rewardPredictionError;
        
        // Combine with reward history for stability
        float avgReward = calculateAverageRewardHistory();
        
        // Reward-modulated learning: Δw = eligibility * RPE * reward_influence
        float delta = eligibilityTrace * rpe * rewardInfluence;
        
        // Include baseline reward modulation
        delta += eligibilityTrace * avgReward * 0.1f;
        
        return delta;
    }
    
    // Calculate metaplasticity factor
    float calculateMetaplasticityFactor(Synapse* synapse) const {
        if (!synapse) return 1.0f;
        
        float weight = synapse->getWeight();
        
        // Normalize weight to [0, 1]
        float normalizedWeight = (weight - (-1.0f)) / (1.0f - (-1.0f));
        
        // Metaplasticity: learning rate decreases near bounds
        float boundaryFactor = 1.0f - std::abs(normalizedWeight - 0.5f) * 2.0f;
        
        // Homeostatic pressure modulates learning
        float homeostaticFactor = 1.0f + homeostaticPressure;
        
        return boundaryFactor * homeostaticFactor;
    }
    
    // Apply homeostatic regulation
    float applyHomeostaticRegulation(Synapse* synapse, float proposedDelta) {
        if (!synapse) return 1.0f;
        
        float currentWeight = synapse->getWeight();
        float distanceToTarget = std::abs(currentWeight - homeostaticTarget);
        
        // Homeostatic regulation pushes weights toward target
        if (distanceToTarget > 0.1f) {
            float homeostaticPush = (homeostaticTarget - currentWeight) * 0.1f;
            return 1.0f + homeostaticPush;
        }
        
        return 1.0f;
    }
    
    // Calculate average synaptic activity
    float calculateAverageActivity(const std::vector<Timestamp>& spikes) const {
        if (spikes.empty()) return 0.0f;
        
        float totalActivity = 0.0f;
        for (Timestamp spike : spikes) {
            totalActivity += spike;
        }
        
        return totalActivity / static_cast<float>(spikes.size());
    }
    
    // Calculate covariance between pre and post activity
    float calculateCovariance(const std::vector<Timestamp>& preSpikes,
                             const std::vector<Timestamp>& postSpikes) const {
        if (preSpikes.size() < 2 || postSpikes.size() < 2) {
            return 0.0f;
        }
        
        float preMean = calculateAverageActivity(preSpikes);
        float postMean = calculateAverageActivity(postSpikes);
        
        float covariance = 0.0f;
        for (size_t i = 0; i < preSpikes.size() && i < postSpikes.size(); ++i) {
            float preDev = preSpikes[i] - preMean;
            float postDev = postSpikes[i] - postMean;
            covariance += preDev * postDev;
        }
        
        return covariance / static_cast<float>(preSpikes.size());
    }
    
    // Calculate average reward history
    float calculateAverageRewardHistory() const {
        float sum = 0.0f;
        for (int i = 0; i < 10; ++i) {
            sum += rewardHistory[i];
        }
        return sum / 10.0f;
    }
    
    // Update learning statistics
    void updateLearningStatistics(float weightChange, float rewardChange) {
        totalLearningEvent += std::abs(weightChange);
        totalReward += rewardChange;
        
        // Update average learning rate
        if (totalLearningEvent > 0.0f) {
            averageLearningRate = totalLearningEvent / (totalLearningEvent + 1.0f);
        }
        
        // Update homeostatic pressure
        homeostaticPressure = std::min(1.0f, homeostaticPressure + 0.01f * std::abs(weightChange));
        
        // Decay homeostatic pressure
        homeostaticPressure *= 0.95f;
        
        // Update synaptic strength
        synapticStrength = std::min(1.0f, synapticStrength + 0.005f * std::abs(weightChange));
        synapticStrength = std::max(0.1f, synapticStrength);
        
        // Update previous weight change
        previousWeightChange = weightChange;
    }
    
    // Update reward prediction error
    void updateRewardPredictionError(float error) {
        rewardPredictionError = error;
        
        // Update eligibility trace
        eligibilityTrace += error * 0.1f;
        eligibilityTrace = std::max(0.0f, std::min(1.0f, eligibilityTrace));
    }
    
    // Update reward history
    void updateRewardHistory(float reward) {
        // Shift history
        for (int i = 9; i > 0; --i) {
            rewardHistory[i] = rewardHistory[i-1];
        }
        rewardHistory[0] = reward;
        
        totalReward += reward;
    }
};

AdvancedPlasticityRule::AdvancedPlasticityRule() : pImpl(new Impl) {}

AdvancedPlasticityRule::~AdvancedPlasticityRule() = default;

void AdvancedPlasticityRule::update(Synapse* synapse, const std::vector<Timestamp>& preSpikes,
                                   const std::vector<Timestamp>& postSpikes, TimestepDuration dt) {
    // Enhanced plasticity rule with STDP, Hebbian, and reward modulation
    
    pImpl->update(synapse, preSpikes, postSpikes, dt);
}

void AdvancedPlasticityRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Enhanced weight change with bounds and constraints
    float newWeight = synapse->getWeight() + delta;
    
    // Apply soft bounds (saturating around 0 for excitatory, -1 for inhibitory)
    float w = synapse->getWeight();
    bool awayFromZero = std::abs(w + delta) > std::abs(w);
    delta *= awayFromZero ? (1.0f - std::abs(w)) : std::abs(w);
    
    newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    
    // Ensure minimum absolute weight
    if (synapse->getType() == SynapseType::Excitatory && newWeight < 0.01f) {
        newWeight = 0.01f;
    } else if (synapse->getType() == SynapseType::Inhibitory && newWeight > -0.01f) {
        newWeight = -0.01f;
    }
    
    synapse->setWeight(newWeight);
}

const char* AdvancedPlasticityRule::getName() const {
    return "AdvancedPlasticityRule";
}

void AdvancedPlasticityRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float AdvancedPlasticityRule::getLearningRate() const {
    return pImpl->learningRate;
}

void AdvancedPlasticityRule::setSTDPParameters(float ltp, float ltd, float tau) {
    pImpl->ltpWeight = std::clamp(ltp, 0.0f, 1.0f);
    pImpl->ltdWeight = std::clamp(ltd, 0.0f, 1.0f);
    pImpl->stdpTimeConstant = std::clamp(tau, 1.0f, 100.0f);
}

void AdvancedPlasticityRule::updateRewardPredictionError(float error) {
    pImpl->updateRewardPredictionError(error);
}

void AdvancedPlasticityRule::updateRewardHistory(float reward) {
    pImpl->updateRewardHistory(reward);
}

float AdvancedPlasticityRule::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

float AdvancedPlasticityRule::getHomeostaticPressure() const {
    return pImpl->homeostaticPressure;
}

float AdvancedPlasticityRule::getSynapticStrength() const {
    return pImpl->synapticStrength;
}

float AdvancedPlasticityRule::getAverageLearningRate() const {
    return pImpl->averageLearningRate;
}

} // namespace nlm
