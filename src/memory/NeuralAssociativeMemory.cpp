#include "NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

NeuralAssociativeMemory::NeuralAssociativeMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , associations_()
    , patternNeurons_()
{
}

NeuralAssociativeMemory::~NeuralAssociativeMemory() = default;

void NeuralAssociativeMemory::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize NeuralAssociativeMemory with null brain pointer");
        return;
    }
    
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralAssociativeMemory initialized");
}

void NeuralAssociativeMemory::associate(const std::vector<float>& patternA,
                                       const std::vector<float>& patternB,
                                       float strength) {
    if (!brain_) {
        NLM_LOG_WARNING("Cannot associate patterns: brain is not initialized");
        return;
    }
    
    if (patternA.empty() || patternB.empty()) {
        NLM_LOG_WARNING("Cannot associate empty patterns");
        return;
    }
    
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA != INVALID_NEURON_ID && neuronB != INVALID_NEURON_ID) {
        // Check if association exists
        for (auto& assoc : associations_) {
            if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
                (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
                // Update strength
                float newStrength = std::clamp(std::get<2>(assoc) + strength * 0.1f, 0.0f, 1.0f);
                std::get<2>(assoc) = newStrength;
                NLM_LOG_DEBUG(std::string("Updated association strength: ") + std::to_string(newStrength));
                return;
            }
        }
        
        // Create new association
        associations_.emplace_back(neuronA, neuronB, strength);
        NLM_LOG_INFO(std::string("Created associative memory link between neurons ") + 
            std::to_string(neuronA.value) + " and " + std::to_string(neuronB.value) +
            ", strength: " + std::to_string(strength));
    }
}

void NeuralAssociativeMemory::associateFromExperience(const EpisodicMemoryItem& episode) {
    if (!brain_) {
        NLM_LOG_WARNING("Cannot associate from experience: brain is not initialized");
        return;
    }
    
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
    
    NLM_LOG_DEBUG(std::string("Associating from experience: reward ") + std::to_string(episode.reward));
}

std::vector<std::vector<float>> NeuralAssociativeMemory::retrieve(
    const std::vector<float>& queryPattern,
    size_t maxResults) const {
    
    if (queryPattern.empty() || !brain_) {
        return std::vector<std::vector<float>>();
    }
    
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
        NLM_LOG_DEBUG("Pattern not found in associative memory (similarity: " + std::to_string(bestSim) + ")");
        return std::vector<std::vector<float>>();
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
    
    if (!results.empty()) {
        NLM_LOG_INFO(std::string("Retrieved ") + std::to_string(results.size()) +
            " associated patterns from memory");
    }
    
    return results;
}

float NeuralAssociativeMemory::getAssociationStrength(
    const std::vector<float>& patternA,
    const std::vector<float>& patternB) const {
    
    if (patternA.empty() || patternB.empty()) {
        return 0.0f;
    }
    
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
    
    if (!brain_ || delta == 0.0f) {
        return;
    }
    
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA == INVALID_NEURON_ID || neuronB == INVALID_NEURON_ID) {
        NLM_LOG_DEBUG("Cannot update association: patterns not found");
        return;
    }
    
    bool found = false;
    for (auto& assoc : associations_) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            float newStrength = std::clamp(std::get<2>(assoc) + delta, 0.0f, 1.0f);
            std::get<2>(assoc) = newStrength;
            found = true;
            NLM_LOG_DEBUG(std::string("Updated association strength to: ") + std::to_string(newStrength));
            break;
        }
    }
    
    if (!found) {
        NLM_LOG_DEBUG("Created new association during update");
        float strength = std::max(0.0f, delta);
        associations_.emplace_back(neuronA, neuronB, strength);
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::spreadActivation(
    const std::vector<float>& cuePattern,
    size_t steps) const {
    
    if (cuePattern.empty() || steps == 0 || !brain_) {
        return std::vector<std::vector<float>>();
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
        NLM_LOG_DEBUG("No pattern found for activation spread cue");
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
    
    if (activated.size() > 1) {
        NLM_LOG_INFO(std::string("Spread activation through ") + std::to_string(activated.size()) + " memory patterns");
    }
    
    return activated;
}

void NeuralAssociativeMemory::clear() {
    patternNeurons_.clear();
    associations_.clear();
    NLM_LOG_INFO("NeuralAssociativeMemory cleared");
}

size_t NeuralAssociativeMemory::getAssociationCount() const {
    return associations_.size();
}

size_t NeuralAssociativeMemory::getPatternCount() const {
    return patternNeurons_.size();
}

NeuronId NeuralAssociativeMemory::findPatternNeuron(const std::vector<float>& pattern) {
    if (pattern.empty()) {
        return INVALID_NEURON_ID;
    }
    
    // Check if we already have a similar pattern
    for (size_t i = 0; i < patternNeurons_.size(); ++i) {
        if (computeSimilarity(pattern, patternNeurons_[i].second) > 0.8f) {
            return patternNeurons_[i].first;
        }
    }
    
    // Create new pattern neuron with unique ID
    NeuronId newId(static_cast<uint64_t>(patternNeurons_.size() + 30000));
    
    // Register the new pattern neuron with the brain
    if (brain_) {
        // In a real implementation, would create actual neural population
        NLM_LOG_DEBUG(std::string("Created new pattern neuron ") + std::to_string(newId.value));
    }
    
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

} // namespace nlm