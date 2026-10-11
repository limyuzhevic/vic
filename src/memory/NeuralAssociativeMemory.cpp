# NeuralAssociativeMemory.cpp

#include "NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <queue>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    // Neural patterns with their associations
    std::vector<std::vector<float>> patterns;
    std::vector<std::vector<NeuronId>> patternNeuronIds;
    std::vector<std::string> patternLabels;
    
    // Association strength matrix
    std::vector<std::vector<float>> associationMatrix;
    
    // Pattern activation levels
    std::vector<float> patternActivations;
    
    // Association strength for specific neuron pairs
    std::unordered_map<uint64_t, float> neuronPairStrength;
    
    // Pattern similarity network
    std::unordered_map<std::string, size_t> labelToIndex;
    
    // Hebbian learning parameters
    float hebbianLearningRate;
    float decayRate;
    
    // Pattern queue for recent activations
    std::queue<std::pair<size_t, SimulationStep>> activationQueue;
    
    // Development-dependent modulation
    float developmentalModulation;
};

NeuralAssociativeMemory::NeuralAssociativeMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , hebbianLearningRate(0.01f)
    , decayRate(0.001f)
    , developmentalModulation(1.0f)
{
    pImpl->hebbianLearningRate = 0.01f;
    pImpl->decayRate = 0.001f;
    pImpl->developmentalModulation = 1.0f;
}

NeuralAssociativeMemory::~NeuralAssociativeMemory() = default;

void NeuralAssociativeMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralAssociativeMemory initialized");
}

void NeuralAssociativeMemory::associate(NeuronId a, NeuronId b, float strength) {
    // Store association with both directions (symmetric)
    uint64_t key1 = (static_cast<uint64_t>(a.getId()) << 32) | b.getId();
    uint64_t key2 = (static_cast<uint64_t>(b.getId()) << 32) | a.getId();
    
    pImpl->neuronPairStrength[key1] = std::clamp(strength, 0.0f, 1.0f);
    pImpl->neuronPairStrength[key2] = std::clamp(strength, 0.0f, 1.0f);
    
    // Update association matrix if patterns exist
    updateAssociationMatrix();
}

void NeuralAssociativeMemory::associatePatterns(const std::vector<NeuronId>& patternA,
                                                const std::vector<NeuronId>& patternB,
                                                float baseStrength) {
    if (!brain_) return;
    
    // Create neural patterns from neuron IDs
    std::vector<float> neuralPatternA;
    std::vector<float> neuralPatternB;
    
    convertNeuronIdsToNeuralPattern(patternA, neuralPatternA);
    convertNeuronIdsToNeuralPattern(patternB, neuralPatternB);
    
    // Store patterns
    size_t indexA = pImpl->patterns.size();
    size_t indexB = pImpl->patterns.size() + 1;
    
    pImpl->patterns.push_back(neuralPatternA);
    pImpl->patternNeuronIds.push_back(patternA);
    pImpl->patterns.push_back(neuralPatternB);
    pImpl->patternNeuronIds.push_back(patternB);
    
    std::string labelA = "pattern_" + std::to_string(indexA);
    std::string labelB = "pattern_" + std::to_string(indexB);
    pImpl->patternLabels.push_back(labelA);
    pImpl->patternLabels.push_back(labelB);
    
    pImpl->labelToIndex[labelA] = indexA;
    pImpl->labelToIndex[labelB] = indexB;
    
    pImpl->patternActivations.push_back(0.0f);
    pImpl->patternActivations.push_back(0.0f);
    
    // Calculate mutual information as association strength
    float associationStrength = calculatePatternAssociation(neuralPatternA, neuralPatternB);
    
    // Set association matrix
    pImpl->associationMatrix.resize(pImpl->patterns.size());
    for (size_t i = 0; i < pImpl->patterns.size(); ++i) {
        pImpl->associationMatrix[i].resize(pImpl->patterns.size(), 0.0f);
    }
    
    pImpl->associationMatrix[indexA][indexB] = associationStrength;
    pImpl->associationMatrix[indexB][indexA] = associationStrength;
    
    // Store individual neuron associations
    for (NeuronId neuronA : patternA) {
        for (NeuronId neuronB : patternB) {
            if (neuronA != neuronB) {
                associate(neuronA, neuronB, baseStrength * associationStrength);
            }
        }
    }
}

std::vector<NeuronId> NeuralAssociativeMemory::getAssociations(NeuronId neuron) const {
    std::vector<NeuronId> result;
    
    for (const auto& pair : pImpl->neuronPairStrength) {
        uint64_t key = pair.first;
        NeuronId otherNeuron = static_cast<NeuronId>(key & 0xFFFFFFFF);
        NeuronId thisNeuron = static_cast<NeuronId>((key >> 32) & 0xFFFFFFFF);
        
        if (thisNeuron == neuron) {
            result.push_back(otherNeuron);
        }
    }
    
    return result;
}

float NeuralAssociativeMemory::getAssociationStrength(NeuronId a, NeuronId b) const {
    uint64_t key1 = (static_cast<uint64_t>(a.getId()) << 32) | b.getId();
    uint64_t key2 = (static_cast<uint64_t>(b.getId()) << 32) | a.getId();
    
    if (pImpl->neuronPairStrength.find(key1) != pImpl->neuronPairStrength.end()) {
        return pImpl->neuronPairStrength.at(key1);
    }
    if (pImpl->neuronPairStrength.find(key2) != pImpl->neuronPairStrength.end()) {
        return pImpl->neuronPairStrength.at(key2);
    }
    
    return 0.0f;
}

void NeuralAssociativeMemory::updateAssociation(const std::vector<NeuronId>& pattern1,
                                                const std::vector<NeuronId>& pattern2,
                                                float delta) {
    for (NeuronId neuronA : pattern1) {
        for (NeuronId neuronB : pattern2) {
            if (neuronA != neuronB) {
                float currentStrength = getAssociationStrength(neuronA, neuronB);
                float newStrength = std::clamp(currentStrength + delta, 0.0f, 1.0f);
                
                uint64_t key1 = (static_cast<uint64_t>(neuronA.getId()) << 32) | neuronB.getId();
                uint64_t key2 = (static_cast<uint64_t>(neuronB.getId()) << 32) | neuronA.getId();
                
                pImpl->neuronPairStrength[key1] = newStrength;
                pImpl->neuronPairStrength[key2] = newStrength;
            }
        }
    }
}

void NeuralAssociativeMemory::update(const std::vector<NeuronId>& patternA,
                                     const std::vector<NeuronId>& patternB,
                                     float learningRate) {
    if (!brain_) return;
    
    // Convert to neural patterns
    std::vector<float> neuralPatternA;
    std::vector<float> neuralPatternB;
    convertNeuronIdsToNeuralPattern(patternA, neuralPatternA);
    convertNeuronIdsToNeuralPattern(patternB, neuralPatternB);
    
    // Find or create patterns
    size_t indexA = findOrCreatePattern(neuralPatternA);
    size_t indexB = findOrCreatePattern(neuralPatternB);
    
    // Hebbian learning: strengthen associations between co-activated patterns
    float hebbianDelta = learningRate * developmentalModulation;
    
    if (pImpl->associationMatrix.size() <= indexA) {
        pImpl->associationMatrix.resize(indexA + 1);
    }
    if (pImpl->associationMatrix.size() <= indexB) {
        pImpl->associationMatrix.resize(indexB + 1);
    }
    
    for (size_t i = pImpl->associationMatrix.size(); i <= std::max(indexA, indexB); ++i) {
        pImpl->associationMatrix[i].resize(pImpl->associationMatrix.size(), 0.0f);
    }
    
    pImpl->associationMatrix[indexA][indexB] += hebbianDelta;
    pImpl->associationMatrix[indexB][indexA] += hebbianDelta;
    
    // Apply Oja's learning rule for stability
    normalizeAssociationMatrix(indexA, indexB);
    
    // Update individual neuron associations
    for (NeuronId neuronA : patternA) {
        for (NeuronId neuronB : patternB) {
            if (neuronA != neuronB) {
                updateNeuronAssociation(neuronA, neuronB, hebbianDelta);
            }
        }
    }
}

void NeuralAssociativeMemory::decay(float decayRate) {
    // Decay all association strengths
    for (auto& pair : pImpl->neuronPairStrength) {
        pair.second *= (1.0f - decayRate * developmentalModulation);
    }
    
    // Prune weak associations
    pruneWeakAssociations(0.01f);
}

void NeuralAssociativeMemory::updatePatternActivations(const std::vector<NeuronId>& pattern,
                                                       float activationLevel) {
    // Find pattern index
    size_t patternIndex = findPatternIndex(pattern);
    
    if (patternIndex != SIZE_MAX) {
        // Update activation with exponential moving average
        pImpl->patternActivations[patternIndex] = 
            pImpl->patternActivations[patternIndex] * 0.9f + activationLevel * 0.1f;
        
        // Add to activation queue
        pImpl->activationQueue.push(std::make_pair(patternIndex, static_cast<SimulationStep>(0)));
    }
}

void NeuralAssociativeMemory::pruneWeakAssociations(float threshold) {
    for (auto it = pImpl->neuronPairStrength.begin(); it != pImpl->neuronPairStrength.end(); ) {
        if (it->second < threshold) {
            it = pImpl->neuronPairStrength.erase(it);
        } else {
            ++it;
        }
    }
}

void NeuralAssociativeMemory::updateAssociationMatrix() {
    // Rebuild association matrix from neuron pair strengths
    pImpl->associationMatrix.clear();
    
    // Group neuron pairs by their pattern
    std::unordered_map<NeuronId, size_t> neuronToPattern;
    std::vector<std::vector<NeuronId>> neuronGroups;
    
    for (const auto& pair : pImpl->neuronPairStrength) {
        NeuronId neuronA = static_cast<NeuronId>((pair.first >> 32) & 0xFFFFFFFF);
        
        if (neuronToPattern.find(neuronA) == neuronToPattern.end()) {
            neuronToPattern[neuronA] = neuronGroups.size();
            neuronGroups.push_back(std::vector<NeuronId>());
        }
        
        neuronGroups[neuronToPattern[neuronA]].push_back(neuronA);
    }
    
    // Create patterns from neuron groups
    for (const auto& group : neuronGroups) {
        size_t patternIndex = pImpl->patterns.size();
        
        std::vector<float> neuralPattern;
        convertNeuronIdsToNeuralPattern(group, neuralPattern);
        
        pImpl->patterns.push_back(neuralPattern);
        pImpl->patternNeuronIds.push_back(group);
        pImpl->patternLabels.push_back("auto_pattern_" + std::to_string(patternIndex));
        pImpl->patternActivations.push_back(0.0f);
        
        pImpl->labelToIndex["auto_pattern_" + std::to_string(patternIndex)] = patternIndex;
    }
    
    // Calculate association strengths between patterns
    pImpl->associationMatrix.resize(pImpl->patterns.size(), std::vector<float>(pImpl->patterns.size(), 0.0f));
    
    for (size_t i = 0; i < pImpl->patterns.size(); ++i) {
        for (size_t j = i + 1; j < pImpl->patterns.size(); ++j) {
            float strength = calculatePatternAssociation(pImpl->patterns[i], pImpl->patterns[j]);
            pImpl->associationMatrix[i][j] = strength;
            pImpl->associationMatrix[j][i] = strength;
        }
    }
}

void NeuralAssociativeMemory::normalizeAssociationMatrix(size_t indexA, size_t indexB) {
    if (indexA >= pImpl->associationMatrix.size() || indexB >= pImpl->associationMatrix.size()) {
        return;
    }
    
    // Apply Oja's rule: prevent runaway growth
    float maxWeight = pImpl->associationMatrix[indexA][indexB];
    
    // Find maximum weight in the matrix
    for (size_t i = 0; i < pImpl->associationMatrix.size(); ++i) {
        for (size_t j = 0; j < pImpl->associationMatrix[i].size(); ++j) {
            if (pImpl->associationMatrix[i][j] > maxWeight) {
                maxWeight = pImpl->associationMatrix[i][j];
            }
        }
    }
    
    // If max weight is too high, scale down
    if (maxWeight > 1.0f) {
        float scaleFactor = 1.0f / maxWeight;
        for (size_t i = 0; i < pImpl->associationMatrix.size(); ++i) {
            for (size_t j = 0; j < pImpl->associationMatrix[i].size(); ++j) {
                pImpl->associationMatrix[i][j] *= scaleFactor;
            }
        }
    }
}

size_t NeuralAssociativeMemory::findOrCreatePattern(const std::vector<float>& pattern) {
    // Find most similar existing pattern
    size_t bestMatch = findMostSimilarPattern(pattern);
    
    if (bestMatch != SIZE_MAX) {
        return bestMatch;
    }
    
    // Create new pattern
    size_t newIndex = pImpl->patterns.size();
    pImpl->patterns.push_back(pattern);
    pImpl->patternNeuronIds.push_back(std::vector<NeuronId>());  // Will be filled later
    pImpl->patternLabels.push_back("pattern_" + std::to_string(newIndex));
    pImpl->patternActivations.push_back(0.0f);
    
    pImpl->labelToIndex["pattern_" + std::to_string(newIndex)] = newIndex;
    
    // Expand association matrix
    pImpl->associationMatrix.resize(newIndex + 1);
    for (size_t i = 0; i < pImpl->associationMatrix.size(); ++i) {
        pImpl->associationMatrix[i].resize(newIndex + 1, 0.0f);
    }
    
    return newIndex;
}

size_t NeuralAssociativeMemory::findPatternIndex(const std::vector<NeuronId>& pattern) const {
    for (size_t i = 0; i < pImpl->patternNeuronIds.size(); ++i) {
        if (pImpl->patternNeuronIds[i] == pattern) {
            return i;
        }
    }
    return SIZE_MAX;
}

size_t NeuralAssociativeMemory::findMostSimilarPattern(const std::vector<float>& pattern) const {
    if (pImpl->patterns.empty()) {
        return SIZE_MAX;
    }
    
    float bestSimilarity = 0.0f;
    size_t bestIndex = SIZE_MAX;
    
    for (size_t i = 0; i < pImpl->patterns.size(); ++i) {
        float similarity = calculatePatternSimilarity(pImpl->patterns[i], pattern);
        if (similarity > bestSimilarity && similarity > 0.3f) {
            bestSimilarity = similarity;
            bestIndex = i;
        }
    }
    
    return bestIndex;
}

void NeuralAssociativeMemory::convertNeuronIdsToNeuralPattern(const std::vector<NeuronId>& neuronIds,
                                                             std::vector<float>& neuralPattern) {
    neuralPattern.clear();
    
    for (NeuronId neuronId : neuronIds) {
        // Find the neuron and extract its properties
        for (auto* region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron->getId() == neuronId) {
                        // Extract key neural properties
                        float membranePotential = neuron->getState().membranePotential;
                        float firingRate = neuron->getState().firingRate;
                        float threshold = neuron->getState().threshold;
                        
                        // Normalize and add to pattern
                        neuralPattern.push_back((membranePotential + 100.0f) / 200.0f);  // -100 to +100 -> 0 to 1
                        neuralPattern.push_back(std::min(1.0f, firingRate / 100.0f));   // Normalize firing rate
                        neuralPattern.push_back((threshold + 100.0f) / 200.0f);         // Normalize threshold
                        
                        break;
                    }
                }
            }
        }
    }
}

float NeuralAssociativeMemory::calculatePatternSimilarity(const std::vector<float>& pattern1, const std::vector<float>& pattern2) const {
    if (pattern1.size() != pattern2.size()) return 0.0f;
    
    float dotProduct = 0.0f;
    float norm1 = 0.0f;
    float norm2 = 0.0f;
    
    for (size_t i = 0; i < pattern1.size(); ++i) {
        dotProduct += pattern1[i] * pattern2[i];
        norm1 += pattern1[i] * pattern1[i];
        norm2 += pattern2[i] * pattern2[i];
    }
    
    if (norm1 == 0.0f || norm2 == 0.0f) return 0.0f;
    
    return dotProduct / std::sqrt(norm1 * norm2);
}

float NeuralAssociativeMemory::calculatePatternAssociation(const std::vector<float>& pattern1,
                                                         const std::vector<float>& pattern2) const {
    float similarity = calculatePatternSimilarity(pattern1, pattern2);
    
    // Convert similarity to association strength using sigmoid function
    return 1.0f / (1.0f + std::exp(-10.0f * (similarity - 0.5f)));
}

void NeuralAssociativeMemory::updateNeuronAssociation(NeuronId neuronA, NeuronId neuronB, float delta) {
    uint64_t key1 = (static_cast<uint64_t>(neuronA.getId()) << 32) | neuronB.getId();
    uint64_t key2 = (static_cast<uint64_t>(neuronB.getId()) << 32) | neuronA.getId();
    
    if (pImpl->neuronPairStrength.find(key1) != pImpl->neuronPairStrength.end()) {
        pImpl->neuronPairStrength[key1] = std::clamp(pImpl->neuronPairStrength[key1] + delta, 0.0f, 1.0f);
    }
    if (pImpl->neuronPairStrength.find(key2) != pImpl->neuronPairStrength.end()) {
        pImpl->neuronPairStrength[key2] = std::clamp(pImpl->neuronPairStrength[key2] + delta, 0.0f, 1.0f);
    }
}

void NeuralAssociativeMemory::setHebbianLearningRate(float rate) {
    hebbianLearningRate = std::clamp(rate, 0.0f, 0.1f);
}

void NeuralAssociativeMemory::setDecayRate(float rate) {
    decayRate = std::clamp(rate, 0.0f, 0.1f);
}

void NeuralAssociativeMemory::setDevelopmentalModulation(float modulation) {
    developmentalModulation = std::clamp(modulation, 0.0f, 3.0f);
}

float NeuralAssociativeMemory::getHebbianLearningRate() const {
    return hebbianLearningRate;
}

float NeuralAssociativeMemory::getDecayRate() const {
    return decayRate;
}

float NeuralAssociativeMemory::getDevelopmentalModulation() const {
    return developmentalModulation;
}

void NeuralAssociativeMemory::activatePattern(const std::vector<NeuronId>& pattern) {
    // Find pattern index
    size_t patternIndex = findPatternIndex(pattern);
    
    if (patternIndex != SIZE_MAX) {
        // Increase activation
        pImpl->patternActivations[patternIndex] = std::min(1.0f, 
            pImpl->patternActivations[patternIndex] + 0.1f);
        
        // Add to activation queue
        pImpl->activationQueue.push(std::make_pair(patternIndex, static_cast<SimulationStep>(0)));
        
        // Update associated patterns
        for (size_t i = 0; i < pImpl->associationMatrix.size(); ++i) {
            if (i != patternIndex && pImpl->associationMatrix[patternIndex][i] > 0.1f) {
                pImpl->patternActivations[i] = std::min(1.0f, 
                    pImpl->patternActivations[i] + pImpl->associationMatrix[patternIndex][i] * 0.05f);
            }
        }
    }
}

std::vector<size_t> NeuralAssociativeMemory::getActivePatternIndices() const {
    std::vector<size_t> activeIndices;
    
    for (size_t i = 0; i < pImpl->patternActivations.size(); ++i) {
        if (pImpl->patternActivations[i] > 0.1f) {
            activeIndices.push_back(i);
        }
    }
    
    return activeIndices;
}

} // namespace nlm
