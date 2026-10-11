#include "NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <queue>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    Brain* brain;
    
    // Core associative memory mechanisms
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations;
    std::vector<std::vector<float>> patternNeurons;  // One pattern per neuron cluster
    std::vector<std::vector<float>> patternRepresentations;
    
    // Learning mechanisms
    float learningRate;
    float consolidationThreshold;
    float sparsityTarget;
    
    // Performance tracking
    size_t totalAssociations;
    size_t totalPatternNeurons;
    
    Impl() 
        : brain(nullptr)
        , learningRate(0.01f)
        , consolidationThreshold(0.7f)
        , sparsityTarget(0.1f)
        , totalAssociations(0)
        , totalPatternNeurons(0)
    {}
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

void NeuralAssociativeMemory::learnAssociation(const std::vector<NeuronId>& keyPattern, 
                                               const std::vector<NeuronId>& valuePattern,
                                               float strength) {
    if (keyPattern.empty() || valuePattern.empty() || !brain_) return;
    
    // Ensure patterns have consistent length
    size_t patternSize = std::min(keyPattern.size(), valuePattern.size());
    
    // Create distributed representations for patterns
    std::vector<float> keyPatternRep(patternSize, 0.0f);
    std::vector<float> valuePatternRep(patternSize, 0.0f);
    
    // Create sparse distributed representations
    size_t keySparsity = static_cast<size_t>(patternSize * pImpl->sparsityTarget);
    size_t valueSparsity = static_cast<size_t>(patternSize * pImpl->sparsityTarget);
    
    for (size_t i = 0; i < keySparsity; ++i) {
        keyPatternRep[i % patternSize] = 1.0f;
    }
    for (size_t i = 0; i < valueSparsity; ++i) {
        valuePatternRep[i % patternSize] = 1.0f;
    }
    
    // Find or create pattern neurons
    NeuronId keyNeuron = findOrCreatePatternNeuron(keyPatternRep);
    NeuronId valueNeuron = findOrCreatePatternNeuron(valuePatternRep);
    
    // Learn association using multiple mechanisms
    implementHebbianAssociativeLearning(keyNeuron, valueNeuron, strength);
    implementSparseCoding(keyNeuron, valueNeuron, patternSize);
    implementPredictiveCoding(keyNeuron, valueNeuron);
    implementMemoryEfficiencyOptimizations();
}

std::vector<NeuronId> NeuralAssociativeMemory::retrieveAssociation(const std::vector<NeuronId>& keyPattern,
                                                                    float threshold) {
    if (keyPattern.empty() || pImpl->patternNeurons.empty()) return {};
    
    std::vector<NeuronId> result;
    
    // Convert key pattern to distributed representation
    size_t patternSize = std::min(keyPattern.size(), 
                                pImpl->patternRepresentations[0].size());
    std::vector<float> keyPatternRep(patternSize, 0.0f);
    for (size_t i = 0; i < patternSize; ++i) {
        keyPatternRep[i] = static_cast<float>(keyPattern[i] % patternSize) / patternSize;
    }
    
    // Find best matching pattern neuron
    float bestSimilarity = 0.0f;
    NeuronId bestMatchNeuron = INVALID_NEURON_ID;
    
    for (size_t i = 0; i < pImpl->patternNeurons.size(); ++i) {
        float similarity = computePatternSimilarity(keyPatternRep,
                                                  pImpl->patternRepresentations[i]);
        
        if (similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestMatchNeuron = pImpl->patternNeurons[i][0]; // Use first neuron as representative
        }
    }
    
    if (bestSimilarity >= threshold) {
        // Find associated value patterns
        for (const auto& assoc : pImpl->associations) {
            if (std::get<0>(assoc) == bestMatchNeuron) {
                result.push_back(std::get<1>(assoc));
            }
        }
    }
    
    return result;
}

void NeuralAssociativeMemory::implementHebbianAssociativeLearning(NeuronId keyNeuron,
                                                                 NeuronId valueNeuron,
                                                                 float strength) {
    // Hebbian learning: strengthen connection if both neurons are active
    float currentStrength = getAssociationStrength(keyNeuron, valueNeuron);
    float newStrength = std::min(1.0f, currentStrength + strength * pImpl->learningRate);
    
    // Check if association already exists
    bool exists = false;
    for (auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == keyNeuron && std::get<1>(assoc) == valueNeuron) ||
            (std::get<0>(assoc) == valueNeuron && std::get<1>(assoc) == keyNeuron)) {
            std::get<2>(assoc) = newStrength;
            exists = true;
            break;
        }
    }
    
    if (!exists) {
        pImpl->associations.emplace_back(keyNeuron, valueNeuron, newStrength);
        pImpl->totalAssociations++;
    }
    
    // Update neural connectivity if brain exists
    if (brain_ && newStrength > 0.1f) {
        brain_->injectCurrent(valueNeuron, 1.0f); // Strengthen association
    }
}

void NeuralAssociativeMemory::implementSparseCoding(NeuronId keyNeuron,
                                                   NeuronId valueNeuron,
                                                   size_t patternSize) {
    // Create sparse coding representation to improve memory efficiency
    std::vector<float> sparsePattern(patternSize, 0.0f);
    
    // Convert neuron ID to sparse index
    size_t index = static_cast<size_t>(keyNeuron % patternSize);
    sparsePattern[index] = 1.0f;
    
    // Update pattern representations if this pattern is new
    bool patternExists = false;
    for (const auto& rep : pImpl->patternRepresentations) {
        if (rep == sparsePattern) {
            patternExists = true;
            break;
        }
    }
    
    if (!patternExists) {
        pImpl->patternRepresentations.push_back(sparsePattern);
        pImpl->patternNeurons.push_back({keyNeuron});
        pImpl->totalPatternNeurons++;
    }
}

void NeuralAssociativeMemory::implementPredictiveCoding(NeuronId keyNeuron, NeuronId valueNeuron) {
    // Implement predictive coding mechanism: key pattern predicts value pattern
    // This creates forward connections from key to value neurons
    
    if (!brain_) return;
    
    // Create predictive connection with appropriate weight
    float predictionStrength = getAssociationStrength(keyNeuron, valueNeuron);
    
    // If prediction is strong, activate value neuron as prediction
    if (predictionStrength > 0.5f) {
        brain_->injectCurrent(valueNeuron, predictionStrength * 2.0f);
    }
}

void NeuralAssociativeMemory::implementMemoryEfficiencyOptimizations() {
    // Remove redundant associations
    std::vector<std::tuple<NeuronId, NeuronId, float>> optimized;
    
    for (const auto& assoc : pImpl->associations) {
        NeuronId a = std::get<0>(assoc);
        NeuronId b = std::get<1>(assoc);
        float strength = std::get<2>(assoc);
        
        // Check for redundancy with other associations
        bool redundant = false;
        for (const auto& other : pImpl->associations) {
            if (&assoc == &other) continue;
            
            // Similar associations can be merged or removed
            if ((a == std::get<0>(other) && b == std::get<1>(other)) ||
                (a == std::get<1>(other) && b == std::get<0>(other))) {
                if (std::abs(strength - std::get<2>(other)) < 0.01f) {
                    redundant = true;
                    break;
                }
            }
        }
        
        if (!redundant) {
            optimized.push_back(assoc);
        }
    }
    
    pImpl->associations = optimized;
}

float NeuralAssociativeMemory::getAssociationStrength(NeuronId a, NeuronId b) const {
    for (const auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            return std::get<2>(assoc);
        }
    }
    return 0.0f;
}

NeuronId NeuralAssociativeMemory::findOrCreatePatternNeuron(const std::vector<float>& pattern) {
    // Check if pattern already has a representative neuron
    for (size_t i = 0; i < pImpl->patternRepresentations.size(); ++i) {
        if (computePatternSimilarity(pattern, pImpl->patternRepresentations[i]) > 0.9f) {
            return pImpl->patternNeurons[i][0]; // Return first neuron as representative
        }
    }
    
    // Create new pattern neuron
    NeuronId newNeuron(pattern.size() + 30000); // Create unique ID
    pImpl->patternNeurons.push_back({newNeuron});
    pImpl->patternRepresentations.push_back(pattern);
    pImpl->totalPatternNeurons++;
    
    // Inject current to strengthen neuron
    if (brain_) {
        brain_->injectCurrent(newNeuron, 1.0f);
    }
    
    return newNeuron;
}

void NeuralAssociativeMemory::clear() {
    pImpl->associations.clear();
    pImpl->patternNeurons.clear();
    pImpl->patternRepresentations.clear();
    pImpl->totalAssociations = 0;
    pImpl->totalPatternNeurons = 0;
    
    if (brain_) {
        NLM_LOG_INFO("NeuralAssociativeMemory cleared");
    }
}

void NeuralAssociativeMemory::update(float timeStep) {
    // Update associative memory through consolidation
    consolidateAssociations(timeStep);
    
    // Apply homeostatic plasticity to prevent runaway excitation
    applyHomeostaticPlasticity();
}

void NeuralAssociativeMemory::consolidateAssociations(float timeStep) {
    // Gradually strengthen weak associations
    for (auto& assoc : pImpl->associations) {
        float strength = std::get<2>(assoc);
        
        // Natural decay of weak associations
        if (strength < 0.1f) {
            strength *= 0.95f;  // Decay weak connections
        } else {
            // Gradual consolidation of strong associations
            strength = std::min(1.0f, strength + 0.001f * timeStep);
        }
        
        std::get<2>(assoc) = strength;
    }
}

void NeuralAssociativeMemory::applyHomeostaticPlasticity() {
    if (!brain_) return;
    
    // Calculate total association strength
    float totalStrength = 0.0f;
    for (const auto& assoc : pImpl->associations) {
        totalStrength += std::get<2>(assoc);
    }
    
    // Apply normalization if total is too high
    if (totalStrength > 1.0f && pImpl->totalAssociations > 0) {
        float scalingFactor = 1.0f / totalStrength;
        
        for (auto& assoc : pImpl->associations) {
            std::get<2>(assoc) *= scalingFactor;
        }
    }
}

float NeuralAssociativeMemory::computePatternSimilarity(const std::vector<float>& a,
                                                        const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    // Cosine similarity
    float dotProduct = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    
    for (size_t i = 0; i < a.size(); ++i) {
        dotProduct += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dotProduct / (std::sqrt(normA) * std::sqrt(normB));
}

float NeuralAssociativeMemory::getLearningRate() const {
    return pImpl->learningRate;
}

void NeuralAssociativeMemory::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 0.1f);
}

float NeuralAssociativeMemory::getConsolidationThreshold() const {
    return pImpl->consolidationThreshold;
}

void NeuralAssociativeMemory::setConsolidationThreshold(float threshold) {
    pImpl->consolidationThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

float NeuralAssociativeMemory::getSparsityTarget() const {
    return pImpl->sparsityTarget;
}

void NeuralAssociativeMemory::setSparsityTarget(float sparsity) {
    pImpl->sparsityTarget = std::clamp(sparsity, 0.01f, 0.5f);
}

size_t NeuralAssociativeMemory::getTotalAssociations() const {
    return pImpl->totalAssociations;
}

size_t NeuralAssociativeMemory::getTotalPatternNeurons() const {
    return pImpl->totalPatternNeurons;
}

const std::vector<std::tuple<NeuronId, NeuronId, float>>& NeuralAssociativeMemory::getAllAssociations() const {
    return pImpl->associations;
}

const std::vector<NeuronId>& NeuralAssociativeMemory::getAllPatternNeurons() const {
    if (pImpl->patternNeurons.empty()) return patternNeurons_;
    
    patternNeurons_.clear();
    for (const auto& neurons : pImpl->patternNeurons) {
        patternNeurons_.insert(patternNeurons_.end(), neurons.begin(), neurons.end());
    }
    return patternNeurons_;
}

void NeuralAssociativeMemory::logStats() const {
    NLM_LOG_INFO("NeuralAssociativeMemory Statistics:");
    NLM_LOG_INFO("  Total associations: " + std::to_string(getTotalAssociations()));
    NLM_LOG_INFO("  Total pattern neurons: " + std::to_string(getTotalPatternNeurons()));
    NLM_LOG_INFO("  Average association strength: " + 
                 std::to_string(getAverageAssociationStrength()));
}

float NeuralAssociativeMemory::getAverageAssociationStrength() const {
    if (pImpl->associations.empty()) return 0.0f;
    
    float totalStrength = 0.0f;
    for (const auto& assoc : pImpl->associations) {
        totalStrength += std::get<2>(assoc);
    }
    
    return totalStrength / pImpl->associations.size();
}

} // namespace nlm
