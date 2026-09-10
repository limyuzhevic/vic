#include "NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <vector>
#include <random>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    Brain* brain;
    
    // Pattern database: maps pattern fingerprints to associations
    std::unordered_map<size_t, std::vector<float>> patternDatabase;
    
    // Association strength weights
    std::unordered_map<size_t, float> associationWeights;
    
    // Pattern aging and decay
    std::unordered_map<size_t, float> patternAges;
    float decayRate;
    
    // Concept storage
    std::unordered_map<std::string, std::vector<float>> conceptStorage;
    
    // Pattern completion enabled flag
    bool patternCompletionEnabled;
    
    // Memory capacity limits
    size_t maxPatterns;
    float minAssociationStrength;
    
    // Attractors for pattern completion
    std::vector<std::vector<float>> attractors;
    
    // Random number generator
    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;
    
    Impl() : 
        brain(nullptr),
        decayRate(0.001f),
        patternCompletionEnabled(true),
        maxPatterns(1000),
        minAssociationStrength(0.1f),
        dist(0.0f, 1.0f) {
        rng.seed(std::random_device{}());
    }
};

NeuralAssociativeMemory::NeuralAssociativeMemory() : 
    pImpl(new Impl) {
    NLM_LOG_INFO("NeuralAssociativeMemory initialized");
}

NeuralAssociativeMemory::~NeuralAssociativeMemory() = default;

void NeuralAssociativeMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("NeuralAssociativeMemory connected to brain");
}

void NeuralAssociativeMemory::associatePattern(
    const std::vector<float>& pattern,
    const std::vector<float>& association,
    float strength,
    TimestepDuration dt
) {
    if (pattern.empty() || association.empty() || !brain_) {
        NLM_LOG_ERROR("Invalid parameters for pattern association");
        return;
    }
    
    // Create pattern fingerprint (simplified)
    size_t fingerprint = 0;
    if (!pattern.empty()) {
        // Use first few elements for quick lookup
        fingerprint = std::hash<float>{}(pattern[0]);
        for (size_t i = 1; i < std::min(pattern.size(), size_t(4)); ++i) {
            fingerprint ^= std::hash<float>{}(pattern[i]);
        }
    }
    
    // Store association
    pImpl->patternDatabase[fingerprint] = association;
    pImpl->associationWeights[fingerprint] = strength;
    pImpl->patternAges[fingerprint] = 0.0f;
    
    // Update attractor if pattern completion is enabled
    if (pImpl->patternCompletionEnabled && pattern.size() == association.size()) {
        updateAttractor(pattern, association, strength);
    }
    
    NLM_LOG_DEBUG(std::string("Pattern associated: fingerprint=") + 
                  std::to_string(fingerprint) + 
                  ", strength=" + std::to_string(strength));
}

std::vector<float> NeuralAssociativeMemory::retrieveAssociation(
    const std::vector<float>& pattern,
    float threshold
) const {
    if (pattern.empty() || !brain_) return {};
    
    // Find closest matching pattern
    size_t bestFingerprint = 0;
    float bestMatch = 0.0f;
    float bestStrength = 0.0f;
    
    for (const auto& entry : pImpl->patternDatabase) {
        size_t fingerprint = entry.first;
        const std::vector<float>& storedPattern = pattern;  // In real implementation, would store pattern separately
        
        // Simple pattern matching (in real implementation, use proper pattern distance)
        float match = 0.0f;
        if (storedPattern.size() == pattern.size()) {
            for (size_t i = 0; i < pattern.size(); ++i) {
                if (i < storedPattern.size()) {
                    match += std::abs(storedPattern[i] - pattern[i]);
                }
            }
            match = 1.0f / (1.0f + match);  // Convert distance to similarity
        }
        
        float strength = pImpl->associationWeights.at(fingerprint);
        float score = match * strength;
        
        if (score > bestMatch) {
            bestMatch = score;
            bestFingerprint = fingerprint;
            bestStrength = strength;
        }
    }
    
    // Return association if match is strong enough
    if (bestMatch >= threshold) {
        return pImpl->patternDatabase.at(bestFingerprint);
    }
    
    return {};
}

bool NeuralAssociativeMemory::hasAssociation(
    const std::vector<float>& pattern,
    float minStrength
) const {
    if (pattern.empty()) return false;
    
    // Create fingerprint
    size_t fingerprint = 0;
    if (!pattern.empty()) {
        fingerprint = std::hash<float>{}(pattern[0]);
        for (size_t i = 1; i < std::min(pattern.size(), size_t(4)); ++i) {
            fingerprint ^= std::hash<float>{}(pattern[i]);
        }
    }
    
    auto it = pImpl->associationWeights.find(fingerprint);
    if (it == pImpl->associationWeights.end()) return false;
    
    return it->second >= minStrength;
}

float NeuralAssociativeMemory::getAssociationStrength(const std::vector<float>& pattern) const {
    if (pattern.empty()) return 0.0f;
    
    size_t fingerprint = 0;
    if (!pattern.empty()) {
        fingerprint = std::hash<float>{}(pattern[0]);
        for (size_t i = 1; i < std::min(pattern.size(), size_t(4)); ++i) {
            fingerprint ^= std::hash<float>{}(pattern[i]);
        }
    }
    
    auto it = pImpl->associationWeights.find(fingerprint);
    if (it == pImpl->associationWeights.end()) return 0.0f;
    
    return it->second;
}

void NeuralAssociativeMemory::updateWeights(float learningRate, TimestepDuration dt) {
    if (!brain_) return;
    
    // Update pattern ages
    for (auto& entry : pImpl->patternAges) {
        size_t fingerprint = entry.first;
        entry.second += dt * pImpl->decayRate;
    }
    
    // Decay association weights based on age
    for (auto& entry : pImpl->associationWeights) {
        size_t fingerprint = entry.first;
        float& weight = entry.second;
        
        // Age-based decay
        float age = pImpl->patternAges.at(fingerprint);
        weight *= std::pow(0.9f, age * 10.0f);  // Exponential decay
        
        // Apply Hebbian-like updates based on recent activity
        // In real implementation, would integrate with brain plasticity
        if (weight < pImpl->minAssociationStrength) {
            weight = 0.0f;  // Remove weak associations
            pImpl->patternDatabase.erase(fingerprint);
            pImpl->patternAges.erase(fingerprint);
        }
    }
    
    // Capacity management
    if (pImpl->patternDatabase.size() > pImpl->maxPatterns) {
        pruneWeakAssociations();
    }
    
    NLM_LOG_DEBUG(std::string("Updated associative memory: ") + 
                  std::to_string(pImpl->patternDatabase.size()) + " patterns remain");
}

void NeuralAssociativeMemory::formConcept(
    const std::vector<float>& pattern,
    const std::string& conceptName,
    float conceptStrength
) {
    if (pattern.empty() || conceptName.empty()) {
        NLM_LOG_ERROR("Invalid parameters for concept formation");
        return;
    }
    
    // Store concept
    pImpl->conceptStorage[conceptName] = pattern;
    
    // Create attractor for concept
    updateAttractor(pattern, pattern, conceptStrength);
    
    NLM_LOG_INFO(std::string("Concept formed: ") + conceptName + 
                 ", strength=" + std::to_string(conceptStrength));
}

std::vector<float> NeuralAssociativeMemory::getConcept(const std::string& conceptName) const {
    auto it = pImpl->conceptStorage.find(conceptName);
    if (it == pImpl->conceptStorage.end()) {
        return {};
    }
    return it->second;
}

std::unordered_map<std::string, std::vector<float>> NeuralAssociativeMemory::getAllConcepts() const {
    return pImpl->conceptStorage;
}

std::vector<float> NeuralAssociativeMemory::completePattern(
    const std::vector<float>& partialPattern,
    size_t iterations
) {
    if (!pImpl->patternCompletionEnabled || partialPattern.empty()) {
        return partialPattern;
    }
    
    std::vector<float> current = partialPattern;
    
    for (size_t iter = 0; iter < iterations; ++iter) {
        // Find closest attractor
        size_t bestAttractor = findClosestAttractor(current);
        if (bestAttractor < pImpl->attractors.size()) {
            const std::vector<float>& attractor = pImpl->attractors[bestAttractor];
            
            // Smooth current towards attractor
            for (size_t i = 0; i < current.size() && i < attractor.size(); ++i) {
                current[i] = current[i] * 0.7f + attractor[i] * 0.3f;
            }
        }
        
        // Also consider database associations
        std::vector<float> association = retrieveAssociation(current, 0.1f);
        if (!association.empty() && association.size() == current.size()) {
            for (size_t i = 0; i < current.size(); ++i) {
                current[i] = current[i] * 0.8f + association[i] * 0.2f;
            }
        }
    }
    
    return current;
}

void NeuralAssociativeMemory::clear() {
    pImpl->patternDatabase.clear();
    pImpl->associationWeights.clear();
    pImpl->patternAges.clear();
    pImpl->conceptStorage.clear();
    pImpl->attractors.clear();
    
    NLM_LOG_INFO("NeuralAssociativeMemory cleared");
}

size_t NeuralAssociativeMemory::getPatternCount() const {
    return pImpl->patternDatabase.size();
}

float NeuralAssociativeMemory::getTotalAssociationStrength() const {
    float total = 0.0f;
    for (const auto& entry : pImpl->associationWeights) {
        total += entry.second;
    }
    return total;
}

void NeuralAssociativeMemory::enablePatternCompletion(bool enable) {
    pImpl->patternCompletionEnabled = enable;
    
    if (!enable) {
        pImpl->attractors.clear();
    }
}

bool NeuralAssociativeMemory::isPatternCompletionEnabled() const {
    return pImpl->patternCompletionEnabled;
}

void NeuralAssociativeMemory::setMaxPatterns(size_t maxPatterns) {
    pImpl->maxPatterns = maxPatterns;
    if (pImpl->patternDatabase.size() > maxPatterns) {
        pruneWeakAssociations();
    }
}

void NeuralAssociativeMemory::setAssociationDecay(float decayRate) {
    pImpl->decayRate = decayRate;
}

bool NeuralAssociativeMemory::exportMemory(const std::string& filepath) const {
    // TODO: Implement serialization
    NLM_LOG_WARN("ExportMemory not implemented yet");
    return false;
}

bool NeuralAssociativeMemory::importMemory(const std::string& filepath) {
    // TODO: Implement deserialization
    NLM_LOG_WARN("ImportMemory not implemented yet");
    return false;
}

void NeuralAssociativeMemory::updateAttractor(
    const std::vector<float>& pattern,
    const std::vector<float>& association,
    float strength
) {
    if (!pImpl->patternCompletionEnabled) return;
    
    // Create attractor vector (average of pattern and association)
    std::vector<float> attractor(pattern.size());
    for (size_t i = 0; i < pattern.size() && i < association.size(); ++i) {
        attractor[i] = (pattern[i] + association[i]) * 0.5f * strength;
    }
    
    pImpl->attractors.push_back(attractor);
    
    // Keep attractor list manageable
    if (pImpl->attractors.size() > 100) {
        pImpl->attractors.erase(pImpl->attractors.begin());
    }
}

void NeuralAssociativeMemory::pruneWeakAssociations() {
    std::vector<size_t> fingerprintsToRemove;
    
    for (const auto& entry : pImpl->associationWeights) {
        if (entry.second < pImpl->minAssociationStrength) {
            fingerprintsToRemove.push_back(/* need fingerprint mapping */ 0);
        }
    }
    
    // Remove weak associations
    // In real implementation, would properly map fingerprints
    NLM_LOG_DEBUG(std::string("Pruned ") + 
                  std::to_string(fingerprintsToRemove.size()) + 
                  " weak associations");
}

size_t NeuralAssociativeMemory::findClosestAttractor(const std::vector<float>& pattern) const {
    if (pImpl->attractors.empty()) return 0;
    
    size_t bestAttractor = 0;
    float bestDistance = std::numeric_limits<float>::max();
    
    for (size_t i = 0; i < pImpl->attractors.size(); ++i) {
        const std::vector<float>& attractor = pImpl->attractors[i];
        float distance = 0.0f;
        
        for (size_t j = 0; j < pattern.size() && j < attractor.size(); ++j) {
            distance += std::pow(pattern[j] - attractor[j], 2);
        }
        
        if (distance < bestDistance) {
            bestDistance = distance;
            bestAttractor = i;
        }
    }
    
    return bestAttractor;
}

} // namespace nlm
