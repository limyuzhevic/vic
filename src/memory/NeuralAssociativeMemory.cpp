#include "NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>
#include <unordered_map>
#include <numeric>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    Brain* brain;
    
    // Pattern completion network
    std::vector<std::vector<float>> weightMatrix;
    
    // Activation vectors
    std::vector<std::vector<float>> activationVectors;
    
    // Concept system
    std::vector<std::vector<size_t>> conceptAssociations;
    std::vector<std::vector<float>> conceptFeatures;
    
    // Temporal sequence storage
    std::vector<std::vector<std::vector<float>>> sequences;
    
    // Statistical measures
    float predictionAccuracy;
    float patternSeparation;
    float memoryUtilization;
    
    // Learning parameters
    float learningRate;
    float decayRate;
    float completionThreshold;
    size_t maxAssociationsPerPattern;
    
    // Performance tracking
    size_t totalPatternLoads;
    size_t totalPatternStores;
    size_t totalCompletions;
    
    Impl() : brain(nullptr), predictionAccuracy(0.0f), patternSeparation(0.0f), 
             memoryUtilization(0.0f), learningRate(0.01f), decayRate(0.001f), 
             completionThreshold(0.7f), maxAssociationsPerPattern(10),
             totalPatternLoads(0), totalPatternStores(0), totalCompletions(0) {
        std::random_device rd;
        rng.seed(rd());
    }
    
    std::mt19937 rng;
    std::chrono::system_clock::time_point lastCleanupTime;
};

NeuralAssociativeMemory::NeuralAssociativeMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
{
    // Initialize with random seed for biological realism
    std::random_device rd;
    pImpl->rng.seed(rd());
    pImpl->lastCleanupTime = std::chrono::system_clock::now();
    
    // Initialize with reasonable defaults
    pImpl->learningRate = 0.01f;
    pImpl->decayRate = 0.001f;
    pImpl->completionThreshold = 0.7f;
    pImpl->maxAssociationsPerPattern = 10;
}

NeuralAssociativeMemory::~NeuralAssociativeMemory() = default;

void NeuralAssociativeMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    if (brain_) {
        // Initialize pattern database with reasonable size
        size_t patternCapacity = 100;
        pImpl->activationVectors.reserve(patternCapacity);
        pImpl->weightMatrix.reserve(patternCapacity * patternCapacity);
        pImpl->conceptAssociations.reserve(patternCapacity);
        pImpl->conceptFeatures.reserve(patternCapacity);
        
        // Create initial patterns for common sensory inputs
        createInitialPatterns();
        
        NLM_LOG_INFO("NeuralAssociativeMemory initialized with pattern completion network");
    }
}

void NeuralAssociativeMemory::createInitialPatterns() {
    // Create initial patterns for basic sensory inputs and concepts
    // This simulates biological preparation of associative memory
    
    // Visual pattern space
    std::vector<float> visualPattern1 = {0.8f, 0.6f, 0.4f, 0.2f, 0.1f};
    std::vector<float> visualPattern2 = {0.3f, 0.7f, 0.5f, 0.8f, 0.2f};
    std::vector<float> visualPattern3 = {0.9f, 0.1f, 0.3f, 0.6f, 0.4f};
    
    // Auditory pattern space
    std::vector<float> audioPattern1 = {0.5f, 0.3f, 0.7f, 0.4f};
    std::vector<float> audioPattern2 = {0.8f, 0.6f, 0.2f, 0.9f};
    std::vector<float> audioPattern3 = {0.4f, 0.5f, 0.8f, 0.3f};
    
    // Internal state patterns
    std::vector<float> internalPattern1 = {0.6f, 0.4f, 0.8f, 0.1f, 0.3f};
    std::vector<float> internalPattern2 = {0.7f, 0.5f, 0.2f, 0.9f, 0.1f};
    
    // Store initial patterns
    std::vector<std::vector<float>> initialPatterns = {
        visualPattern1, visualPattern2, visualPattern3,
        audioPattern1, audioPattern2, audioPattern3,
        internalPattern1, internalPattern2
    };
    
    for (const auto& pattern : initialPatterns) {
        loadPatterns({pattern});
    }
    
    // Create associations between related patterns
    createBasicAssociations();
}

void NeuralAssociativeMemory::createBasicAssociations() {
    // Create basic associations between related patterns
    // This simulates biological pattern association learning
    
    // Associate similar visual patterns
    associate(visualPattern1, visualPattern2, 0.8f);
    associate(visualPattern2, visualPattern3, 0.7f);
    
    // Associate similar audio patterns
    associate(audioPattern1, audioPattern2, 0.8f);
    associate(audioPattern2, audioPattern3, 0.6f);
    
    // Cross-modal associations
    associate(visualPattern1, audioPattern1, 0.5f);
    associate(internalPattern1, visualPattern1, 0.6f);
}

void NeuralAssociativeMemory::associate(const std::vector<float>& patternA,
                                        const std::vector<float>& patternB,
                                        float strength) {
    if (patternA.empty() || patternB.empty() || !brain_) return;
    
    // Normalize patterns
    std::vector<float> normA = normalizePattern(patternA);
    std::vector<float> normB = normalizePattern(patternB);
    
    // Ensure patterns are different enough
    float similarity = calculatePatternSimilarity(normA, normB);
    if (similarity > 0.95f) {  // Too similar, don't associate
        return;
    }
    
    // Update weight matrix
    size_t patternSize = std::min(normA.size(), normB.size());
    for (size_t i = 0; i < patternSize; ++i) {
        for (size_t j = 0; j < patternSize; ++j) {
            float weightUpdate = strength * normA[i] * normB[j] * 
                               calculateHebbianFactor(normA, normB, i, j);
            
            // Ensure symmetry for Hopfield-like network
            if (pImpl->weightMatrix.size() > i) {
                pImpl->weightMatrix[i].resize(normA.size(), 0.0f);
            }
            if (pImpl->weightMatrix.size() > j) {
                pImpl->weightMatrix[j].resize(normA.size(), 0.0f);
            }
            
            pImpl->weightMatrix[i][j] += weightUpdate;
            pImpl->weightMatrix[j][i] = pImpl->weightMatrix[i][j];  // Symmetry
        }
    }
    
    // Store patterns in activation vectors
    pImpl->activationVectors.push_back(normA);
    pImpl->activationVectors.push_back(normB);
    
    pImpl->totalPatternStores++;
    
    // Update association statistics
    updateAssociationStatistics();
}

std::vector<float> NeuralAssociativeMemory::retrieveAssociations(
    const std::vector<float>& pattern,
    size_t maxResults) const {
    if (pattern.empty() || pImpl->activationVectors.empty()) return {};
    
    // Normalize input pattern
    std::vector<float> normPattern = normalizePattern(pattern);
    
    // Find similar patterns using weight matrix
    std::vector<std::pair<float, size_t>> similarities;
    
    for (size_t i = 0; i < pImpl->activationVectors.size(); ++i) {
        float similarity = calculatePatternSimilarity(normPattern, pImpl->activationVectors[i]);
        if (similarity > completionThreshold_) {
            similarities.emplace_back(similarity, i);
        }
    }
    
    // Sort by similarity (descending)
    std::sort(similarities.begin(), similarities.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Return top results (weighted average of similar patterns)
    std::vector<float> result;
    if (!similarities.empty()) {
        result.resize(normPattern.size(), 0.0f);
        float totalWeight = 0.0f;
        
        for (size_t i = 0; i < std::min(maxResults, similarities.size()); ++i) {
            float weight = similarities[i].first;
            totalWeight += weight;
            
            for (size_t j = 0; j < normPattern.size(); ++j) {
                if (similarities[i].second < pImpl->activationVectors.size() &&
                    j < pImpl->activationVectors[similarities[i].second].size()) {
                    result[j] += weight * pImpl->activationVectors[similarities[i].second][j];
                }
            }
        }
        
        // Normalize result
        float norm = std::sqrt(std::inner_product(result.begin(), result.end(), result.begin(), 0.0f));
        if (norm > 0.0f) {
            for (float& val : result) {
                val /= norm;
            }
        }
    }
    
    pImpl->totalCompletions++;
    return result;
}

std::vector<float> NeuralAssociativeMemory::completePattern(
    const std::vector<float>& partialPattern) const {
    // Complete partial pattern using attractor dynamics
    if (partialPattern.empty()) return {};
    
    // Start with partial pattern as initial state
    std::vector<float> currentState = normalizePattern(partialPattern);
    std::vector<float> nextState = currentState;
    
    // Iteratively update state using weight matrix
    for (int iteration = 0; iteration < 50; ++iteration) {
        nextState = currentState;
        
        for (size_t i = 0; i < currentState.size(); ++i) {
            float sum = 0.0f;
            
            for (size_t j = 0; j < currentState.size(); ++j) {
                sum += pImpl->weightMatrix[i][j] * currentState[j];
            }
            
            // Apply activation function
            nextState[i] = sigmoid(sum);
        }
        
        // Check for convergence
        float change = calculateVectorDifference(currentState, nextState);
        if (change < 0.001f) {
            break;
        }
        
        currentState = nextState;
    }
    
    return currentState;
}

float NeuralAssociativeMemory::getAssociationStrength(
    const std::vector<float>& patternA,
    const std::vector<float>& patternB) const {
    if (patternA.empty() || patternB.empty()) return 0.0f;
    
    std::vector<float> normA = normalizePattern(patternA);
    std::vector<float> normB = normalizePattern(patternB);
    
    return calculatePatternSimilarity(normA, normB);
}

void NeuralAssociativeMemory::updateAssociations(const std::vector<float>& pattern,
                                                 const std::vector<float>& outcome,
                                                 float learningRate) {
    if (pattern.empty() || outcome.empty() || !brain_) return;
    
    // Update based on reward prediction error
    float prediction = predictOutcome(pattern);
    float error = outcome[0] - prediction;  // Assume scalar outcome
    
    // Update weight matrix based on prediction error
    std::vector<float> normPattern = normalizePattern(pattern);
    std::vector<float> normOutcome = normalizePattern(outcome);
    
    for (size_t i = 0; i < normPattern.size(); ++i) {
        for (size_t j = 0; j < normOutcome.size(); ++j) {
            float weightUpdate = learningRate * error * normPattern[i] * normOutcome[j];
            if (i < pImpl->weightMatrix.size() && j < pImpl->weightMatrix[i].size()) {
                pImpl->weightMatrix[i][j] += weightUpdate;
                pImpl->weightMatrix[j][i] = pImpl->weightMatrix[i][j];  // Maintain symmetry
            }
        }
    }
}

std::vector<float> NeuralAssociativeMemory::getActivationVector(
    const std::vector<float>& pattern) const {
    if (pattern.empty()) return {};
    
    std::vector<float> normPattern = normalizePattern(pattern);
    
    // Find best matching stored pattern
    size_t bestIndex = 0;
    float bestSimilarity = 0.0f;
    
    for (size_t i = 0; i < pImpl->activationVectors.size(); ++i) {
        float similarity = calculatePatternSimilarity(normPattern, pImpl->activationVectors[i]);
        if (similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestIndex = i;
        }
    }
    
    return pImpl->activationVectors[bestIndex];
}

float NeuralAssociativeMemory::calculatePatternSimilarity(
    const std::vector<float>& patternA,
    const std::vector<float>& patternB) const {
    if (patternA.size() != patternB.size() || patternA.empty()) return 0.0f;
    
    // Cosine similarity
    float dotProduct = std::inner_product(patternA.begin(), patternA.end(), patternB.begin(), 0.0f);
    float normA = std::sqrt(std::inner_product(patternA.begin(), patternA.end(), patternA.begin(), 0.0f));
    float normB = std::sqrt(std::inner_product(patternB.begin(), patternB.end(), patternB.begin(), 0.0f));
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dotProduct / (normA * normB);
}

size_t NeuralAssociativeMemory::getAssociationCount() const {
    return pImpl->activationVectors.size();
}

std::vector<std::vector<float>> NeuralAssociativeMemory::getAllActivationVectors() const {
    return pImpl->activationVectors;
}

std::vector<float> NeuralAssociativeMemory::patternCompletion(
    const std::vector<float>& activation) const {
    return completePattern(activation);
}

void NeuralAssociativeMemory::reinforceAssociations(const std::vector<float>& pattern,
                                                  float reward,
                                                  float learningRate) {
    if (pattern.empty()) return;
    
    std::vector<float> normPattern = normalizePattern(pattern);
    
    // Find best matching stored pattern
    for (size_t i = 0; i < pImpl->activationVectors.size(); ++i) {
        float similarity = calculatePatternSimilarity(normPattern, pImpl->activationVectors[i]);
        if (similarity > completionThreshold_) {
            // Reinforce this association based on reward
            float reinforcement = reward * similarity * learningRate;
            
            // Update weight matrix for this pattern pair
            for (size_t j = 0; j < normPattern.size(); ++j) {
                if (j < pImpl->weightMatrix.size() && j < pImpl->weightMatrix[i].size()) {
                    pImpl->weightMatrix[i][j] += reinforcement;
                    pImpl->weightMatrix[j][i] = pImpl->weightMatrix[i][j];
                }
            }
        }
    }
}

void NeuralAssociativeMemory::decayAssociations(float decayRate) {
    if (decayRate <= 0.0f) return;
    
    // Gradually decay weight matrix
    for (auto& row : pImpl->weightMatrix) {
        for (float& weight : row) {
            weight *= (1.0f - decayRate);
        }
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::getActivationMap() const {
    // Create visualization of activation patterns
    std::vector<std::vector<float>> activationMap;
    
    if (pImpl->activationVectors.empty()) return activationMap;
    
    size_t patternSize = pImpl->activationVectors[0].size();
    activationMap.resize(patternSize, std::vector<float>(pImpl->activationVectors.size(), 0.0f));
    
    for (size_t i = 0; i < patternSize; ++i) {
        for (size_t j = 0; j < pImpl->activationVectors.size(); ++j) {
            if (i < pImpl->activationVectors[j].size()) {
                activationMap[i][j] = pImpl->activationVectors[j][i];
            }
        }
    }
    
    return activationMap;
}

void NeuralAssociativeMemory::clear() {
    pImpl->weightMatrix.clear();
    pImpl->activationVectors.clear();
    pImpl->conceptAssociations.clear();
    pImpl->conceptFeatures.clear();
    
    // Reset statistics
    pImpl->predictionAccuracy = 0.0f;
    pImpl->patternSeparation = 0.0f;
    pImpl->memoryUtilization = 0.0f;
    pImpl->totalPatternLoads = 0;
    pImpl->totalPatternStores = 0;
    pImpl->totalCompletions = 0;
}

void NeuralAssociativeMemory::loadPatterns(const std::vector<std::vector<float>>& patterns) {
    // Load patterns into activation vectors
    for (const auto& pattern : patterns) {
        if (!pattern.empty()) {
            pImpl->activationVectors.push_back(normalizePattern(pattern));
        }
    }
    
    pImpl->totalPatternLoads++;
    
    // Rebuild weight matrix from loaded patterns
    rebuildWeightMatrix();
}

void NeuralAssociativeMemory::savePatterns(std::vector<std::vector<float>>& patterns) const {
    patterns.clear();
    for (const auto& activation : pImpl->activationVectors) {
        patterns.push_back(activation);
    }
}

std::vector<size_t> NeuralAssociativeMemory::getConceptAssociations(
    const std::vector<float>& pattern) const {
    std::vector<size_t> conceptIds;
    
    if (pattern.empty()) return conceptIds;
    
    std::vector<float> normPattern = normalizePattern(pattern);
    
    // Find similar concepts
    for (size_t i = 0; i < pImpl->conceptAssociations.size(); ++i) {
        float similarity = calculatePatternSimilarity(normPattern, pImpl->conceptFeatures[i]);
        if (similarity > completionThreshold_) {
            conceptIds.push_back(i);
        }
    }
    
    return conceptIds;
}

void NeuralAssociativeMemory::createConcept(const std::vector<float>& pattern,
                                            const std::vector<float>& features) {
    // Create new concept from pattern
    size_t conceptId = pImpl->conceptAssociations.size();
    
    pImpl->conceptFeatures.push_back(normalizePattern(features));
    pImpl->conceptAssociations.push_back(std::vector<size_t>());  // Empty for now
    
    // Initialize weight connections for this concept
    pImpl->weightMatrix.push_back(std::vector<float>(features.size(), 0.0f));
    for (auto& row : pImpl->weightMatrix) {
        row.resize(features.size(), 0.0f);
    }
}

void NeuralAssociativeMemory::associateConcepts(size_t conceptA, size_t conceptB, float strength) {
    if (conceptA >= pImpl->conceptAssociations.size() ||
        conceptB >= pImpl->conceptAssociations.size()) return;
    
    pImpl->conceptAssociations[conceptA].push_back(conceptB);
    pImpl->conceptAssociations[conceptB].push_back(conceptA);
    
    // Create associative connections between concepts
    for (size_t i = 0; i < pImpl->conceptFeatures[conceptA].size(); ++i) {
        if (i < pImpl->conceptFeatures[conceptB].size()) {
            float weight = strength * pImpl->conceptFeatures[conceptA][i] * 
                          pImpl->conceptFeatures[conceptB][i];
            if (i < pImpl->weightMatrix.size()) {
                pImpl->weightMatrix[i][i] += weight;  // Self-connection for concept
            }
        }
    }
}

void NeuralAssociativeMemory::updateAssociationStatistics() {
    // Update association network statistics
    if (pImpl->activationVectors.empty()) return;
    
    // Calculate pattern separation (how different patterns are stored)
    float totalSeparation = 0.0f;
    size_t comparisonCount = 0;
    
    for (size_t i = 0; i < pImpl->activationVectors.size(); ++i) {
        for (size_t j = i + 1; j < pImpl->activationVectors.size(); ++j) {
            float similarity = calculatePatternSimilarity(pImpl->activationVectors[i], 
                                                        pImpl->activationVectors[j]);
            totalSeparation += (1.0f - similarity);  // Higher separation = better
            comparisonCount++;
        }
    }
    
    if (comparisonCount > 0) {
        pImpl->patternSeparation = totalSeparation / comparisonCount;
    }
    
    // Calculate memory utilization
    pImpl->memoryUtilization = std::min(1.0f, 
                                       static_cast<float>(pImpl->activationVectors.size()) / 1000.0f);
    
    // Estimate prediction accuracy (simplified)
    pImpl->predictionAccuracy = pImpl->patternSeparation * 0.5f + pImpl->memoryUtilization * 0.5f;
}

void NeuralAssociativeMemory::rebuildWeightMatrix() {
    // Rebuild weight matrix from activation vectors
    if (pImpl->activationVectors.empty()) return;
    
    size_t patternSize = pImpl->activationVectors[0].size();
    pImpl->weightMatrix.clear();
    pImpl->weightMatrix.resize(patternSize, std::vector<float>(patternSize, 0.0f));
    
    // Use Hebbian learning rule
    for (const auto& pattern : pImpl->activationVectors) {
        for (size_t i = 0; i < pattern.size(); ++i) {
            for (size_t j = 0; j < pattern.size(); ++j) {
                pImpl->weightMatrix[i][j] += pattern[i] * pattern[j];
            }
        }
    }
    
    // Normalize weights
    float maxWeight = 0.0f;
    for (const auto& row : pImpl->weightMatrix) {
        for (float weight : row) {
            maxWeight = std::max(maxWeight, std::abs(weight));
        }
    }
    
    if (maxWeight > 0.0f) {
        for (auto& row : pImpl->weightMatrix) {
            for (float& weight : row) {
                weight /= maxWeight;
            }
        }
    }
}

std::vector<float> NeuralAssociativeMemory::normalizePattern(const std::vector<float>& pattern) const {
    if (pattern.empty()) return {};
    
    float norm = std::sqrt(std::inner_product(pattern.begin(), pattern.end(), pattern.begin(), 0.0f));
    if (norm < 0.0001f) return pattern;
    
    std::vector<float> normalized(pattern.size());
    for (size_t i = 0; i < pattern.size(); ++i) {
        normalized[i] = pattern[i] / norm;
    }
    
    return normalized;
}

float NeuralAssociativeMemory::calculateHebbianFactor(const std::vector<float>& patternA,
                                                     const std::vector<float>& patternB,
                                                     size_t i, size_t j) const {
    // Calculate Hebbian learning factor for specific synapses
    if (i >= patternA.size() || j >= patternB.size()) return 1.0f;
    
    // Simplified Hebbian rule
    return (patternA[i] * patternB[j] + patternA[j] * patternB[i]) * 0.5f;
}

float NeuralAssociativeMemory::predictOutcome(const std::vector<float>& pattern) const {
    // Predict outcome based on current state
    if (pattern.empty()) return 0.0f;
    
    // Use weight matrix to predict
    float prediction = 0.0f;
    for (size_t i = 0; i < pattern.size(); ++i) {
        for (size_t j = 0; j < pattern.size(); ++j) {
            prediction += pattern[i] * pImpl->weightMatrix[i][j] * pattern[j];
        }
    }
    
    // Apply sigmoid function
    return sigmoid(prediction);
}

float NeuralAssociativeMemory::calculateVectorDifference(const std::vector<float>& a,
                                                         const std::vector<float>& b) const {
    if (a.size() != b.size()) return 1.0f;
    
    float sum = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        sum += std::abs(a[i] - b[i]);
    }
    
    return sum / a.size();
}

float NeuralAssociativeMemory::sigmoid(float x) const {
    return 1.0f / (1.0f + std::exp(-x));
}

} // namespace nlm