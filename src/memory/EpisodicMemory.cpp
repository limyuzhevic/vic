#include "MemoryInterface.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <queue>

namespace nlm {

class EpisodicMemory::Impl {
public:
    Brain* brain;
    size_t capacity;
    size_t maxReplayAge;
    bool replayEnabled;
    
    // Episodes
    std::vector<MemoryItem> episodes;
    std::deque<SimulationStep> replayQueue;
    
    // Neural representations
    std::vector<size_t> activeNeurons;
    
    Impl(size_t cap, size_t maxAge) : capacity(cap), maxReplayAge(maxAge), replayEnabled(true) {}
};

EpisodicMemory::EpisodicMemory()
    : pImpl(new Impl(1000, 100))
{
    NLM_LOG_INFO("EpisodicMemory created");
}

EpisodicMemory::~EpisodicMemory() = default;

void EpisodicMemory::store(const std::vector<float>& pattern, float relevance) {
    if (pattern.empty()) return;
    
    // Store as an episode
    MemoryItem episode(pImpl->episodes.size(), pattern);
    episode.relevance = relevance;
    episode.age = 0;
    
    if (pImpl->episodes.size() >= pImpl->capacity) {
        pImpl->episodes.erase(pImpl->episodes.begin());
    }
    
    pImpl->episodes.push_back(episode);
    
    // Neural integration
    if (pImpl->brain && pImpl->episodes.size() > 1) {
        // Create associative links with previous episode
        const MemoryItem& prevEpisode = pImpl->episodes[pImpl->episodes.size() - 2];
        // In a real implementation, this would create associations
    }
}

std::vector<float> EpisodicMemory::retrieve(size_t index) const {
    if (index < pImpl->episodes.size()) {
        return pImpl->episodes[index].pattern;
    }
    return {};
}

bool EpisodicMemory::contains(const std::vector<float>& pattern) const {
    for (const auto& episode : pImpl->episodes) {
        if (pattern.size() == episode.pattern.size()) {
            bool match = true;
            for (size_t i = 0; i < pattern.size(); ++i) {
                if (std::abs(pattern[i] - episode.pattern[i]) > 0.1f) {
                    match = false;
                    break;
                }
            }
            if (match) return true;
        }
    }
    return false;
}

void EpisodicMemory::decay(float decayRate) {
    for (auto& episode : pImpl->episodes) {
        episode.relevance *= (1.0f - decayRate);
        episode.age++;
    }
}

void EpisodicMemory::consolidate(float relevanceThreshold) {
    // Remove low-relevance episodes
    pImpl->episodes.erase(
        std::remove_if(pImpl->episodes.begin(), pImpl->episodes.end(),
                     [relevanceThreshold](const MemoryItem& episode) {
                         return episode.relevance < relevanceThreshold;
                     }),
        pImpl->episodes.end()
    );
}

void EpisodicMemory::clear() {
    pImpl->episodes.clear();
    pImpl->replayQueue.clear();
}

size_t EpisodicMemory::getCapacity() const {
    return pImpl->capacity;
}

size_t EpisodicMemory::getCurrentSize() const {
    return pImpl->episodes.size();
}

float EpisodicMemory::getLoad() const {
    if (pImpl->capacity == 0) return 0.0f;
    return static_cast<float>(pImpl->episodes.size()) / pImpl->capacity;
}

void EpisodicMemory::updateFromBrain(Brain* brain) {
    pImpl->brain = brain;
}

void EpisodicMemory::applyToBrain(Brain* brain) {
    if (!brain || pImpl->brain != brain) return;
    
    // Replay recent episodes
    if (pImpl->replayEnabled && !pImpl->episodes.empty()) {
        // Replay the most recent episode
        size_t recentIndex = std::max(0, static_cast<int>(pImpl->episodes.size()) - 3);
        replay(recentIndex);
    }
}

void EpisodicMemory::storeEpisode(const std::vector<float>& sensoryPattern, 
                                 const std::vector<float>& action,
                                 float reward,
                                 const std::vector<float>& resultingPattern) {
    // Create a composite pattern containing all episode information
    std::vector<float> episodePattern;
    
    // Add sensory pattern (normalized)
    for (float val : sensoryPattern) {
        episodePattern.push_back(val / 10.0f);  // Normalize
    }
    
    // Add action (encoded as additional dimensions)
    for (size_t i = 0; i < std::min(action.size(), 3u); ++i) {
        episodePattern.push_back(action[i] / 10.0f);
    }
    
    // Add reward
    episodePattern.push_back(reward);
    
    // Add resulting pattern
    for (float val : resultingPattern) {
        episodePattern.push_back(val / 10.0f);
    }
    
    store(episodePattern, reward);
}

std::vector<const MemoryItem*> EpisodicMemory::retrieveSimilar(const std::vector<float>& pattern, 
                                                              size_t maxResults) const {
    std::vector<std::pair<float, size_t>> scores;
    
    for (size_t i = 0; i < pImpl->episodes.size(); ++i) {
        float similarity = computeSimilarity(pattern, pImpl->episodes[i]);
        scores.emplace_back(similarity, i);
    }
    
    // Sort by similarity (descending)
    std::sort(scores.begin(), scores.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Return top results
    std::vector<const MemoryItem*> results;
    for (size_t i = 0; i < std::min(maxResults, scores.size()); ++i) {
        if (scores[i].first > 0.3f) {  // Threshold
            results.push_back(&pImpl->episodes[scores[i].second]);
        }
    }
    
    return results;
}

std::vector<const MemoryItem*> EpisodicMemory::retrieveTemporal(SimulationStep startTime,
                                                               SimulationStep endTime,
                                                               size_t maxResults) const {
    std::vector<const MemoryItem*> results;
    
    for (const auto& episode : pImpl->episodes) {
        if (episode.age >= startTime && episode.age <= endTime) {
            results.push_back(&episode);
        }
    }
    
    // Sort by recency (most recent first)
    std::sort(results.begin(), results.end(),
              [](const MemoryItem* a, const MemoryItem* b) {
                  return a->age > b->age;
              });
    
    if (results.size() > maxResults) {
        results.resize(maxResults);
    }
    
    return results;
}

void EpisodicMemory::replay(size_t index) {
    if (index >= pImpl->episodes.size()) return;
    
    const MemoryItem& episode = pImpl->episodes[index];
    
    // Inject pattern into brain
    if (pImpl->brain) {
        for (size_t i = 0; i < episode.pattern.size() && i < pImpl->activeNeurons.size(); ++i) {
            float activation = episode.pattern[i] * 3.0f;  // Amplify for replay
            pImpl->brain->injectCurrent(pImpl->activeNeurons[i], activation);
        }
    }
    
    // Add to replay queue for timing
    pImpl->replayQueue.push_back(episode.age);
}

void EpisodicMemory::enableReplay(bool enable) {
    pImpl->replayEnabled = enable;
}

bool EpisodicMemory::isReplayEnabled() const {
    return pImpl->replayEnabled;
}

void EpisodicMemory::advanceTime(SimulationStep steps) {
    for (auto& episode : pImpl->episodes) {
        episode.age += steps;
    }
}

void EpisodicMemory::consolidateOld(float ageThreshold) {
    // Remove very old episodes
    pImpl->episodes.erase(
        std::remove_if(pImpl->episodes.begin(), pImpl->episodes.end(),
                     [ageThreshold](const MemoryItem& episode) {
                         return episode.age > ageThreshold;
                     }),
        pImpl->episodes.end()
    );
}

float EpisodicMemory::computeSimilarity(const std::vector<float>& a,
                                       const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    float dot = 0.0f, normA = 0.0f, normB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(normA) * std::sqrt(normB));
}

} // namespace nlm