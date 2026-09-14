#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

namespace nlm {

// NeuralSemanticMemory: Stores knowledge as distributed neural patterns
// NOT a symbolic knowledge base - knowledge is encoded as weight changes in neural populations
//
// Key mechanisms:
// - Semantic vectors: Words/conceptual entities as neural activity patterns
// - Relational knowledge: Distributional semantics through co-occurrence
// - Structured knowledge: Hierarchical organization in neural space
// - Concept emergence: Discovery of abstract patterns through neural dynamics

class NeuralSemanticMemory {
public:
    NeuralSemanticMemory();
    ~NeuralSemanticMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Store a semantic concept (word, object, idea)
    // Encodes the concept as a neural activity pattern
    void storeConcept(const std::string& key, const std::vector<float>& pattern);

    // Retrieve semantic vector for a concept
    std::vector<float> retrieveConcept(const std::string& key) const;

    // Check if concept exists
    bool hasConcept(const std::string& key) const;

    // Get all concept keys
    std::vector<std::string> getConceptKeys() const;

    // Compute semantic similarity between concepts
    float semanticSimilarity(const std::string& conceptA,
                           const std::string& conceptB) const;

    // Store relational knowledge between concepts
    void storeRelation(const std::string& subject,
                      const std::string& predicate,
                      const std::string& object,
                      float strength = 1.0f);

    // Retrieve objects for a subject-predicate relation
    std::vector<std::string> retrieveRelation(const std::string& subject,
                                            const std::string& predicate) const;

    // Get all relations for a subject
    std::vector<std::pair<std::string, std::string>> getRelationsForSubject(const std::string& subject) const;

    // Get all relations for an object
    std::vector<std::pair<std::string, std::string>> getRelationsForObject(const std::string& object) const;

    // Learn from neural activity (pattern completion during retrieval)
    void learnFromActivity(const std::vector<float>& pattern, float strength = 1.0f);

    // Propagate knowledge to related concepts
    void propagateKnowledge(const std::string& sourceConcept, float influenceStrength = 0.5f);

    // Extract conceptual hierarchy (find broader/narrower concepts)
    std::vector<std::string> extractConceptHierarchy(const std::string& concept) const;

    // Get concept activation levels (for attention)
    std::vector<std::pair<std::string, float>> getConceptActivations() const;

    // Update concept weights based on usage and outcomes
    void updateConceptWeights(float learningRate = 0.01f);

    // Clear all semantic knowledge
    void clear();

    // Get number of concepts stored
    size_t getConceptCount() const { return concepts_.size(); }

    // Get relation count
    size_t getRelationCount() const { return relations_.size(); }

    // Decay concept weights (forget less used concepts)
    void decayConcepts(float decayRate);

    // Freeze concept (no longer changes)
    void freezeConcept(const std::string& key);

    // Get concept strength/frequency
    float getConceptStrength(const std::string& key) const;

    // Set concept strength
    void setConceptStrength(const std::string& key, float strength);

    // Get storage capacity (approximate neural units available)
    size_t getCapacity() const { return capacity_; }

    // Set storage capacity
    void setCapacity(size_t cap) { capacity_ = cap; }

    // Is concept being actively used?
    bool isActive(const std::string& key) const;

    // Mark concept as used (increases activation)
    void markUsed(const std::string& key);

    // Get most similar concepts for a given concept
    std::vector<std::pair<std::string, float>> getSimilarConcepts(const std::string& concept,
                                                                  size_t maxResults = 5) const;

    // Get concept statistics (frequency, last used, etc.)
    struct ConceptStats {
        size_t frequency;
        SimulationStep lastUsed;
        float activation;
        float strength;
        SimulationStep creationTime;
    };

    ConceptStats getConceptStats(const std::string& key) const;

    // Decay concept based on age and frequency
    void decayConcept(const std::string& key, float timeFactor);

private:
    // Create neural representation for a concept
    NeuronId createConceptNeuron(const std::string& key, const std::vector<float>& pattern);

    // Find nearest existing concept for a pattern
    std::string findNearestConcept(const std::vector<float>& pattern, float threshold = 0.7f) const;

    // Compute semantic distance between two concept patterns
    float computeSemanticDistance(const std::vector<float>& patternA,
                                 const std::vector<float>& patternB) const;

    // Propagate activation from one concept to related concepts
    void propagateActivation(const std::string& sourceConcept, float strength = 1.0f);

    // Build concept hierarchy based on similarity
    void buildConceptHierarchy();

    // Get all relations involving a concept (subject or object)
    std::vector<std::pair<std::string, std::string>> getAllRelationsInvolving(const std::string& concept) const;

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    size_t capacity_;

    // Semantic concepts
    std::unordered_map<std::string, NeuronId> conceptNeurons_;
    std::unordered_map<std::string, std::vector<float>> conceptPatterns_;
    std::unordered_map<std::string, float> conceptStrength_;
    std::unordered_map<std::string, SimulationStep> conceptUsageCount_;
    std::unordered_map<std::string, SimulationStep> conceptLastUsed_;
    std::unordered_map<std::string, SimulationStep> conceptCreationTime_;
    std::unordered_map<std::string, bool> conceptFrozen_;
    std::unordered_map<std::string, float> conceptActivation_;

    // Semantic relations (subject, predicate, object)
    std::vector<std::tuple<std::string, std::string, std::string>> relations_;
    std::unordered_map<std::string, std::vector<std::string>> subjectToObjects_;
    std::unordered_map<std::string, std::vector<std::string>> objectToSubjects_;
    std::unordered_map<std::string, std::vector<std::string>> subjectToPredicates_;
    std::unordered_map<std::string, std::vector<std::string>> predicateToObjects_;

    // Hierarchy
    std::unordered_map<std::string, std::string> conceptParent_;
    std::unordered_map<std::string, std::vector<std::string>> conceptChildren_;
    bool hierarchyBuilt_;
};

} // namespace nlm
