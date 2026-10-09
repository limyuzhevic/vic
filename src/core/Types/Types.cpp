// Core type definitions implementation

#include "Types.hpp"
#include <stdexcept>

namespace nlm {

// Implementation of NeuronId
NeuronId::NeuronId(uint64_t v) : value(v) {}

uint64_t NeuronId::index() const { return value; }

bool NeuronId::operator==(const NeuronId& other) const { return value == other.value; }
bool NeuronId::operator!=(const NeuronId& other) const { return value != other.value; }
bool NeuronId::operator<(const NeuronId& other) const { return value < other.value; }
bool NeuronId::operator>(const NeuronId& other) const { return value > other.value; }

// Implementation of SynapseId
SynapseId::SynapseId(uint64_t v) : value(v) {}

uint64_t SynapseId::index() const { return value; }

bool SynapseId::operator==(const SynapseId& other) const { return value == other.value; }
bool SynapseId::operator!=(const SynapseId& other) const { return value != other.value; }

// Implementation of RegionId
RegionId::RegionId(uint64_t v) : value(v) {}

uint64_t RegionId::index() const { return value; }

bool RegionId::operator==(const RegionId& other) const { return value == other.value; }
bool RegionId::operator!=(const RegionId& other) const { return value != other.value; }

// Implementation of PopulationId
PopulationId::PopulationId(uint64_t v) : value(v) {}

uint64_t PopulationId::index() const { return value; }

bool PopulationId::operator==(const PopulationId& other) const { return value == other.value; }
bool PopulationId::operator!=(const PopulationId& other) const { return value != other.value; }

// Implementation of InterRegionConnectionId
InterRegionConnectionId::InterRegionConnectionId(uint64_t v) : value(v) {}

uint64_t InterRegionConnectionId::index() const { return value; }

bool InterRegionConnectionId::operator==(const InterRegionConnectionId& other) const { return value == other.value; }
bool InterRegionConnectionId::operator!=(const InterRegionConnectionId& other) const { return value != other.value; }

// Implementation of SimulationStep
SimulationStep::SimulationStep(uint64_t v) : value(v) {}

uint64_t SimulationStep::index() const { return value; }

// Implementation of Timestamp
Timestamp::Timestamp(double v) : value(v) {}

double Timestamp::getMilliseconds() const { return value * 1000.0; }

// Implementation of Delay
Delay::Delay(uint32_t s) : steps(s) {}

} // namespace nlm
