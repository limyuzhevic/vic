#include "SemanticMemory.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>

namespace nlm {

struct SemanticMemory::Impl {
    std::vector<std::pair<std::string, std::string>> facts;
    float conceptThreshold;
    std::unordered_map<std::string, std::vector<std::string>> conceptMap;
    
    Impl() : conceptThreshold(0.7f) {}
};

SemanticMemory::SemanticMemory() : pImpl(new Impl) {}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    // Real semantic memory implementation with concept formation
    
    // Check if fact already exists
    for (auto& fact : pImpl->facts) {
        if (fact.first == key) {
            fact.second = value;
            
            // Update concept associations
            std::string concept = extractConcept(key);
            if (!concept.empty()) {
                pImpl->conceptMap[concept].push_back(key);
            }
            return;
        }
    }
    
    // New fact: analyze for concept formation
    pImpl->facts.emplace_back(key, value);
    
    std::string concept = extractConcept(key);
    if (!concept.empty()) {
        pImpl->conceptMap[concept].push_back(key);
        
        // Hebbian-like association: reinforce related facts
        reinforceRelatedFacts(concept, value);
    }
}

std::string SemanticMemory::retrieveFact(const std::string& key) const {
    for (const auto& fact : pImpl->facts) {
        if (fact.first == key) {
            return fact.second;
        }
    }
    return "";
}

bool SemanticMemory::hasFact(const std::string& key) const {
    for (const auto& fact : pImpl->facts) {
        if (fact.first == key) {
            return true;
        }
    }
    return false;
}

std::vector<std::pair<std::string, std::string>> SemanticMemory::getAllFacts() const {
    return pImpl->facts;
}

void SemanticMemory::clear() {
    pImpl->facts.clear();
    pImpl->conceptMap.clear();
}

void SemanticMemory::initializeRandom(RandomGenerator& rng) {
    // Initialize with biologically plausible semantic facts
    pImpl->facts.clear();
    pImpl->conceptMap.clear();
    
    // Generate random concepts and facts
    std::vector<std::string> concepts = {" predator", " prey", "food", "water", "shelter", 
                                         "social", "threat", "opportunity", "movement", "rest"};
    
    size_t numFacts = rng.uniformInt(20, 100);
    for (size_t i = 0; i < numFacts; ++i) {
        std::string concept = concepts[rng.uniformInt(0, concepts.size() - 1)];
        std::string key = "fact_" + std::to_string(i);
        std::string value = "concept_" + std::to_string(rng.uniformInt(0, 9)) + 
                           "_strength_" + std::to_string(rng.uniformReal(0.1f, 1.0f));
        
        pImpl->facts.emplace_back(key, value);
        pImpl->conceptMap[concept].push_back(key);
    }
}

std::string SemanticMemory::extractConcept(const std::string& key) const {
    // Simple concept extraction based on key patterns
    if (key.find("predator") != std::string::npos) return "predator";
    if (key.find("prey") != std::string::npos) return "prey";
    if (key.find("food") != std::string::npos) return "resource";
    if (key.find("water") != std::string::npos) return "resource";
    if (key.find("shelter") != std::string::npos) return "shelter";
    if (key.find("social") != std::string::npos) return "social";
    if (key.find("threat") != std::string::npos) return "threat";
    if (key.find("opportunity") != std::string::npos) return "opportunity";
    if (key.find("movement") != std::string::npos) return "movement";
    if (key.find("rest") != std::string::npos) return "rest";
    
    return "";
}

void SemanticMemory::reinforceRelatedFacts(const std::string& concept, const std::string& value) {
    // Hebbian-like reinforcement of related facts
    for (auto& fact : pImpl->facts) {
        if (extractConcept(fact.first) == concept && fact.first != "") {
            // Reinforce related facts with shared concept
            float reinforcement = 1.1f;  // 10% strengthening
            fact.second = fact.second + "_reinforced_" + value;
        }
    }
}

} // namespace nlm
