#include "NeuralSemanticMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <set>

namespace nlm {

struct NeuralSemanticMemory::Impl {
    Brain* brain;
    
    // Pattern neurons for concept storage
    std::vector<std::pair<std::string, std::vector<float>>> conceptNeurons;
    
    // Learning parameters
    float conceptLearningRate;
    float patternCompletionThreshold;
    float associationThreshold;
    
    Impl() : brain(nullptr), conceptLearningRate(0.1f), 
             patternCompletionThreshold(0.6f), associationThreshold(0.3f) {}
};

NeuralSemanticMemory::NeuralSemanticMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
{
}

NeuralSemanticMemory::~NeuralSemanticMemory() = default;

void NeuralSemanticMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralSemanticMemory initialized");
}

void NeuralSemanticMemory::storeConcept(const std::string& name, 
                                        const std::vector<float>& pattern,
                                        const std::vector<std::string>& relatedConcepts) {
    if (pattern.empty() || !brain_) return;
    
    // Check if concept already exists
    if (conceptPrototypes_.find(name) != conceptPrototypes_.end()) {
        // Update existing concept
        updateConcept(name, pattern, pImpl->conceptLearningRate);
    } else {
        // Create new concept
        conceptPrototypes_[name] = pattern;
        establishConceptHierarchy(name);
        
        // Store in concept neurons
        pImpl->conceptNeurons.push_back({name, pattern});
        
        // Create associations with related concepts
        for (const auto& related : relatedConcepts) {
            if (conceptPrototypes_.find(related) != conceptPrototypes_.end()) {
                associateConcepts(name, related, 0.5f);
            }
        }
        
        // Establish neural connections
        std::vector<float> patternCopy = pattern;
        for (float& val : patternCopy) {
            val *= 5.0f;  // Scale for neural injection
        }
        
        // Inject pattern into neural network
        for (size_t i = 0; i < patternCopy.size() && i < conceptPrototypes_.size(); ++i) {
            // Find neuron for this pattern position
            // This is a simplified approach - real implementation would use proper neural mapping
        }
    }
    
    // Reset activation
    conceptActivations_[name] = 0.0f;
}

std::vector<float> NeuralSemanticMemory::retrieveConcept(const std::string& name) const {
    auto it = conceptPrototypes_.find(name);
    if (it != conceptPrototypes_.end()) {
        return it->second;
    }
    return std::vector<float>();
}

std::vector<std::pair<std::string, float>> NeuralSemanticMemory::retrieveSimilar(
    const std::vector<float>& pattern, size_t maxResults) const {
    
    std::vector<std::pair<std::string, float>> results;
    
    for (const auto& entry : conceptPrototypes_) {
        const std::string& name = entry.first;
        const std::vector<float>& conceptPattern = entry.second;
        
        float similarity = computeConceptSimilarity(pattern, conceptPattern);
        if (similarity > pImpl->patternCompletionThreshold) {
            results.emplace_back(name, similarity);
        }
    }
    
    // Sort by similarity (descending)
    std::sort(results.begin(), results.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Limit results
    if (results.size() > maxResults) {
        results.resize(maxResults);
    }
    
    return results;
}

bool NeuralSemanticMemory::hasConcept(const std::string& name) const {
    return conceptPrototypes_.find(name) != conceptPrototypes_.end();
}

std::vector<std::string> NeuralSemanticMemory::getAllConcepts() const {
    std::vector<std::string> names;
    names.reserve(conceptPrototypes_.size());
    for (const auto& entry : conceptPrototypes_) {
        names.push_back(entry.first);
    }
    return names;
}

std::vector<std::string> NeuralSemanticMemory::getRelatedConcepts(const std::string& name) const {
    auto it = conceptRelationships_.find(name);
    if (it != conceptRelationships_.end()) {
        return it->second;
    }
    return std::vector<std::string>();
}

void NeuralSemanticMemory::associateConcepts(const std::string& conceptA,
                                            const std::string& conceptB,
                                            float strength) {
    if (!hasConcept(conceptA) || !hasConcept(conceptB)) return;
    
    conceptAssociations_[conceptA][conceptB] = strength;
    conceptAssociations_[conceptB][conceptA] = strength;
    
    // Also add to relationships
    conceptRelationships_[conceptA].push_back(conceptB);
    conceptRelationships_[conceptB].push_back(conceptA);
}

float NeuralSemanticMemory::getConceptAssociation(const std::string& conceptA,
                                                 const std::string& conceptB) const {
    auto itA = conceptAssociations_.find(conceptA);
    if (itA != conceptAssociations_.end()) {
        auto itB = itA->second.find(conceptB);
        if (itB != itA->second.end()) {
            return itB->second;
        }
    }
    return 0.0f;
}

void NeuralSemanticMemory::updateConcept(const std::string& name,
                                         const std::vector<float>& newPattern,
                                         float learningRate) {
    if (!hasConcept(name)) {
        storeConcept(name, newPattern);
        return;
    }
    
    // Gradually update concept prototype
    std::vector<float>& oldPattern = conceptPrototypes_[name];
    if (oldPattern.size() != newPattern.size()) {
        // Handle size mismatch by truncating or padding
        if (newPattern.size() > oldPattern.size()) {
            oldPattern.resize(newPattern.size(), 0.0f);
        } else {
            // Truncate
            oldPattern = std::vector<float>(newPattern.begin(), newPattern.begin() + oldPattern.size());
        }
    }
    
    for (size_t i = 0; i < oldPattern.size(); ++i) {
        oldPattern[i] = oldPattern[i] * (1.0f - learningRate) + 
                      newPattern[i] * learningRate;
    }
}

float NeuralSemanticMemory::getConceptActivation(const std::string& name) const {
    auto it = conceptActivations_.find(name);
    if (it != conceptActivations_.end()) {
        return it->second;
    }
    return 0.0f;
}

float NeuralSemanticMemory::getAverageActivation() const {
    if (conceptActivations_.empty()) return 0.0f;
    
    float total = 0.0f;
    for (const auto& entry : conceptActivations_) {
        total += entry.second;
    }
    return total / conceptActivations_.size();
}

void NeuralSemanticMemory::clear() {
    conceptPrototypes_.clear();
    conceptLevels_.clear();
    conceptRelationships_.clear();
    conceptAssociations_.clear();
    conceptActivations_.clear();
    conceptRewards_.clear();
    conceptPredictions_.clear();
    pImpl->conceptNeurons.clear();
}

void NeuralSemanticMemory::updateConceptTD(const std::string& name,
                                           const std::vector<float>& reward,
                                           float delta, float learningRate) {
    if (!hasConcept(name)) return;
    
    // Temporal difference learning update
    auto itPred = conceptPredictions_.find(name);
    if (itPred != conceptPredictions_.end()) {
        std::vector<float>& prediction = itPred->second;
        std::vector<float>& newReward = reward;
        
        for (size_t i = 0; i < prediction.size() && i < newReward.size(); ++i) {
            prediction[i] += learningRate * (newReward[i] - prediction[i] + delta);
        }
    }
}

size_t NeuralSemanticMemory::getConceptLevel(const std::string& name) const {
    auto it = conceptLevels_.find(name);
    if (it != conceptLevels_.end()) {
        return it->second;
    }
    return 0; // Default to atomic level
}

std::vector<float> NeuralSemanticMemory::getConceptPattern(const std::string& name) {
    // Find existing concept or create new one
    auto it = conceptPrototypes_.find(name);
    if (it != conceptPrototypes_.end()) {
        return it->second;
    }
    
    // Create new concept pattern
    std::vector<float> newPattern;
    // This would normally create a distributed pattern based on neural architecture
    // For now, return empty pattern
    return newPattern;
}

float NeuralSemanticMemory::computeConceptSimilarity(const std::vector<float>& pattern1,
                                                   const std::vector<float>& pattern2) const {
    if (pattern1.size() != pattern2.size() || pattern1.empty()) return 0.0f;
    
    // Cosine similarity
    float dot = 0.0f, norm1 = 0.0f, norm2 = 0.0f;
    for (size_t i = 0; i < pattern1.size(); ++i) {
        dot += pattern1[i] * pattern2[i];
        norm1 += pattern1[i] * pattern1[i];
        norm2 += pattern2[i] * pattern2[i];
    }
    
    if (norm1 < 0.0001f || norm2 < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(norm1) * std::sqrt(norm2));
}

void NeuralSemanticMemory::establishConceptHierarchy(const std::string& conceptName) {
    // Simple hierarchy: based on concept name length and character patterns
    // In a real implementation, this would be based on conceptual complexity
    size_t level = 0;
    if (conceptName.find('_') != std::string::npos) {
        level = 1; // Intermediate concept
    }
    if (conceptName.length() > 10) {
        level = std::min(level + 1, 2u); // Abstract concept
    }
    conceptLevels_[conceptName] = level;
}

} // namespace nlm
