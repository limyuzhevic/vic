# C++ types that are exposed through Python bindings
# ================================================

# These would normally be defined in the C++ headers and exposed via pybind11
# For the Python package structure, we define them here for type checking

from dataclasses import dataclass
from typing import Any

@dataclass
class NeuronId:
    """Unique identifier for a neuron."""
    value: int = 0
    
    def index(self) -> int:
        """Get the index of this neuron."""
        return self.value
    
    def __eq__(self, other: Any) -> bool:
        return isinstance(other, NeuronId) and self.value == other.value
    
    def __hash__(self) -> int:
        return hash(self.value)
    
    def __repr__(self) -> str:
        return f"<NeuronId: {self.value}>"

@dataclass
class SynapseId:
    """Unique identifier for a synapse."""
    value: int = 0
    
    def index(self) -> int:
        """Get the index of this synapse."""
        return self.value
    
    def __eq__(self, other: Any) -> bool:
        return isinstance(other, SynapseId) and self.value == other.value
    
    def __hash__(self) -> int:
        return hash(self.value)
    
    def __repr__(self) -> str:
        return f"<SynapseId: {self.value}>"

@dataclass
class RegionId:
    """Unique identifier for a brain region."""
    value: int = 0
    
    def index(self) -> int:
        """Get the index of this region."""
        return self.value
    
    def __eq__(self, other: Any) -> bool:
        return isinstance(other, RegionId) and self.value == other.value
    
    def __hash__(self) -> int:
        return hash(self.value)
    
    def __repr__(self) -> str:
        return f"<RegionId: {self.value}>"

@dataclass
class PopulationId:
    """Unique identifier for a neuron population."""
    value: int = 0
    
    def index(self) -> int:
        """Get the index of this population."""
        return self.value
    
    def __eq__(self, other: Any) -> bool:
        return isinstance(other, PopulationId) and self.value == other.value
    
    def __hash__(self) -> int:
        return hash(self.value)

# Invalid identifiers for error handling
INVALID_NEURON_ID = NeuronId()
INVALID_SYNAPSE_ID = SynapseId()
INVALID_REGION_ID = RegionId()
INVALID_POPULATION_ID = PopulationId()

__all__ = [
    'NeuronId',
    'SynapseId',
    'RegionId',
    'PopulationId',
    'INVALID_NEURON_ID',
    'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
]

# Also export from types.py for backward compatibility
__all__.extend([
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
])