#include "NeuralAssociativeMemory.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_set>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    Brain* brain;
    std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons;
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations;
    std::unordered_map<uint64_t, float> neuronActivityCache;
    
    Impl() : brain(nullptr) {}
};

NeuralAssociativeMemory::NeuralAssociativeMemory() : pImpl(new Impl) {}

NeuralAssociativeMemory::~NeuralAssociativeMemory() = default;

void NeuralAssociativeMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
}

NeuronId NeuralAssociativeMemory::getPatternNeuron(const std::vector<float>& pattern) {
    // Create hash for pattern
    uint64_t hash = 0;
    for (float val : pattern) {
        hash = hash * 31 + static_cast<uint64_t>(val * 1000.0f);
    }
    
    // Check if pattern already has a neuron
    auto it = pImpl->neuronActivityCache.find(hash);
    if (it != pImpl->neuronActivityCache.end()) {
        // Find and return the neuron
        for (const auto& pn : pImpl->patternNeurons) {
            if (pn.first.index() == it->second) {
                return pn.first;
            }
        }
    }
    
    // Create new neuron for this pattern
    NeuronId neuronId(pImpl->patternNeurons.size() + 1);
    pImpl->patternNeurons.emplace_back(neuronId, pattern);
    pImpl->neuronActivityCache[hash] = neuronId.index();
    
    // Inject current into the neuron to make it active
    if (pImpl->brain) {
        pImpl->brain->injectCurrent(neuronId, 10.0f);
    }
    
    return neuronId;
}

float NeuralAssociativeMemory::computeSimilarity(const std::vector<float>& a,
                                                const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) {
        return 0.0f;
    }
    
    float sumSq = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        float diff = a[i] - b[i];
        sumSq += diff * diff;
    }
    
    return 1.0f / (1.0f + std::sqrt(sumSq));
}

void NeuralAssociativeMemory::associate(const std::vector<float>& patternA,
                                       const std::vector<float>& patternB,
                                       float strength) {
    // Get neurons for both patterns
    NeuronId neuronA = getPatternNeuron(patternA);
    NeuronId neuronB = getPatternNeuron(patternB);
    
    // Check if association already exists
    for (auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            // Update existing association
            std::get<2>(assoc) = strength;
            return;
        }
    }
    
    // Create new association
    pImpl->associations.emplace_back(neuronA, neuronB, strength);
}

void NeuralAssociativeMemory::associateFromExperience(const EpisodicMemoryItem& episode) {
    if (!pImpl->brain || episode.activeNeurons.empty()) {
        return;
    }
    
    // Use the sensory state as pattern A
    std::vector<float> sensoryPattern = episode.sensoryState;
    
    // Create neural activation pattern from active neurons
    std::vector<float> neuralPattern;
    for (size_t i = 0; i < pImpl->brain->getTotalNeuronCount(); ++i) {
        neuralPattern.push_back(0.0f);  // Initialize with zeros
    }
    
    for (NeuronId neuronId : episode.activeNeurons) {
        if (neuronId.index() < neuralPattern.size()) {
            neuralPattern[neuronId.index()] = 1.0f;
        }
    }
    
    // Associate with reward as strength
    associate(sensoryPattern, neuralPattern, episode.reward);
}

std::vector<std::vector<float>> NeuralAssociativeMemory::retrieve(const std::vector<float>& queryPattern,
                                                                 size_t maxResults) const {
    std::vector<std::vector<float>> results;
    
    // Find most similar patterns
    std::vector<std::pair<float, std::vector<float>>> similarities;
    
    for (const auto& pn : pImpl->patternNeurons) {
        float similarity = computeSimilarity(queryPattern, pn.second);
        similarities.emplace_back(similarity, pn.second);
    }
    
    // Sort by similarity (highest first)
    std::sort(similarities.begin(), similarities.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Take top results
    size_t count = std::min(maxResults, similarities.size());
    for (size_t i = 0; i < count; ++i) {
        results.push_back(similarities[i].second);
    }
    
    return results;
}

float NeuralAssociativeMemory::getAssociationStrength(const std::vector<float>& patternA,
                                                     const std::vector<float>& patternB) const {
    // Get neurons for patterns
    NeuronId neuronA = NeuronId(0); // This would need to be implemented properly
    NeuronId neuronB = NeuronId(0);
    
    for (const auto& pn : pImpl->patternNeurons) {
        if (pn.second == patternA) {
            neuronA = pn.first;
            break;
        }
    }
    
    for (const auto& pn : pImpl->patternNeurons) {
        if (pn.second == patternB) {
            neuronB = pn.first;
            break;
        }
    }
    
    // Find association strength
    for (const auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            return std::get<2>(assoc);
        }
    }
    
    return 0.0f;
}

void NeuralAssociativeMemory::updateAssociation(const std::vector<float>& patternA,
                                                const std::vector<float>& patternB,
                                                float delta) {
    // Get neurons for patterns
    NeuronId neuronA = NeuronId(0);
    NeuronId neuronB = NeuronId(0);
    
    for (const auto& pn : pImpl->patternNeurons) {
        if (pn.second == patternA) {
            neuronA = pn.first;
            break;
        }
    }
    
    for (const auto& pn : pImpl->patternNeurons) {
        if (pn.second == patternB) {
            neuronB = pn.first;
            break;
        }
    }
    
    // Update association strength
    for (auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + delta, 0.0f, 1.0f);
            return;
        }
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::spreadActivation(const std::vector<float>& cuePattern,
                                                                         size_t steps) const {
    std::vector<std::vector<float>> results;
    results.push_back(cuePattern);
    
    std::vector<float> currentPattern = cuePattern;
    
    for (size_t step = 0; step < steps; ++step) {
        std::vector<float> nextPattern;
        float totalActivation = 0.0f;
        
        // Compute activation for each pattern
        for (const auto& pn : pImpl->patternNeurons) {
            float similarity = computeSimilarity(currentPattern, pn.second);
            if (similarity > 0.1f) {  // Threshold for activation
                nextPattern = pn.second;
                totalActivation += similarity;
            }
        }
        
        if (totalActivation > 0.0f) {
            // Normalize activation
            for (float& val : nextPattern) {
                val /= totalActivation;
            }
            results.push_back(nextPattern);
            currentPattern = nextPattern;
        }
    }
    
    return results;
}

void NeuralAssociativeMemory::clear() {
    pImpl->patternNeurons.clear();
    pImpl->associations.clear();
    pImpl->neuronActivityCache.clear();
}

} // namespace nlm