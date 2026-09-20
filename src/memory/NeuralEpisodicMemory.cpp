#include "EpisodicMemory.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <chrono>
#include <random>

namespace nlm {

struct NeuralEpisodicMemory::Impl {
    Brain* brain;
    std::vector<Episode> episodes;
    size_t maxEpisodes;
    float consolidationProgress;
    float replayStrength;
    float agingRate;
    float temporalSpacing;
    std::vector<bool> importantFlags;
    
    Impl() : brain(nullptr), maxEpisodes(1000), consolidationProgress(0.0f), 
             replayStrength(0.8f), agingRate(0.0001f), temporalSpacing(100.0f) {}
};

NeuralEpisodicMemory::NeuralEpisodicMemory() : pImpl(new Impl) {}

NeuralEpisodicMemory::~NeuralEpisodicMemory() = default;

void NeuralEpisodicMemory::initialize(Brain* brainRef) {
    pImpl->brain = brainRef;
    episodes.clear();
    importantFlags.clear();
    
    NLM_LOG_DEBUG("NeuralEpisodicMemory initialized");
}

void NeuralEpisodicMemory::setMaxEpisodes(size_t maxEps) {
    pImpl->maxEpisodes = maxEps;
    
    // Trim old episodes if over limit
    while (episodes.size() > maxEps) {
        // Remove least important episodes
        std::vector<size_t> episodeIds;
        for (size_t i = 0; i < episodes.size(); ++i) {
            episodeIds.push_back(i);
        }
        
        // Sort by importance (simplified: by length and recent activity)
        std::sort(episodeIds.begin(), episodeIds.end(),
            [this](size_t a, size_t b) {
                return episodes[a].getPatternCount() < episodes[b].getPatternCount();
            });
        
        // Remove oldest
        episodes.erase(episodes.begin());
        importantFlags.erase(importantFlags.begin());
    }
}

void NeuralEpisodicMemory::recordEpisode(const std::vector<std::vector<float>>& patterns,
                                         const std::vector<Timestamp>& spikes,
                                         float reward, float predictionError) {
    size_t episodeId = episodes.size();
    
    Episode episode(episodeId);
    episode.neuralPatterns = patterns;
    episode.spikeTimestamps = spikes;
    episode.rewardSignals.push_back(reward);
    episode.predictionErrors.push_back(predictionError);
    episode.startTime = spikes.empty() ? 0.0f : spikes[0];
    episode.endTime = spikes.empty() ? 0.0f : spikes.back();
    
    // Mark as important if it has high reward or prediction error
    bool important = (reward > 0.1f) || (std::abs(predictionError) > 0.05f);
    importantFlags.push_back(important);
    
    episodes.push_back(std::move(episode));
    
    NLM_LOG_DEBUG("NeuralEpisodicMemory recorded episode " + std::to_string(episodeId) +
                 " (patterns: " + std::to_string(patterns.size()) + ", " +
                 "reward: " + std::to_string(reward) + ", " +
                 "error: " + std::to_string(predictionError) + ")");
}

std::vector<size_t> NeuralEpisodicMemory::getEpisodesForReplay(size_t count) const {
    if (episodes.empty()) {
        return std::vector<size_t>();
    }
    
    // Select episodes for replay based on:
    // 1. Importance (high reward/error)
    // 2. Recentness
    // 3. Diversity (different time periods)
    
    std::vector<size_t> episodeIds;
    episodeIds.reserve(episodes.size());
    
    for (size_t i = 0; i < episodes.size(); ++i) {
        episodeIds.push_back(i);
    }
    
    // Sort by importance and recency
    std::sort(episodeIds.begin(), episodeIds.end(),
        [this](size_t a, size_t b) {
            // Prefer important episodes
            if (importantFlags[a] != importantFlags[b]) {
                return importantFlags[a] > importantFlags[b];
            }
            // Prefer more recent episodes
            return episodes[a].endTime > episodes[b].endTime;
        });
    
    // Return top N episodes
    size_t returnCount = std::min(count, episodes.size());
    std::vector<size_t> selected(episodeIds.begin(), episodeIds.begin() + returnCount);
    
    NLM_LOG_DEBUG("NeuralEpisodicMemory selecting " + std::to_string(returnCount) +
                 " episodes for replay");
    
    return selected;
}

Episode NeuralEpisodicMemory::getEpisode(size_t episodeId) const {
    if (episodeId >= episodes.size()) {
        return Episode(episodeId);  // Return empty episode
    }
    return episodes[episodeId];
}

std::vector<float> NeuralEpisodicMemory::completePattern(const std::vector<float>& partialPattern) const {
    // Simple pattern completion: find similar stored patterns
    if (partialPattern.empty() || episodes.empty()) {
        return partialPattern;
    }
    
    std::vector<float> completed = partialPattern;
    
    // Look for similar patterns in recent episodes
    for (size_t i = episodes.size() - 1; i < episodes.size() && i < 10; --i) {
        const auto& episode = episodes[i];
        if (!episode.neuralPatterns.empty()) {
            // Simple averaging of similar patterns
            // In real implementation, use proper similarity metrics
            size_t patternIdx = i % episode.neuralPatterns.size();
            const auto& storedPattern = episode.neuralPatterns[patternIdx];
            
            if (storedPattern.size() == completed.size()) {
                for (size_t j = 0; j < completed.size(); ++j) {
                    completed[j] = (completed[j] + storedPattern[j]) * 0.5f;
                }
            }
        }
    }
    
    NLM_LOG_DEBUG("NeuralEpisodicMemory pattern completed");
    return completed;
}

size_t NeuralEpisodicMemory::getEpisodeCount() const {
    return episodes.size();
}

size_t NeuralEpisodicMemory::getMaxEpisodes() const {
    return pImpl->maxEpisodes;
}

float NeuralEpisodicMemory::getConsolidationProgress() const {
    return pImpl->consolidationProgress;
}

void NeuralEpisodicMemory::replayEpisode(size_t episodeId, float speed) {
    if (episodeId >= episodes.size()) {
        return;
    }
    
    const auto& episode = episodes[episodeId];
    
    NLM_LOG_DEBUG("NeuralEpisodicMemory replaying episode " + std::to_string(episodeId) +
                 " (duration: " + std::to_string(episode.getDuration()) + "s)");
    
    // Replay would involve:
    // 1. Activating neural patterns
    // 2. Generating spikes
    // 3. Updating brain state
    // For now, just log
}

void NeuralEpisodicMemory::replaySequence(const std::vector<size_t>& episodeIds) {
    NLM_LOG_DEBUG("NeuralEpisodicMemory replaying " + std::to_string(episodeIds.size()) +
                 " episodes in sequence");
}

void NeuralEpisodicMemory::consolidateEpisodes(const std::vector<size_t>& episodeIds) {
    pImpl->consolidationProgress = std::min(1.0f, pImpl->consolidationProgress + 0.1f);
    
    NLM_LOG_DEBUG("NeuralEpisodicMemory consolidating " + std::to_string(episodeIds.size()) +
                 " episodes (progress: " + std::to_string(pImpl->consolidationProgress) + ")");
}

void NeuralEpisodicMemory::consolidateImportantEpisodes() {
    std::vector<size_t> importantIds;
    
    for (size_t i = 0; i < importantFlags.size(); ++i) {
        if (importantFlags[i]) {
            importantIds.push_back(i);
        }
    }
    
    if (!importantIds.empty()) {
        consolidateEpisodes(importantIds);
        NLM_LOG_DEBUG("NeuralEpisodicMemory consolidated " + std::to_string(importantIds.size()) +
                     " important episodes");
    }
}

void NeuralEpisodicMemory::update(float timeStep) {
    // Apply aging and decay
    applyAging(pImpl->agingRate * timeStep);
    
    // Update consolidation progress
    if (!episodes.empty()) {
        pImpl->consolidationProgress = std::min(1.0f, pImpl->consolidationProgress + 0.001f * timeStep);
    }
}

void NeuralEpisodicMemory::integrateWithWorkingMemory(WorkingMemory* workingMem) {
    if (!workingMem || !pImpl->brain) {
        return;
    }
    
    // Integrate recent episodic patterns into working memory
    if (!episodes.empty()) {
        const auto& lastEpisode = episodes.back();
        if (!lastEpisode.neuralPatterns.empty()) {
            for (const auto& pattern : lastEpisode.neuralPatterns) {
                workingMem->addTrace(pattern);
            }
            
            NLM_LOG_DEBUG("NeuralEpisodicMemory integrated " + std::to_string(lastEpisode.getPatternCount()) +
                         " patterns into working memory");
        }
    }
}

void NeuralEpisodicMemory::reset() {
    episodes.clear();
    importantFlags.clear();
    pImpl->consolidationProgress = 0.0f;
    NLM_LOG_DEBUG("NeuralEpisodicMemory reset");
}

void NeuralEpisodicMemory::applyAging(float decayRate) {
    for (auto& episode : episodes) {
        // Gradually reduce episode strength over time
        // Could involve clearing old spike patterns, etc.
    }
}

} // namespace nlm