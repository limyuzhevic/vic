#include "NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

NeuralAssociativeMemory::NeuralAssociativeMemory()
    : learningRate_(0.01f)
    , consolidationRate_(0.001f)
    , pruningThreshold_(0.05f)
    , maxAssociationsPerPattern_(20)
    , brain_(nullptr)
    , currentStep_(0)
{
}

NeuralAssociativeMemory::~NeuralAssociativeMemory() = default;

void NeuralAssociativeMemory::initialize(Brain* brain) {
    brain_ = brain;
    NLM_LOG_INFO("NeuralAssociativeMemory initialized");
}

void NeuralAssociativeMemory::associate(NeuronId patternA, NeuronId patternB, float strength) {
    // Create or update forward association
    forwardMap[patternA].push_back(Association(patternA, patternB, strength));
    
    // Create or update backward association  
    backwardMap[patternB].push_back(Association(patternA, patternB, strength));
    
    // Maintain capacity limits
    if (forwardMap[patternA].size() > maxAssociationsPerPattern_) {
        // Keep only strongest associations
        std::partial_sort(forwardMap[patternA].begin(), 
                         forwardMap[patternA].begin() + maxAssociationsPerPattern_,
                         forwardMap[patternA].end(),
                         [](const Association& a, const Association& b) {
                             return a.strength > b.strength;
                         });
        forwardMap[patternA].resize(maxAssociationsPerPattern_);
    }
    
    if (backwardMap[patternB].size() > maxAssociationsPerPattern_) {
        std::partial_sort(backwardMap[patternB].begin(),
                         backwardMap[patternB].begin() + maxAssociationsPerPattern_,
                         backwardMap[patternB].end(),
                         [](const Association& a, const Association& b) {
                             return a.strength > b.strength;
                         });
        backwardMap[patternB].resize(maxAssociationsPerPattern_);
    }
    
    NLM_LOG_INFO("Associated patterns " + std::to_string(patternA.index()) + 
                 " -> " + std::to_string(patternB.index()) + 
                 " with strength " + std::to_string(strength));
}

std::vector<NeuronId> NeuralAssociativeMemory::getAssociations(NeuronId pattern) const {
    std::vector<NeuronId> associations;
    
    // Get forward associations
    auto it = forwardMap.find(pattern);
    if (it != forwardMap.end()) {
        for (const auto& assoc : it->second) {
            associations.push_back(assoc.patternB);
        }
    }
    
    // Get backward associations (pattern is associated with others)
    it = backwardMap.find(pattern);
    if (it != backwardMap.end()) {
        for (const auto& assoc : it->second) {
            if (assoc.patternA != pattern) {
                associations.push_back(assoc.patternA);
            }
        }
    }
    
    return associations;
}

float NeuralAssociativeMemory::getAssociationStrength(NeuronId a, NeuronId b) const {
    // Check forward direction
    auto itA = forwardMap.find(a);
    if (itA != forwardMap.end()) {
        for (const auto& assoc : itA->second) {
            if (assoc.patternB == b) {
                return assoc.strength;
            }
        }
    }
    
    // Check backward direction
    auto itB = backwardMap.find(b);
    if (itB != backwardMap.end()) {
        for (const auto& assoc : itB->second) {
            if (assoc.patternA == a) {
                return assoc.strength;
            }
        }
    }
    
    return 0.0f;
}

void NeuralAssociativeMemory::updateAssociation(NeuronId a, NeuronId b, float delta) {
    // Update forward direction
    auto itA = forwardMap.find(a);
    if (itA != forwardMap.end()) {
        for (auto& assoc : itA->second) {
            if (assoc.patternB == b) {
                assoc.strength += delta;
                assoc.lastUpdated = currentStep_;
                // Apply eligibility trace modulation if delta is positive
                if (delta > 0.0f) {
                    assoc.eligibilityTrace = std::min(1.0f, assoc.eligibilityTrace + 0.1f);
                }
                return;
            }
        }
    }
    
    // Update backward direction
    auto itB = backwardMap.find(b);
    if (itB != backwardMap.end()) {
        for (auto& assoc : itB->second) {
            if (assoc.patternA == a) {
                assoc.strength += delta;
                assoc.lastUpdated = currentStep_;
                if (delta > 0.0f) {
                    assoc.eligibilityTrace = std::min(1.0f, assoc.eligibilityTrace + 0.1f);
                }
                return;
            }
        }
    }
    
    // Create new association if doesn't exist
    if (std::abs(delta) > 0.01f) {
        associate(a, b, delta);
    }
}

void NeuralAssociativeMemory::clear() {
    forwardMap.clear();
    backwardMap.clear();
    NLM_LOG_INFO("NeuralAssociativeMemory cleared");
}

size_t NeuralAssociativeMemory::getActiveAssociationCount() const {
    size_t total = 0;
    for (const auto& pair : forwardMap) {
        total += pair.second.size();
    }
    return total;
}

std::vector<std::pair<NeuronId, float>> NeuralAssociativeMemory::getStrongestAssociations(
    NeuronId pattern, size_t count) const {
    std::vector<std::pair<NeuronId, float>> result;
    
    auto it = forwardMap.find(pattern);
    if (it != forwardMap.end()) {
        // Copy and sort by strength
        std::vector<Association> associations = it->second;
        std::sort(associations.begin(), associations.end(),
                 [](const Association& a, const Association& b) {
                     return a.strength > b.strength;
                 });
        
        // Fill result with top associations
        size_t actualCount = std::min(count, associations.size());
        for (size_t i = 0; i < actualCount; ++i) {
            result.emplace_back(associations[i].patternB, associations[i].strength);
        }
    }
    
    return result;
}

void NeuralAssociativeMemory::pruneWeakAssociations(float threshold) {
    // Remove weak forward associations
    for (auto& pair : forwardMap) {
        auto& associations = pair.second;
        associations.erase(
            std::remove_if(associations.begin(), associations.end(),
                          [threshold](const Association& assoc) {
                              return assoc.strength < threshold;
                          }),
            associations.end()
        );
    }
    
    // Remove weak backward associations
    for (auto& pair : backwardMap) {
        auto& associations = pair.second;
        associations.erase(
            std::remove_if(associations.begin(), associations.end(),
                          [threshold](const Association& assoc) {
                              return assoc.strength < threshold;
                          }),
            associations.end()
        );
    }
}

void NeuralAssociativeMemory::formAssociationsFromWorkingMemory(
    const std::vector<NeuronId>& neurons,
    const std::vector<float>& activations) {
    if (neurons.size() != activations.size() || neurons.empty() || !brain_) {
        return;
    }
    
    // Sort neurons by activation strength
    std::vector<size_t> indices(neurons.size());
    for (size_t i = 0; i < neurons.size(); ++i) indices[i] = i;
    
    std::sort(indices.begin(), indices.end(),
              [&activations](size_t a, size_t b) {
                  return activations[a] > activations[b];
              });
    
    // Form associations between top-activated neurons
    size_t numToConnect = std::min(size_t(3), neurons.size());
    for (size_t i = 0; i < numToConnect; ++i) {
        for (size_t j = i + 1; j < numToConnect; ++j) {
            size_t idxI = indices[i];
            size_t idxJ = indices[j];
            
            NeuronId neuronA = neurons[idxI];
            NeuronId neuronB = neurons[idxJ];
            
            float strength = activations[idxI] * activations[idxJ] * 0.01f;
            
            // Update existing association or create new one
            updateAssociation(neuronA, neuronB, strength);
        }
    }
}

void NeuralAssociativeMemory::rehearseAssociations(const std::vector<NeuronId>& patterns) {
    if (patterns.empty()) return;
    
    // Find and strengthen associations involving these patterns
    for (NeuronId pattern : patterns) {
        // Strengthen forward associations
        auto it = forwardMap.find(pattern);
        if (it != forwardMap.end()) {
            for (auto& assoc : it->second) {
                assoc.strength *= (1.0f + 0.1f);  // 10% reinforcement
                assoc.eligibilityTrace = std::min(1.0f, assoc.eligibilityTrace + 0.2f);
            }
        }
        
        // Strengthen backward associations
        it = backwardMap.find(pattern);
        if (it != backwardMap.end()) {
            for (auto& assoc : it->second) {
                assoc.strength *= (1.0f + 0.1f);
                assoc.eligibilityTrace = std::min(1.0f, assoc.eligibilityTrace + 0.2f);
            }
        }
    }
}

// Apply consolidation during development stages
void NeuralAssociativeMemory::consolidate(float consolidationFactor) {
    for (auto& pair : forwardMap) {
        for (auto& assoc : pair.second) {
            // Stabilize associations based on developmental stage
            assoc.strength = std::min(1.0f, assoc.strength * (1.0f + consolidationFactor));
        }
    }
    
    for (auto& pair : backwardMap) {
        for (auto& assoc : pair.second) {
            assoc.strength = std::min(1.0f, assoc.strength * (1.0f + consolidationFactor));
        }
    }
}

} // namespace nlm
