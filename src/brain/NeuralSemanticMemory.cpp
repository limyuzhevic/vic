#include "NeuralSemanticMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct NeuralSemanticMemory::Impl {
    Brain* brain;
    
    // For concept competition - winner-take-all
    std::vector<float> conceptActivations;
    
    Impl() : brain(nullptr) {}
};

NeuralSemanticMemory::NeuralSemanticMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , lastCleanupStep_(0)
{
}

NeuralSemanticMemory::~NeuralSemanticMemory() = default;

void NeuralSemanticMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralSemanticMemory initialized");
}

bool NeuralSemanticMemory::learnConcept(const std::vector<float>& pattern,
                                       const std::vector<float>& features,
                                       float reward,
                                       SimulationStep currentTime) {
    if (pattern.empty()) return false;
    
    // Check for existing concept
    size_t existingConcept = 0;
    bool found = false;
    
    for (size_t i = 0; i < concepts_.size(); ++i) {
        float similarity = computeConceptSimilarity(pattern, concepts_[i]);
        if (similarity > 0.7f) {
            existingConcept = i;
            found = true;
            break;
        }
    }
    
    if (found) {
        // Update existing concept
        ConceptForSemanticMemory& concept = concepts_[existingConcept];
        
        // Update pattern through Hebbian-like learning
        float alpha = 0.1f;  // Learning rate
        if (concept.pattern.size() == pattern.size()) {
            for (size_t i = 0; i < concept.pattern.size(); ++i) {
                concept.pattern[i] = concept.pattern[i] * (1.0f - alpha) + pattern[i] * alpha;
            }
        }
        
        // Update features
        if (!features.empty() && concept.features.size() == features.size()) {
            for (size_t i = 0; i < concept.features.size(); ++i) {
                concept.features[i] = concept.features[i] * 0.9f + features[i] * 0.1f;
            }
        }
        
        // Update reliability based on reward
        concept.reliability = (concept.reliability * 0.9f) + (reward > 0.0f ? 0.5f : 0.2f);
        concept.confirmationCount++;
        concept.learnedAt = currentTime;
        
        return true;
    } else {
        // Create new concept
        ConceptForSemanticMemory newConcept;
        newConcept.pattern = pattern;
        newConcept.features = features;
        newConcept.learnedAt = currentTime;
        newConcept.reliability = (reward > 0.0f) ? 0.8f : 0.3f;
        newConcept.confirmationCount = 1;
        
        concepts_.push_back(newConcept);
        NLM_LOG_INFO("NeuralSemanticMemory: Created new concept with reliability " + 
                    std::to_string(newConcept.reliability));
        
        return true;
    }
}

const ConceptForSemanticMemory* NeuralSemanticMemory::retrieveConcept(
    const std::vector<float>& pattern) const {
    if (concepts_.empty() || pattern.empty()) return nullptr;
    
    float bestSimilarity = 0.0f;
    size_t bestIndex = 0;
    
    for (size_t i = 0; i < concepts_.size(); ++i) {
        float similarity = computeConceptSimilarity(pattern, concepts_[i]);
        if (similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestIndex = i;
        }
    }
    
    if (bestSimilarity > 0.5f) {
        return &concepts_[bestIndex];
    }
    
    return nullptr;
}

const ConceptForSemanticMemory* NeuralSemanticMemory::getConcept(size_t index) const {
    if (index < concepts_.size()) {
        return &concepts_[index];
    }
    return nullptr;
}

void NeuralSemanticMemory::confirmConcept(size_t conceptIndex, float confidence) {
    if (conceptIndex < concepts_.size()) {
        ConceptForSemanticMemory& concept = concepts_[conceptIndex];
        concept.reliability = std::min(1.0f, concept.reliability + confidence * 0.1f);
        concept.confirmationCount++;
    }
}

void NeuralSemanticMemory::decayConcepts(SimulationStep currentTime, float decayRate) {
    if (currentTime - lastCleanupStep_ < 100) return;  // Clean up every 100 steps
    
    lastCleanupStep_ = currentTime;
    
    std::vector<ConceptForSemanticMemory> survivingConcepts;
    
    for (const auto& concept : concepts_) {
        // Decay concepts that haven't been confirmed recently
        float age = (currentTime - concept.learnedAt) / 1000.0f;  // Normalize to steps
        if (age > 10.0f && concept.reliability < 0.3f) {
            // Too old and unreliable - discard
            continue;
        }
        
        // Apply general decay
        ConceptForSemanticMemory surviving = concept;
        surviving.reliability *= (1.0f - decayRate);
        
        if (surviving.reliability > 0.1f) {
            survivingConcepts.push_back(surviving);
        }
    }
    
    concepts_ = survivingConcepts;
    NLM_LOG_INFO("NeuralSemanticMemory: Decayed to " + std::to_string(concepts_.size()) + " concepts");
}

void NeuralSemanticMemory::clear() {
    concepts_.clear();
    lastCleanupStep_ = 0;
}

size_t NeuralSemanticMemory::findOrCreateConcept(const std::vector<float>& pattern,
                                                 const std::vector<float>& features,
                                                 float reward) {
    // Implementation would search for similar concepts and create new ones if needed
    return 0;
}

float NeuralSemanticMemory::computeConceptSimilarity(const std::vector<float>& pattern,
                                                    const ConceptForSemanticMemory& concept) const {
    if (pattern.size() != concept.pattern.size() || pattern.empty()) return 0.0f;
    
    // Cosine similarity
    float dot = 0.0f, normPattern = 0.0f, normConcept = 0.0f;
    
    for (size_t i = 0; i < pattern.size(); ++i) {
        dot += pattern[i] * concept.pattern[i];
        normPattern += pattern[i] * pattern[i];
        normConcept += concept.pattern[i] * concept.pattern[i];
    }
    
    if (normPattern < 0.0001f || normConcept < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(normPattern) * std::sqrt(normConcept));
}

} // namespace nlm
