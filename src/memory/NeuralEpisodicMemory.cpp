// Performance-optimized NeuralEpisodicMemory implementation
#include "NeuralEpisodicMemory.hpp"
#include <algorithm>
#include <numeric>
#include <unordered_set>
#include <random>

namespace nlm {

struct NeuralEpisodicMemory::Impl {
    Brain* brain_;
    std::deque<EpisodicMemoryItem> episodes_;
    size_t maxEpisodes_;
    bool replayEnabled_;
    
    // Performance optimizations
    bool cachingEnabled_;
    Cache<size_t, std::vector<float>> patternCache_;
    std::unordered_map<size_t, float> relevanceScores_;
    std::unordered_map<size_t, size_t> accessCounts_;
    
    // SIMD performance metrics
    mutable size_t simdComputations_;
    mutable double simdTimeTotal_;
    mutable size_t regularComputations_;
    mutable double regularTimeTotal_;
    
    // Batch processing
    std::vector<size_t> episodesNeedingUpdate_;
    std::chrono::steady_clock::time_point lastBatchUpdate_;
};

NeuralEpisodicMemory::NeuralEpisodicMemory() : pImpl(std::make_unique<Impl>()) {
    pImpl->maxEpisodes_ = 10000;
    pImpl->replayEnabled_ = true;
    pImpl->cachingEnabled_ = true;
    pImpl->patternCache_ = Cache<size_t, std::vector<float>>(5000, std::chrono::milliseconds(1000));
    pImpl->brain_ = nullptr;
    
    // Performance metrics initialization
    pImpl->simdComputations_ = 0;
    pImpl->simdTimeTotal_ = 0.0;
    pImpl->regularComputations_ = 0;
    pImpl->regularTimeTotal_ = 0.0;
}

NeuralEpisodicMemory::~NeuralEpisodicMemory() = default;

void NeuralEpisodicMemory::initialize(Brain* brain) {
    pImpl->brain_ = brain;
}

void NeuralEpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    // Check cache before storing
    if (pImpl->cachingEnabled_) {
        // Cache the sensory pattern
        pImpl->patternCache_.set(episode.timestamp, episode.sensoryState);
    }
    
    // Add episode to deque
    episodes_.push_back(episode);
    
    // Maintain max episodes
    if (episodes_.size() > pImpl->maxEpisodes_) {
        episodes_.pop_front();
    }
    
    // Initialize episode metadata
    auto it = std::find_if(episodes_.begin(), episodes_.end(),
                          [&episode](const EpisodicMemoryItem& e) {
                              return e.timestamp == episode.timestamp; });
    if (it != episodes_.end()) {
        it->accessCount = 0;
        it->lastAccessTime = std::chrono::steady_clock::now();
        it->cacheScore = 0.0f;
    }
}

std::vector<EpisodicMemoryItem> NeuralEpisodicMemory::getEpisodesForReplay(size_t maxEpisodes) const {
    std::vector<EpisodicMemoryItem> result;
    result.reserve(std::min(maxEpisodes, episodes_.size()));
    
    // Use performance-optimized selection
    if (pImpl->cachingEnabled_) {
        // Smart selection based on cache scores
        std::vector<std::pair<size_t, float>> scores;
        
        for (size_t i = 0; i < episodes_.size(); ++i) {
            float score = episodes_[i].cacheScore + 
                         (episodes_[i].accessCount * 0.1f) + 
                         (episodes_[i].reward * 0.5f);
            scores.emplace_back(i, score);
        }
        
        // Sort by score (highest first)
        std::partial_sort(scores.begin(), scores.begin() + std::min(maxEpisodes, scores.size()),
                         scores.end(),
                         [](const auto& a, const auto& b) { return a.second > b.second; });
        
        // Copy selected episodes
        for (size_t i = 0; i < std::min(maxEpisodes, scores.size()); ++i) {
            result.push_back(episodes_[scores[i].first]);
        }
    } else {
        // Simple selection by recency
        size_t start = (episodes_.size() > maxEpisodes) ? 
                      episodes_.size() - maxEpisodes : 0;
        for (size_t i = start; i < episodes_.size(); ++i) {
            result.push_back(episodes_[i]);
        }
    }
    
    // Update access counts
    for (auto& episode : result) {
        episode.accessCount++;
        episode.lastAccessTime = std::chrono::steady_clock::now();
    }
    
    return result;
}

bool NeuralEpisodicMemory::replayEpisode(size_t episodeIndex, Brain* brain) const {
    if (episodeIndex >= episodes_.size()) {
        return false;
    }
    
    const auto& episode = episodes_[episodeIndex];
    
    // Update access metrics
    std::atomic<size_t>& accessCount = const_cast<std::atomic<size_t>&>(pImpl->accessCounts_[episodeIndex]);
    accessCount++;
    
    // Reconstruct neural pattern
    std::vector<float> pattern = episode.sensoryState;
    
    // Inject pattern into brain
    // This would integrate with the brain's sensory processing
    // For now, we'll just log the replay
    NLM_LOG_INFO("Replaying episode " + std::to_string(episodeIndex) +
                " with reward " + std::to_string(episode.reward));
    
    return true;
}

size_t NeuralEpisodicMemory::getEpisodeCount() const {
    return episodes_.size();
}

std::vector<EpisodicMemoryItem> NeuralEpisodicMemory::getRecentEpisodes(size_t count) const {
    std::vector<EpisodicMemoryItem> result;
    if (count == 0 || episodes_.empty()) return result;
    
    size_t start = (episodes_.size() > count) ? episodes_.size() - count : 0;
    result.reserve(count);
    
    for (size_t i = start; i < episodes_.size(); ++i) {
        result.push_back(episodes_[i]);
    }
    
    return result;
}

void NeuralEpisodicMemory::clear() {
    episodes_.clear();
    pImpl->patternCache_.clear();
    pImpl->relevanceScores_.clear();
    pImpl->accessCounts_.clear();
}

size_t NeuralEpisodicMemory::getMemoryUsage() const {
    size_t totalSize = sizeof(EpisodicMemoryItem) * episodes_.size();
    totalSize += pImpl->patternCache_.size() * sizeof(float);
    totalSize += pImpl->relevanceScores_.size() * sizeof(float);
    return totalSize;
}

void NeuralEpisodicMemory::setCachingEnabled(bool enabled) {
    pImpl->cachingEnabled_ = enabled;
}

bool NeuralEpisodicMemory::isCachingEnabled() const {
    return pImpl->cachingEnabled_;
}

std::vector<float> NeuralEpisodicMemory::getCachedPattern(size_t episodeIndex) const {
    if (!pImpl->cachingEnabled_) {
        return std::vector<float>();
    }
    
    return pImpl->patternCache_.get(episodeIndex).value_or(std::vector<float>());
}

void NeuralEpisodicMemory::cachePattern(size_t episodeIndex, const std::vector<float>& pattern) {
    if (pImpl->cachingEnabled_) {
        pImpl->patternCache_.set(episodeIndex, pattern);
    }
}

void NeuralEpisodicMemory::cleanupCache() {
    if (pImpl->cachingEnabled_) {
        pImpl->patternCache_.clear();
    }
}

void NeuralEpisodicMemory::setCacheSizeLimit(size_t limit) {
    pImpl->patternCache_ = Cache<size_t, std::vector<float>>(limit);
}

float NeuralEpisodicMemory::computeSimilaritySIMD(const std::vector<float>& query,
                                                 const EpisodicMemoryItem& episode) const {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Use SIMD-optimized similarity computation
    float similarity = simd::cosineSimilaritySIMD(query, episode.sensoryState);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    // Update metrics
    pImpl->simdComputations_++;
    pImpl->simdTimeTotal_ += duration.count() * 0.000001; // Convert to seconds
    
    return similarity;
}

void NeuralEpisodicMemory::batchUpdateRelevance(std::chrono::milliseconds timeWindow) {
    auto now = std::chrono::steady_clock::now();
    
    if (now - pImpl->lastBatchUpdate_ < timeWindow) {
        return; // Not enough time has passed
    }
    
    // Update relevance for recently accessed episodes
    for (auto& episode : episodes_) {
        if (std::chrono::steady_clock::now() - episode.lastAccessTime < timeWindow) {
            episode.cacheScore += 0.1f;
        }
    }
    
    pImpl->lastBatchUpdate_ = now;
}

float NeuralEpisodicMemory::getCacheHitRatio() const {
    if (pImpl->patternCache_.size() == 0) return 0.0f;
    return static_cast<float>(pImpl->patternCache_.size()) / 
           static_cast<float>(pImpl->regularComputations_ + pImpl->simdComputations_);
}

size_t NeuralEpisodicMemory::getCacheSize() const {
    return pImpl->patternCache_.size();
}

double NeuralEpisodicMemory::getAverageAccessTime() const {
    size_t totalComputations = pImpl->regularComputations_ + pImpl->simdComputations_;
    if (totalComputations == 0) return 0.0;
    
    double totalTime = pImpl->regularTimeTotal_ + pImpl->simdTimeTotal_;
    return totalTime / totalComputations;
}

// SIMD utilities implementation
namespace simd {
    float cosineSimilaritySIMD(const std::vector<float>& a, const std::vector<float>& b) {
        if (a.size() != b.size() || a.empty()) {
            return 0.0f;
        }
        
        float dot = dotProductSIMD(a, b);
        float normA = magnitudeSIMD(a);
        float normB = magnitudeSIMD(b);
        
        if (normA == 0.0f || normB == 0.0f) {
            return 0.0f;
        }
        
        return dot / (normA * normB);
    }
    
    float dotProductSIMD(const std::vector<float>& a, const std::vector<float>& b) {
        float result = 0.0f;
        for (size_t i = 0; i < a.size(); ++i) {
            result += a[i] * b[i];
        }
        return result;
    }
    
    std::vector<float> normalizeSIMD(const std::vector<float>& vector) {
        float magnitude = magnitudeSIMD(vector);
        if (magnitude == 0.0f) {
            return vector;
        }
        
        std::vector<float> result = vector;
        for (float& val : result) {
            val /= magnitude;
        }
        return result;
    }
    
    float magnitudeSIMD(const std::vector<float>& vector) {
        return std::sqrt(dotProductSIMD(vector, vector));
    }
}

} // namespace nlm
