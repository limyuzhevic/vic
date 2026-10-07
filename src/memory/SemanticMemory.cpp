#include "SemanticMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <queue>

namespace nlm {

struct SemanticMemory::Impl {
    std::priority_queue<std::pair<float, std::string>> activeConcepts;
    std::unordered_map<std::string, size_t> conceptStepMap;
    std::unordered_map<std::string, std::vector<std::pair<std::string, std::string>>> conceptToConceptMap;
};

SemanticMemory::SemanticMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , nextConceptId_(0)
{
}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::initialize(Brain* brain) {
    pImpl = std::make_unique<Impl>();
    brain_ = brain;
    nextConceptId_ = 0;
    NLM_LOG_INFO("SemanticMemory initialized");
}

void SemanticMemory::storeConcept(const std::string& name,
                                 const std::vector<float>& activationPattern,
                                 const std::string& description) {
    if (name.empty()) return;
    
    Concept concept;
    concept.name = name;
    concept.activationPattern = activationPattern;
    concept.description = description;
    concept.activationLevel = 1.0f;
    concept.creationStep = nextConceptId_++;
    
    concepts_[name] = concept;
    conceptPatterns_.push_back(activationPattern);
    
    pImpl->activeConcepts.push({1.0f, name});
    pImpl->conceptStepMap[name] = concept.creationStep;
    
    NLM_LOG_INFO("Stored concept: " + name + " (pattern size: " + std::to_string(activationPattern.size()) + ")");
}

std::vector<float> SemanticMemory::retrieveConcept(const std::string& name) const {
    auto it = concepts_.find(name);
    if (it != concepts_.end()) {
        return it->second.activationPattern;
    }
    return std::vector<float>();
}

bool SemanticMemory::hasConcept(const std::string& name) const {
    return concepts_.find(name) != concepts_.end();
}

std::string SemanticMemory::getConceptDescription(const std::string& name) const {
    auto it = concepts_.find(name);
    if (it != concepts_.end()) {
        return it->second.description;
    }
    return "";
}

std::vector<std::string> SemanticMemory::getConceptNames() const {
    std::vector<std::string> names;
    names.reserve(concepts_.size());
    for (const auto& pair : concepts_) {
        names.push_back(pair.first);
    }
    return names;
}

void SemanticMemory::clear() {
    concepts_.clear();
    conceptPatterns_.clear();
    pImpl->activeConcepts = std::priority_queue<std::pair<float, std::string>>();
    pImpl->conceptStepMap.clear();
    pImpl->conceptToConceptMap.clear();
    NLM_LOG_INFO("SemanticMemory cleared");
}

std::vector<std::string> SemanticMemory::findSimilarConcepts(const std::vector<float>& pattern,
                                                             size_t maxResults) const {
    auto nearest = findNearestConcepts(pattern, maxResults * 2);
    
    std::vector<std::string> similarConcepts;
    similarConcepts.reserve(nearest.size());
    
    for (const auto& pair : nearest) {
        similarConcepts.push_back(pair.first);
        if (similarConcepts.size() >= maxResults) break;
    }
    
    return similarConcepts;
}

void SemanticMemory::linkConcepts(const std::string& conceptA, const std::string& conceptB,
                                  const std::string& relationshipType, float strength) {
    if (!hasConcept(conceptA) || !hasConcept(conceptB)) return;
    
    pImpl->conceptToConceptMap[conceptA].push_back({conceptB, relationshipType});
    pImpl->conceptToConceptMap[conceptB].push_back({conceptA, relationshipType});
}

std::vector<std::string> SemanticMemory::getRelatedConcepts(const std::string& conceptName,
                                                            const std::string& relationshipType) const {
    std::vector<std::string> related;
    
    auto it = pImpl->conceptToConceptMap.find(conceptName);
    if (it == pImpl->conceptToConceptMap.end()) return related;
    
    for (const auto& pair : it->second) {
        if (relationshipType.empty() || pair.second == relationshipType) {
            related.push_back(pair.first);
        }
    }
    
    return related;
}

void SemanticMemory::updateConcept(const std::string& name,
                                   const std::vector<float>& newPattern,
                                   float learningRate) {
    auto it = concepts_.find(name);
    if (it == concepts_.end()) return;
    
    // Interpolate between old and new patterns
    std::vector<float>& oldPattern = it->second.activationPattern;
    size_t minSize = std::min(oldPattern.size(), newPattern.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        oldPattern[i] = oldPattern[i] * (1.0f - learningRate) + newPattern[i] * learningRate;
    }
    
    // Update activation level
    it->second.activationLevel = std::min(1.0f, it->second.activationLevel * 1.1f);
}

float SemanticMemory::getConceptActivation(const std::string& name) const {
    auto it = concepts_.find(name);
    if (it != concepts_.end()) {
        return it->second.activationLevel;
    }
    return 0.0f;
}

void SemanticMemory::encodeFromSensoryInput(const std::vector<float>& sensoryPattern,
                                            const std::string& suggestedName) {
    // Simple encoding: create a concept based on the pattern
    std::string name = suggestedName.empty() ? "Concept_" + std::to_string(nextConceptId_++) : suggestedName;
    storeConcept(name, sensoryPattern, "Encoded from sensory input");
}

float SemanticMemory::computePatternSimilarity(const std::vector<float>& patternA,
                                              const std::vector<float>& patternB) const {
    if (patternA.size() != patternB.size()) return 0.0f;
    
    float dotProduct = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    
    for (size_t i = 0; i < patternA.size(); ++i) {
        dotProduct += patternA[i] * patternB[i];
        normA += patternA[i] * patternA[i];
        normB += patternB[i] * patternB[i];
    }
    
    if (normA == 0.0f || normB == 0.0f) return 0.0f;
    
    return dotProduct / std::sqrt(normA * normB);
}

std::vector<std::pair<std::string, float>> SemanticMemory::findNearestConcepts(const std::vector<float>& pattern,
                                                                               size_t maxResults) const {
    std::vector<std::pair<std::string, float>> similarities;
    similarities.reserve(conceptPatterns_.size());
    
    for (const auto& pair : concepts_) {
        float similarity = computePatternSimilarity(pattern, pair.second.activationPattern);
        similarities.push_back({pair.first, similarity});
    }
    
    std::partial_sort(similarities.begin(), std::min(similarities.begin() + maxResults, similarities.end()),
                     similarities.end(),
                     [](const auto& a, const auto& b) { return a.second > b.second; });
    
    similarities.resize(std::min(similarities.size(), maxResults));
    return similarities;
}

std::string SemanticMemory::explainConcept(const std::string& name) const {
    auto it = concepts_.find(name);
    if (it == concepts_.end()) return "Concept not found";
    
    std::ostringstream oss;
    oss << "Concept: " << it->second.name << "\n";
    oss << "Description: " << it->second.description << "\n";
    oss << "Activation Level: " << it->second.activationLevel << "\n";
    oss << "Pattern Size: " << it->second.activationPattern.size() << "\n";
    
    // Get related concepts
    auto related = getRelatedConcepts(name);
    if (!related.empty()) {
        oss << "Related Concepts: ";
        for (size_t i = 0; i < related.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << related[i];
        }
        oss << "\n";
    }
    
    return oss.str();
}

} // namespace nlm
