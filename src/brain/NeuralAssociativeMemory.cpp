#include "NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    // Pattern neurons (one per unique sensory pattern cluster)
    std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons;
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations;
    
    // Activation levels for spreading
    std::vector<float> activationLevels;
    
    // Memory consolidation thresholds
    float consolidationThreshold;
    float decayRate;
    
    Impl() : consolidationThreshold(0.3f), decayRate(0.01f) {}
};

NeuralAssociativeMemory::NeuralAssociativeMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , dopamine_(nullptr)
{
}

NeuralAssociativeMemory::~NeuralAssociativeMemory() = default;

void NeuralAssociativeMemory::initialize(Brain* brain) {
    pImpl = std::make_unique<Impl>();
    brain_ = brain;
    
    // Get dopamine for neuromodulation
    if (brain_) {
        dopamine_ = brain->getDopamine();
    }
    
    NLM_LOG_INFO("NeuralAssociativeMemory initialized");
}

void NeuralAssociativeMemory::associate(const std::vector<float>& patternA,
                                         const std::vector<float>& patternB,
                                         float strength) {
    if (!brain_ || patternA.empty() || patternB.empty()) {
        return;
    }
    
    // Find or create pattern neurons
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA != INVALID_NEURON_ID && neuronB != INVALID_NEURON_ID) {
        // Create or update association
        float currentStrength = getAssociationStrength(patternA, patternB);
        
        // Check if association already exists
        bool exists = false;
        for (auto& assoc : pImpl->associations) {
            if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
                (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
                std::get<2>(assoc) = strength;
                exists = true;
                break;
            }
        }
        
        if (!exists) {
            pImpl->associations.emplace_back(neuronA, neuronB, strength);
        }
        
        // Create bidirectional Hebbian connection
        if (brain_) {
            // This would create actual synaptic connections in real implementation
            // For now, just log the association
            NLM_LOG_INFO("Created association between patterns (strength: " + std::to_string(strength) + ")");
        }
    }
}

void NeuralAssociativeMemory::associateFromExperience(const EpisodicMemoryItem& episode) {
    if (!brain_ || !dopamine_) {
        return;
    }
    
    // Create associations between patterns in this experience
    if (!episode.sensoryState.empty() && !episode.resultingSensoryState.empty()) {
        float strength = 1.0f;
        
        // Enhance association with dopamine modulation
        float dopamineMod = dopamine_->getPlasticityFactor();
        strength *= dopamineMod;
        
        // Store reward in association strength
        strength *= (1.0f + episode.reward);
        
        // Create association
        associate(episode.sensoryState, episode.resultingSensoryState, strength);
        
        // Also associate action with consequences
        if (episode.action != ActionType::Wait) {
            std::vector<float> actionPattern;
            actionPattern.push_back(static_cast<float>(episode.action));
            associate(actionPattern, episode.resultingSensoryState, strength * 0.8f);
        }
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::retrieve(const std::vector<float>& queryPattern,
                                                                      size_t maxResults) const {
    std::vector<std::vector<float>> results;
    
    if (pImpl->associations.empty()) {
        return results;
    }
    
    // Find pattern neuron for query
    NeuronId queryNeuron = INVALID_NEURON_ID;
    for (const auto& pattern : pImpl->patternNeurons) {
        float similarity = computeSimilarity(queryPattern, pattern.second);
        if (similarity > 0.7f) {
            queryNeuron = pattern.first;
            break;
        }
    }
    
    if (queryNeuron == INVALID_NEURON_ID) {
        return results;
    }
    
    // Find all associated patterns
    for (const auto& assoc : pImpl->associations) {
        if (std::get<0>(assoc) == queryNeuron) {
            // Find the pattern representation
            for (const auto& pattern : pImpl->patternNeurons) {
                if (pattern.first == std::get<1>(assoc)) {
                    results.push_back(pattern.second);
                    break;
                }
            }
        } else if (std::get<1>(assoc) == queryNeuron) {
            // Find the pattern representation
            for (const auto& pattern : pImpl->patternNeurons) {
                if (pattern.first == std::get<0>(assoc)) {
                    results.push_back(pattern.second);
                    break;
                }
            }
        }
    }
    
    // Limit results
    if (results.size() > maxResults) {
        results.resize(maxResults);
    }
    
    return results;
}

float NeuralAssociativeMemory::getAssociationStrength(const std::vector<float>& patternA,
                                                       const std::vector<float>& patternB) const {
    NeuronId neuronA = INVALID_NEURON_ID;
    NeuronId neuronB = INVALID_NEURON_ID;
    
    // Find neurons for these patterns
    for (const auto& pattern : pImpl->patternNeurons) {
        if (pattern.second == patternA) {
            neuronA = pattern.first;
        } else if (pattern.second == patternB) {
            neuronB = pattern.first;
        }
    }
    
    // Search for association
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
    // Find current strength
    float currentStrength = getAssociationStrength(patternA, patternB);
    float newStrength = currentStrength + delta;
    
    // Apply stabilization - prevent negative or excessive strengths
    newStrength = std::max(0.0f, std::min(newStrength, 2.0f));
    
    if (newStrength > 0.0f) {
        // Update associations
        NeuronId neuronA = INVALID_NEURON_ID;
        NeuronId neuronB = INVALID_NEURON_ID;
        
        for (const auto& pattern : pImpl->patternNeurons) {
            if (pattern.second == patternA) {
                neuronA = pattern.first;
            } else if (pattern.second == patternB) {
                neuronB = pattern.first;
            }
        }
        
        // Update existing association
        for (auto& assoc : pImpl->associations) {
            if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
                (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
                std::get<2>(assoc) = newStrength;
                break;
            }
        }
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::spreadActivation(const std::vector<float>& cuePattern,
                                                                           size_t steps) const {
    std::vector<std::vector<float>> results;
    
    if (pImpl->associations.empty()) {
        return results;
    }
    
    // Find strongly activated patterns (retrieve)
    std::vector<std::vector<float>> activatedPatterns = retrieve(cuePattern, 10);
    
    // Add cue pattern itself
    results.push_back(cuePattern);
    
    // For each step, spread to associated patterns
    for (size_t step = 0; step < steps; ++step) {
        std::vector<std::vector<float>> newPatterns;
        
        for (const auto& pattern : activatedPatterns) {
            // Find patterns associated with this pattern
            for (const auto& assoc : pImpl->associations) {
                // Check for association with pattern
                for (const auto& storedPattern : pImpl->patternNeurons) {
                    if (storedPattern.second == pattern) {
                        if (std::get<0>(assoc) == storedPattern.first) {
                            // Get the associated pattern
                            for (const auto& otherPattern : pImpl->patternNeurons) {
                                if (otherPattern.first == std::get<1>(assoc)) {
                                    newPatterns.push_back(otherPattern.second);
                                    break;
                                }
                            }
                        } else if (std::get<1>(assoc) == storedPattern.first) {
                            // Get the associated pattern
                            for (const auto& otherPattern : pImpl->patternNeurons) {
                                if (otherPattern.first == std::get<0>(assoc)) {
                                    newPatterns.push_back(otherPattern.second);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        
        // Add new patterns to results
        for (const auto& pattern : newPatterns) {
            bool alreadyPresent = false;
            for (const auto& existing : results) {
                if (computeSimilarity(pattern, existing) > 0.8f) {
                    alreadyPresent = true;
                    break;
                }
            }
            
            if (!alreadyPresent) {
                results.push_back(pattern);
            }
        }
        
        // Update activated patterns for next step
        activatedPatterns = newPatterns;
        
        // Stop if no new patterns
        if (newPatterns.empty()) {
            break;
        }
    }
    
    return results;
}

void NeuralAssociativeMemory::clear() {
    pImpl->patternNeurons.clear();
    pImpl->associations.clear();
    pImpl->activationLevels.clear();
}

void NeuralAssociativeMemory::enhanceWithNeuromodulation(float modulationFactor) {
    if (!dopamine_) {
        return;
    }
    
    // Apply neuromodulation to associations
    for (auto& assoc : pImpl->associations) {
        float& strength = std::get<2>(assoc);
        
        // Enhance weak associations with neuromodulation
        if (strength < 0.5f) {
            strength *= (1.0f + modulationFactor * 0.5f);
        }
        
        // Stabilize strong associations
        if (strength > 1.5f) {
            strength = 1.5f + (strength - 1.5f) * 0.5f;  // Reduce excessive strengths
        }
        
        // Apply decay to older associations
        strength *= (1.0f - pImpl->decayRate);
    }
    
    // Consolidate weak associations
    std::vector<std::tuple<NeuronId, NeuronId, float>> toRemove;
    for (const auto& assoc : pImpl->associations) {
        if (std::get<2>(assoc) < pImpl->consolidationThreshold) {
            toRemove.push_back(assoc);
        }
    }
    
    for (const auto& assoc : toRemove) {
        pImpl->associations.erase(
            std::remove(pImpl->associations.begin(), pImpl->associations.end(), assoc),
            pImpl->associations.end()
        );
    }
}

NeuronId NeuralAssociativeMemory::findPatternNeuron(const std::vector<float>& pattern) {
    // Check if pattern already exists
    for (const auto& existing : pImpl->patternNeurons) {
        float similarity = computeSimilarity(pattern, existing.second);
        if (similarity > 0.8f) {
            return existing.first;
        }
    }
    
    // Create new pattern neuron
    if (brain_) {
        NeuronId newId = NeuronId(pImpl->patternNeurons.size() + 20000);
        pImpl->patternNeurons.emplace_back(newId, pattern);
        return newId;
    }
    
    return INVALID_NEURON_ID;
}

float NeuralAssociativeMemory::computeSimilarity(const std::vector<float>& a,
                                                 const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) {
        return 0.0f;
    }
    
    // Cosine similarity
    float dotProduct = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    
    for (size_t i = 0; i < a.size(); ++i) {
        dotProduct += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) {
        return 0.0f;
    }
    
    return dotProduct / (std::sqrt(normA) * std::sqrt(normB));
}

} // namespace nlm