#include "NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    Brain* brain;
    
    // Associations: pattern -> related pattern
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations;
    
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
    
    // Find or create neurons for these patterns
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    // Store association with strength
    associations_.emplace_back(neuronA, neuronB, strength);
    
    // Create bidirectional connection
    if (neuronA && neuronB) {
        // Create strong connection between the two neurons
        // (In a real implementation, this would use proper synaptic formation)
        NLM_LOG_DEBUG("Created association between neurons: " + std::to_string(neuronA.value) + 
                     " and " + std::to_string(neuronB.value) + " with strength: " + std::to_string(strength));
    }
}

void NeuralAssociativeMemory::associateFromExperience(const EpisodicMemoryItem& episode) {
    if (!brain_ || episode.activeNeurons.empty()) return;
    
    // Extract neural patterns from the episode
    std::vector<float> patternA;
    std::vector<float> patternB;
    
    // Simplified: use first half of neurons as pattern A, second half as pattern B
    size_t half = episode.activeNeurons.size() / 2;
    if (half < 1) return;
    
    for (size_t i = 0; i < half && i < episode.activeNeurons.size(); ++i) {
        patternA.push_back(episode.neuronActivations[i]);
    }
    
    for (size_t i = half; i < episode.activeNeurons.size(); ++i) {
        if (i - half < episode.neuronActivations.size()) {
            patternB.push_back(episode.neuronActivations[i]);
        }
    }
    
    // Create association based on experience
    if (!patternA.empty() && !patternB.empty()) {
        float strength = episode.reward > 0 ? episode.reward : 0.5f;
        associate(patternA, patternB, strength);
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::retrieve(const std::vector<float>& queryPattern,
                                                                 size_t maxResults) const {
    std::vector<std::vector<float>> results;
    
    if (queryPattern.empty()) return results;
    
    // Find neurons most similar to query pattern
    std::vector<NeuronId> similarNeurons;
    float querySimilarity = 0.0f;
    
    for (const auto& association : associations_) {
        NeuronId a, b;
        float strength;
        std::tie(a, b, strength) = association;
        
        // Simplified: treat one side as the "stored" pattern
        std::vector<float> storedPattern = findPatternFromNeuron(a);
        
        if (!storedPattern.empty()) {
            float similarity = computeSimilarity(queryPattern, storedPattern);
            if (similarity > querySimilarity || results.empty()) {
                querySimilarity = similarity;
                if (similarity > 0.1f) {  // Threshold for relevance
                    similarNeurons.push_back(b);
                }
            }
        }
    }
    
    // Retrieve patterns from similar neurons
    for (NeuronId neuron : similarNeurons) {
        std::vector<float> pattern = findPatternFromNeuron(neuron);
        if (!pattern.empty()) {
            results.push_back(pattern);
            if (results.size() >= maxResults) break;
        }
    }
    
    return results;
}

float NeuralAssociativeMemory::getAssociationStrength(const std::vector<float>& patternA,
                                                    const std::vector<float>& patternB) const {
    if (patternA.empty() || patternB.empty()) return 0.0f;
    
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    for (const auto& association : associations_) {
        NeuronId a, b;
        float strength;
        std::tie(a, b, strength) = association;
        
        if (a == neuronA && b == neuronB) {
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
    
    for (auto& association : associations_) {
        NeuronId a, b;
        float& strength = std::get<2>(association);
        
        std::tie(a, b, std::ignore) = association;
        
        if ((a == neuronA && b == neuronB) || (a == neuronB && b == neuronA)) {
            strength += delta;
            strength = std::max(0.0f, strength);  // Ensure non-negative
            break;
        }
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::spreadActivation(const std::vector<float>& cuePattern,
                                                                          size_t steps) const {
    std::vector<std::vector<float>> results;
    
    if (cuePattern.empty()) return results;
    
    // Find neurons that match cue pattern
    std::vector<NeuronId> cueNeurons;
    for (const auto& association : associations_) {
        NeuronId a, b;
        std::tie(a, b, std::ignore) = association;
        
        std::vector<float> patternA = findPatternFromNeuron(a);
        if (!patternA.empty() && computeSimilarity(cuePattern, patternA) > 0.5f) {
            cueNeurons.push_back(b);
        }
    }
    
    // Spread activation to related neurons
    for (int step = 0; step < static_cast<int>(steps); ++step) {
        std::vector<NeuronId> nextNeurons;
        
        for (NeuronId cueNeuron : cueNeurons) {
            // Find neurons associated with this neuron
            for (const auto& association : associations_) {
                NeuronId a, b;
                std::tie(a, b, std::ignore) = association;
                
                // Check if this neuron is part of the chain
                if (a == cueNeuron) {
                    nextNeurons.push_back(b);
                } else if (b == cueNeuron) {
                    nextNeurons.push_back(a);
                }
            }
        }
        
        // Remove duplicates
        std::sort(nextNeurons.begin(), nextNeurons.end());
        nextNeurons.erase(std::unique(nextNeurons.begin(), nextNeurons.end()), nextNeurons.end());
        
        // Convert to patterns
        for (NeuronId neuron : nextNeurons) {
            std::vector<float> pattern = findPatternFromNeuron(neuron);
            if (!pattern.empty()) {
                bool alreadyInResults = false;
                for (const auto& existing : results) {
                    if (computeSimilarity(existing, pattern) > 0.8f) {
                        alreadyInResults = true;
                        break;
                    }
                }
                if (!alreadyInResults) {
                    results.push_back(pattern);
                    if (results.size() >= 20) break;  // Limit results
                }
            }
        }
        
        cueNeurons = nextNeurons;
        
        if (cueNeurons.empty()) break;
    }
    
    return results;
}

void NeuralAssociativeMemory::clear() {
    associations_.clear();
    NLM_LOG_INFO("NeuralAssociativeMemory cleared");
}

NeuronId NeuralAssociativeMemory::findPatternNeuron(const std::vector<float>& pattern) {
    if (pattern.empty() || !brain_) return INVALID_NEURON_ID;
    
    // Try to find existing neuron for this pattern
    for (const auto& entry : patternNeurons_) {
        if (computeSimilarity(pattern, entry.second) > 0.9f) {
            return entry.first;
        }
    }
    
    // Create new neuron for this pattern
    NeuronId newNeuron(patternNeurons_.size() + 10000);
    patternNeurons_.emplace_back(newNeuron, pattern);
    
    NLM_LOG_DEBUG("Created new pattern neuron for association with " + std::to_string(pattern.size()) + " features");
    
    return newNeuron;
}

std::vector<float> NeuralAssociativeMemory::findPatternFromNeuron(NeuronId neuron) const {
    for (const auto& entry : patternNeurons_) {
        if (entry.first == neuron) {
            return entry.second;
        }
    }
    return std::vector<float>();
}

float NeuralAssociativeMemory::computeSimilarity(const std::vector<float>& a,
                                               const std::vector<float>& b) const {
    if (a.empty() || b.empty()) return 0.0f;
    
    size_t size = std::min(a.size(), b.size());
    if (size == 0) return 0.0f;
    
    float similarity = 0.0f;
    for (size_t i = 0; i < size; ++i) {
        similarity += std::abs(a[i] - b[i]);
    }
    
    // Normalize to [0, 1] where 1 means most similar
    return 1.0f - std::min(1.0f, similarity / static_cast<float>(size));
}

} // namespace nlm