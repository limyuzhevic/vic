#include "AssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <queue>

namespace nlm {

class AssociativeMemory::Impl {
public:
    Brain* brain;
    size_t capacity;
    float decayRate;
    
    // Pattern neurons
    std::vector<std::pair<std::string, std::vector<float>>> patternNeurons;
    
    // Associations
    std::vector<std::tuple<std::string, std::string, float>> associations;
    
    Impl(size_t cap, float decay) : capacity(cap), decayRate(decay) {}
};

AssociativeMemory::AssociativeMemory()
    : pImpl(new Impl(500, 0.01f))
{
    NLM_LOG_INFO("AssociativeMemory created");
}

AssociativeMemory::~AssociativeMemory() = default;

void AssociativeMemory::store(const std::vector<float>& pattern, float relevance) {
    // Create association for this pattern
    std::string patternName = "pattern_" + std::to_string(pImpl->patternNeurons.size());
    
    // Find or create pattern neuron
    NeuronId neuronId = findPatternNeuron(pattern);
    if (neuronId != INVALID_NEURON_ID) {
        // Associate with itself (self-connection for pattern completion)
        associate(pattern, pattern, relevance);
    }
}

std::vector<float> AssociativeMemory::retrieve(size_t index) const {
    if (index < pImpl->patternNeurons.size()) {
        return pImpl->patternNeurons[index].second;
    }
    return {};
}

bool AssociativeMemory::contains(const std::vector<float>& pattern) const {
    for (const auto& pair : pImpl->patternNeurons) {
        if (computeSimilarity(pattern, pair.second) > 0.8f) {
            return true;
        }
    }
    return false;
}

void AssociativeMemory::decay(float decayRate) {
    for (auto& assoc : pImpl->associations) {
        std::get<2>(assoc) *= (1.0f - decayRate * pImpl->decayRate);
    }
}

void AssociativeMemory::consolidate(float relevanceThreshold) {
    // Remove weak associations
    pImpl->associations.erase(
        std::remove_if(pImpl->associations.begin(), pImpl->associations.end(),
                     [relevanceThreshold](const auto& assoc) {
                         return std::get<2>(assoc) < relevanceThreshold;
                     }),
        pImpl->associations.end()
    );
}

void AssociativeMemory::clear() {
    pImpl->patternNeurons.clear();
    pImpl->associations.clear();
}

size_t AssociativeMemory::getCapacity() const {
    return pImpl->capacity;
}

size_t AssociativeMemory::getCurrentSize() const {
    return pImpl->associations.size();
}

float AssociativeMemory::getLoad() const {
    if (pImpl->capacity == 0) return 0.0f;
    return static_cast<float>(pImpl->associations.size()) / pImpl->capacity;
}

void AssociativeMemory::updateFromBrain(Brain* brain) {
    pImpl->brain = brain;
}

void AssociativeMemory::applyToBrain(Brain* brain) {
    if (!brain || pImpl->brain != brain) return;
    
    // Apply strong associations
    for (const auto& assoc : pImpl->associations) {
        if (std::get<2>(assoc) > 0.5f) {
            // Find pattern neurons
            std::vector<float> patternA, patternB;
            bool foundA = false, foundB = false;
            
            for (const auto& pn : pImpl->patternNeurons) {
                if (pn.first == std::get<0>(assoc)) {
                    patternA = pn.second;
                    foundA = true;
                }
                if (pn.first == std::get<1>(assoc)) {
                    patternB = pn.second;
                    foundB = true;
                }
            }
            
            if (foundA && foundB) {
                // Create pattern completion
                std::vector<float> completed(patternA.size(), 0.0f);
                for (size_t i = 0; i < patternA.size(); ++i) {
                    completed[i] = (patternA[i] + patternB[i]) * 0.5f;
                }
                
                // Inject into brain
                for (size_t i = 0; i < completed.size() && i < activeNeurons_.size(); ++i) {
                    float activation = completed[i] * std::get<2>(assoc) * 3.0f;
                    brain->injectCurrent(activeNeurons_[i], activation);
                }
            }
        }
    }
}

void AssociativeMemory::createAssociation(const std::vector<float>& patternA,
                                         const std::vector<float>& patternB,
                                         float strength) {
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
    }
}

std::vector<std::vector<float>> AssociativeMemory::spreadActivation(const std::vector<float>& cue,
                                                                    size_t steps) const {
    
    std::vector<std::vector<float>> activated;
    std::set<NeuronId> visited;
    
    // Find initial pattern neuron
    NeuronId currentNeuron;
    for (const auto& pn : pImpl->patternNeurons) {
        if (computeSimilarity(cue, pn.second) > 0.5f) {
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

float AssociativeMemory::getAssociationStrength(const std::vector<float>& patternA,
                                                const std::vector<float>& patternB) const {
    NeuronId neuronA, neuronB;
    float bestSimA = 0.0f, bestSimB = 0.0f;
    
    for (const auto& pn : pImpl->patternNeurons) {
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
    
    for (const auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            return std::get<2>(assoc);
        }
    }
    
    return 0.0f;
}

void AssociativeMemory::updateAssociation(const std::vector<float>& patternA,
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

void AssociativeMemory::clearAssociations() {
    pImpl->associations.clear();
}

NeuronId AssociativeMemory::findPatternNeuron(const std::vector<float>& pattern) {
    // Check if we already have a similar pattern
    for (const auto& pn : pImpl->patternNeurons) {
        if (computeSimilarity(pattern, pn.second) > 0.8f) {
            return pn.first;
        }
    }
    
    // Create new pattern neuron
    NeuronId newId(pImpl->patternNeurons.size() + 30000);
    pImpl->patternNeurons.emplace_back(std::to_string(newId), pattern);
    return newId;
}

float AssociativeMemory::computeSimilarity(const std::vector<float>& a,
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

} // namespace nlm