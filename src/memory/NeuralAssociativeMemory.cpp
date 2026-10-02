#include "NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

NeuralAssociativeMemory::NeuralAssociativeMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , hebbianRate_(0.01f)
    , strengthMin_(0.0f)
    , strengthMax_(1.0f)
{
}

NeuralAssociativeMemory::~NeuralAssociativeMemory() = default;

void NeuralAssociativeMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralAssociativeMemory initialized");
}

void NeuralAssociativeMemory::createAssociation(const std::vector<float>& patternA,
                                                const std::vector<float>& patternB,
                                                float initialStrength) {
    if (patternA.empty() || patternB.empty()) {
        return;
    }
    
    // Clamp initial strength to valid range
    float clampedStrength = std::clamp(initialStrength, strengthMin_, strengthMax_);
    
    // Check if pattern A already exists as a key
    auto& targetsA = associations_[patternA];
    
    // Check if pattern B already exists as a key
    auto& targetsB = associations_[patternB];
    
    // Create bidirectional association
    targetsA[patternB] = clampedStrength;
    targetsB[patternA] = clampedStrength;
    
    // Add patterns to patterns list if not already present
    auto addPatternIfNotExists = [this](const std::vector<float>& pattern) {
        if (std::find(patterns_.begin(), patterns_.end(), pattern) == patterns_.end()) {
            patterns_.push_back(pattern);
        }
    };
    
    addPatternIfNotExists(patternA);
    addPatternIfNotExists(patternB);
    
    NLM_LOG_DEBUG("Created association with strength " + std::to_string(clampedStrength));
}

std::vector<float> NeuralAssociativeMemory::retrieveAssociation(
    const std::vector<float>& pattern, float threshold) const {
    
    if (pattern.empty() || associations_.empty()) {
        return std::vector<float>();
    }
    
    // Find best matching pattern
    size_t bestMatchIdx = findBestMatch(pattern, patterns_, threshold);
    
    if (bestMatchIdx == SIZE_MAX) {
        return std::vector<float>();
    }
    
    const std::vector<float>& bestPattern = patterns_[bestMatchIdx];
    
    // Get all associations for this pattern
    auto it = associations_.find(bestPattern);
    if (it == associations_.end()) {
        return std::vector<float>();
    }
    
    // Return the strongest associated pattern
    float maxStrength = 0.0f;
    std::vector<float> strongestPattern;
    
    for (const auto& association : it->second) {
        if (association.second > maxStrength) {
            maxStrength = association.second;
            strongestPattern = association.first;
        }
    }
    
    return strongestPattern;
}

bool NeuralAssociativeMemory::hasAssociations(const std::vector<float>& pattern) const {
    auto it = associations_.find(pattern);
    return it != associations_.end() && !it->second.empty();
}

float NeuralAssociativeMemory::getAssociationStrength(
    const std::vector<float>& patternA,
    const std::vector<float>& patternB) const {
    
    auto itA = associations_.find(patternA);
    if (itA == associations_.end()) {
        return 0.0f;
    }
    
    auto itB = itA->second.find(patternB);
    if (itB == itA->second.end()) {
        return 0.0f;
    }
    
    return itB->second;
}

void NeuralAssociativeMemory::updateAssociation(const std::vector<float>& patternA,
                                                const std::vector<float>& patternB,
                                                float learningRate,
                                                bool shouldStrengthen) {
    if (patternA.empty() || patternB.empty()) {
        return;
    }
    
    // Get current strength
    float currentStrength = getAssociationStrength(patternA, patternB);
    
    if (shouldStrengthen) {
        // Hebbian learning: strengthen connection
        currentStrength += learningRate * (1.0f - currentStrength);
    } else {
        // Anti-Hebbian learning: weaken connection
        currentStrength -= learningRate * currentStrength;
    }
    
    // Clamp to valid range
    currentStrength = std::clamp(currentStrength, strengthMin_, strengthMax_);
    
    // Update both directions (bidirectional)
    associations_[patternA][patternB] = currentStrength;
    associations_[patternB][patternA] = currentStrength;
}

std::vector<std::vector<float>> NeuralAssociativeMemory::getAllPatterns() const {
    return patterns_;
}

void NeuralAssociativeMemory::clear() {
    associations_.clear();
    patterns_.clear();
    NLM_LOG_INFO("NeuralAssociativeMemory cleared");
}

size_t NeuralAssociativeMemory::getAssociationCount() const {
    size_t count = 0;
    for (const auto& patternMap : associations_) {
        count += patternMap.second.size();
    }
    // Divide by 2 to avoid double counting (bidirectional)
    return count / 2;
}

float NeuralAssociativeMemory::getAverageAssociationStrength() const {
    if (getAssociationCount() == 0) {
        return 0.0f;
    }
    
    float totalStrength = 0.0f;
    size_t totalAssociations = 0;
    
    for (const auto& patternMap : associations_) {
        for (const auto& association : patternMap.second) {
            totalStrength += association.second;
            totalAssociations++;
        }
    }
    
    return totalStrength / totalAssociations;
}

void NeuralAssociativeMemory::consolidate(float strengthenThreshold,
                                           float weakenThreshold) {
    strengthenThreshold = std::clamp(strengthenThreshold, 0.0f, 1.0f);
    weakenThreshold = std::clamp(weakenThreshold, 0.0f, 1.0f);
    
    for (auto& patternMap : associations_) {
        for (auto& association : patternMap.second) {
            if (association.second > strengthenThreshold) {
                // Strengthen strong associations
                association.second = std::min(strengthMax_,
                                             association.second * 1.1f);
            } else if (association.second < weakenThreshold) {
                // Weaken weak associations
                association.second = std::max(strengthMin_,
                                             association.second * 0.9f);
            }
        }
    }
}

std::vector<std::pair<std::vector<float>, float>> NeuralAssociativeMemory::getAssociations(
    const std::vector<float>& pattern,
    size_t maxResults) const {
    
    std::vector<std::pair<std::vector<float>, float>> results;
    
    auto it = associations_.find(pattern);
    if (it == associations_.end()) {
        return results;
    }
    
    for (const auto& association : it->second) {
        if (results.size() < maxResults) {
            results.push_back(association);
        } else {
            // Keep only the strongest associations
            auto weakestIt = std::min_element(results.begin(), results.end(),
                                             [](const auto& a, const auto& b) {
                                                 return a.second < b.second;
                                             });
            if (association.second > weakestIt->second) {
                *weakestIt = association;
            }
        }
    }
    
    // Sort by strength (strongest first)
    std::sort(results.begin(), results.end(),
              [](const auto& a, const auto& b) {
                  return a.second > b.second;
              });
    
    return results;
}

float NeuralAssociativeMemory::computePatternSimilarity(
    const std::vector<float>& a,
    const std::vector<float>& b) const {
    
    if (a.empty() || b.empty() || a.size() != b.size()) {
        return 0.0f;
    }
    
    float dotProduct = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    
    for (size_t i = 0; i < a.size(); ++i) {
        dotProduct += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA == 0.0f || normB == 0.0f) {
        return 0.0f;
    }
    
    return dotProduct / std::sqrt(normA * normB);
}

size_t NeuralAssociativeMemory::findBestMatch(
    const std::vector<float>& pattern,
    const std::vector<std::vector<float>>& candidates,
    float threshold) const {
    
    if (candidates.empty()) {
        return SIZE_MAX;
    }
    
    float bestSimilarity = 0.0f;
    size_t bestMatchIdx = SIZE_MAX;
    
    for (size_t i = 0; i < candidates.size(); ++i) {
        float similarity = computePatternSimilarity(pattern, candidates[i]);
        if (similarity > threshold && similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestMatchIdx = i;
        }
    }
    
    return bestMatchIdx;
}

} // namespace nlm
