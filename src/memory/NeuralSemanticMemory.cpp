#include "NeuralSemanticMemory.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <queue>

namespace nlm {

struct NeuralSemanticMemory::Impl {
    Brain* brain;
    float similarityThreshold;
    float activationDecayRate;
    size_t maxConcepts;
    
    Impl() : brain(nullptr), similarityThreshold(0.8f), activationDecayRate(0.01f), maxConcepts(1000) {}
};

NeuralSemanticMemory::NeuralSemanticMemory() : pImpl(new Impl) {}

NeuralSemanticMemory::~NeuralSemanticMemory() = default;

void NeuralSemanticMemory::initialize(Brain* brain) {
    this->brain_ = brain;
    pImpl->brain = brain;
    NLM_LOG_INFO("NeuralSemanticMemory initialized");
}

void NeuralSemanticMemory::storeConcept(const std::string& name,
                                       const std::vector<float>& pattern,
                                       const std::string& description) {
    // Limit total concepts
    if (concepts_.size() >= pImpl->maxConcepts) {
        // Remove least used concept
        std::string leastUsedConcept;
        float minActivation = std::numeric_limits<float>::max();
        for (const auto& pair : conceptActivations_) {
            if (pair.second < minActivation) {
                minActivation = pair.second;
                leastUsedConcept = pair.first;
            }
        }
        if (!leastUsedConcept.empty()) {
            concepts_.erase(leastUsedConcept);
            conceptNeurons_.erase(leastUsedConcept);
            conceptLevels_.erase(leastUsedConcept);
            conceptActivations_.erase(leastUsedConcept);
            conceptDescriptions_.erase(leastUsedConcept);
            conceptSubconcepts_.erase(leastUsedConcept);
        }
    }
    
    // Store concept
    concepts_[name] = pattern;
    conceptNeurons_[name] = findOrCreatePatternNeuron(pattern);
    conceptLevels_[name] = 0;  // Default to root level
    conceptActivations_[name] = 1.0f;  // Initial activation
    conceptDescriptions_[name] = description;
    
    NLM_LOG_INFO("Stored concept: " + name);
}

const std::vector<float>* NeuralSemanticMemory::getConcept(const std::string& name) const {
    auto it = concepts_.find(name);
    if (it != concepts_.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<std::string> NeuralSemanticMemory::findSimilarConcepts(const std::vector<float>& pattern,
                                                                  size_t maxResults) const {
    std::vector<std::pair<std::string, float>> similarConcepts;
    
    for (const auto& pair : concepts_) {
        float similarity = computeSemanticSimilarity(pattern, pair.second);
        if (similarity >= pImpl->similarityThreshold) {
            similarConcepts.emplace_back(pair.first, similarity);
        }
    }
    
    // Sort by similarity
    std::sort(similarConcepts.begin(), similarConcepts.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Return top results
    std::vector<std::string> result;
    size_t count = 0;
    for (const auto& pair : similarConcepts) {
        if (count >= maxResults) break;
        result.push_back(pair.first);
        ++count;
    }
    
    return result;
}

int NeuralSemanticMemory::getConceptLevel(const std::string& name) const {
    auto it = conceptLevels_.find(name);
    if (it != conceptLevels_.end()) {
        return it->second;
    }
    return -1;  // Not found
}

std::vector<std::string> NeuralSemanticMemory::getAllConcepts() const {
    std::vector<std::string> result;
    result.reserve(concepts_.size());
    for (const auto& pair : concepts_) {
        result.push_back(pair.first);
    }
    return result;
}

void NeuralSemanticMemory::clear() {
    concepts_.clear();
    conceptNeurons_.clear();
    conceptLevels_.clear();
    conceptActivations_.clear();
    conceptDescriptions_.clear();
    conceptSubconcepts_.clear();
    NLM_LOG_INFO("NeuralSemanticMemory cleared");
}

void NeuralSemanticMemory::updateFromExperience(const std::vector<float>& sensoryPattern,
                                              const std::vector<float>& resultingPattern) {
    // Find similar concepts for the sensory pattern
    std::vector<std::string> similarConcepts = findSimilarConcepts(sensoryPattern, 3);
    
    if (!similarConcepts.empty()) {
        // Update concept activations based on experience
        for (const std::string& conceptName : similarConcepts) {
            float activationDelta = 0.1f * (resultingPattern.size() > 0 ? 1.0f : 0.0f);
            conceptActivations_[conceptName] = 
                std::min(1.0f, conceptActivations_[conceptName] + activationDelta);
        }
    }
    
    // Create new concept from resulting pattern if not already present
    std::vector<std::string> resultConcepts = findSimilarConcepts(resultingPattern, 1);
    if (resultConcepts.empty()) {
        // Create concept from resulting pattern
        std::string conceptName = "Concept_" + std::to_string(concepts_.size());
        storeConcept(conceptName, resultingPattern, "Derived from experience");
    }
}

float NeuralSemanticMemory::getConceptActivation(const std::string& name) const {
    auto it = conceptActivations_.find(name);
    if (it != conceptActivations_.end()) {
        return it->second;
    }
    return 0.0f;
}

NeuronId NeuralSemanticMemory::findOrCreatePatternNeuron(const std::vector<float>& pattern) {
    // Check if we already have a similar pattern
    for (const auto& pair : patternNeurons_) {
        if (computeSimilarity(pattern, pair.second) > 0.8f) {
            return pair.second;
        }
    }
    
    // Create new neuron for this pattern
    NeuronId newId(INVALID_NEURON_ID.index() + patternNeurons_.size() + 1);
    patternNeurons_[newId.index()] = newId;
    concepts_[newId.index() + "_temp"] = pattern;  // Temp entry for consistency
    
    return newId;
}

float NeuralSemanticMemory::computeSimilarity(const std::vector<float>& a,
                                           const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    float dotProduct = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dotProduct += a[i] * b[i];
    }
    
    float magnitudeA = 0.0f;
    float magnitudeB = 0.0f;
    for (const auto& val : a) magnitudeA += val * val;
    for (const auto& val : b) magnitudeB += val * val;
    
    magnitudeA = std::sqrt(magnitudeA);
    magnitudeB = std::sqrt(magnitudeB);
    
    if (magnitudeA == 0.0f || magnitudeB == 0.0f) return 0.0f;
    
    return dotProduct / (magnitudeA * magnitudeB);
}

} // namespace nlm
