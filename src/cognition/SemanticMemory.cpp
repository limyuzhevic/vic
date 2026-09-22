#include "SemanticMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

SemanticMemory::SemanticMemory()
    : pImpl(new Impl)
{
    // Initialize with some basic concepts
    const float basicPatterns[] = {0.1f, 0.1f, 0.1f, 0.1f, 0.1f};
    std::vector<float> basicPattern(basicPatterns, basicPatterns + 5);
    concepts_.emplace_back("neutral", basicPattern);
    conceptStrengths_["neutral"] = 1.0f;
}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    concepts_.emplace_back(key, std::vector<float>(value.size(), 0.3f));
    conceptStrengths_[key] = 1.0f;
}

void SemanticMemory::storeFromNeurons(const std::vector<NeuronId>& neurons, const std::string& concept) {
    std::vector<float> pattern;
    for (size_t i = 0; i < 10; ++i) {
        pattern.push_back(0.1f + static_cast<float>(i) * 0.05f);
    }
    
    // Check if concept already exists
    for (auto& c : concepts_) {
        if (c.first == concept) {
            c.second = pattern;
            conceptStrengths_[concept] = 1.0f;
            return;
        }
    }
    
    concepts_.emplace_back(concept, pattern);
    conceptStrengths_[concept] = 1.0f;
}

std::string SemanticMemory::retrieveFact(const std::string& key) const {
    for (const auto& c : concepts_) {
        if (c.first == key) {
            return c.first; // Simple implementation
        }
    }
    return "";
}

bool SemanticMemory::hasFact(const std::string& key) const {
    return conceptStrengths_.find(key) != conceptStrengths_.end();
}

std::vector<std::pair<std::string, std::string>> SemanticMemory::getAllFacts() const {
    std::vector<std::pair<std::string, std::string>> facts;
    for (const auto& c : concepts_) {
        facts.emplace_back(c.first, c.first);
    }
    return facts;
}

std::string SemanticMemory::getConcept(const std::vector<float>& pattern) const {
    float bestSimilarity = 0.0f;
    std::string bestConcept;
    
    for (const auto& c : concepts_) {
        float similarity = 0.0f;
        if (pattern.size() == c.second.size() && !pattern.empty()) {
            for (size_t i = 0; i < pattern.size(); ++i) {
                similarity += pattern[i] * c.second[i];
            }
            similarity /= pattern.size();
        }
        
        if (similarity > bestSimilarity && conceptStrengths_.at(c.first) > 0.5f) {
            bestSimilarity = similarity;
            bestConcept = c.first;
        }
    }
    
    return bestConcept;
}

void SemanticMemory::clear() {
    concepts_.clear();
    conceptStrengths_.clear();
}

void SemanticMemory::updateConceptStrength(const std::string& concept, float delta) {
    auto it = conceptStrengths_.find(concept);
    if (it != conceptStrengths_.end()) {
        it->second = std::clamp(it->second + delta, 0.0f, 2.0f);
        if (it->second < 0.1f) {
            conceptStrengths_.erase(it);
            // Remove from concepts vector
            concepts_.erase(
                std::remove_if(concepts_.begin(), concepts_.end(),
                    [&concept](const auto& c) { return c.first == concept; }),
                concepts_.end()
            );
        }
    }
}

float SemanticMemory::getConceptStability(const std::string& concept) const {
    auto it = conceptStrengths_.find(concept);
    if (it == conceptStrengths_.end()) {
        return 0.0f;
    }
    return std::min(1.0f, it->second / 2.0f);
}

} // namespace nlm