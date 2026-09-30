// NeuralAssociativeMemory.hpp
#pragma once

#include "../core/Types/Types.hpp"
#include <vector>

namespace nlm {

class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();
    
    // Create association between neurons
    void associate(NeuronId a, NeuronId b, float strength);
    
    // Get associated neurons
    std::vector<NeuronId> getAssociations(NeuronId neuron) const;
    
    // Get association strength
    float getAssociationStrength(NeuronId a, NeuronId b) const;
    
    // Update association strength
    void updateAssociation(NeuronId a, NeuronId b, float delta);
    
    // Clear all associations
    void clear();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm