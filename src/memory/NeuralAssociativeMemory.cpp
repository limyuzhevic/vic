// NeuralAssociativeMemory Implementation

#include "NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_map>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

NeuralAssociativeMemory::NeuralAssociativeMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , maxPatterns_(1000)
    , decayRate_(0.001f)
    , activeTraces_(0)
    , totalAssociationStrength_(0.0f)
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
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA != INVALID_NEURON_ID && neuronB != INVALID_NEURON_ID) {
        // Check if association exists
        for (auto& assoc : associations_) {
            if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
                (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
                // Update strength
                std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + strength * 0.1f, 0.0f, 1.0f);
                return;
            }
        }
        
        // Create new association
        associations_.emplace_back(neuronA, neuronB, strength);
    }
}

void NeuralAssociativeMemory::storePattern(const std::vector<float>& pattern, float strength) {
    NeuronId neuron = findPatternNeuron(pattern);
    
    if (neuron != INVALID_NEURON_ID) {
        // Update existing pattern with new strength
        for (auto& [id, pat] : patternNeurons_) {
            if (id == neuron) {
                // Blend strengths
                for (size_t i = 0; i < std::min(pat.size(), pattern.size()); ++i) {
                    pat[i] = pat[i] * 0.5f + pattern[i] * strength * 0.5f;
                }
                break;
            }
        }
        
        // Update association count
        activeTraces_++;
        
        // Update total association strength
        totalAssociationStrength_ += strength;
    }
}

std::vector<float> NeuralAssociativeMemory::recallPattern(const std::vector<float>& query) const {
    std::vector<std::pair<float, std::vector<float>>> scored;
    
    // Find all similar patterns
    for (const auto& [neuronId, pattern] : patternNeurons_) {
        float sim = computeSimilarity(query, pattern);
        if (sim > 0.3f) {
            scored.emplace_back(sim, pattern);
        }
    }
    
    // Sort by similarity (descending)
    std::sort(scored.begin(), scored.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    if (scored.empty()) {
        return {};
    }
    
    // Return the best match
    return scored[0].second;
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
}

std::vector<std::vector<float>> NeuralAssociativeMemory::retrieve(
    const std::vector<float>& queryPattern,
    size_t maxResults) const {
    
    NeuronId queryNeuron;
    float bestSim = 0.0f;
    
    // Find best matching pattern neuron
    for (size_t i = 0; i < patternNeurons_.size(); ++i) {
        float sim = computeSimilarity(queryPattern, patternNeurons_[i].second);
        if (sim > bestSim) {
            bestSim = sim;
            queryNeuron = patternNeurons_[i].first;
        }
    }
    
    if (bestSim < 0.3f) {
        return {};
    }
    
    // Find associated patterns
    std::vector<std::vector<float>> results;
    std::vector<std::pair<float, std::vector<float>>> scored;
    
    for (const auto& assoc : associations_) {
        NeuronId otherNeuron;
        
        if (std::get<0>(assoc) == queryNeuron) {
            otherNeuron = std::get<1>(assoc);
        } else if (std::get<1>(assoc) == queryNeuron) {
            otherNeuron = std::get<0>(assoc);
        }
        
        if (otherNeuron != INVALID_NEURON_ID) {
            for (const auto& pn : patternNeurons_) {
                if (pn.first == otherNeuron) {
                    float strength = std::get<2>(assoc);
                    scored.emplace_back(strength, pn.second);
                    break;
                }
            }
        }
    }
    
    // Sort by strength
    std::sort(scored.begin(), scored.end(),
        [](const auto& a, const auto& b) { return a.first > b.first; });
    
    for (size_t i = 0; i < std::min(maxResults, scored.size()); ++i) {
        results.push_back(scored[i].second);
    }
    
    return results;
}

float NeuralAssociativeMemory::getAssociationStrength(
    const std::vector<float>& patternA,
    const std::vector<float>& patternB) const {
    
    NeuronId neuronA, neuronB;
    float bestSimA = 0.0f, bestSimB = 0.0f;
    
    for (const auto& pn : patternNeurons_) {
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
    
    for (const auto& assoc : associations_) {
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
        return activated;
    }
    
    // Spread through association network
    for (size_t step = 0; step < steps; ++step) {
        std::vector<NeuronId> nextNeurons;
        
        for (const auto& assoc : associations_) {
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
                for (const auto& pn : patternNeurons_) {
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

void NeuralAssociativeMemory::clear() {
    patternNeurons_.clear();
    associations_.clear();
    pImpl->patternLastUsed.clear();
    activeTraces_ = 0;
    totalAssociationStrength_ = 0.0f;
}

void NeuralAssociativeMemory::clearWeakPatterns(float threshold) {
    std::vector<size_t> toRemove;
    
    // Find patterns with low activation (simplified - would need better tracking)
    // For now, just enforce max capacity
    while (patternNeurons_.size() > maxPatterns_ * 0.8f) {
        patternNeurons_.erase(patternNeurons_.begin());
    }
    
    // Also clear weak associations
    std::vector<size_t> assocToRemove;
    for (size_t i = 0; i < associations_.size(); ++i) {
        if (std::get<2>(associations_[i]) < threshold) {
            assocToRemove.push_back(i);
        }
    }
    
    // Remove weak associations (in reverse order)
    for (auto it = assocToRemove.rbegin(); it != assocToRemove.rend(); ++it) {
        associations_.erase(associations_.begin() + *it);
    }
}

void NeuralAssociativeMemory::decayPatterns(float decayRate) {
    for (auto& [neuronId, pattern] : patternNeurons_) {
        // Decay pattern strength (simplified implementation)
        for (auto& value : pattern) {
            value *= (1.0f - decayRate);
        }
    }
    
    for (auto& [a, b, strength] : associations_) {
        strength *= (1.0f - decayRate);
    }
}

void NeuralAssociativeMemory::strengthenUsedPatterns(float factor) {
    auto currentTime = static_cast<SimulationStep>(pImpl->brain ? pImpl->brain->getTotalSpikeCount() : 0);
    
    for (auto& [neuronId, pattern] : patternNeurons_) {
        auto it = pImpl->patternLastUsed.find(neuronId);
        if (it != pImpl->patternLastUsed.end()) {
            // Check if recently used
            if (currentTime - it->second < 100) {  // Recently used within 100 steps
                for (auto& value : pattern) {
                    value = std::min(1.0f, value * factor);
                }
            }
        }
        pImpl->patternLastUsed[neuronId] = currentTime;
    }
}

NeuronId NeuralAssociativeMemory::findPatternNeuron(const std::vector<float>& pattern) {
    // Check if we already have a similar pattern
    for (const auto& pn : patternNeurons_) {
        if (computeSimilarity(pattern, pn.second) > 0.8f) {
            return pn.first;
        }
    }
    
    // Check capacity before creating new pattern
    if (patternNeurons_.size() >= maxPatterns_) {
        // Find and remove weakest pattern
        size_t weakestIdx = 0;
        float weakestStrength = 1.0f;
        
        for (size_t i = 0; i < patternNeurons_.size(); ++i) {
            // Estimate strength from pattern values
            float strength = 0.0f;
            for (float val : patternNeurons_[i].second) {
                strength += std::abs(val);
            }
            strength /= patternNeurons_[i].second.size();
            
            if (strength < weakestStrength) {
                weakestStrength = strength;
                weakestIdx = i;
            }
        }
        
        patternNeurons_.erase(patternNeurons_.begin() + weakestIdx);
        // Also remove any associations involving this neuron
        std::vector<size_t> assocToRemove;
        for (size_t i = 0; i < associations_.size(); ++i) {
            if (std::get<0>(associations_[i]) == patternNeurons_[weakestIdx].first ||
                std::get<1>(associations_[i]) == patternNeurons_[weakestIdx].first) {
                assocToRemove.push_back(i);
            }
        }
        for (auto it = assocToRemove.rbegin(); it != assocToRemove.rend(); ++it) {
            associations_.erase(associations_.begin() + *it);
        }
    }
    
    // Create new pattern neuron
    NeuronId newId(patternNeurons_.size() + 30000);
    patternNeurons_.push_back({newId, pattern});
    pImpl->patternLastUsed[newId] = 0;
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
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(normA) * std::sqrt(normB));
}

} // namespace nlm