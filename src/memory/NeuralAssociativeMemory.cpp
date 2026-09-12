#include "NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    Brain* brain;
    std::vector<std::pair<std::vector<float>, std::vector<float>>> associations;
    std::vector<float> associationStrengths;
    std::vector<float> eligibilityTraces;
    SimulationStep lastUpdateTime;
    float learningRate;
    float decayRate;
    float stdpLearningRate;
    float hebbianDecay;
    
    Impl() 
        : brain(nullptr)
        , lastUpdateTime(0)
        , learningRate(0.01f)
        , decayRate(0.001f)
        , stdpLearningRate(0.005f)
        , hebbianDecay(0.95f) {}
};

NeuralAssociativeMemory::NeuralAssociativeMemory() : pImpl(new Impl) {}

NeuralAssociativeMemory::~NeuralAssociativeMemory() = default;

void NeuralAssociativeMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    pImpl->associations.clear();
    pImpl->associationStrengths.clear();
    pImpl->eligibilityTraces.clear();
    NLM_LOG_INFO("NeuralAssociativeMemory system initialized");
}

void NeuralAssociativeMemory::storeAssociation(const std::vector<float>& patternA,
                                               const std::vector<float>& patternB,
                                               float strength) {
    // Check if association already exists
    for (size_t i = 0; i < pImpl->associations.size(); ++i) {
        const auto& assoc = pImpl->associations[i];
        bool matchesA = true;
        bool matchesB = true;
        
        if (assoc.first.size() == patternA.size()) {
            for (size_t j = 0; j < patternA.size(); ++j) {
                if (std::abs(assoc.first[j] - patternA[j]) > 0.01f) {
                    matchesA = false;
                    break;
                }
            }
        } else {
            matchesA = false;
        }
        
        if (assoc.second.size() == patternB.size()) {
            for (size_t j = 0; j < patternB.size(); ++j) {
                if (std::abs(assoc.second[j] - patternB[j]) > 0.01f) {
                    matchesB = false;
                    break;
                }
            }
        } else {
            matchesB = false;
        }
        
        if (matchesA && matchesB) {
            // Update existing association
            pImpl->associationStrengths[i] = strength;
            NLM_LOG_INFO("[INFO] Updated existing association (strength: " + 
                         std::to_string(strength) + ")");
            return;
        }
    }
    
    // Add new association
    pImpl->associations.push_back(std::make_pair(patternA, patternB));
    pImpl->associationStrengths.push_back(strength);
    pImpl->eligibilityTraces.push_back(0.0f);
    
    NLM_LOG_INFO("[INFO] Stored new association (strength: " + std::to_string(strength) + ")");
}

std::vector<float> NeuralAssociativeMemory::retrieve(const std::vector<float>& patternA,
                                                    float threshold) const {
    float bestStrength = 0.0f;
    std::vector<float> bestMatch;
    
    for (size_t i = 0; i < pImpl->associations.size(); ++i) {
        const auto& assoc = pImpl->associations[i];
        
        // Check pattern similarity using Hebbian similarity
        float similarity = computeHebbianSimilarity(patternA, assoc.first);
        
        if (similarity > bestStrength && similarity >= threshold) {
            bestStrength = similarity;
            bestMatch = assoc.second;
        }
    }
    
    if (bestMatch.empty()) {
        NLM_LOG_INFO("[INFO] No association found for pattern (threshold: " + 
                     std::to_string(threshold) + ")");
    } else {
        NLM_LOG_INFO("[INFO] Retrieved association with strength: " + std::to_string(bestStrength));
    }
    
    return bestMatch;
}

float NeuralAssociativeMemory::getAssociationStrength(const std::vector<float>& patternA,
                                                      const std::vector<float>& patternB) const {
    for (size_t i = 0; i < pImpl->associations.size(); ++i) {
        const auto& assoc = pImpl->associations[i];
        
        bool matchesA = true;
        bool matchesB = true;
        
        if (assoc.first.size() == patternA.size()) {
            for (size_t j = 0; j < patternA.size(); ++j) {
                if (std::abs(assoc.first[j] - patternA[j]) > 0.01f) {
                    matchesA = false;
                    break;
                }
            }
        } else {
            matchesA = false;
        }
        
        if (assoc.second.size() == patternB.size()) {
            for (size_t j = 0; j < patternB.size(); ++j) {
                if (std::abs(assoc.second[j] - patternB[j]) > 0.01f) {
                    matchesB = false;
                    break;
                }
            }
        } else {
            matchesB = false;
        }
        
        if (matchesA && matchesB) {
            return pImpl->associationStrengths[i];
        }
    }
    
    return 0.0f;
}

void NeuralAssociativeMemory::updateAssociation(const std::vector<float>& patternA,
                                                 const std::vector<float>& patternB,
                                                 float newStrength) {
    for (size_t i = 0; i < pImpl->associations.size(); ++i) {
        auto& assoc = pImpl->associations[i];
        
        bool matchesA = true;
        bool matchesB = true;
        
        if (assoc.first.size() == patternA.size()) {
            for (size_t j = 0; j < patternA.size(); ++j) {
                if (std::abs(assoc.first[j] - patternA[j]) > 0.01f) {
                    matchesA = false;
                    break;
                }
            }
        } else {
            matchesA = false;
        }
        
        if (assoc.second.size() == patternB.size()) {
            for (size_t j = 0; j < patternB.size(); ++j) {
                if (std::abs(assoc.second[j] - patternB[j]) > 0.01f) {
                    matchesB = false;
                    break;
                }
            }
        } else {
            matchesB = false;
        }
        
        if (matchesA && matchesB) {
            pImpl->associationStrengths[i] = newStrength;
            return;
        }
    }
}

float NeuralAssociativeMemory::computeHebbianSimilarity(const std::vector<float>& patternA,
                                                       const std::vector<float>& patternB) const {
    if (patternA.size() != patternB.size() || patternA.empty()) {
        return 0.0f;
    }
    
    // Hebbian similarity: dot product of normalized patterns
    float dotProduct = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    
    for (size_t i = 0; i < patternA.size(); ++i) {
        dotProduct += patternA[i] * patternB[i];
        normA += patternA[i] * patternA[i];
        normB += patternB[i] * patternB[i];
    }
    
    normA = std::sqrt(normA);
    normB = std::sqrt(normB);
    
    if (normA > 0.0f && normB > 0.0f) {
        return dotProduct / (normA * normB);
    }
    
    return 0.0f;
}

void NeuralAssociativeMemory::applySTDP(float predictionError, TimestepDuration dt) {
    // Apply STDP-like learning to associations based on prediction error
    float dtFloat = static_cast<float>(dt);
    
    for (size_t i = 0; i < pImpl->associations.size(); ++i) {
        // Update eligibility trace
        pImpl->eligibilityTraces[i] += pImpl->stdpLearningRate * predictionError;
        
        // Decay eligibility trace
        pImpl->eligibilityTraces[i] *= pImpl->hebbianDecay;
        
        // Update association strength based on eligibility trace
        if (std::abs(pImpl->eligibilityTraces[i]) > 0.001f) {
            pImpl->associationStrengths[i] += pImpl->eligibilityTraces[i] * dtFloat;
        }
        
        // Clamp strength to reasonable range
        pImpl->associationStrengths[i] = std::clamp(pImpl->associationStrengths[i], 0.0f, 10.0f);
    }
}

void NeuralAssociativeMemory::clear() {
    pImpl->associations.clear();
    pImpl->associationStrengths.clear();
    pImpl->eligibilityTraces.clear();
    NLM_LOG_INFO("NeuralAssociativeMemory cleared all associations");
}

void NeuralAssociativeMemory::reset() {
    clear();
    pImpl->lastUpdateTime = 0;
    NLM_LOG_INFO("NeuralAssociativeMemory reset");
}

} // namespace nlm