#include "NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    Brain* brain;
    
    // Maps patterns to neuron IDs for pattern storage
    std::unordered_map<size_t, NeuronId> patternToNeuron;
    
    // Similarity matrix for quick lookup (store as sparse)
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations;
    
    // Pattern cache for frequently accessed patterns
    std::vector<std::vector<float>> patternCache;
    
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
    if (patternA.empty() || patternB.empty() || !brain_) return;
    
    // Get or create neuron IDs for patterns
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA.isValid() && neuronB.isValid()) {
        // Update or create association
        bool found = false;
        for (auto& [a, b, s] : pImpl->associations) {
            if ((a == neuronA && b == neuronB) || (a == neuronB && b == neuronA)) {
                s = (s + strength) * 0.5f;  // Average with existing
                found = true;
                break;
            }
        }
        
        if (!found) {
            pImpl->associations.emplace_back(neuronA, neuronB, strength);
        }
        
        // Strengthen synaptic connections between neurons
        if (brain_) {
            // Create bidirectional connection with Hebbian-like strengthening
            float effectiveStrength = strength * 0.1f;  // Scale for neural dynamics
            brain_->injectCurrent(neuronA, effectiveStrength);
            brain_->injectCurrent(neuronB, effectiveStrength);
        }
        
        NLM_LOG_INFO("Created association between patterns: strength=" + std::to_string(strength));
    }
}

void NeuralAssociativeMemory::associateFromExperience(const EpisodicMemoryItem& episode) {
    if (!brain_) return;
    
    // Use sensory state as patterns
    std::vector<float> patternA = episode.sensoryState;
    std::vector<float> patternB = episode.resultingSensoryState;
    
    if (!patternA.empty() && !patternB.empty()) {
        // Association strength based on reward and novelty
        float strength = (episode.reward + episode.novelty) * 0.5f;
        associate(patternA, patternB, strength);
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::retrieve(const std::vector<float>& queryPattern,
                                                                    size_t maxResults) const {
    std::vector<std::vector<float>> results;
    
    if (queryPattern.empty() || !brain_) return results;
    
    // Find the neuron most similar to query pattern
    NeuronId bestMatch = NeuronId();
    float bestSimilarity = 0.0f;
    
    for (const auto& [pattern, neuron] : pImpl->patternToNeuron) {
        float similarity = computeSimilarity(queryPattern, pattern);
        if (similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestMatch = neuron;
        }
    }
    
    if (!bestMatch.isValid()) return results;
    
    // Find all patterns associated with the best match
    for (const auto& [neuronA, neuronB, strength] : pImpl->associations) {
        if (neuronA == bestMatch || neuronB == bestMatch) {
            // Get the associated pattern
            NeuronId otherNeuron = (neuronA == bestMatch) ? neuronB : neuronA;
            
            // Find pattern for otherNeuron
            std::vector<float> associatedPattern;
            for (const auto& [pattern, neuron] : pImpl->patternToNeuron) {
                if (neuron == otherNeuron) {
                    associatedPattern = pattern;
                    break;
                }
            }
            
            if (!associatedPattern.empty() && results.size() < maxResults) {
                results.push_back(associatedPattern);
            }
        }
    }
    
    return results;
}

float NeuralAssociativeMemory::getAssociationStrength(const std::vector<float>& patternA,
                                                     const std::vector<float>& patternB) const {
    if (patternA.empty() || patternB.empty()) return 0.0f;
    
    NeuronId neuronA = NeuronId();
    NeuronId neuronB = NeuronId();
    
    // Find neurons for these patterns
    for (const auto& [pattern, neuron] : pImpl->patternToNeuron) {
        if (pattern == patternA) neuronA = neuron;
        if (pattern == patternB) neuronB = neuron;
    }
    
    if (!neuronA.isValid() || !neuronB.isValid()) return 0.0f;
    
    // Look for association
    for (const auto& [a, b, strength] : pImpl->associations) {
        if ((a == neuronA && b == neuronB) || (a == neuronB && b == neuronA)) {
            return strength;
        }
    }
    
    return 0.0f;
}

void NeuralAssociativeMemory::updateAssociation(const std::vector<float>& patternA,
                                               const std::vector<float>& patternB,
                                               float delta) {
    NeuronId neuronA = NeuronId();
    NeuronId neuronB = NeuronId();
    
    // Find neurons for these patterns
    for (const auto& [pattern, neuron] : pImpl->patternToNeuron) {
        if (pattern == patternA) neuronA = neuron;
        if (pattern == patternB) neuronB = neuron;
    }
    
    if (!neuronA.isValid() || !neuronB.isValid()) return;
    
    // Update association strength
    for (auto& [a, b, strength] : pImpl->associations) {
        if ((a == neuronA && b == neuronB) || (a == neuronB && b == neuronA)) {
            strength += delta;
            strength = std::max(0.0f, std::min(1.0f, strength));  // Clamp to [0, 1]
            break;
        }
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::spreadActivation(
    const std::vector<float>& cuePattern, size_t steps) const {
    std::vector<std::vector<float>> results;
    
    if (cuePattern.empty() || !brain_) return results;
    
    std::vector<float> currentPattern = cuePattern;
    results.push_back(currentPattern);
    
    for (size_t step = 0; step < steps; ++step) {
        std::vector<std::vector<float>> newPatterns;
        
        // Get patterns directly associated with current pattern
        std::vector<float> nextPattern;
        for (const auto& [pattern, neuron] : pImpl->patternToNeuron) {
            if (computeSimilarity(currentPattern, pattern) > 0.7f) {
                nextPattern = pattern;
                break;
            }
        }
        
        if (nextPattern.empty()) break;
        
        // Find patterns associated with the next pattern
        for (const auto& [neuronA, neuronB, strength] : pImpl->associations) {
            // Look for neuron corresponding to nextPattern
            for (const auto& [pattern, neuron] : pImpl->patternToNeuron) {
                if (pattern == nextPattern && (neuron == neuronA || neuron == neuronB)) {
                    NeuronId otherNeuron = (neuron == neuronA) ? neuronB : neuronA;
                    
                    // Find pattern for otherNeuron
                    for (const auto& [otherPattern, otherNeuron2] : pImpl->patternToNeuron) {
                        if (otherNeuron2 == otherNeuron) {
                            newPatterns.push_back(otherPattern);
                            break;
                        }
                    }
                    break;
                }
            }
        }
        
        // Add new patterns to results (avoid duplicates)
        for (const auto& pattern : newPatterns) {
            bool duplicate = false;
            for (const auto& existing : results) {
                if (computeSimilarity(pattern, existing) > 0.8f) {
                    duplicate = true;
                    break;
                }
            }
            if (!duplicate) {
                results.push_back(pattern);
                currentPattern = pattern; // Continue chain
                break;
            }
        }
        
        if (newPatterns.empty()) break;
    }
    
    return results;
}

void NeuralAssociativeMemory::clear() {
    pImpl->associations.clear();
    pImpl->patternToNeuron.clear();
    pImpl->patternCache.clear();
    NLM_LOG_INFO("NeuralAssociativeMemory cleared");
}

NeuronId NeuralAssociativeMemory::findPatternNeuron(const std::vector<float>& pattern) {
    if (!brain_ || pattern.empty()) return NeuronId();
    
    // Check cache first
    size_t patternHash = std::hash<std::vector<float>>{}(pattern);
    auto it = pImpl->patternToNeuron.find(patternHash);
    if (it != pImpl->patternToNeuron.end()) {
        return it->second;
    }
    
    // Find neurons that match the pattern
    std::vector<NeuronId> candidateNeurons;
    
    // Search through brain regions for matching activation patterns
    for (auto& region : brain_->getRegions()) {
        auto neurons = region->getAllNeurons();
        for (auto* neuron : neurons) {
            if (neuron->isFiring() || 
                std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                // Create a simple activation signature
                float activation = neuron->getState().membranePotential - neuron->getState().restingPotential;
                
                // Match against pattern (simplified - could be more sophisticated)
                if (pattern.size() > 0 && activation > pattern[0] * 0.8f) {
                    candidateNeurons.push_back(neuron->getId());
                }
            }
        }
    }
    
    NeuronId result;
    if (!candidateNeurons.empty()) {
        // Use the most active candidate
        result = candidateNeurons[0];
    } else {
        // Create new neuron ID (in real implementation, would actually create neuron)
        static uint64_t nextId = 10000;
        result = NeuronId(nextId++);
    }
    
    // Cache the mapping
    pImpl->patternToNeuron[patternHash] = result;
    
    return result;
}

float NeuralAssociativeMemory::computeSimilarity(const std::vector<float>& a,
                                               const std::vector<float>& b) const {
    if (a.empty() || b.empty()) return 0.0f;
    
    // Simple dot product similarity
    float dotProduct = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    
    size_t minSize = std::min(a.size(), b.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        dotProduct += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA == 0.0f || normB == 0.0f) return 0.0f;
    
    return dotProduct / std::sqrt(normA * normB);
}

} // namespace nlm