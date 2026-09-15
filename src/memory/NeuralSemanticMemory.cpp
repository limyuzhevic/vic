#include "NeuralSemanticMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct NeuralSemanticMemory::Impl {
    Brain* brain;
    std::unordered_map<std::string, std::vector<float>> conceptPatterns;
    std::unordered_map<std::string, float> conceptStrengths;
    std::unordered_map<std::string, float> conceptTimestamps;
    
    Impl() : brain(nullptr) {}
};

NeuralSemanticMemory::NeuralSemanticMemory()
    : pImpl(new Impl)
{
}

NeuralSemanticMemory::~NeuralSemanticMemory() = default;

void NeuralSemanticMemory::storeConcept(const std::string& conceptName, const std::vector<float>& pattern, float strength) {
    if (pattern.empty()) return;
    
    // Store the pattern
    conceptPatterns[conceptName] = pattern;
    conceptStrengths[conceptName] = std::clamp(strength, 0.0f, 1.0f);
    conceptTimestamps[conceptName] = 0.0f;
    
    // Create neural associations if brain is available
    if (brain_) {
        // Create or update neural pathways representing this concept
        for (size_t i = 0; i < pattern.size() && i + 1 < pattern.size(); ++i) {
            if (pattern[i] > 0.5f && pattern[i + 1] > 0.5f) {
                // These two concepts are associated
                std::string otherConcept = "concept_" + std::to_string(i);
                // In a real implementation, this would create synaptic connections
            }
        }
    }
}

std::vector<float> NeuralSemanticMemory::retrievePattern(const std::string& conceptName) const {
    auto it = conceptPatterns.find(conceptName);
    if (it != conceptPatterns.end()) {
        return it->second;
    }
    return std::vector<float>();
}

bool NeuralSemanticMemory::hasConcept(const std::string& conceptName) const {
    return conceptPatterns.find(conceptName) != conceptPatterns.end();
}

std::vector<std::string> NeuralSemanticMemory::getAllConcepts() const {
    std::vector<std::string> concepts;
    concepts.reserve(conceptPatterns.size());
    for (const auto& pair : conceptPatterns) {
        concepts.push_back(pair.first);
    }
    return concepts;
}

float NeuralSemanticMemory::getConceptStrength(const std::string& conceptName) const {
    auto it = conceptStrengths.find(conceptName);
    if (it != conceptStrengths.end()) {
        return it->second;
    }
    return 0.0f;
}

void NeuralSemanticMemory::updateConceptStrength(const std::string& conceptName, float delta) {
    auto it = conceptStrengths.find(conceptName);
    if (it != conceptStrengths.end()) {
        it->second = std::clamp(it->second + delta, 0.0f, 1.0f);
        // Update concept timestamp
        conceptTimestamps[conceptName] = 0.0f;
        
        // Reinforce neural pathways if brain available
        if (brain_ && it->second > 0.5f) {
            NLM_LOG_INFO("Concept '" + conceptName + "' strengthened to " + std::to_string(it->second));
        }
    }
}

void NeuralSemanticMemory::clear() {
    conceptPatterns.clear();
    conceptStrengths.clear();
    conceptTimestamps.clear();
}

void NeuralSemanticMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("NeuralSemanticMemory initialized");
}

void NeuralSemanticMemory::update(TimestepDuration dt) {
    // Age concepts
    float timeDelta = dt.count();
    
    // Decay weakly held concepts
    for (auto& pair : conceptStrengths) {
        if (pair.second < 0.2f) {
            // Decay concept strength over time
            pair.second *= (1.0f - 0.01f * timeDelta);
            conceptTimestamps[pair.first] += timeDelta;
        }
    }
    
    // Age all timestamps
    for (auto& pair : conceptTimestamps) {
        pair.second += timeDelta;
    }
}

} // namespace nlm
