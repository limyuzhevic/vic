#include "NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <numeric>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    Brain* brain;
    size_t capacity;
    float learningRate;
    float consolidationThreshold;
    size_t nextId;
    float neuromodulationLevel;
    float predictionErrorWeight;
    
    // Temporary associations (short-term)
    std::vector<Association> associations;
    
    // Long-term associations
    std::vector<Association> longTermAssociations;
    
    // Pattern matching cache
    std::unordered_map<size_t, std::vector<size_t>> patternCache;
    
    Impl() : brain(nullptr), capacity(1000), learningRate(0.01f), 
             consolidationThreshold(0.5f), nextId(1), neuromodulationLevel(1.0f),
             predictionErrorWeight(0.3f) {}
};

NeuralAssociativeMemory::NeuralAssociativeMemory()
    : pImpl(std::make_unique<Impl>())
    , capacity_(1000)
    , learningRate_(0.01f)
    , consolidationThreshold_(0.5f)
    , nextId_(1)
    , brain_(nullptr)
    , neuromodulationLevel_(1.0f)
    , predictionErrorWeight_(0.3f)
{
}

NeuralAssociativeMemory::~NeuralAssociativeMemory() = default;

void NeuralAssociativeMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralAssociativeMemory initialized");
}

void NeuralAssociativeMemory::associate(const std::vector<float>& patternA,
                                       const std::vector<float>& patternB,
                                       float strength,
                                       bool bidirectional) {
    if (!brain_ || patternA.empty() || patternB.empty()) {
        return;
    }
    
    float similarity = computePatternSimilarity(patternA, patternB);
    
    // Create new association or update existing one
    bool found = false;
    for (auto& assoc : pImpl->associations) {
        if (computePatternSimilarity(patternA, assoc.patternA) > 0.95f &&
            computePatternSimilarity(patternB, assoc.patternB) > 0.95f) {
            // Update existing association
            assoc.strength = std::min(1.0f, assoc.strength + strength * learningRate_);
            assoc.lastAccessed = nextId_++;
            found = true;
            break;
        }
    }
    
    if (!found) {
        Association newAssoc(patternA, patternB, strength);
        newAssoc.lastAccessed = nextId_++;
        
        // Add to associations
        pImpl->associations.push_back(newAssoc);
        
        // Add bidirectional if requested
        if (bidirectional) {
            Association reverseAssoc(patternB, patternA, strength);
            reverseAssoc.lastAccessed = nextId_++;
            pImpl->associations.push_back(reverseAssoc);
        }
        
        // Cache for fast lookup
        size_t idA = std::hash<std::vector<float>>{}(patternA);
        size_t idB = std::hash<std::vector<float>>{}(patternB);
        pImpl->patternCache[idA].push_back(pImpl->associations.size() - 1);
        if (bidirectional) {
            pImpl->patternCache[idB].push_back(pImpl->associations.size() - 2);
        }
    }
}

std::vector<float> NeuralAssociativeMemory::retrieveAssociation(const std::vector<float>& pattern,
                                                                 float similarityThreshold) const {
    std::vector<size_t> matchingIndices = findMatchingAssociations(pattern, similarityThreshold);
    
    if (matchingIndices.empty()) {
        return std::vector<float>(); // No associations found
    }
    
    // Find best match
    size_t bestIndex = matchingIndices[0];
    float bestSimilarity = computePatternSimilarity(pattern, pImpl->associations[bestIndex].patternA);
    
    for (size_t idx : matchingIndices) {
        float similarity = computePatternSimilarity(pattern, pImpl->associations[idx].patternA);
        if (similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestIndex = idx;
        }
    }
    
    // Return associated pattern
    if (bestSimilarity > 0.5f) {
        return pImpl->associations[bestIndex].patternB;
    }
    
    return std::vector<float>();
}

std::vector<std::vector<float>> NeuralAssociativeMemory::getAssociations(const std::vector<float>& pattern) const {
    std::vector<std::vector<float>> result;
    
    // Use cache if available
    size_t patternHash = std::hash<std::vector<float>>{}(pattern);
    auto cacheIt = pImpl->patternCache.find(patternHash);
    if (cacheIt != pImpl->patternCache.end()) {
        for (size_t idx : cacheIt->second) {
            if (idx < pImpl->associations.size()) {
                result.push_back(pImpl->associations[idx].patternB);
            }
        }
        return result;
    }
    
    // Manual search
    for (const auto& assoc : pImpl->associations) {
        float similarity = computePatternSimilarity(pattern, assoc.patternA);
        if (similarity > 0.7f) {
            result.push_back(assoc.patternB);
        }
    }
    
    return result;
}

void NeuralAssociativeMemory::updateAssociation(const std::vector<float>& patternA,
                                                const std::vector<float>& patternB,
                                                float delta) {
    for (auto& assoc : pImpl->associations) {
        if ((computePatternSimilarity(patternA, assoc.patternA) > 0.95f &&
             computePatternSimilarity(patternB, assoc.patternB) > 0.95f) ||
            (computePatternSimilarity(patternB, assoc.patternA) > 0.95f &&
             computePatternSimilarity(patternA, assoc.patternB) > 0.95f)) {
            
            assoc.strength = std::clamp(assoc.strength + delta, 0.0f, 1.0f);
            if (std::abs(delta) > 0.1f) {
                assoc.lastAccessed = nextId_++;
            }
            break;
        }
    }
}

bool NeuralAssociativeMemory::hasAssociations(const std::vector<float>& pattern) const {
    return !getAssociations(pattern).empty();
}

float NeuralAssociativeMemory::getAssociationStrength(const std::vector<float>& patternA,
                                                      const std::vector<float>& patternB) const {
    for (const auto& assoc : pImpl->associations) {
        if ((computePatternSimilarity(patternA, assoc.patternA) > 0.95f &&
             computePatternSimilarity(patternB, assoc.patternB) > 0.95f) ||
            (computePatternSimilarity(patternB, assoc.patternA) > 0.95f &&
             computePatternSimilarity(patternA, assoc.patternB) > 0.95f)) {
            return assoc.strength;
        }
    }
    return 0.0f;
}

std::vector<float> NeuralAssociativeMemory::completePattern(const std::vector<float>& partialPattern,
                                                             float threshold) const {
    std::vector<size_t> matchingIndices = findMatchingAssociations(partialPattern, threshold);
    
    if (matchingIndices.empty()) {
        return partialPattern; // No associations, return input as-is
    }
    
    CompletionResult result;
    result.confidence = 0.0f;
    result.matchedAssociations = matchingIndices.size();
    
    // Combine all associated patterns
    std::vector<std::vector<float>> candidatePatterns;
    for (size_t idx : matchingIndices) {
        if (idx < pImpl->associations.size()) {
            candidatePatterns.push_back(pImpl->associations[idx].patternB);
        }
    }
    
    if (!candidatePatterns.empty()) {
        // Average all patterns to create completion
        size_t patternSize = candidatePatterns[0].size();
        std::vector<float> completedPattern(patternSize, 0.0f);
        
        for (const auto& pattern : candidatePatterns) {
            for (size_t i = 0; i < patternSize && i < pattern.size(); ++i) {
                completedPattern[i] += pattern[i];
            }
        }
        
        // Normalize
        float total = std::accumulate(completedPattern.begin(), completedPattern.end(), 0.0f);
        if (total > 0.0f) {
            for (size_t i = 0; i < completedPattern.size(); ++i) {
                completedPattern[i] /= total;
            }
        }
        
        result.completedPattern = completedPattern;
        result.confidence = static_cast<float>(matchingIndices.size()) / static_cast<float>(pImpl->associations.size());
    }
    
    return result.completedPattern;
}

void NeuralAssociativeMemory::consolidateAssociations(float threshold) {
    // Move strong associations to long-term storage
    std::vector<size_t> toConsolidate;
    
    for (size_t i = 0; i < pImpl->associations.size(); ++i) {
        if (pImpl->associations[i].strength > threshold && 
            !pImpl->associations[i].consolidated) {
            toConsolidate.push_back(i);
        }
    }
    
    consolidateToLongTerm(toConsolidate);
}

void NeuralAssociativeMemory::learnFromPredictionError(const std::vector<float>& predicted,
                                                       const std::vector<float>& actual,
                                                       float error) {
    if (predicted.empty() || actual.empty()) {
        return;
    }
    
    // Stronger learning with larger prediction errors
    float learningBoost = std::min(1.0f, std::abs(error) * predictionErrorWeight_);
    
    // Create association based on prediction error
    if (learningBoost > 0.1f) {
        // Use predicted pattern as one side, actual as the other
        associate(predicted, actual, learningBoost);
        
        // Also try reversed association
        associate(actual, predicted, learningBoost * 0.5f);
    }
}

void NeuralAssociativeMemory::applyNeuromodulation(const Neuromodulator& modulator) {
    neuromodulationLevel_ = modulator.getLevel();
    
    // Boost association strength based on neuromodulation
    boostAssociationStrength(neuromodulationLevel_);
    
    // Adjust learning rate
    learningRate_ *= (1.0f + neuromodulationLevel_ * 0.5f);
}

void NeuralAssociativeMemory::clear() {
    pImpl->associations.clear();
    pImpl->longTermAssociations.clear();
    pImpl->patternCache.clear();
    nextId_ = 1;
    neuromodulationLevel_ = 1.0f;
    learningRate_ = 0.01f;
}

float NeuralAssociativeMemory::getMemoryActivity() const {
    if (pImpl->associations.empty()) {
        return 0.0f;
    }
    
    float totalStrength = 0.0f;
    for (const auto& assoc : pImpl->associations) {
        totalStrength += assoc.strength;
    }
    
    return totalStrength / static_cast<float>(pImpl->associations.size());
}

void NeuralAssociativeMemory::updateFromNeuralActivity(const Brain* brain) {
    if (!brain) {
        return;
    }
    
    // Get current neural activity pattern
    std::vector<float> currentPattern = neuralActivityToPattern(brain);
    
    if (currentPattern.empty()) {
        return;
    }
    
    // Find patterns with similar neural activity
    std::vector<size_t> matchingIndices = findMatchingAssociations(currentPattern, 0.6f);
    
    // Decay weak associations
    decayWeakAssociations();
    
    // Consolidate associations that meet threshold
    if (needsConsolidation()) {
        consolidateAssociations(consolidationThreshold_);
    }
}

bool NeuralAssociativeMemory::needsConsolidation() const {
    float totalAssociationStrength = 0.0f;
    for (const auto& assoc : pImpl->associations) {
        totalAssociationStrength += assoc.strength;
    }
    
    float averageStrength = totalAssociationStrength / std::max(1.0f, static_cast<float>(pImpl->associations.size()));
    
    return averageStrength < consolidationThreshold_ && !pImpl->associations.empty();
}

float NeuralAssociativeMemory::computePatternSimilarity(const std::vector<float>& patternA,
                                                       const std::vector<float>& patternB) const {
    if (patternA.empty() || patternB.empty()) {
        return 0.0f;
    }
    
    size_t sizeA = patternA.size();
    size_t sizeB = patternB.size();
    size_t compareSize = std::min(sizeA, sizeB);
    
    if (compareSize == 0) {
        return 0.0f;
    }
    
    float totalDiff = 0.0f;
    for (size_t i = 0; i < compareSize; ++i) {
        float diff = std::abs(patternA[i] - patternB[i]);
        totalDiff += diff;
    }
    
    float avgDiff = totalDiff / static_cast<float>(compareSize);
    float similarity = std::max(0.0f, 1.0f - avgDiff);
    
    return similarity;
}

std::vector<size_t> NeuralAssociativeMemory::findMatchingAssociations(const std::vector<float>& pattern,
                                                                      float threshold) const {
    std::vector<size_t> result;
    
    // Use cache if available
    size_t patternHash = std::hash<std::vector<float>>{}(pattern);
    auto cacheIt = pImpl->patternCache.find(patternHash);
    if (cacheIt != pImpl->patternCache.end()) {
        for (size_t idx : cacheIt->second) {
            if (computePatternSimilarity(pattern, pImpl->associations[idx].patternA) >= threshold) {
                result.push_back(idx);
            }
        }
        return result;
    }
    
    // Manual search
    for (size_t i = 0; i < pImpl->associations.size(); ++i) {
        float similarity = computePatternSimilarity(pattern, pImpl->associations[i].patternA);
        if (similarity >= threshold) {
            result.push_back(i);
        }
    }
    
    return result;
}

void NeuralAssociativeMemory::createHebbianAssociation(const std::vector<float>& patternA,
                                                      const std::vector<float>& patternB,
                                                      float strength) {
    // Hebbian learning: strengthen connections between co-active neurons
    float similarity = computePatternSimilarity(patternA, patternB);
    
    if (similarity > 0.1f) {
        float hebbianStrength = strength * similarity * learningRate_ * neuromodulationLevel_;
        associate(patternA, patternB, hebbianStrength);
    }
}

void NeuralAssociativeMemory::consolidateToLongTerm(const std::vector<size_t>& indices) {
    for (size_t idx : indices) {
        if (idx < pImpl->associations.size()) {
            Association& assoc = pImpl->associations[idx];
            
            // Move to long-term storage
            pImpl->longTermAssociations.push_back(assoc);
            
            // Mark as consolidated
            assoc.consolidated = true;
            
            // Apply decay for memory efficiency
            assoc.decayFactor = 0.9f;
        }
    }
    
    // Clean up temporary cache
    pImpl->patternCache.clear();
}

void NeuralAssociativeMemory::decayWeakAssociations() {
    std::vector<size_t> toRemove;
    
    for (size_t i = 0; i < pImpl->associations.size(); ++i) {
        auto& assoc = pImpl->associations[i];
        
        // Decay strength over time
        assoc.strength *= assoc.decayFactor;
        
        // Remove very weak associations
        if (assoc.strength < 0.01f) {
            toRemove.push_back(i);
        }
    }
    
    // Remove weak associations (in reverse order)
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        size_t idx = *it;
        
        // Remove from cache
        for (auto& cacheEntry : pImpl->patternCache) {
            auto& indices = cacheEntry.second;
            indices.erase(std::remove(indices.begin(), indices.end(), idx), indices.end());
        }
        
        // Remove from associations
        pImpl->associations.erase(pImpl->associations.begin() + idx);
        
        // Adjust indices in cache
        for (auto& cacheEntry : pImpl->patternCache) {
            std::vector<size_t>& indices = cacheEntry.second;
            for (size_t& cacheIdx : indices) {
                if (cacheIdx > idx) {
                    cacheIdx--;
                }
            }
        }
    }
}

std::vector<float> NeuralAssociativeMemory::neuralActivityToPattern(const Brain* brain) const {
    if (!brain || !brain_) {
        return std::vector<float>();
    }
    
    // Get neural activity from sensory and internal neurons
    std::vector<float> pattern;
    
    // Sample neural activity from different populations
    auto regions = brain_->getRegions();
    if (regions.empty()) {
        return pattern;
    }
    
    // Collect activity levels from neurons
    for (const auto& region : regions) {
        auto neurons = region->getAllNeurons();
        for (auto* neuron : neurons) {
            if (neuron) {
                float activity = neuron->getMembranePotential();
                if (activity > 0.1f) {
                    pattern.push_back(activity);
                }
            }
        }
    }
    
    // Normalize pattern
    if (!pattern.empty()) {
        float sum = std::accumulate(pattern.begin(), pattern.end(), 0.0f);
        for (size_t i = 0; i < pattern.size(); ++i) {
            pattern[i] /= sum;
        }
    }
    
    return pattern;
}

void NeuralAssociativeMemory::boostAssociationStrength(float factor) {
    for (auto& assoc : pImpl->associations) {
        assoc.strength = std::min(1.0f, assoc.strength * (1.0f + factor * 0.5f));
        if (factor > 0.5f) {
            assoc.lastAccessed = nextId_++;
        }
    }
}

} // namespace nlm
