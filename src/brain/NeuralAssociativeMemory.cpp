#include "NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    // Fast lookup table for associations (for quick recall)
    std::unordered_map<uint64_t, std::vector<std::pair<NeuronId, float, std::vector<float>>>> lookupTable;
    
    // Context-based association storage
    std::vector<std::tuple<NeuronId, NeuronId, float, std::vector<float>>> strongAssociations;
    std::vector<std::tuple<NeuronId, NeuronId, float, std::vector<float>>> weakAssociations;
    
    // Activity-dependent weights
    std::unordered_map<NeuronId, float> neuronActivityWeights;
    
    // Association strength decay
    std::unordered_map<std::tuple<NeuronId, NeuronId, std::vector<float>>, float> decayCounters;
    
    // Learning parameters
    float hebbianLearningRate;
    float predictionErrorLearningRate;
    float contextModulationStrength;
    float completionThreshold;
    
    Impl() : 
        hebbianLearningRate(0.01f),
        predictionErrorLearningRate(0.02f),
        contextModulationStrength(0.5f),
        completionThreshold(0.3f) {}
};

NeuralAssociativeMemory::NeuralAssociativeMemory() : 
    pImpl(new Impl),
    brain_(nullptr) {
    NLM_LOG_INFO("NeuralAssociativeMemory created");
}

NeuralAssociativeMemory::~NeuralAssociativeMemory() {
    NLM_LOG_INFO("NeuralAssociativeMemory destroyed");
}

void NeuralAssociativeMemory::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize NeuralAssociativeMemory with null brain");
        return;
    }
    
    brain_ = brain;
    NLM_LOG_INFO("NeuralAssociativeMemory initialized");
}

void NeuralAssociativeMemory::associate(NeuronId a, NeuronId b, 
                                        const std::vector<float>& context,
                                        float strength, float predictionError) {
    if (!brain_) return;
    
    // Create normalized context signature for lookup
    std::vector<float> normalizedContext = context;
    float contextNorm = 1.0f;
    for (float val : normalizedContext) {
        contextNorm += val * val;
    }
    contextNorm = std::sqrt(contextNorm);
    if (contextNorm > 0.0f) {
        for (float& val : normalizedContext) {
            val /= contextNorm;
        }
    }
    
    uint64_t contextHash = std::hash<std::vector<float>>{}(normalizedContext);
    
    // Check if association already exists with same context
    auto& lookup = pImpl->lookupTable;
    if (lookup.find(contextHash) == lookup.end()) {
        lookup[contextHash] = std::vector<std::pair<NeuronId, float, std::vector<float>>>();
    }
    
    bool found = false;
    for (auto& entry : lookup[contextHash]) {
        if ((entry.first == a && lookup.find(std::hash<std::vector<float>>{}(std::vector<float>{b})) != lookup.end()) ||
            (entry.first == b && lookup.find(std::hash<std::vector<float>>{}(std::vector<float>{a})) != lookup.end())) {
            // Update existing association
            entry.second = strength * (1.0f + predictionError * pImpl->predictionErrorLearningRate);
            entry.third = normalizedContext;
            found = true;
            break;
        }
    }
    
    if (!found) {
        // Create new association
        lookup[contextHash].emplace_back(a, strength * (1.0f + predictionError * pImpl->predictionErrorLearningRate), normalizedContext);
    }
    
    // Also add to association lists
    float modulatedStrength = strength * (1.0f + predictionError * pImpl->predictionErrorLearningRate);
    
    // Add to strong associations
    bool inStrong = false;
    for (const auto& assoc : pImpl->strongAssociations) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            inStrong = true;
            break;
        }
    }
    
    if (!inStrong) {
        pImpl->strongAssociations.emplace_back(a, b, modulatedStrength, normalizedContext);
    }
    
    // Check if should be in weak associations (lower strength)
    if (modulatedStrength > 0.1f) {
        bool inWeak = false;
        for (const auto& assoc : pImpl->weakAssociations) {
            if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
                (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
                inWeak = true;
                // Update strength
                std::get<2>(const_cast<std::tuple<NeuronId, NeuronId, float, std::vector<float>>&>(assoc)) = modulatedStrength;
                break;
            }
        }
        if (!inWeak) {
            pImpl->weakAssociations.emplace_back(a, b, modulatedStrength, normalizedContext);
        }
    }
}

std::vector<NeuronId> NeuralAssociativeMemory::getAssociations(NeuronId neuron) const {
    std::vector<NeuronId> result;
    
    for (const auto& assoc : pImpl->strongAssociations) {
        if (std::get<0>(assoc) == neuron) {
            result.push_back(std::get<1>(assoc));
        } else if (std::get<1>(assoc) == neuron) {
            result.push_back(std::get<0>(assoc));
        }
    }
    
    return result;
}

float NeuralAssociativeMemory::getAssociationStrength(NeuronId a, NeuronId b,
                                                       const std::vector<float>& context) const {
    // Normalize context for lookup
    std::vector<float> normalizedContext = context;
    float contextNorm = 1.0f;
    for (float val : normalizedContext) {
        contextNorm += val * val;
    }
    contextNorm = std::sqrt(contextNorm);
    if (contextNorm > 0.0f) {
        for (float& val : normalizedContext) {
            val /= contextNorm;
        }
    }
    
    uint64_t contextHash = std::hash<std::vector<float>>{}(normalizedContext);
    
    // Look up direct association
    if (pImpl->lookupTable.find(contextHash) != pImpl->lookupTable.end()) {
        for (const auto& entry : pImpl->lookupTable.at(contextHash)) {
            if ((entry.first == a && b != a) || (entry.first == b && a != b)) {
                return entry.second;
            }
        }
    }
    
    // Check if association exists without context specificity
    for (const auto& assoc : pImpl->strongAssociations) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            // Return base strength without context modulation
            return std::get<2>(assoc);
        }
    }
    
    return 0.0f;
}

void NeuralAssociativeMemory::updateAssociation(NeuronId a, NeuronId b, float delta,
                                                 const std::vector<float>& context) {
    // Normalize context
    std::vector<float> normalizedContext = context;
    float contextNorm = 1.0f;
    for (float val : normalizedContext) {
        contextNorm += val * val;
    }
    contextNorm = std::sqrt(contextNorm);
    if (contextNorm > 0.0f) {
        for (float& val : normalizedContext) {
            val /= contextNorm;
        }
    }
    
    uint64_t contextHash = std::hash<std::vector<float>>{}(normalizedContext);
    
    // Update in lookup table
    if (pImpl->lookupTable.find(contextHash) != pImpl->lookupTable.end()) {
        for (auto& entry : pImpl->lookupTable.at(contextHash)) {
            if ((entry.first == a && b != a) || (entry.first == b && a != b)) {
                entry.second += delta;
                entry.second = std::max(0.0f, std::min(1.0f, entry.second));  // Clamp to [0, 1]
                
                // Also update in strong associations
                for (auto& strong : pImpl->strongAssociations) {
                    if ((std::get<0>(strong) == a && std::get<1>(strong) == b) ||
                        (std::get<0>(strong) == b && std::get<1>(strong) == a)) {
                        std::get<2>(strong) = entry.second;
                        break;
                    }
                }
                break;
            }
        }
    }
}

std::vector<NeuronId> NeuralAssociativeMemory::retrieveSimilar(NeuronId seed,
                                                             const std::vector<float>& context,
                                                             size_t maxResults) const {
    std::vector<std::pair<float, NeuronId>> candidates;
    
    // Get all associations for the seed neuron
    std::vector<NeuronId> allAssociations = getAssociations(seed);
    
    // For each associated neuron, check if they have additional associations
    for (NeuronId assoc : allAssociations) {
        std::vector<NeuronId> secondOrder = getAssociations(assoc);
        
        // Filter out those already in first-order
        for (NeuronId candidate : secondOrder) {
            if (candidate != seed && 
                std::find(allAssociations.begin(), allAssociations.end(), candidate) == allAssociations.end()) {
                
                float strength = getAssociationStrength(seed, candidate, context);
                
                // Boost strength if context matches
                if (!context.empty()) {
                    strength *= pImpl->contextModulationStrength;
                }
                
                candidates.emplace_back(strength, candidate);
            }
        }
    }
    
    // Sort by strength (descending)
    std::sort(candidates.begin(), candidates.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Extract top results
    std::vector<NeuronId> result;
    for (size_t i = 0; i < std::min(maxResults, candidates.size()); ++i) {
        result.push_back(candidates[i].second);
    }
    
    return result;
}

void NeuralAssociativeMemory::patternCompletion(const std::vector<NeuronId>& partialPattern,
                                                std::vector<NeuronId>& completedPattern) const {
    completedPattern = partialPattern;
    
    // Use iterative completion based on association strengths
    std::vector<bool> completed(partialPattern.size(), false);
    
    // Mark completed patterns
    for (size_t i = 0; i < partialPattern.size(); ++i) {
        if (partialPattern[i].value != 0) {  // Non-zero IDs indicate completed
            completed[i] = true;
        }
    }
    
    // Try to fill in missing patterns
    bool improved = true;
    while (improved) {
        improved = false;
        
        for (size_t i = 0; i < partialPattern.size(); ++i) {
            if (!completed[i]) {
                NeuronId currentNeuron = partialPattern[i];
                
                // Find strongly associated neurons
                std::vector<NeuronId> associations = getAssociations(currentNeuron);
                
                for (NeuronId assoc : associations) {
                    // Check if this association could complete the pattern
                    bool allAssociatedComplete = true;
                    for (size_t j = 0; j < partialPattern.size(); ++j) {
                        if (j != i && !completed[j]) {
                            float strength = getAssociationStrength(partialPattern[j], assoc, std::vector<float>());
                            if (strength > pImpl->completionThreshold) {
                                allAssociatedComplete = false;
                                break;
                            }
                        }
                    }
                    
                    if (allAssociatedComplete) {
                        // Complete this pattern
                        completedPattern[i] = assoc;
                        completed[i] = true;
                        improved = true;
                        break;
                    }
                }
            }
        }
    }
}

std::vector<std::tuple<NeuronId, NeuronId, float, std::vector<float>>>
    NeuralAssociativeMemory::getAllAssociations() const {
    std::vector<std::tuple<NeuronId, NeuronId, float, std::vector<float>>> result;
    
    // Add strong associations
    for (const auto& assoc : pImpl->strongAssociations) {
        result.push_back(assoc);
    }
    
    // Add weak associations
    for (const auto& assoc : pImpl->weakAssociations) {
        // Don't duplicate strong associations
        bool duplicate = false;
        for (const auto& strong : pImpl->strongAssociations) {
            if ((std::get<0>(strong) == std::get<0>(assoc) && std::get<1>(strong) == std::get<1>(assoc)) ||
                (std::get<0>(strong) == std::get<1>(assoc) && std::get<1>(strong) == std::get<0>(assoc))) {
                duplicate = true;
                break;
            }
        }
        if (!duplicate) {
            result.push_back(assoc);
        }
    }
    
    return result;
}

void NeuralAssociativeMemory::clear() {
    pImpl->lookupTable.clear();
    pImpl->strongAssociations.clear();
    pImpl->weakAssociations.clear();
    pImpl->neuronActivityWeights.clear();
    pImpl->decayCounters.clear();
    
    NLM_LOG_INFO("NeuralAssociativeMemory cleared");
}

void NeuralAssociativeMemory::update(Brain* brain, TimestepDuration dt) {
    if (!brain_ || !brain) return;
    
    // Update decay counters
    std::vector<std::tuple<NeuronId, NeuronId, float, std::vector<float>>> toRemove;
    
    for (const auto& assoc : pImpl->strongAssociations) {
        std::tuple<NeuronId, NeuronId, float, std::vector<float>> key =
            std::make_tuple(std::get<0>(assoc), std::get<1>(assoc), std::get<2>(assoc), std::get<3>(assoc));
        
        if (pImpl->decayCounters.find(key) == pImpl->decayCounters.end()) {
            pImpl->decayCounters[key] = 0.0f;
        }
        
        pImpl->decayCounters[key] += dt * 0.01f;  // Decay rate
        
        // Remove if decayed too much
        if (pImpl->decayCounters[key] > 100.0f) {  // 100 time units = decay
            toRemove.push_back(assoc);
        }
    }
    
    // Remove decayed associations
    for (const auto& assoc : toRemove) {
        // Remove from strong associations
        for (auto it = pImpl->strongAssociations.begin(); it != pImpl->strongAssociations.end(); ++it) {
            if (std::get<0>(*it) == std::get<0>(assoc) && std::get<1>(*it) == std::get<1>(assoc)) {
                pImpl->strongAssociations.erase(it);
                break;
            }
        }
        
        // Remove decay counter
        std::tuple<NeuronId, NeuronId, float, std::vector<float>> key =
            std::make_tuple(std::get<0>(assoc), std::get<1>(assoc), std::get<2>(assoc), std::get<3>(assoc));
        pImpl->decayCounters.erase(key);
    }
    
    // Update neuron activity weights based on current brain state
    // (In a real implementation, this would track actual neural activity)
    for (auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                if (neuron->getId().value > 0) {  // Valid neuron ID
                    pImpl->neuronActivityWeights[neuron->getId()] += dt * 0.001f;  // Increase weight with activity
                }
            }
        }
    }
}

} // namespace nlm