#include "WorkingMemory.hpp"

namespace nlm {

void WorkingMemory::store(NeuronId neuron, float value) {}
float WorkingMemory::retrieve(NeuronId neuron) const { return 0.0f; }
bool WorkingMemory::contains(NeuronId neuron) const { return false; }
void WorkingMemory::clear() {}
size_t WorkingMemory::getCapacity() const { return 0; }
size_t WorkingMemory::getCurrentSize() const { return 0; }
void WorkingMemory::decay(float decayRate) {}

} // namespace nlm
