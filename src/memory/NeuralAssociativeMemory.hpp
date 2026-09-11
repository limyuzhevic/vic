#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <tuple>
#include <algorithm>

namespace nlm {

// Forward declarations
class Brain;

// Neural associative memory: relationships between neural representations
// PLACEHOLDER - Phase 2 will implement real associative memory
class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();
    
    // Initialize with brain reference for neural integration
    // TODO PHASE 2: Implement real associative memory with neural mechanisms
    void initialize(Brain* brain);
    
    // Create association between neurons
    // TODO PHASE 2: Implement neural Hebbian learning for associations
    void createAssociation(NeuronId a, NeuronId b, float strength);
    
    // Get association strength
    float getAssociationStrength(NeuronId a, NeuronId b) const;
    
    // Update association
    void updateAssociation(NeuronId a, NeuronId b, float delta);
    
    // Apply Hebbian learning from active pattern
    void applyHebbianLearning(const std::vector<NeuronId>& activePattern);
    
    // Get neuron excitability (current activation state)
    float getNeuronExcitability(NeuronId neuron) const;
    
    // Update excitability from neural pattern
    void updateExcitabilityFromPattern(const std::vector<NeuronId>& pattern, float magnitude);
    
    // Decay association strengths
    void decay(float rate);
    
    // Clear all associations
    void clear();
    
    // Get all associations
    std::vector<std::tuple<NeuronId, NeuronId, float>> getAllAssociations() const;
    
    // Get neurons associated with given neuron
    std::vector<NeuronId> getAssociatedNeurons(NeuronId neuron) const;
    
    // Set maximum number of associations
    void setMaxAssociations(size_t max);
    
    // Set strength limits
    void setStrengthLimits(float min, float max);

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm