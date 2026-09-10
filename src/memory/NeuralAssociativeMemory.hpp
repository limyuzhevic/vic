#pragma once

#include "../core/Types/Types.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace nlm {

// NeuralAssociativeMemory: Pattern-based memory system
// Phase 6: Implements associative memory for concept formation and pattern completion
// Stores associations between related neural patterns for recall and prediction

class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();
    
    // Initialize with brain reference for plasticity integration
    void initialize(class Brain* brain);
    
    // Store new pattern associations (Hebbian learning)
    void associatePattern(
        const std::vector<float>& pattern,
        const std::vector<float>& association,
        float strength = 1.0f,
        TimestepDuration dt = 1.0
    );
    
    // Retrieve associated pattern for given input
    std::vector<float> retrieveAssociation(
        const std::vector<float>& pattern,
        float threshold = 0.5f
    ) const;
    
    // Check if pattern has strong associations
    bool hasAssociation(
        const std::vector<float>& pattern,
        float minStrength = 0.1f
    ) const;
    
    // Get association strength for pattern
    float getAssociationStrength(const std::vector<float>& pattern) const;
    
    // Update all pattern weights based on recent activity
    void updateWeights(float learningRate = 0.01f, TimestepDuration dt = 1.0);
    
    // Concept formation from experience patterns
    void formConcept(
        const std::vector<float>& pattern,
        const std::string& conceptName,
        float conceptStrength = 0.8f
    );
    
    // Get concept from name
    std::vector<float> getConcept(const std::string& conceptName) const;
    
    // Get all concepts
    std::unordered_map<std::string, std::vector<float>> getAllConcepts() const;
    
    // Pattern completion using attractor dynamics
    std::vector<float> completePattern(
        const std::vector<float>& partialPattern,
        size_t iterations = 10
    );
    
    // Clear memory (reset for new episode)
    void clear();
    
    // Get memory statistics
    size_t getPatternCount() const;
    float getTotalAssociationStrength() const;
    
    // Enable/disable pattern completion
    void enablePatternCompletion(bool enable);
    bool isPatternCompletionEnabled() const;
    
    // Set memory capacity limits
    void setMaxPatterns(size_t maxPatterns);
    void setAssociationDecay(float decayRate);
    
    // Export/import memory (for checkpointing)
    bool exportMemory(const std::string& filepath) const;
    bool importMemory(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
};

} // namespace nlm
