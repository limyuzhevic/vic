#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Novelty detection signal
// Computes novelty from comparison with previous observations

class Novelty {
public:
    Novelty();
    ~Novelty();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get novelty level
    float getLevel() const;
    void setLevel(float level);
    
    // Detect novelty from observation
    void detectNovelty(const class Observation& observation, 
                       const class Observation& previousObservation);
    
    // Detect novelty from sensory input pattern
    void detectNovelty(const std::vector<float>& currentPattern,
                       const std::vector<float>& previousPattern);
    
    // Decay novelty over time
    void update(TimestepDuration dt);
    
    // Get novelty history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Additional functionality
    void triggerExploration(float noveltyLevel);
    void triggerCuriosity(float noveltyLevel);
    
private:
    struct Impl;
    Impl* pImpl;
};

inline Novelty::Novelty() : pImpl(new Impl) {}

inline Novelty::~Novelty() = default;

inline void Novelty::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Novelty detection initialized with " + std::to_string(pImpl->historySize) + 
                 std::string(" history slots"));
}

inline float Novelty::getLevel() const {
    return pImpl->level;
}

inline void Novelty::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->maxLevel);
}

inline void Novelty::detectNovelty(const Observation& observation,
                            const Observation& previousObservation) {
    if (!pImpl->brain) {
        NLM_LOG_WARNING("Novelty detection called before initialization");
        return;
    }
    
    std::vector<float> currentPattern = extractFeatures(observation);
    std::vector<float> previousPattern = extractFeatures(previousObservation);
    
    if (currentPattern.empty() || previousPattern.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    float novelty = computeNovelty(currentPattern, previousPattern);
    
    if (novelty > pImpl->adaptationThreshold) {
        pImpl->level = std::min(pImpl->maxLevel, pImpl->level + novelty * 2.0f * pImpl->accumulationRate);
        
        if (pImpl->brain) {
            pImpl->brain->triggerExploration(novelty);
        }
        
        NLM_LOG_DEBUG(std::string("High novelty detected: ") + std::to_string(novelty));
    } else {
        pImpl->level += (novelty - pImpl->level) * pImpl->accumulationRate;
    }
    
    if (pImpl->patternHistory.size() >= pImpl->historySize) {
        pImpl->patternHistory.erase(pImpl->patternHistory.begin());
    }
    pImpl->patternHistory.push_back(novelty);
    
    std::vector<float> sensoryPattern = observation.getSensoryData();
    if (sensoryPattern.size() > 0) {
        if (pImpl->sensoryHistory.size() >= pImpl->historySize) {
            pImpl->sensoryHistory.erase(pImpl->sensoryHistory.begin());
        }
        pImpl->sensoryHistory.push_back(sensoryPattern[0]);
    }
}

inline void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                            const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    float totalDiff = 0.0f;
    size_t compareLen = std::min(currentPattern.size(), previousPattern.size());
    
    for (size_t i = 0; i < compareLen; ++i) {
        float diff = std::abs(currentPattern[i] - previousPattern[i]);
        totalDiff += diff;
    }
    
    float avgDiff = totalDiff / compareLen;
    
    float novelty = std::min(1.0f, avgDiff / pImpl->noveltyThreshold);
    
    float historicalInfluence = 0.0f;
    if (pImpl->patternHistory.size() > 5) {
        float recentSum = 0.0f;
        size_t count = std::min<size_t>(pImpl->patternHistory.size(), 10);
        for (size_t i = pImpl->patternHistory.size() - count; i < pImpl->patternHistory.size(); ++i) {
            recentSum += pImpl->patternHistory[i];
        }
        historicalInfluence = recentSum / count;
    }
    
    float combinedNovelty = 0.7f * novelty + 0.3f * historicalInfluence;
    
    if (combinedNovelty > pImpl->adaptationThreshold) {
        pImpl->level = std::min(pImpl->maxLevel, pImpl->level + combinedNovelty * 1.5f);
        
        if (pImpl->brain) {
            pImpl->brain->triggerCuriosity(combinedNovelty);
        }
    } else {
        pImpl->level += (combinedNovelty - pImpl->level) * pImpl->accumulationRate;
    }
    
    if (pImpl->patternHistory.size() >= pImpl->historySize) {
        pImpl->patternHistory.erase(pImpl->patternHistory.begin());
    }
    pImpl->patternHistory.push_back(combinedNovelty);
}

inline void Novelty::update(TimestepDuration dt) {
    float decayFactor = std::exp(-pImpl->decayRate * static_cast<float>(dt));
    pImpl->level *= decayFactor;
    
    if (pImpl->level < 0.01f && pImpl->patternHistory.size() > 10) {
        float recentNovelty = 0.0f;
        size_t recentCount = std::min<size_t>(pImpl->patternHistory.size(), 10);
        for (size_t i = 0; i < recentCount; ++i) {
            recentNovelty += pImpl->patternHistory[pImpl->patternHistory.size() - 1 - i];
        }
        recentNovelty /= recentCount;
        
        if (recentNovelty < pImpl->noveltyThreshold * 0.5f) {
            pImpl->level = 0.0f;
        }
    }
}

inline const std::vector<float>& Novelty::getHistory() const {
    return pImpl->patternHistory;
}

inline void Novelty::clearHistory() {
    pImpl->patternHistory.clear();
    pImpl->sensoryHistory.clear();
    pImpl->predictionErrors.clear();
}

} // namespace nlm
