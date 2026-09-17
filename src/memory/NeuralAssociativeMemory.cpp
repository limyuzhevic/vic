// NeuralAssociativeMemory implementation
// This implements Hebbian-based pattern associations between neural activity

#include "NeuralAssociativeMemory.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    Brain* brain_;
    std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons_;
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations_;
    float maxAssociationStrength_;
    float decayRate_;
    size_t maxPatterns_;
};

NeuralAssociativeMemory::NeuralAssociativeMemory()
    : pImpl(std::make_unique<Impl>()) {
    pImpl->maxAssociationStrength_ = 1.0f;
    pImpl->decayRate_ = 0.0001f;
    pImpl->maxPatterns_ = 1000;
}

NeuralAssociativeMemory::~NeuralAssociativeMemory() = default;

void NeuralAssociativeMemory::initialize(Brain* brain) {
    pImpl->brain_ = brain;
    NLM_LOG_INFO("NeuralAssociativeMemory initialized");
}

void NeuralAssociativeMemory::associate(const std::vector<float>& patternA,
                                       const std::vector<float>& patternB,
                                       float strength) {
    if (!pImpl->brain_ || patternA.empty() || patternB.empty()) return;
    
    // Find or create neurons representing these patterns
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA == INVALID_NEURON_ID || neuronB == INVALID_NEURON_ID) return;
    
    // Check if association already exists
    for (auto& assoc : pImpl->associations_) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            // Update existing association
            float& currentStrength = std::get<2>(assoc);
            currentStrength = std::min(currentStrength + strength, pImpl->maxAssociationStrength_);
            NLM_LOG_DEBUG("Updated association strength to " + std::to_string(currentStrength));
            return;
        }
    }
    
    // Create new association
    pImpl->associations_.emplace_back(neuronA, neuronB, strength);
    
    // Create bidirectional connection for Hebbian learning
    // This allows the patterns to activate each other
    if (auto* brain = pImpl->brain_) {
        // Find neurons by ID
        Neuron* neuronAPtr = nullptr;
        Neuron* neuronBPtr = nullptr;
        
        for (auto& region : brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron->getId() == neuronA) neuronAPtr = neuron;
                    if (neuron->getId() == neuronB) neuronBPtr = neuron;
                }
            }
        }
        
        if (neuronAPtr && neuronBPtr) {
            // Create forward and backward synapses with Hebbian learning enabled
            region->addSynapse(neuronAPtr->getId(), neuronBPtr->getId(), strength, 
                             SynapseType::Hebbian, true, true);
            region->addSynapse(neuronBPtr->getId(), neuronAPtr->getId(), strength,
                             SynapseType::Hebbian, true, true);
        }
    }
    
    NLM_LOG_DEBUG("Created new associative link with strength " + std::to_string(strength));
}

void NeuralAssociativeMemory::associateFromExperience(const EpisodicMemoryItem& episode) {
    if (episode.activeNeurons.size() >= 2) {
        // Extract activation patterns from neurons that were active
        std::vector<float> patternA, patternB;
        
        for (size_t i = 0; i < episode.activeNeurons.size() / 2; ++i) {
            patternA.push_back(episode.neuronActivations[i]);
            patternB.push_back(episode.neuronActivations[episode.activeNeurons.size() / 2 + i]);
        }
        
        if (!patternA.empty() && !patternB.empty()) {
            // Associate based on co-activation during the experience
            float associationStrength = episode.reward * 0.1f; // Scale by reward
            associate(patternA, patternB, associationStrength);
        }
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::retrieve(
    const std::vector<float>& queryPattern, size_t maxResults) const {
    std::vector<std::vector<float>> results;
    
    if (queryPattern.empty()) return results;
    
    std::vector<std::pair<std::vector<float>, float>> scoredPatterns;
    
    // For each stored pattern, compute similarity and strength
    for (const auto& assoc : pImpl->associations_) {
        NeuronId patternA = std::get<0>(assoc);
        NeuronId patternB = std::get<1>(assoc);
        float strength = std::get<2>(assoc);
        
        // Find patterns
        std::vector<float> pattern1, pattern2;
        bool foundA = false, foundB = false;
        
        for (const auto& stored : pImpl->patternNeurons_) {
            if (stored.first == patternA) {
                pattern1 = stored.second;
                foundA = true;
            }
            if (stored.first == patternB) {
                pattern2 = stored.second;
                foundB = true;
            }
        }
        
        // Compute similarity to query
        float similarityA = computeSimilarity(queryPattern, pattern1);
        float similarityB = computeSimilarity(queryPattern, pattern2);
        float avgSimilarity = (similarityA + similarityB) * 0.5f;
        
        if (avgSimilarity > 0.1f) { // Threshold for relevance
            float score = strength * avgSimilarity;
            scoredPatterns.emplace_back(pattern1, score);
            scoredPatterns.emplace_back(pattern2, score);
        }
    }
    
    // Sort by score and return top results
    std::sort(scoredPatterns.begin(), scoredPatterns.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    size_t count = std::min(maxResults, scoredPatterns.size());
    for (size_t i = 0; i < count; ++i) {
        results.push_back(scoredPatterns[i].first);
    }
    
    return results;
}

float NeuralAssociativeMemory::getAssociationStrength(
    const std::vector<float>& patternA,
    const std::vector<float>& patternB) const {
    if (patternA.empty() || patternB.empty()) return 0.0f;
    
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA == INVALID_NEURON_ID || neuronB == INVALID_NEURON_ID) return 0.0f;
    
    for (const auto& assoc : pImpl->associations_) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            return std::get<2>(assoc);
        }
    }
    
    return 0.0f;
}

void NeuralAssociativeMemory::updateAssociation(
    const std::vector<float>& patternA,
    const std::vector<float>& patternB,
    float delta) {
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA == INVALID_NEURON_ID || neuronB == INVALID_NEURON_ID) return;
    
    for (auto& assoc : pImpl->associations_) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            float& strength = std::get<2>(assoc);
            strength += delta;
            if (strength < 0.0f) strength = 0.0f;
            if (strength > pImpl->maxAssociationStrength_) strength = pImpl->maxAssociationStrength_;
            break;
        }
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::spreadActivation(
    const std::vector<float>& cuePattern, size_t steps) const {
    std::vector<std::vector<float>> results;
    
    if (cuePattern.empty() || steps == 0) return results;
    
    std::vector<std::vector<float>> currentLevel = retrieve(cuePattern, pImpl->patternNeurons_.size());
    results = currentLevel;
    
    // Multi-step spreading activation
    for (size_t step = 1; step < steps; ++step) {
        std::vector<std::vector<float>> nextLevel;
        
        for (const auto& pattern : currentLevel) {
            // Find associated patterns
            std::vector<float> patternNeuronId;
            bool found = false;
            
            for (const auto& stored : pImpl->patternNeurons_) {
                if (computeSimilarity(pattern, stored.second) > 0.8f) {
                    patternNeuronId = stored.second;
                    found = true;
                    break;
                }
            }
            
            if (found) {
                // Get associated patterns through direct associations
                for (const auto& assoc : pImpl->associations_) {
                    NeuronId patternA = std::get<0>(assoc);
                    NeuronId patternB = std::get<1>(assoc);
                    
                    // Check if this pattern matches either side of the association
                    for (const auto& stored : pImpl->patternNeurons_) {
                        if ((stored.first == patternA && computeSimilarity(pattern, stored.second) > 0.8f) ||
                            (stored.first == patternB && computeSimilarity(pattern, stored.second) > 0.8f)) {
                            // Find the other pattern in this association
                            NeuronId otherPattern = (stored.first == patternA) ? patternB : patternA;
                            
                            for (const auto& otherStored : pImpl->patternNeurons_) {
                                if (otherStored.first == otherPattern) {
                                    nextLevel.push_back(otherStored.second);
                                    results.push_back(otherStored.second);
                                }
                            }
                        }
                    }
                }
            }
        }
        
        // Deduplicate and limit results
        std::sort(nextLevel.begin(), nextLevel.end());
        nextLevel.erase(std::unique(nextLevel.begin(), nextLevel.end()), nextLevel.end());
        
        currentLevel = nextLevel;
        
        // Check for cycles
        if (currentLevel.empty()) break;
    }
    
    return results;
}

void NeuralAssociativeMemory::clear() {
    pImpl->associations_.clear();
    pImpl->patternNeurons_.clear();
    NLM_LOG_INFO("NeuralAssociativeMemory cleared");
}

NeuronId NeuralAssociativeMemory::findPatternNeuron(const std::vector<float>& pattern) {
    // Check if pattern already exists
    for (const auto& stored : pImpl->patternNeurons_) {
        if (computeSimilarity(pattern, stored.second) > 0.9f) {
            return stored.first;
        }
    }
    
    // Create new pattern neuron
    if (pImpl->patternNeurons_.size() >= pImpl->maxPatterns_) {
        // Remove oldest pattern if at capacity
        pImpl->patternNeurons_.erase(pImpl->patternNeurons_.begin());
    }
    
    // For now, create a neuron ID based on pattern hash
    // In a full implementation, this would create actual neurons in brain regions
    uint64_t hash = 0;
    for (float val : pattern) {
        hash = hash * 31 + static_cast<uint64_t>(val * 1000);
    }
    
    NeuronId newId;
    newId.value = hash % 1000000; // Keep within reasonable range
    
    pImpl->patternNeurons_.emplace_back(newId, pattern);
    NLM_LOG_DEBUG("Created new pattern neuron with ID " + std::to_string(newId.value));
    
    return newId;
}

float NeuralAssociativeMemory::computeSimilarity(const std::vector<float>& a,
                                                 const std::vector<float>& b) const {
    if (a.empty() || b.empty()) return 0.0f;
    
    size_t minSize = std::min(a.size(), b.size());
    if (minSize == 0) return 0.0f;
    
    float dotProduct = 0.0f;
    float normA = 0.0f, normB = 0.0f;
    
    for (size_t i = 0; i < minSize; ++i) {
        dotProduct += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA == 0.0f || normB == 0.0f) return 0.0f;
    
    return dotProduct / std::sqrt(normA * normB);
}

} // namespace nlm