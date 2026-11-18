'''Core types and utilities for pynlm'''

from __future__ import annotations

from typing import ClassVar

# Type aliases for compatibility with C++ types
SimulationStep = int
Timestamp = float
TimestepDuration = float
NeuronIndex = int
SynapseIndex = int
PopulationIndex = int
RegionIndex = int
SynapticWeight = float
MembranePotential = float
FiringRate = float
Delay = int


class NeuronId:
    """Unique identifier for a neuron.
    
    This class provides a strongly-typed neuron identifier with comparison
    operators and hash support for use in collections.
    """
    
    def __init__(self, value: int = 0):
        """Initialize a NeuronId.
        
        Args:
            value: The numeric value of the neuron ID. Defaults to 0.
        """
        self.value: int = value
    
    def index(self) -> int:
        """Get the numeric index of this neuron ID.
        
        Returns:
            The numeric value of this neuron ID.
        """
        return self.value
    
    def __eq__(self, other: object) -> bool:
        """Check equality with another NeuronId.
        
        Args:
            other: The other object to compare.
            
        Returns:
            True if the other object is a NeuronId with the same value.
        """
        if isinstance(other, NeuronId):
            return self.value == other.value
        return False
    
    def __ne__(self, other: object) -> bool:
        """Check inequality with another NeuronId.
        
        Args:
            other: The other object to compare.
            
        Returns:
            True if the other object is not equal to this NeuronId.
        """
        return not self.__eq__(other)
    
    def __lt__(self, other: NeuronId) -> bool:
        """Check if this NeuronId is less than another.
        
        Args:
            other: The other NeuronId to compare.
            
        Returns:
            True if this NeuronId's value is less than the other's.
        """
        return self.value < other.value
    
    def __hash__(self) -> int:
        """Get hash of this NeuronId.
        
        Returns:
            Hash of the neuron ID value.
        """
        return hash(self.value)
    
    def __repr__(self) -> str:
        """Get string representation of this NeuronId.
        
        Returns:
            String representation of the neuron ID.
        """
        return f"<NeuronId: {self.value}>"
    
    def __str__(self) -> str:
        """Get string representation of this NeuronId.
        
        Returns:
            String representation of the neuron ID.
        """
        return f"NeuronId({self.value})"


class SynapseId:
    """Unique identifier for a synapse.
    
    This class provides a strongly-typed synapse identifier with comparison
    operators and hash support for use in collections.
    """
    
    def __init__(self, value: int = 0):
        """Initialize a SynapseId.
        
        Args:
            value: The numeric value of the synapse ID. Defaults to 0.
        """
        self.value: int = value
    
    def index(self) -> int:
        """Get the numeric index of this synapse ID.
        
        Returns:
            The numeric value of this synapse ID.
        """
        return self.value
    
    def __eq__(self, other: object) -> bool:
        """Check equality with another SynapseId.
        
        Args:
            other: The other object to compare.
            
        Returns:
            True if the other object is a SynapseId with the same value.
        """
        if isinstance(other, SynapseId):
            return self.value == other.value
        return False
    
    def __ne__(self, other: object) -> bool:
        """Check inequality with another SynapseId.
        
        Args:
            other: The other object to compare.
            
        Returns:
            True if the other object is not equal to this SynapseId.
        """
        return not self.__eq__(other)
    
    def __lt__(self, other: SynapseId) -> bool:
        """Check if this SynapseId is less than another.
        
        Args:
            other: The other SynapseId to compare.
            
        Returns:
            True if this SynapseId's value is less than the other's.
        """
        return self.value < other.value
    
    def __hash__(self) -> int:
        """Get hash of this SynapseId.
        
        Returns:
            Hash of the synapse ID value.
        """
        return hash(self.value)


class RegionId:
    """Unique identifier for a brain region.
    
    This class provides a strongly-typed region identifier with comparison
    operators and hash support for use in collections.
    """
    
    def __init__(self, value: int = 0):
        """Initialize a RegionId.
        
        Args:
            value: The numeric value of the region ID. Defaults to 0.
        """
        self.value: int = value
    
    def index(self) -> int:
        """Get the numeric index of this region ID.
        
        Returns:
            The numeric value of this region ID.
        """
        return self.value
    
    def __eq__(self, other: object) -> bool:
        """Check equality with another RegionId.
        
        Args:
            other: The other object to compare.
            
        Returns:
            True if the other object is a RegionId with the same value.
        """
        if isinstance(other, RegionId):
            return self.value == other.value
        return False
    
    def __ne__(self, other: object) -> bool:
        """Check inequality with another RegionId.
        
        Args:
            other: The other object to compare.
            
        Returns:
            True if the other object is not equal to this RegionId.
        """
        return not self.__eq__(other)
    
    def __lt__(self, other: RegionId) -> bool:
        """Check if this RegionId is less than another.
        
        Args:
            other: The other RegionId to compare.
            
        Returns:
            True if this RegionId's value is less than the other's.
        """
        return self.value < other.value
    
    def __hash__(self) -> int:
        """Get hash of this RegionId.
        
        Returns:
            Hash of the region ID value.
        """
        return hash(self.value)


class PopulationId:
    """Unique identifier for a neuron population.
    
    This class provides a strongly-typed population identifier with comparison
    operators and hash support for use in collections.
    """
    
    def __init__(self, value: int = 0):
        """Initialize a PopulationId.
        
        Args:
            value: The numeric value of the population ID. Defaults to 0.
        """
        self.value: int = value
    
    def index(self) -> int:
        """Get the numeric index of this population ID.
        
        Returns:
            The numeric value of this population ID.
        """
        return self.value
    
    def __eq__(self, other: object) -> bool:
        """Check equality with another PopulationId.
        
        Args:
            other: The other object to compare.
            
        Returns:
            True if the other object is a PopulationId with the same value.
        """
        if isinstance(other, PopulationId):
            return self.value == other.value
        return False
    
    def __ne__(self, other: object) -> bool:
        """Check inequality with another PopulationId.
        
        Args:
            other: The other object to compare.
            
        Returns:
            True if the other object is not equal to this PopulationId.
        """
        return not self.__eq__(other)
    
    def __lt__(self, other: PopulationId) -> bool:
        """Check if this PopulationId is less than another.
        
        Args:
            other: The other PopulationId to compare.
            
        Returns:
            True if this PopulationId's value is less than the other's.
        """
        return self.value < other.value
    
    def __hash__(self) -> int:
        """Get hash of this PopulationId.
        
        Returns:
            Hash of the population ID value.
        """
        return hash(self.value)


class SpikeEvent:
    """Event representing a spike from a neuron.
    
    This class represents an action potential event that propagates
    through the neural network.
    """
    
    def __init__(
        self,
        source_neuron: NeuronId = NeuronId(),
        timestamp: float = 0.0,
        step: int = 0
    ):
        """Initialize a SpikeEvent.
        
        Args:
            source_neuron: The neuron that generated the spike.
            timestamp: The simulation time when the spike occurred.
            step: The simulation step when the spike occurred.
        """
        self.source_neuron: NeuronId = source_neuron
        self.timestamp: float = timestamp
        self.step: int = step


class DelayedSpikeEvent:
    """Spike event with transmission delay.
    
    This class represents a spike that will be delivered to a destination
    neuron after a specified delay.
    """
    
    def __init__(
        self,
        source_neuron: NeuronId = NeuronId(),
        destination_neuron: NeuronId = NeuronId(),
        synapse_id: SynapseId = SynapseId(),
        weight: float = 0.0,
        synapse_type: 'SynapseType' = 'SynapseType.Excitatory',
        timestamp: float = 0.0,
        delivery_time: float = 0.0,
        step: int = 0,
        delivery_step: int = 0,
        is_excitatory: bool = True
    ):
        """Initialize a DelayedSpikeEvent.
        
        Args:
            source_neuron: The neuron that generated the spike.
            destination_neuron: The target neuron that will receive the spike.
            synapse_id: The synapse through which the spike travels.
            weight: The synaptic weight at the time of spike.
            synapse_type: The type of synapse.
            timestamp: When the spike occurs.
            delivery_time: When the spike should be delivered.
            step: Current simulation step.
            delivery_step: Step when the spike should be delivered.
            is_excitatory: Whether the spike is excitatory.
        """
        self.source_neuron: NeuronId = source_neuron
        self.destination_neuron: NeuronId = destination_neuron
        self.synapse_id: SynapseId = synapse_id
        self.weight: float = weight
        self.synapse_type: SynapseType = synapse_type
        self.timestamp: float = timestamp
        self.delivery_time: float = delivery_time
        self.step: int = step
        self.delivery_step: int = delivery_step
        self.is_excitatory: bool = is_excitatory


# Invalid ID constants (maintained for backward compatibility)
INVALID_NEURON_ID = NeuronId()
INVALID_SYNAPSE_ID = SynapseId()
INVALID_REGION_ID = RegionId()
INVALID_POPULATION_ID = PopulationId()