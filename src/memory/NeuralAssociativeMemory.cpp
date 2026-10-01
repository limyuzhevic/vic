// NeuralAssociativeMemory Implementation
// This provides the neural associative memory system that was missing implementation

#include "src/memory/NeuralAssociativeMemory.hpp"
#include "src/brain/Brain.hpp"
#include <algorithm>
#include <random>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    // Neural patterns stored as connections to neuron groups
    std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons;
    
    // Associative connections between patterns
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations;
    
    // Pattern similarity matrix for efficient lookup
    // patternIndex -> (similarPatternIndex, similarityScore)
    std::vector<std::vector<std::pair<size_t, float>>> similarityMatrix;
    
    // Learning parameters
    float learningRate;
    float similarityThreshold;
    float maxAssociationsPerPattern;
    
    Impl() : learningRate(0.01f), similarityThreshold(0.7f), maxAssociationsPerPattern(5) {}
};

NeuralAssociativeMemory::NeuralAssociativeMemory() : pImpl(new Impl) {}

NeuralAssociativeMemory::~NeuralAssociativeMemory() = default;

void NeuralAssociativeMemory::initialize(Brain* brain) {
    brain_ = brain;
    // Initialize with empty state
}

void NeuralAssociativeMemory::associate(const std::vector<float>& patternA,
                                       const std::vector<float>& patternB,
                                       float strength) {
    // Find or create neuron representations for these patterns
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    // Remove existing association between these patterns
    associations.erase(
        std::remove_if(associations.begin(), associations.end(),
            [neuronA, neuronB](const auto& assoc) {
                auto& [a, b, s] = assoc;
                return (a == neuronA && b == neuronB) || (a == neuronB && b == neuronA);
            }),
        associations.end()
    );
    
    // Add new association
    associations.emplace_back(neuronA, neuronB, strength);
    
    // Update similarity matrix
    size_t idxA = getPatternIndex(neuronA);
    size_t idxB = getPatternIndex(neuronB);
    
    if (idxA < similarityMatrix.size() && idxB < similarityMatrix.size()) {
        float similarity = computeSimilarity(patternA, patternB);
        similarityMatrix[idxA].emplace_back(idxB, similarity);
        similarityMatrix[idxB].emplace_back(idxA, similarity);
    }
}

void NeuralAssociativeMemory::associateFromExperience(const EpisodicMemoryItem& episode) {
    // Convert episodic memory to neural pattern
    std::vector<float> patternA = extractNeuralPattern(episode);
    
    // Find similar existing episodes to create associations
    for (const auto& otherPattern : getExistingPatterns()) {
        float similarity = computeSimilarity(patternA, otherPattern);
        if (similarity > pImpl->similarityThreshold) {
            associate(patternA, otherPattern, similarity * pImpl->learningRate);
        }
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::retrieve(const std::vector<float>& queryPattern,
                                                                 size_t maxResults) const {
    std::vector<std::vector<float>> results;
    
    // Find patterns similar to the query
    std::vector<std::pair<size_t, float>> similarPatterns = findSimilarPatterns(queryPattern);
    
    // Sort by similarity
    std::sort(similarPatterns.begin(), similarPatterns.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Return top results
    for (size_t i = 0; i < std::min(maxResults, similarPatterns.size()); ++i) {
        size_t patternIdx = similarPatterns[i].first;
        results.push_back(getPatternByIndex(patternIdx));
    }
    
    return results;
}

float NeuralAssociativeMemory::getAssociationStrength(const std::vector<float>& patternA,
                                                      const std::vector<float>& patternB) const {
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    for (const auto& assoc : associations) {
        auto& [a, b, strength] = assoc;
        if ((a == neuronA && b == neuronB) || (a == neuronB && b == neuronA)) {
            return strength;
        }
    }
    
    return 0.0f;
}

void NeuralAssociativeMemory::updateAssociation(const std::vector<float>& patternA,
                                                const std::vector<float>& patternB,
                                                float delta) {
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    for (auto& assoc : associations) {
        auto& [a, b, strength] = assoc;
        if ((a == neuronA && b == neuronB) || (a == neuronB && b == neuronA)) {
            strength = std::clamp(strength + delta, 0.0f, 1.0f);
            return;
        }
    }
}

void NeuralAssociativeMemory::spreadActivation(const std::vector<float>& cuePattern,
                                                size_t steps) const {
    // This is a complex operation that would traverse the associative graph
    // For now, return patterns directly associated with the cue
    std::vector<std::vector<float>> directResults = retrieve(cuePattern, pImpl->maxAssociationsPerPattern);
    
    // In a full implementation, we would:
    // 1. Activate patterns similar to the cue
    // 2. For each activated pattern, activate its associated patterns
    // 3. Repeat for specified number of steps
    // 4. Return all patterns that became activated
    
    // For now, just return the direct associations
    (void)steps; // Unused parameter
}

void NeuralAssociativeMemory::clear() {
    pImpl->patternNeurons.clear();
    pImpl->associations.clear();
    pImpl->similarityMatrix.clear();
}

// Private helper methods
NeuronId NeuralAssociativeMemory::findPatternNeuron(const std::vector<float>& pattern) {
    // Look for existing neuron for this pattern
    for (const auto& existingPattern : pImpl->patternNeurons) {
        if (computeSimilarity(pattern, existingPattern.second) > pImpl->similarityThreshold) {
            return existingPattern.first;
        }
    }
    
    // Create new neuron for this pattern
    NeuronId newNeuronId = NeuronId(pImpl->patternNeurons.size() + 1);
    pImpl->patternNeurons.emplace_back(newNeuronId, pattern);
    
    // Initialize similarity matrix entry
    pImpl->similarityMatrix.resize(pImpl->patternNeurons.size());
    
    return newNeuronId;
}

size_t NeuralAssociativeMemory::getPatternIndex(NeuronId neuronId) const {
    for (size_t i = 0; i < pImpl->patternNeurons.size(); ++i) {
        if (pImpl->patternNeurons[i].first == neuronId) {
            return i;
        }
    }
    return SIZE_MAX;
}

std::vector<std::vector<float>> NeuralAssociativeMemory::getExistingPatterns() const {
    std::vector<std::vector<float>> patterns;
    for (const auto& patternNeuron : pImpl->patternNeurons) {
        patterns.push_back(patternNeuron.second);
    }
    return patterns;
}

std::vector<float> NeuralAssociativeMemory::getPatternByIndex(size_t index) const {
    if (index < pImpl->patternNeurons.size()) {
        return pImpl->patternNeurons[index].second;
    }
    return std::vector<float>();
}

std::vector<std::pair<size_t, float>> NeuralAssociativeMemory::findSimilarPatterns(const std::vector<float>& queryPattern) const {
    std::vector<std::pair<size_t, float>> results;
    
    for (size_t i = 0; i < pImpl->patternNeurons.size(); ++i) {
        float similarity = computeSimilarity(queryPattern, pImpl->patternNeurons[i].second);
        if (similarity > pImpl->similarityThreshold) {
            results.emplace_back(i, similarity);
        }
    }
    
    return results;
}

float NeuralAssociativeMemory::computeSimilarity(const std::vector<float>& a, const std::vector<float>& b) const {
    if (a.size() != b.size()) return 0.0f;
    
    float dotProduct = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    
    for (size_t i = 0; i < a.size(); ++i) {
        dotProduct += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA == 0.0f || normB == 0.0f) return 0.0f;
    
    return dotProduct / std::sqrt(normA * normB);
}

std::vector<float> NeuralAssociativeMemory::extractNeuralPattern(const EpisodicMemoryItem& episode) {
    // Convert episodic memory to neural pattern
    // This is a simplified version - in practice, this would be more sophisticated
    std::vector<float> pattern;
    
    // Include active neuron activations
    pattern.reserve(episode.activeNeurons.size());
    for (float activation : episode.neuronActivations) {
        pattern.push_back(activation);
    }
    
    // Include reward information
    pattern.push_back(episode.reward);
    
    // Include novelty
    pattern.push_back(episode.novelty);
    
    // Normalize pattern
    float norm = 0.0f;
    for (float val : pattern) {
        norm += val * val;
    }
    if (norm > 0.0f) {
        float scale = 1.0f / std::sqrt(norm);
        for (float& val : pattern) {
            val *= scale;
        }
    }
    
    return pattern;
}

} // namespace nlm