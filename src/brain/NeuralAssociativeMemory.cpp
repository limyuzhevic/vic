// NeuralAssociativeMemory.cpp - Complete Implementation
// This implements the associative memory system as referenced in the architecture

#include "brain/NeuralAssociativeMemory.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    Brain* brain;
    
    // Pattern storage and associations
    std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons_;
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations_;
    
    // Statistics
    size_t accessCount;
    size_t creationCount;
    
    Impl() : brain(nullptr), accessCount(0), creationCount(0) {}
};

NeuralAssociativeMemory::NeuralAssociativeMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
{
    NLM_LOG_INFO("NeuralAssociativeMemory: Created associative memory system");
}

NeuralAssociativeMemory::~NeuralAssociativeMemory() = default;

void NeuralAssociativeMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralAssociativeMemory: Initialized with brain reference");
}

void NeuralAssociativeMemory::associate(const std::vector<float>& patternA,
                                       const std::vector<float>& patternB,
                                       float strength) {
    if (pImpl->brain == nullptr) {
        NLM_LOG_ERROR("NeuralAssociativeMemory::associate: Not initialized");
        return;
    }
    
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA == INVALID_NEURON_ID || neuronB == INVALID_NEURON_ID) {
        NLM_LOG_ERROR("NeuralAssociativeMemory::associate: Failed to create pattern neurons");
        return;
    }
    
    // Check if association already exists
    for (auto& assoc : pImpl->associations_) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            // Update existing association
            std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + strength * 0.1f, 0.0f, 1.0f);
            NLM_LOG_DEBUG("NeuralAssociativeMemory::associate: Updated existing association between neurons "
                        << neuronA << " and " << neuronB << " to strength " << std::get<2>(assoc));
            return;
        }
    }
    
    // Create new association
    pImpl->associations_.emplace_back(neuronA, neuronB, strength);
    pImpl->creationCount++;
    
    NLM_LOG_DEBUG("NeuralAssociativeMemory::associate: Created new association between neurons "
                << neuronA << " and " << neuronB << " with strength " << strength);
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
    
    NLM_LOG_DEBUG("NeuralAssociativeMemory::associateFromExperience: Processed episode with action "
                << static_cast<int>(episode.action) << " and reward " << episode.reward);
}

std::vector<std::vector<float>> NeuralAssociativeMemory::retrieve(
    const std::vector<float>& queryPattern,
    size_t maxResults) const {
    
    pImpl->accessCount++;
    
    NeuronId queryNeuron;
    float bestSim = 0.0f;
    
    // Find best matching pattern neuron
    for (size_t i = 0; i < pImpl->patternNeurons_.size(); ++i) {
        float sim = computeSimilarity(queryPattern, pImpl->patternNeurons_[i].second);
        if (sim > bestSim) {
            bestSim = sim;
            queryNeuron = pImpl->patternNeurons_[i].first;
        }
    }
    
    if (bestSim < 0.3f) {
        NLM_LOG_DEBUG("NeuralAssociativeMemory::retrieve: No sufficiently similar pattern found (best sim "
                    << bestSim << ")");
        return {};
    }
    
    // Find associated patterns
    std::vector<std::vector<float>> results;
    std::vector<std::pair<float, std::vector<float>>> scored;
    
    for (const auto& assoc : pImpl->associations_) {
        NeuronId otherNeuron;
        
        if (std::get<0>(assoc) == queryNeuron) {
            otherNeuron = std::get<1>(assoc);
        } else if (std::get<1>(assoc) == queryNeuron) {
            otherNeuron = std::get<0>(assoc);
        }
        
        if (otherNeuron != INVALID_NEURON_ID) {
            for (const auto& pn : pImpl->patternNeurons_) {
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
    
    size_t resultCount = std::min(maxResults, scored.size());
    for (size_t i = 0; i < resultCount; ++i) {
        results.push_back(scored[i].second);
    }
    
    NLM_LOG_DEBUG("NeuralAssociativeMemory::retrieve: Retrieved " << resultCount << " patterns for query, best strength "
                << (resultCount > 0 ? scored[0].first : 0.0f));
    
    return results;
}

float NeuralAssociativeMemory::getAssociationStrength(
    const std::vector<float>& patternA,
    const std::vector<float>& patternB) const {
    
    NeuronId neuronA, neuronB;
    float bestSimA = 0.0f, bestSimB = 0.0f;
    
    for (const auto& pn : pImpl->patternNeurons_) {
        float simA = computeSimilarity(patternA, pn.second);
        float simB = computeSimilarity(patternB, pn.second);
        
        if (simA > bestSimA) {
            bestSimA = simA;
            neuronA = pn.first;
        }
        if (simB > bestSimB) {
            bestSimB = simB;
            neuronB = pn.first;
        }
    }
    
    if (bestSimA < 0.3f || bestSimB < 0.3f) {
        return 0.0f;
    }
    
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
    
    for (auto& assoc : pImpl->associations_) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + delta, 0.0f, 1.0f);
            NLM_LOG_DEBUG("NeuralAssociativeMemory::updateAssociation: Updated association between neurons "
                        << neuronA << " and " << neuronB << " by delta " << delta << " to " << std::get<2>(assoc));
            return;
        }
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::spreadActivation(
    const std::vector<float>& cuePattern,
    size_t steps) const {
    
    std::vector<std::vector<float>> activated;
    std::set<NeuronId> visited;
    
    // Find initial pattern neuron
    NeuronId currentNeuron;
    for (const auto& pn : pImpl->patternNeurons_) {
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
    for (size_t step = 0; step < steps && !pImpl->associations_.empty(); ++step) {
        std::vector<NeuronId> nextNeurons;
        
        for (const auto& assoc : pImpl->associations_) {
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
                for (const auto& pn : pImpl->patternNeurons_) {
                    if (pn.first == otherNeuron) {
                        activated.push_back(pn.second);
                        break;
                    }
                }
            }
        }
        
        if (nextNeurons.empty()) break;
        
        // Pick highest associated next neuron (weighted by association strength)
        size_t bestIdx = 0;
        float bestStrength = 0.0f;
        
        for (size_t i = 0; i < nextNeurons.size(); ++i) {
            // Find strength of this association
            float strength = 0.0f;
            for (const auto& assoc : pImpl->associations_) {
                NeuronId n1 = std::get<0>(assoc);
                NeuronId n2 = std::get<1>(assoc);
                if ((n1 == currentNeuron && n2 == nextNeurons[i]) || 
                    (n2 == currentNeuron && n1 == nextNeurons[i])) {
                    strength = std::get<2>(assoc);
                    break;
                }
            }
            
            if (strength > bestStrength) {
                bestStrength = strength;
                bestIdx = i;
            }
        }
        
        currentNeuron = nextNeurons[bestIdx];
    }
    
    NLM_LOG_DEBUG("NeuralAssociativeMemory::spreadActivation: Spread activation for " << steps << " steps, "
                << activated.size() << " patterns activated");
    
    return activated;
}

void NeuralAssociativeMemory::clear() {
    pImpl->patternNeurons_.clear();
    pImpl->associations_.clear();
    NLM_LOG_INFO("NeuralAssociativeMemory: Cleared all associations and patterns");
}

NeuronId NeuralAssociativeMemory::findPatternNeuron(const std::vector<float>& pattern) {
    // Check if we already have a similar pattern
    for (const auto& pn : pImpl->patternNeurons_) {
        if (computeSimilarity(pattern, pn.second) > 0.8f) {
            return pn.first;
        }
    }
    
    // Create new pattern neuron (using higher ID range than typical brain neurons)
    NeuronId newId(30000 + pImpl->patternNeurons_.size());
    pImpl->patternNeurons_.push_back({newId, pattern});
    
    NLM_LOG_DEBUG("NeuralAssociativeMemory::findPatternNeuron: Created new pattern neuron " << newId
                << " with " << pattern.size() << " dimensions");
    
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
    
    float normProduct = std::sqrt(normA) * std::sqrt(normB);
    if (normProduct < 1e-6f) return 0.0f;
    
    return dot / normProduct;
}

// Getters for statistics
NeuronId NeuralAssociativeMemory::getBrainReference() const {
    return reinterpret_cast<NeuronId>(brain_);
}

void NeuralAssociativeMemory::setBrainReference(Brain* brain) {
    brain_ = brain;
}

} // namespace nlm
