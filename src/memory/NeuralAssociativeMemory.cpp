#include "NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    Brain* brain;
    
    // Pattern to pattern association matrix
    // Organized by pattern index for efficient access
    std::vector<std::vector<float>> associationMatrix;
    
    // Reverse lookup: pattern -> list of associated patterns
    std::unordered_map<size_t, std::vector<size_t>> patternAssociations;
    
    // Pattern storage
    std::vector<std::vector<float>> patterns;
    
    // Association strength decay over time
    std::vector<float> associationStrengths;
    
    // Learning parameters
    float hebbianLearningRate;
    float stdpLearningRate;
    float decayRate;
    float capacity;
    
    Impl() 
        : brain(nullptr)
        , hebbianLearningRate(0.01f)
        , stdpLearningRate(0.005f)
        , decayRate(0.001f)
        , capacity(1000) {}
};

NeuralAssociativeMemory::NeuralAssociativeMemory()
    : pImpl(new Impl)
{
}

NeuralAssociativeMemory::~NeuralAssociativeMemory() = default;

void NeuralAssociativeMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("NeuralAssociativeMemory initialized");
}

void NeuralAssociativeMemory::store(const std::vector<float>& patternA, const std::vector<float>& patternB, float strength) {
    if (patternA.empty() || patternB.empty() || !pImpl->brain) return;
    
    // Check if patterns already exist
    size_t idxA = findPattern(patternA);
    size_t idxB = findPattern(patternB);
    
    if (idxA == size_t(-1)) {
        idxA = patterns.size();
        patterns.push_back(patternA);
        associationMatrix.push_back(std::vector<float>(patterns.size(), 0.0f));
        patternAssociations[idxA] = std::vector<size_t>();
    }
    
    if (idxB == size_t(-1)) {
        idxB = patterns.size();
        patterns.push_back(patternB);
        associationMatrix.push_back(std::vector<float>(patterns.size(), 0.0f));
        for (auto& associations : associationMatrix) {
            associations.push_back(0.0f);
        }
        patternAssociations[idxB] = std::vector<size_t>();
    }
    
    // Update association matrix
    associationMatrix[idxA][idxB] = std::max(associationMatrix[idxA][idxB], strength);
    associationMatrix[idxB][idxA] = std::max(associationMatrix[idxB][idxA], strength);
    
    // Update reverse lookup
    if (strength > 0.1f) {
        patternAssociations[idxA].push_back(idxB);
        patternAssociations[idxB].push_back(idxA);
    }
    
    // Prune if over capacity
    if (associationMatrix.size() > pImpl->capacity) {
        prune(pImpl->capacity * 0.8f);
    }
}

std::vector<float> NeuralAssociativeMemory::retrieve(const std::vector<float>& pattern) const {
    size_t idx = findPattern(pattern);
    if (idx == size_t(-1)) {
        return std::vector<float>();
    }
    
    std::vector<float> result;
    if (patternAssociations.count(idx)) {
        for (size_t associatedIdx : patternAssociations.at(idx)) {
            if (associatedIdx != idx && !patterns[associatedIdx].empty()) {
                result.push_back(patterns[associatedIdx][0]);  // Simplified: return first element of pattern
            }
        }
    }
    
    return result;
}

std::vector<std::pair<std::vector<float>, float>> NeuralAssociativeMemory::getAssociations(const std::vector<float>& pattern) const {
    std::vector<std::pair<std::vector<float>, float>> result;
    size_t idx = findPattern(pattern);
    
    if (!patternAssociations.count(idx)) {
        return result;
    }
    
    for (size_t associatedIdx : patternAssociations.at(idx)) {
        if (associatedIdx != idx) {
            result.emplace_back(patterns[associatedIdx], associationMatrix[idx][associatedIdx]);
        }
    }
    
    // Sort by strength (strongest first)
    std::sort(result.begin(), result.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    return result;
}

void NeuralAssociativeMemory::updateAssociation(const std::vector<float>& patternA, const std::vector<float>& patternB, float delta) {
    size_t idxA = findPattern(patternA);
    size_t idxB = findPattern(patternB);
    
    if (idxA == size_t(-1) || idxB == size_t(-1)) {
        return;
    }
    
    // Update association strength
    associationMatrix[idxA][idxB] = std::max(0.0f, associationMatrix[idxA][idxB] + delta);
    associationMatrix[idxB][idxA] = associationMatrix[idxA][idxB];
    
    // Update reverse lookup
    if (associationMatrix[idxA][idxB] <= 0.1f) {
        // Remove from reverse lookup if weak
        auto itA = std::find(patternAssociations[idxA].begin(), patternAssociations[idxA].end(), idxB);
        if (itA != patternAssociations[idxA].end()) {
            patternAssociations[idxA].erase(itA);
        }
        
        auto itB = std::find(patternAssociations[idxB].begin(), patternAssociations[idxB].end(), idxA);
        if (itB != patternAssociations[idxB].end()) {
            patternAssociations[idxB].erase(itB);
        }
    } else {
        // Add to reverse lookup if strong enough
        if (!std::any_of(patternAssociations[idxA].begin(), patternAssociations[idxA].end(), 
                       [idxB](size_t idx) { return idx == idxB; })) {
            patternAssociations[idxA].push_back(idxB);
        }
        
        if (!std::any_of(patternAssociations[idxB].begin(), patternAssociations[idxB].end(), 
                       [idxA](size_t idx) { return idx == idxA; })) {
            patternAssociations[idxB].push_back(idxA);
        }
    }
}

float NeuralAssociativeMemory::getAssociationStrength(const std::vector<float>& patternA, const std::vector<float>& patternB) const {
    size_t idxA = findPattern(patternA);
    size_t idxB = findPattern(patternB);
    
    if (idxA == size_t(-1) || idxB == size_t(-1)) {
        return 0.0f;
    }
    
    return associationMatrix[idxA][idxB];
}

void NeuralAssociativeMemory::clear() {
    associationMatrix.clear();
    patternAssociations.clear();
    patterns.clear();
    associationStrengths.clear();
    NLM_LOG_INFO("NeuralAssociativeMemory cleared");
}

std::vector<std::vector<float>> NeuralAssociativeMemory::getAllPatterns() const {
    return patterns;
}

void NeuralAssociativeMemory::prune(float threshold) {
    // Remove weak associations
    for (size_t i = 0; i < associationMatrix.size(); ++i) {
        for (size_t j = 0; j < associationMatrix[i].size(); ++j) {
            if (associationMatrix[i][j] < threshold) {
                associationMatrix[i][j] = 0.0f;
            }
        }
    }
    
    // Rebuild reverse lookup
    patternAssociations.clear();
    for (size_t i = 0; i < associationMatrix.size(); ++i) {
        for (size_t j = 0; j < associationMatrix[i].size(); ++j) {
            if (associationMatrix[i][j] >= threshold) {
                patternAssociations[i].push_back(j);
            }
        }
    }
    
    // Remove empty patterns
    std::vector<size_t> emptyIndices;
    for (size_t i = 0; i < associationMatrix.size(); ++i) {
        if (patternAssociations.count(i) == 0) {
            emptyIndices.push_back(i);
        }
    }
    
    // Remove empty patterns and rebuild matrix
    if (!emptyIndices.empty()) {
        std::vector<std::vector<float>> newMatrix;
        std::vector<std::vector<float>> newPatterns;
        std::vector<size_t> indexMap(associationMatrix.size(), size_t(-1));
        
        size_t newIdx = 0;
        for (size_t i = 0; i < associationMatrix.size(); ++i) {
            if (std::find(emptyIndices.begin(), emptyIndices.end(), i) == emptyIndices.end()) {
                indexMap[i] = newIdx++;
                newMatrix.push_back(associationMatrix[i]);
                newPatterns.push_back(patterns[i]);
            }
        }
        
        // Rebuild reverse lookup with new indices
        patternAssociations.clear();
        for (size_t i = 0; i < newMatrix.size(); ++i) {
            for (size_t j = 0; j < newMatrix[i].size(); ++j) {
                if (newMatrix[i][j] >= threshold) {
                    patternAssociations[i].push_back(j);
                }
            }
        }
        
        associationMatrix = newMatrix;
        patterns = newPatterns;
    }
}

size_t NeuralAssociativeMemory::getCapacity() const {
    return pImpl->capacity;
}

void NeuralAssociativeMemory::setCapacity(size_t capacity) {
    pImpl->capacity = capacity;
}

size_t NeuralAssociativeMemory::getAssociationCount() const {
    size_t count = 0;
    for (const auto& row : associationMatrix) {
        for (float strength : row) {
            if (strength >= 0.1f) {
                count++;
            }
        }
    }
    return count / 2;  // Each association counted twice
}

void NeuralAssociativeMemory::hebbianLearning(const std::vector<float>& patternA, const std::vector<float>& patternB) {
    if (patternA.empty() || patternB.empty() || !pImpl->brain) return;
    
    // Simple Hebbian learning: max(0, current + weight * patternA * patternB)
    size_t idxA = findPattern(patternA);
    size_t idxB = findPattern(patternB);
    
    if (idxA == size_t(-1) || idxB == size_t(-1)) {
        return;
    }
    
    // Simplified Hebbian rule
    for (size_t i = 0; i < std::min(patternA.size(), patternB.size()); ++i) {
        float product = patternA[i] * patternB[i];
        if (product > 0.0f) {
            associationMatrix[idxA][idxB] += pImpl->hebbianLearningRate * product;
            associationMatrix[idxB][idxA] = associationMatrix[idxA][idxB];
        }
    }
}

void NeuralAssociativeMemory::temporalLearning(const std::vector<float>& patternA, const std::vector<float>& patternB, 
                                                SimulationStep timeDifference) {
    if (patternA.empty() || patternB.empty() || !pImpl->brain) return;
    
    size_t idxA = findPattern(patternA);
    size_t idxB = findPattern(patternB);
    
    if (idxA == size_t(-1) || idxB == size_t(-1)) {
        return;
    }
    
    // Temporal learning: associations strengthen based on timing
    float temporalFactor = 1.0f / std::max(1.0f, static_cast<float>(timeDifference));
    float learningAmount = pImpl->stdpLearningRate * temporalFactor;
    
    associationMatrix[idxA][idxB] += learningAmount;
    associationMatrix[idxB][idxA] = associationMatrix[idxA][idxB];
}

// Helper function to find pattern in patterns vector
size_t NeuralAssociativeMemory::findPattern(const std::vector<float>& pattern) const {
    for (size_t i = 0; i < patterns.size(); ++i) {
        if (patterns[i] == pattern) {
            return i;
        }
    }
    return size_t(-1);
}

} // namespace nlm
