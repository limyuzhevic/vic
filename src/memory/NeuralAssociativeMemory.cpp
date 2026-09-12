#include "NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <map>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    Brain* brain;
    
    // Random generator for stochastic association formation
    std::mt19937 rng;
    
    Impl() : brain(nullptr), rng(std::random_device{}()) {}
};

NeuralAssociativeMemory::NeuralAssociativeMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , decayRate_(0.01f)
{
    std::random_device rd;
    pImpl->rng.seed(rd());
}

NeuralAssociativeMemory::~NeuralAssociativeMemory() = default;

void NeuralAssociativeMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    clear();
    
    NLM_LOG_INFO("NeuralAssociativeMemory initialized");
}

void NeuralAssociativeMemory::associate(const std::vector<float>& patternA,
                   const std::vector<float>& patternB,
                   float initialStrength) {
    if (!brain_ || patternA.empty() || patternB.empty()) return;
    
    // Check if patterns already exist
    size_t idxA = -1, idxB = -1;
    
    for (size_t i = 0; i < patterns_.size(); ++i) {
        if (patterns_[i] == patternA) {
            idxA = i;
        }
        if (patterns_[i] == patternB) {
            idxB = i;
        }
        
        if (idxA != -1 && idxB != -1) break;
    }
    
    // Add patterns if they don't exist
    if (idxA == -1) {
        idxA = patterns_.size();
        patterns_.push_back(patternA);
    }
    
    if (idxB == -1) {
        idxB = patterns_.size();
        patterns_.push_back(patternB);
    }
    
    // Check if association already exists
    for (auto& assoc : associations_) {
        if ((assoc.patternA == patternA && assoc.patternB == patternB) ||
            (assoc.patternA == patternB && assoc.patternB == patternA)) {
            // Update existing association strength
            assoc.strength = std::max(assoc.strength, initialStrength);
            assoc.accessCount++;
            assoc.lastAccessed = currentStep_;
            NLM_LOG_INFO("Updated association strength to " + std::to_string(assoc.strength));
            return;
        }
    }
    
    // Create new association
    Association newAssoc(patternA, patternB, initialStrength);
    associations_.push_back(newAssoc);
    
    NLM_LOG_INFO("Created new association between patterns with strength " + std::to_string(initialStrength));
}

std::vector<float> NeuralAssociativeMemory::getAssociation(const std::vector<float>& pattern) const {
    std::vector<float> result;
    
    for (const auto& assoc : associations_) {
        if (assoc.patternA == pattern) {
            result = assoc.patternB;
            break;
        } else if (assoc.patternB == pattern) {
            result = assoc.patternA;
            break;
        }
    }
    
    return result;
}

std::vector<std::pair<std::vector<float>, float>> NeuralAssociativeMemory::getAllAssociations(
    const std::vector<float>& pattern) const {
    std::vector<std::pair<std::vector<float>, float>> result;
    
    for (const auto& assoc : associations_) {
        if (assoc.patternA == pattern) {
            result.push_back(std::make_pair(assoc.patternB, assoc.strength));
        } else if (assoc.patternB == pattern) {
            result.push_back(std::make_pair(assoc.patternA, assoc.strength));
        }
    }
    
    return result;
}

void NeuralAssociativeMemory::updateAssociation(const std::vector<float>& patternA,
                          const std::vector<float>& patternB,
                          float delta) {
    for (auto& assoc : associations_) {
        if ((assoc.patternA == patternA && assoc.patternB == patternB) ||
            (assoc.patternA == patternB && assoc.patternB == patternA)) {
            assoc.strength += delta;
            assoc.strength = std::max(0.0f, std::min(assoc.strength, 1.0f));
            assoc.accessCount++;
            assoc.lastAccessed = currentStep_;
            
            NLM_LOG_INFO("Updated association strength to " + std::to_string(assoc.strength));
            return;
        }
    }
}

float NeuralAssociativeMemory::getAssociationStrength(const std::vector<float>& patternA,
                                const std::vector<float>& patternB) const {
    for (const auto& assoc : associations_) {
        if ((assoc.patternA == patternA && assoc.patternB == patternB) ||
            (assoc.patternA == patternB && assoc.patternB == patternA)) {
            return assoc.strength;
        }
    }
    
    return 0.0f;
}

bool NeuralAssociativeMemory::hasStrongAssociation(const std::vector<float>& patternA,
                             const std::vector<float>& patternB,
                             float threshold) const {
    return getAssociationStrength(patternA, patternB) >= threshold;
}

std::vector<float> NeuralAssociativeMemory::patternCompletion(const std::vector<float>& partialPattern,
                                         size_t maxResults) const {
    std::vector<std::pair<float, std::vector<float>>> scoredPatterns;
    
    for (const auto& pattern : patterns_) {
        float similarity = 0.0f;
        
        if (!partialPattern.empty() && !pattern.empty() && partialPattern.size() == pattern.size()) {
            for (size_t i = 0; i < partialPattern.size(); ++i) {
                similarity += std::abs(partialPattern[i] - pattern[i]);
            }
            similarity = 1.0f - (similarity / partialPattern.size());
        }
        
        scoredPatterns.push_back(std::make_pair(similarity, pattern));
    }
    
    // Sort by similarity (descending)
    std::sort(scoredPatterns.begin(), scoredPatterns.end(),
              [](const std::pair<float, std::vector<float>>& a,
                 const std::pair<float, std::vector<float>>& b) {
                  return a.first > b.first;
              });
    
    std::vector<float> result;
    size_t count = std::min(maxResults, scoredPatterns.size());
    
    for (size_t i = 0; i < count; ++i) {
        result.push_back(scoredPatterns[i].second);
    }
    
    return result;
}

void NeuralAssociativeMemory::clear() {
    associations_.clear();
    patterns_.clear();
}

std::vector<std::vector<float>> NeuralAssociativeMemory::getAllPatterns() const {
    return patterns_;
}

} // namespace nlm