# Constants module - Constant values
# ==============================

from .types import NeuronId, SynapseId, RegionId, PopulationId

# Invalid identifiers for error handling
INVALID_NEURON_ID = NeuronId()
INVALID_SYNAPSE_ID = SynapseId()
INVALID_REGION_ID = RegionId()
INVALID_POPULATION_ID = PopulationId()

__all__ = [
    'INVALID_NEURON_ID',
    'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
]