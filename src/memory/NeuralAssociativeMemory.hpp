#pragma once

/**
 * NeuralAssociativeMemory.hpp - Associative memory system for NLM
 * 
 * This system implements pattern completion and association learning:
 * - Hebbian-like associative learning
 * - Pattern completion and recall
 * - Spreading activation
 * - Distributed representation
 * 
 * Works with episodic memory to form associations between:
 * - Sensory patterns and rewards
 * - Actions and outcomes  
 * - Sequences of experiences
 */

#include "../core/Types/Types.hpp"
#include <vector>
#include <string>

namespace nlm {

class Brain;
class EpisodicMemoryItem;

/**
 * Associative memory system that forms connections between patterns
 * and enables pattern completion.
 * 
 * This is a key component for learning relationships between:
 * - What happens (sensory input)
 * - What it means (reward prediction)
 * - What to do (action selection)
 */
class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();
    
    /**
     * Initialize the associative memory system
     * 
     * @param brain Pointer to the brain this system is part of
     */
    void initialize(Brain* brain);
    
    /**
     * Form an association between two patterns
     * 
     * @param patternA First pattern (e.g., sensory input)
     * @param patternB Second pattern (e.g., reward or action)
     * @param strength Initial association strength (0.0 to 1.0)
     */
    void associate(const std::vector<float>& patternA,
                   const std::vector<float>& patternB,
                   float strength);
    
    /**
     * Create associations from an episodic memory item
     * 
     * Extracts patterns from the episode and forms associations
     * between sensory state, reward, and action.
     * 
     * @param episode Episodic memory item to process
     */
    void associateFromExperience(const EpisodicMemoryItem& episode);
    
    /**
     * Retrieve patterns similar to the input pattern
     * 
     * Enables pattern completion - given a partial pattern,
     * recall similar stored patterns.
     * 
     * @param pattern Input pattern to match
     * @param maxResults Maximum number of results to return
     * @return Vector of similar patterns (associated patterns)
     */
    std::vector<std::vector<float>> retrieve(
        const std::vector<float>& pattern,
        size_t maxResults = 5) const;
    
    /**
     * Get association strength between two patterns
     * 
     * @param patternA First pattern
     * @param patternB Second pattern
     * @return Association strength (0.0 to 1.0)
     */
    float getAssociationStrength(
        const std::vector<float>& patternA,
        const std::vector<float>& patternB) const;
    
    /**
     * Update an existing association
     * 
     * @param patternA First pattern
     * @param patternB Second pattern
     * @param newStrength New association strength
     */
    void updateAssociation(
        const std::vector<float>& patternA,
        const std::vector<float>& patternB,
        float newStrength);
    
    /**
     * Perform spreading activation from a seed pattern
     * 
     * Activates associated patterns based on similarity,
     * enabling recall of related memories.
     * 
     * @param pattern Seed pattern to activate from
     * @param activationThreshold Minimum similarity to activate
     * @return Vector of activated patterns
     */
    std::vector<std::vector<float>> spreadActivation(
        const std::vector<float>& pattern,
        float activationThreshold = 0.6f) const;
    
    /**
     * Clear all associations
     * 
     * Useful for memory consolidation or system reset
     */
    void clear();
    
    /**
     * Get current associations
     * 
     * @return Vector of all stored associations (for debugging/monitoring)
     */
    const std::vector<Association>& getAssociations() const {
        return pImpl->associations;
    }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods
    void createPatternNode(const std::vector<float>& pattern);
    void linkPatterns(const std::vector<float>& patternA,
                      const std::vector<float>& patternB,
                      float strength);
    NeuronId findPatternNeuron(const std::vector<float>& pattern);
    float computeSimilarity(const std::vector<float>& a,
                           const std::vector<float>& b) const;
    std::vector<float> normalize(const std::vector<float>& vec) const;
    std::vector<float> extractSensoryPattern(const EpisodicMemoryItem& episode) const;
    std::vector<float> extractActionPattern(const EpisodicMemoryItem& episode) const;
    Timestamp getCurrentTime() const;
    
    // Forward declaration
    struct Association {
        std::vector<float> patternA;
        std::vector<float> patternB;
        float strength;
        Timestamp timestamp;
        
        Association(const std::vector<float>& a, const std::vector<float>& b, float s)
            : patternA(a), patternB(b), strength(s), timestamp(0.0f) {}
    };
};

} // namespace nlm