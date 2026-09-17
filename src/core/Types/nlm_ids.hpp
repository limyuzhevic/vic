#pragma once

#include "Types.hpp"

// Re-export ID types from include/nlm_ids.hpp
typing NeuronId = BasicId<NeuronIdTag, true>;
typing SynapseId = BasicId<SynapseIdTag, true>;
typing RegionId = BasicId<RegionIdTag, true>;
typing PopulationId = BasicId<PopulationIdTag, false>;

// Invalid ID sentinel (moved from original Types.hpp)
constexpr NeuronId INVALID_NEURON_ID = NeuronId(UINT64_MAX);
constexpr SynapseId INVALID_SYNAPSE_ID = SynapseId(UINT64_MAX);
constexpr RegionId INVALID_REGION_ID = RegionId(UINT64_MAX);
constexpr PopulationId INVALID_POPULATION_ID = PopulationId(UINT64_MAX);
