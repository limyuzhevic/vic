#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

class Brain;

class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();
    
    void initialize(Brain* brain);
    void associate(NeuronId a, NeuronId b, float strength);
    std::vector<NeuronId> getAssociations(NeuronId neuron) const;
    float getAssociationStrength(NeuronId a, NeuronId b) const;
    void updateAssociation(NeuronId a, NeuronId b, float delta);
    void clear();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
