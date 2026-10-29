#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <cmath>
#include <memory>

namespace nlm {

// Enhanced Semantic Memory: Neural pattern-based concept formation
// Replaces placeholder SemanticMemory with real neural implementation

class NeuralSemanticMemory {
public:
    NeuralSemanticMemory();
    ~NeuralSemanticMemory();
    
    // Initialize with brain reference for neural integration
    bool initialize(Brain* brain);
    
    // Process pattern to form/update concepts
    void processPattern(const std::vector<float>& pattern, float activationStrength = 1.0f);
    
    // Learn pattern as new concept
    void learnPattern(const std::vector<float>& pattern, const std::string& context = "");
    
    // Retrieve related concepts based on similarity
    std::vector<std::pair<std::string, float>> retrieveConcepts(const std::vector<float>& pattern, size_t maxResults = 5) const;
    
    // Get concept activation by name
    float getConceptActivation(const std::string& conceptName) const;
    
    // Get all concepts above activation threshold
    std::vector<std::string> getActiveConcepts(float threshold = 0.1f) const;
    
    // Get concept representation by name
    std::vector<float> getConceptRepresentation(const std::string& conceptName) const;
    
    // Update concept from episodic memory
    void updateFromEpisodicMemory(const EpisodicMemoryItem& episode);
    
    // Get concept relationships (association strengths)
    float getConceptRelationship(const std::string& conceptA, const std::string& conceptB) const;
    
    // Set concept relationship
    void setConceptRelationship(const std::string& conceptA, const std::string& conceptB, float strength);
    
    // Get concept usage statistics
    struct ConceptStats {
        std::string name;
        size_t activationCount;
        float averageActivation;
        float lastActivation;
        float totalAssociations;
        float knowledgeLevel;
    };
    
    std::vector<ConceptStats> getConceptStats() const;
    
    // Clear memory and reset
    void clear();
    
    // Get capacity limits
    size_t getMaxConcepts() const;
    size_t getCurrentConceptCount() const;
    
    // Self-model integration
    std::string getSelfModel() const;
    void updateSelfModel(const std::vector<float>& currentState);

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm