#ifndef NLM_CORE_TYPES_TYPES_HPP
#define NLM_CORE_TYPES_TYPES_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace nlm {

typedef uint64_t NeuronId;
typedef uint64_t SynapseId;
typedef uint64_t RegionId;
typedef uint64_t PopulationId;
typedef uint64_t SynapseHandle;

typedef float MembranePotential;
typedef float SynapticWeight;
typedef double Timestamp;
typedef uint64_t SimulationStep;
typedef float Delay;

enum class NeuronType {
    Internal,
    Sensory,
    Motor
};

enum class SynapseType {
    Excitatory,
    Inhibitory,
    Modulatory
};

enum class FiringState {
    Resting,
    Depolarizing,
    Firing,
    Refractory
};

enum class DevelopmentalStage {
    Initial,
    CriticalPeriod,
    Maturation,
    Adult
};

// Type aliases for convenience
using NeuronIdList = std::vector<NeuronId>;
using SynapseIdList = std::vector<SynapseId>;
using RegionIdList = std::vector<RegionId>;

// Constants
const NeuronId INVALID_NEURON_ID = 0;
const SynapseId INVALID_SYNAPSE_ID = 0;
const RegionId INVALID_REGION_ID = 0;
const PopulationId INVALID_POPULATION_ID = 0;

} // namespace nlm

#endif // NLM_CORE_TYPES_TYPES_HPP
