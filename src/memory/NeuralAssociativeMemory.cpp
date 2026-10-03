#include "NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    Brain* brain;
    
    // Maintenance connections (recurrent)
    std::vector<std::pair<NeuronId, NeuronId>> maintenanceSynapses;
    
    // Memory trace ages
    std::vector<SimulationStep> traceAges;
    
    Impl() : brain(nullptr) {}
};

NeuralAssociativeMemory::NeuralAssociativeMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
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
                                       float strength) {
    if (patternA.empty() || patternB.empty() || !brain_) return;
    
    // Find neurons to encode this pattern
    size_t neuronsNeeded = std::min(patternA.size(), patternNeurons_.size());
    
    NeuronId neuronA = INVALID_NEURON_ID;
    NeuronId neuronB = INVALID_NEURON_ID;
    
    // Get or create neurons for pattern A
    if (neuronsNeeded > 0) {
        neuronA = findPatternNeuron(patternA);
    }
    
    // Get or create neurons for pattern B
    neuronsNeeded = std::min(patternB.size(), patternNeurons_.size());
    if (neuronsNeeded > 0) {
        neuronB = findPatternNeuron(patternB);
    }
    
    if (neuronA != INVALID_NEURON_ID && neuronB != INVALID_NEURON_ID) {
        // Check if association exists
        for (auto& assoc : associations_) {
            if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
                (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
                // Update strength
                std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + strength * 0.1f, 0.0f, 1.0f);
                return;
            }
        }
        
        // Create new association
        associations_.emplace_back(neuronA, neuronB, strength);
    }
}

void NeuralAssociativeMemory::associateFromExperience(const EpisodicMemoryItem& episode) {
    if (!episode.sensoryState.empty() && !episode.resultingSensoryState.empty()) {
        associate(episode.sensoryState, episode.resultingSensoryState, 0.8f);
    }
    
    // Associate action with resulting state
    if (!episode.sensoryState.empty()) {
        // Create action-specific pattern
        std::vector<float> actionPattern(episode.sensoryState.size(), 0.0f);
        actionPattern[0] = static_cast<float>(episode.action) / 10.0f;
        
        associate(actionPattern, episode.resultingSensoryState, 0.7f);
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::retrieve(
    const std::vector<float>& queryPattern,
    size_t maxResults) const {
    
    if (queryPattern.empty() || patternNeurons_.empty()) {
        return {};
    }
    
    NeuronId queryNeuron = findPatternNeuron(queryPattern);
    
    if (queryNeuron == INVALID_NEURON_ID) {
        return {};
    }
    
    // Find associated patterns
    std::vector<std::pair<float, std::vector<float>>> scored;
    
    for (const auto& assoc : associations_) {
        NeuronId otherNeuron;
        
        if (std::get<0>(assoc) == queryNeuron) {
            otherNeuron = std::get<1>(assoc);
        } else if (std::get<1>(assoc) == queryNeuron) {
            otherNeuron = std::get<0>(assoc);
        } else {
            continue;
        }
        
        if (otherNeuron != INVALID_NEURON_ID) {
            for (const auto& pn : patternNeurons_) {
                if (pn.first == otherNeuron) {
                    float strength = std::get<2>(assoc);
                    scored.emplace_back(strength, pn.second);
                    break;
                }
            }
        }
    }
    
    // Sort by strength (descending)
    std::sort(scored.begin(), scored.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Return top results
    std::vector<std::vector<float>> results;
    size_t count = std::min(maxResults, scored.size());
    
    for (size_t i = 0; i < count; ++i) {
        results.push_back(scored[i].second);
    }
    
    return results;
}

float NeuralAssociativeMemory::getAssociationStrength(const std::vector<float>& patternA,
                                                      const std::vector<float>& patternB) const {
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA == INVALID_NEURON_ID || neuronB == INVALID_NEURON_ID) {
        return 0.0f;
    }
    
    for (const auto& assoc : associations_) {
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
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA == INVALID_NEURON_ID || neuronB == INVALID_NEURON_ID) {
        return;
    }
    
    for (auto& assoc : associations_) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + delta, 0.0f, 1.0f);
            return;
        }
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::spreadActivation(
    const std::vector<float>& cuePattern,
    size_t steps) const {
    std::vector<std::vector<float>> results;
    
    // Start with the cue pattern
    results.push_back(cuePattern);
    
    // Spread activation for specified number of steps
    for (size_t step = 0; step < steps; ++step) {
        std::vector<std::vector<float>> newResults;
        
        for (const auto& pattern : results) {
            // Find associated patterns
            for (const auto& assoc : associations_) {
                NeuronId neuron1 = std::get<0>(assoc);
                NeuronId neuron2 = std::get<1>(assoc);
                
                // Check if either neuron matches a pattern in patternNeurons_
                for (const auto& pn : patternNeurons_) {
                    if (pn.first == neuron1 || pn.first == neuron2) {
                        newResults.push_back(pn.second);
                        break;
                    }
                }
            }
        }
        
        // Add new results to the main results
        for (const auto& pattern : newResults) {
            if (std::find(results.begin(), results.end(), pattern) == results.end()) {
                results.push_back(pattern);
            }
        }
    }
    
    return results;
}

void NeuralAssociativeMemory::clear() {
    associations_.clear();
    patternNeurons_.clear();
}

NeuronId NeuralAssociativeMemory::findPatternNeuron(const std::vector<float>& pattern) {
    if (pattern.empty() || !brain_) {
        return INVALID_NEURON_ID;
    }
    
    // Check if this exact pattern already exists
    for (const auto& pn : patternNeurons_) {
        if (computeSimilarity(pattern, pn.second) > 0.95f) {
            return pn.first;
        }
    }
    
    // Create new neuron for this pattern
    // Find a region to create the neuron in
    auto regions = brain_->getRegions();
    if (regions.empty()) {
        return INVALID_NEURON_ID;
    }
    
    // Get the first region
    auto* region = regions[0].get();
    if (!region) {
        return INVALID_NEURON_ID;
    }
    
    // Find a neuron ID that's not used
    size_t nextId = 10000;  // Offset to avoid conflicts
    for (const auto& pn : patternNeurons_) {
        if (pn.first.index() >= nextId) {
            nextId = pn.first.index() + 1;
        }
    }
    
    // Create a new neuron (this is simplified - real implementation would need actual neuron creation)
    NeuronId newNeuron(nextId);
    patternNeurons_.emplace_back(newNeuron, pattern);
    
    return newNeuron;
}

float NeuralAssociativeMemory::computeSimilarity(const std::vector<float>& a,
                                                const std::vector<float>& b) const {
    if (a.empty() || b.empty()) {
        return 0.0f;
    }
    
    size_t minSize = std::min(a.size(), b.size());
    float dotProduct = 0.0f;
    
    for (size_t i = 0; i < minSize; ++i) {
        dotProduct += a[i] * b[i];
    }
    
    float normA = 0.0f, normB = 0.0f;
    for (float val : a) normA += val * val;
    for (float val : b) normB += val * val;
    
    normA = std::sqrt(normA);
    normB = std::sqrt(normB);
    
    if (normA == 0.0f || normB == 0.0f) {
        return 0.0f;
    }
    
    return dotProduct / (normA * normB);
}

} // namespace nlm