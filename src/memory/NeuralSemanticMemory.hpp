#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>

namespace nlm {

// SemanticMemory: Stores conceptual knowledge and facts
// NOT a database - semantic representations are encoded in neural patterns
//
// Key mechanisms:
// - Concepts stored as neural activity patterns
// - Hierarchical concept organization
// - Concept emergence from experience patterns
// - Cross-modal concept association

class NeuralSemanticMemory {
public:
    NeuralSemanticMemory();
    ~NeuralSemanticMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Store a concept with associated neural pattern
    // Concepts emerge from experience through neural activity patterns
    void storeConcept(const std::string& name,
                     const std::vector<float>& pattern,
                     const std::string& description = "");

    // Retrieve concept by name
    const std::vector<float>* getConcept(const std::string& name) const;

    // Find concepts similar to a pattern
    std::vector<std::string> findSimilarConcepts(const std::vector<float>& pattern,
                                               size_t maxResults = 5) const;

    // Get concept hierarchy level
    int getConceptLevel(const std::string& name) const;

    // Get all concept names
    std::vector<std::string> getAllConcepts() const;

    // Clear all concepts
    void clear();

    // Update concept from experience
    void updateFromExperience(const std::vector<float>& sensoryPattern,
                            const std::vector<float>& resultingPattern);

    // Get concept activation
    float getConceptActivation(const std::string& name) const;

private:
    // Find or create pattern neuron for a concept
    NeuronId findOrCreatePatternNeuron(const std::vector<float>& pattern);

    // Compute semantic similarity between patterns
    float computeSemanticSimilarity(const std::vector<float>& a,
                                  const std::vector<float>& b) const;

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;

    // Concept storage
    std::unordered_map<std::string, std::vector<float>> concepts_;
    std::unordered_map<std::string, NeuronId> conceptNeurons_;
    std::unordered_map<std::string, int> conceptLevels_;
    std::unordered_map<std::string, float> conceptActivations_;
    std::unordered_map<std::string, std::string> conceptDescriptions_;
    std::unordered_map<std::string, std::vector<std::string>> conceptSubconcepts_;
};
