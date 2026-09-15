#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

struct Hebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    
    // BCM theory parameters
    float theta;              // BCM threshold variable
    float tauTheta;           // Time constant for theta adaptation
    float thetaHistory;       // Running estimate of theta
    float bcmLearningRate;    // BCM-specific learning rate
    
    // Oja's rule parameter
    float ojaAlpha;           // Oja's rule parameter (typically 0.5)
    
    // Covariance-based learning
    float covarianceLearningRate;
    size_t activityHistorySize;
    std::map<SynapseId, std::vector<float>> preActivityHistory;
    std::map<SynapseId, std::vector<float>> postActivityHistory;
    
    // Activity-dependent learning rate
    float activityDependentLearning;
    
    // Synaptic tagging and capture
    float taggingThreshold;
    float captureDuration;
    std::map<SynapseId, float> tagTimestamps;
    std::map<SynapseId, std::vector<float>> captureTraces;
    
    // Neuromodulation integration
    float modulationFactor;
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             theta(0.5f), tauTheta(1000.0f), thetaHistory(0.5f), bcmLearningRate(0.01f),
             ojaAlpha(0.5f), covarianceLearningRate(0.01f), activityHistorySize(1000),
             activityDependentLearning(0.01f), taggingThreshold(0.1f), captureDuration(100.0f),
             modulationFactor(1.0f) {}
};

Hebbian::Hebbian() : pImpl(new Impl) {}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                       const std::vector<Timestamp>& preSpikes,
                       const std::vector<Timestamp>& postSpikes,
                       TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Update activity history
    float preActivity = static_cast<float>(preSpikes.size());
    float postActivity = static_cast<float>(postSpikes.size());
    
    // Store activity history
    if (pImpl->preActivityHistory.find(synapse->getId()) == pImpl->preActivityHistory.end()) {
        pImpl->preActivityHistory[synapse->getId()] = std::vector<float>(pImpl->activityHistorySize, 0.0f);
    }
    if (pImpl->postActivityHistory.find(synapse->getId()) == pImpl->postActivityHistory.end()) {
        pImpl->postActivityHistory[synapse->getId()] = std::vector<float>(pImpl->activityHistorySize, 0.0f);
    }
    
    // Shift history and add new activity
    std::rotate(pImpl->preActivityHistory[synapse->getId()].rbegin(),
                pImpl->preActivityHistory[synapse->getId()].rbegin() + 1,
                pImpl->preActivityHistory[synapse->getId()].rend());
    pImpl->preActivityHistory[synapse->getId()].front() = preActivity;
    
    std::rotate(pImpl->postActivityHistory[synapse->getId()].rbegin(),
                pImpl->postActivityHistory[synapse->getId()].rbegin() + 1,
                pImpl->postActivityHistory[synapse->getId()].rend());
    pImpl->postActivityHistory[synapse->getId()].front() = postActivity;
    
    // Calculate covariance-based learning (Oja's rule with BCM)
    float totalDelta = 0.0f;
    
    // 1. Standard Hebbian component
    float hebbianDelta = pImpl->learningRate * preActivity * postActivity;
    totalDelta += hebbianDelta;
    
    // 2. BCM theory component
    // BCM modifies Hebbian learning with a threshold theta that adapts based on post-synaptic activity
    float postActivityFiringRate = postActivity > 0.0f ? postActivity : 0.0f;
    
    // Update theta based on post-synaptic activity (BCM theory)
    float thetaUpdate = (pImpl->tauTheta > 0.0f) ? 
        (pImpl->bcmLearningRate * (postActivityFiringRate - pImpl->thetaHistory) * dt) : 0.0f;
    pImpl->thetaHistory += thetaUpdate;
    
    // BCM learning rule: Δw ∝ postActivity * (postActivity - theta)
    float bcmDelta = pImpl->bcmLearningRate * postActivityFiringRate * (postActivityFiringRate - pImpl->thetaHistory);
    totalDelta += bcmDelta;
    
    // 3. Oja's rule component
    // Oja's rule normalizes the weight change: Δw = η * pre * post - η * α * w * pre^2
    float ojaDelta = pImpl->bcmLearningRate * preActivity * postActivity
                    - pImpl->ojaAlpha * pImpl->bcmLearningRate * synapse->getWeight() * preActivity * preActivity;
    totalDelta += ojaDelta;
    
    // 4. Covariance-based learning
    // Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
    float avgPre = 0.0f, avgPost = 0.0f, avgPrePost = 0.0f;
    
    if (pImpl->preActivityHistory[synapse->getId()].size() > 0) {
        avgPre = std::accumulate(pImpl->preActivityHistory[synapse->getId()].begin(),
                                pImpl->preActivityHistory[synapse->getId()].end(), 0.0f) /
                 static_cast<float>(pImpl->preActivityHistory[synapse->getId()].size());
    }
    
    if (pImpl->postActivityHistory[synapse->getId()].size() > 0) {
        avgPost = std::accumulate(pImpl->postActivityHistory[synapse->getId()].begin(),
                                 pImpl->postActivityHistory[synapse->getId()].end(), 0.0f) /
                  static_cast<float>(pImpl->postActivityHistory[synapse->getId()].size());
    }
    
    if (pImpl->preActivityHistory[synapse->getId()].size() > 1) {
        // Calculate covariance
        float sum = 0.0f;
        size_t count = 0;
        for (size_t i = 0; i < pImpl->preActivityHistory[synapse->getId()].size(); ++i) {
            if (i < pImpl->postActivityHistory[synapse->getId()].size()) {
                sum += pImpl->preActivityHistory[synapse->getId()][i] *
                       pImpl->postActivityHistory[synapse->getId()][i];
                ++count;
            }
        }
        if (count > 0) {
            avgPrePost = sum / static_cast<float>(count);
            float covariance = avgPrePost - (avgPre * avgPost);
            float covarianceDelta = pImpl->covarianceLearningRate * covariance;
            totalDelta += covarianceDelta;
        }
    }
    
    // 5. Activity-dependent learning rate modulation
    float activityLevel = std::max(preActivity, postActivity);
    float activityModulation = 1.0f + pImpl->activityDependentLearning * activityLevel;
    totalDelta *= activityModulation;
    
    // 6. Neuromodulation integration
    totalDelta *= pImpl->modulationFactor;
    
    // 7. Synaptic tagging and capture
    if (preActivity > pImpl->taggingThreshold) {
        // Tag the synapse for potential capture
        pImpl->tagTimestamps[synapse->getId()] = static_cast<float>(preSpikes.back());
        
        // Add to capture trace
        if (pImpl->captureTraces.find(synapse->getId()) == pImpl->captureTraces.end()) {
            pImpl->captureTraces[synapse->getId()] = std::vector<float>(10, 0.0f);
        }
        std::rotate(pImpl->captureTraces[synapse->getId()].rbegin(),
                    pImpl->captureTraces[synapse->getId()].rbegin() + 1,
                    pImpl->captureTraces[synapse->getId()].rend());
        pImpl->captureTraces[synapse->getId()].front() = totalDelta;
    }
    
    // Check for capture condition
    float tagAge = 0.0f;
    auto tagIt = pImpl->tagTimestamps.find(synapse->getId());
    if (tagIt != pImpl->tagTimestamps.end()) {
        tagAge = static_cast<float>(preSpikes.back()) - tagIt->second;
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change
        applyWeightChange(synapse, totalDelta);
    }
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* Hebbian::getName() const {
    return "Hebbian";
}

void Hebbian::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float Hebbian::getLearningRate() const {
    return pImpl->learningRate;
}

void Hebbian::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

float Hebbian::getMaxWeight() const {
    return pImpl->maxWeight;
}

void Hebbian::setTheta(float theta) {
    pImpl->theta = std::clamp(theta, 0.0f, 1.0f);
}

float Hebbian::getTheta() const {
    return pImpl->theta;
}

void Hebbian::setTauTheta(float tau) {
    pImpl->tauTheta = std::max(tau, 1.0f);
}

float Hebbian::getTauTheta() const {
    return pImpl->tauTheta;
}

void Hebbian::setBCMLearning(float learningRate) {
    pImpl->bcmLearningRate = std::clamp(learningRate, 0.0f, 1.0f);
}

float Hebbian::getBCMLearning() const {
    return pImpl->bcmLearningRate;
}

void Hebbian::setOjaAlpha(float alpha) {
    pImpl->ojaAlpha = std::clamp(alpha, 0.0f, 1.0f);
}

float Hebbian::getOjaAlpha() const {
    return pImpl->ojaAlpha;
}

void Hebbian::setCovarianceLearningRate(float rate) {
    pImpl->covarianceLearningRate = std::clamp(rate, 0.0f, 1.0f);
}

float Hebbian::getCovarianceLearningRate() const {
    return pImpl->covarianceLearningRate;
}

void Hebbian::setActivityHistorySize(size_t size) {
    pImpl->activityHistorySize = std::max(size, static_cast<size_t>(10));
}

size_t Hebbian::getActivityHistorySize() const {
    return pImpl->activityHistorySize;
}

void Hebbian::setActivityDependentLearning(float rate) {
    pImpl->activityDependentLearning = std::clamp(rate, 0.0f, 0.1f);
}

float Hebbian::getActivityDependentLearning() const {
    return pImpl->activityDependentLearning;
}

void Hebbian::setTaggingThreshold(float threshold) {
    pImpl->taggingThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

float Hebbian::getTaggingThreshold() const {
    return pImpl->taggingThreshold;
}

void Hebbian::setCaptureDuration(float duration) {
    pImpl->captureDuration = std::max(duration, 1.0f);
}

float Hebbian::getCaptureDuration() const {
    return pImpl->captureDuration;
}

void Hebbian::setModulationFactor(float factor) {
    pImpl->modulationFactor = std::clamp(factor, 0.0f, 10.0f);
}

float Hebbian::getModulationFactor() const {
    return pImpl->modulationFactor;
}

void Hebbian::configure(float learningRate, float maxWeight) {
    setLearningRate(learningRate);
    setMaxWeight(maxWeight);
}

void Hebbian::configureBCM(float theta, float learningRate) {
    setTheta(theta);
    setBCMLearning(learningRate);
}

void Hebbian::configureOja(float alpha) {
    setOjaAlpha(alpha);
}

void Hebbian::configureCovariance(float rate, size_t historySize) {
    setCovarianceLearningRate(rate);
    setActivityHistorySize(historySize);
}

float Hebbian::getAveragePreActivity(const Synapse* synapse) const {
    if (!synapse) return 0.0f;
    auto it = pImpl->preActivityHistory.find(synapse->getId());
    if (it == pImpl->preActivityHistory.end()) return 0.0f;
    if (it->second.empty()) return 0.0f;
    return std::accumulate(it->second.begin(), it->second.end(), 0.0f) / static_cast<float>(it->second.size());
}

float Hebbian::getAveragePostActivity(const Synapse* synapse) const {
    if (!synapse) return 0.0f;
    auto it = pImpl->postActivityHistory.find(synapse->getId());
    if (it == pImpl->postActivityHistory.end()) return 0.0f;
    if (it->second.empty()) return 0.0f;
    return std::accumulate(it->second.begin(), it->second.end(), 0.0f) / static_cast<float>(it->second.size());
}

float Hebbian::getCovariance(const Synapse* synapse) const {
    if (!synapse) return 0.0f;
    float avgPre = getAveragePreActivity(synapse);
    float avgPost = getAveragePostActivity(synapse);
    
    // Calculate covariance from history
    auto preIt = pImpl->preActivityHistory.find(synapse->getId());
    auto postIt = pImpl->postActivityHistory.find(synapse->getId());
    
    if (preIt == pImpl->preActivityHistory.end() || postIt == pImpl->postActivityHistory.end() ||
        preIt->second.empty() || postIt->second.empty()) return 0.0f;
    
    float sum = 0.0f;
    size_t count = 0;
    for (size_t i = 0; i < preIt->second.size() && i < postIt->second.size(); ++i) {
        sum += preIt->second[i] * postIt->second[i];
        ++count;
    }
    
    if (count == 0) return 0.0f;
    float avgPrePost = sum / static_cast<float>(count);
    return avgPrePost - (avgPre * avgPost);
}

} // namespace nlm