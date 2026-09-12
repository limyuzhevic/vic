// src/memory/NeuralAssociativeMemory.cpp
#include "NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    Brain* brain;
    
    // Pattern neurons for association
    std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons;
    
    // Associations between patterns
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations;
    
    Impl() : brain(nullptr) {}
};

NeuralAssociativeMemory::NeuralAssociativeMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
{
    NLM_LOG_INFO("NeuralAssociativeMemory created");
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
    if (!brain_) return;
    
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA != INVALID_NEURON_ID && neuronB != INVALID_NEURON_ID) {
        // Check if association exists
        for (auto& assoc : pImpl->associations) {
            if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
                (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
                // Update strength
                std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + strength * 0.1f, 0.0f, 1.0f);
                return;
            }
        }
        
        // Create new association
        pImpl->associations.emplace_back(neuronA, neuronB, strength);
        
        // Create Hebbian connection between the pattern neurons
        if (auto* region = brain_->getRegion(neuronA.index() / 1000)) {
            // Find the source and destination neurons in the region
            // In a real implementation, we'd have a proper mapping
            // For now, we'll just log that we're creating an association
            NLM_LOG_INFO("Creating associative connection between pattern neurons");
        }
    }
}

void NeuralAssociativeMemory::associateFromExperience(const EpisodicMemoryItem& episode) {
    if (!episode.sensoryState.empty() && !episode.resultingSensoryState.empty() && brain_) {
        // Create associations between input and output patterns
        associate(episode.sensoryState, episode.resultingSensoryState, 0.8f);
    }
    
    // Associate action with resulting state
    if (!episode.sensoryState.empty() && brain_) {
        // Create action-specific pattern
        std::vector<float> actionPattern(episode.sensoryState.size(), 0.0f);
        actionPattern[0] = static_cast<float>(episode.action) / 10.0f;
        
        associate(actionPattern, episode.resultingSensoryState, 0.7f);
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::retrieve(
    const std::vector<float>& queryPattern,
    size_t maxResults) const {
    
    if (pImpl->associations.empty()) return {};
    
    // Find the neuron that best matches the query pattern
    NeuronId bestMatchNeuron = INVALID_NEURON_ID;
    float bestSimilarity = 0.0f;
    
    for (const auto& pn : pImpl->patternNeurons) {
        float sim = computeSimilarity(queryPattern, pn.second);
        if (sim > bestSimilarity) {
            bestSimilarity = sim;
            bestMatchNeuron = pn.first;
        }
    }
    
    if (bestSimilarity < 0.3f || bestMatchNeuron == INVALID_NEURON_ID) {
        return {};
    }
    
    // Find all patterns associated with this neuron
    std::vector<std::vector<float>> results;
    
    for (const auto& assoc : pImpl->associations) {
        NeuronId otherNeuron;
        
        if (std::get<0>(assoc) == bestMatchNeuron) {
            otherNeuron = std::get<1>(assoc);
        } else if (std::get<1>(assoc) == bestMatchNeuron) {
            otherNeuron = std::get<0>(assoc);
        } else {
            continue;
        }
        
        // Find the pattern for this associated neuron
        for (const auto& pn : pImpl->patternNeurons) {
            if (pn.first == otherNeuron) {
                results.push_back(pn.second);
                break;
            }
        }
    }
    
    // Return up to maxResults
    if (results.size() > maxResults) {
        results.resize(maxResults);
    }
    
    return results;
}

float NeuralAssociativeMemory::getAssociationStrength(
    const std::vector<float>& patternA,
    const std::vector<float>& patternB) const {
    
    NeuronId neuronA, neuronB;
    float bestSimA = 0.0f, bestSimB = 0.0f;
    
    // Find the neuron that best matches patternA
    for (const auto& pn : pImpl->patternNeurons) {
        float simA = computeSimilarity(patternA, pn.second);
        if (simA > bestSimA) {
            bestSimA = simA;
            neuronA = pn.first;
        }
        
        float simB = computeSimilarity(patternB, pn.second);
        if (simB > bestSimB) {
            bestSimB = simB;
            neuronB = pn.first;
        }
    }
    
    if (bestSimA < 0.3f || bestSimB < 0.3f) {
        return 0.0f;
    }
    
    // Look up the association strength
    for (const auto& assoc : pImpl->associations) {
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
    
    for (auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + delta, 0.0f, 1.0f);
            return;
        }
    }
}

void NeuralAssociativeMemory::clear() {
    pImpl->patternNeurons.clear();
    pImpl->associations.clear();
}

NeuronId NeuralAssociativeMemory::findPatternNeuron(const std::vector<float>& pattern) {
    // Check if we already have a similar pattern
    for (const auto& pn : pImpl->patternNeurons) {
        if (computeSimilarity(pattern, pn.second) > 0.8f) {
            return pn.first;
        }
    }
    
    // Create new pattern neuron
    NeuronId newId(pImpl->patternNeurons.size() + 30000);
    pImpl->patternNeurons.push_back({newId, pattern});
    return newId;
}

float NeuralAssociativeMemory::computeSimilarity(const std::vector<float>& a,
                                                const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    float dot = 0.0f, normA = 0.0f, normB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(normA) * std::sqrt(normB));
}

std::vector<std::vector<float>> NeuralAssociativeMemory::spreadActivation(
    const std::vector<float>& cuePattern,
    size_t steps) const {
    
    std::vector<std::vector<float>> activated;
    std::set<NeuronId> visited;
    
    // Find initial pattern neuron
    NeuronId currentNeuron = INVALID_NEURON_ID;
    for (const auto& pn : pImpl->patternNeurons) {
        if (computeSimilarity(cuePattern, pn.second) > 0.5f) {
            currentNeuron = pn.first;
            activated.push_back(pn.second);
            visited.insert(currentNeuron);
            break;
        }
    }
    
    if (currentNeuron == INVALID_NEURON_ID) {
        return activated;
    }
    
    // Spread through association network
    for (size_t step = 0; step < steps; ++step) {
        std::vector<NeuronId> nextNeurons;
        
        for (const auto& assoc : pImpl->associations) {
            NeuronId otherNeuron = INVALID_NEURON_ID;
            
            if (std::get<0>(assoc) == currentNeuron && visited.find(std::get<1>(assoc)) == visited.end()) {
                otherNeuron = std::get<1>(assoc);
            } else if (std::get<1>(assoc) == currentNeuron && visited.find(std::get<0>(assoc)) == visited.end()) {
                otherNeuron = std::get<0>(assoc);
            }
            
            if (otherNeuron != INVALID_NEURON_ID && std::get<2>(assoc) > 0.3f) {
                nextNeurons.push_back(otherNeuron);
                visited.insert(otherNeuron);
                
                // Find pattern for this neuron
                for (const auto& pn : pImpl->patternNeurons) {
                    if (pn.first == otherNeuron) {
                        activated.push_back(pn.second);
                        break;
                    }
                }
            }
        }
        
        if (nextNeurons.empty()) break;
        
        // Pick highest associated next neuron
        currentNeuron = nextNeurons[0];
    }
    
    return activated;
}

} // namespace nlm
