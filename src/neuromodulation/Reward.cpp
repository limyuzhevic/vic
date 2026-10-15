#include "Reward.hpp"
#include "../sensory/Vision.hpp"
#include "../sensory/Audio.hpp"
#include "../sensory/InternalSignals.hpp"
#include "../environment/Action.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

// Statistics for reward tracking
struct RewardStatistics {
    float mean;
    float variance;
    float min;
    float max;
    int count;
    
    RewardStatistics() : mean(0.0f), variance(0.0f), min(0.0f), max(0.0f), count(0) {}
};

// Reward function types
enum RewardFunction {
    Additive,
    Multiplicative,
    TanhScaling,
    Sigmoid,
    Max,
    WeightedSum
};

// Reward components
struct RewardComponents {
    float extrinsic;
    float intrinsic;
    float exploration;
    float curiosity;
    
    RewardComponents() : extrinsic(0.0f), intrinsic(0.0f), exploration(0.0f), curiosity(0.0f) {}
};

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    
    // Reward components
    RewardComponents components;
    
    // Statistics
    RewardStatistics stats;
    
    // Reward configuration
    float extrinsicWeight;
    float intrinsicWeight;
    float explorationWeight;
    float curiosityWeight;
    
    // Reward function
    RewardFunction functionType;
    
    // Exploration tracking
    std::vector<float> recentActions;
    std::vector<float> actionHistory;
    float noveltyThreshold;
    
    // State tracking
    int consecutiveSuccesses;
    float maxRecentReward;
    float minRecentReward;
    
    // Reward history
    std::vector<float> extrinsicHistory;
    std::vector<float> intrinsicHistory;
    std::vector<float> explorationHistory;
    
    Impl() : 
        currentValue(0.0f), 
        accumulatedReward(0.0f),
        extrinsicWeight(0.6f),
        intrinsicWeight(0.2f),
        explorationWeight(0.1f),
        curiosityWeight(0.1f),
        functionType(RewardFunction::Additive),
        noveltyThreshold(0.5f),
        consecutiveSuccesses(0),
        maxRecentReward(-1.0f),
        minRecentReward(1.0f) {
    }
};

Reward::Reward() : pImpl(new Impl) {
    // Initialize history
    pImpl->history.reserve(1000);
    pImpl->extrinsicHistory.reserve(1000);
    pImpl->intrinsicHistory.reserve(1000);
    pImpl->explorationHistory.reserve(1000);
}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = value;
}

void Reward::add(float delta) {
    pImpl->accumulatedReward += delta;
    pImpl->currentValue += delta;
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    
    // Reset statistics
    pImpl->stats.mean = 0.0f;
    pImpl->stats.variance = 0.0f;
    pImpl->stats.min = 0.0f;
    pImpl->stats.max = 0.0f;
    pImpl->stats.count = 0;
    
    pImpl->consecutiveSuccesses = 0;
    pImpl->maxRecentReward = -1.0f;
    pImpl->minRecentReward = 1.0f;
}

float Reward::computeReward(const Observation& observation) const {
    if (!observation.isValid()) {
        return 0.0f;
    }
    
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (!sensoryInput) {
        return 0.0f;
    }
    
    RewardComponents components;
    
    // Compute different reward components
    if (const Vision* vision = dynamic_cast<const Vision*>(sensoryInput)) {
        components = computeVisionRewards(*vision);
    }
    else if (const Audio* audio = dynamic_cast<const Audio*>(sensoryInput)) {
        components = computeAudioRewards(*audio);
    }
    else if (const InternalSignals* internal = dynamic_cast<const InternalSignals*>(sensoryInput)) {
        components = computeInternalRewards(*internal);
    }
    
    // Combine components based on weights
    float totalRaw = components.extrinsic * pImpl->extrinsicWeight +
                    components.intrinsic * pImpl->intrinsicWeight +
                    components.exploration * pImpl->explorationWeight +
                    components.curiosity * pImpl->curiosityWeight;
    
    // Apply reward function
    float reward = applyRewardFunction(totalRaw);
    
    // Update history and statistics
    updateHistory(reward, components);
    
    return reward;
}

RewardComponents Reward::computeVisionRewards(const Vision& vision) const {
    RewardComponents components;
    
    const std::vector<float>& data = vision.getData();
    size_t width = vision.getWidth();
    size_t height = vision.getHeight();
    
    // Extrinsic reward: based on visual goals or salient features
    float totalLuminance = 0.0f;
    float maxPixel = 0.0f;
    for (float pixel : data) {
        totalLuminance += pixel;
        if (pixel > maxPixel) maxPixel = pixel;
    }
    
    if (data.size() > 0) {
        components.extrinsic = (totalLuminance / data.size()) * 2.0f - 1.0f;
        
        // Success bonus for high luminance (could indicate food, light, etc.)
        if (maxPixel > 0.8f) {
            components.extrinsic += 1.0f;
        }
    }
    
    // Intrinsic reward: based on visual variance (novelty)
    float visualVariance = computeVariance(data);
    components.intrinsic = std::min(visualVariance * 0.5f, 1.0f);
    
    // Exploration reward: based on contrast changes
    float contrastChange = computeContrastChange(data);
    components.exploration = std::min(contrastChange * 0.3f, 1.0f);
    
    return components;
}

RewardComponents Reward::computeAudioRewards(const Audio& audio) const {
    RewardComponents components;
    
    const std::vector<float>& data = audio.getData();
    
    if (data.empty()) {
        return components;
    }
    
    // Extrinsic reward: based on audio energy
    float audioEnergy = 0.0f;
    for (float sample : data) {
        audioEnergy += sample * sample;
    }
    audioEnergy /= data.size();
    components.extrinsic = std::min(audioEnergy * 10.0f, 1.0f);
    
    // Intrinsic reward: based on frequency changes
    float spectralChange = computeSpectralChange(data);
    components.intrinsic = std::min(spectralChange * 0.5f, 1.0f);
    
    return components;
}

RewardComponents Reward::computeInternalRewards(const InternalSignals& internal) const {
    RewardComponents components;
    
    const std::vector<float>& signals = internal.getData();
    
    if (signals.empty()) {
        return components;
    }
    
    // Extrinsic reward: based on high activation (goal proximity)
    float maxSignal = 0.0f;
    for (float signal : signals) {
        if (signal > maxSignal) maxSignal = signal;
    }
    components.extrinsic = std::min(maxSignal, 1.0f);
    
    // Track consecutive successes
    if (maxSignal > 0.5f) {
        pImpl->consecutiveSuccesses++;
    } else {
        pImpl->consecutiveSuccesses = 0;
    }
    
    // Success bonus
    if (pImpl->consecutiveSuccesses >= 3) {
        components.extrinsic += 0.5f;
    }
    
    // Intrinsic reward: based on signal variance (novelty)
    float variance = computeVariance(signals);
    components.intrinsic = std::min(variance * 2.0f, 1.0f);
    
    // Curiosity reward: based on prediction error (if available)
    components.curiosity = computeCuriosityReward();
    
    return components;
}

float Reward::applyRewardFunction(float rawReward) const {
    switch (pImpl->functionType) {
        case RewardFunction::Additive:
            return std::max(rawReward, 0.0f);
        
        case RewardFunction::Multiplicative:
            return rawReward > 0.0f ? rawReward : 0.0f;
        
        case RewardFunction::TanhScaling:
            return std::tanh(rawReward);
        
        case RewardFunction::Sigmoid:
            return 1.0f / (1.0f + std::exp(-rawReward));
        
        case RewardFunction::Max:
            return std::max(rawReward, pImpl->stats.max);
        
        case RewardFunction::WeightedSum:
            return rawReward;
        
        default:
            return std::max(rawReward, 0.0f);
    }
}

void Reward::updateHistory(float reward, const RewardComponents& components) {
    // Update main history
    pImpl->history.push_back(reward);
    
    // Update component histories
    pImpl->extrinsicHistory.push_back(components.extrinsic);
    pImpl->intrinsicHistory.push_back(components.intrinsic);
    pImpl->explorationHistory.push_back(components.exploration);
    
    // Keep history at reasonable size
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
        pImpl->extrinsicHistory.erase(pImpl->extrinsicHistory.begin());
        pImpl->intrinsicHistory.erase(pImpl->intrinsicHistory.begin());
        pImpl->explorationHistory.erase(pImpl->explorationHistory.begin());
    }
    
    // Update statistics
    updateStatistics(reward);
    
    // Update recent reward extremes
    if (reward > pImpl->maxRecentReward) {
        pImpl->maxRecentReward = reward;
    }
    if (reward < pImpl->minRecentReward) {
        pImpl->minRecentReward = reward;
    }
}

void Reward::updateStatistics(float reward) {
    pImpl->stats.count++;
    
    // Welford's algorithm for online variance computation
    float delta = reward - pImpl->stats.mean;
    pImpl->stats.mean += delta / pImpl->stats.count;
    pImpl->stats.variance += delta * (reward - pImpl->stats.mean);
    
    if (pImpl->stats.count == 1) {
        pImpl->stats.min = reward;
        pImpl->stats.max = reward;
    } else {
        if (reward < pImpl->stats.min) pImpl->stats.min = reward;
        if (reward > pImpl->stats.max) pImpl->stats.max = reward;
    }
    
    if (pImpl->stats.count > 1) {
        pImpl->stats.variance /= (pImpl->stats.count - 1);
    }
}

float Reward::computeVariance(const std::vector<float>& data) const {
    if (data.empty()) return 0.0f;
    
    float mean = 0.0f;
    for (float val : data) mean += val;
    mean /= data.size();
    
    float variance = 0.0f;
    for (float val : data) {
        float diff = val - mean;
        variance += diff * diff;
    }
    variance /= data.size();
    
    return variance;
}

float Reward::computeContrastChange(const std::vector<float>& data) const {
    // Simplified contrast change computation
    // Look for high-frequency changes in pixel intensities
    float contrastChange = 0.0f;
    if (data.size() >= 3) {
        for (size_t i = 1; i < data.size() - 1; ++i) {
            float diff = std::abs(data[i] - data[i-1]) + std::abs(data[i] - data[i+1]);
            contrastChange += diff;
        }
        contrastChange /= data.size();
    }
    return contrastChange;
}

float Reward::computeSpectralChange(const std::vector<float>& data) const {
    // Simplified spectral change - look for rapid changes in audio signal
    float spectralChange = 0.0f;
    if (data.size() >= 3) {
        for (size_t i = 1; i < data.size() - 1; ++i) {
            float diff = std::abs(data[i] - data[i-1]) + std::abs(data[i] - data[i+1]);
            spectralChange += diff;
        }
        spectralChange /= data.size();
    }
    return spectralChange;
}

float Reward::computeCuriosityReward() const {
    // Simple curiosity reward based on prediction error
    // This would normally use a prediction model
    float predictionError = 0.0f;
    
    // In a real implementation, this would compare predicted vs actual
    // For now, use signal variance as a proxy for prediction error
    if (pImpl->recentActions.size() >= 3) {
        predictionError = computeVariance(pImpl->recentActions);
    }
    
    return std::min(predictionError * 5.0f, 1.0f);
}

void Reward::setExtrinsicWeight(float weight) {
    pImpl->extrinsicWeight = weight;
}

void Reward::setIntrinsicWeight(float weight) {
    pImpl->intrinsicWeight = weight;
}

void Reward::setExplorationWeight(float weight) {
    pImpl->explorationWeight = weight;
}

void Reward::setCuriosityWeight(float weight) {
    pImpl->curiosityWeight = weight;
}

void Reward::setRewardFunction(RewardFunction func) {
    pImpl->functionType = func;
}

void Reward::addAction(float actionValue) {
    pImpl->actionHistory.push_back(actionValue);
    
    // Keep only recent actions for curiosity computation
    if (pImpl->actionHistory.size() > 100) {
        pImpl->actionHistory.erase(pImpl->actionHistory.begin());
    }
}

float Reward::getExtrinsicWeight() const {
    return pImpl->extrinsicWeight;
}

float Reward::getIntrinsicWeight() const {
    return pImpl->intrinsicWeight;
}

float Reward::getExplorationWeight() const {
    return pImpl->explorationWeight;
}

float Reward::getCuriosityWeight() const {
    return pImpl->curiosityWeight;
}

RewardFunction Reward::getRewardFunction() const {
    return pImpl->functionType;
}

float Reward::getStatisticsMean() const {
    return pImpl->stats.mean;
}

float Reward::getStatisticsVariance() const {
    return pImpl->stats.variance;
}

float Reward::getStatisticsMin() const {
    return pImpl->stats.min;
}

float Reward::getStatisticsMax() const {
    return pImpl->stats.max;
}

int Reward::getStatisticsCount() const {
    return pImpl->stats.count;
}

float Reward::getConsecutiveSuccesses() const {
    return pImpl->consecutiveSuccesses;
}

float Reward::getMaxRecentReward() const {
    return pImpl->maxRecentReward;
}

float Reward::getMinRecentReward() const {
    return pImpl->minRecentReward;
}

} // namespace nlm
