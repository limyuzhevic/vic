#pragma once

#include "EnhancedWorkingMemory.hpp"
#include "../core/Types/Types.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_map>
#include <chrono>

namespace nlm {

class WorkingMemoryPatternMatcher {
public:
    WorkingMemoryPatternMatcher() : patternCacheSize_(100) {}
    
    ~WorkingMemoryPatternMatcher() {}
    
    // Compute pattern similarity using multiple metrics
    struct PatternSimilarity {
        float cosineSimilarity;
        float euclideanDistance;
        float correlationCoefficient;
        float combinedScore;
    };
    
    PatternSimilarity computeSimilarity(const std::vector<float>& patternA,
                                        const std::vector<float>& patternB) const {
        PatternSimilarity result;
        
        if (patternA.empty() || patternB.empty() || patternA.size() != patternB.size()) {
            return {0.0f, 0.0f, 0.0f, 0.0f};
        }
        
        // Cosine similarity
        float dotProduct = 0.0f;
        float normA = 0.0f;
        float normB = 0.0f;
        
        for (size_t i = 0; i < patternA.size(); ++i) {
            dotProduct += patternA[i] * patternB[i];
            normA += patternA[i] * patternA[i];
            normB += patternB[i] * patternB[i];
        }
        
        result.cosineSimilarity = (normA < 0.0001f || normB < 0.0001f) ? 0.0f : 
                                 dotProduct / (std::sqrt(normA) * std::sqrt(normB));
        
        // Euclidean distance (normalized)
        float euclideanSum = 0.0f;
        for (size_t i = 0; i < patternA.size(); ++i) {
            float diff = patternA[i] - patternB[i];
            euclideanSum += diff * diff;
        }
        result.euclideanDistance = std::sqrt(euclideanSum);
        
        // Correlation coefficient
        float meanA = std::accumulate(patternA.begin(), patternA.end(), 0.0f) / patternA.size();
        float meanB = std::accumulate(patternB.begin(), patternB.end(), 0.0f) / patternB.size();
        
        float cov = 0.0f;
        float varA = 0.0f;
        float varB = 0.0f;
        
        for (size_t i = 0; i < patternA.size(); ++i) {
            float devA = patternA[i] - meanA;
            float devB = patternB[i] - meanB;
            cov += devA * devB;
            varA += devA * devA;
            varB += devB * devB;
        }
        
        result.correlationCoefficient = (varA < 0.0001f || varB < 0.0001f) ? 0.0f : 
                                       cov / (std::sqrt(varA) * std::sqrt(varB));
        
        // Combined similarity score (weighted average)
        result.combinedScore = (result.cosineSimilarity * 0.4f + 
                               (1.0f - result.euclideanDistance / 10.0f) * 0.3f +
                               result.correlationCoefficient * 0.3f);
        
        return result;
    }
    
    // Find best matching patterns in a collection
    std::vector<std::pair<size_t, PatternSimilarity>> findBestMatches(
        const std::vector<float>& queryPattern,
        const std::vector<std::vector<float>>& patterns,
        size_t maxResults = 5,
        float minSimilarity = 0.3f) const {
        
        std::vector<std::pair<size_t, PatternSimilarity>> results;
        
        for (size_t i = 0; i < patterns.size(); ++i) {
            PatternSimilarity sim = computeSimilarity(queryPattern, patterns[i]);
            
            if (sim.combinedScore >= minSimilarity) {
                results.emplace_back(i, sim);
            }
        }
        
        // Sort by combined score (descending)
        std::sort(results.begin(), results.end(),
                  [](const auto& a, const auto& b) {
                      return a.second.combinedScore > b.second.combinedScore;
                  });
        
        // Limit results
        if (results.size() > maxResults) {
            results.resize(maxResults);
        }
        
        return results;
    }
    
    // Compute pattern affinity for memory consolidation
    float computePatternAffinity(const std::vector<float>& patternA,
                                 const std::vector<float>& patternB) const {
        PatternSimilarity sim = computeSimilarity(patternA, patternB);
        return sim.combinedScore;
    }
    
    // Find similar patterns in working memory (using neuron activations)
    std::vector<std::pair<NeuronId, PatternSimilarity>> findSimilarInWorkingMemory(
        const std::vector<float>& queryPattern,
        const std::unordered_map<uint64_t, float>& neuronActivations,
        const std::unordered_map<uint64_t, std::vector<float>>& neuronPatterns,
        size_t maxResults = 5) const {
        
        std::vector<std::pair<NeuronId, PatternSimilarity>> results;
        
        for (const auto& pair : neuronActivations) {
            NeuronId neuron(pair.first);
            
            // Find pattern associated with this neuron
            auto patternIt = neuronPatterns.find(pair.first);
            if (patternIt != neuronPatterns.end()) {
                PatternSimilarity sim = computeSimilarity(queryPattern, patternIt->second);
                
                if (sim.combinedScore > 0.1f) {  // Minimum threshold
                    results.emplace_back(neuron, sim);
                }
            }
        }
        
        // Sort by similarity
        std::sort(results.begin(), results.end(),
                  [](const auto& a, const auto& b) {
                      return a.second.combinedScore > b.second.combinedScore;
                  });
        
        if (results.size() > maxResults) {
            results.resize(maxResults);
        }
        
        return results;
    }
    
    // Compute pattern evolution over time (for replay strengthening)
    std::vector<float> computePatternEvolution(
        const std::vector<std::vector<float>>& historicalPatterns,
        size_t lookbackCount = 10) const {
        
        if (historicalPatterns.empty()) {
            return std::vector<float>();
        }
        
        size_t startIndex = historicalPatterns.size() > lookbackCount ? 
                           historicalPatterns.size() - lookbackCount : 0;
        
        size_t patternSize = historicalPatterns[0].size();
        std::vector<float> evolution(patternSize, 0.0f);
        
        for (size_t i = startIndex; i < historicalPatterns.size(); ++i) {
            for (size_t j = 0; j < patternSize; ++j) {
                evolution[j] += historicalPatterns[i][j];
            }
        }
        
        float factor = 1.0f / (historicalPatterns.size() - startIndex);
        for (size_t j = 0; j < patternSize; ++j) {
            evolution[j] *= factor;
        }
        
        return evolution;
    }
    
    // Pattern cache for frequently accessed patterns
    void cachePattern(const std::vector<float>& pattern) {
        patternCache_.push_back(pattern);
        
        // Maintain cache size
        if (patternCache_.size() > patternCacheSize_) {
            patternCache_.erase(patternCache_.begin());
        }
    }
    
    // Retrieve cached pattern if available
    bool getCachedPattern(const std::vector<float>& pattern, std::vector<float>& result) const {
        for (const auto& cached : patternCache_) {
            PatternSimilarity sim = computeSimilarity(pattern, cached);
            if (sim.combinedScore > 0.95f) {  // Near exact match
                result = cached;
                return true;
            }
        }
        return false;
    }
    
    // Get pattern cache statistics
    struct CacheStats {
        size_t cacheSize;
        size_t hits;
        size_t misses;
        float hitRate;
    };
    
    CacheStats getCacheStats() const {
        CacheStats stats;
        stats.cacheSize = patternCache_.size();
        stats.hits = cacheHits_;
        stats.misses = cacheMisses_;
        
        if (cacheHits_ + cacheMisses_ > 0) {
            stats.hitRate = static_cast<float>(cacheHits_) / (cacheHits_ + cacheMisses_);
        } else {
            stats.hitRate = 0.0f;
        }
        
        return stats;
    }
    
private:
    // Cache of recently accessed patterns
    std::vector<std::vector<float>> patternCache_;
    size_t patternCacheSize_;
    
    // Cache statistics
    size_t cacheHits_;
    size_t cacheMisses_;
};

} // namespace nlm