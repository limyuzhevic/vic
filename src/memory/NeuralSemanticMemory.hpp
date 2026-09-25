#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

namespace nlm {

// NeuralSemanticMemory: Stores knowledge and facts using neural patterns
// NOT a simple key-value store - uses distributed neural encoding
//
// Key mechanisms:
// - Distributed pattern encoding of concepts
// - Pattern completion for retrieval
// - Associative links between related concepts
// - Hierarchical organization (atomic to abstract concepts)
// - Neural plasticity for knowledge consolidation

class NeuralSemanticMemory {
public:
    NeuralSemanticMemory();
    ~NeuralSemanticMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Store a concept using neural pattern
    // The concept is encoded as distributed neural activity
    void storeConcept(const std::string& name, const std::vector<float>& pattern,
                     const std::vector<std::string>& relatedConcepts = {});

    // Retrieve concept by name or partial pattern
    std::vector<float> retrieveConcept(const std::string& name) const;

    // Retrieve concepts similar to a pattern
    std::vector<std::pair<std::string, float>> retrieveSimilar(
        const std::vector<float>& pattern, size_t maxResults = 5) const;

    // Check if concept exists
    bool hasConcept(const std::string& name) const;

    // Get all concepts
    std::vector<std::string> getAllConcepts() const;

    // Get related concepts for a given concept
    std::vector<std::string> getRelatedConcepts(const std::string& name) const;

    // Add association between two concepts
    void associateConcepts(const std::string& conceptA, const std::string& conceptB,
                         float strength = 1.0f);

    // Get association strength between concepts
    float getConceptAssociation(const std::string& conceptA,
                               const std::string& conceptB) const;

    // Update concept prototype based on experience
    void updateConcept(const std::string& name, const std::vector<float>& newPattern,
                      float learningRate = 0.1f);

    // Get concept activation level
    float getConceptActivation(const std::string& name) const;

    // Get average activation of all concepts
    float getAverageActivation() const;

    // Clear all concepts
    void clear();

    // Get number of concepts
    size_t getConceptCount() const { return conceptPrototypes_.size(); }

    // Update concept based on prediction error (TD learning)
    void updateConceptTD(const std::string& name, const std::vector<float>& reward,
                        float delta, float learningRate = 0.1f);

    // Get concept hierarchy level
    size_t getConceptLevel(const std::string& name) const;

private:
    // Find or create concept neuron pattern
    std::vector<float> getConceptPattern(const std::string& name);

    // Compute concept similarity using pattern matching
    float computeConceptSimilarity(const std::vector<float>& pattern1,
                                  const std::vector<float>& pattern2) const;

    // Establish conceptual hierarchy
    void establishConceptHierarchy(const std::string& conceptName);

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::unordered_map<std::string, std::vector<float>> conceptPrototypes_;
    std::unordered_map<std::string, size_t> conceptLevels_;
    std::unordered_map<std::string, std::vector<std::string>> conceptRelationships_;
    std::unordered_map<std::string, std::unordered_map<std::string, float>> conceptAssociations_;
    std::unordered_map<std::string, float> conceptActivations_;
    std::unordered_map<std::string, std::vector<float>> conceptRewards_;
    std::unordered_map<std::string, std::vector<float>> conceptPredictions_;
};

} // namespace nlm
