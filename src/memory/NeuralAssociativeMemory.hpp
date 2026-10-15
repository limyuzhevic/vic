#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <string>

namespace nlm {

// Forward declarations
class Brain;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class Dopamine;
class Acetylcholine;

// NeuralAssociativeMemory: Implements pattern association and relational memory
// Provides bidirectional associative memory with capacity and competition management
// Integrates with working memory, episodic memory, and neuromodulatory systems
// Supports context-dependent associative learning through STDP and Hebbian plasticity

class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();
    
    // Initialize with brain reference for integration
    void initialize(Brain* brain);
    
    // Create association between two neural patterns
    // Both patterns should be from actual neural activity
    // Strength determines the association weight (0.0 to 1.0)
    void associate(const std::vector<float>& patternA,
                   const std::vector<float>& patternB,
                   float strength = 1.0f);
    
    // Create association based on experience
    // Extracts patterns from episodic memory and creates associations
    void associateFromExperience(const class EpisodicMemoryItem& episode);
    
    // Retrieve patterns associated with a query pattern
    // Returns patterns ordered by association strength/relevance
    std::vector<std::vector<float>> retrieve(const std::vector<float>& queryPattern,
                                             size_t maxResults = 5) const;
    
    // Get association strength between two patterns
    // Returns 0.0 if patterns are not associated
    float getAssociationStrength(const std::vector<float>& patternA,
                                 const std::vector<float>& patternB) const;
    
    // Update association based on outcome
    // Delta can be positive (strengthening) or negative (weakening)
    void updateAssociation(const std::vector<float>& patternA,
                           const std::vector<float>& patternB,
                           float delta);
    
    // Spread activation from cue pattern
    // Returns patterns that become activated through association chains
    // Steps parameter controls propagation depth
    std::vector<std::vector<float>> spreadActivation(const std::vector<float>& cuePattern,
                                                     size_t steps = 2) const;
    
    // Clear all associations
    void clear();
    
    // Get number of associations stored
    size_t getAssociationCount() const { return pImpl->associations_.size(); }
    
    // Get number of patterns stored
    size_t getPatternCount() const { return pImpl->patternNeurons_.size(); }
    
    // Get average association strength
    float getAverageAssociationStrength() const { return pImpl->averageAssociationStrength_; }
    
    // Get total associations created
    size_t getTotalAssociationsCreated() const { return pImpl->totalAssociationsCreated_; }
    
    // Get total retrievals performed
    size_t getTotalRetrievals() const { return pImpl->totalRetrievals_; }
    
private:
    // Helper function to find or create pattern neuron for a pattern
    NeuronId findPatternNeuron(const std::vector<float>& pattern);
    
    // Compute pattern similarity using weighted dot product
    float computeSimilarity(const std::vector<float>& a,
                           const std::vector<float>& b) const;
    
    // Find the most similar pattern to query
    size_t findMostSimilarPattern(const std::vector<float>& queryPattern) const;
    
    // Get all neurons associated with a given neuron
    std::vector<NeuronId> getAssociatedNeurons(NeuronId neuron) const;
    
    // Create or update association between two neurons
    void createAssociation(NeuronId neuronA, NeuronId neuronB, float strength);
    
    // Get current association strength between two neurons
    float getAssociationStrength(NeuronId neuronA, NeuronId neuronB) const;
    
    // Update existing association strength
    void updateAssociation(NeuronId neuronA, NeuronId neuronB, float delta);
    
    // Apply neuromodulatory effects to associations
    void applyNeuromodulatoryAssociation(NeuronId neuronA, NeuronId neuronB, float strength);
    
    // Integrate with working memory for temporal context
    void integrateWithWorkingMemory(const std::vector<float>& patternA,
                                    const std::vector<float>& patternB);
    
    // Apply plasticity rules to strengthen associations
    void applyPlasticityToAssociations(NeuronId neuronA, NeuronId neuronB, float strength);
    
    // Apply Hebbian plasticity update
    void applyHebbianUpdate(NeuronId neuronA, NeuronId neuronB, float delta);
    
    // Remove weakest pattern for capacity management
    void removeWeakestPattern();
    
    // Remove weakest association for capacity management
    void removeWeakestAssociation();
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Legacy members for compatibility (kept for reference)
    Brain* brain_;
    std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons_;
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations_;
};

} // namespace nlm
