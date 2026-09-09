#include "NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <unordered_set>
#include <queue>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    Brain* brain;
    std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons;
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations;
    std::unordered_map<uint64_t, NeuronId> patternHashToNeuron;
    std::unordered_map<uint64_t, std::vector<NeuronId>> reverseAssociations;
    
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
    if (!brain_ || patternA.empty() || patternB.empty()) return;
    
    // Find or create neurons for these patterns
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA.isValid() && neuronB.isValid()) {
        // Check if association already exists
        for (const auto& assoc : pImpl->associations) {
            if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
                (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
                // Update existing association
                std::get<2>(const_cast<std::tuple<NeuronId, NeuronId, float>&>(assoc)) = strength;
                return;
            }
        }
        
        // Create new association
        pImpl->associations.emplace_back(neuronA, neuronB, strength);
        
        // Update reverse associations for quick lookup
        pImpl->reverseAssociations[neuronA.getValue()].push_back(neuronB.getValue());
        pImpl->reverseAssociations[neuronB.getValue()].push_back(neuronA.getValue());
        
        NLM_LOG_INFO("Created association between neurons " + 
                    std::to_string(neuronA.getValue()) + " and " +
                    std::to_string(neuronB.getValue()) + " with strength " + 
                    std::to_string(strength));
    }
}

void NeuralAssociativeMemory::associateFromExperience(const EpisodicMemoryItem& episode) {
    if (!brain_ || episode.sensoryState.empty()) return;
    
    // Convert episodic content to neural patterns
    std::vector<float> patternA = episode.sensoryState;
    std::vector<float> patternB;
    
    // Use reward and action as secondary pattern
    patternB.push_back(episode.reward);
    for (float activation : episode.neuronActivations) {
        patternB.push_back(activation);
    }
    
    // Create association with relevance based on episode age
    float strength = 1.0f - (episode.age / 1000.0f); // Older episodes = weaker associations
    associate(patternA, patternB, strength);
}

std::vector<std::vector<float>> NeuralAssociativeMemory::retrieve(const std::vector<float>& queryPattern,
                                                                    size_t maxResults) const {
    std::vector<std::vector<float>> results;
    
    if (!brain_ || patternNeurons_.empty()) return results;
    
    // Find the most similar pattern neuron
    NeuronId bestNeuron = findPatternNeuron(queryPattern);
    
    if (!bestNeuron.isValid()) return results;
    
    // Retrieve all patterns associated with this neuron
    std::vector<float> pattern = getPatternFromNeuron(bestNeuron);
    
    // Find associated neurons
    auto it = pImpl->reverseAssociations.find(bestNeuron.getValue());
    if (it == pImpl->reverseAssociations.end()) return results;
    
    size_t count = 0;
    for (NeuronId assocNeuronId : it->second) {
        // Get associated pattern
        std::vector<float> assocPattern = getPatternFromNeuron(assocNeuronId);
        
        // Filter results by similarity threshold
        float similarity = computeSimilarity(queryPattern, assocPattern);
        if (similarity > 0.3f) {
            results.push_back(assocPattern);
            count++;
            if (count >= maxResults) break;
        }
    }
    
    return results;
}

float NeuralAssociativeMemory::getAssociationStrength(const std::vector<float>& patternA,
                                                      const std::vector<float>& patternB) const {
    if (!brain_ || patternA.empty() || patternB.empty()) return 0.0f;
    
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (!neuronA.isValid() || !neuronB.isValid()) return 0.0f;
    
    // Check for association
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
    if (!brain_ || patternA.empty() || patternB.empty()) return;
    
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (!neuronA.isValid() || !neuronB.isValid()) return;
    
    // Update association strength
    for (auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + delta, 0.0f, 1.0f);
            
            // Update reverse associations
            auto itA = pImpl->reverseAssociations.find(neuronA.getValue());
            auto itB = pImpl->reverseAssociations.find(neuronB.getValue());
            
            if (std::get<2>(assoc) <= 0.01f) {
                // Remove weak associations
                if (itA != pImpl->reverseAssociations.end()) {
                    auto& listA = itA->second;
                    listA.erase(std::remove(listA.begin(), listA.end(), neuronB.getValue()), listA.end());
                }
                if (itB != pImpl->reverseAssociations.end()) {
                    auto& listB = itB->second;
                    listB.erase(std::remove(listB.begin(), listB.end(), neuronA.getValue()), listB.end());
                }
            }
            
            return;
        }
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::spreadActivation(
    const std::vector<float>& cuePattern, size_t steps) const {
    std::vector<std::vector<float>> results;
    
    if (!brain_ || steps == 0) return results;
    
    // Find starting neuron
    NeuronId startNeuron = findPatternNeuron(cuePattern);
    if (!startNeuron.isValid()) return results;
    
    // BFS to find activated patterns through association chain
    std::queue<std::pair<NeuronId, size_t>> queue;  // (neuron, depth)
    std::unordered_set<uint64_t> visited;  // Prevent cycles
    
    queue.push({startNeuron, 0});
    visited.insert(startNeuron.getValue());
    
    while (!queue.empty()) {
        auto current = queue.front();
        queue.pop();
        
        // Get all patterns associated with this neuron
        auto it = pImpl->reverseAssociations.find(current.first.getValue());
        if (it == pImpl->reverseAssociations.end()) continue;
        
        for (NeuronId assocNeuronId : it->second) {
            if (visited.find(assocNeuronId.getValue()) != visited.end()) continue;
            
            if (current.second + 1 <= steps) {
                visited.insert(assocNeuronId.getValue());
                queue.push({assocNeuronId, current.second + 1});
                
                // Store retrieved pattern
                std::vector<float> pattern = getPatternFromNeuron(assocNeuronId);
                float similarity = computeSimilarity(cuePattern, pattern);
                
                if (similarity > 0.2f) {
                    results.push_back(pattern);
                }
            }
        }
    }
    
    return results;
}

void NeuralAssociativeMemory::clear() {
    pImpl->patternNeurons.clear();
    pImpl->associations.clear();
    pImpl->patternHashToNeuron.clear();
    pImpl->reverseAssociations.clear();
    NLM_LOG_INFO("NeuralAssociativeMemory cleared");
}

NeuronId NeuralAssociativeMemory::findPatternNeuron(const std::vector<float>& pattern) {
    if (!brain_ || pattern.empty()) return NeuronId();
    
    // Create hash for pattern
    uint64_t hash = 0;
    for (float val : pattern) {
        hash = hash * 31 + static_cast<uint64_t>(val * 1000);  // Scale to integer
    }
    
    // Check if pattern already exists
    auto it = pImpl->patternHashToNeuron.find(hash);
    if (it != pImpl->patternHashToNeuron.end()) {
        return it->second;
    }
    
    // Create new neuron for this pattern
    RegionId region(1);  // Default to first region
    NeuronId newNeuron = brain_->addRegion("AssociativeMemory");
    
    if (newNeuron.isValid()) {
        // Store pattern in associative memory
        pImpl->patternNeurons.emplace_back(newNeuron, pattern);
        pImpl->patternHashToNeuron[hash] = newNeuron;
        
        // Initialize neuron with pattern activity
        storePatternInNeuron(newNeuron, pattern);
        
        NLM_LOG_INFO("Created new pattern neuron with ID " + std::to_string(newNeuron.getValue()) + 
                    " for pattern with " + std::to_string(pattern.size()) + " values");
    }
    
    return newNeuron;
}

float NeuralAssociativeMemory::computeSimilarity(const std::vector<float>& a,
                                                const std::vector<float>& b) const {
    if (a.empty() || b.empty()) return 0.0f;
    
    size_t minSize = std::min(a.size(), b.size());
    if (minSize == 0) return 0.0f;
    
    float sumSqDiff = 0.0f;
    for (size_t i = 0; i < minSize; ++i) {
        float diff = a[i] - b[i];
        sumSqDiff += diff * diff;
    }
    
    // Cosine similarity
    float normA = 0.0f, normB = 0.0f;
    for (size_t i = 0; i < minSize; ++i) {
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA <= 0.0f || normB <= 0.0f) return 0.0f;
    
    float cosine = 1.0f - std::sqrt(sumSqDiff / (normA * normB));
    return std::clamp(cosine, 0.0f, 1.0f);
}

void NeuralAssociativeMemory::storePatternInNeuron(NeuronId neuron, const std::vector<float>& pattern) {
    if (!brain_ || !neuron.isValid() || pattern.empty()) return;
    
    // Distribute pattern across available neurons
    size_t neuronsInRegion = 100; // Approximate
    size_t neuronsNeeded = std::min(pattern.size(), neuronsInRegion);
    
    for (size_t i = 0; i < neuronsNeeded; ++i) {
        float activation = pattern[i % pattern.size()] * 2.0f - 1.0f;  // Normalize to [-1, 1]
        storeToNeuron(neuron, activation);
    }
}

void NeuralAssociativeMemory::storeToNeuron(NeuronId neuron, float activation) {
    if (!brain_ || !neuron.isValid()) return;
    
    // Find the actual region and population for this neuron
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (Neuron* n : pop->getNeurons()) {
                if (n->getId() == neuron) {
                    n->injectCurrent(activation * 5.0f);  // Scale factor
                    return;
                }
            }
        }
    }
}

std::vector<float> NeuralAssociativeMemory::getPatternFromNeuron(NeuronId neuron) const {
    std::vector<float> pattern;
    
    if (!brain_ || !neuron.isValid()) return pattern;
    
    // Get neuron state to reconstruct pattern
    for (const auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (Neuron* n : pop->getNeurons()) {
                if (n->getId() == neuron) {
                    // Use membrane potential to estimate activation
                    float normPot = (n->getState().membranePotential + 75.0f) / 125.0f;
                    pattern.push_back(std::clamp(normPot, 0.0f, 1.0f));
                    
                    // Add a few more neurons to get longer pattern
                    if (pattern.size() < 10) {
                        pattern.push_back(std::abs(normPot - 0.5f));
                    }
                }
            }
        }
    }
    
    // Ensure pattern has reasonable size
    if (pattern.empty()) {
        pattern = std::vector<float>(5, 0.5f); // Default pattern
    }
    
    return pattern;
}

} // namespace nlm