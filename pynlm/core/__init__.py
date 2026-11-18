'''Core types and utilities module for pynlm'''

from .types import (
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId,
    SimulationStep,
    Timestamp,
    TimestepDuration,
    NeuronIndex,
    SynapseIndex,
    PopulationIndex,
    RegionIndex,
    SynapticWeight,
    MembranePotential,
    FiringRate,
    Delay,
    SpikeEvent,
    DelayedSpikeEvent,
    INVALID_NEURON_ID,
    INVALID_SYNAPSE_ID,
    INVALID_REGION_ID,
    INVALID_POPULATION_ID,
)

from .enums import (
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    ActionType,
    MotorCommand,
    WorldObjectType,
    ConfigSource,
)

__all__ = [
    # Type definitions
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'SimulationStep', 'Timestamp', 'TimestepDuration',
    'NeuronIndex', 'SynapseIndex', 'PopulationIndex', 'RegionIndex',
    'SynapticWeight', 'MembranePotential', 'FiringRate', 'Delay',
    'SpikeEvent', 'DelayedSpikeEvent',
    # Enumerations
    'NeuronType', 'SynapseType', 'DevelopmentalStage',
    'FiringState', 'ActionType', 'MotorCommand',
    'WorldObjectType', 'ConfigSource',
    # Constants
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID', 'INVALID_POPULATION_ID',
]