#include "SemanticMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

class SemanticMemory::Impl {
public:
    Brain* brain;
    size_t capacity;
    
    // Concepts
    std::unordered_map<std::string, MemoryItem> concepts;
    
    // Similarity search structure
    std::vector<std::pair<std::string, std::vector<float>>> conceptPatterns;
    
    // Neural representations
    std::vector<size_t> activeNeurons;
    
    Impl(size_t cap) : capacity(cap) {}
};

SemanticMemory::SemanticMemory()
    : pImpl(new Impl(500))
{
    NLM_LOG_INFO("SemanticMemory created");
}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::store(const std::vector<float>& pattern, float relevance) {
    // Learn this pattern as a concept
    std::string conceptName = "concept_" + std::to_string(pImpl->concepts.size());
    learnConcept(conceptName, pattern);
    updateConceptStrength(conceptName, relevance);
}

std::vector<float> SemanticMemory::retrieve(size_t index) const {
    // Find concept by index
    size_t count = 0;
    for (const auto& pair : pImpl->concepts) {
        if (count == index) {
            return pair.second.pattern;
        }
        count++;
    }
    return {};
}

bool SemanticMemory::contains(const std::vector<float>& pattern) const {
    for (const auto& pair : pImpl->concepts) {
        const MemoryItem& concept = pair.second;
        if (concept.pattern.size() == pattern.size()) {
            bool match = true;
            for (size_t i = 0; i < pattern.size(); ++i) {
                if (std::abs(pattern[i] - concept.pattern[i]) > 0.1f) {
                    match = false;
                    break;
                }
            }
            if (match) return true;
        }
    }
    return false;
}

void SemanticMemory::decay(float decayRate) {
    for (auto& pair : pImpl->concepts) {
        pair.second.relevance *= (1.0f - decayRate);
        pair.second.strength *= (1.0f - decayRate);
    }
}

void SemanticMemory::consolidate(float relevanceThreshold) {
    // Remove low-relevance concepts
    pImpl->concepts.erase(
        std::remove_if(pImpl->concepts.begin(), pImpl->concepts.end(),
                     [relevanceThreshold](const auto& pair) {
                         return pair.second.relevance < relevanceThreshold;
                     }),
        pImpl->concepts.end()
    );
}

void SemanticMemory::clear() {
    pImpl->concepts.clear();
    pImpl->conceptPatterns.clear();
}

size_t SemanticMemory::getCapacity() const {
    return pImpl->capacity;
}

size_t SemanticMemory::getCurrentSize() const {
    return pImpl->concepts.size();
}

float SemanticMemory::getLoad() const {
    if (pImpl->capacity == 0) return 0.0f;
    return static_cast<float>(pImpl->concepts.size()) / pImpl->capacity;
}

void SemanticMemory::updateFromBrain(Brain* brain) {
    pImpl->brain = brain;
}

void SemanticMemory::applyToBrain(Brain* brain) {
    if (!brain || pImpl->brain != brain) return;
    
    // Apply high-strength concepts to brain
    for (const auto& pair : pImpl->concepts) {
        const MemoryItem& concept = pair.second;
        if (concept.strength > 0.5f) {
            // Inject concept pattern
            for (size_t i = 0; i < concept.pattern.size() && i < pImpl->activeNeurons.size(); ++i) {
                float activation = concept.pattern[i] * concept.strength * 2.0f;
                brain->injectCurrent(pImpl->activeNeurons[i], activation);
            }
        }
    }
}

void SemanticMemory::learnConcept(const std::string& name, const std::vector<float>& pattern) {
    MemoryItem concept(pattern.size(), pattern);
    concept.relevance = 1.0f;
    concept.strength = 1.0f;
    
    pImpl->concepts[name] = concept;
    pImpl->conceptPatterns.emplace_back(name, pattern);
}

std::string SemanticMemory::retrieveConcept(const std::vector<float>& pattern) const {
    float bestSimilarity = 0.0f;
    std::string bestMatch = "";
    
    for (const auto& pair : pImpl->concepts) {
        float similarity = computeSimilarity(pattern, pair.second.pattern);
        if (similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestMatch = pair.first;
        }
    }
    
    return bestMatch;
}

float SemanticMemory::getConceptStrength(const std::string& name) const {
    auto it = pImpl->concepts.find(name);
    if (it != pImpl->concepts.end()) {
        return it->second.strength;
    }
    return 0.0f;
}

void SemanticMemory::updateConceptStrength(const std::string& name, float delta) {
    auto it = pImpl->concepts.find(name);
    if (it != pImpl->concepts.end()) {
        it->second.strength = std::clamp(it->second.strength + delta, 0.0f, 1.0f);
        it->second.relevance = std::clamp(it->second.relevance + delta, 0.0f, 1.0f);
    }
}

std::vector<std::string> SemanticMemory::getAllConcepts() const {
    std::vector<std::string> names;
    names.reserve(pImpl->concepts.size());
    for (const auto& pair : pImpl->concepts) {
        names.push_back(pair.first);
    }
    return names;
}

void SemanticMemory::formConceptsFromData(const std::vector<std::vector<float>>& patterns) {
    // Simple clustering - in reality would use more sophisticated algorithms
    for (size_t i = 0; i < patterns.size(); i += 10) {  // Sample patterns
        std::string conceptName = "concept_cluster_" + std::to_string(i/10);
        
        // Average the patterns
        std::vector<float> avgPattern(patterns[0].size(), 0.0f);
        size_t count = 0;
        
        for (size_t j = 0; j < patterns.size() && count < 10; ++j) {
            if (patterns[j].size() == avgPattern.size()) {
                for (size_t k = 0; k < avgPattern.size(); ++k) {
                    avgPattern[k] += patterns[j][k];
                }
                count++;
            }
        }
        
        if (count > 0) {
            for (float& val : avgPattern) {
                val /= count;
            }
            learnConcept(conceptName, avgPattern);
        }
    }
}

void SemanticMemory::mergeConcepts(const std::string& conceptA, const std::string& conceptB) {
    auto itA = pImpl->concepts.find(conceptA);
    auto itB = pImpl->concepts.find(conceptB);
    
    if (itA != pImpl->concepts.end() && itB != pImpl->concepts.end()) {
        // Create merged concept
        std::string mergedName = "merged_" + conceptA + "_" + conceptB;
        
        MemoryItem merged(itA->second.pattern.size());
        for (size_t i = 0; i < itA->second.pattern.size(); ++i) {
            merged.pattern.push_back((itA->second.pattern[i] + itB->second.pattern[i]) * 0.5f);
        }
        
        merged.relevance = (itA->second.relevance + itB->second.relevance) * 0.5f;
        merged.strength = (itA->second.strength + itB->second.strength) * 0.5f;
        
        pImpl->concepts[mergedName] = merged;
        
        // Remove old concepts
        pImpl->concepts.erase(itA);
        pImpl->concepts.erase(itB);
        
        // Update concept patterns
        pImpl->conceptPatterns.clear();
        for (const auto& pair : pImpl->concepts) {
            pImpl->conceptPatterns.emplace_back(pair.first, pair.second.pattern);
        }
    }
}

float SemanticMemory::computeSimilarity(const std::vector<float>& a,
                                       const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    float dot = 0.0f, normA = 0.0f, normB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(normA) * std::sqrt(normB));
}

} // namespace nlm