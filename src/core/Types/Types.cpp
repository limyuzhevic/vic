#include "Types.hpp"

namespace nlm {

// Types.cpp - Implementation of type system utilities

// Helper function to get hash of neuron ID
size_t NeuronId::getHash() const {
    return static_cast<size_t>(value);
}

// Helper function to get hash of synapse ID  
size_t SynapseId::getHash() const {
    return static_cast<size_t>(value);
}

// Helper function to get hash of region ID
size_t RegionId::getHash() const {
    return static_cast<size_t>(value);
}

// Helper function to get hash of population ID
size_t PopulationId::getHash() const {
    return static_cast<size_t>(value);
}

// Function to create invalid IDs
NeuronId NeuronId::invalid() { return NeuronId(0); }
SynapseId SynapseId::invalid() { return SynapseId(0); }
RegionId RegionId::invalid() { return RegionId(0); }
PopulationId PopulationId::invalid() { return PopulationId(0); }

// Function to check if ID is valid
bool NeuronId::isValid() const { return value != 0; }
bool SynapseId::isValid() const { return value != 0; }
bool RegionId::isValid() const { return value != 0; }
bool PopulationId::isValid() const { return value != 0; }

// Index functions
NeuronIndex NeuronId::getIndex() const { return static_cast<NeuronIndex>(value); }
SynapseIndex SynapseId::getIndex() const { return static_cast<SynapseIndex>(value); }
RegionIndex RegionId::getIndex() const { return static_cast<RegionIndex>(value); }
PopulationIndex PopulationId::getIndex() const { return static_cast<PopulationIndex>(value); }

// Conversion to string
std::string NeuronId::toString() const {
    return "Neuron(" + std::to_string(value) + ")";
}
std::string SynapseId::toString() const {
    return "Synapse(" + std::to_string(value) + ")";
}
std::string RegionId::toString() const {
    return "Region(" + std::to_string(value) + ")";
}
std::string PopulationId::toString() const {
    return "Population(" + std::to_string(value) + ")";
}

} // namespace nlm
