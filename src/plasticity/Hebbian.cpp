#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>
#include <limits>

namespace nlm {

struct Hebbian::Impl {
    float learningRate;
    float theta;                    // BCM sliding threshold
    float thetaLearningRate;        // BCM threshold adaptation rate
    float maxWeight;
    float minWeight;
    
    // Rule flags
    bool useBCM;
    bool useOja;
    bool useSpikeBased;
    bool useAntiHebbian;
    bool useScaling;
    
    // Activity normalization
    float postActivityNorm;
    float preActivityNorm;
    
    // Learning modulation
    float learningRateMod;
    float globalLearningMod;
    float scalingThreshold;
    float timeDependentModulation;
    
    // STDP parameters
    float stdpTimeWindow;
    float stdpMaxWeightChange;
    
    Impl() : learningRate(0.01f),
             theta(0.5f),
             thetaLearningRate(0.001f),
             maxWeight(10.0f),
             minWeight(-10.0f),
             useBCM(true),
             useOja(true),
             useSpikeBased(true),
             useAntiHebbian(true),
             useScaling(true),
             postActivityNorm(0.01f),
             preActivityNorm(0.01f),
             learningRateMod(1.0f),
             globalLearningMod(1.0f),
             scalingThreshold(0.8f),
             timeDependentModulation(1.0f),
             stdpTimeWindow(20.0f),  // 20ms STDP time window
             stdpMaxWeightChange(0.001f) {}
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
    
    // Check if synapse is excitatory or inhibitory
    bool isExcitatory = synapse->isExcitatory();
    
    // Get current weight
    float w = synapse->getWeight();
    
    // Apply multiple Hebbian learning rules based on synapse type
    
    // 1. BCM (Bienenstock-Cooper-Munro) learning rule for stability
    // Δw = x_post * (y_post - θ), where θ is a sliding threshold
    if (pImpl->useBCM) {
        // Get average post-synaptic activity
        float avgPostActivity = calculateAverageActivity(postSpikes, dt);
        // Sliding threshold adapts to activity history
        pImpl->theta = pImpl->theta + pImpl->thetaLearningRate * 
                      (avgPostActivity - pImpl->theta);
        
        // Apply BCM rule
        float postActivity = calculateSpikeFrequency(postSpikes, dt);
        float deltaBCM = pImpl->learningRate * postActivity * (postActivity - pImpl->theta);
        
        // Scale by sign of weight for stability
        if (isExcitatory) {
            // For excitatory synapses, promote growth
            deltaBCM *= (1.0f + 0.1f * std::min(1.0f, std::abs(w) / pImpl->maxWeight));
        } else {
            // For inhibitory synapses, promote depression
            deltaBCM *= -1.0f * (1.0f + 0.1f * std::min(1.0f, std::abs(w) / pImpl->maxWeight));
        }
        applyWeightChange(synapse, deltaBCM);
    }
    
    // 2. Oja's rule for preventing runaway excitation
    // Δw = η * y_post * x_post - η * y_post² * w
    if (pImpl->useOja) {
        float postActivity = calculateSpikeFrequency(postSpikes, dt);
        float preActivity = calculateSpikeFrequency(preSpikes, dt);
        
        // Normalized spike indicators
        float y_post_norm = postActivity * pImpl->postActivityNorm;
        float x_post_norm = preActivity * pImpl->preActivityNorm;
        
        float deltaOja = pImpl->learningRate * 
                       (y_post_norm * x_post_norm - 
                        y_post_norm * y_post_norm * w);
        
        // Activity-dependent learning rate modulation
        float activityModulation = pImpl->learningRateMod * 
                                  (1.0f - pImpl->min(1.0f, std::abs(w) / pImpl->maxWeight));
        deltaOja *= activityModulation;
        
        applyWeightChange(synapse, deltaOja);
    }
    
    // 3. Spike-based Hebbian rule (similar to STDP without precise timing)
    if (pImpl->useSpikeBased) {
        // Find closest spike pair (simplified spike timing)
        float minTimeDiff = std::numeric_limits<float>::max();
        float timeDiff = 0.0f;
        
        for (Timestamp pre : preSpikes) {
            for (Timestamp post : postSpikes) {
                timeDiff = std::abs(pre - post);
                if (timeDiff < minTimeDiff) {
                    minTimeDiff = timeDiff;
                }
            }
        }
        
        // Depression for negative time differences (post before pre)
        // Potentiation for positive time differences (pre before post)
        float deltaSTDP = pImpl->learningRate * 
                         calculateSTDPWindow(minTimeDiff) * 
                         (isExcitatory ? 1.0f : -1.0f);
        
        applyWeightChange(synapse, deltaSTDP);
    }
    
    // 4. Anti-Hebbian learning for inhibitory synapses
    if (!isExcitatory && pImpl->useAntiHebbian) {
        float postActivity = calculateSpikeFrequency(postSpikes, dt);
        float preActivity = calculateSpikeFrequency(preSpikes, dt);
        
        // Anti-Hebbian: Δw = -η * x_post * y_post
        float deltaAnti = -pImpl->learningRate * preActivity * postActivity;
        
        // Stronger for inhibitory synapses
        deltaAnti *= 2.0f;
        
        applyWeightChange(synapse, deltaAnti);
    }
    
    // 5. Synaptic scaling for weight normalization
    if (pImpl->useScaling) {
        // Scale weights based on average activity
        float avgActivity = (calculateAverageActivity(preSpikes, dt) + 
                           calculateAverageActivity(postSpikes, dt)) * 0.5f;
        
        // Apply scaling factor
        if (avgActivity > pImpl->scalingThreshold) {
            float scaleFactor = 1.0f / std::sqrt(1.0f + std::pow(w, 2));
            
            // Only scale if weight is too large
            if (std::abs(w) > pImpl->maxWeight * 0.8f) {
                float scaleDelta = scaleFactor - 1.0f;
                applyWeightChange(synapse, w * scaleDelta);
            }
        }
    }
    
    // Apply global learning rate modulation
    float globalModulation = pImpl->globalLearningMod * pImpl->timeDependentModulation;
    if (globalModulation != 1.0f && !preSpikes.empty() && !postSpikes.empty()) {
        // Normalize the total change
        float weightChange = synapse->getWeight();
        float scaleFactor = globalModulation * 
                           pImpl->min(1.0f, std::abs(weightChange) / pImpl->maxWeight);
        float finalDelta = (synapse->getWeight() - w) * scaleFactor;
        
        if (finalDelta != 0.0f) {
            applyWeightChange(synapse, finalDelta);
        }
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

void Hebbian::setMinWeight(float minWeight) {
    pImpl->minWeight = minWeight;
}

float Hebbian::getMinWeight() const {
    return pImpl->minWeight;
}

// BCM parameters
void Hebbian::setBCMThreshold(float theta) {
    pImpl->theta = theta;
}

float Hebbian::getBCMThreshold() const {
    return pImpl->theta;
}

void Hebbian::setBCMThresholdRate(float rate) {
    pImpl->thetaLearningRate = rate;
}

float Hebbian::getBCMThresholdRate() const {
    return pImpl->thetaLearningRate;
}

// Rule configuration flags
void Hebbian::enableBCM(bool enable) {
    pImpl->useBCM = enable;
}

void Hebbian::enableOja(bool enable) {
    pImpl->useOja = enable;
}

void Hebbian::enableSpikeBased(bool enable) {
    pImpl->useSpikeBased = enable;
}

void Hebbian::enableAntiHebbian(bool enable) {
    pImpl->useAntiHebbian = enable;
}

void Hebbian::enableScaling(bool enable) {
    pImpl->useScaling = enable;
}

// Activity normalization
void Hebbian::setPostActivityNorm(float norm) {
    pImpl->postActivityNorm = norm;
}

float Hebbian::getPostActivityNorm() const {
    return pImpl->postActivityNorm;
}

void Hebbian::setPreActivityNorm(float norm) {
    pImpl->preActivityNorm = norm;
}

float Hebbian::getPreActivityNorm() const {
    return pImpl->preActivityNorm;
}

// Learning rate modulation
void Hebbian::setLearningRateMod(float mod) {
    pImpl->learningRateMod = mod;
}

float Hebbian::getLearningRateMod() const {
    return pImpl->learningRateMod;
}

void Hebbian::setGlobalLearningMod(float mod) {
    pImpl->globalLearningMod = mod;
}

float Hebbian::getGlobalLearningMod() const {
    return pImpl->globalLearningMod;
}

void Hebbian::setScalingThreshold(float threshold) {
    pImpl->scalingThreshold = threshold;
}

float Hebbian::getScalingThreshold() const {
    return pImpl->scalingThreshold;
}

void Hebbian::setTimeDependentModulation(float mod) {
    pImpl->timeDependentModulation = mod;
}

float Hebbian::getTimeDependentModulation() const {
    return pImpl->timeDependentModulation;
}

// STDP parameters
void Hebbian::setSTDPTimeWindow(float window) {
    pImpl->stdpTimeWindow = window;
}

float Hebbian::getSTDPTimeWindow() const {
    return pImpl->stdpTimeWindow;
}

void Hebbian::setSTDPMaxWeightChange(float change) {
    pImpl->stdpMaxWeightChange = change;
}

float Hebbian::getSTDPMaxWeightChange() const {
    return pImpl->stdpMaxWeightChange;
}

// Helper methods
float Hebbian::calculateAverageActivity(const std::vector<Timestamp>& spikes, TimestepDuration dt) const {
    if (spikes.empty()) return 0.0f;
    
    // Calculate average firing rate (spikes per second)
    float totalTime = dt > 0.0 ? static_cast<float>(dt) : 1.0f;
    return static_cast<float>(spikes.size()) / totalTime;
}

float Hebbian::calculateSpikeFrequency(const std::vector<Timestamp>& spikes, TimestepDuration dt) const {
    if (spikes.size() < 2) return 0.0f;
    
    // Calculate average inter-spike interval
    std::vector<float> intervals;
    intervals.reserve(spikes.size() - 1);
    
    for (size_t i = 1; i < spikes.size(); ++i) {
        float interval = static_cast<float>(spikes[i] - spikes[i-1]);
        if (interval > 0.0f) {
            intervals.push_back(interval);
        }
    }
    
    if (intervals.empty()) return 0.0f;
    
    // Calculate average frequency
    float avgInterval = 0.0f;
    for (float interval : intervals) {
        avgInterval += interval;
    }
    avgInterval /= intervals.size();
    
    return 1000.0f / avgInterval;  // Convert to Hz (assuming time in ms)
}

float Hebbian::calculateSTDPWindow(float timeDiff) const {
    // STDP window function: exponential decay
    float normalizedTime = std::min(1.0f, timeDiff / pImpl->stdpTimeWindow);
    return std::exp(-normalizedTime * 5.0f) * pImpl->stdpMaxWeightChange;
}

float Hebbian::min(float a, float b) const {
    return a < b ? a : b;
}

} // namespace nlm
