#include "SemanticMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <unordered_map>
#include <sstream>

namespace nlm {

struct SemanticMemory::Impl {
    // Facts stored as key-value pairs with retrieval counts for reinforcement
    std::unordered_map<std::string, std::pair<std::string, int>> facts;
    
    // Concept hierarchies for semantic organization
    std::unordered_map<std::string, std::vector<std::string>> conceptHierarchy;
    
    // Associative links between concepts
    std::vector<std::pair<std::string, std::string>> conceptAssociations;
    
    // Fact usage statistics for importance ranking
    std::unordered_map<std::string, float> factImportance;
    
    Impl() {}
};

SemanticMemory::SemanticMemory() : pImpl(new Impl) {
    // Initialize with basic semantic concepts
    initializeBasicConcepts();
}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::initializeBasicConcepts() {
    // Add basic semantic concepts that are essential for the NLM to function
    storeFact("agent", "entity_that_takes_actions_in_environment");
    storeFact("world", "environment_containing_objects_and_states");
    storeFact("perception", "sensory_input_processed_by_brain");
    storeFact("action", "motor_output_produced_by_brain");
    storeFact("reward", "feedback_from_environment_promoting_behavior");
    storeFact("curiosity", "drive_to_explore_new_situations");
    storeFact("novelty", "unusual_or_unexpected_input");
    storeFact("prediction", "expected_outcome_or_future_state");
    storeFact("error", "mismatch_between_prediction_and_actual");
    storeFact("learning", "change_in_behavior_or_connection_strength");
}

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    pImpl->facts[key] = std::make_pair(value, 0); // Initialize retrieval count
    pImpl->factImportance[key] = 1.0f; // Default importance
    
    // Update concept hierarchies
    size_t spacePos = key.find(' ');
    if (spacePos != std::string::npos) {
        std::string parentConcept = key.substr(0, spacePos);
        std::string childConcept = key.substr(spacePos + 1);
        pImpl->conceptHierarchy[parentConcept].push_back(childConcept);
    }
    
    NLM_LOG_DEBUG("Stored semantic fact: " + key + " -> " + value);
}

std::string SemanticMemory::retrieveFact(const std::string& key) const {
    auto it = pImpl->facts.find(key);
    if (it != pImpl->facts.end()) {
        // Increment retrieval count for reinforcement learning
        const_cast<std::pair<std::string, int>&>(it->second).second++;
        
        // Increase importance based on usage
        float& importance = const_cast<float&>(pImpl->factImportance[key]);
        importance = std::min(1.0f, importance + 0.01f * it->second);
        
        return it->second.first;
    }
    return "";
}

bool SemanticMemory::hasFact(const std::string& key) const {
    return pImpl->facts.find(key) != pImpl->facts.end();
}

std::vector<std::pair<std::string, std::string>> SemanticMemory::getAllFacts() const {
    std::vector<std::pair<std::string, std::string>> result;
    for (const auto& fact : pImpl->facts) {
        result.emplace_back(fact.first, fact.second.first);
    }
    
    // Sort by importance
    std::sort(result.begin(), result.end(),
        [this](const auto& a, const auto& b) {
            float impA = pImpl->factImportance.at(a.first);
            float impB = pImpl->factImportance.at(b.first);
            return impA > impB;
        });
    
    return result;
}

void SemanticMemory::clear() {
    pImpl->facts.clear();
    pImpl->conceptHierarchy.clear();
    pImpl->conceptAssociations.clear();
    pImpl->factImportance.clear();
    NLM_LOG_INFO("Semantic memory cleared");
}

void SemanticMemory::associateConcepts(const std::string& conceptA, const std::string& conceptB, float strength) {
    // Check if already associated
    for (const auto& assoc : pImpl->conceptAssociations) {
        if ((assoc.first == conceptA && assoc.second == conceptB) ||
            (assoc.first == conceptB && assoc.second == conceptA)) {
            return;
        }
    }
    
    pImpl->conceptAssociations.emplace_back(conceptA, conceptB);
    
    // Update concept hierarchies if needed
    if (pImpl->conceptHierarchy.find(conceptA) == pImpl->conceptHierarchy.end() &&
        pImpl->conceptHierarchy.find(conceptB) == pImpl->conceptHierarchy.end()) {
        // Add both to a common root concept
        storeFact("abstract_connection", "");
    }
}

std::vector<std::string> SemanticMemory::getRelatedConcepts(const std::string& concept) const {
    std::vector<std::string> related;
    
    // Get child concepts from hierarchy
    auto it = pImpl->conceptHierarchy.find(concept);
    if (it != pImpl->conceptHierarchy.end()) {
        for (const auto& child : it->second) {
            related.push_back(child);
        }
    }
    
    // Get parent concepts (reverse lookup)
    for (const auto& parent : pImpl->conceptHierarchy) {
        for (const auto& child : parent.second) {
            if (child == concept) {
                related.push_back(parent.first);
            }
        }
    }
    
    // Get directly associated concepts
    for (const auto& assoc : pImpl->conceptAssociations) {
        if (assoc.first == concept && !contains(related, assoc.second)) {
            related.push_back(assoc.second);
        }
        if (assoc.second == concept && !contains(related, assoc.first)) {
            related.push_back(assoc.first);
        }
    }
    
    return related;
}

float SemanticMemory::getFactImportance(const std::string& key) const {
    auto it = pImpl->factImportance.find(key);
    return (it != pImpl->factImportance.end()) ? it->second : 0.0f;
}

std::vector<std::string> SemanticMemory::getConceptsByImportance() const {
    std::vector<std::pair<std::string, float>> conceptScores;
    
    // Collect concept scores from facts
    for (const auto& fact : pImpl->facts) {
        conceptScores.emplace_back(fact.first, pImpl->factImportance.at(fact.first));
    }
    
    // Also include concepts from associations
    for (const auto& assoc : pImpl->conceptAssociations) {
        conceptScores.emplace_back(assoc.first, pImpl->factImportance.count(assoc.first) ? 
            pImpl->factImportance.at(assoc.first) : 0.5f);
        conceptScores.emplace_back(assoc.second, pImpl->factImportance.count(assoc.second) ? 
            pImpl->factImportance.at(assoc.second) : 0.5f);
    }
    
    // Remove duplicates and sort
    std::sort(conceptScores.begin(), conceptScores.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::vector<std::string> result;
    for (const auto& pair : conceptScores) {
        if (!contains(result, pair.first)) {
            result.push_back(pair.first);
        }
    }
    
    return result;
}

bool SemanticMemory::contains(const std::vector<std::string>& vec, const std::string& value) const {
    return std::find(vec.begin(), vec.end(), value) != vec.end();
}

} // namespace nlm