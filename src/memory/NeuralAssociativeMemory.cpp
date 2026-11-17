#include "NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Neuron.hpp"
#include <algorithm>
#include <cmath>
#include <iomanip>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    float associationStrength;     // Base association strength
    float decayRate;                // How quickly associations weaken
    float maxAssociations;         // Maximum number of associations
    float minSimilarity;           // Minimum similarity to create association
    
    // Statistics
    size_t totalAssociationsCreated;
    size_t totalAssociationsUpdated;
    
    Impl() : associationStrength(1.0f), decayRate(0.05f), maxAssociations(100),
             minSimilarity(0.5f),
             totalAssociationsCreated(0), totalAssociationsUpdated( beliebt(0) {}
};

NeuralAssociativeMemory::NeuralAssociativeMemory() : pImpl(new Impl) {}

NeuralAssociativeMemory::~NeuralAssociativeMemory() = default;

void NeuralAssociativeMemory::initialize(Brain* brain) {
    brain_ = brain;
    NLM_LOG_INFO("NeuralAssociativeMemory initialized with brain reference");
}

void NeuralAssociativeMemory::associate(const std::vector<float>& patternA,
                                       const std::vector<float>& patternB,
                                       float strength) {
    if (!brain_ || patternA.empty() || patternB.empty()) {
        return;
    }
    
    // Find or create neurons for patterns
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA != INVALID_NEURON_ID && neuronB != INVALID_NEURON_ID) {
        // Create association
        associate(neuronA, neuronB, strength);
    }
}

void NeuralAssociativeMemory::associateFromExperience(const EpisodicMemoryItem& episode) {
    if (!brain_ || episode.activeNeurons.size() < 2) {
        return;
    }
    
    // Create associations between all pairs of active neurons in the episode
    for (size_t i = 0; i < episode.activeNeurons.size(); ++i) {
        for (size_t j = i + 1; j < episode.activeNeurons.size(); ++j) {
            NeuronId neuronA = episode.activeNeurons[i];
            NeuronId neuronB = episode.activeNeurons[j];
            
            // Association strength based on activation correlation
            float strength = episode.neuronActivations[i] * episode.neuronActivations[j];
            associate(neuronA, neuronB, strength);
        }
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::retrieve(
    const std::vector<float>& queryPattern, size_t maxResults) const {
    std::vector<std::vector<float>> results;
    
    if (queryPattern.empty() || !brain_) {
        return results;
    }
    
    // Find similar patterns
    std::vector<std::pair<float, std::vector<float>>> scoredPatterns;
    
    for (const auto& patternNeuron : patternNeurons_) {
        float similarity = computeSimilarity(queryPattern, patternNeuron.second);
        
        if (similarity >= pImpl->minSimilarity) {
            scoredPatterns.emplace_back(similarity, patternNeuron.second);
        }
    }
    
    // Sort by similarity (descending)
    std::sort(scoredPatterns.begin(), scoredPatterns.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Return top results
    size_t count = std::min(maxResults, scoredPatterns.size());
    for (size_t i = 0; i < count; ++i) {
        results.push_back(scoredPatterns[i].second);
    }
    
    return results;
}

float NeuralAssociativeMemory::getAssociationStrength(
    const std::vector<float>& patternA,
    const std::vector<float>& patternB) const {
    if (patternA.empty() || patternB.empty()) {
        return 0.0f;
    }
    
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA == INVALID_NEURON_ID || neuronB == INVALID_NEURON_ID) {
        return 0.0f;
    }
    
    // Find association strength
    for (const auto& assoc : associations_) {
        NeuronId a = std::get<0>(assoc);
        NeuronId b = std::get<1>(assoc);
        float strength = std::get<2>(assoc);
        
        if ((a == neuronA && b == neuronB) || (a == neuronB && b == neuronA)) {
            return strength;
        }
    }
    
    return 0.0f;
}

void NeuralAssociativeMemory::updateAssociation(
    const std::vector<float>& patternA,
    const std::vector<float>& patternB,
    float delta) {
    if (patternA.empty() || patternB.empty()) {
        return;
    }
    
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA == INVALID_NEURON_ID || neuronB == INVALID_NEURON_ID) {
        return;
    }
    
    // Update or create association
    for (auto& assoc : associations_) {
        NeuronId a = std::get<0>(assoc);
        NeuronId b = std::get<1>(assoc);
        float& strength = std::get<2>(assoc);
        
        if ((a == neuronA && b == neuronB) || (a == neuronB && b == neuronA)) {
            strength = std::clamp(strength + delta, 0.0f, 1.0f);
            ++pImpl->totalAssociationsUpdated;
            return;
        }
    }
    
    // Create new association
    float strength = std::clamp(delta, 0.0f, 1.0f);
    associations_.emplace_back(neuronA, neuronB, strength);
    ++pImpl->totalAssociationsCreated;
}

std::vector<std::vector<float>> NeuralAssociativeMemory::spreadActivation(
    const std::vector<float>& cuePattern, size_t steps) const {
    std::vector<std::vector<float>> results;
    
    if (cuePattern.empty() || !brain_) {
        return results;
    }
    
    // Find directly associated patterns
    std::vector<std::vector<float>> currentLevel;
    
    for (const auto& patternNeuron : patternNeurons_) {
        float similarity = computeSimilarity(cuePattern, patternNeuron.second);
        
        if (similarity >= pImpl->minSimilarity * 0.8f) {  // Lower threshold for activation
            currentLevel.push_back(patternNeuron.second);
        }
    }
    
    // Iterate through association chains
    for (size_t step = 0; step < steps; ++step) {
        std::vector<std::vector<float>> nextLevel;
        
        for (const auto& pattern : currentLevel) {
            // Find neurons for this pattern
            NeuronId patternNeuron = INVALID_NEURON_ID;
            for (const auto& pn : patternNeurons_) {
                if (pn.second == pattern) {
                    patternNeuron = pn.first;
                    break;
                }
            }
            
            if (patternNeuron != INVALID_NEURON_ID) {
                // Find associations for this neuron
                for (const auto& assoc : associations_) {
                    NeuronId a = std::get<0>(assoc);
                    NeuronId b = std::get<1>(assoc);
                    float strength = std::get<2>(assoc);
                    
                    // Find the other neuron in the association
                    NeuronId otherNeuron = (a == patternNeuron) ? b : a;
                    
                    // Find pattern for the other neuron
                    for (const auto& pn : patternNeurons_) {
                        if (pn.first == otherNeuron) {
                            nextLevel.push_back(pn.second);
                            break;
                        }
                    }
                }
            }
        }
        
        // Add unique patterns to results
        for (const auto& pattern : nextLevel) {
            bool found = false;
            for (const auto& existing : results) {
                if (existing == pattern) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                results.push_back(pattern);
            }
        }
        
        currentLevel = nextLevel;
        if (currentLevel.empty()) break;
    }
    
    return results;
}

void NeuralAssociativeMemory::clear() {
    associations_.clear();
    patternNeurons_.clear();
    NLM_LOG_INFO("NeuralAssociativeMemory cleared");
}

NeuronId NeuralAssociativeMemory::findPatternNeuron(const std::vector<float>& pattern) {
    // Find existing neuron for this pattern
    for (const auto& patternNeuron : patternNeurons_) {
        if (computeSimilarity(pattern, patternNeuron.second) >= pImpl->minSimilarity) {
            return patternNeuron.first;
        }
    }
    
    // Create new neuron for pattern
    if (patternNeurons_.size() >= pImpl->maxAssociations) {
        return INVALID_NEURON_ID;
    }
    
    NeuronId newNeuronId = brain_->getTotalNeuronCount() + patternNeurons_.size();
    patternNeurons_.emplace_back(newNeuronId, pattern);
    
    NLM_LOG_INFO("Created new pattern neuron with ID: " + std::to_string(newNeuronId));
    return newNeuronId;
}

float NeuralAssociativeMemory::computeSimilarity(const std::vector<float>& a,
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

} // namespace nlm
