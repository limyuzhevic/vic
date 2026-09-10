#include "NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <set>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    Brain* brain;
    
    // Association network dynamics
    std::vector<float> patternEnergies;
    std::vector<float> activationLevels;
    
    // Pattern completion state
    std::set<NeuronId> visitedNeurons;
    std::vector<std::vector<float>> activationQueue;
    
    Impl() : brain(nullptr) {}
};

NeuralAssociativeMemory::NeuralAssociativeMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , enabled_(true)
    , capacity_(1000)
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
    if (!enabled_ || patternA.empty() || patternB.empty()) {
        return;
    }
    
    // Find or create neurons for these patterns
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA == INVALID_NEURON_ID || neuronB == INVALID_NEURON_ID) {
        // Cannot create new neurons in this simplified model
        return;
    }
    
    // Check if association exists
    for (auto& assoc : associations_) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            // Update existing association
            std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + strength * 0.1f, 0.0f, 1.0f);
            return;
        }
    }
    
    // Create new association if under capacity
    if (associations_.size() < capacity_) {
        associations_.emplace_back(neuronA, neuronB, strength);
        NLM_LOG_INFO("Created association between neurons " + 
                     std::to_string(neuronA.value) + " and " + 
                     std::to_string(neuronB.value) + " with strength " + 
                     std::to_string(strength));
    }
}

void NeuralAssociativeMemory::associateFromExperience(const EpisodicMemoryItem& episode) {
    if (!enabled_) {
        return;
    }
    
    // Associate sensory state with resulting state
    if (!episode.sensoryState.empty() && !episode.resultingSensoryState.empty()) {
        associate(episode.sensoryState, episode.resultingSensoryState, 0.8f);
    }
    
    // Associate action with resulting state
    if (!episode.sensoryState.empty()) {
        // Create action-specific pattern (simplified representation)
        std::vector<float> actionPattern(episode.sensoryState.size(), 0.0f);
        actionPattern[0] = static_cast<float>(episode.action) / 10.0f; // Normalize action type
        
        associate(actionPattern, episode.resultingSensoryState, 0.7f);
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::retrieve(
    const std::vector<float>& queryPattern,
    size_t maxResults) const {
    if (!enabled_ || queryPattern.empty()) {
        return {};
    }
    
    NeuronId queryNeuron = findPatternNeuron(queryPattern);
    if (queryNeuron == INVALID_NEURON_ID) {
        return {};
    }
    
    // Find directly associated patterns
    std::vector<std::pair<float, std::vector<float>>> scored;
    
    for (const auto& assoc : associations_) {
        NeuronId otherNeuron;
        
        if (std::get<0>(assoc) == queryNeuron) {
            otherNeuron = std::get<1>(assoc);
        } else if (std::get<1>(assoc) == queryNeuron) {
            otherNeuron = std::get<0>(assoc);
        } else {
            continue;
        }
        
        // Find the pattern for this neuron
        for (const auto& pn : patternNeurons_) {
            if (pn.first == otherNeuron) {
                float strength = std::get<2>(assoc);
                scored.emplace_back(strength, pn.second);
                break;
            }
        }
    }
    
    // Sort by strength (descending)
    std::sort(scored.begin(), scored.end(),
        [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Extract top results
    std::vector<std::vector<float>> results;
    for (size_t i = 0; i < std::min(maxResults, scored.size()); ++i) {
        results.push_back(scored[i].second);
    }
    
    return results;
}

float NeuralAssociativeMemory::getAssociationStrength(
    const std::vector<float>& patternA,
    const std::vector<float>& patternB) const {
    
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA == INVALID_NEURON_ID || neuronB == INVALID_NEURON_ID) {
        return 0.0f;
    }
    
    for (const auto& assoc : associations_) {
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
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA == INVALID_NEURON_ID || neuronB == INVALID_NEURON_ID) {
        return;
    }
    
    for (auto& assoc : associations_) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + delta, 0.0f, 1.0f);
            return;
        }
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::spreadActivation(
    const std::vector<float>& cuePattern,
    size_t steps) const {
    if (!enabled_ || cuePattern.empty()) {
        return {};
    }
    
    std::vector<std::vector<float>> activated;
    std::set<NeuronId> visited;
    
    // Find initial pattern neuron
    NeuronId currentNeuron;
    for (const auto& pn : patternNeurons_) {
        if (computeSimilarity(cuePattern, pn.second) > 0.5f) {
            currentNeuron = pn.first;
            activated.push_back(pn.second);
            visited.insert(currentNeuron);
            break;
        }
    }
    
    if (currentNeuron == INVALID_NEURON_ID) {
        return {};
    }
    
    // Spread activation through associations
    for (size_t step = 0; step < steps; ++step) {
        std::vector<NeuronId> toActivate;
        
        // Find all neurons associated with currently activated neurons
        for (NeuronId neuron : visited) {
            for (const auto& assoc : associations_) {
                NeuronId neighbor;
                if (std::get<0>(assoc) == neuron) {
                    neighbor = std::get<1>(assoc);
                } else if (std::get<1>(assoc) == neuron) {
                    neighbor = std::get<0>(assoc);
                } else {
                    continue;
                }
                
                if (visited.find(neighbor) == visited.end()) {
                    toActivate.push_back(neighbor);
                    visited.insert(neighbor);
                }
            }
        }
        
        // Add newly activated neurons to results
        for (NeuronId neuron : toActivate) {
            for (const auto& pn : patternNeurons_) {
                if (pn.first == neuron) {
                    activated.push_back(pn.second);
                    break;
                }
            }
        }
        
        if (toActivate.empty()) {
            break; // No more spreading
        }
    }
    
    return activated;
}

void NeuralAssociativeMemory::storePattern(const std::vector<float>& pattern, NeuronId neuronId) {
    if (patternNeurons_.size() >= capacity_) {
        // Remove oldest patterns to make room
        patternNeurons_.erase(patternNeurons_.begin());
    }
    
    patternNeurons_.emplace_back(neuronId, pattern);
}

float NeuralAssociativeMemory::computeSimilarity(const std::vector<float>& a,
                                                 const std::vector<float>& b) const {
    if (a.empty() || b.empty() || a.size() != b.size()) {
        return 0.0f;
    }
    
    // Pearson correlation coefficient as similarity measure
    float sumA = std::accumulate(a.begin(), a.end(), 0.0f);
    float sumB = std::accumulate(b.begin(), b.end(), 0.0f);
    float sumAB = 0.0f;
    float sumASq = 0.0f;
    float sumBSq = 0.0f;
    
    for (size_t i = 0; i < a.size(); ++i) {
        sumAB += a[i] * b[i];
        sumASq += a[i] * a[i];
        sumBSq += b[i] * b[i];
    }
    
    float numerator = sumAB - (sumA * sumB) / a.size();
    float denominator = std::sqrt((sumASq - sumA * sumA / a.size()) * 
                                  (sumBSq - sumB * sumB / a.size()));
    
    if (denominator == 0.0f) {
        return 0.0f;
    }
    
    return numerator / denominator;
}

NeuronId NeuralAssociativeMemory::findPatternNeuron(const std::vector<float>& pattern) const {
    float bestSimilarity = 0.0f;
    NeuronId bestNeuron = INVALID_NEURON_ID;
    
    for (const auto& pn : patternNeurons_) {
        float sim = computeSimilarity(pattern, pn.second);
        if (sim > bestSimilarity && sim > 0.3f) { // Threshold for good match
            bestSimilarity = sim;
            bestNeuron = pn.first;
        }
    }
    
    return bestNeuron;
}

void NeuralAssociativeMemory::clear() {
    patternNeurons_.clear();
    associations_.clear();
    pImpl->visitedNeurons.clear();
    pImpl->activationQueue.clear();
}

const std::vector<std::pair<NeuronId, std::vector<float>>>& NeuralAssociativeMemory::getPatternNeurons() const {
    return patternNeurons_;
}

const std::vector<std::tuple<NeuronId, NeuronId, float>>& NeuralAssociativeMemory::getAssociations() const {
    return associations_;
}

size_t NeuralAssociativeMemory::getPatternCount() const {
    return patternNeurons_.size();
}

size_t NeuralAssociativeMemory::getAssociationCount() const {
    return associations_.size();
}

} // namespace nlm