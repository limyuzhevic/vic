#include "NeuralEpisodicMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <unordered_map>

namespace nlm {

struct EpisodicMemory::Impl {
    std::vector<EpisodicMemoryItemEnhanced> episodes;
    size_t maxEpisodes;
    SimulationStep consolidationInterval;
    float replayFrequency;
    float similarityThreshold;
    
    // Statistical tracking
    float consolidationProgress;
    float replayEffectiveness;
    float temporalCompression;
    
    // Time tracking
    SimulationStep lastConsolidationTime;
    SimulationStep lastReplayTime;
    
    // Context mapping
    std::unordered_map<std::string, std::vector<size_t>> contextIndex;
    
    // Random number generation
    std::mt19937 rng;
    
    Impl(size_t max) : maxEpisodes(max), consolidationInterval(100), 
                       replayFrequency(0.3f), similarityThreshold(0.7f),
                       consolidationProgress(0.0f), replayEffectiveness(0.0f),
                       temporalCompression(1.0f), lastConsolidationTime(0),
                       lastReplayTime(0) {
        rng.seed(std::chrono::steady_clock::now().time_since_epoch().count());
    }
};

EpisodicMemory::EpisodicMemory() : pImpl(new Impl(1000)) {}

EpisodicMemory::~EpisodicMemory() = default;

void EpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    storeEpisodeWithContext(episode.neurons, episode.values, "", episode.timestamp);
}

void EpisodicMemory::storeEpisodeWithContext(const std::vector<NeuronId>& neurons, 
                                            const std::vector<float>& values,
                                            const std::string& context,
                                            SimulationStep currentStep) {
    // Calculate novelty score (lower if similar to recent episodes)
    float noveltyScore = calculateNoveltyScore(neurons, currentStep);
    
    // Calculate relevance score (based on activation strength)
    float relevanceScore = calculateRelevanceScore(values);
    
    // Create enhanced episode
    EpisodicMemoryItemEnhanced enhancedEpisode;
    enhancedEpisode.timestamp = currentStep;
    enhancedEpisode.neurons = neurons;
    enhancedEpisode.values = values;
    enhancedEpisode.context = context;
    enhancedEpisode.noveltyScore = noveltyScore;
    enhancedEpisode.relevanceScore = relevanceScore;
    enhancedEpisode.accessCount = 0;
    enhancedEpisode.lastAccessTime = currentStep;
    enhancedEpisode.temporalCompression = 1.0f;
    
    // Update context index
    if (!context.empty()) {
        pImpl->contextIndex[context].push_back(pImpl->episodes.size());
    }
    
    // Add to episodes
    pImpl->episodes.push_back(enhancedEpisode);
    
    // Enforce max episodes limit
    if (pImpl->episodes.size() > pImpl->maxEpisodes) {
        size_t excess = pImpl->episodes.size() - pImpl->maxEpisodes;
        
        // Remove oldest episodes
        for (size_t i = 0; i < excess; ++i) {
            const auto& oldEpisode = pImpl->episodes[i];
            if (!oldEpisode.context.empty()) {
                auto& contextList = pImpl->contextIndex[oldEpisode.context];
                contextList.erase(
                    std::remove(contextList.begin(), contextList.end(), i),
                    contextList.end()
                );
            }
        }
        
        pImpl->episodes.erase(pImpl->episodes.begin(), pImpl->episodes.begin() + excess);
        
        // Rebuild context index
        rebuildContextIndex();
    }
}

EpisodicMemoryItem EpisodicMemory::retrieveEpisode(size_t index) const {
    if (index >= pImpl->episodes.size()) {
        return EpisodicMemoryItem();
    }
    
    // Update access statistics
    const_cast<EpisodicMemory*>(this)->pImpl->episodes[index].accessCount++;
    const_cast<EpisodicMemory*>(this)->pImpl->episodes[index].lastAccessTime = 
        const_cast<EpisodicMemory*>(this)->pImpl->episodes[index].timestamp;
    
    // Convert back to original format
    EpisodicMemoryItem item;
    item.timestamp = pImpl->episodes[index].timestamp;
    item.neurons = pImpl->episodes[index].neurons;
    item.values = pImpl->episodes[index].values;
    item.metadata = pImpl->episodes[index].context;
    
    return item;
}

std::vector<EpisodicMemoryItem> EpisodicMemory::retrieveEpisodesByTime(SimulationStep startTime, 
                                                                       SimulationStep endTime) const {
    std::vector<EpisodicMemoryItem> result;
    
    for (const auto& episode : pImpl->episodes) {
        if (episode.timestamp >= startTime && episode.timestamp <= endTime) {
            EpisodicMemoryItem item;
            item.timestamp = episode.timestamp;
            item.neurons = episode.neurons;
            item.values = episode.values;
            item.metadata = episode.context;
            result.push_back(item);
        }
    }
    
    return result;
}

std::vector<EpisodicMemoryItem> EpisodicMemory::retrieveEpisodesByContext(const std::string& context) const {
    std::vector<EpisodicMemoryItem> result;
    
    auto it = pImpl->contextIndex.find(context);
    if (it != pImpl->contextIndex.end()) {
        for (size_t index : it->second) {
            if (index < pImpl->episodes.size()) {
                const auto& episode = pImpl->episodes[index];
                EpisodicMemoryItem item;
                item.timestamp = episode.timestamp;
                item.neurons = episode.neurons;
                item.values = episode.values;
                item.metadata = episode.context;
                result.push_back(item);
            }
        }
    }
    
    return result;
}

std::vector<EpisodicMemoryItem> EpisodicMemory::retrieveSimilarEpisodes(const std::vector<NeuronId>& pattern, 
                                                                          float similarityThreshold) const {
    std::vector<EpisodicMemoryItem> result;
    float threshold = similarityThreshold;
    
    for (const auto& episode : pImpl->episodes) {
        float similarity = calculatePatternSimilarity(episode.neurons, pattern);
        if (similarity >= threshold) {
            EpisodicMemoryItem item;
            item.timestamp = episode.timestamp;
            item.neurons = episode.neurons;
            item.values = episode.values;
            item.metadata = episode.context;
            result.push_back(item);
        }
    }
    
    return result;
}

size_t EpisodicMemory::getEpisodeCount() const {
    return pImpl->episodes.size();
}

SimulationStep EpisodicMemory::getOldestEpisodeTime() const {
    if (pImpl->episodes.empty()) return 0;
    return pImpl->episodes.front().timestamp;
}

SimulationStep EpisodicMemory::getNewestEpisodeTime() const {
    if (pImpl->episodes.empty()) return 0;
    return pImpl->episodes.back().timestamp;
}

float EpisodicMemory::getTemporalCoherence() const {
    if (pImpl->episodes.size() < 2) return 1.0f;
    
    float totalCoherence = 0.0f;
    size_t validPairs = 0;
    
    for (size_t i = 0; i < pImpl->episodes.size() - 1; ++i) {
        const auto& episode1 = pImpl->episodes[i];
        const auto& episode2 = pImpl->episodes[i + 1];
        
        float timeGap = episode2.timestamp - episode1.timestamp;
        float coherence = std::exp(-timeGap / 100.0f);  // Decay with time
        totalCoherence += coherence;
        validPairs++;
    }
    
    return validPairs > 0 ? totalCoherence / validPairs : 1.0f;
}

void EpisodicMemory::consolidate(float relevanceThreshold, float noveltyThreshold) {
    float currentTime = getNewestEpisodeTime();
    
    // Check if consolidation should run
    if (currentTime - pImpl->lastConsolidationTime < pImpl->consolidationInterval) {
        return;
    }
    
    pImpl->lastConsolidationTime = currentTime;
    pImpl->consolidationProgress = 0.0f;
    
    // Consolidate highly relevant and novel episodes
    for (auto& episode : pImpl->episodes) {
        if (episode.relevanceScore >= relevanceThreshold && 
            episode.noveltyScore >= noveltyThreshold) {
            
            // Apply temporal compression
            episode.temporalCompression *= 0.9f;
            
            // Update access statistics
            episode.accessCount *= 1.5f;
        }
    }
    
    pImpl->consolidationProgress = 1.0f;
}

void EpisodicMemory::replayRecentEpisodes(float replayStrength) {
    float currentTime = getNewestEpisodeTime();
    
    // Check if replay should run
    if (currentTime - pImpl->lastReplayTime < pImpl->consolidationInterval * pImpl->replayFrequency) {
        return;
    }
    
    pImpl->lastReplayTime = currentTime;
    
    // Get recent episodes for replay
    size_t recentCount = std::min<size_t>(10, pImpl->episodes.size());
    if (recentCount == 0) return;
    
    // Calculate replay effectiveness based on episode quality
    float totalEffectiveness = 0.0f;
    size_t effectiveEpisodes = 0;
    
    for (size_t i = pImpl->episodes.size() - recentCount; i < pImpl->episodes.size(); ++i) {
        auto& episode = pImpl->episodes[i];
        
        // Calculate effectiveness based on novelty and relevance
        float effectiveness = episode.noveltyScore * episode.relevanceScore * replayStrength;
        
        // Apply strengthening
        for (float& value : episode.values) {
            value += effectiveness * 0.1f;
            value = std::clamp(value, 0.0f, 1.0f);
        }
        
        totalEffectiveness += effectiveness;
        effectiveEpisodes++;
    }
    
    pImpl->replayEffectiveness = effectiveEpisodes > 0 ? totalEffectiveness / effectiveEpisodes : 0.0f;
}

void EpisodicMemory::replayWithStrengthening(float baseStrength) {
    // Enhanced replay with active strengthening
    std::uniform_real_distribution<float> dist(0.5f, 1.5f);
    
    for (auto& episode : pImpl->episodes) {
        // Calculate strengthening based on episode properties
        float strength = baseStrength * episode.relevanceScore * episode.noveltyScore;
        
        // Apply strengthening to values
        for (float& value : episode.values) {
            value += strength * 0.05f;
            value = std::clamp(value, 0.0f, 1.0f);
        }
        
        // Update access count
        episode.accessCount++;
    }
}

void EpisodicMemory::resolveTemporalInterference() {
    // Resolve interference between temporally proximal episodes
    if (pImpl->episodes.size() < 2) return;
    
    std::sort(pImpl->episodes.begin(), pImpl->episodes.end(),
              [](const EpisodicMemoryItemEnhanced& a, const EpisodicMemoryItemEnhanced& b) {
                  return a.timestamp < b.timestamp;
              });
    
    // Apply interference resolution based on temporal distance
    for (size_t i = 1; i < pImpl->episodes.size(); ++i) {
        float timeGap = pImpl->episodes[i].timestamp - pImpl->episodes[i-1].timestamp;
        
        if (timeGap < 50.0f) {  // Close in time
            // Reduce interference between close episodes
            pImpl->episodes[i].relevanceScore *= (1.0f - 0.1f * (50.0f - timeGap) / 50.0f);
            pImpl->episodes[i-1].relevanceScore *= (1.0f - 0.1f * (50.0f - timeGap) / 50.0f);
        }
    }
}

void EpisodicMemory::resolveContentInterference() {
    // Resolve interference between semantically similar episodes
    for (size_t i = 0; i < pImpl->episodes.size(); ++i) {
        for (size_t j = i + 1; j < pImpl->episodes.size(); ++j) {
            float similarity = calculatePatternSimilarity(
                pImpl->episodes[i].neurons, pImpl->episodes[j].neurons);
            
            if (similarity > 0.8f) {  // Highly similar content
                // Reduce interference
                pImpl->episodes[i].relevanceScore *= (1.0f - similarity * 0.2f);
                pImpl->episodes[j].relevanceScore *= (1.0f - similarity * 0.2f);
            }
        }
    }
}

std::vector<EpisodicMemoryItem> EpisodicMemory::getRecentEpisodes(size_t count) const {
    std::vector<EpisodicMemoryItem> result;
    size_t start = pImpl->episodes.size() > count ? 
                   pImpl->episodes.size() - count : 0;
    
    for (size_t i = start; i < pImpl->episodes.size(); ++i) {
        const auto& episode = pImpl->episodes[i];
        EpisodicMemoryItem item;
        item.timestamp = episode.timestamp;
        item.neurons = episode.neurons;
        item.values = episode.values;
        item.metadata = episode.context;
        result.push_back(item);
    }
    
    return result;
}

void EpisodicMemory::pruneOldEpisodes(size_t maxCount) {
    if (pImpl->episodes.size() <= maxCount) return;
    
    // Sort by timestamp (oldest first)
    std::sort(pImpl->episodes.begin(), pImpl->episodes.end(),
              [](const EpisodicMemoryItemEnhanced& a, const EpisodicMemoryItemEnhanced& b) {
                  return a.timestamp < b.timestamp;
              });
    
    // Remove oldest episodes
    size_t excess = pImpl->episodes.size() - maxCount;
    pImpl->episodes.erase(pImpl->episodes.begin(), pImpl->episodes.begin() + excess);
    
    // Rebuild context index
    rebuildContextIndex();
}

float EpisodicMemory::getConsolidationProgress() const {
    return pImpl->consolidationProgress;
}

float EpisodicMemory::getReplayEffectiveness() const {
    return pImpl->replayEffectiveness;
}

float EpisodicMemory::getTemporalCompressionRatio() const {
    if (pImpl->episodes.empty()) return 1.0f;
    
    float totalCompression = 0.0f;
    for (const auto& episode : pImpl->episodes) {
        totalCompression += episode.temporalCompression;
    }
    
    return totalCompression / pImpl->episodes.size();
}

float EpisodicMemory::calculateNoveltyScore(const std::vector<NeuronId>& neurons, SimulationStep currentTime) const {
    float novelty = 0.0f;
    
    // Check similarity to recent episodes
    for (const auto& episode : pImpl->episodes) {
        if (currentTime - episode.timestamp > 1000.0f) continue;  // Only consider recent episodes
        
        float similarity = calculatePatternSimilarity(neurons, episode.neurons);
        novelty += (1.0f - similarity);
    }
    
    return novelty / std::max<size_t>(pImpl->episodes.size(), 1);
}

float EpisodicMemory::calculateRelevanceScore(const std::vector<float>& values) const {
    float mean = std::accumulate(values.begin(), values.end(), 0.0f) / values.size();
    float variance = 0.0f;
    
    for (float val : values) {
        variance += (val - mean) * (val - mean);
    }
    variance /= values.size();
    
    return std::sqrt(variance);  // Use standard deviation as relevance
}

float EpisodicMemory::calculatePatternSimilarity(const std::vector<NeuronId>& pattern1,
                                                 const std::vector<NeuronId>& pattern2) const {
    if (pattern1.empty() || pattern2.empty()) return 0.0f;
    
    // Convert to hash for comparison
    std::unordered_map<NeuronId, int> hash1, hash2;
    
    for (NeuronId id : pattern1) hash1[id]++;
    for (NeuronId id : pattern2) hash2[id]++;
    
    // Calculate Jaccard similarity
    int intersection = 0;
    int union_size = 0;
    
    for (const auto& pair : hash1) {
        union_size += pair.second;
        if (hash2.find(pair.first) != hash2.end()) {
            intersection += std::min(pair.second, hash2[pair.first]);
        }
    }
    
    for (const auto& pair : hash2) {
        if (hash1.find(pair.first) == hash1.end()) {
            union_size += pair.second;
        }
    }
    
    return union_size > 0 ? static_cast<float>(intersection) / union_size : 0.0f;
}

std::vector<float> EpisodicMemory::getActivationStatistics(const std::string& context) const {
    std::vector<float> statistics;
    
    auto it = pImpl->contextIndex.find(context);
    if (it == pImpl->contextIndex.end()) return statistics;
    
    std::vector<float> allValues;
    for (size_t index : it->second) {
        if (index < pImpl->episodes.size()) {
            for (float val : pImpl->episodes[index].values) {
                allValues.push_back(val);
            }
        }
    }
    
    if (allValues.empty()) return statistics;
    
    // Calculate basic statistics
    float sum = std::accumulate(allValues.begin(), allValues.end(), 0.0f);
    statistics.push_back(sum / allValues.size());  // Mean
    
    float variance = 0.0f;
    for (float val : allValues) {
        variance += (val - statistics[0]) * (val - statistics[0]);
    }
    variance /= allValues.size();
    statistics.push_back(std::sqrt(variance));  // Standard deviation
    
    statistics.push_back(*std::min_element(allValues.begin(), allValues.end()));  // Min
    statistics.push_back(*std::max_element(allValues.begin(), allValues.end()));  // Max
    
    return statistics;
}

void EpisodicMemory::rebuildContextIndex() {
    pImpl->contextIndex.clear();
    
    for (size_t i = 0; i < pImpl->episodes.size(); ++i) {
        const auto& episode = pImpl->episodes[i];
        if (!episode.context.empty()) {
            pImpl->contextIndex[episode.context].push_back(i);
        }
    }
}

void EpisodicMemory::setMaxEpisodes(size_t max) {
    pImpl->maxEpisodes = max;
    pruneOldEpisodes(max);
}

void EpisodicMemory::setConsolidationInterval(SimulationStep interval) {
    pImpl->consolidationInterval = interval;
}

void EpisodicMemory::setReplayFrequency(float frequency) {
    pImpl->replayFrequency = std::max(0.0f, std::min(frequency, 1.0f));
}

void EpisodicMemory::setSimilarityThreshold(float threshold) {
    pImpl->similarityThreshold = std::max(0.0f, std::min(threshold, 1.0f));
}

} // namespace nlm