# NeuralSemanticMemory.cpp

#include "NeuralSemanticMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace nlm {

struct NeuralSemanticMemory::Impl {
    // Neural patterns for facts
    std::vector<std::vector<float>> factPatterns;
    std::vector<std::string> factKeys;
    
    // Pattern activation levels
    std::vector<float> patternActivations;
    
    // Similarity search structure
    std::unordered_map<std::string, size_t> keyToIndex;
    
    // Consolidation threshold
    float consolidationThreshold;
    
    // Development stage effect
    float developmentalModulation;
};

NeuralSemanticMemory::NeuralSemanticMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , consolidationThreshold(0.5f)
    , developmentalModulation(1.0f)
{
    pImpl->consolidationThreshold = 0.5f;
    pImpl->developmentalModulation = 1.0f;
}

NeuralSemanticMemory::~NeuralSemanticMemory() = default;

void NeuralSemanticMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralSemanticMemory initialized");
}

void NeuralSemanticMemory::storeFact(const std::string& key, const std::string& value, const std::vector<float>& pattern) {
    if (!brain_) return;
    
    // Check if fact already exists
    auto it = pImpl->keyToIndex.find(key);
    if (it != pImpl->keyToIndex.end()) {
        size_t index = it->second;
        pImpl->factPatterns[index] = pattern;
        pImpl->factKeys[index] = key;
        pImpl->patternActivations[index] = 1.0f;
        return;
    }
    
    // New fact - add it
    pImpl->keyToIndex[key] = pImpl->factPatterns.size();
    pImpl->factPatterns.push_back(pattern);
    pImpl->factKeys.push_back(key);
    pImpl->patternActivations.push_back(1.0f);
    
    // Neural consolidation: reinforce related patterns
    consolidateWithRelatedPatterns(pImpl->factPatterns.size() - 1);
}

std::string NeuralSemanticMemory::retrieveFact(const std::string& key) const {
    auto it = pImpl->keyToIndex.find(key);
    if (it == pImpl->keyToIndex.end()) {
        return "";
    }
    return pImpl->factKeys[it->second];
}

bool NeuralSemanticMemory::hasFact(const std::string& key) const {
    return pImpl->keyToIndex.find(key) != pImpl->keyToIndex.end();
}

std::vector<std::pair<std::string, std::string>> NeuralSemanticMemory::getAllFacts() const {
    std::vector<std::pair<std::string, std::string>> result;
    for (size_t i = 0; i < pImpl->factPatterns.size(); ++i) {
        result.emplace_back(pImpl->factKeys[i], "");
    }
    return result;
}

void NeuralSemanticMemory::clear() {
    pImpl->factPatterns.clear();
    pImpl->factKeys.clear();
    pImpl->patternActivations.clear();
    pImpl->keyToIndex.clear();
}

void NeuralSemanticMemory::update(const std::vector<std::vector<float>>& patterns, float learningRate) {
    if (patterns.empty() || !brain_) return;
    
    // Find most similar existing fact
    size_t bestMatch = findMostSimilarPattern(patterns);
    
    if (bestMatch != SIZE_MAX) {
        // Update existing fact with new pattern
        pImpl->factPatterns[bestMatch] = patterns[0];
        pImpl->factKeys[bestMatch] = "updated_fact_" + std::to_string(bestMatch);
        pImpl->patternActivations[bestMatch] = std::min(1.0f, pImpl->patternActivations[bestMatch] + learningRate);
    }
}

void NeuralSemanticMemory::decay(float decayRate) {
    for (auto& activation : pImpl->patternActivations) {
        activation *= (1.0f - decayRate * developmentalModulation);
    }
}

void NeuralSemanticMemory::consolidateWithRelatedPatterns(size_t newIndex) {
    // Find semantically similar patterns
    std::vector<size_t> similarIndices;
    for (size_t i = 0; i < pImpl->factPatterns.size(); ++i) {
        if (i == newIndex) continue;
        
        float similarity = calculatePatternSimilarity(pImpl->factPatterns[i], pImpl->factPatterns[newIndex]);
        if (similarity > 0.7f) {  // High similarity threshold
            similarIndices.push_back(i);
        }
    }
    
    // Strengthen connections to similar patterns
    for (size_t similarIndex : similarIndices) {
        pImpl->patternActivations[similarIndex] = std::min(1.0f, pImpl->patternActivations[similarIndex] * 1.1f);
    }
}

size_t NeuralSemanticMemory::findMostSimilarPattern(const std::vector<std::vector<float>>& patterns) const {
    if (pImpl->factPatterns.empty() || patterns.empty()) {
        return SIZE_MAX;
    }
    
    float bestSimilarity = 0.0f;
    size_t bestIndex = SIZE_MAX;
    
    for (size_t i = 0; i < pImpl->factPatterns.size(); ++i) {
        float similarity = calculatePatternSimilarity(pImpl->factPatterns[i], patterns[0]);
        if (similarity > bestSimilarity && similarity > 0.3f) {  // Minimum similarity threshold
            bestSimilarity = similarity;
            bestIndex = i;
        }
    }
    
    return bestIndex;
}

float NeuralSemanticMemory::calculatePatternSimilarity(const std::vector<float>& pattern1, const std::vector<float>& pattern2) const {
    if (pattern1.size() != pattern2.size()) return 0.0f;
    
    float dotProduct = 0.0f;
    float norm1 = 0.0f;
    float norm2 = 0.0f;
    
    for (size_t i = 0; i < pattern1.size(); ++i) {
        dotProduct += pattern1[i] * pattern2[i];
        norm1 += pattern1[i] * pattern1[i];
        norm2 += pattern2[i] * pattern2[i];
    }
    
    if (norm1 == 0.0f || norm2 == 0.0f) return 0.0f;
    
    return dotProduct / std::sqrt(norm1 * norm2);
}

void NeuralSemanticMemory::setConsolidationThreshold(float threshold) {
    consolidationThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

void NeuralSemanticMemory::setDevelopmentalModulation(float modulation) {
    developmentalModulation = std::clamp(modulation, 0.0f, 2.0f);
}

float NeuralSemanticMemory::getConsolidationThreshold() const {
    return consolidationThreshold;
}

float NeuralSemanticMemory::getDevelopmentalModulation() const {
    return developmentalModulation;
}

void NeuralSemanticMemory::storeFactFromNeuron(NeuronId neuron, const std::string& key) {
    if (!brain_) return;
    
    // Get neuron activation pattern
    std::vector<float> pattern;
    for (auto* region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuronPtr : pop->getNeurons()) {
                if (neuronPtr->getId() == neuron) {
                    // Create simple pattern based on neuron properties
                    pattern.push_back(neuronPtr->getState().membranePotential);
                    pattern.push_back(neuronPtr->getState().firingRate);
                    break;
                }
            }
        }
    }
    
    if (!pattern.empty()) {
        storeFact(key, "neural_fact", pattern);
    }
}

std::vector<float> NeuralSemanticMemory::activateSimilarPatterns(const std::vector<float>& pattern) const {
    std::vector<float> activatedPatterns;
    
    for (const auto& existingPattern : pImpl->factPatterns) {
        float similarity = calculatePatternSimilarity(existingPattern, pattern);
        if (similarity > 0.5f) {  // Medium similarity threshold
            activatedPatterns.push_back(similarity);
        }
    }
    
    return activatedPatterns;
}

} // namespace nlm
