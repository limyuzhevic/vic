#pragma once

#include "AdvancedAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <set>

namespace nlm {

AdvancedAssociativeMemory::AdvancedAssociativeMemory()
    : brain_(nullptr)
{
    NLM_LOG_INFO("AdvancedAssociativeMemory initialized");
}

AdvancedAssociativeMemory::~AdvancedAssociativeMemory() {
    NLM_LOG_INFO("AdvancedAssociativeMemory destroyed");
}

void AdvancedAssociativeMemory::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize AdvancedAssociativeMemory with null brain");
        return;
    }
    brain_ = brain;
    NLM_LOG_INFO("AdvancedAssociativeMemory initialized with brain reference");
}

void AdvancedAssociativeMemory::associate(const std::vector<float>& patternA,
                                         const std::vector<float>& patternB,
                                         float baseStrength) {
    // Find or create pattern neurons
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);

    if (neuronA == INVALID_NEURON_ID || neuronB == INVALID_NEURON_ID) {
        return;
    }

    // Check if association already exists
    for (auto& assoc : associations_) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            std::get<2>(assoc) = std::min(1.0f, std::get<2>(assoc) + baseStrength * 0.1f);
            return;
        }
    }

    // Create new association
    associations_.emplace_back(neuronA, neuronB, baseStrength);

    NLM_LOG_DEBUG("Created association between patterns with strength " + std::to_string(baseStrength));
}

void AdvancedAssociativeMemory::associateFromExperience(const EpisodicMemoryItem& episode) {
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

std::vector<std::vector<float>> AdvancedAssociativeMemory::retrieveForward(
    const std::vector<float>& patternA,
    size_t maxResults) const {
    std::vector<std::pair<float, std::vector<float>>> scoredPatterns;
    NeuronId neuronA = findPatternNeuron(patternA);

    if (neuronA == INVALID_NEURON_ID) {
        return std::vector<std::vector<float>>();
    }

    for (const auto& assoc : associations_) {
        if (std::get<0>(assoc) == neuronA) {
            auto it = std::find_if(patternNeurons_.begin(), patternNeurons_.end(),
                                [neuronB = std::get<1>(assoc)](const PatternNeuron& pn) {
                                    return pn.neuronId == neuronB;
                                });
            if (it != patternNeurons_.end()) {
                scoredPatterns.emplace_back(std::get<2>(assoc), it->pattern);
            }
        }
    }

    // Sort by strength
    std::sort(scoredPatterns.begin(), scoredPatterns.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });

    std::vector<std::vector<float>> results;
    for (size_t i = 0; i < std::min(maxResults, scoredPatterns.size()); ++i) {
        results.push_back(scoredPatterns[i].second);
    }

    return results;
}

std::vector<std::vector<float>> AdvancedAssociativeMemory::retrieveBackward(
    const std::vector<float>& patternB,
    size_t maxResults) const {
    std::vector<std::pair<float, std::vector<float>>> scoredPatterns;
    NeuronId neuronB = findPatternNeuron(patternB);

    if (neuronB == INVALID_NEURON_ID) {
        return std::vector<std::vector<float>>();
    }

    for (const auto& assoc : associations_) {
        if (std::get<1>(assoc) == neuronB) {
            auto it = std::find_if(patternNeurons_.begin(), patternNeurons_.end(),
                                [neuronA = std::get<0>(assoc)](const PatternNeuron& pn) {
                                    return pn.neuronId == neuronA;
                                });
            if (it != patternNeurons_.end()) {
                scoredPatterns.emplace_back(std::get<2>(assoc), it->pattern);
            }
        }
    }

    // Sort by strength
    std::sort(scoredPatterns.begin(), scoredPatterns.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });

    std::vector<std::vector<float>> results;
    for (size_t i = 0; i < std::min(maxResults, scoredPatterns.size()); ++i) {
        results.push_back(scoredPatterns[i].second);
    }

    return results;
}

float AdvancedAssociativeMemory::getAssociationStrength(const std::vector<float>& patternA,
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

void AdvancedAssociativeMemory::updateAssociation(const std::vector<float>& patternA,
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

std::vector<std::vector<float>> AdvancedAssociativeMemory::spreadActivation(
    const std::vector<float>& cuePattern,
    size_t steps) const {

    std::vector<std::vector<float>> activated;
    std::set<NeuronId> visited;

    // Find initial pattern neuron
    NeuronId currentNeuron = INVALID_NEURON_ID;
    for (const auto& pn : patternNeurons_) {
        if (computeSimilarity(cuePattern, pn.pattern) > 0.5f) {
            currentNeuron = pn.neuronId;
            activated.push_back(pn.pattern);
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
                    if (pn.neuronId == otherNeuron) {
                        activated.push_back(pn.pattern);
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

void AdvancedAssociativeMemory::establishChain(const std::vector<std::vector<float>>& patternSequence) {
    if (patternSequence.size() < 2) {
        return;
    }

    std::vector<size_t> chainIndices;

    for (const auto& pattern : patternSequence) {
        NeuronId neuron = findPatternNeuron(pattern);
        if (neuron != INVALID_NEURON_ID) {
            auto it = std::find_if(patternNeurons_.begin(), patternNeurons_.end(),
                                [neuron](const PatternNeuron& pn) { return pn.neuronId == neuron; });
            if (it != patternNeurons_.end()) {
                // Find the association that connects these neurons
                for (size_t i = 0; i < associations_.size(); ++i) {
                    if ((std::get<0>(associations_[i]) == neuron || std::get<1>(associations_[i]) == neuron) &&
                        (std::get<0>(associations_[i]) == patternSequence[0].neuronId || std::get<1>(associations_[i]) == patternSequence[0].neuronId)) {
                        chainIndices.push_back(i);
                        break;
                    }
                }
            }
        }
    }

    if (chainIndices.size() >= 2) {
        // Update association chains
        for (size_t i = 0; i < associations_.size(); ++i) {
            if (std::find(chainIndices.begin(), chainIndices.end(), i) != chainIndices.end()) {
                associations_[i].chainIndices = chainIndices;
            }
        }
    }

    NLM_LOG_DEBUG("Established association chain with " + std::to_string(chainIndices.size()) + " links");
}

std::vector<std::vector<float>> AdvancedAssociativeMemory::retrieveViaChain(
    const std::vector<float>& startingPattern,
    size_t maxChainLength) const {

    std::vector<std::vector<float>> results;
    NeuronId currentNeuron = findPatternNeuron(startingPattern);

    if (currentNeuron == INVALID_NEURON_ID) {
        return results;
    }

    // Find starting association
    size_t currentChainIndex = INVALID_NEURON_ID;
    for (size_t i = 0; i < associations_.size(); ++i) {
        if ((std::get<0>(associations_[i]) == currentNeuron || std::get<1>(associations_[i]) == currentNeuron)) {
            currentChainIndex = i;
            break;
        }
    }

    if (currentChainIndex == INVALID_NEURON_ID) {
        return results;
    }

    // Traverse the chain
    for (size_t step = 0; step < maxChainLength; ++step) {
        // Find pattern for current neuron
        auto it = std::find_if(patternNeurons_.begin(), patternNeurons_.end(),
                              [currentNeuron](const PatternNeuron& pn) { return pn.neuronId == currentNeuron; });

        if (it != patternNeurons_.end()) {
            results.push_back(it->pattern);
        }

        // Find next neuron in chain
        bool foundNext = false;
        for (const auto& assoc : associations_) {
            if (assoc.chainIndices.size() > step + 1) {
                NeuronId nextNeuron = INVALID_NEURON_ID;
                if (std::get<0>(assoc) == currentNeuron) {
                    nextNeuron = std::get<1>(assoc);
                } else if (std::get<1>(assoc) == currentNeuron) {
                    nextNeuron = std::get<0>(assoc);
                }

                if (nextNeuron != INVALID_NEURON_ID) {
                    currentNeuron = nextNeuron;
                    foundNext = true;
                    break;
                }
            }
        }

        if (!foundNext) {
            break;
        }
    }

    return results;
}

void AdvancedAssociativeMemory::applyAssociationDecay(float decayRate) {
    for (auto& assoc : associations_) {
        assoc.strength *= (1.0f - decayRate);
    }

    for (auto& pn : patternNeurons_) {
        pn.activationLevel *= (1.0f - decayRate * 0.5f);
    }

    NLM_LOG_DEBUG("Applied association decay with rate: " + std::to_string(decayRate));
}

void AdvancedAssociativeMemory::strengthenAssociations(const std::vector<float>& contextPattern) {
    for (auto& assoc : associations_) {
        // Check if this association is relevant to the context
        float relevance = std::min(1.0f, computeSimilarity(assoc.patternA, contextPattern)) +
                         std::min(1.0f, computeSimilarity(assoc.patternB, contextPattern)) * 0.5f;

        if (relevance > 0.3f) {
            assoc.strength = std::min(1.0f, assoc.strength + relevance * 0.1f);
        }
    }

    NLM_LOG_DEBUG("Strengthened associations based on context pattern");
}

void AdvancedAssociativeMemory::clear() {
    patternNeurons_.clear();
    associations_.clear();
    associationChains_.clear();
    patternCache_.clear();

    NLM_LOG_INFO("AdvancedAssociativeMemory cleared");
}

void AdvancedAssociativeMemory::updateAssociationStrengths() {
    // Update association strengths based on activation levels
    for (auto& assoc : associations_) {
        assoc.strength *= 0.95f; // Natural decay
    }
}

void AdvancedAssociativeMemory::computeSpreadActivation() {
    // Compute spreading activation for all patterns
    for (auto& pn : patternNeurons_) {
        float activation = 0.0f;
        for (const auto& assoc : associations_) {
            if ((std::get<0>(assoc) == pn.neuronId || std::get<1>(assoc) == pn.neuronId)) {
                activation += std::get<2>(assoc) * 0.5f;
            }
        }
        pn.activationLevel = std::min(1.0f, activation);
    }
}

void AdvancedAssociativeMemory::maintainAssociationChains() {
    // Maintain association chains
    for (auto& assoc : associations_) {
        assoc.chainIndices.clear();
        // Chains would be maintained through association updates
    }
}

NeuronId AdvancedAssociativeMemory::findPatternNeuron(const std::vector<float>& pattern) {
    // Check if we already have a similar pattern
    for (const auto& pn : patternNeurons_) {
        if (computeSimilarity(pattern, pn.pattern) > 0.8f) {
            return pn.neuronId;
        }
    }

    // Create new pattern neuron
    NeuronId newId(patternNeurons_.size() + 30000);
    patternNeurons_.push_back({newId, pattern});
    return newId;
}

float AdvancedAssociativeMemory::computeSimilarity(const std::vector<float>& a,
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