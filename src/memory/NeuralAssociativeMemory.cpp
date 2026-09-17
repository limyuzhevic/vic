#include "NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_set>
#include <queue>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    Brain* brain;
    
    // Spreading activation state
    std::vector<float> nodeActivations;
    std::vector<float> associationStrengths;
    std::vector<std::unordered_set<NeuronId>> activationSets;
    
    // Co-activation history for Hebbian learning
    std::vector<std::vector<float>> coActivationHistory;
    
    // Cue-to-pattern mapping
    std::unordered_map<NeuronId, std::vector<NeuronId>> cueToPatternMap;
    
    // Neuromodulation factors
    float rewardModulation;
    float noveltyModulation;
    
    Impl() : brain(nullptr), rewardModulation(1.0f), noveltyModulation(1.0f) {}
};

NeuralAssociativeMemory::NeuralAssociativeMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
{
    // Pre-allocate
    patternNeurons_.reserve(100);
    associations_.reserve(500);
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
    // Create pattern neurons if needed
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA != INVALID_NEURON_ID && neuronB != INVALID_NEURON_ID && neuronA != neuronB) {
        // Check if association exists
        for (auto& assoc : associations_) {
            if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
                (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
                // Update strength with decay and new learning
                float oldStrength = std::get<2>(assoc);
                float newStrength = std::clamp(oldStrength * 0.9f + strength * 0.1f, 0.0f, 1.0f);
                std::get<2>(assoc) = newStrength;
                
                // Record co-activation for Hebbian learning
                recordCoActivation(neuronA, neuronB, newStrength);
                return;
            }
        }
        
        // Create new association
        associations_.emplace_back(neuronA, neuronB, strength);
        
        // Record co-activation
        recordCoActivation(neuronA, neuronB, strength);
        
        // Update spreading activation
        pImpl->associationStrengths.push_back(strength);
        pImpl->activationSets.emplace_back(std::unordered_set<NeuronId>{neuronA, neuronB});
    }
}

void NeuralAssociativeMemory::associateFromExperience(const EpisodicMemoryItem& episode) {
    if (!episode.sensoryState.empty() && !episode.resultingSensoryState.empty()) {
        // Associate input with output
        associate(episode.sensoryState, episode.resultingSensoryState, 0.8f);
    }
    
    // Associate action with resulting state
    if (!episode.sensoryState.empty()) {
        // Create action-specific pattern
        std::vector<float> actionPattern(episode.sensoryState.size(), 0.0f);
        actionPattern[0] = static_cast<float>(episode.action) / 10.0f;
        
        associate(actionPattern, episode.resultingSensoryState, 0.7f);
    }
    
    // Associate reward prediction
    float reward = episode.reward;
    if (reward != 0.0f) {
        std::vector<float> rewardPattern(episode.sensoryState.size(), 0.0f);
        rewardPattern[0] = reward;
        
        associate(rewardPattern, episode.resultingSensoryState, 0.6f * std::abs(reward));
    }
    
    // Associate novelty
    float novelty = episode.novelty;
    if (novelty > 0.5f) {
        std::vector<float> noveltyPattern(episode.sensoryState.size(), 0.0f);
        noveltyPattern[0] = novelty;
        
        associate(noveltyPattern, episode.resultingSensoryState, 0.5f * novelty);
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::retrieve(
    const std::vector<float>& queryPattern,
    size_t maxResults) const {
    
    // Find best matching pattern neuron
    NeuronId queryNeuron;
    float bestSim = 0.0f;
    
    for (size_t i = 0; i < patternNeurons_.size(); ++i) {
        float sim = computeSimilarity(queryPattern, patternNeurons_[i].second);
        if (sim > bestSim) {
            bestSim = sim;
            queryNeuron = patternNeurons_[i].first;
        }
    }
    
    if (bestSim < 0.3f) {
        return {};  // Not enough match to retrieve
    }
    
    // Find associated patterns using spreading activation
    std::vector<std::pair<float, std::vector<float>>> scored;
    std::unordered_set<NeuronId> visited;
    
    // Start with direct associations
    std::queue<NeuronId> toProcess;
    toProcess.push(queryNeuron);
    visited.insert(queryNeuron);
    
    while (!toProcess.empty()) {
        NeuronId current = toProcess.front();
        toProcess.pop();
        
        // Find all neurons associated with current
        for (const auto& assoc : associations_) {
            NeuronId otherNeuron;
            
            if (std::get<0>(assoc) == current) {
                otherNeuron = std::get<1>(assoc);
            } else if (std::get<1>(assoc) == current) {
                otherNeuron = std::get<0>(assoc);
            } else {
                continue;
            }
            
            if (visited.find(otherNeuron) == visited.end()) {
                visited.insert(otherNeuron);
                
                // Get strength of association
                float strength = std::get<2>(assoc);
                
                // Apply neuromodulation factors
                strength *= pImpl->rewardModulation * 0.5f + pImpl->noveltyModulation * 0.5f;
                
                // Find pattern for this neuron
                for (const auto& pn : patternNeurons_) {
                    if (pn.first == otherNeuron) {
                        float score = strength * bestSim * (1.0f + pImpl->nodeActivations[current.index()]);
                        scored.emplace_back(score, pn.second);
                        toProcess.push(otherNeuron);
                        break;
                    }
                }
            }
        }
    }
    
    // Sort by score (descending)
    std::sort(scored.begin(), scored.end(),
        [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Return top results
    std::vector<std::vector<float>> results;
    for (size_t i = 0; i < std::min(maxResults, scored.size()); ++i) {
        results.push_back(scored[i].second);
    }
    
    return results;
}

float NeuralAssociativeMemory::getAssociationStrength(const std::vector<float>& patternA,
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

void NeuralAssociativeMemory::updateAssociation(const std::vector<float>& patternA,
                                                const std::vector<float>& patternB,
                                                float delta) {
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    for (auto& assoc : associations_) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + delta, 0.0f, 1.0f);
            
            // Update co-activation history
            recordCoActivation(neuronA, neuronB, std::get<2>(assoc));
            return;
        }
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::spreadActivation(
    const std::vector<float>& cuePattern,
    size_t steps) const {
    
    std::vector<std::vector<float>> activated;
    std::unordered_set<NeuronId> visited;
    
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
        
        // Pick next neuron with highest association strength
        NeuronId nextBest = nextNeurons[0];
        float maxStrength = std::get<2>(associations_[0]);
        
        for (NeuronId n : nextNeurons) {
            for (const auto& assoc : associations_) {
                if ((std::get<0>(assoc) == currentNeuron && std::get<1>(assoc) == n) ||
                    (std::get<0>(assoc) == n && std::get<1>(assoc) == currentNeuron)) {
                    if (std::get<2>(assoc) > maxStrength) {
                        maxStrength = std::get<2>(assoc);
                        nextBest = n;
                    }
                    break;
                }
            }
        }
        
        currentNeuron = nextBest;
    }
    
    return activated;
}

void NeuralAssociativeMemory::clear() {
    patternNeurons_.clear();
    associations_.clear();
    pImpl->associationStrengths.clear();
    pImpl->activationSets.clear();
    pImpl->coActivationHistory.clear();
    pImpl->cueToPatternMap.clear();
}

NeuronId NeuralAssociativeMemory::findPatternNeuron(const std::vector<float>& pattern) {
    // Check if we already have a similar pattern
    for (const auto& pn : patternNeurons_) {
        if (computeSimilarity(pattern, pn.second) > 0.8f) {
            return pn.first;
        }
    }
    
    // Create new pattern neuron
    NeuronId newId(patternNeurons_.size() + 40000);
    patternNeurons_.push_back({newId, pattern});
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

void NeuralAssociativeMemory::recordCoActivation(NeuronId neuronA, NeuronId neuronB, float strength) {
    // Record co-activation history for Hebbian learning
    pImpl->coActivationHistory.push_back({strength, strength});
    
    // Apply Hebbian-like learning to strengthen connections
    if (pImpl->brain) {
        // This would normally update synaptic weights in the brain
        // For now, we just record the co-activation
    }
}

void NeuralAssociativeMemory::applyNeuromodulation(float reward, float novelty) {
    pImpl->rewardModulation = std::max(0.1f, 1.0f - std::abs(reward) * 2.0f);
    pImpl->noveltyModulation = std::max(0.1f, 1.0f + novelty * 0.5f);
}

} // namespace nlm
