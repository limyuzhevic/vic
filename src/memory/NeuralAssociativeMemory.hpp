#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Forward declaration
class Brain;

/**
 * Neural Associative Memory - Biologically-inspired associative memory system
 * 
 * Implements Hebbian-like associative learning based on neural activation patterns
 * and synaptic connectivity. Stores associations between neuron groups with strength
 * that decays over time and can be modified through learning.
 * 
 * Phase 6 Integration: Connected to brain's neural substrate for real associations
 */
class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();
    
    /**
     * Initialize the associative memory system
     * 
     * @param brain Pointer to the brain for neural connectivity
     * @param sparsity Fraction of neuron connections that can be formed (0.0-1.0)
     */
    void initialize(std::shared_ptr<Brain> brain, float sparsity = 0.1f);
    
    /**
     * Create or update an association between two neurons
     * 
     * @param a First neuron ID
     * @param b Second neuron ID  
     * @param strength Association strength (0.0-1.0)
     */
    void associate(NeuronId a, NeuronId b, float strength);
    
    /**
     * Get the current strength of association between two neurons
     * 
     * @param a First neuron ID
     * @param b Second neuron ID
     * @return Association strength (0.0-1.0, 0.0 if no association)
     */
    float getAssociationStrength(NeuronId a, NeuronId b) const;
    
    /**
     * Update association strength by delta (positive for strengthening, negative for weakening)
     * 
     * @param a First neuron ID
     * @param b Second neuron ID
     * @param delta Strength change to apply
     */
    void updateAssociation(NeuronId a, NeuronId b, float delta);
    
    /**
     * Get all neurons associated with a given neuron
     * 
     * @param neuron Neuron ID to find associations for
     * @return List of associated neuron IDs
     */
    std::vector<NeuronId> getAssociatedNeurons(NeuronId neuron) const;
    
    /**
     * Get activation-based association strength for a neuron
     * 
     * @param neuron Neuron ID
     * @param threshold Minimum association strength to consider
     * @return Average strength of associations above threshold
     */
    float getActivationBasedAssociation(NeuronId neuron, float threshold = 0.1f) const;
    
    /**
     * Decay all associations based on current time
     * 
     * @param rate Decay rate per time unit
     */
    void decayAll(float rate);
    
    /**
     * Clean up weak associations below threshold
     */
    void cleanup();
    
    /**
     * Advance internal clock for time-based decay
     * 
     * @param dt Time delta to advance
     */
    void advanceTime(float dt);
    
    /**
     * Get current timestamp
     */
    float getCurrentTime() const { return pImpl ? pImpl->currentTime : 0.0f; }
    
    /**
     * Get total number of associations stored
     */
    size_t getAssociationCount() const { return pImpl ? pImpl->associations.size() : 0; }
    
    /**
     * Clear all associations
     */
    void clear() { if (pImpl) pImpl->associations.clear(); }
    
    /**
     * Set brain pointer for neural integration (called during initialization)
     */
    void setBrain(std::shared_ptr<Brain> brain) { brain_ = brain; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::shared_ptr<Brain> brain_;
};

} // namespace nlm