#include "AssociativeMemory.hpp"

namespace nlm {

void AssociativeMemory::associate(NeuronId a, NeuronId b, float strength) {}
std::vector<NeuronId> AssociativeMemory::getAssociations(NeuronId neuron) const { return std::vector<NeuronId>(); }
float AssociativeMemory::getAssociationStrength(NeuronId a, NeuronId b) const { return 0.0f; }
void AssociativeMemory::updateAssociation(NeuronId a, NeuronId b, float delta) {}
void AssociativeMemory::clear() {}

} // namespace nlm
