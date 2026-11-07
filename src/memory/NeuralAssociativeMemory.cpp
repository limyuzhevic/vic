#include "NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <random>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    class Brain* brain;
    size_t capacity;
    std::vector<std::pair<std::vector<float>, std::vector<float>>> patterns1;
    std::vector<std::pair<std::vector<float>, std::vector<float>>> patterns2;
    std::vector<float> weights;
    std::vector<std::vector<float>> activations;
    float learningRate;
    
    Impl() : brain(nullptr), capacity(1000), learningRate(0.01f) {}
};

NeuralAssociativeMemory::NeuralAssociativeMemory() : pImpl(std::make_unique<Impl>()) {}

NeuralAssociativeMemory::~NeuralAssociativeMemory() = default;

bool NeuralAssociativeMemory::initialize(class Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize NeuralAssociativeMemory with null brain");
        return false;
    }
    
    pImpl->brain = brain;
    pImpl->patterns1.clear();
    pImpl->patterns2.clear();
    pImpl->weights.clear();
    pImpl->activations.clear();
    
    NLM_LOG_INFO("NeuralAssociativeMemory initialized");
    return true;
}

void NeuralAssociativeMemory::storeAssociation(const std::vector<float>& pattern1,
                                               const std::vector<float>& pattern2,
                                               float weight) {
    pImpl->patterns1.push_back(pattern1);
    pImpl->patterns2.push_back(pattern2);
    pImpl->weights.push_back(weight);
    pImpl->activations.push_back(std::vector<float>(pattern1.size(), 0.0f));
    
    NLM_LOG_DEBUG("Stored association with weight " + std::to_string(weight));
}

std::vector<std::pair<std::vector<float>, float>> NeuralAssociativeMemory::retrieveAssociations(
    const std::vector<float>& pattern,
    int maxResults) {
    
    std::vector<std::pair<std::vector<float>, float>> results;
    
    if (pImpl->patterns1.empty()) return results;
    
    // Calculate similarities
    std::vector<std::pair<size_t, float>> similarities;
    for (size_t i = 0; i < pImpl->patterns1.size(); ++i) {
        float similarity = 0.0f;
        size_t minSize = std::min(pattern.size(), pImpl->patterns1[i].size());
        for (size_t j = 0; j < minSize; ++j) {
            similarity += pattern[j] * pImpl->patterns1[i][j];
        }
        similarities.push_back(std::make_pair(i, similarity));
    }
    
    // Sort by similarity (descending)
    std::sort(similarities.begin(), similarities.end(),
              [](const std::pair<size_t, float>& a, const std::pair<size_t, float>& b) {
                  return a.second > b.second;
              });
    
    // Return top results
    size_t count = std::min(static_cast<size_t>(maxResults), similarities.size());
    for (size_t i = 0; i < count; ++i) {
        size_t idx = similarities[i].first;
        results.push_back(std::make_pair(pImpl->patterns2[idx], pImpl->weights[idx]));
    }
    
    return results;
}

void NeuralAssociativeMemory::clear() {
    pImpl->patterns1.clear();
    pImpl->patterns2.clear();
    pImpl->weights.clear();
    pImpl->activations.clear();
    NLM_LOG_INFO("NeuralAssociativeMemory cleared");
}

size_t NeuralAssociativeMemory::getAssociationCount() const {
    return pImpl->patterns1.size();
}

void NeuralAssociativeMemory::update(TimestepDuration dt) {
    // Update activation traces
    for (auto& activation : pImpl->activations) {
        for (auto& act : activation) {
            act *= 0.99f;  // Decay
        }
    }
}

size_t NeuralAssociativeMemory::getCapacity() const {
    return pImpl->capacity;
}

void NeuralAssociativeMemory::setCapacity(size_t capacity) {
    pImpl->capacity = capacity;
    NLM_LOG_INFO("NeuralAssociativeMemory capacity set to " + std::to_string(capacity));
}

} // namespace nlm