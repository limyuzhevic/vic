#include "NeuralAssociativeMemory.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    Brain* brain;
    std::vector<std::vector<float>> keys;
    std::vector<std::vector<float>> values;
    std::vector<float> strengths;
    std::vector<std::vector<float>> valuePatterns;  // Additional value patterns
    float minSimilarity;
    size_t maxAssociations;
    
    Impl() : brain(nullptr), minSimilarity(0.5f), maxAssociations(10000) {}
};

NeuralAssociativeMemory::NeuralAssociativeMemory() : pImpl(new Impl) {}

NeuralAssociativeMemory::~NeuralAssociativeMemory() = default;

void NeuralAssociativeMemory::initialize(Brain* brainRef) {
    pImpl->brain = brainRef;
    clear();
    
    NLM_LOG_DEBUG("NeuralAssociativeMemory initialized");
}

void NeuralAssociativeMemory::learnAssociation(const std::vector<float>& keyPattern,
                                                const std::vector<float>& valuePattern,
                                                float strength) {
    // Simple implementation: store associations with similarity matching
    if (keys.size() >= pImpl->maxAssociations) {
        // Remove weakest association
        size_t minIndex = 0;
        float minStrength = pImpl->strengths[0];
        for (size_t i = 1; i < pImpl->strengths.size(); ++i) {
            if (pImpl->strengths[i] < minStrength) {
                minStrength = pImpl->strengths[i];
                minIndex = i;
            }
        }
        keys.erase(keys.begin() + minIndex);
        values.erase(values.begin() + minIndex);
        pImpl->strengths.erase(pImpl->strengths.begin() + minIndex);
        pImpl->valuePatterns.erase(pImpl->valuePatterns.begin() + minIndex);
    }
    
    keys.push_back(keyPattern);
    values.push_back(valuePattern);
    pImpl->strengths.push_back(strength);
    pImpl->valuePatterns.push_back(valuePattern);
    
    NLM_LOG_DEBUG("NeuralAssociativeMemory learned association (total: " +
                 std::to_string(keys.size()) + ")");
}

std::vector<float> NeuralAssociativeMemory::retrieveValue(const std::vector<float>& keyPattern,
                                                           float threshold) const {
    if (keys.empty() || keyPattern.empty()) {
        return std::vector<float>();
    }
    
    std::vector<float> bestMatch;
    float bestSimilarity = 0.0f;
    
    for (size_t i = 0; i < keys.size(); ++i) {
        float similarity = computeSimilarity(keyPattern, keys[i]);
        
        if (similarity > bestSimilarity && similarity >= threshold) {
            bestSimilarity = similarity;
            bestMatch = values[i];
        }
    }
    
    if (bestMatch.empty() && !keys.empty()) {
        // Return default if no match above threshold
        bestMatch = values[0];
        bestSimilarity = computeSimilarity(keyPattern, keys[0]);
    }
    
    NLM_LOG_DEBUG("NeuralAssociativeMemory retrieved value with similarity: " +
                 std::to_string(bestSimilarity));
    
    return bestMatch;
}

void NeuralAssociativeMemory::clear() {
    keys.clear();
    values.clear();
    pImpl->strengths.clear();
    pImpl->valuePatterns.clear();
    NLM_LOG_DEBUG("NeuralAssociativeMemory cleared");
}

size_t NeuralAssociativeMemory::getAssociationCount() const {
    return keys.size();
}

void NeuralAssociativeMemory::reset() {
    clear();
}

float NeuralAssociativeMemory::Impl::computeSimilarity(const std::vector<float>& a,
                                                       const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) {
        return 0.0f;
    }
    
    // Cosine similarity
    float dot = std::inner_product(a.begin(), a.end(), b.begin(), 0.0f);
    float normA = std::sqrt(std::inner_product(a.begin(), a.end(), a.begin(), 0.0f));
    float normB = std::sqrt(std::inner_product(b.begin(), b.end(), b.begin(), 0.0f));
    
    if (normA == 0.0f || normB == 0.0f) {
        return 0.0f;
    }
    
    return dot / (normA * normB);
}

} // namespace nlm