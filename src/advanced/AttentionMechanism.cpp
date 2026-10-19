// Attention Mechanism Implementation for NLM

#include "AttentionMechanism.hpp"
#include <cmath>
#include <random>
#include <algorithm>
#include <numeric>
#include <sstream>

namespace nlm {

struct AttentionMechanism::Impl {
    size_t numHeads;
    size_t embeddingSize;
    float dropoutRate;
    AttentionType attentionType;
    
    // Multi-head attention components
    std::vector<AttentionHead> heads;
    std::vector<std::vector<float>> headOutputs;
    
    // Statistics
    size_t totalAttentionApplications;
    float totalAttentionScore;
    std::vector<float> headAttentionWeights;
    
    // Random number generator for dropout
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    
    Impl(size_t nh, size_t es, float dr, AttentionType at) 
        : numHeads(nh), embeddingSize(es), dropoutRate(dr), attentionType(at),
          totalAttentionApplications(0), totalAttentionScore(0.0f), uniformDist(0.0f, 1.0f) {
        
        // Initialize attention heads
        heads.reserve(numHeads);
        headOutputs.reserve(numHeads);
        headAttentionWeights.reserve(numHeads);
        
        for (size_t i = 0; i < numHeads; ++i) {
            size_t headSize = embeddingSize / numHeads;
            if (headSize < 1) headSize = 1;
            heads.emplace_back(headSize, embeddingSize, dropoutRate);
            headOutputs.push_back(std::vector<float>(headSize));
            headAttentionWeights.push_back(1.0f / static_cast<float>(numHeads));
        }
        
        // Initialize random number generator
        std::random_device rd;
        rng.seed(rd());
    }
    
    ~Impl() {
        // Clean up resources
    }
};

AttentionMechanism::AttentionMechanism(size_t numHeads, size_t embeddingSize, 
                                     float dropoutRate, AttentionType type)
    : pImpl(std::make_unique<Impl>(numHeads, embeddingSize, dropoutRate, type)) {}

AttentionMechanism::~AttentionMechanism() = default;

AttentionMechanism::AttentionMechanism(AttentionMechanism&&) noexcept = default;

AttentionMechanism& AttentionMechanism::operator=(AttentionMechanism&&) noexcept = default;

std::vector<float> AttentionMechanism::applyAttention(const std::vector<float>& query,
                                                     const std::vector<float>& key,
                                                     const std::vector<float>& value,
                                                     bool training) {
    pImpl->totalAttentionApplications++;
    
    // Simple scaled dot-product attention
    size_t seqLen = key.size() / pImpl->embeddingSize;
    std::vector<float> output;
    
    for (size_t i = 0; i < seqLen; ++i) {
        // Extract query vector
        std::vector<float> q(pImpl->embeddingSize);
        size_t qOffset = 0;
        for (size_t h = 0; h < pImpl->numHeads; ++h) {
            size_t headSize = pImpl->embeddingSize / pImpl->numHeads;
            for (size_t j = 0; j < headSize; ++j) {
                q[j + qOffset] = key[i * pImpl->embeddingSize + j + qOffset];
            }
            qOffset += headSize;
        }
        
        // Compute attention scores
        float attentionScore = 0.0f;
        for (size_t j = 0; j < pImpl->embeddingSize; ++j) {
            attentionScore += query[j] * key[i * pImpl->embeddingSize + j];
        }
        attentionScore /= std::sqrt(static_cast<float>(pImpl->embeddingSize));
        
        pImpl->totalAttentionScore += std::abs(attentionScore);
        
        // Apply softmax
        std::vector<float> attentionWeights(seqLen);
        float sumExp = 0.0f;
        for (size_t j = 0; j < seqLen; ++j) {
            float score = 0.0f;
            for (size_t k = 0; k < pImpl->embeddingSize; ++k) {
                score += query[k] * key[j * pImpl->embeddingSize + k];
            }
            score /= std::sqrt(static_cast<float>(pImpl->embeddingSize));
            attentionWeights[j] = std::exp(score);
            sumExp += attentionWeights[j];
        }
        
        for (size_t j = 0; j < seqLen; ++j) {
            attentionWeights[j] /= sumExp;
        }
        
        // Apply dropout during training
        if (training) {
            for (size_t j = 0; j < seqLen; ++j) {
                if (pImpl->uniformDist(pImpl->rng) < pImpl->dropoutRate) {
                    attentionWeights[j] = 0.0f;
                }
            }
            
            // Renormalize after dropout
            float newSum = 0.0f;
            for (float w : attentionWeights) {
                newSum += w;
            }
            if (newSum > 0.0f) {
                for (float& w : attentionWeights) {
                    w /= newSum;
                }
            }
        }
        
        // Compute weighted sum of values
        std::vector<float> weightedValue(pImpl->embeddingSize, 0.0f);
        for (size_t j = 0; j < seqLen; ++j) {
            float weight = attentionWeights[j];
            for (size_t k = 0; k < pImpl->embeddingSize; ++k) {
                weightedValue[k] += weight * value[j * pImpl->embeddingSize + k];
            }
        }
        
        output.insert(output.end(), weightedValue.begin(), weightedValue.end());
    }
    
    return output;
}

std::vector<float> AttentionMechanism::applySelfAttention(const std::vector<float>& input,
                                                          bool training) {
    // For self-attention, query = key = value = input
    return applyAttention(input, input, input, training);
}

std::vector<float> AttentionMechanism::applyCrossAttention(const std::vector<float>& query,
                                                          const std::vector<float>& key,
                                                          const std::vector<float>& value,
                                                          bool training) {
    return applyAttention(query, key, value, training);
}

std::vector<float> AttentionMechanism::applyMultiHeadAttention(const std::vector<float>& input,
                                                               size_t numHeads,
                                                               bool training) {
    if (pImpl->numHeads == 0 || pImpl->embeddingSize == 0) {
        return input;
    }
    
    // For multi-head attention, we split the input into heads
    size_t headSize = pImpl->embeddingSize / pImpl->numHeads;
    std::vector<std::vector<float>> headOutputs(pImpl->numHeads, std::vector<float>(headSize));
    
    // Split input into heads
    size_t inputSize = input.size();
    for (size_t i = 0; i < inputSize; ++i) {
        size_t headIndex = i / headSize;
        size_t headOffset = i % headSize;
        if (headIndex < pImpl->numHeads) {
            headOutputs[headIndex][headOffset] = input[i];
        }
    }
    
    // Apply self-attention to each head
    std::vector<float> concatenatedOutput;
    for (size_t h = 0; h < pImpl->numHeads; ++h) {
        std::vector<float> headOutput = applySelfAttention(headOutputs[h], training);
        concatenatedOutput.insert(concatenatedOutput.end(), headOutput.begin(), headOutput.end());
    }
    
    return concatenatedOutput;
}

std::vector<float> AttentionMechanism::applyHierarchicalAttention(const std::vector<float>& input,
                                                                  size_t levels,
                                                                  bool training) {
    // Hierarchical attention processes input at multiple levels
    std::vector<float> currentInput = input;
    std::vector<float> hierarchicalOutput = input;
    
    for (size_t level = 0; level < levels; ++level) {
        // Apply attention at current level
        std::vector<float> levelOutput = applySelfAttention(currentInput, training);
        
        // Combine with hierarchical output
        for (size_t i = 0; i < levelOutput.size() && i < hierarchicalOutput.size(); ++i) {
            hierarchicalOutput[i] = (levelOutput[i] + hierarchicalOutput[i]) / 2.0f;
        }
        
        // Prepare for next level (simplified downsampling)
        if (currentInput.size() > 1) {
            currentInput.resize(currentInput.size() / 2);
        }
    }
    
    return hierarchicalOutput;
}

std::vector<float> AttentionMechanism::applyModulatedAttention(const std::vector<float>& query,
                                                              const std::vector<float>& key,
                                                              const std::vector<float>& value,
                                                              const std::vector<float>& plasticityWeights,
                                                              bool training) {
    // Apply attention with plasticity modulation
    std::vector<float> output = applyAttention(query, key, value, training);
    
    // Modulate output with plasticity weights
    for (size_t i = 0; i < output.size() && i < plasticityWeights.size(); ++i) {
        output[i] *= plasticityWeights[i];
    }
    
    return output;
}

std::vector<float> AttentionMechanism::applyActionAttention(const std::vector<float>& state,
                                                            const std::vector<float>& actionValue,
                                                            bool training) {
    // Attention for action selection
    std::vector<float> query = state;
    std::vector<float> key = actionValue;
    std::vector<float> value = actionValue;
    
    return applyAttention(query, key, value, training);
}

std::vector<float> AttentionMechanism::applyMemoryAttention(const std::vector<float>& query,
                                                            const std::vector<std::vector<float>>& memoryKeys,
                                                            const std::vector<std::vector<float>>& memoryValues,
                                                            bool training) {
    // Attention for memory readout
    std::vector<float> flattenedKeys;
    std::vector<float> flattenedValues;
    
    for (const auto& key : memoryKeys) {
        flattenedKeys.insert(flattenedKeys.end(), key.begin(), key.end());
    }
    
    for (const auto& value : memoryValues) {
        flattenedValues.insert(flattenedValues.end(), value.begin(), value.end());
    }
    
    return applyAttention(query, flattenedKeys, flattenedValues, training);
}

void AttentionMechanism::updateAttention(const std::vector<float>& experience,
                                        float learningRate) {
    // Update attention weights based on experience
    // Simplified implementation
    pImpl->totalAttentionScore += learningRate * std::abs(experience[0]);
}

std::vector<std::vector<float>> AttentionMechanism::getAttentionWeights() const {
    // Return attention weights for interpretability
    std::vector<std::vector<float>> weights;
    
    if (pImpl->totalAttentionApplications > 0) {
        float avgScore = pImpl->totalAttentionScore / pImpl->totalAttentionApplications;
        
        for (size_t i = 0; i < pImpl->numHeads; ++i) {
            weights.push_back(std::vector<float>(pImpl->embeddingSize / pImpl->numHeads, avgScore));
        }
    }
    
    return weights;
}

std::vector<float> AttentionMechanism::getAttentionScores() const {
    // Return attention scores
    std::vector<float> scores;
    if (pImpl->totalAttentionApplications > 0) {
        float avgScore = pImpl->totalAttentionScore / pImpl->totalAttentionApplications;
        scores.resize(pImpl->totalAttentionApplications, avgScore);
    }
    return scores;
}

void AttentionMechanism::reset() {
    pImpl->totalAttentionApplications = 0;
    pImpl->totalAttentionScore = 0.0f;
    std::fill(pImpl->headAttentionWeights.begin(), pImpl->headAttentionWeights.end(), 
             1.0f / static_cast<float>(pImpl->numHeads));
}

AttentionMechanism::Statistics AttentionMechanism::getStatistics() const {
    Statistics stats;
    stats.totalAttentionApplications = pImpl->totalAttentionApplications;
    stats.averageAttentionScore = (pImpl->totalAttentionApplications > 0) ? 
        pImpl->totalAttentionScore / pImpl->totalAttentionApplications : 0.0f;
    stats.headAttentionWeights = pImpl->headAttentionWeights;
    return stats;
}

void AttentionMechanism::setAttentionType(AttentionType type) {
    pImpl->attentionType = type;
}

AttentionType AttentionMechanism::getAttentionType() const {
    return pImpl->attentionType;
}

void AttentionMechanism::setNumHeads(size_t numHeads) {
    pImpl->numHeads = numHeads;
    pImpl->headAttentionWeights.resize(numHeads);
    std::fill(pImpl->headAttentionWeights.begin(), pImpl->headAttentionWeights.end(), 
             1.0f / static_cast<float>(numHeads));
}

void AttentionMechanism::setEmbeddingSize(size_t embeddingSize) {
    pImpl->embeddingSize = embeddingSize;
}

void AttentionMechanism::setDropoutRate(float dropoutRate) {
    pImpl->dropoutRate = dropoutRate;
    for (auto& head : pImpl->heads) {
        head.dropoutRate = dropoutRate;
    }
}

size_t AttentionMechanism::getNumHeads() const {
    return pImpl->numHeads;
}

size_t AttentionMechanism::getEmbeddingSize() const {
    return pImpl->embeddingSize;
}

float AttentionMechanism::getDropoutRate() const {
    return pImpl->dropoutRate;
}

// HierarchicalProcessing Implementation

struct HierarchicalProcessing::Impl {
    size_t numLevels;
    size_t baseFilterSize;
    float downsampleFactor;
    
    std::vector<std::vector<float>> hierarchicalFeatures;
    
    Impl(size_t nl, size_t bfs, float dsf) 
        : numLevels(nl), baseFilterSize(bfs), downsampleFactor(dsf) {
        hierarchicalFeatures.reserve(nl);
    }
};

HierarchicalProcessing::HierarchicalProcessing(size_t numLevels, size_t baseFilterSize, 
                                             float downsampleFactor)
    : pImpl(std::make_unique<Impl>(numLevels, baseFilterSize, downsampleFactor)) {}

HierarchicalProcessing::~HierarchicalProcessing() = default;

HierarchicalProcessing::HierarchicalProcessing(HierarchicalProcessing&&) noexcept = default;

HierarchicalProcessing& HierarchicalProcessing::operator=(HierarchicalProcessing&&) noexcept = default;

std::vector<std::vector<float>> HierarchicalProcessing::extractHierarchicalFeatures(const std::vector<float>& input,
                                                                                  size_t levels) {
    pImpl->hierarchicalFeatures.clear();
    
    std::vector<float> currentInput = input;
    for (size_t level = 0; level < levels && level < pImpl->numLevels; ++level) {
        // Apply filter at current level
        size_t filterSize = pImpl->baseFilterSize * std::pow(2, level);
        std::vector<float> filteredFeatures;
        
        for (size_t i = 0; i < currentInput.size(); i += filterSize) {
            if (i + filterSize <= currentInput.size()) {
                // Simple averaging filter
                float sum = 0.0f;
                for (size_t j = 0; j < filterSize && i + j < currentInput.size(); ++j) {
                    sum += currentInput[i + j];
                }
                filteredFeatures.push_back(sum / std::min(filterSize, currentInput.size() - i));
            }
        }
        
        pImpl->hierarchicalFeatures.push_back(filteredFeatures);
        
        // Downsample for next level
        if (currentInput.size() > 1) {
            currentInput.resize(currentInput.size() / pImpl->downsampleFactor);
        }
    }
    
    return pImpl->hierarchicalFeatures;
}

std::vector<float> HierarchicalProcessing::multiScaleProcessing(const std::vector<float>& input,
                                                                const std::vector<size_t>& filterSizes) {
    std::vector<float> processedOutput;
    
    for (size_t filterSize : filterSizes) {
        std::vector<float> filteredInput;
        for (size_t i = 0; i < input.size(); i += filterSize) {
            if (i + filterSize <= input.size()) {
                float sum = 0.0f;
                for (size_t j = 0; j < filterSize; ++j) {
                    sum += input[i + j];
                }
                filteredInput.push_back(sum / filterSize);
            }
        }
        
        // Combine with previous output
        if (!processedOutput.empty()) {
            for (size_t i = 0; i < filteredInput.size() && i < processedOutput.size(); ++i) {
                processedOutput[i] = (filteredInput[i] + processedOutput[i]) / 2.0f;
            }
        } else {
            processedOutput = filteredInput;
        }
    }
    
    return processedOutput;
}

std::vector<float> HierarchicalProcessing::hierarchicalSkipConnections(const std::vector<std::vector<float>>& features) {
    std::vector<float> skipConnectedOutput;
    
    if (!features.empty()) {
        // Start with the deepest level features
        skipConnectedOutput = features.back();
        
        // Add skip connections from shallower levels
        for (size_t i = features.size() - 2; i < features.size(); --i) {
            const auto& levelFeatures = features[i];
            for (size_t j = 0; j < levelFeatures.size() && j < skipConnectedOutput.size(); ++j) {
                skipConnectedOutput[j] += levelFeatures[j];
            }
        }
        
        // Normalize
        for (float& value : skipConnectedOutput) {
            value /= features.size();
        }
    }
    
    return skipConnectedOutput;
}

std::vector<float> HierarchicalProcessing::hierarchicalUpsampling(const std::vector<std::vector<float>>& features,
                                                                size_t targetLevel) {
    if (targetLevel >= features.size()) {
        return std::vector<float>();
    }
    
    std::vector<float> upsampled = features[targetLevel];
    
    // Upsample by repeating elements (simplified)
    std::vector<float> result;
    for (float value : upsampled) {
        result.push_back(value);
        result.push_back(value);  // Duplicate for upsampling
    }
    
    return result;
}

std::vector<float> HierarchicalProcessing::hierarchicalAttention(const std::vector<float>& input,
                                                                size_t numAttentionLevels) {
    // Apply hierarchical attention
    std::vector<float> attentionOutput = input;
    
    for (size_t level = 0; level < numAttentionLevels && level < pImpl->numLevels; ++level) {
        // Apply attention at this level
        std::vector<float> levelOutput = input;
        for (size_t i = 1; i < levelOutput.size(); ++i) {
            levelOutput[i] = (levelOutput[i - 1] + levelOutput[i]) / 2.0f;
        }
        
        // Blend with previous output
        for (size_t i = 0; i < levelOutput.size() && i < attentionOutput.size(); ++i) {
            attentionOutput[i] = (levelOutput[i] + attentionOutput[i]) / 2.0f;
        }
    }
    
    return attentionOutput;
}

void HierarchicalProcessing::applyHierarchicalPlasticity(std::vector<std::vector<float>>& weights,
                                                        const std::vector<float>& errorSignals,
                                                        float learningRate) {
    for (auto& weightMatrix : weights) {
        for (size_t i = 0; i < weightMatrix.size(); ++i) {
            // Apply plasticity rule
            weightMatrix[i] += learningRate * errorSignals[i % errorSignals.size()];
        }
    }
}

void HierarchicalProcessing::hierarchicalDevelopment(std::vector<std::vector<float>>& weights,
                                                    size_t developmentalStage) {
    // Apply developmental changes based on stage
    switch (developmentalStage) {
        case 0: // Initial stage - small random changes
            for (auto& weightMatrix : weights) {
                for (float& weight : weightMatrix) {
                    weight *= 1.1f; // 10% increase
                }
            }
            break;
            
        case 1: // Critical period - larger changes
            for (auto& weightMatrix : weights) {
                for (float& weight : weightMatrix) {
                    weight *= 1.5f; // 50% increase
                }
            }
            break;
            
        case 2: // Maturation - stabilization
            for (auto& weightMatrix : weights) {
                for (float& weight : weightMatrix) {
                    weight *= 1.05f; // 5% increase
                }
            }
            break;
            
        default: // Adult - minimal changes
            for (auto& weightMatrix : weights) {
                for (float& weight : weightMatrix) {
                    weight *= 1.01f; // 1% increase
                }
            }
            break;
    }
}

std::vector<std::vector<float>> HierarchicalProcessing::getHierarchicalFeatures() const {
    return pImpl->hierarchicalFeatures;
}

std::vector<HierarchicalProcessing::LevelStatistics> HierarchicalProcessing::getLevelStatistics() const {
    std::vector<LevelStatistics> stats;
    
    for (size_t i = 0; i < pImpl->hierarchicalFeatures.size(); ++i) {
        LevelStatistics levelStat;
        levelStat.level = i;
        levelStat.filterSize = pImpl->baseFilterSize * std::pow(2, i);
        
        const auto& features = pImpl->hierarchicalFeatures[i];
        if (!features.empty()) {
            float sum = std::accumulate(features.begin(), features.end(), 0.0f);
            levelStat.activationMean = sum / features.size();
            
            float sumSq = 0.0f;
            for (float val : features) {
                float diff = val - levelStat.activationMean;
                sumSq += diff * diff;
            }
            levelStat.activationStd = std::sqrt(sumSq / features.size());
            
            // Calculate sparsity
            size_t nonZero = 0;
            for (float val : features) {
                if (std::abs(val) > 0.01f) {
                    nonZero++;
                }
            }
            levelStat.sparsity = static_cast<float>(nonZero) / features.size();
        }
        
        stats.push_back(levelStat);
    }
    
    return stats;
}

void HierarchicalProcessing::reset() {
    pImpl->hierarchicalFeatures.clear();
}

void HierarchicalProcessing::setNumLevels(size_t numLevels) {
    pImpl->numLevels = numLevels;
}

void HierarchicalProcessing::setBaseFilterSize(size_t filterSize) {
    pImpl->baseFilterSize = filterSize;
}

void HierarchicalProcessing::setDownsampleFactor(float factor) {
    pImpl->downsampleFactor = factor;
}

size_t HierarchicalProcessing::getNumLevels() const {
    return pImpl->numLevels;
}

size_t HierarchicalProcessing::getBaseFilterSize() const {
    return pImpl->baseFilterSize;
}

float HierarchicalProcessing::getDownsampleFactor() const {
    return pImpl->downsampleFactor;
}

// Factory functions implementation
namespace nlm {
    AttentionMechanism makeAttentionMechanism(AttentionType type) {
        return AttentionMechanism(8, 512, 0.1, type);
    }
    
    HierarchicalProcessing makeHierarchicalProcessing(size_t levels) {
        return HierarchicalProcessing(levels);
    }
    
    AttentionModulatedPlasticity makeAttentionModulatedPlasticity() {
        return AttentionModulatedPlasticity();
    }
    
    MultiModalAttention makeMultiModalAttention() {
        return MultiModalAttention();
    }
    
    ActionSelectionWithAttention makeActionSelectionWithAttention(size_t actionSpaceSize) {
        return ActionSelectionWithAttention(actionSpaceSize);
    }
}

} // namespace nlm