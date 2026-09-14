#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>

namespace nlm {

// ConceptInstance: Represents a single instance of a concept
// For semantic memory, concepts are stored as stable knowledge
struct ConceptForSemanticMemory {
    // The neural activity pattern representing this concept
    std::vector<float> pattern;
    
    // Associated sensory features  
    std::vector<float> features;
    
    // When this concept was learned
    SimulationStep learnedAt;
    
    // How reliable this concept is (0-1)
    float reliability;
    
    // How many times this concept has been confirmed
    size_t confirmationCount;
    
    ConceptForSemanticMemory()
        : learnedAt(0), reliability(0.0f), confirmationCount(0) {}
};

// SemanticMemory: Stable knowledge about the world
// Encodes what the brain has learned to be true about its environment
//
// Key mechanisms:
// - Concepts form through repeated exposure and reinforcement
// - Concepts stabilize when they consistently predict outcomes
// - Concepts can be generalized to handle variations
// - Concepts can be retrieved given partial cues

class NeuralSemanticMemory {
public:
    NeuralSemanticMemory();
    ~NeuralSemanticMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Learn a new concept from experience
    // Returns true if concept was learned or updated, false if too novel
    bool learnConcept(const std::vector<float>& pattern,
                     const std::vector<float>& features,
                     float reward,
                     SimulationStep currentTime);

    // Retrieve the concept that best matches a pattern
    // Returns nullptr if no matching concept found
    const ConceptForSemanticMemory* retrieveConcept(
        const std::vector<float>& pattern) const;

    // Get all stored concepts
    const std::vector<ConceptForSemanticMemory>& getConcepts() const { return concepts_; }

    // Get concept by index
    const ConceptForSemanticMemory* getConcept(size_t index) const;

    // Get number of concepts
    size_t getConceptCount() const { return concepts_.size(); }

    // Update concept reliability based on confirmation
    void confirmConcept(size_t conceptIndex, float confidence);

    // Forget concept if unreinforced for too long
    void decayConcepts(SimulationStep currentTime, float decayRate);

    // Clear all concepts
    void clear();

private:
    // Find or create concept for pattern
    size_t findOrCreateConcept(const std::vector<float>& pattern,
                               const std::vector<float>& features,
                               float reward);

    // Compute similarity between pattern and concept
    float computeConceptSimilarity(const std::vector<float>& pattern,
                                   const ConceptForSemanticMemory& concept) const;

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::vector<ConceptForSemanticMemory> concepts_;
    SimulationStep lastCleanupStep_;
};

} // namespace nlm
