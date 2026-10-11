#include "Novelty.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct Novelty::Impl {
    Brain* brain;
    float level;
    float decayRate;
    float baselineLevel;
    std::vector<float> history;
    std::vector<float> patternHistory;
    float noveltyThreshold;
    float adaptiveThreshold;
    std::vector<float> predictionHistory;
    
    Impl() : brain(nullptr), level(0.0f), decayRate(0.05f), baselineLevel(0.0f),
             noveltyThreshold(0.3f), adaptiveThreshold(0.3f) {}
};

Novelty::Novelty() : pImpl(new Impl) {}

Novelty::~Novelty() = default;

void Novelty::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Novelty detection initialized");
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = level;
}

void Novelty::detectNovelty(const Observation& observation,
                            const Observation& previousObservation) {
    // Simple implementation using observation features
    float novelty = 0.0f;
    
    const auto& currentData = observation.getData();
    const auto& previousData = previousObservation.getData();
    
    if (!currentData.empty() && !previousData.empty()) {
        size_t compareSize = std::min(currentData.size(), previousData.size());
        
        // Compute feature differences
        float totalDiff = 0.0f;
        for (size_t i = 0; i < compareSize; ++i) {
            float diff = std::abs(currentData[i] - previousData[i]);
            totalDiff += diff;
        }
        
        // Normalize and apply novelty scaling
        float avgDiff = totalDiff / compareSize;
        novelty = std::min(1.0f, avgDiff / pImpl->noveltyThreshold);
    }
    
    pImpl->level = novelty;
    pImpl->history.push_back(novelty);
    
    // Update baseline based on recent history
    if (pImpl->history.size() > 100) {
        float sum = 0.0f;
        for (size_t i = 1; i < pImpl->history.size(); ++i) {
            sum += pImpl->history[i];
        }
        pImpl->baselineLevel = sum / pImpl->history.size();
        
        // Adjust adaptive threshold
        pImpl->adaptiveThreshold = pImpl->baselineLevel * 2.0f;
    }
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                            const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Compute difference between patterns using multiple metrics
    float totalDiff = 0.0f;
    size_t compareLen = std::min(currentPattern.size(), previousPattern.size());
    
    for (size_t i = 0; i < compareLen; ++i) {
        float diff = std::abs(currentPattern[i] - previousPattern[i]);
        totalDiff += diff;
    }
    
    // Normalize by pattern size and apply novelty scaling
    float avgDiff = totalDiff / compareLen;
    
    // Apply adaptive thresholding
    float normalizedNovelty = avgDiff / pImpl->adaptiveThreshold;
    
    // Combine with baseline
    float novelContribution = std::min(1.0f, normalizedNovelty);
    pImpl->level = novelContribution * 0.7f + pImpl->baselineLevel * 0.3f;
    
    pImpl->history.push_back(pImpl->level);
    
    // Store current pattern for next comparison
    pImpl->patternHistory = currentPattern;
    
    // Update prediction history for temporal novelty detection
    if (!previousPattern.empty()) {
        pImpl->predictionHistory.push_back(1.0f - novelContribution);
    }
}

void Novelty::update(TimestepDuration dt) {
    // Decay novelty over time
    pImpl->level *= (1.0f - pImpl->decayRate * static_cast<float>(dt));
    
    // Keep history bounded
    if (pImpl->history.size() > 10000) {
        pImpl->history.erase(pImpl->history.begin());
        if (!pImpl->patternHistory.empty()) {
            pImpl->patternHistory.erase(pImpl->patternHistory.begin());
        }
        if (!pImpl->predictionHistory.empty()) {
            pImpl->predictionHistory.erase(pImpl->predictionHistory.begin());
        }
    }
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
    pImpl->patternHistory.clear();
    pImpl->predictionHistory.clear();
    pImpl->baselineLevel = 0.0f;
    pImpl->level = 0.0f;
}

void Novelty::integrateWithMemorySystems() {
    if (!pImpl->brain) return;
    
    // Use novelty to guide episodic memory encoding
    NeuralEpisodicMemory* episodicMemory = pImpl->brain->getEpisodicMemory();
    if (episodicMemory) {
        // Store episode with novelty indicator
        float noveltyLevel = pImpl->level;
        
        // Find episodes with similar patterns
        if (!pImpl->patternHistory.empty()) {
            auto similar = episodicMemory->retrieveSimilar(pImpl->patternHistory, 5);
            for (const auto* episode : similar) {
                // Update episode relevance based on novelty
                episodicMemory->updateRelevance(episode->timestamp, 
                                               noveltyLevel * 0.1f);
            }
        }
    }
}

void Novelty::driveExploration(Brain* brain) {
    if (!brain || pImpl->level < 0.1f) return;
    
    // Use novelty to drive exploration behavior
    // Increase action selection randomness based on novelty level
    float explorationModulation = pImpl->level * 0.5f;  // Modulate exploration by 0-0.5
    
    // This would integrate with action selection in a full implementation
    // For now, just log the exploration drive
    if (pImpl->level > 0.5f) {
        NLM_LOG_INFO("High novelty driving exploration (level: " + 
                    std::to_string(pImpl->level) + ")");
    }
}

void Novelty::modulateAttention(float noveltyLevel) {
    if (!pImpl->brain) return;
    
    // Use novelty to modulate attention
    AttentionalSelection* attention = pImpl->brain->getAttention();
    if (attention) {
        float attentionBoost = noveltyLevel * 0.3f;  // Boost attention by novelty
        
        // Apply to attended regions
        for (RegionId region : attention->getAttendedRegions()) {
            attention->applyTopDownBias(NeuronId(region.index()), 
                                       attentionBoost);
        }
        
        // Update competition threshold
        float newThreshold = 0.3f + noveltyLevel * 0.4f;
        attention->setCompetitionThreshold(newThreshold);
    }
}

void Novelty::triggerStructuralPlasticity(float noveltyLevel) {
    if (!pImpl->brain) return;
    
    // Use novelty to trigger structural changes
    StructuralPlasticity* plasticity = pImpl->brain->getStructuralPlasticity();
    if (plasticity && noveltyLevel > 0.7f) {
        // High novelty triggers increased synaptogenesis
        float formationRate = plasticity->getFormationRate() * (1.0f + noveltyLevel * 2.0f);
        plasticity->setFormationRate(formationRate);
        
        NLM_LOG_INFO("Novelty-triggered synaptogenesis (level: " + 
                    std::to_string(noveltyLevel) + ")");
    }
}

void Novelty::updateLearningRate(float noveltyLevel) {
    // Adjust learning rate based on novelty
    float adaptiveLearningRate = 0.1f + noveltyLevel * 0.3f;  // 0.1 to 0.4
    
    // This would apply to various learning mechanisms
    // For example: STDP learning rates, Hebbian learning rates, etc.
    
    // Log learning rate adjustment
    if (noveltyLevel > 0.5f) {
        NLM_LOG_INFO("Novelty-adjusted learning rate (level: " + 
                    std::to_string(noveltyLevel) + ")");
    }
}

void Novelty::applyTemporalNoveltyAnalysis() {
    // Analyze novelty over time for temporal patterns
    if (pImpl->predictionHistory.size() < 10) return;
    
    // Compute novelty trends
    float recentAvg = 0.0f;
    size_t recentCount = std::min(size_t(10), pImpl->predictionHistory.size());
    
    for (size_t i = 0; i < recentCount; ++i) {
        recentAvg += pImpl->predictionHistory[pImpl->predictionHistory.size() - 1 - i];
    }
    recentAvg /= recentCount;
    
    // Adapt threshold based on trends
    if (recentAvg < 0.2f) {
        // Low novelty trend - increase threshold
        pImpl->adaptiveThreshold *= 1.1f;
    } else if (recentAvg > 0.8f) {
        // High novelty trend - decrease threshold
        pImpl->adaptiveThreshold *= 0.9f;
    }
}

float Novelty::getAdaptiveThreshold() const {
    return pImpl->adaptiveThreshold;
}

float Novelty::getBaselineLevel() const {
    return pImpl->baselineLevel;
}

const std::vector<float>& Novelty::getPatternHistory() const {
    return pImpl->patternHistory;
}

void Novelty::setDecayRate(float rate) {
    pImpl->decayRate = std::clamp(rate, 0.0f, 1.0f);
}

void Novelty::setNoveltyThreshold(float threshold) {
    pImpl->noveltyThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

} // namespace nlm
