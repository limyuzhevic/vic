#include "EnhancedAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <unordered_map>

namespace nlm {

struct EnhancedAssociativeMemory::Impl {
    Brain* brain;
    
    // Pattern similarity cache for performance
    mutable std::unordered_map<uint64_t, float> similarityCache;
    
    // Enhanced activation spreads for multi-step propagation
    struct ActivationSpread {
        std::vector<std::pair<NeuronId, float>> neurons;
        std::vector<float> similarityContributions;
    };
    
    Impl() : brain(nullptr) {
        similarityCache.reserve(10000);
    }
};

EnhancedAssociativeMemory::EnhancedAssociativeMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , similarityThreshold_(0.3f)
    , maxSimilaritySteps_(3)
    , similarityDecay_(0.8f)
    , totalRetrievals_(0)
    , accumulatedSimilarity_(0.0f)
    , fuzzyMatches_(0)
    , hardMatches_(0)
{
}

EnhancedAssociativeMemory::~EnhancedAssociativeMemory() = default;

void EnhancedAssociativeMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("EnhancedAssociativeMemory initialized with fuzzy matching");
}

void EnhancedAssociativeMemory::associate(const std::vector<float>& patternA,
                                         const std::vector<float>& patternB,
                                         float strength) {
    if (patternA.empty() || patternB.empty() || !brain_) return;
    
    // Normalize patterns for consistent similarity computation
    std::vector<float> normPatternA = normalizePattern(patternA);
    std::vector<float> normPatternB = normalizePattern(patternB);
    
    NeuronId neuronA = findPatternNeuronEnhanced(normPatternA);
    NeuronId neuronB = findPatternNeuronEnhanced(normPatternB);
    
    if (neuronA != INVALID_NEURON_ID && neuronB != INVALID_NEURON_ID) {
        // Check if association exists
        for (auto& assoc : associations_) {
            if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
                (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
                // Update strength with smoothing
                float currentStrength = std::get<2>(assoc);
                std::get<2>(assoc) = std::clamp(
                    currentStrength + strength * 0.1f, 
                    0.0f, 1.0f);
                return;
            }
        }
        
        // Create new association with enhanced strength
        float enhancedStrength = strength * (1.0f + computeEnhancedSimilarity(normPatternA, normPatternB));
        associations_.emplace_back(neuronA, neuronB, enhancedStrength);
    }
}

void EnhancedAssociativeMemory::associateFromExperience(const class EpisodicMemoryItem& episode,
                                                       float noveltyBonus) {
    if (!episode.sensoryState.empty() && !episode.resultingSensoryState.empty()) {
        // Normalize patterns for consistent similarity
        std::vector<float> normSensory = normalizePattern(episode.sensoryState);
        std::vector<float> normResult = normalizePattern(episode.resultingSensoryState);
        
        // Base association strength with reward influence
        float baseStrength = 0.8f + episode.reward * 0.2f;
        
        // Apply novelty bonus if provided
        if (noveltyBonus > 0.0f) {
            baseStrength *= (1.0f + noveltyBonus);
        }
        
        associate(normSensory, normResult, baseStrength);
    }
    
    // Associate action with resulting state
    if (!episode.sensoryState.empty()) {
        // Create action-specific pattern
        std::vector<float> actionPattern(episode.sensoryState.size(), 0.0f);
        actionPattern[0] = static_cast<float>(episode.action) / 10.0f;
        
        std::vector<float> normAction = normalizePattern(actionPattern);
        std::vector<float> normResult = normalizePattern(episode.resultingSensoryState);
        
        associate(normAction, normResult, 0.7f + episode.novelty * 0.3f);
    }
}

std::vector<std::pair<std::vector<float>, float>> EnhancedAssociativeMemory::retrieveFuzzy(
    const std::vector<float>& queryPattern,
    size_t maxResults) const {
    
    totalRetrievals_++;
    
    std::vector<std::pair<std::vector<float>, float>> results;
    std::vector<PatternMatch> patternMatches = findPatternMatches(queryPattern);
    
    // Sort by enhanced similarity score
    std::sort(patternMatches.begin(), patternMatches.end(),
              [](const PatternMatch& a, const PatternMatch& b) {
                  float scoreA = (a.cosineSimilarity + a.euclideanSimilarity + a.correlationSimilarity) / 3.0f;
                  float scoreB = (b.cosineSimilarity + b.euclideanSimilarity + b.correlationSimilarity) / 3.0f;
                  return scoreA > scoreB;
              });
    
    for (size_t i = 0; i < std::min(maxResults, patternMatches.size()); ++i) {
        const PatternMatch& match = patternMatches[i];
        
        // Calculate weighted similarity based on associations
        float weightedSimilarity = computeWeightedSimilarity(queryPattern, match, associations_);
        
        if (weightedSimilarity > similarityThreshold_) {
            results.emplace_back(match.pattern, weightedSimilarity);
            accumulatedSimilarity_ += weightedSimilarity;
            
            if (weightedSimilarity > 0.7f) {
                fuzzyMatches_++;
            } else {
                hardMatches_++;
            }
        }
    }
    
    return results;
}

std::vector<std::vector<float>> EnhancedAssociativeMemory::retrieve(
    const std::vector<float>& queryPattern,
    size_t maxResults) const {
    // Use fuzzy retrieval for backward compatibility with enhanced features
    auto fuzzyResults = retrieveFuzzy(queryPattern, maxResults);
    
    std::vector<std::vector<float>> results;
    for (const auto& result : fuzzyResults) {
        results.push_back(result.first);
    }
    
    return results;
}

float EnhancedAssociativeMemory::getAssociationStrength(const std::vector<float>& patternA,
                                                        const std::vector<float>& patternB) const {
    // Find patterns with fuzzy matching
    std::vector<PatternMatch> matchesA = findPatternMatches(patternA);
    std::vector<PatternMatch> matchesB = findPatternMatches(patternB);
    
    if (matchesA.empty() || matchesB.empty()) return 0.0f;
    
    // Find best matching pair
    float bestStrength = 0.0f;
    
    for (const auto& matchA : matchesA) {
        for (const auto& matchB : matchesB) {
            // Find association between these neurons
            for (const auto& assoc : associations_) {
                if (((std::get<0>(assoc) == matchA.neuronId && std::get<1>(assoc) == matchB.neuronId) ||
                     (std::get<0>(assoc) == matchB.neuronId && std::get<1>(assoc) == matchA.neuronId)) &&
                    std::get<2>(assoc) > bestStrength) {
                    bestStrength = std::get<2>(assoc);
                }
            }
        }
    }
    
    return bestStrength;
}

void EnhancedAssociativeMemory::updateAssociation(const std::vector<float>& patternA,
                                                  const std::vector<float>& patternB,
                                                  float delta,
                                                  float noveltyWeight) {
    std::vector<float> normPatternA = normalizePattern(patternA);
    std::vector<float> normPatternB = normalizePattern(patternB);
    
    NeuronId neuronA = findPatternNeuronEnhanced(normPatternA);
    NeuronId neuronB = findPatternNeuronEnhanced(normPatternB);
    
    // Apply novelty weighting to delta
    float weightedDelta = delta * (1.0f + noveltyWeight);
    
    updateAssociationWithNovelty(associations_, neuronA, neuronB, weightedDelta, noveltyWeight);
}

std::vector<std::pair<std::vector<float>, float>> EnhancedAssociativeMemory::spreadActivationEnhanced(
    const std::vector<float>& cuePattern,
    size_t steps,
    float decayFactor) const {
    
    std::vector<std::pair<std::vector<float>, float>> activated;
    std::set<NeuronId> visited;
    
    // Find initial pattern neuron with enhanced matching
    std::vector<PatternMatch> patternMatches = findPatternMatches(cuePattern);
    
    if (patternMatches.empty()) {
        return activated;
    }
    
    // Sort by similarity and pick best match
    std::sort(patternMatches.begin(), patternMatches.end(),
              [](const PatternMatch& a, const PatternMatch& b) {
                  float scoreA = (a.cosineSimilarity + a.euclideanSimilarity + a.correlationSimilarity) / 3.0f;
                  float scoreB = (b.cosineSimilarity + b.euclideanSimilarity + b.correlationSimilarity) / 3.0f;
                  return scoreA > scoreB;
              });
    
    NeuronId currentNeuron = patternMatches[0].neuronId;
    float currentSimilarity = (patternMatches[0].cosineSimilarity + patternMatches[0].euclideanSimilarity + patternMatches[0].correlationSimilarity) / 3.0f;
    
    activated.emplace_back(patternMatches[0].pattern, currentSimilarity);
    visited.insert(currentNeuron);
    
    // Enhanced spreading activation with similarity gradients
    for (size_t step = 0; step < steps && step < maxSimilaritySteps_; ++step) {
        std::vector<std::pair<NeuronId, float>> nextNeurons;
        
        // Find neurons associated with current neuron
        for (const auto& assoc : associations_) {
            NeuronId otherNeuron = INVALID_NEURON_ID;
            
            if (std::get<0>(assoc) == currentNeuron && visited.find(std::get<1>(assoc)) == visited.end()) {
                otherNeuron = std::get<1>(assoc);
            } else if (std::get<1>(assoc) == currentNeuron && visited.find(std::get<0>(assoc)) == visited.end()) {
                otherNeuron = std::get<0>(assoc);
            }
            
            if (otherNeuron != INVALID_NEURON_ID && std::get<2>(assoc) > 0.3f * decayFactor) {
                nextNeurons.emplace_back(otherNeuron, std::get<2>(assoc) * decayFactor);
                visited.insert(otherNeuron);
            }
        }
        
        if (nextNeurons.empty()) break;
        
        // Pick neuron with highest association strength
        auto bestIt = std::max_element(nextNeurons.begin(), nextNeurons.end(),
                                      [](const auto& a, const auto& b) { return a.second < b.second; });
        
        currentNeuron = bestIt->first;
        currentSimilarity = bestIt->second;
        
        // Find pattern for this neuron
        for (const auto& pn : patternNeurons_) {
            if (pn.first == currentNeuron) {
                activated.emplace_back(pn.second, currentSimilarity);
                break;
            }
        }
    }
    
    return activated;
}

const std::vector<std::tuple<NeuronId, NeuronId, float>>& EnhancedAssociativeMemory::getAllAssociations() const {
    return associations_;
}

const std::vector<std::pair<NeuronId, std::vector<float>>>& EnhancedAssociativeMemory::getPatternNeurons() const {
    return patternNeurons_;
}

void EnhancedAssociativeMemory::clear() {
    patternNeurons_.clear();
    associations_.clear();
    
    // Clear cache
    pImpl->similarityCache.clear();
    
    // Reset statistics
    totalRetrievals_ = 0;
    accumulatedSimilarity_ = 0.0f;
    fuzzyMatches_ = 0;
    hardMatches_ = 0;
}

EnhancedAssociativeMemory::FuzzyMatchingStats EnhancedAssociativeMemory::getFuzzyStats() const {
    FuzzyMatchingStats stats;
    stats.totalRetrievals = totalRetrievals_;
    stats.fuzzyMatches = fuzzyMatches_;
    stats.hardMatches = hardMatches_;
    
    if (totalRetrievals_ > 0) {
        stats.averageSimilarityScore = accumulatedSimilarity_ / totalRetrievals_;
    } else {
        stats.averageSimilarityScore = 0.0f;
    }
    
    stats.thresholdUsed = similarityThreshold_;
    
    return stats;
}

float EnhancedAssociativeMemory::computeEnhancedSimilarity(const std::vector<float>& a,
                                                           const std::vector<float>& b) const {
    // Use cache for performance
    size_t hash = 0;
    for (size_t i = 0; i < std::min(a.size(), b.size()); ++i) {
        hash ^= std::hash<float>{}(a[i]) + std::hash<float>{}(b[i]) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
    
    auto cacheIt = pImpl->similarityCache.find(hash);
    if (cacheIt != pImpl->similarityCache.end()) {
        return cacheIt->second;
    }
    
    SimilarityMetrics metrics = computeSimilarityMetrics(a, b);
    
    // Weighted combination of similarity measures
    float enhancedSimilarity = 
        metrics.cosineSimilarity * 0.5f +
        (1.0f - metrics.euclideanDistance) * 0.3f +
        metrics.correlationSimilarity * 0.2f;
    
    // Cache result
    pImpl->similarityCache[hash] = enhancedSimilarity;
    
    return enhancedSimilarity;
}

std::vector<float> EnhancedAssociativeMemory::normalizePattern(const std::vector<float>& pattern) const {
    std::vector<float> normalized = pattern;
    
    // Compute L2 norm
    float norm = 0.0f;
    for (float value : normalized) {
        norm += value * value;
    }
    norm = std::sqrt(norm);
    
    if (norm > 0.0001f) {
        for (float& value : normalized) {
            value /= norm;
        }
    }
    
    return normalized;
}

EnhancedAssociativeMemory::SimilarityMetrics EnhancedAssociativeMemory::computeSimilarityMetrics(const std::vector<float>& a,
                                                                                                const std::vector<float>& b) const {
    SimilarityMetrics metrics;
    
    size_t minSize = std::min(a.size(), b.size());
    if (minSize == 0) {
        return metrics;
    }
    
    // Cosine similarity
    float dotProduct = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    
    for (size_t i = 0; i < minSize; ++i) {
        dotProduct += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    metrics.cosineSimilarity = dotProduct / (std::sqrt(normA) * std::sqrt(normB) + 0.0001f);
    
    // Euclidean distance (converted to similarity)
    float euclideanDist = 0.0f;
    for (size_t i = 0; i < minSize; ++i) {
        float diff = a[i] - b[i];
        euclideanDist += diff * diff;
    }
    euclideanDist = std::sqrt(euclideanDist);
    metrics.euclideanSimilarity = std::max(0.0f, 1.0f - euclideanDist / std::sqrt(minSize));
    
    // Correlation similarity
    float meanA = std::accumulate(a.begin(), a.begin() + minSize, 0.0f) / minSize;
    float meanB = std::accumulate(b.begin(), b.begin() + minSize, 0.0f) / minSize;
    
    float cov = 0.0f;
    float varA = 0.0f;
    float varB = 0.0f;
    
    for (size_t i = 0; i < minSize; ++i) {
        float aDev = a[i] - meanA;
        float bDev = b[i] - meanB;
        cov += aDev * bDev;
        varA += aDev * aDev;
        varB += bDev * bDev;
    }
    
    float stdA = std::sqrt(varA + 0.0001f);
    float stdB = std::sqrt(varB + 0.0001f);
    metrics.correlationSimilarity = cov / (stdA * stdB + 0.0001f);
    
    return metrics;
}

NeuronId EnhancedAssociativeMemory::findPatternNeuronEnhanced(const std::vector<float>& pattern) {
    // Check cache first
    uint64_t hash = 0;
    for (float value : pattern) {
        hash ^= std::hash<float>{}(value) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
    
    auto cacheIt = pImpl->similarityCache.find(hash);
    if (cacheIt != pImpl->similarityCache.end()) {
        // Return a representative neuron from existing patterns
        for (const auto& pn : patternNeurons_) {
            if (computeEnhancedSimilarity(pattern, pn.second) > 0.9f) {
                return pn.first;
            }
        }
    }
    
    // Check if we already have a similar pattern
    float bestSimilarity = 0.0f;
    NeuronId bestNeuron = INVALID_NEURON_ID;
    
    for (const auto& pn : patternNeurons_) {
        float similarity = computeEnhancedSimilarity(pattern, pn.second);
        if (similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestNeuron = pn.first;
        }
    }
    
    // Return existing neuron if similar enough
    if (bestNeuron != INVALID_NEURON_ID && bestSimilarity > 0.8f) {
        return bestNeuron;
    }
    
    // Create new pattern neuron
    NeuronId newId(patternNeurons_.size() + 30000);
    patternNeurons_.push_back({newId, pattern});
    return newId;
}

float EnhancedAssociativeMemory::computeWeightedSimilarity(
    const std::vector<float>& query,
    const std::pair<NeuronId, std::vector<float>>& pattern,
    const std::vector<std::tuple<NeuronId, NeuronId, float>>& associations) const {
    
    float baseSimilarity = computeEnhancedSimilarity(query, pattern.second);
    
    if (baseSimilarity < similarityThreshold_) {
        return 0.0f;
    }
    
    // Find all associations involving this pattern neuron
    float totalWeight = 0.0f;
    float weightedSum = 0.0f;
    
    for (const auto& assoc : associations) {
        if (std::get<0>(assoc) == pattern.first || std::get<1>(assoc) == pattern.first) {
            float strength = std::get<2>(assoc);
            totalWeight += strength;
            weightedSum += strength * baseSimilarity;
        }
    }
    
    if (totalWeight > 0.0f) {
        return weightedSum / totalWeight;
    }
    
    return baseSimilarity;
}

std::vector<EnhancedAssociativeMemory::PatternMatch> EnhancedAssociativeMemory::findPatternMatches(
    const std::vector<float>& query) const {
    
    std::vector<PatternMatch> matches;
    
    for (const auto& pn : patternNeurons_) {
        PatternMatch match;
        match.neuronId = pn.first;
        match.pattern = pn.second;
        
        // Compute all similarity metrics
        SimilarityMetrics metrics = computeSimilarityMetrics(query, pn.second);
        match.cosineSimilarity = metrics.cosineSimilarity;
        match.euclideanSimilarity = metrics.euclideanSimilarity;
        match.correlationSimilarity = metrics.correlationSimilarity;
        
        matches.push_back(match);
    }
    
    return matches;
}

void EnhancedAssociativeMemory::updateAssociationWithNovelty(
    const std::vector<std::tuple<NeuronId, NeuronId, float>>& associations,
    NeuronId neuronA, NeuronId neuronB,
    float delta, float noveltyWeight) {
    
    for (auto& assoc : associations) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            float currentStrength = std::get<2>(assoc);
            float updatedStrength = std::clamp(
                currentStrength + delta * (1.0f + noveltyWeight),
                0.0f, 1.0f);
            std::get<2>(assoc) = updatedStrength;
            return;
        }
    }
}

} // namespace nlm
