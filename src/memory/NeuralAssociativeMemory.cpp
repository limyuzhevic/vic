#include "NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <unordered_map>
#include <set>
#include <queue>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    // Maps from pattern neurons to related patterns
    std::unordered_map<NeuronId, std::vector<NeuronId>> patternAssociations;
    
    // Strength of associations between patterns
    std::unordered_map<std::pair<NeuronId, NeuronId>, float> associationStrengths;
    
    // Association history for incremental learning
    std::unordered_map<std::pair<NeuronId, NeuronId>, std::vector<float>> associationHistory;
    
    Impl() {}
};

NeuralAssociativeMemory::NeuralAssociativeMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
{
}

NeuralAssociativeMemory::~NeuralAssociativeMemory() = default;

void NeuralAssociativeMemory::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize NeuralAssociativeMemory with null brain");
        return;
    }
    brain_ = brain;
    NLM_LOG_INFO("NeuralAssociativeMemory initialized");
}

void NeuralAssociativeMemory::associate(const std::vector<float>& patternA,
                                      const std::vector<float>& patternB,
                                      float strength) {
    if (!brain_) return;
    
    if (patternA.empty() || patternB.empty()) return;
    
    // Find or create neurons for these patterns
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA.isValid() && neuronB.isValid()) {
        // Create association
        pImpl->patternAssociations[neuronA].push_back(neuronB);
        pImpl->patternAssociations[neuronB].push_back(neuronA);
        
        // Set or update strength
        if (neuronA < neuronB) {
            pImpl->associationStrengths[{neuronA, neuronB}] = strength;
        } else {
            pImpl->associationStrengths[{neuronB, neuronA}] = strength;
        }
        
        // Record in history
        pImpl->associationHistory[{neuronA, neuronB}].push_back(strength);
        
        NLM_LOG_DEBUG("Created association between neurons " + std::to_string(neuronA.value) + 
                     " and " + std::to_string(neuronB.value) + " with strength " + 
                     std::to_string(strength));
    }
}

void NeuralAssociativeMemory::associateFromExperience(const EpisodicMemoryItem& episode) {
    if (!brain_) return;
    
    // Create associations based on co-occurring neurons in the experience
    std::set<NeuronId> activeNeurons(episode.activeNeurons.begin(), episode.activeNeurons.end());
    
    // Find all unique pairs of co-occurring neurons
    std::vector<NeuronId> neuronList(activeNeurons.begin(), activeNeurons.end());
    for (size_t i = 0; i < neuronList.size(); ++i) {
        for (size_t j = i + 1; j < neuronList.size(); ++j) {
            float strength = (episode.neuronActivations.size() > i && 
                           episode.neuronActivations.size() > j)
                           ? (episode.neuronActivations[i] + episode.neuronActivations[j]) * 0.5f
                           : 1.0f;
            
            associate({neuronList[i].value}, {neuronList[j].value}, strength);
        }
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::retrieve(
    const std::vector<float>& queryPattern,
    size_t maxResults) const {
    std::vector<std::vector<float>> results;
    
    if (!brain_ || queryPattern.empty()) return results;
    
    // Find neuron representing the query pattern
    NeuronId queryNeuron = findPatternNeuron(queryPattern);
    
    if (!queryNeuron.isValid()) {
        NLM_LOG_DEBUG("No neuron found for query pattern");
        return results;
    }
    
    // Get associated neurons
    auto it = pImpl->patternAssociations.find(queryNeuron);
    if (it == pImpl->patternAssociations.end()) return results;
    
    // For each associated neuron, retrieve its pattern
    for (NeuronId associatedNeuron : it->second) {
        // Find pattern for this neuron
        auto patternIt = std::find_if(
            pImpl->patternNeurons_.begin(), pImpl->patternNeurons_.end(),
            [associatedNeuron](const auto& entry) { return entry.first == associatedNeuron; }
        );
        
        if (patternIt != pImpl->patternNeurons_.end()) {
            results.push_back(patternIt->second);
            if (results.size() >= maxResults) break;
        }
    }
    
    return results;
}

float NeuralAssociativeMemory::getAssociationStrength(
    const std::vector<float>& patternA,
    const std::vector<float>& patternB) const {
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (!neuronA.isValid() || !neuronB.isValid()) return 0.0f;
    
    std::pair<NeuronId, NeuronId> key1 = std::minmax(neuronA, neuronB);
    auto it = pImpl->associationStrengths.find(key1);
    if (it != pImpl->associationStrengths.end()) {
        return it->second;
    }
    
    return 0.0f;
}

void NeuralAssociativeMemory::updateAssociation(
    const std::vector<float>& patternA,
    const std::vector<float>& patternB,
    float delta) {
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (!neuronA.isValid() || !neuronB.isValid()) return;
    
    std::pair<NeuronId, NeuronId> key1 = std::minmax(neuronA, neuronB);
    auto strengthIt = pImpl->associationStrengths.find(key1);
    
    if (strengthIt != pImpl->associationStrengths.end()) {
        strengthIt->second = std::clamp(strengthIt->second + delta, 0.0f, 1.0f);
        
        // Record in history
        pImpl->associationHistory[key1].push_back(strengthIt->second);
        
        NLM_LOG_DEBUG("Updated association strength between neurons " + 
                     std::to_string(key1.first.value) + " and " + 
                     std::to_string(key1.second.value) + " to " + 
                     std::to_string(strengthIt->second));
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::spreadActivation(
    const std::vector<float>& cuePattern,
    size_t steps) const {
    std::vector<std::vector<float>> results;
    
    if (!brain_ || cuePattern.empty() || steps == 0) return results;
    
    // Queue for BFS traversal
    std::queue<NeuronId> queue;
    std::set<NeuronId> visited;
    
    NeuronId startNeuron = findPatternNeuron(cuePattern);
    if (!startNeuron.isValid()) return results;
    
    queue.push(startNeuron);
    visited.insert(startNeuron);
    
    for (size_t step = 0; step < steps && !queue.empty(); ++step) {
        size_t levelSize = queue.size();
        
        for (size_t i = 0; i < levelSize; ++i) {
            NeuronId current = queue.front();
            queue.pop();
            
            // Get current neuron pattern
            auto patternIt = std::find_if(
                pImpl->patternNeurons_.begin(), pImpl->patternNeurons_.end(),
                [current](const auto& entry) { return entry.first == current; }
            );
            
            if (patternIt == pImpl->patternNeurons_.end()) continue;
            
            results.push_back(patternIt->second);
            
            // Add associated neurons to queue
            auto assocIt = pImpl->patternAssociations.find(current);
            if (assocIt != pImpl->patternAssociations.end()) {
                for (NeuronId associated : assocIt->second) {
                    if (visited.find(associated) == visited.end()) {
                        visited.insert(associated);
                        queue.push(associated);
                    }
                }
            }
        }
    }
    
    return results;
}

void NeuralAssociativeMemory::clear() {
    pImpl->patternAssociations.clear();
    pImpl->associationStrengths.clear();
    pImpl->associationHistory.clear();
    pImpl->patternNeurons_.clear();
    
    NLM_LOG_INFO("Cleared all associative memory");
}

NeuronId NeuralAssociativeMemory::findPatternNeuron(const std::vector<float>& pattern) {
    if (pattern.empty() || !brain_) return NeuronId();
    
    float minDistance = std::numeric_limits<float>::max();
    NeuronId bestMatch;
    
    for (const auto& entry : pImpl->patternNeurons_) {
        float distance = computeSimilarity(pattern, entry.second);
        if (distance < minDistance) {
            minDistance = distance;
            bestMatch = entry.first;
        }
    }
    
    // Create new neuron if pattern not found or too dissimilar
    if (!bestMatch.isValid() || minDistance > 0.5f) {
        NeuronId newNeuron = NeuronId(static_cast<uint64_t>(pImpl->patternNeurons_.size() + 10000));
        pImpl->patternNeurons_.push_back({newNeuron, pattern});
        bestMatch = newNeuron;
        
        NLM_LOG_DEBUG("Created new pattern neuron " + std::to_string(newNeuron.value) + 
                     " for pattern of size " + std::to_string(pattern.size()));
    }
    
    return bestMatch;
}

float NeuralAssociativeMemory::computeSimilarity(
    const std::vector<float>& a,
    const std::vector<float>& b) const {
    if (a.empty() || b.empty()) return 0.0f;
    
    size_t minSize = std::min(a.size(), b.size());
    float dotProduct = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    
    for (size_t i = 0; i < minSize; ++i) {
        dotProduct += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA == 0.0f || normB == 0.0f) return 0.0f;
    
    return dotProduct / std::sqrt(normA * normB);
}