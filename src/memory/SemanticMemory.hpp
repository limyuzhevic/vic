#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

// SemanticMemory: Integrates conceptual knowledge with neural patterns
// NOT just text storage - concepts are encoded as distributed neural patterns
//
// Key mechanisms:
// - Concepts stored as activation patterns across neural populations
// - Hierarchical organization (basic concepts -> abstract concepts)
// - Pattern completion for concept retrieval
// - Concept formation from sensory input
// - Relational links between concepts

class SemanticMemory {
public:
    SemanticMemory();
    ~SemanticMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Store conceptual knowledge
    // Concept is a distributed neural activation pattern
    void storeConcept(const std::string& name,
                     const std::vector<float>& activationPattern,
                     const std::string& description = "");

    // Retrieve concept by name
    std::vector<float> retrieveConcept(const std::string& name) const;

    // Check if concept exists
    bool hasConcept(const std::string& name) const;

    // Get concept description
    std::string getConceptDescription(const std::string& name) const;

    // Get all concept names
    std::vector<std::string> getConceptNames() const;

    // Clear all concepts
    void clear();

    // Find similar concepts (pattern completion)
    std::vector<std::string> findSimilarConcepts(const std::vector<float>& pattern,
                                                size_t maxResults = 5) const;

    // Create relationship between concepts
    void linkConcepts(const std::string& conceptA, const std::string& conceptB,
                      const std::string& relationshipType, float strength = 1.0f);

    // Get related concepts
    std::vector<std::string> getRelatedConcepts(const std::string& conceptName,
                                                const std::string& relationshipType = "") const;

    // Update concept based on feedback
    void updateConcept(const std::string& name,
                      const std::vector<float>& newPattern,
                      float learningRate = 0.1f);

    // Get concept activation level
    float getConceptActivation(const std::string& name) const;

    // Encode new concept from sensory input
    void encodeFromSensoryInput(const std::vector<float>& sensoryPattern,
                               const std::string& suggestedName = "");

    // Get concept hierarchy level
    size_t getConceptLevel(const std::string& name) const { return 0; } // Placeholder

    // Generate explanation of concept
    std::string explainConcept(const std::string& name) const;

private:
    // Concept structure
    struct Concept {
        std::string name;
        std::vector<float> activationPattern;
        std::string description;
        float activationLevel;
        size_t creationStep;
        std::vector<std::string> relatedConcepts;
        std::unordered_map<std::string, std::pair<std::string, float>> relationships;
        
        Concept() : activationLevel(0.0f), creationStep(0) {}
    };

    // Pattern similarity computation
    float computePatternSimilarity(const std::vector<float>& patternA,
                                  const std::vector<float>& patternB) const;

    // Find nearest concepts in activation space
    std::vector<std::pair<std::string, float>> findNearestConcepts(const std::vector<float>& pattern,
                                                                   size_t maxResults = 10) const;

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::unordered_map<std::string, Concept> concepts_;
    std::vector<std::vector<float>> conceptPatterns_;
    size_t nextConceptId_;
};

} // namespace nlm
