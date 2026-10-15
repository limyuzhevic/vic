#include "SpikeCorrelationCache.hpp"

namespace nlm {

void SpikeCorrelationCache::storeSpikesInternal(SynapseId synapseId,
                                              const std::vector<Timestamp>& preSpikes,
                                              const std::vector<Timestamp>& postSpikes) {
    // Check if we already have an entry for this synapse
    auto it = synapseToIndex_.find(synapseId);
    if (it != synapseToIndex_.end()) {
        // Update existing entry
        size_t index = it->second;
        if (index < cache_.size()) {
            // Move to end (LRU-like behavior)
            cache_[index].entry = CacheEntry(preSpikes, postSpikes);
            cache_[index].entry.statsValid = false;
            cache_[index].accessCount.fetch_add(1, std::memory_order_relaxed);
        }
    } else {
        // Add new entry
        CacheEntry entry(preSpikes, postSpikes);
        cache_.emplace_back(synapseId, entry);
        synapseToIndex_[synapseId] = cache_.size() - 1;
        
        // Enforce size limit
        enforceSizeLimit();
    }
}

std::pair<std::vector<Timestamp>, std::vector<Timestamp>> 
SpikeCorrelationCache::getSpikesInternal(SynapseId synapseId) const {
    auto it = synapseToIndex_.find(synapseId);
    if (it == synapseToIndex_.end()) {
        return std::make_pair(std::vector<Timestamp>(), std::vector<Timestamp>());
    }
    
    size_t index = it->second;
    if (index >= cache_.size()) {
        return std::make_pair(std::vector<Timestamp>(), std::vector<Timestamp>());
    }
    
    // Update access count for LRU
    cache_[index].accessCount.fetch_add(1, std::memory_order_relaxed);
    
    return std::make_pair(cache_[index].entry.preSpikes, cache_[index].entry.postSpikes);
}

SpikeCorrelationCache::CorrelationStats 
SpikeCorrelationCache::computeStatsInternal(const CacheEntry& entry) const {
    CorrelationStats stats;
    
    if (entry.preSpikes.empty() || entry.postSpikes.empty()) {
        return stats;
    }
    
    stats.preSpikeCount = entry.preSpikes.size();
    stats.postSpikeCount = entry.postSpikes.size();
    stats.totalPairs = stats.preSpikeCount * stats.postSpikeCount;
    
    // Compute basic timing statistics
    float sumPre = 0.0f, sumPost = 0.0f;
    for (Timestamp t : entry.preSpikes) {
        sumPre += static_cast<float>(t);
    }
    for (Timestamp t : entry.postSpikes) {
        sumPost += static_cast<float>(t);
    }
    
    stats.meanPreTime = sumPre / stats.preSpikeCount;
    stats.meanPostTime = sumPost / stats.postSpikeCount;
    
    // Compute standard deviations
    float sumSqPre = 0.0f, sumSqPost = 0.0f;
    for (Timestamp t : entry.preSpikes) {
        float diff = static_cast<float>(t) - stats.meanPreTime;
        sumSqPre += diff * diff;
    }
    for (Timestamp t : entry.postSpikes) {
        float diff = static_cast<float>(t) - stats.meanPostTime;
        sumSqPost += diff * diff;
    }
    
    stats.stdDevPreTime = std::sqrt(sumSqPre / stats.preSpikeCount);
    stats.stdDevPostTime = std::sqrt(sumSqPost / stats.postSpikeCount);
    
    // Compute correlation coefficient
    float cov = 0.0f;
    for (size_t i = 0; i < entry.preSpikes.size(); ++i) {
        for (size_t j = 0; j < entry.postSpikes.size(); ++j) {
            float pre = static_cast<float>(entry.preSpikes[i]);
            float post = static_cast<float>(entry.postSpikes[j]);
            cov += (pre - stats.meanPreTime) * (post - stats.meanPostTime);
        }
    }
    cov /= static_cast<float>(stats.totalPairs);
    
    float stdDevPre = stats.stdDevPreTime;
    float stdDevPost = stats.stdDevPostTime;
    if (stdDevPre > 0.0f && stdDevPost > 0.0f) {
        stats.correlationCoefficient = cov / (stdDevPre * stdDevPost);
    }
    
    // Compute temporal distance and coincidence rate
    size_t correlated = 0;
    for (Timestamp pre : entry.preSpikes) {
        for (Timestamp post : entry.postSpikes) {
            float dt = static_cast<float>(post - pre);
            if (std::abs(dt) < 100.0f) {  // 100ms window
                correlated++;
            }
        }
    }
    
    stats.correlatedPairs = correlated;
    stats.coincidenceRate = static_cast<float>(correlated) / static_cast<float>(stats.totalPairs);
    
    // Compute average temporal distance
    float totalDistance = 0.0f;
    size_t count = 0;
    for (Timestamp pre : entry.preSpikes) {
        for (Timestamp post : entry.postSpikes) {
            float dt = static_cast<float>(post - pre);
            if (std::abs(dt) < 1000.0f) {  // Consider spikes within 1s
                totalDistance += std::abs(dt);
                ++count;
            }
        }
    }
    
    if (count > 0) {
        stats.temporalDistance = totalDistance / static_cast<float>(count);
    }
    
    return stats;
}

void SpikeCorrelationCache::enforceSizeLimit() {
    if (cache_.size() > maxCacheSize_) {
        // Remove least recently used entries
        std::vector<size_t> indices(cache_.size());
        std::iota(indices.begin(), indices.end(), 0);
        
        // Sort by access count (ascending)
        std::sort(indices.begin(), indices.end(),
                 [this](size_t a, size_t b) {
                     return cache_[a].accessCount.load(std::memory_order_relaxed) <
                            cache_[b].accessCount.load(std::memory_order_relaxed);
                 });
        
        // Remove oldest entries
        size_t removeCount = cache_.size() - maxCacheSize_;
        for (size_t i = 0; i < removeCount; ++i) {
            size_t index = indices[i];
            synapseToIndex_.erase(cache_[index].synapseId.load(std::memory_order_relaxed));
        }
        
        // Remove from vector
        cache_.erase(cache_.begin(), cache_.begin() + removeCount);
        
        // Update indices for remaining entries
        for (size_t i = 0; i < cache_.size(); ++i) {
            synapseToIndex_[cache_[i].synapseId.load(std::memory_order_relaxed)] = i;
        }
    }
}

} // namespace nlm
