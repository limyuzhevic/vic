#include "NeuralSemanticMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_set>

namespace nlm {

NeuralSemanticMemory::NeuralSemanticMemory()
    : brain_(nullptr)
{
}

NeuralSemanticMemory::~NeuralSemanticMemory() = default;

void NeuralSemanticMemory::initialize(Brain* brain) {
    brain_ = brain;
    NLM_LOG_INFO("Semantic memory initialized");
}

void NeuralSemanticMemory::storeConcept(const std::string& conceptName, const std::vector<float>& pattern) {
    if (pattern.empty() || !brain_) return;
    
    // Check if concept already exists
    auto it = nameToIndex_.find(conceptName);
    if (it != nameToIndex_.end()) {
        // Update existing concept
        size_t index = it->second;
        concepts_[index].pattern = pattern;
        concepts_[index].strength = std::min(1.0f, concepts_[index].strength + 0.1f);
        concepts_[index].lastUpdated = 0; // Current step
        
        // Increase coherence by smoothing with neighbors
        for (size_t i = 0; i < concepts_.size(); ++i) {
            if (i != index) {
                concepts_[index].coherence += calculateSimilarity(pattern, concepts_[i].pattern) * 0.05f;
            }
        }
    } else {
        // Add new concept
        Concept newConcept;
        newConcept.name = conceptName;
        newConcept.pattern = pattern;
        newConcept.strength = 1.0f;
        newConcept.lastUpdated = 0;
        newConcept.coherence = 0.0f;
        
        concepts_.push_back(newConcept);
        nameToIndex_[conceptName] = concepts_.size() - 1;
        
        NLM_LOG_INFO("Stored new semantic concept: " + conceptName);
    }
}

std::vector<float> NeuralSemanticMemory::retrieveConcept(const std::string& conceptName) const {
    auto it = nameToIndex_.find(conceptName);
    if (it != nameToIndex_.end()) {
        return concepts_[it->second].pattern;
    }
    return std::vector<float>();
}

std::vector<std::string> NeuralSemanticMemory::getConceptNames() const {
    std::vector<std::string> names;
    names.reserve(concepts_.size());
    for (const auto& concept : concepts_) {
        names.push_back(concept.name);
    }
    return names;
}

void NeuralSemanticMemory::updateConcepts(const std::vector<std::vector<float>>& patterns) {
    if (!brain_) return;
    
    // Update concepts based on neural activity patterns
    for (const auto& pattern : patterns) {
        // Find most similar existing concept
        std::string bestMatch = "";
        float bestSimilarity = 0.0f;
        
        for (const auto& concept : concepts_) {
            float similarity = calculateSimilarity(pattern, concept.pattern);
            if (similarity > bestSimilarity) {
                bestSimilarity = similarity;
                bestMatch = concept.name;
            }
        }
        
        if (!bestMatch.empty() && bestSimilarity > 0.5f) {
            // Enhance existing concept
            auto it = nameToIndex_.find(bestMatch);
            if (it != nameToIndex_.end()) {
                concepts_[it->second].strength = std::min(1.0f, concepts_[it->second].strength + 0.05f);
            }
        } else if (patterns.size() < 10) {
            // Create new concept for novel patterns
            std::string newName = "concept_" + std::to_string(getConceptCount());
            storeConcept(newName, pattern);
        }
    }
}

std::vector<std::string> NeuralSemanticMemory::findSimilarConcepts(const std::vector<float>& pattern, float threshold) const {
    std::vector<std::string> similarConcepts;
    
    for (const auto& concept : concepts_) {
        float similarity = calculateSimilarity(pattern, concept.pattern);
        if (similarity >= threshold) {
            similarConcepts.push_back(concept.name);
        }
    }
    
    return similarConcepts;
}

float NeuralSemanticMemory::getConceptStrength(const std::string& conceptName) const {
    auto it = nameToIndex_.find(conceptName);
    if (it != nameToIndex_.end()) {
        return concepts_[it->second].strength;
    }
    return 0.0f;
}

void NeuralSemanticMemory::setConceptStrength(const std::string& conceptName, float strength) {
    auto it = nameToIndex_.find(conceptName);
    if (it != nameToIndex_.end()) {
        concepts_[it->second].strength = std::clamp(strength, 0.0f, 1.0f);
    }
}

void NeuralSemanticMemory::clear() {
    concepts_.clear();
    nameToIndex_.clear();
    NLM_LOG_INFO("Semantic memory cleared");
}

size_t NeuralSemanticMemory::getConceptCount() const {
    return concepts_.size();
}

void NeuralSemanticMemory::decayConcepts(float decayRate) {
    if (decayRate <= 0.0f) return;
    
    for (auto& concept : concepts_) {
        concept.strength *= (1.0f - decayRate);
        if (concept.strength < 0.01f) {
            // Remove weak concepts
            nameToIndex_.erase(concept.name);
        }
    }
    
    // Rebuild concepts vector after removal
    std::vector<Concept> newConcepts;
    for (const auto& concept : concepts_) {
        if (concept.strength >= 0.01f) {
            newConcepts.push_back(concept);
        }
    }
    concepts_ = std::move(newConcepts);
    
    // Update nameToIndex map
    nameToIndex_.clear();
    for (size_t i = 0; i < concepts_.size(); ++i) {
        nameToIndex_[concepts_[i].name] = i;
    }
}

float NeuralSemanticMemory::calculateSimilarity(const std::vector<float>& pattern1, const std::vector<float>& pattern2) const {
    if (pattern1.size() != pattern2.size() || pattern1.empty()) {
        return 0.0f;
    }
    
    float dotProduct = 0.0f;
    float norm1 = 0.0f;
    float norm2 = 0.0f;
    
    for (size_t i = 0; i < pattern1.size(); ++i) {
        dotProduct += pattern1[i] * pattern2[i];
        norm1 += pattern1[i] * pattern1[i];
        norm2 += pattern2[i] * pattern2[i];
    }
    
    float denominator = std::sqrt(norm1) * std::sqrt(norm2);
    return denominator > 0.0f ? dotProduct / denominator : 0.0f;
}

} // namespace nlm