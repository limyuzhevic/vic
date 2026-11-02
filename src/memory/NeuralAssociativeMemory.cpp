#include "NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Neuron.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_set>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    Brain* brain;
    
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
    
    // Get or create neurons for these patterns
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA.index() == 0 || neuronB.index() == 0) return;
    
    // Check if association already exists
    for (const auto& assoc : associations_) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            // Update existing association strength
            float& currentStrength = std::get<2>(assoc);
            currentStrength = std::max(currentStrength, strength);
            NLM_LOG_INFO("Updated association strength between neurons " +
                        std::to_string(neuronA.index()) + " and " +
                        std::to_string(neuronB.index()) + " to " +
                        std::to_string(currentStrength));
            return;
        }
    }
    
    // Create new association
    associations_.emplace_back(neuronA, neuronB, strength);
    NLM_LOG_INFO("Created new association between neurons " +
                 std::to_string(neuronA.index()) + " and " +
                 std::to_string(neuronB.index()) + " with strength " + std::to_string(strength));
}

void NeuralAssociativeMemory::associateFromExperience(const EpisodicMemoryItem& episode) {
    if (episode.activeNeurons.size() < 2 || !brain_) return;
    
    // Create patterns from neural activity
    std::vector<float> patternA;
    std::vector<float> patternB;
    
    // For now, create two patterns from the episode's active neurons
    // In a more sophisticated implementation, this would use actual neural activity patterns
    size_t halfSize = episode.activeNeurons.size() / 2;
    if (halfSize < 2) return;
    
    for (size_t i = 0; i < halfSize; ++i) {
        patternA.push_back(1.0f);  // Simplified: all neurons active in pattern A
        patternB.push_back(1.0f);  // Simplified: all neurons active in pattern B
    }
    
    // Associate the patterns
    float strength = episode.reward;  // Use reward as association strength
    associate(patternA, patternB, strength);
}

std::vector<std::vector<float>> NeuralAssociativeMemory::retrieve(
    const std::vector<float>& queryPattern, size_t maxResults) const {
    std::vector<std::vector<float>> results;
    
    if (queryPattern.empty() || !brain_) return results;
    
    // Get or create neuron for query pattern
    NeuronId queryNeuron = findPatternNeuron(queryPattern);
    if (queryNeuron.index() == 0) return results;
    
    // Find associated patterns
    std::vector<size_t> foundIndices;
    
    for (size_t i = 0; i < associations_.size(); ++i) {
        const auto& assoc = associations_[i];
        NeuronId neuronA = std::get<0>(assoc);
        NeuronId neuronB = std::get<1>(assoc);
        float strength = std::get<2>(assoc);
        
        // Check if either neuron is the query neuron
        if (neuronA == queryNeuron) {
            // Find pattern B for neuronA
            auto it = std::find_if(patternNeurons_.begin(), patternNeurons_.end(),
                                 [&neuronB](const auto& p) { return p.first == neuronB; });
            if (it != patternNeurons_.end()) {
                results.push_back(it->second);
                foundIndices.push_back(i);
            }
        } else if (neuronB == queryNeuron) {
            // Find pattern A for neuronB
            auto it = std::find_if(patternNeurons_.begin(), patternNeurons_.end(),
                                 [&neuronA](const auto& p) { return p.first == neuronA; });
            if (it != patternNeurons_.end()) {
                results.push_back(it->second);
                foundIndices.push_back(i);
            }
        }
    }
    
    // Sort by strength (strongest associations first)
    std::vector<std::pair<size_t, float>> strengthWithIndex;
    for (size_t idx : foundIndices) {
        if (idx < associations_.size()) {
            strengthWithIndex.emplace_back(idx, std::get<2>(associations_[idx]));
        }
    }
    
    std::sort(strengthWithIndex.begin(), strengthWithIndex.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Build results in sorted order
    std::vector<std::vector<float>> sortedResults;
    for (size_t i = 0; i < std::min(maxResults, strengthWithIndex.size()); ++i) {
        size_t assocIdx = strengthWithIndex[i].first;
        if (assocIdx < associations_.size()) {
            // Add the other pattern
            const auto& assoc = associations_[assocIdx];
            NeuronId neuronA = std::get<0>(assoc);
            NeuronId neuronB = std::get<1>(assoc);
            
            if (neuronA == queryNeuron) {
                auto it = std::find_if(patternNeurons_.begin(), patternNeurons_.end(),
                                     [&neuronB](const auto& p) { return p.first == neuronB; });
                if (it != patternNeurons_.end()) {
                    sortedResults.push_back(it->second);
                }
            } else {
                auto it = std::find_if(patternNeurons_.begin(), patternNeurons_.end(),
                                     [&neuronA](const auto& p) { return p.first == neuronA; });
                if (it != patternNeurons_.end()) {
                    sortedResults.push_back(it->second);
                }
            }
        }
    }
    
    return sortedResults;
}

float NeuralAssociativeMemory::getAssociationStrength(
    const std::vector<float>& patternA,
    const std::vector<float>& patternB) const {
    if (!brain_ || patternA.empty() || patternB.empty()) return 0.0f;
    
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA.index() == 0 || neuronB.index() == 0) return 0.0f;
    
    // Check for both directions
    for (const auto& assoc : associations_) {
        NeuronId a = std::get<0>(assoc);
        NeuronId b = std::get<1>(assoc);
        float strength = std::get<2>(assoc);
        
        if ((a == neuronA && b == neuronB) || (a == neuronB && b == neuronA)) {
            return strength;
        }
    }
    
    return 0.0f;
}

void NeuralAssociativeMemory::updateAssociation(
    const std::vector<float>& patternA,
    const std::vector<float>& patternB,
    float delta) {
    if (!brain_ || patternA.empty() || patternB.empty()) return;
    
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA.index() == 0 || neuronB.index() == 0) return;
    
    // Find existing association
    for (auto& assoc : associations_) {
        NeuronId a = std::get<0>(assoc);
        NeuronId b = std::get<1>(assoc);
        
        if ((a == neuronA && b == neuronB) || (a == neuronB && b == neuronA)) {
            // Update strength
            float& currentStrength = std::get<2>(assoc);
            currentStrength += delta;
            currentStrength = std::max(0.0f, currentStrength);  // Prevent negative
            NLM_LOG_INFO("Updated association strength to " + std::to_string(currentStrength));
            return;
        }
    }
    
    // Create new association if significant delta
    if (std::abs(delta) > 0.01f) {
        associate(patternA, patternB, std::abs(delta));
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::spreadActivation(
    const std::vector<float>& cuePattern, size_t steps) const {
    std::vector<std::vector<float>> results;
    
    if (cuePattern.empty() || !brain_) return results;
    
    // Get neuron for cue pattern
    NeuronId cueNeuron = findPatternNeuron(cuePattern);
    if (cueNeuron.index() == 0) return results;
    
    // BFS to find associated patterns
    std::unordered_set<NeuronId> visited;
    std::vector<NeuronId> currentLevel;
    currentLevel.push_back(cueNeuron);
    visited.insert(cueNeuron);
    
    for (size_t step = 0; step < steps; ++step) {
        std::vector<NeuronId> nextLevel;
        
        // Find all neurons associated with current level
        for (NeuronId currentNeuron : currentLevel) {
            for (const auto& assoc : associations_) {
                NeuronId a = std::get<0>(assoc);
                NeuronId b = std::get<1>(assoc);
                
                if (a == currentNeuron && !visited.count(b)) {
                    nextLevel.push_back(b);
                    visited.insert(b);
                } else if (b == currentNeuron && !visited.count(a)) {
                    nextLevel.push_back(a);
                    visited.insert(a);
                }
            }
        }
        
        // Add found neurons to results
        for (NeuronId neuron : nextLevel) {
            auto it = std::find_if(patternNeurons_.begin(), patternNeurons_.end(),
                                 [&neuron](const auto& p) { return p.first == neuron; });
            if (it != patternNeurons_.end()) {
                results.push_back(it->second);
            }
        }
        
        currentLevel = nextLevel;
        if (currentLevel.empty()) break;
    }
    
    return results;
}

void NeuralAssociativeMemory::clear() {
    associations_.clear();
    patternNeurons_.clear();
    NLM_LOG_INFO("NeuralAssociativeMemory cleared");
}

NeuronId NeuralAssociativeMemory::findPatternNeuron(const std::vector<float>& pattern) {
    if (pattern.empty() || !brain_) return NeuronId(0);
    
    // Check if pattern already exists in our database
    auto it = std::find_if(patternNeurons_.begin(), patternNeurons_.end(),
                         [&pattern](const auto& p) { return p.second == pattern; });
    if (it != patternNeurons_.end()) {
        return it->first;
    }
    
    // Create new neuron for this pattern
    // Find available neuron ID
    uint64_t maxId = 0;
    for (const auto& region : brain_->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto& neuron : pop->getNeurons()) {
                maxId = std::max(maxId, neuron->getId().index());
            }
        }
    }
    
    NeuronId newNeuron(maxId + 1);
    
    // Add to pattern database
    patternNeurons_.emplace_back(newNeuron, pattern);
    
    NLM_LOG_INFO("Created new pattern neuron " + std::to_string(newNeuron.index()));
    return newNeuron;
}

float NeuralAssociativeMemory::computeSimilarity(const std::vector<float>& a,
                                                 const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        sum += std::abs(a[i] - b[i]);
    }
    
    return 1.0f - (sum / static_cast<float>(a.size()));  // Normalize to [0, 1]
}

} // namespace nlm